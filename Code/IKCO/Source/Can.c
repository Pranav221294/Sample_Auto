/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/


//***************Interrupt service routine*********************************

//#pragma	interrupt INTC0REC MY_Rx_MsgBuf_Processing
//#pragma	interrupt INTC0TRX MY_Tx_MsgBuf
//#pragma	interrupt INTC0ERR MY_ISR_CAN_Error
//#pragma interrupt INTC0WUP MY_ISR_CAN_Wakeup
//----------------------------------------------------------------------------------------------------------------
#include "r_cg_macrodriver.h"
#include "Can.h"
#include "J1939.h" 
#include "customTypedef.h"
#include "r_cg_wdt.h"
#include "Eol.h" 
#include "r_cg_userdefine.h"
// Global declarations --------------------------------------------------------

static bool bBusOffFlag = FALSE;
static bool bBusOffDtcFlag = FALSE;


static u16 *TRQ_REGISTER_BUF_14;
static u16 *TRQ_REGISTER_BUF_15;


// Private prototypes --------------------------------------------------------

/// @addtogroup CAN_LIBRARY
/// @{
 
/// @defgroup CanLib Private functions used in CAN file
//// 
///   
/// @addtogroup CanLib
/// @{

void _vCANMsgBufInit();
void _vCANInit();
void _vRxMsgBufInit(u8, u32);
void _vTxMsgBufInit(u8, u32, u8 );
void MY_Rx_MsgBuf_Processing(void);

/// @} // endgroup CanLib

/// @} // endgroup CAN_LIBRARY


