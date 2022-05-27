/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 * @brief Dimmer switch for HA profile implementation.
 */

#include <zephyr.h>
#include <device.h>
#include <logging/log.h>
#include <ram_pwrdn.h>
#include <sys/reboot.h>
#include <drivers/watchdog.h>

#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zigbee/zigbee_app_utils.h>
#include <zigbee/zigbee_error_handler.h>
#include <zb_nrf_platform.h>
#include "zb_mem_config_custom.h"

#include <drivers/uart.h>
#include <usb/usb_device.h>
#include "emfat.h"
#include "modbus_hard.h"

#include "main.h"
#include "io_adc.h"
#include "modbus_reg.h"
#include "modbus_x_macros.h"
#include "user_mem.h"
#include "measure_NTC.h"

#include <drivers/usb/usb_dc.h>

#define WDT_NODE DT_INST(0, nordic_nrf_watchdog)

#ifdef WDT_NODE
#define WDT_DEV_NAME DT_LABEL(WDT_NODE)
#endif
/* Do not erase NVRAM to save the network parameters after device reboot or
 * power-off. NOTE: If this option is set to ZB_TRUE then do full device erase
 * for all network devices before running other samples.
 */
#define ERASE_PERSISTENT_CONFIG    ZB_FALSE

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile light switch (End Device) source code.
#endif

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

//-----------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------
int wdt_channel_id;
const struct device *wdt;

extern emfat_t emfat;
extern emfat_entry_t entries[];
extern uint16_t ADC_Value_T[];
extern uint16_t MBbuf_main [];

static volatile uint16_t usb_state;

static zb_uint8_t  attr_zcl_version = ZB_ZCL_VERSION;
static zb_uint8_t  attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN;
static zb_uint16_t attr_identify_time;

static uint16_t zigbee_conf_state_save=0;

zb_zcl_temp_measurement_attrs_t attr_temp_measurement;

//-----------------------------------------------------------------------
// Declare zigbee context
//-----------------------------------------------------------------------

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &attr_zcl_version,
				 &attr_power_source);

/* Declare attribute list for Identify cluster. */
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &attr_identify_time);

ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(temperature_attr_list, 
				&attr_temp_measurement.measure_value,
				&attr_temp_measurement.min_measure_value,
				&attr_temp_measurement.max_measure_value,
				&attr_temp_measurement.tolerance); 

ZB_HA_DECLARE_TEMPERATURE_SENSOR_CLUSTER_LIST(temperature_sensor_clusters,
					basic_attr_list,
					identify_attr_list,
					temperature_attr_list);

ZB_HA_DECLARE_TEMPERATURE_SENSOR_EP(temperature_sensor_ep,
                               TEMPERATURE_SENSOR_ENDPOINT,
                               temperature_sensor_clusters);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(temperature_sensor_ctx, temperature_sensor_ep);

//-----------------------------------------------------------------------
// Define task
//-----------------------------------------------------------------------
 K_THREAD_DEFINE(com_task_id, MAIN_STACKSIZE, com_task, NULL, NULL, NULL, MAIN_PRIORITY , 0, 1000); 

//-----------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------
uint16_t crc16_x25(uint8_t* pData, int length)
{
    uint32_t i;
    uint16_t wCrc = 0xffff;
    while (length--) 
	{
        wCrc ^= *(uint8_t *)pData++ << 0;
        for (i=0; i < 8; i++)
            wCrc = wCrc & 0x0001 ? (wCrc >> 1) ^ 0x8408 : wCrc >> 1;
    }
    return wCrc ^ 0xffff;
}

