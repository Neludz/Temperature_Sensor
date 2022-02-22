


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

#include "user_mem.h"

LOG_MODULE_REGISTER(modbus_hard, LOG_LEVEL_INF);


//=========================================================================================
//  task   
   K_FIFO_DEFINE(mh_fifo);
   K_THREAD_DEFINE(mh_task_Modbus_id, STACKSIZE, mh_task_Modbus, NULL, NULL, NULL,
		 -1 , 0, 1500); 
    

//=========================================================================================
extern uint16_t MBbuf_main [];
extern const t_default_state default_state[];

const struct device *dev_usb_acm;

mb_struct MB_USB;
uint8_t USB_MB_Buf[MB_FRAME_MAX];

//=========================================================================================



void mh_usb_init (void)
{
   // int ret;
  	dev_usb_acm = device_get_binding("CDC_ACM_0");
	if (!dev_usb_acm) {
		LOG_DBG("CDC_ACM_0 device not found");
		return;
	} 
    MB_USB.p_write = MBbuf_main;
    MB_USB.p_read = MBbuf_main;
    MB_USB.reg_read_last=NUM_BUF-1;
    MB_USB.reg_write_last=NUM_BUF-1;
    MB_USB.eep_state=EEP_FREE;
    MB_USB.er_frame_bad=EV_NOEVENT;
    MB_USB.slave_address=MB_ANY_ADDRESS;	//0==any address
    MB_USB.mb_state=STATE_IDLE;
    MB_USB.p_mb_buff=&USB_MB_Buf[0];
    MB_USB.f_save = mh_Write_Eeprom;
    MB_USB.f_start_trans = mh_USB_Transmit_Start;

}


void mh_Write_Eeprom (void *mbb)
{
uint32_t err;
mb_struct *st_mb;
st_mb = (void*) mbb;
	for (int32_t i = 0; i < (st_mb->eep_indx); i++)
	{
		if(EESave_Check(i+(st_mb->eep_start_save))==REG_OK)
		{
        err = user_mem_update_reg16 (((st_mb->eep_start_save)+i),  (uint16_t*) &(st_mb->p_write[i+(st_mb->eep_start_save)]));
		}
	}
k_sleep(K_MSEC(1500));
st_mb->eep_state = EEP_FREE;
}
void mh_USB_Transmit_Start (void *mbb)
{
   // LOG_INF("===================mh_USB_Transmit_Start");
    uart_irq_tx_enable(dev_usb_acm);
}


void uart_usb_interrupt_handler (const struct device *dev, void*user_data)
{
uint8_t buf[64];
mb_struct *st_mb=&MB_USB;
int recv_len;
//LOG_INF("uart_usb_interrupt_handler===================");
while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {

		if (uart_irq_rx_ready(dev)) {
            recv_len = uart_fifo_read(dev, buf, sizeof(buf));  
            if(All_Idle_Check(&MB_USB)==REG_OK) {
            st_mb=&MB_USB;
            //LOG_INF("uart_fifo_read == %d", recv_len);
			    if (recv_len>=MB_FRAME_MIN) {
                MB_USB.mb_index=(recv_len);
                MB_USB.mb_state=STATE_PARS;
				memcpy (MB_USB.p_mb_buff, buf, recv_len);
                k_fifo_put(&mh_fifo, st_mb);	
			    }
            }    
		}

		if (uart_irq_tx_ready(dev)&&MB_USB.mb_state==STATE_SEND) {
			int wrote_len;
            //LOG_INF("uart_irq_tx_ready++++++----!!!!!!!!!!!!!!!");
			//if (!MB_USB.response_size) {
			//	uart_irq_tx_disable(dev);
           //     MB_USB.mb_state=STATE_IDLE;
			//} else {
				wrote_len = uart_fifo_fill(dev, MB_USB.p_mb_buff, MB_USB.response_size);
				//MB_USB.response_size=MB_USB.response_size-wrote_len;
                //LOG_INF("uart_fifo_fill==================");
                uart_irq_tx_disable(dev);
                MB_USB.mb_state=STATE_IDLE;
		//	}
		}
	}

}

void mh_task_Modbus (void)
{
    LOG_INF("start task mh_task_Modbus");
    mb_struct *st_mb;
    k_sleep(K_MSEC(2000));
    uart_irq_callback_set(dev_usb_acm, uart_usb_interrupt_handler);   
    LOG_INF("uart_usb_asm_callback_set");
    uart_irq_rx_enable(dev_usb_acm);
    LOG_INF("uart_usb_asm_irq_rx_enable");
    while (1) 
    {	
		//k_yield();
        st_mb = k_fifo_get(&mh_fifo, K_FOREVER);
        MBparsing((mb_struct*) st_mb);
        //LOG_INF("MBparsing!!!!!!!!!!!!!!!");
	}
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

//=========================================================================================