/*********************************************************************//**
 *
 * CAN module initialization and assine buffer for CAN msg receiving
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vCanInit()
{
 	u32	MsgBuf_address;
	
	_vCANInit();
	_vCANMsgBufInit();
	_vRxMsgBufInit(BUFFER_NUMBER_0,FILTER_ID_1);
	_vRxMsgBufInit(BUFFER_NUMBER_1,FILTER_ID_1);
	_vRxMsgBufInit(BUFFER_NUMBER_2,FILTER_ID_1);
	_vRxMsgBufInit(BUFFER_NUMBER_3,FILTER_ID_1);
	_vRxMsgBufInit(BUFFER_NUMBER_4,FILTER_ID_1);
	_vRxMsgBufInit(BUFFER_NUMBER_5,FILTER_ID_1);
	
	_vRxMsgBufInit(BUFFER_NUMBER_6,FILTER_ID_2);
	_vRxMsgBufInit(BUFFER_NUMBER_7,FILTER_ID_2);
	_vRxMsgBufInit(BUFFER_NUMBER_8,FILTER_ID_2);	 
	_vRxMsgBufInit(BUFFER_NUMBER_9,FILTER_ID_2);
	_vRxMsgBufInit(BUFFER_NUMBER_10,FILTER_ID_2);

	_vRxMsgBufInit(BUFFER_NUMBER_11,FILTER_ID_3);
	_vRxMsgBufInit(BUFFER_NUMBER_12,FILTER_ID_3);
	_vRxMsgBufInit(BUFFER_NUMBER_13,FILTER_ID_3);

	//Set biffer used for Yx
	MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * BUFFER_NUMBER_14));
	TRQ_REGISTER_BUF_14 = ((unsigned char *)(MsgBuf_address + 0x0e));						  // Check TRQ bit

	MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * BUFFER_NUMBER_15));
	TRQ_REGISTER_BUF_15 = ((unsigned char *)(MsgBuf_address + 0x0e));						  // Check TRQ bit

	
}


/*********************************************************************//**
 *
 * CAN module initialization 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void _vCANInit()
{	 
  	 	eBAUD_RATE CanBd;

		SCAN0 = 1;				// 1-CAN pins on P00(TX) & P01(RX)
								// 0-CAN pins on P71(TX) & P70(RX)
		CRxD_Port_PM = 1;		//Make Rx pin input  	
		CRxD_Port = 1;					
		CTxD_Port_PM = 0;		//Make Tx pin Output 
		
		PCKSEL |= 0x10;    
		C0GMCS  = 0x00;
		
		//Set CAN priority level 1
        C0RECPR0 = 1;
        C0RECPR1 = 0;


		C0GMCTRL= 0x0100; 
		
		CanBd = eGetCanBaudRate();
		
		if(CanBd ==  e250)
		{
			
			C0GMCS	= 0x01;	   
			C0BRP = 0x07;		
			C0BTR	= 0x0104;			 
		}	
		else if(CanBd ==  e500)
		{
			C0GMCS	= 0x01;	   
			C0BRP = 0x03;  
			C0BTR	= 0x0104;	
		}
		else if(CanBd ==  e1000)
		{
			
			C0GMCS	= 0x01; 		   
			C0BRP = 0x01;  
			C0BTR	= 0x0104;
		}

		C0IE = 0x3f00;	
		C0RECIF = 0;			// clear receive interrupt flag
		C0RECMK = 0;			// Enable Receive interrupt
		C0WUPIF = 0;			// clear wakeup interrupt flag
		C0WUPMK = 0;			// enable wakeup interrupt
		C0ERRIF = 0;			// clear error interrupt flag
		C0ERRMK = 0;			// enable error interrupt

		C0CTRL   = 0x817E;   		// clear AL,VALID(no receive valid message),PSMODE1,PSMODE0(no power save mode)
		// clear OPMODE2,OPMODE1 and set OPMODE0(normal mode) 
		// set CCERC(clear C0ERC and C0INFO registers in initialization mode) 

		C0MASK1L = (u16)MASK_1;
		C0MASK1H = (u16)(MASK_1 >> 16);

		C0MASK2L = (u16)MASK_2;
		C0MASK2H = (u16)(MASK_2 >> 16);

		C0MASK3L = (u16)MASK_3;
		C0MASK3H = (u16)(MASK_3 >> 16);
   	

}

 
/*********************************************************************//**
 *
 * Re-initialization CAN module  , when user want to chnage baud rate in running time 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/ 
 void vReCANInit()
 {
 	eBAUD_RATE CanBd;
	static eBAUD_RATE PreviousCanBd = e250;
	CanBd = eGetCanBaudRate();

	if(PreviousCanBd != CanBd) 
	{
		PreviousCanBd = CanBd;
		 
	   while((C0CTRL & 0x0007) != 0x00)   // for init mode 
	   {
		 
		  C0CTRL &= 0xf8ff;   // regsiter bit set
		  C0CTRL |= 0x0007;    // register bit clear
		  NOP();NOP();
			
	   }
	   
	   	SCAN0 = 1;			 // 1-CAN pins on P00(TX) & P01(RX)
						    // 0-CAN pins on P71(TX) & P70(RX)
	 
		 CRxD_Port_PM =  1; 		 
		 CRxD_Port = 1; 				 
		 CTxD_Port_PM =  0;

		
			
		//Set CAN priority level 1
	        C0RECPR0 = 1;
	        C0RECPR1 = 0;
		 
		 if(CanBd ==	e250)
			 {
				 C0GMCS  = 0x01;	
				 C0BRP = 0x07;		 
				 C0BTR	 = 0x0104;			  
			 }	 
			 else if(CanBd ==  e500)
			 {
				
				 C0GMCS  = 0x01;	
				 C0BRP = 0x03;	
				 C0BTR	 = 0x0104;	 
			 }
			 else if(CanBd ==  e1000)
			 {
				 C0GMCS  = 0x01;			
				 C0BRP = 0x01;	
				 C0BTR	 = 0x0104;
			 }
	 
		 C0IE = 0x3f00;
		 
		 C0RECIF = 0;			 // clear receive interrupt flag
		 C0RECMK = 0;			 // Enable Receive interrupt
		 C0WUPIF = 0;			 // clear wakeup interrupt flag
		 C0WUPMK = 0;			 // enable wakeup interrupt
		 C0ERRIF = 0;			 // clear error interrupt flag
		 C0ERRMK = 0;			 // enable error interrupt
		 
		 
		 if((C0ERC ==0) && (C0INFO == 0))
		 {
			 C0CTRL|=0x8000;
		 }
	 
		  C0CTRL   = 0x817E;		 // clear AL,VALID(no receive valid message),PSMODE1,PSMODE0(no power save mode)
						 			// clear OPMODE2,OPMODE1 and set OPMODE0(normal mode) 
	}				 			// set CCERC(clear C0ERC and C0INFO registers in initialization mode) 
	 
 }



 /*********************************************************************//**
 *
 * CAN module buffer init. 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void _vCANMsgBufInit()
{
	u8	buffer_number;
	u32	MsgBuf_address;

	for (buffer_number = 0 ; buffer_number < 16 ; buffer_number++)		// Init all message buffer
	 {
		
		MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * buffer_number));	// Set CAN message buffer[n] register address
		
		*((u16 *)(MsgBuf_address + 0x0e)) = 0x0006;		// Clear TRQ, DN bit
				
		*((u16 *)(MsgBuf_address + 0x0e)) = 0x0001;		// Clear RDY bit
		
		*((u8 *)(MsgBuf_address + 0x09)) &= 0xf8;		// Clear MA0 bit
	 }
}

/*********************************************************************//**
*
* Assign buffer for transmit msg and   msg ID ,DLC feed in buffer address 
*
* @param	  buffer for transmit msg
*             Message ID to be transmit
*             Message length
*
* @return	  None
*************************************************************************/

