#ifndef MODBUS_X_H_INCLUDED
#define MODBUS_X_H_INCLUDED

#include "modbus_reg.h"
#include "main.h"
#include <zboss_api.h>

//-----------------------------------------------------------------------
// define
//-----------------------------------------------------------------------
#define LIMIT_REG	//check limit
#define EEPROM_REG	//use eeprom

#define REG_END_REGISTER    Reg_End

//-----------------------------------------------------------------------
// Modbus registers X macros
//-----------------------------------------------------------------------
/*
#define READ_R			(0)
#define WRITE_R			(0x01)	// 0 bit
#define EESAVE_R		(0x02)	// 1 bit
#define LIM_SIGN		(0x04)	// 2 bit for limit     		  <--|
#define LIM_UNSIGN		(0x08)  // 3 bit for limit	    	  <--|----------------
#define LIM_MASK	    (0x0C)	// 2 and 3 bit for limit	  <--|				  |
*///																			  |
//	     Number		Name for enum			Default	   Min	    Max   	__________Permission_______
//		    								 Value    Level    Level   |  R/W     EEPROM    LIMIT  |
//		    												  or Mask  | 						   |
#define MAIN_BUF_TABLE\
	X_BUF(0,	Reg_Start,						0,		0, 		0,		READ_R)\
	X_BUF(1,	Reg_T_0_Channel,				0,		0, 		0,	 	READ_R)\
    X_BUF(20,	Reg_Zigbee_Connect_Status,      0,  	0, 	    0,      READ_R)\
    X_BUF(29,	Reg_Send_Command,		        0,	    0, 	    0,      WRITE_R)\
    X_BUF(40,	Reg_Zigbee_Set_Mode,            ZI_SET_MODE_BIT_DEFAULT,\
                                                        0,  	0,      WRITE_R | EESAVE_R)\
    X_BUF(41,	Reg_Zigbee_Set_Channel,		    ZI_SET_CHANNEL_DEFAULT,\
                                                        11,	    26,     WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(42,	Reg_Zigbee_Wait_Time_S,         ZI_SET_WAIT_TIME_S_DEFAULT,\
                                                        0,	    0,      WRITE_R | EESAVE_R)\
    X_BUF(47,	Reg_Zigbee_Timeout,             ED_AGING_TIMEOUT_64MIN,\
                                                		ED_AGING_TIMEOUT_2MIN,\
                                                        		ED_AGING_TIMEOUT_16384MIN,\
                                                        		        WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(60,	Reg_Zigbee_FICR_Addr_0,			0,      0,		0,      READ_R)\
    X_BUF(61,	Reg_Zigbee_FICR_Addr_1,         0,      0,		0,      READ_R)\
    X_BUF(62,	Reg_Zigbee_FICR_Addr_2,         0,      0,		0,      READ_R)\
    X_BUF(63,	Reg_Zigbee_FICR_Addr_3,         0,      0,		0,      READ_R)\
    X_BUF(64,	Reg_Zigbee_FICR_Addr_4,         0,      0,		0,      READ_R)\
    X_BUF(65,	Reg_Zigbee_FICR_Addr_5,         0,      0,		0,      READ_R)\
    X_BUF(66,	Reg_Zigbee_FICR_Addr_6,         0,      0,		0,      READ_R)\
    X_BUF(67,	Reg_Zigbee_FICR_Addr_7,         0,      0,		0,      READ_R)\
    X_BUF(70,	Reg_Zigbee_Addr_0_N_Panel,      0,      0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(71,	Reg_Zigbee_Addr_1_N_Sens_Point, 1,      0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(72,	Reg_Zigbee_Addr_2,              0xF4,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(73,	Reg_Zigbee_Addr_3,              0xCE,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(74,	Reg_Zigbee_Addr_4,              0x47,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(75,	Reg_Zigbee_Addr_5,              0xCA,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(76,	Reg_Zigbee_Addr_6,              0x36,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(77,	Reg_Zigbee_Addr_7,              0x5B,   0,		0xFF,   WRITE_R | EESAVE_R)\
    X_BUF(80,	Reg_Zigbee_Code_0,              0x0102, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(81,	Reg_Zigbee_Code_1,              0x0304, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(82,	Reg_Zigbee_Code_2,              0x0506, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(83,	Reg_Zigbee_Code_3,              0x0708, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(84,	Reg_Zigbee_Code_4,              0x090A, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(85,	Reg_Zigbee_Code_5,              0x0B0C, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(86,	Reg_Zigbee_Code_6,              0x0D0E, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(87,	Reg_Zigbee_Code_7,              0x0F11, 0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(90,	Reg_NTC_R2_Value_W1,            50000,	0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(91,	Reg_NTC_R2_Value_W2,            0,		0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(92,	Reg_NTC_R_Divider_W1,           20000,	0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(93,	Reg_NTC_R_Divider_W2,           0,		0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(94,	Reg_NTC_B_Value,                3950,	0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(95,	Reg_NTC_T2_Value,               25,		0,		0xFFFF, WRITE_R | EESAVE_R)\
    X_BUF(96,	Reg_NTC_Start_Temperature,      -10,	-40,	20, 	WRITE_R | EESAVE_R | LIM_SIGN)\
    X_BUF(97,	Reg_NTC_Step_Temperature,       1,		1,		5, 		WRITE_R | EESAVE_R | LIM_UNSIGN)\
    X_BUF(98,	Reg_NTC_Temper_Number_Step,     136,    0,      SIZE_NTC_TABLE,\
                                                                        WRITE_R | EESAVE_R | LIM_UNSIGN)\
	X_BUF(119,	Reg_End,						0,	    0,		0,		READ_R)\


#endif /* MODBUS_X_H_INCLUDED */
