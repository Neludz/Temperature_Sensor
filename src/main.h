#ifndef MAIN_H
#define MAIN_H

//-----------------------------------------------------------------------
// define
//-----------------------------------------------------------------------

#define VERSION                             101    //1.01
#define SIZE_NTC_TABLE                      166
 
#define TEMPERATURE_SENSOR_ENDPOINT         10

#define MAIN_STACKSIZE                      2048
#define MAIN_PRIORITY                       -1

// Reg_Zigbee_Set_Mode bit register
#define  ZI_SET_MODE_BIT_SET_FICR_ADDR      1
#define  ZI_SET_MODE_BIT_SET_CODE           2
#define  ZI_SET_MODE_BIT_DEFAULT            ZI_SET_MODE_BIT_SET_CODE

//Reg_Zigbee_Wait_Time_S
#define  ZI_SET_WAIT_TIME_S_DEFAULT         10

#define TIME_BETWEEN_NEXT_CONNECTION_MS     5000 
#define MAX_SENSOR_IN_ROUTER                16    //from router

//Reg_Zigbee_Set_Channel
#define  ZI_SET_CHANNEL_DEFAULT             16

//delay start and reset
#define DELAY_FOR_POWER_TEST_MS             2500
#define DELAY_AFTER_POWER_TEST_MS           10000

#undef  ZB_DEV_REJOIN_TIMEOUT_MS
#define ZB_DEV_REJOIN_TIMEOUT_MS            200000

#define DELAY_TOTAL_ZI_RECONNECT_MS         ((ZB_DEV_REJOIN_TIMEOUT_MS+20000))

//Command
#define CODE_RESET_ZIGBEE_DATA              122
#define CODE_RESET_TO_DEFAULT               13
#define CODE_RESTART                        14
#define CODE_CONNECT_BUTTON                 15
#define CODE_HARD_RESET_STACK               155


#define WDT_MAX_WINDOW                      9000U

//-----------------------------------------------------------------------
// prototype
//-----------------------------------------------------------------------
void com_task (void *arg1, void *arg2, void *arg3);

#endif //MAIN_H