void address_and_code_init(void)
{   
	zb_ieee_addr_t ieee_addr;
    char ieee_addr_string[20];
	int i;
	zb_uint8_t     ic[ZB_CCM_KEY_SIZE + 2]; // +2 for CRC16
	uint16_t crc;

 	if(!(MBbuf_main[Reg_Zigbee_Set_Mode]&ZI_SET_MODE_BIT_SET_FICR_ADDR))
	{
	 	for(i=0;  i<8;  i++)
        { 
			*((uint8_t*)ieee_addr+i)=(uint8_t)(MBbuf_main[Reg_Zigbee_Addr_0_N_Panel+i]&0xFF) ;	
        }
		zb_set_long_address(ieee_addr); 
		ieee_addr_to_str(ieee_addr_string, 20, ieee_addr);
    	LOG_INF("addr: %s", ieee_addr_string);		
	}
	zb_osif_get_ieee_eui64(ieee_addr);
	for(i=0;  i<8;  i++)
    {
    	MBbuf_main[Reg_Zigbee_FICR_Addr_0+i]=*((uint8_t*)(ieee_addr+i))&0xFF;
    } 
    if ((MBbuf_main[Reg_Zigbee_Set_Mode]&ZI_SET_MODE_BIT_SET_CODE))
    {       
        for(i=0;  i<8;  i++)
        {
            ic[i*2]=(MBbuf_main[Reg_Zigbee_Code_0+i]>>8)&0xFF;
            ic[(i*2)+1]=MBbuf_main[Reg_Zigbee_Code_0+i]&0xFF;
        }     
        crc = crc16_x25(ic, ZB_CCM_KEY_SIZE);
        ic[ZB_CCM_KEY_SIZE]=(crc)&0xFF;
        ic[ZB_CCM_KEY_SIZE+1]=(crc>>8)&0xFF;
        zb_secur_ic_set(ZB_IC_TYPE_128,ic);          
    }   
}

void func_connect(zb_uint8_t param)
{
	user_input_indicate();
}

void func_reset_local(zb_uint8_t param)
{
	zigbee_conf_state_save = 0;	
	zb_bdb_reset_via_local_action(0U);
	user_input_indicate();
}

void func_erase(zb_uint8_t param)
{
	zb_nvram_erase();
	sys_reboot(SYS_REBOOT_COLD);
}

void Recon_timer_cb(zb_uint8_t param)
{
	LOG_INF("Recon_timer_cb---------------------------------------");
  	if(!ZB_JOINED())
  	{
		if((zigbee_conf_state_save) && (usb_state!=USB_DC_CONFIGURED))
		{  	
      		LOG_INF("Restart Zigbee");
			func_erase(0);
		}
		else
		{
      		LOG_INF("Reconnect Zigbee");
			func_connect(0);
		}	
  	} 
}

void zi_reboot(void)
{
	zb_ret_t zb_err_code;
	LOG_INF("zi_reboot");

	zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(Recon_timer_cb, ZB_ALARM_ALL_CB); 	
	ZB_ERROR_CHECK(zb_err_code); 
	zb_err_code = ZB_SCHEDULE_APP_ALARM(Recon_timer_cb, 0, ZB_MILLISECONDS_TO_BEACON_INTERVAL(DELAY_TOTAL_ZI_RECONNECT_MS));
  	ZB_ERROR_CHECK(zb_err_code);
}

void zigbee_status_update(zb_bufid_t bufid)
{
	zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
	zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);
	zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);

	switch (sig) 
	{
	case ZB_BDB_SIGNAL_DEVICE_REBOOT:
		/* fall-through */
		zigbee_conf_state_save = 1;
		LOG_INF("!!!!!!!!!!!!!!!!!!!!!!!!!!!!ZB_BDB_SIGNAL_DEVICE_REBOOT");
	case ZB_BDB_SIGNAL_STEERING:
		LOG_INF("_!_!_!_!_!_!_!_!_!_!_!_!_!_!_ZB_BDB_SIGNAL_STEERING");
		if (status == RET_OK) 
		{
			MBbuf_main[Reg_Zigbee_Connect_Status]=1;	
		} 
		else 
		{
			MBbuf_main[Reg_Zigbee_Connect_Status]=0;
		}
		zi_reboot();
		break;
	case ZB_ZDO_SIGNAL_LEAVE:
		/* Update network status LED */
		LOG_INF("ZB_ZDO_SIGNAL_LEAVE");
		zigbee_conf_state_save=1;
		zi_reboot();
		MBbuf_main[Reg_Zigbee_Connect_Status]=0;
		break;
	default:
		break;
	}
}

