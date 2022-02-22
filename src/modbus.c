

#include "modbus.h"

#include "modbus_reg.h"
//==================CRC=================================

/* Table of CRC values for high–order byte */
const unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;
/* Table of CRC values for low–order byte */
const char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};


unsigned int CRC16 ( unsigned char *puchMsg, unsigned int usDataLen )
{
	unsigned char uchCRCHi = 0xFF ; 						/* high byte of CRC initialized  	*/
	unsigned char uchCRCLo = 0xFF ; 						/* low byte of CRC initialized  	*/
	unsigned char uIndex; 									/* will index into CRC lookup table	*/

	while (usDataLen--) {
		uIndex = uchCRCLo ^ *puchMsg++;  					/* calculate the CRC   */
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return ((unsigned int)uchCRCHi << 8 | uchCRCLo);
}

//======================================================
// check registers from request
#ifdef EEPROM_REG
eMBEep  Eeprom_Check_in_Request (uint16_t Start_Reg, uint16_t Count)
{
	if((Count+Start_Reg)>=NUM_BUF)
	{
	return 	EEP_FREE;
	}
	for (int32_t i = 0; i < Count; i++)
	{
		if(EESave_Check(Start_Reg+i)==REG_OK)
		{
		return EEP_SAVE;
		}
	}
return 	EEP_FREE;
}
#endif
//--------------------------------------------------------------------------------------
#ifdef LIMIT_REG
eMBExcep Limit_Check_in_Request (uint16_t Number_Reg, uint16_t Value)
{
	if (Write_Check(Number_Reg)==REG_ERR)
	{
	return MBE_ILLEGAL_DATA_ADDRESS;
	}

	if (Limit_Check (Number_Reg, Value)==REG_ERR)
	{
	return	MBE_ILLEGAL_DATA_VALUE;
	}

return MBE_NONE;
}
#endif
//--------------------------------------------------------------------------------------

void MBparsing(mb_struct *mbb)
{

	if( InvalidFrame(mbb)) {
		mbb->mb_state = STATE_IDLE;
		return;
	}
	if( FrameParse(mbb)) // Returns TRUE if a response is needed
	    {
        mbb->mb_state=STATE_SEND;
	    mbb->mb_index=0;
	    mbb->f_start_trans(mbb);
        }
	else
	    {
         mbb->mb_state=STATE_IDLE;
        }
#ifdef EEPROM_REG
    if  (mbb->eep_state==EEP_SAVE)
        {
        mbb->f_save(mbb);
        }
#endif
}
//--------------------------------------------------------------------------------------

bool InvalidFrame( mb_struct *mbb)
{
	uint8_t	PDU_len;
	if( EV_HAPPEND == mbb->er_frame_bad) {
		return true;
	}
	if( mbb->mb_index < MB_FRAME_MIN) {
		return true;
	}
	if((mbb->slave_address != mbb->p_mb_buff[0])&(mbb->p_mb_buff[0]!=MB_ADDRESS_BROADCAST)&(mbb->slave_address!=0)) {
		return true;
	}
#ifdef EEPROM_REG
	if(mbb->eep_state!=EEP_FREE) {
        return true;
    }
#endif
	PDU_len = mbb->mb_index;
	if( CRC16( (uint8_t*)mbb->p_mb_buff, PDU_len)) {
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------------

bool FrameParse (mb_struct *mbb)
{							// Returns TRUE if a response is needed
#ifdef LIMIT_REG
uint16_t 	Value, RegIndxInter, j;
//eMBExcep	ExceptionInter;
#endif

uint16_t 	RegIndx, RegNmb, RegLast,   i;
uint8_t		BytesN;
eMBExcep	Exception;					// If a Modbus exception happens - we put the var in MBBuff[2]

bool NeedResponse = true;
	if( mbb->p_mb_buff[0] == MB_ADDRESS_BROADCAST)
	{
	NeedResponse = false;				// We parse the request but we don'n give a response
	}

	switch( mbb->p_mb_buff[1])
	{					// Function code
//--------------------------------------------------
	case MB_FUNC_READ_HOLDING_REGISTER:
					//		  03	0...LASTINDEX-1      0...125
					// addr  func    AddrHi  AddrLo  QuantHi  QuantLo  CRC CRC
					//  0	  1			2		3		4		5		 6	7
		if( 8 == mbb->mb_index)
		{								// In this function mb_index == 8.
		RegIndx = (mbb->p_mb_buff[2]<<8) | (mbb->p_mb_buff[3]&0xFF);
		RegNmb  = (mbb->p_mb_buff[4]<<8) | (mbb->p_mb_buff[5]&0xFF);
		RegLast = RegIndx + RegNmb;
			if( (RegIndx > mbb->reg_read_last) ||
                ((RegLast-1) > mbb->reg_read_last) ||
				 (RegNmb>MB_MAX_REG) )			// max quantity registers in inquiry
            {
			Exception = MBE_ILLEGAL_DATA_ADDRESS;
			break;
            }
														// Make response. MBBuff[0] and MBBuff[1] are ready
			mbb->p_mb_buff[2] = RegNmb << 1;
			mbb->mb_index = 3;
	        	while( RegIndx < RegLast )
	        	{
	        	mbb->p_mb_buff[mbb->mb_index++] = (uint8_t)(*(mbb->p_read+RegIndx) >> 8)  ;
	        	mbb->p_mb_buff[mbb->mb_index++] = (uint8_t)(*(mbb->p_read+RegIndx) & 0xFF);
	        	++RegIndx;
	        	}
	    Exception = MBE_NONE;						// OK, make CRC to MBBuff[mb_index] and response is ready
	    break;
		}
	Exception = MBE_ILLEGAL_DATA_VALUE;	// PDU length incorrect
	break;
		//--------------------------------------------------
	case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
					//		  16	0...LASTINDEX-1      0...125	 0...250
					// addr  func    AddrHi  AddrLo  QuantHi  QuantLo  Bytes  RG1Hi RG1LO ... CRC CRC
					//  0	  1			2		3		4		5		 6		7	  8	  ...

		if( mbb->mb_index > 10)
		{
		Exception = MBE_NONE;						// ...and mb_index is a length of response
		       											// The must: mb_index == 11, 13, 15, ...
		RegIndx = (mbb->p_mb_buff[2]<<8) | (mbb->p_mb_buff[3]&0xFF);
		RegNmb  = (mbb->p_mb_buff[4]<<8) | (mbb->p_mb_buff[5]&0xFF);
		RegLast = RegIndx + RegNmb;
		BytesN	= mbb->p_mb_buff[6];
			if(  (RegIndx > mbb->reg_write_last) ||
				((RegLast-1) > mbb->reg_write_last) ||
				(RegNmb>MB_MAX_REG))
			{
			Exception = MBE_ILLEGAL_DATA_ADDRESS;
			break;
			}
			if( BytesN != (RegNmb << 1) ||
				mbb->mb_index != (9+BytesN) )
			{				// 1 reg - mb_index=11, 2 regs - 13,... 5 regs - 19, etc.
			Exception = MBE_ILLEGAL_DATA_VALUE;
			break;
			}

		i = 7;							// Registers' values are from MBBuff[7] and more
		#ifdef EEPROM_REG
		//=================check EEPROM start==============================
		mbb->eep_state = Eeprom_Check_in_Request(RegIndx, RegNmb);
			if(mbb->eep_state==EEP_SAVE)
			{
			mbb->eep_start_save = RegIndx;
			mbb->eep_indx = RegNmb;
			}
		//=================check EEPROM end==============================
		#endif

		#ifdef LIMIT_REG
		//=================check data start==============================
			j = i;
	            for (RegIndxInter = RegIndx; RegIndxInter < RegLast; RegIndxInter++)
				{
				Value = mbb->p_mb_buff[j++]<<8;
				Value |= mbb->p_mb_buff[j++];
				Exception = Limit_Check_in_Request(RegIndxInter, Value);
					if	(Exception != MBE_NONE)
					{
					break;
					}
				}
	            if(Exception != MBE_NONE)
				{
				break;
				}
		//=================check data end==============================
		#endif
			while( RegIndx < RegLast)
			{
			*(mbb->p_write+RegIndx)  = mbb->p_mb_buff[i++]<<8;	// High, then Low byte
			*(mbb->p_write+RegIndx) |= mbb->p_mb_buff[i++]	;	// ... are packed in a WORD
			++RegIndx;
			}
		mbb->mb_index = 6;									// MBBuff[0] to MBBuff[5] are ready (unchanged)
		break;
		}
	Exception = MBE_ILLEGAL_DATA_VALUE;				// PDU length incorrect
	break;

		//--------------------------------------------------
	case MB_FUNC_WRITE_REGISTER:
					//		  06		0...250     	 0...255
					// addr  func    AddrHi  AddrLo   RG1Hi  	RG1LO   CRC		 CRC
					//  0	  1			2		3		4		  5	     6	 	  7

		if( mbb->mb_index == 8)
		{
		Exception = MBE_NONE;						// ...and mb_index is a length of response
		RegIndx = (mbb->p_mb_buff[2]<<8) | (mbb->p_mb_buff[3]&0xFF);

			if((RegIndx > mbb->reg_write_last))
			{
			Exception = MBE_ILLEGAL_DATA_ADDRESS;
			break;
			}

		i = 4;					// Registers' value are from MBBuff[4]-[5]
		#ifdef EEPROM_REG
		//=================check EEPROM start==============================
		mbb->eep_state = Eeprom_Check_in_Request(RegIndx, 1);
			if(mbb->eep_state==EEP_SAVE)
			{
			mbb->eep_start_save = RegIndx;
			mbb->eep_indx = 1;
			}
		//=================check EEPROM end==============================
		#endif

		#ifdef LIMIT_REG
		//=================check data start==============================
		RegIndxInter = RegIndx;
		j = i;

		Value = mbb->p_mb_buff[j++]<<8;
		Value |= mbb->p_mb_buff[j++];
		Exception = Limit_Check_in_Request(RegIndxInter, Value);
			if	(Exception != MBE_NONE)
			{
			//mbb->eep_state = EEP_FREE;
			break;
			}
		//=================check data end==============================
		#endif
		*(mbb->p_write+RegIndx)  = mbb->p_mb_buff[i++]<<8;	// High, then Low byte
		*(mbb->p_write+RegIndx) |= mbb->p_mb_buff[i++]	;	// ... are packed in a WORD

		mbb->mb_index = 6;									// MBBuff[0] to MBBuff[5] are ready (unchanged)

		break;
		}
	Exception = MBE_ILLEGAL_DATA_VALUE;				// PDU length incorrect
	break;
		//--------------------------------------------------
	default:
	Exception = MBE_ILLEGAL_FUNCTION;
	break;
	}

	/*
	 * At this point the "mb_index" is a length of the response (w/o CRC bytes)
	 * mb_index is variable if MB_FUNC_READ_HOLDING_REGISTER, 6 if MB_FUNC_WRITE_MULTIPLE_REGISTERS)
	 * but if there's some exception - it will be shortened to 3 in both cases
	 */

	if( Exception != MBE_NONE)
	{						// Any exception?
	mbb->p_mb_buff[1] |= MB_FUNC_ERROR;						// Add 0x80 to function code
	mbb->p_mb_buff[2] =  Exception;							// Exception code
	mbb->mb_index = 3;										// Length of response is fixed if exception
#ifdef EEPROM_REG
	mbb->eep_state = EEP_FREE;
#endif
	}
	i = CRC16( (uint8_t*)mbb->p_mb_buff, mbb->mb_index);				// MBBuff is a pointer, mb_index is a size
	mbb->p_mb_buff[mbb->mb_index++] = i & 0xFF;						// CRC: Lo then Hi
	mbb->p_mb_buff[mbb->mb_index  ] = i >> 8;
	mbb->response_size = mbb->mb_index+1;
	return NeedResponse? true:false;
}