void _vTxMsgBufInit( u8 buffer_number, u32 tx_msg_ID, u8 tx_msg_DLC)
{
	u32	MsgBuf_address;
	u8 data_cnt;

	MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * buffer_number));	// Set CAN message buffer[n] register address
	//clear ready bit
	*((u16 *)(MsgBuf_address + 0x0e)) = 0x0001; 
	
	// Set C0MCONFm register, Transmit message buffer, MA0=1,msg buffer used
	*((u8 *)(MsgBuf_address + 0x09)) = 0x01;      		

	*(( u16 *)(MsgBuf_address + 0x0a)) = (u16)tx_msg_ID;   			//Extended frame,C0MIDLm = ID0 to ID15;
	*(( u16 *)(MsgBuf_address + 0x0c)) = (u16)(tx_msg_ID >> 16) ; 	//C0MIDHm = ID16 to ID28;
	*(( u16*)(MsgBuf_address + 0x0c)) |= (u16)(1<<15); 				//C0MIDHm --> set IDE -->
																	//IDE = 1 --> Extended format mode (ID28 to ID0: 29 bits)


	*(( u8 *)(MsgBuf_address + 0x08)) = tx_msg_DLC;     			// set C0MDLCm -data length

	for(data_cnt = 0 ; data_cnt < tx_msg_DLC ; data_cnt++)			
	{
		*(( u8 *)(MsgBuf_address + (0x01 * data_cnt))) = 0x00;    // clear each byte data=0x00
	}

	#if 1
		*(( u16 *)(MsgBuf_address + 0x0e)) = 0x001e;  // C0MCTRLm
		// clear MOW,IE,DN,TRQ bit
		// MOV=0,The message buffer is not overwritten by a newly received data frame.
		// IE=0,Normal message transmission completion interrupt disabled
		// DN=0,A data frame or remote frame is not stored in the message buffer.
		// TRQ=0,No message frame transmitting request that is pending or being transmitted
	#endif

	#if 0							
		*(( u16 *)(MsgBuf_address + 0x0e)) = 0x0816;  // clear MOW,DN,TRQ bit
		// set IE=1,Normal message transmission completion interrupt enabled
	#endif
	
	*(( u16 *)(MsgBuf_address + 0x0e)) = 0x0100; 	// // Set RDY bit,The CAN module can write to the message buffer

}
 
