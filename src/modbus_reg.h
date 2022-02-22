#ifndef MODBUS_REG_H_INCLUDED
#define MODBUS_REG_H_INCLUDED
//======================Include========================================
#include <stdint.h>
#include "modbus_x_macros.h"
#include "modbus.h"
//======================Define========================================


//=========================   Modbus registers X macros   =========================

// If not define "MODBUS_REG_END_TO_END" then number register is determined in "a" field from X-macros

//#define MODBUS_REG_END_TO_END

#ifndef MODBUS_REG_END_TO_END
#define REG_COUNT                Reg_End+1
#endif

typedef enum {
    REG_OK 		= 0x00,
    REG_ERR 		= 0x01
} Reg_Error_Code;


#define LIM_BIT_MASK	(0x0C)	//check limit mask (2&3 bits)

//  MAIN_BUF_Start_Table_Mask
#define READ_R			(0)
#define WRITE_R			(0x01)	// 0 bit
#define EESAVE_R		(0x02)	// 1 bit
#define LIM_SIGN		(0x04)	// 2 bit for limit     		  <--|
#define LIM_UNSIGN		(0x08)  // 3 bit for limit	    	  <--|----------------
#define LIM_MASK	        (0x0C)	// 2 and 3 bit for limit	  <--|				  |
//																				  |
//	 Number		Name for enum			Default	   Min	    Max   	__________Permission_______
//										 Value    Level    Level   |  R/W     EEPROM    LIMIT  |
//														  or Mask  | 						   |

//------========MAIN_BUF_End_Table========------

typedef struct
{						//X_BUF(a,b,c,d,e,f)
uint16_t		Default_Value;		//c
uint16_t		Min_Level;			//d
uint16_t		Max_Level_Mask;		//e
uint32_t		Permission;			//f
} t_default_state;

#ifdef MODBUS_REG_END_TO_END
 enum
{
  #define X_BUF(a,b,c,d,e,f) b,
          MAIN_BUF_TABLE
  #undef  X_BUF
          NUM_BUF  //Reg count
};
#else
 enum
{
  #define X_BUF(a,b,c,d,e,f) b=a,
          MAIN_BUF_TABLE
  #undef  X_BUF
          NUM_BUF=REG_COUNT
};
#endif

 //=========================   prototype   =========================

 Reg_Error_Code EESave_Check (uint16_t Number_Reg);
 Reg_Error_Code Write_Check (uint16_t Number_Reg);
 Reg_Error_Code Limit_Check (uint16_t Number_Reg, uint16_t Value);
 Reg_Error_Code All_Idle_Check (mb_struct *st_mb);

#endif /* MODBUS_REG_H_INCLUDED*/
