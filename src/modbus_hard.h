#ifndef MODBUS_HARD_H_INCLUDED
#define MODBUS_HARD_H_INCLUDED

#include "modbus.h"

#define MB_STACKSIZE                    2048
#define MB_PRIORITY                     -2

#define DELAY_AFTER_EEPROM_WRITE_MS     1500

typedef struct {					
	uint32_t        kernel_data;
    mb_struct       MB_data;
} mb_queue_struct;

void mh_Write_Eeprom (void *mbb);
void mh_USB_Transmit_Start (void *mbb);
void mh_task_Modbus (void *arg1, void *arg2, void *arg3);
void mh_usb_init (void);
void mh_Factory (void);
void mh_Buf_Init (void);

#endif /* MODBUS_HARD_H_INCLUDED */