/*********************************************************************//**
*
* Assign buffer for Receive msg and filter ID 
*
* @param	  Buffer Number
*             Filter ID 
*
* @return	  None
*************************************************************************/
void _vRxMsgBufInit( u8 buffer_number,u32 rx_mask_ID)
{
	u32 MsgBuf_address;

	MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * buffer_number));		// Set CAN message buffer[n] register address
	

	// Set C0MCONFm register 
	*(( u8 *)(MsgBuf_address + 0x09)) = 0x09;   // Receive message buffer(no mask), MA0=1,msg buffer used

	if((buffer_number==0)||(buffer_number==1)||(buffer_number==2)||(buffer_number==3)||(buffer_number==4)||(buffer_number==5))
	{
		*(( u8 *)(MsgBuf_address + 0x09)) = 0x11;    	// Receive message buf
	}
	else if ((buffer_number==6)||(buffer_number==7)||(buffer_number==8)||(buffer_number==9)||(buffer_number==10))
	{
		*(( u8 *)(MsgBuf_address + 0x09)) = 0x19;   // Receive message buffer(mask 2), MA0=1,msg buffer used
	}
	else if ((buffer_number==11)||(buffer_number==12)||(buffer_number==13)||(buffer_number==14))
	{
		*(( u8 *)(MsgBuf_address + 0x09)) = 0x21;   // Receive message buffer(mask 3), MA0=1,msg buffer used
	}
	else
	{
		// do nothing
	}

	*(( u16 *)(MsgBuf_address + 0x0a)) = (u16)rx_mask_ID;   		//Extended frame,C0MIDLm = ID0 to ID15;

	*(( u16 *)(MsgBuf_address + 0x0c)) = (u16)(rx_mask_ID >> 16) ; 	//C0MIDHm = ID16 to ID28;

	*(( u16 *)(MsgBuf_address + 0x0c)) |= (u16)(1<<15); 	        //C0MIDHm --> set IDE -->
	//IDE = 1 --> Extended format mode (ID28 to ID0: 29 bits)

	*(( u16 *)(MsgBuf_address + 0x0e)) = 0x0916;  // Set C0MCTRLm register 
	// clear MOW,DN,TRQ bit
	// MOV=0,The message buffer is not overwritten by a newly received data frame.
	// set IE=1,Valid message reception completion interrupt enabled.
	// DN=0,A data frame or remote frame is not stored in the message buffer.
	// TRQ=0,No message frame transmitting request that is pending or being transmitted

	*(( u16 *)(MsgBuf_address + 0x0e)) |= 0x0100;  // Set RDY bit,The CAN module can write to the message buffer

}

/*********************************************************************//**
*
* Check CAN Transmmsion Buffer Available
*
* @param	Buffer empty Status 
*
* @return	  None
*************************************************************************/

bool bCheckCanTxBufferAvailable()
{
	bool BufferStatusFlag = FALSE;
	
	if(((*TRQ_REGISTER_BUF_14) & 0x0002) == 0)
	{
		BufferStatusFlag = TRUE;
	}
	else if(((*TRQ_REGISTER_BUF_15) & 0x0002) == 0)
	{
		BufferStatusFlag = TRUE;
	}
	else
	{
		// DO NOTHING 
	}

	return BufferStatusFlag;
}