void command_control(void)
{
    if(MBbuf_main[Reg_Send_Command])
    {
        switch (MBbuf_main[Reg_Send_Command])
        {
        case CODE_CONNECT_BUTTON:
              user_input_indicate();
              break;
        case CODE_RESET_ZIGBEE_DATA:         
              zb_bdb_reset_via_local_action(0U);
              break;
        case CODE_RESTART:
              sys_reboot(SYS_REBOOT_COLD);
              break; 
        case CODE_RESET_TO_DEFAULT:
              mh_Factory();
              break;
        case CODE_HARD_RESET_STACK:
              zb_nvram_erase();             
              sys_reboot(SYS_REBOOT_COLD);
              break;
        default:
              break;
        }
    	MBbuf_main[Reg_Send_Command]=0;
    }
}

/** Function to disable pin reset by writing an invalid configuration (all 0) to PSELRESET[1] */
/*static void disable_pin_reset_and_block(void)
//{
    // Only perform if needed (which is the first time this code runs)
    if (NRF_UICR->PSELRESET[1] != 0)
    {
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

    NRF_UICR->PSELRESET[1] = 0;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
    NVIC_SystemReset();
    }
	 if (NRF_UICR->APPROTECT == 0xFFFFFFFF)
    {

	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}  

    NRF_UICR->APPROTECT= 0xFFFFFF00;   
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}     
    NVIC_SystemReset();
    }
}
*/

//-----------------------------------------------------------------------
// Callback function
//-----------------------------------------------------------------------
/**@brief Zigbee stack event handler.
 *
 * @param[in]   bufid   Reference to the Zigbee stack buffer
 *                      used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
	zb_zdo_app_signal_hdr_t    *sig_hndler = NULL;
	zb_zdo_app_signal_type_t    sig = zb_get_app_signal(bufid, &sig_hndler);
	//zb_ret_t                    status = ZB_GET_APP_SIGNAL_STATUS(bufid);
	//zb_address_ieee_ref_t addr_ref;

	zigbee_status_update(bufid);

	switch (sig) 
	{
	case ZB_BDB_SIGNAL_DEVICE_REBOOT:
		/* fall-through */
	case ZB_BDB_SIGNAL_STEERING:
		/* Call default signal handler. */
		ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
		break;
	default:
		/* Call default signal handler. */
		ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
		break;
	}
	if (bufid) 
	{
		zb_buf_free(bufid);
	}
}

void usb_cb(enum usb_dc_status_code cb_status,
				       const uint8_t *param){
usb_state = cb_status;                          
}

//-----------------------------------------------------------------------
// Task function
//-----------------------------------------------------------------------
void com_task (void *arg1, void *arg2, void *arg3)
{
	int err;
	zb_zcl_status_t zcl_status;
	zb_int16_t value_temp;
	wdt = device_get_binding(WDT_DEV_NAME);
	if (!wdt) 
	{
		LOG_INF("Cannot get WDT device\n");
		return;
	}

	struct wdt_timeout_cfg wdt_config = 
	{
		/* Reset SoC when watchdog timer expires. */
		.flags = WDT_FLAG_RESET_SOC,

		/* Expire watchdog after max window */
		.window.min = 0U,
		.window.max = WDT_MAX_WINDOW,
	};

	wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
	if (wdt_channel_id == -ENOTSUP) 
	{
		/* IWDG driver for STM32 doesn't support callback */
		printk("Callback support rejected, continuing anyway\n");
		wdt_config.callback = NULL;
		wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
	}
	if (wdt_channel_id < 0) 
	{
		printk("Watchdog install error\n");
		return;
	}
	err = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
	if (err < 0) 
	{
		printk("Watchdog setup error\n");
		return;
	}
	wdt_feed(wdt, wdt_channel_id);
	while(1)
	{
		wdt_feed(wdt, wdt_channel_id);		
		k_sleep(K_MSEC(3000));
		/* Get new temperature measured value */	
		if ((ADC_Value_T[0] & TEMPERATURE_ERROR_MASK)!=TEMPERATURE_ERROR_LABEL)
		{
			value_temp = ADC_Value_T[0]*100;
		}
		else
		{
			value_temp = ADC_Value_T[0];
		}
    	zcl_status = zb_zcl_set_attr_val(TEMPERATURE_SENSOR_ENDPOINT, 
                                   	 	ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
                                    	ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                    	ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 
                                    	(zb_int8_t *)&value_temp, 
                                    	ZB_FALSE);     	
		command_control();
	}
}

