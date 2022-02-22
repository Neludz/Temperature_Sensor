#ifndef MAIN_H
#define MAIN_H
//=========================   include   =========================
//#include "measure_NTC.h"

//=========================   define   =========================
#define VERSION                     101    //1.01
#define SIZE_NTC_TABLE              166
 
// Reg_Zigbee_Set_Mode bit register
#define  ZI_SET_MODE_BIT_SET_FICR_ADDR     1
#define  ZI_SET_MODE_BIT_SET_CODE          2
#define  ZI_SET_MODE_BIT_DEFAULT           ZI_SET_MODE_BIT_SET_CODE

//Reg_Zigbee_Wait_Time_S
#define  ZI_SET_WAIT_TIME_S_DEFAULT        10

//Reg_Zigbee_Set_Channel
#define  ZI_SET_CHANNEL_DEFAULT             16

//delay start and reset
#define DELAY_FOR_POWER_TEST_MS             2500
#define DELAY_AFTER_POWER_TEST_MS           10000

/*
#define DELAY_FOR_ZI_RECONNECT_MS           1800000
#define DELAY_TOTAL_ZI_RECONNECT_MS         ((ZB_DEV_REJOIN_TIMEOUT_MS*2)+DELAY_FOR_ZI_RECONNECT_MS)
#define ATTEMPTS_TO_1_HOUR_RESET            (3600000/DELAY_TOTAL_ZI_RECONNECT_MS)
#define NUMBER_HOUR_TO_RESET                48
#define NUMBER_ATTEMPTS_BEFORE_RESET        48      //
*/
#define DELAY_TOTAL_ZI_RECONNECT_MS         ((ZB_DEV_REJOIN_TIMEOUT_MS+10000))

//Command
#define CODE_RESET_ZIGBEE_DATA          122
#define CODE_RESET_TO_DEFAULT           13
#define CODE_RESTART                    14
#define CODE_CONNECT_BUTTON             15
#define CODE_HARD_RESET_STACK           155

//other
#define  ADC_PAUSE_MS_MAIN_H                1000
//=========================   type   =========================


//=========================   prototype   =========================
void com_task (void *arg1, void *arg2, void *arg3);

#endif //MAIN_H