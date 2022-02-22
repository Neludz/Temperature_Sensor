#ifndef MODBUS_HARD_H_INCLUDED
#define MODBUS_HARD_H_INCLUDED


#define STACKSIZE 2048


void mh_Write_Eeprom (void *mbb);
void mh_USB_Transmit_Start (void *mbb);
void mh_task_Modbus (void);
void mh_usb_init (void);
void mh_Factory (void);
void mh_Buf_Init (void);

#endif /* MODBUS_HARD_H_INCLUDED */
