#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED
#include <stdint.h>
#include <stdbool.h>


#define MB_FRAME_MIN     		4       /* Minimal size of a Modbus RTU frame	*/
#define MB_FRAME_MAX     		256     /* Maximal size of a Modbus RTU frame	*/
#define MB_ADDRESS_BROADCAST  	00		/* MBBuff[0] analysis					*/

#define MB_ANY_ADDRESS		  	00		/* 0 - any address						*/
#define MB_MAX_REG				120		/*max quantity registers in inquiry. Should be less than MB_FRAME_MAX considering service bytes. Use for 03 function*/

#define MB_FUNC_NONE							00
#define MB_FUNC_READ_COILS						01
#define MB_FUNC_READ_DISCRETE_INPUTS			02
#define MB_FUNC_WRITE_SINGLE_COIL				05
#define MB_FUNC_WRITE_MULTIPLE_COILS			15
#define MB_FUNC_READ_HOLDING_REGISTER			03	/* implemented now	*/
#define MB_FUNC_READ_INPUT_REGISTER				04
#define MB_FUNC_WRITE_REGISTER					06	/* implemented now	*/
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS		16	/* implemented now	*/
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS	23
#define MB_FUNC_ERROR							0x80


typedef enum {			// Actually only 1 variable uses this type: er_frame_bad
	EV_NOEVENT,
    EV_HAPPEND
} eMBEvents;


typedef enum {
    MBE_NONE 					= 0x00,
    MBE_ILLEGAL_FUNCTION 		= 0x01,
    MBE_ILLEGAL_DATA_ADDRESS	= 0x02,
    MBE_ILLEGAL_DATA_VALUE		= 0x03
} eMBExcep;

typedef enum {
    STATE_IDLE,			// Ready to get a frame from Master
    STATE_RCVE,			// Frame is being received
    STATE_PARS,			// Frame is being parsed (may take some time)
    STATE_SEND,			// Response frame is being sent
    STATE_SENT			// Last byte sent to shift register. Waiting "Last Bit Sent" interrupt

} eEM_state;



typedef enum {
    EEP_FREE = 0,
    EEP_SAVE = 1
} eMBEep;


typedef struct {					// Main program passes interface data to Modbus stack.
	uint16_t    *p_write;			// Pointer to the begin of ParsIn array. Modbus writes data in the array
	uint16_t    *p_read;			// Pointer to the begin of ParsWk array. Modbus takes data from the array
	uint16_t    reg_read_last;		//
    uint16_t    reg_write_last;		//
	uint16_t    eep_start_save;		//	function modbus write here data start index
	uint8_t	    eep_indx;			// and how many registers are pending validation
	eMBEep  	eep_state;			//
	uint8_t     slave_address;		//
	uint8_t	    mb_index;
	eEM_state   mb_state;
	eMBEvents	er_frame_bad;		//
	uint8_t     *p_mb_buff;			//
	uint8_t		response_size;		// Set in FrameParse(), used in transmit
	void   		(*f_save) ( void *mbb);										//for span of register, depending on implementation of Eeprom_Check_Func
	void    	(*f_start_trans) ( void *mbb);
} mb_struct;



unsigned int CRC16 ( unsigned char *puchMsg, unsigned int usDataLen ) ;
void MBparsing(mb_struct *mbb);
bool InvalidFrame( mb_struct *mbb);
bool FrameParse (mb_struct *mbb);
eMBEep  Eeprom_Check_in_Request (uint16_t Start_Reg, uint16_t Count);
eMBExcep Limit_Check_in_Request (uint16_t Number_Reg, uint16_t Value);




#endif /* MODBUS_H_INCLUDED */