//-----------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------
void main(void)
{

#ifdef ENABLE_APPROTECT
	if ((NRF_UICR->APPROTECT & UICR_APPROTECT_PALL_Msk) !=
		(UICR_APPROTECT_PALL_Enabled << UICR_APPROTECT_PALL_Pos)) {

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        NRF_UICR->APPROTECT = ((NRF_UICR->APPROTECT & ~((uint32_t)UICR_APPROTECT_PALL_Msk)) |
		    (UICR_APPROTECT_PALL_Enabled << UICR_APPROTECT_PALL_Pos));

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
   	}
#else
	if ((NRF_UICR->APPROTECT & UICR_APPROTECT_PALL_Msk) !=
		(UICR_APPROTECT_PALL_Disabled << UICR_APPROTECT_PALL_Pos)) {

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        NRF_UICR->APPROTECT = ((NRF_UICR->APPROTECT & ~((uint32_t)UICR_APPROTECT_PALL_Msk)) |
		    (UICR_APPROTECT_PALL_Disabled << UICR_APPROTECT_PALL_Pos));

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
	}
#endif
	int ret;
	LOG_INF("started");

	NRF_POWER->DCDCEN = 0x01;  

	emfat_init(&emfat, "Sensor", entries);
    user_mem_init();
    mh_Buf_Init();
	mh_usb_init();

	ret = usb_enable(usb_cb);
	if (ret != 0) 
	{
		LOG_ERR("Failed to enable USB");
		return;
	}

	io_adc_init();
	k_sleep(K_MSEC(2000));

	attr_temp_measurement.measure_value = TEMPERATURE_NO_MEASURE;

	zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
	zb_set_ed_timeout(MBbuf_main[Reg_Zigbee_Timeout]);
	zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(10000));

	address_and_code_init();

	zb_set_network_ed_role( (1UL << (MBbuf_main[Reg_Zigbee_Set_Channel]))) ;
	//if(usb_state!=USB_DC_CONFIGURED)
	//{
		zigbee_configure_sleepy_behavior(true);
		//if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY))
		//{
			//power_down_unused_ram();
		//}
	//}
	/* Register dimmer switch device context (endpoints). */
	ZB_AF_REGISTER_DEVICE_CTX(&temperature_sensor_ctx);

	if (usb_state!=USB_DC_CONFIGURED)
	{	
		//usb_disable();	
		k_busy_wait ( DELAY_FOR_POWER_TEST_MS*1000 ); 
		k_sleep(K_SECONDS(MBbuf_main[Reg_Zigbee_Wait_Time_S]));
		k_sleep(K_MSEC(DELAY_AFTER_POWER_TEST_MS+\
						  (TIME_BETWEEN_NEXT_CONNECTION_MS * MBbuf_main[Reg_Zigbee_Addr_1_N_Sens_Point])+\
						  (TIME_BETWEEN_NEXT_CONNECTION_MS * MAX_SENSOR_IN_ROUTER * MBbuf_main[Reg_Zigbee_Addr_0_N_Panel])));
	}
	zigbee_enable();

	while (1) 
	{
		k_sleep(K_FOREVER);
	}	
}