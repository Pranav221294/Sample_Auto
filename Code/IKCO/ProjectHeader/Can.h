#ifndef _CAN_H_
#define _CAN_H_

#include "customTypedef.h"

// constant declarations -----------------------------------------------------

#define CRxD_Port_PM	PM0.1
#define CRxD_Port		P0.1
#define CTxD_Port_PM	PM0.0
#define CTxD_Port		P0.0

//Recieving or Transmmiting mesage buffer starting address
#define	MSGBUF_BASE_ADD	((u32)(0x0F0600)) 

//15 buffers used for msg receiving and 1 buffer is used for msg transmmiting 
#define BUFFER_NUMBER_0		0
#define BUFFER_NUMBER_1		1
#define BUFFER_NUMBER_2		2
#define BUFFER_NUMBER_3		3
#define BUFFER_NUMBER_4		4
#define BUFFER_NUMBER_5		5
#define BUFFER_NUMBER_6		6
#define BUFFER_NUMBER_7		7
#define BUFFER_NUMBER_8		8
#define BUFFER_NUMBER_9		9
#define BUFFER_NUMBER_10	10
#define BUFFER_NUMBER_11	11
#define BUFFER_NUMBER_12	12
#define BUFFER_NUMBER_13	13
#define BUFFER_NUMBER_14	14
#define BUFFER_NUMBER_15	15
#define MSG_LEN 			8
#define MSG_LEN_3 			3


//Filter and Mask 
#define MASK_1			0x0000BFFF	 
#define MASK_2			0x0000FBFF	 
#define MASK_3			0x173FFFFF	 

#define FILTER_ID_1 	0x18FE40FF	 
#define FILTER_ID_2 	0x18FD04FF	  
#define FILTER_ID_3 	0x8C000FF

#define UDS_TX  	1
#define J1939_TX  	2


/*

#define MASK_1			0xFFFFFFFF	 
#define MASK_2			0xFFFFFFFF	 
#define MASK_3			0xFFFFFFFF	 

#define FILTER_ID_1 	0xFFFFFFFF	 
#define FILTER_ID_2 	0xFFFFFFFF	  
#define FILTER_ID_3 	0xFFFFFFFF	

*/




// Global declarations --------------------------------------------------------



// Global prototypes ---------------------------------------------------------

/// @addtogroup CAN_LIBRARY
/// @{
 
/// @defgroup CanHeaderLib Global  functions used in CAN file
//// 
///   
/// @addtogroup CanHeaderLib
/// @{


void vTxMsgBufProcessing(u32  ,u8  ,u8* ,bool);
void vCanInit();
void MY_CAN_Busoff();
void vReCANInit();
void vRecoveryFromBusOffState();
void vClearCanBusOffFlag();
bool bGetCanBusOffFlagStatus();
bool bCheckCanTxBufferAvailable(void);


/// @} // endgroup CanHeaderLib

/// @} // endgroup CAN_LIBRARY

#endif


