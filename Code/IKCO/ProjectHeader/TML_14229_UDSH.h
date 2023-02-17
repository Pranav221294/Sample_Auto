/** ###################################################################
**     Filename  : TML_14229_UDSH.h
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 5/4/2016, 5:01 PM
**     Contents  :
**         User source code
**
** ###################################################################*/

/*****************************************************************************
**  Module: TML_14229_UDSH.h
**
** ---------------------------------------------------------------------------
**  Description:    Header file for TML_14229_UDS.c
**
/*** ---------------------------------------------------------------------------
**  COPYRIGHT: Indication Instruments Limited
**              Plot 19, Sector 6
**              Industrial Layout
**              Faridabad - 121006
**
**              Copyright 2016
**
**
** ---------------------------------------------------------------------------
** ==========  ===============	==============================================
** 
** Author:  Khusboo Lalani 
*****************************************************************************/



#ifndef __TML_14229_UDSH_H
#define __TML_14229_UDSH_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "P15765H.h"



/* MODULE TML_14229_UDSH */
  //Negative Responses
  
#define ServiceNotSupported                     0x11 
#define ServiceNotSupportedInActiveSession      0x7F
#define SecurityAccessDenied                    0x33 
#define RequiredTimeDelayNotExpired             0x37 
#define ExceededNumberofAttempts                0x36 
#define InvalidKey                              0x35 
#define WrongBlockSequenceCounter               0x73 
#define SubfunctionNotSupported                 0x12 
#define IncorrectMessageLengthOrInvalidFormat   0x13 
#define RequestOutOfRange                       0x31 
#define RequestSequenceError                    0x24 
#define RequestCorrectlyReceivedResponsePending 0x78 
#define ConditionsNotCorrect                    0x22 
#define GeneralProgrammingFailure               0x72
 
#define NegativeResponseID						0x7F
 
#define P1ServerMax		0x32 //100ms
#define P1ServerMaxMSB	0
#define P1ServerMaxLSB	0x32
 
#define P2ServerMax		0x1388 //5000ms
#define P2ServerMaxMSB	0x13
#define P2ServerMaxLSB	0x88

// number of ids that can be read at a time
#define NUM_READ_ID_SUPPORTED 6

//Partial cluster Variant number
#define VARIANT_BYTE1	55
#define VARIANT_BYTE2	7
#define VARIANT_BYTE3	54
#define VARIANT_BYTE5	99


typedef enum
{
	eDefaultSession = 1,
    eProgrammingSession,
    eExtendedSession = 0x60,
	eNoActiveSession = 0x61
}DIAG_SESSION_TYPE;  

typedef enum
{
	eServerLocked,
	eServerUnlocked

}SERVER_LOCK_TYPE;


typedef enum
{
	eNone,	
	eRequestSeed,
	eSendKey

}SECURITY_ACCESS_TYPE;

typedef enum
{
	eHardReset = 1,
	eKeyOffOnReset,
	eSoftReset,
	eEnableRapidPowerShutdown,
	eDisableRapidPowerShutdown
	
}ECU_RESET_TYPE;

typedef enum
{
	eEnableRxTx,
	eEnableRxDisableTx,
	eDisableRxEnableTx,
	eDisableRxandTx
}COMM_CONTROL_TYPE;

typedef enum
{
	eNormalComm = 1,
	eNMComm,
	eNormalCommNMComm
}COMM_TYPE;

typedef enum
{
	eOn = 1,
	eOff
}STATE_TYPE;

typedef enum
{
	eBody,
	eNetwork,
	eAll,
	eNumofGroups
}DTC_GROUP_TYPE;

typedef enum
{
	eDTCNumByMask = 1,
	eDTCbyStatusMask,
	eSupportedDTC = 0xa,
	eFirstTestFailed,
	eFirstConfirmed,
	eMostRecentTestFailed,
	eMostRecentConfirmed,
	eDTCPermanentStatus =0x15
}DTC_INFO;

 
typedef struct
{
 u8 normalCommTxEnabled :1 ;
 u8 normalCommRxEnabled :1  ;
 u8 networkCommTxEnabled :1 ;
 u8 networkCommRxEnabled :1  ;
 u8 noResponse :1;
}COMM_STRUCT_TYPE;

//control with Cluster (FALSE) or test equipment(TRUE)
typedef struct
{
	u8	 u8SpeedoData;
	u8	 u8TachoData;
	u8	 u8FuelData;	
	u8	 u8CoolantTempData;
	u8	 u8FrontApgData;
	u8	 u8RearApgData;
	u8	 u8OilPressureData;
	u8	 u8DefData;
	bool 	 tellTalesFlag : 1;
	union
	{
		u8 u8IoControlFlagsByte;
		struct
		{

			bool DefEnableFlag : 1; //bit 0
			bool OilPressureEnableFlag : 1;
			bool RearApgEnableFlag : 1;
			bool FrontApgEnableFlag : 1;
			bool FuelGaugeFlag : 1;
			bool TempGaugeFlag : 1; //bit 7
			bool tachometerFlag : 1;
			bool speedometerFlag : 1;
			
		}bits;

	}IoControlFlag;

}TEST_EQUIPMENT_CONTROL;


#define CHECK_PROG_DEP 								0xFF01U