/*********************************************************************//**
*
* Transmit CAN message  
*
* @param	Message ID
*           Buffer number 
*			DLC
*			Message bytes
*
* @return	  None
*************************************************************************/
void vTxMsgBufProcessing(u32 ID_msg_rx ,u8 tx_msg_DLC,u8* tx_msg_data,bool TxType)
{
	u32  MsgBuf_address;
	u16  C0MCTRLm;
	u8 temp;
	u8 buffer_number;
	bool BuzzerCheckingFlag = TRUE;
	u16 BuzzerCheckingTimeOut = 0;

		if(TxType == UDS_TX)
		{

			BuzzerCheckingTimeOut = u16GetClockTicks();

			while(BuzzerCheckingFlag == TRUE)
			{
				if(((*TRQ_REGISTER_BUF_14) & 0x0002) == 0)
				{
					buffer_number = BUFFER_NUMBER_14;// send message
					BuzzerCheckingFlag = FALSE;
				}
				else if(((*TRQ_REGISTER_BUF_15) & 0x0002) == 0)
				{
					buffer_number = BUFFER_NUMBER_15;// send message
					BuzzerCheckingFlag = FALSE;
				}
				
				if(u16GetClockTicks() - BuzzerCheckingTimeOut > 100)
				{
				  BuzzerCheckingFlag = FALSE;
				}
			}
		}
		else
		{
		  		if(((*TRQ_REGISTER_BUF_14) & 0x0002) == 0)
				{
					buffer_number = BUFFER_NUMBER_14;// send message
				}
				else if(((*TRQ_REGISTER_BUF_15) & 0x0002) == 0)
				{
					buffer_number = BUFFER_NUMBER_15;// send message
				}
				else
				{
				 	return;
				}
			
		}
	

	MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * buffer_number));	// Set CAN message buffer[n] register address

	_vTxMsgBufInit(buffer_number,ID_msg_rx,tx_msg_DLC);

	C0MCTRLm = *((u8 *)(MsgBuf_address + 0x0e));		// Check TRQ bit
	if((C0MCTRLm & 0x0002) != 0)
	{
		temp = 0;
		return;
	}

	*((u16 *)(MsgBuf_address + 0x0e)) = 0x0001;    	// Clear RDY bit ,The message buffer can be written by software.

	C0MCTRLm = *((u8 *)(MsgBuf_address + 0x0e));


	if((C0MCTRLm & 0x0001) == 0)					// Set C0MDATAxm register
	{
		u8 data_cnt;

		for(data_cnt = 0 ; data_cnt < tx_msg_DLC ; data_cnt++)
		{
			*((u8 *)(MsgBuf_address + (0x01 * data_cnt))) = tx_msg_data[data_cnt];    //clear each byte data=0x00
		}

		*((u16 *)(MsgBuf_address + 0x0e)) = 0x0100;	// Set RDY bit

		*((u16 *)(MsgBuf_address + 0x0e)) = 0x0200;	// Set TRQ bit
	}

	return;
}


/*********************************************************************//**
*
* CAN interrupt service routing  
*
* @param	None
*
*
* @return	 None
*************************************************************************/
//__interrupt void MY_Rx_MsgBuf_Processing()
void MY_Rx_MsgBuf_Processing()
{
	vJ1939OnFullRx();
	
}

/*********************************************************************//**
*
* (Not using and Not tested)Set CAN Wake mode   
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/
//__interrupt void MY_ISR_CAN_Wakeup()
void MY_ISR_CAN_Wakeup()
{
	C0INTS = 0x0020;    			// clear CINTS5(Wakeup interrupt)
}

/*********************************************************************//**
*
* CAN error Handler  
*
* @param	None
*
*
* @return	  None
*************************************************************************/
//__interrupt void MY_ISR_CAN_Error(void)
void MY_ISR_CAN_Error(void)
{
	u16 read_regiter = 0;

	if((C0INTS & 0x0004) == 0x0004)   // // CINTS2
	{
						
		if((C0INFO & (u16)(1<<4)) == 0x10)		
		{
			bBusOffFlag = TRUE;
			bBusOffDtcFlag = TRUE;
		}
		
		C0INTS|=0x0004;     //(1<<2);  //0x0004
		
	}
	 if((C0INTS & 0x0008) == 0x0008);   // // CINTS3
	{
		read_regiter = C0LEC;
		C0INTS|=0x0008;             //(1<<3);  //0x0008
		
	}
	 if((C0INTS & 0x0010) == 0x0010)   // // CINTS4
	{
		C0INTS|=0x0010;            //(1<<4);        //0x0010
	 }
}

