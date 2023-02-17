/** ###################################################################
**     Filename  : P15765.h
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 4/25/2016, 4:24 PM
**     Contents  :
**         User source code
**
** ###################################################################*/

/*** ---------------------------------------------------------------------------
**	COPYRIGHT: Indication Instruments Limited
**				Plot 19, Sector 6
**				Industrial Layout
**				Faridabad - 121006
**
**				Copyright 2016
**
**
** ---------------------------------------------------------------------------
** ==========  ===============	==============================================
** 
** Author:	Khusboo Lalani 
*****************************************************************************/



#ifndef __P15765_H
#define __P15765_H

#include "j1939.h"
#include "r_cg_macrodriver.h"
#include "customTypedef.h"
/* MODULE P15765 */


#define MAX_LENGTH	49
#define MAX_DATA_LENGTH	( MAX_LENGTH - 5) // - 7; for 6 frames

#define REQ_FUNCTIONAL_SA       		0x33            // TATA Diagnostics Specs
#define REQ_PHYSICAL_SA1        		0xF1
#define REQ_PHYSICAL_SA2        		0xF9
#define REQ_PHYSICAL_SA1_L        		0xF100
#define REQ_PHYSICAL_SA2_L        		0xF900
#define RESPONSE_PF1        					0xDA
#define RESPONSE_PF2        					0xDB


//#define PHYSICAL_RESPONSE_ID_SA1		( 0x1BDAF100 | PREFERRED_SA )
//#define PHYSICAL_RESPONSE_ID_SA2		( 0x1BDAF900 | PREFERRED_SA )

//#define PHYSICAL_RESPONSE_ID_BROADCAST	( 0x1BDB0000 | PREFERRED_SA )


typedef enum{

eSingle = 0,
eFirst,
eConsecutive,
eFlowControl,
eIdle, //supplier specific
eTxCompleted,//supplier specific
eTerminate,
eObsolete //this is used in case data is receieved in the single frame but copied into multiframe structure 
}FRAME_15765_TYPE;

typedef enum 
{
eClearToSend,
eWait,
eOverflowAndTerminate,
eSegmentedTxNotStarted,//supplier specific
eStopped,
eReserved

}MULTIPLE_FRAMES_STATUS;

//NEC MCU have a architecture little endian so change stucture according to it.
typedef struct
{	
    u8 dataBytes[8];

    struct
    {
		//All combined into numBytes_LSB_SerialNum_FlowStatus
		//byte numBytes     :4 0-reserved, valid 1-6, 7 with normal addressing, > 8 invalid
		//byte numBytesMSB  :4;	//if len > rx buf size, abort reception & send overflow fc
		//byte serialNum    :4;	//start with 1 because FF is considered 0, after reaching f, rollover to 0
		//byte flowStatus   :4;	//MSB //0 = cts ,1 = pause sending, wait for new FC & restart timer,2= overflow & terminate sending, 3-f reserved		
		u8 numBytes_MSB_SerialNum_FlowStatus       :4;
		u8 frameType	    						 :4;	//SF / FF / CF / FC //this is MSB
		struct{
			u8 numBytesLSB;
		    struct 
		    {	
				
				u8 blockSize;	//0-ff number of CFs; preferably 0 as per tata spec uds on CAN 1.7
				u8 minSepTime;	//LSB //0-7f = 0-125ms, f1-f9 = 100u-900u, rest reserved & assume 127 ms in those cases. <=100 ms as per Tata's reco
			}FC;	
		}MulPCI;
		
    }NPCI;

}p15765_MSG_TYPE;


typedef struct 
{
	u32 ID;
    p15765_MSG_TYPE msg15765;
	u32 timeStamp;
	u16 delay;
    u8 dataReady;
	bool frameInProgress;
}p15765_MSG_STRUCT_TYPE;

typedef struct
{
u8 mulByteDiagRespData[MAX_LENGTH + 20]; // contains all the data bytes of max. 7 frames minus the 15765 network information
u32 ID;
u32 timeStamp;
u16 numBytes;
u8 serialNum;
MULTIPLE_FRAMES_STATUS status;//this is set by the control frame from the reciver
u8 blockSize; //this is set by the control frame from the reciver
u8 minSepTime; //this is set by the control frame from the reciver
FRAME_15765_TYPE frameInProgress ;
bool dataReady;
bool numFrames;
}multiple_frames;

typedef struct
{

u32 ID;
u8 dataBytes[8];
u32 timeStamp;
u8 numBytes;
u8 dataReady;
}single_frame;

extern u8 testerAddress;
//extern byte gatewayRequestAdr;
//extern byte gatewayResponseAdr;
extern multiple_frames multipleFramesRx;
extern multiple_frames multipleFramesTx;
extern single_frame singleFrameRx; 

extern sJ1939_RX_MSG_TYPE diagnosticsBuf[RX_QUE_SIZE];
//extern p15765_MSG_STRUCT_TYPE UDSData;
extern u32 diagnosticsTimer;

//these variables need not be global get/set can be used
extern u32 HSCANBusOffRecoveryClk;

extern u16 BusOffRecoveryTimeout;
extern u32 diagnosticsCANId;

extern u8 HSCANBusOffFlag;
extern u8 VINNumDidNotMatchFlag;



extern void HSCANBusOffRecovery(void);
extern void diagnosticDataInit(void);
extern void processDiagnosticsMessages(void);
extern void sendDiagnosticsResponse (void);
extern void initDiagnosticsSession(void);
/*
typedef struct p15765_SingleFrame_Message_Struct
{
    j1939_Id_Struct_Type  MsgId;                // 29-bit message identifier
    bool                  frameTxInProgress;    // fast packet transmission in progress
    byte                  ByteCntr;             // Single frame data byte counter
    byte                 *BytePtr;              // Single frame data byte pointer
} p15765_TxMFPacketFrame_Type;


typedef struct
{
  bool   InProgress;                // Reception of FastPacket (false=Idle, true=InProgress)
  byte   SeqCounter;                // Incremented for each transmission
  byte   TxFrame_Counter;           // Number of the last frame transmitted
  byte   Num_Byte_Tx;               // Number of bytes to transfer
  byte   *Byte_Tx_Ptr;              // Pointer to the next byte to send
  byte   *data;                     // pointer to data to send
  word   dataSize;                  // total number of bytes to transfer
  byte   TxIndexer;                 // Indexer into both the J1939_TX_TYPE and J1939_TX_DATA_TYPE types
} p15765_TxMFPacket_Struct_Type;
 */


/* END P15765 */

#endif