#define DTC_STATUS_MASK_SUPPORTED  0xFF //set the bits that are supported in structure "statusByte.bits" to 1
#define DTC_STATUS_MASK_PERMANENT  0x08 //set the bits that are supported in structure "statusByte.bits" to 1
//Diagnostic Services
 
#define StartDiagnosticSession  					0x10
#define ECUReset									0x11
#define SecurityAccess								0x27
#define	CommunicationControl						0x28
#define	TesterPresent 								0x3E
#define	AccessingTimingParameter 					0x83
#define SecureDataTransmission						0x84
#define ControlDTCSetting 							0x85
#define LinkControl 								0x87
#define ReadDataIdentifier 							0x22
#define ReadMemorybyAddress 						0x23
#define ReadScalingDatabyIdentifierRequest 			0x24
#define ReaddatabyPeriodicDataIdentifier 			0x2A
#define WriteDatabyIdentifier 						0x2E
#define WriteMemoryAddress							0x3D
#define ClearDiagnosticInformation					0x14
#define	ReadDTCInformationService	 				0x19
#define InputandOutputControlbyId		 			0x2F
#define RoutineControlService 						0x31

 
#define TMLSpecificLower							0x40
#define TMLSpecificUpper							0x5F
#define SystemSupplierSpecific						0x60

 // dignostics message timeouts
#define SECURITY_ACCESS_DELAY						9000 //10s
#define NO_MESSAGE_TIMEOUT							5000 //5s
#define NEXT_CAN_FRAME_TIMEOUT						750 // 90% of 1000
#define MIN_SEP_TIME								100 //127ms

//global variables
extern u8 frameConrolDataToSend[8];

//cluster variants
extern u8 PARTIAL_BASE_12V_AIR[2];

// all communication is true by default
extern COMM_STRUCT_TYPE commInfo ;
extern TEST_EQUIPMENT_CONTROL sTestEquimentControl;

//global functions
extern void setStatusDefaultSession(void);
extern void resetDiagnosticSession(void);
extern void sessionTimeout(void);
extern bool isItTimeToReset(void);
//global functions : diagnostics services
extern void startDiagnosticsSession (void);
extern void ECUResetService(void);
extern void securityAccessService(void);
extern void securitySessionTimeOutInit(void);
extern void communicationControlService(void);
extern void TesterPresentService(void);

//extern void AccessingTimingParService(void);
extern void LinkControlService(void);
extern void RoutineService(void);
extern void ClearDiagnosticsInformationService(void);
extern void ReadDiagnosticTroubleInformationService(void);
extern void InputOutputControlByIdentifierService(void);
extern void InputOutputControlByIdentifierServiceMul(void);

extern void ControlDTCSettingServiceSingle(void);
extern void ControlDTCSettingServiceMultiple(void);
extern void ReadDataByIdentifierServiceSingle(void);
extern void ReadDataByIdentifierServiceMultiple(void);
extern void WriteDataByIdentifierServiceSingle(void);

extern void data_encrypt(u8 *state, const u8 *key);
extern void data_decrypt(u8 *state, const u8 *key);

extern u16 u16GetClockTicks(void);
extern u32 u32GetClockTicks(void);
bool bIsECUResetByService(void);
u16 u16GetRoutineControlServiceStatus(void);
u16 u16getSoftwareVersion(void);

//cluster variants
/*
#define INSTRUMENT_CLUSTER_BASE_LEVEL_24V_AIR	 			5507 5424 99 04 
#define INSTRUMENT_CLUSTER_STANDARD_LEVEL_24V_AIR 			5507 5424 99 06 
#define INSTRUMENT_CLUSTER_MID_LEVEL_24V_AIR 				5507 5424 99 08 
#define INSTRUMENT_CLUSTER_BASE_LEVEL_12V_HYD 				5507 5420 99 09 
#define INSTRUMENT_CLUSTER_BASE_12V_AIR 					5507 5420 99 13 
#define INSTRUMENT_CLUSTER_STANDARD_LEVEL_12V_HYD 			5507 5420 99 14 
#define INSTRUMENT_CLUTSER_STANDARD_12V_AIR 				5507 5420 99 15 
#define INSTRUMENT_CLUSTER_MID_LEVEL_12V_HYD 				5507 5420 99 16 
#define INSTRUMENT_CLUSTER_MID_12V_AIR 						5507 5420 99 17 
*/
	
// only the different bytes (4&6) in the variant number
//byte PARTIAL_BASE_LEVEL_24V_AIR[2] 	= {24,4}; 
//byte PARTIAL_STANDARD_LEVEL_24V_AIR[2]	= {24,6};
//byte PARTIAL_MID_LEVEL_24V_AIR[2] = {24,8};			

//byte PARTIAL_MID_12V_AIR[2] = {20,17};
//byte PARTIAL_STANDARD_12V_AIR[2] = {20,15};
//byte PARTIAL_BASE_LEVEL_12V_HYD[2] 	= {20,9};
//byte PARTIAL_STANDARD_LEVEL_12V_HYD[2] 	= {20,14};			
//byte PARTIAL_MID_LEVEL_12V_HYD[2] = {20,16};		

/* END TML_14229_UDSH */

#endif