/*********************************************************************//**
*
* When the CAN module is in the bus-off state then The CAN module recovers 
*	from the bus-off state
*
* @param	None
*
*
* @return	  None
*************************************************************************/
void vRecoveryFromBusOffState()
{
   unsigned char temp;
   unsigned int *Ptr_C0MCTRL;

   if(!bBusOffFlag)
   		return;
   bBusOffFlag = FALSE;
   
   Ptr_C0MCTRL=&C0MCTRL00;
		
	for(temp=0;temp<16;temp++)
	{
	   *Ptr_C0MCTRL=  0x0002;		// Clear all TRQ, 	   
	    Ptr_C0MCTRL++;
	NOP();
	}
	C0CTRL &= 0xf8ff;    // C0CTRL = 0x0007;
	C0CTRL |= 0x0007;    // Clear OPMODE
	NOP();NOP();NOP();

	if(C0GMCTRL == 0x0000)       //SET GOM BIT
	{
		C0GMCTRL = 0x0200;   //SET EFSD BIT check it
		NOP();
		C0CTRL|=0x8000;  //SET CCERC BIT
		C0CTRL = 0x0700; //SET OPMODE
	}
	else
	{
		C0CTRL = 0x0700;  //SET OPMODE
	}

	
}

/*********************************************************************//**
*
* (Not using and Not tested)CAN error interrupt   
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/
//__interrupt void MY_Tx_MsgBuf()
void MY_Tx_MsgBuf()
{
    // CINTS0 Interrupt on normal completion of transmission of message frame from message buffer m
    C0INTS = 0x0001;
	
    NOP();
}


/*********************************************************************//**
*
* (Not using and Not tested)CAN sleep mode   
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/
void MY_CAN_SleepMode_Setting()
{
	C0CTRL = 0x0810;   					// set PSMODE0=1,PSMODE1=0, setting CAN sleep mode
	//while((C0CTRL&0x0008) == 0); 		// check PSMODE0=1
}

/*********************************************************************//**
*
* (Not using and Not tested)CAN stop mode   
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/
void MY_CAN_StopMode_Setting()
{
	MY_CAN_SleepMode_Setting();
	C0CTRL = 0x1800;   			// set PSMODE0=1,PSMODE1=1, setting CAN stop mode
	while((C0CTRL&0x0010) == 0); 		// check PSMODE1=1
}
/*********************************************************************//**
*
* (Not using and Not tested)CAN stop mode release 
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/

void MY_CAN_StopMode_Release()
{
	C0CTRL = 0x0810;   			// clear PSMODE1=0, release stop mode to sleep mode
}

/*********************************************************************//**
*
* (Not using and Not tested)CAN sleep mode release 
*
* @param	CAN interrupt service routing  
*
*
* @return	  None
*************************************************************************/

void MY_CAN_SleepMode_Release()
{
	C0CTRL = 0x0008;    			// clear PSMODE0=0,release sleep mode by software
	
	#if 0
	//Sleep mode can also be released by a falling edge at the CAN reception pin---wakeup signal
	C0INTS = 0x0020;    			// clear CINTS5(Wakeup interrupt)
	#endif
}

/*********************************************************************//**
*
*  Get CAN Buff off status
*
* @param	None
*
*
* @return	  CAN Bus off status
*************************************************************************/

bool bGetCanBusOffFlagStatus()
{
	return bBusOffDtcFlag;
}

/*********************************************************************//**
*
*  Clear bus off status 
*
* @param	None
*
*
* @return	  CAN Bus off status
*************************************************************************/
void vClearCanBusOffFlag()
{
	 bBusOffDtcFlag = FALSE;
}






