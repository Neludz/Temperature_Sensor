


#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <sys/ring_buffer.h>
#include <drivers/gpio.h>
#include <drivers/usb/usb_dc.h>

#include <usb/usb_device.h>
#include <logging/log.h>

#include "modbus_hard.h"
#include "modbus_reg.h"
#include "modbus.h"
#include "modbus_x_macros.h"

#include "user_mem.h"

LOG_MODULE_REGISTER(modbus_hard, LOG_LEVEL_INF);

//-----------------------------------------------------------------------
// Variable
//-----------------------------------------------------------------------

uint16_t MBbuf_main[NUM_BUF]={VERSION};
extern const t_default_state default_state[];
const struct device *dev_usb_acm;

mb_queue_struct MB_USB;
uint8_t USB_MB_Buf[MB_FRAME_MAX];

//-----------------------------------------------------------------------
// Define task & queue
//-----------------------------------------------------------------------   
static struct k_thread mh_MB_thread_data;
static k_tid_t mh_MB_tid;

struct k_queue mh_queue;
K_THREAD_STACK_DEFINE(mh_MB_stack_area, MB_STACKSIZE); 
    
//-----------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------
// Callback for usb com
void uart_usb_interrupt_handler (const struct device *dev, void*user_data)
{
	uint8_t buf[64];
	int recv_len;

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {

		if (uart_irq_rx_ready(dev)) 
		{
            recv_len = uart_fifo_read(dev, buf, sizeof(buf));  
            if(All_Idle_Check(&MB_USB.MB_data)==REG_OK) 
			{
			    if (recv_len>=MB_FRAME_MIN) 
				{
            		MB_USB.MB_data.mb_index=(recv_len);
            		MB_USB.MB_data.mb_state=STATE_PARS;
				    memcpy (MB_USB.MB_data.p_mb_buff, buf, recv_len);	
            		k_queue_append(&mh_queue, &MB_USB);
			    }
            }    
		}
		if (uart_irq_tx_ready(dev)&&(MB_USB.MB_data.mb_state==STATE_SEND)) 
    	{
			int wrote_len;
			wrote_len = uart_fifo_fill(dev, MB_USB.MB_data.p_mb_buff, MB_USB.MB_data.response_size);
      		uart_irq_tx_disable(dev);
      		MB_USB.MB_data.mb_state=STATE_IDLE;
		}
	}
}

void mh_usb_init (void)
{
	k_queue_init(&mh_queue);

  	dev_usb_acm = device_get_binding("CDC_ACM_0");
	if (!dev_usb_acm) 
	{
		LOG_DBG("CDC_ACM_0 device not found");
		return;
	} 
    MB_USB.MB_data.p_write = MBbuf_main;
    MB_USB.MB_data.p_read = MBbuf_main;
    MB_USB.MB_data.reg_read_last=NUM_BUF-1;
    MB_USB.MB_data.reg_write_last=NUM_BUF-1;
    MB_USB.MB_data.eep_state=EEP_FREE;
    MB_USB.MB_data.er_frame_bad=EV_NOEVENT;
    MB_USB.MB_data.slave_address=MB_ANY_ADDRESS;	//0==any address
    MB_USB.MB_data.mb_state=STATE_IDLE;
    MB_USB.MB_data.p_mb_buff=&USB_MB_Buf[0];
    MB_USB.MB_data.f_save = mh_Write_Eeprom;
    MB_USB.MB_data.f_start_trans = mh_USB_Transmit_Start;

	mh_MB_tid = k_thread_create(&mh_MB_thread_data,
				    mh_MB_stack_area,
				    K_THREAD_STACK_SIZEOF(mh_MB_stack_area),
				    mh_task_Modbus,
				    NULL, NULL, NULL,
				    MB_PRIORITY,
				    0, K_MSEC(1500));

}

void mh_Write_Eeprom (void *mbb)
{
	uint32_t err;
	mb_struct *st_mb = (void*) mbb;
	for (int32_t i = 0; i < (st_mb->eep_indx); i++)
	{
		if(EESave_Check(i+(st_mb->eep_start_save))==REG_OK)
		{
        	err = user_mem_update_reg16 (((st_mb->eep_start_save)+i),  (uint16_t*) &(st_mb->p_write[i+(st_mb->eep_start_save)]));
		}
	}
	k_sleep(K_MSEC(DELAY_AFTER_EEPROM_WRITE_MS));
	st_mb->eep_state = EEP_FREE;
}

void mh_USB_Transmit_Start (void *mbb)
{
    uart_irq_tx_enable(dev_usb_acm);
}

void mh_Factory (void)
{
	int err;
	for (int32_t i=0; i< NUM_BUF; i++)
	{
		if (EESave_Check(i)==REG_OK)
		{
			MBbuf_main[i] = default_state[i].Default_Value;
        	err = user_mem_update_reg16 (i,  &MBbuf_main[i]);
			//LOG_INF("mh_Factory, id= %d, err =%d \n", i, err);
		}
	}
}

void mh_Buf_Init (void)
{
	int err;

	for (int32_t i=0; i< NUM_BUF; i++)
	{
		if(EESave_Check(i)==REG_OK)
		{
        	err = user_mem_read_reg16 (i, &MBbuf_main[i]);
        	if(err==RET_MEM_NOT_FOUND)
			{
				MBbuf_main[i] = default_state[i].Default_Value;
       			err = user_mem_update_reg16 (i,  &MBbuf_main[i]);	
			}		
        	//LOG_INF("mh_Buf_Init, id= %d, err =%d \n", i, err);
			if(Limit_Check(i, MBbuf_main[i])==REG_ERR)
			{
				MBbuf_main[i]=default_state[i].Default_Value;
				err = user_mem_update_reg16 (i,  &MBbuf_main[i]);
            	//LOG_INF("Limit_Check, id= %d, err =%d \n", i, err);
			}
		}       
	}
}

//-----------------------------------------------------------------------
// Task function
//-----------------------------------------------------------------------
void mh_task_Modbus (void *arg1, void *arg2, void *arg3)
{
    LOG_INF("start task mh_task_Modbus");
    mb_queue_struct *st_mb;
	uart_irq_callback_set(dev_usb_acm, uart_usb_interrupt_handler);   
    LOG_INF("uart_usb_asm_callback_set");
    k_sleep(K_MSEC(2000));
    uart_irq_rx_enable(dev_usb_acm);
    LOG_INF("uart_usb_asm_irq_rx_enable");
    while (1) 
    {	
        st_mb = k_queue_get(&mh_queue, K_FOREVER);
        MBparsing((mb_struct*) &(st_mb->MB_data));
	}
}
