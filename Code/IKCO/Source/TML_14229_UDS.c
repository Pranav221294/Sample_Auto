/** ###################################################################
**     Filename  : TML_14229_UDS.c
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 5/4/2016, 4:58 PM
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


/* MODULE TML_14229_UDS */


#include "TML_14229_UDSH.h"
#include "J1939.h" 
#include "Speedo.h"
#include "Tacho.h"
#include "addressClaim.h"
#include "Can.h"
#include "dataflash.h"
#include "TellTales.h"
#include "ServiceReminder.h"
#include "OdoMeter.h"
#include "HourMeter.h" 
#include "r_cg_userdefine.h"
#include "DtcCycle.h"




// Constant declarations -----------------------------------------------------

#define getRandomNumber (u8)(rand() % 255)

#define CONTROL_WITH_ECU					0
#define CONTROL_WITH_TE 					3


const u8 dtcSupportedGroup[eNumofGroups][3] = {
													{0x80,0x0,0xff},//body group
													{0xc0,0x0,0xff},//network group
													{0xff,0xff,0xff}//all groups
												};

//IDs are defined and are arranged in the order of enum SUPPORTED_DTC and defined in the other files
//Use the enum SUPPORTED_DTC to point to the relevant id in this array
const u8 dtcSupportedId[eNumofSupportedDTC+3][3]={
													{0xa1,0x82,0x11},//FrontAirPresShortGnd
													{0xa1,0x83,0x11},//RearAirPresShortGnd
													{0xa1,0x30,0x11},//FuelLevelShortGnd
													{0x21,0x84,0x11},//OilPressureShortGnd		
													{0xc0,0x01,0x88},//J1939BusOff													
													{0,	0,	0}//eNumofSupportedDTC th: invalid id
													};

//DTC
 u8 firstTestFailedDTC = eNumofSupportedDTC;
 u8 firstConfirmedDTC = eNumofSupportedDTC;
 u8 mostRecentTestFailed = eNumofSupportedDTC;
 u8 mostRecentConfirmedDTC = eNumofSupportedDTC;
 u8 errorFlags[NON_ADC_DTC] = {0,0,0,0,0};													



#define SUCCESSFUL							0x01
#define UNSUCCESSFUL						0x02
#define REASON1_FOR_FAILING					0x04
#define REASON2_FOR_FAILING					0x08

#define START_ROUTINE						0x01U
#define STOP_ROUTINE						0x02U
#define REQUEST_ROUTINE_RESULT				0x03U
#define ERASE_MEMORY						0xFF00U

#define CRC_VERIFICATION					0xFF03U
#define SERVICE_RESET						0x3F00U

#define ROUTINE_CONTROL_TYPE				0x01
#define ROUTINE_IDENTIFIER_BYTE1			0x3F
#define ROUTINE_IDENTIFIER_BYTE2			0x00

#define FIXED_BAUD_RATE						0x01
#define SPECIFIC_BAUD_RATE 					0x02
#define CAN_125K_BAUD_RATE					0x10
#define CAN_250K_BAUD_RATE					0x11
#define CAN_500K_BAUD_RATE					0x12
#define CAN_1M_BAUD_RATE					0x13

#define LID_GEN_BYTE2 						0x0A
#define LID_GEN_BYTE1 						0x01
#define LID_VEH_OPT							0x02
#define LID_FUEL_FILTER_CAL 				0x20
#define LID_BIAS_SPD_MUL_FACTOR				0x24
#define LID_AFE_CALC_UPDATE					0x46
#define LID_VEH_SPD_PPR						0x2C
#define LID_ENG_SPD							0x2D
#define LID_FUEL_TANK_CAP					0x34
#define LID_COOLANT_TEMP_THRES				0x35
#define LID_IFE_CALC_UPDATE					0x25
#define LID_FE_THRESH						0x39
#define LID_DTE_CALC_UPDATE					0x47
#define LID_LOW_AIR_PRES					0x41
#define	LID_ENGINE_RPM						0x43
#define LID_FUEL_CONSUMPTION_CONV_FACTOR	0x44
#define LID_VEH_SPD_CONV_FACTOR				0x45
#define LID_AWAKE_MODE						0x05
#define LID_TELL_TALE						0x03
#define DISP_LCD_SCREEN						0x06
#define LID_ODOMETER						0x40
#define LID_HOURMETER						0x43
#define LID_BUZZER_CONTROL					0x49
#define LID_FIRST_SERVICE_DUE				0x51
#define LID_SERVICE_INTERVAL				0x52
#define LID_SERVICE_THRESHOLD				0x53
#define LID_SVN_NO							0x41
#define LID_PCB_NO							0x42


#define SERVICE_MAX_RANGE					0xFFFFFF
#define RPM_EOL_MAX_RANGE					160	//160*25 = 4k RPM 
#define AFE_MIN_DISTANCE_TRAVEL				255 	 
#define AFE_MUL_FACTOR_MIN					50		 
#define AFE_MUL_FACTOR_MAX					150
#define SPEEDO_PULSES_MIN					4000
#define SPEEDO_PULSES_MAX					40000
#define SPEEDO_OVER_SPEED					140
#define SPEEDO_BIAS_FACTOR_MIN				128
#define SPEEDO_BIAS_FACTOR_MAX				154
#define FE_DATA_RANGE 						160
#define DTE_MUL_FACTOR_MIN 					50
#define DTE_MUL_FACTOR_MAX 					150
#define TEMP_START_RED_ZONE_MIN 			90
#define TEMP_START_RED_ZONE_MAX				120





#define LID_ANALOG_GAUGES					0x01

//---------------------------------------------//
//---------------------------------------------//

#define LID_ECU_ID_BYTE1					0xF1
#define LID_ECU_SERIAL_NUM					0x83 //0x83
#define LID_ECU_SUPPLIER_PART_NUM 			0x91 //0x91
#define LID_ECU_HW_NUM						0x92 //0x92
#define LID_CONTAINER_PART_NUM				0x87
#define LID_ECU_SW_NUM						0x94
#define LID_VEHICLE_NUM						0x90 
#define LID_VARIANT_DATASET_NUM 			0x97
#define LID_PARAMETER_PART_NUMBER			0xA1
#define LID_PROGRAM_SHOP_CODE				0xA2
#define LID_ECUHW_VER_NUM 					0x93
#define LID_ECUSW_VER_NUM 					0x95
#define LID_DATE_OF_LAST_PROGRAM 			0x99
#define LID_VEH_CONFIG_NUM					0xA0
#define LID_REPROGRAMMING_COUNTER			0x98
#define LID_CUSTOMER_PART_NUM				0x9E
#define LID_OIL_PRESSURE_TALE_TELL			0xE2


//EOL Read and Write parameters length
//only R
#define EXTRA_READ_BYTES			 		3
#define EXTRA_READ_MUL_BYTES		 		2

//RW
#define EXTRA_WRITE_BYTES			 		3
#define IO_CONTROL_GAUGES_BYTE		 		(9U + 1U) // 1 extra byte for control option 


//*********************************CONST ECU PARAMETERS*************************************************//

//Ultra 24V***********************************************************************************************************
#if CLUSTER_ULTRA_24V

	const u8 u8EcuPartNumber[ECU_PART_NUMBER_BYTES] = {'0','0','0','0','0','0','5','2','3','2','6','7'};
	const u8 u8TmlEcuHardwareNumber[TML_ECU_HARDWARE_NUMBER_BYTES] = {'5','5','3','0','5','4','2','0','0','1','0','7','_','N','R'};
	const u8 u8TmlContainerPartNumber[TML_CONTAINER_PART_NUMBER_BYTES] = {'5','5','3','0','1','6','2','6','1','2','0','1'};
	const u8 u8TmlEcuSoftwareNumber[TML_ECU_SOFTWARE_NUMBER_BYTES] = {'5','5','3','0','1','6','3','0','1','1','0','1','_','N','R'};
	const u8 u8IILPcbNo[IIL_PCB_NUMBER_BYTES] = {'3','4','0','6','5','R','0','2'}; // released software PCB no. (used for internal purpose only)
	const u16  u16ECUHardwareVersionNum = 0x8511; //IIL Harware no

	#endif 

//Ultra 12V***********************************************************************************************************
#if CLUSTER_ULTRA_12V

	const u8 u8EcuPartNumber[ECU_PART_NUMBER_BYTES] = {'0','0','0','0','0','0','5','2','3','2','6','8'};
	const u8 u8TmlEcuHardwareNumber[TML_ECU_HARDWARE_NUMBER_BYTES] = {'5','5','3','0','5','4','2','0','0','1','0','8','_','N','R'};
	const u8 u8TmlContainerPartNumber[TML_CONTAINER_PART_NUMBER_BYTES] = {'5','5','3','0','1','6','2','6','1','2','0','2'};
	const u8 u8TmlEcuSoftwareNumber[TML_ECU_SOFTWARE_NUMBER_BYTES] = {'5','5','3','0','1','6','3','0','1','1','0','2','_','N','R'};
	const u8 u8IILPcbNo[IIL_PCB_NUMBER_BYTES] = {'3','4','0','6','8','R','0','1'}; // released software PCB no. (used for internal purpose only)
	const u16  u16ECUHardwareVersionNum = 0x8514; //IIL Harware no

#endif 

//Eargo 24V***********************************************************************************************************
#if CLUSTER_ERGO_24V

	const u8 u8EcuPartNumber[ECU_PART_NUMBER_BYTES] = {'0','0','0','0','0','0','5','2','3','2','6','5'};
	const u8 u8TmlEcuHardwareNumber[TML_ECU_HARDWARE_NUMBER_BYTES] = {'2','7','5','4','5','4','2','0','0','1','2','8','_','N','R'};
	const u8 u8TmlContainerPartNumber[TML_CONTAINER_PART_NUMBER_BYTES] = {'2','7','5','4','1','6','2','6','1','2','0','1'};
	const u8 u8TmlEcuSoftwareNumber[TML_ECU_SOFTWARE_NUMBER_BYTES] = {'2','7','5','4','1','6','3','0','1','1','0','2','_','N','R'};
	const u8 u8IILPcbNo[IIL_PCB_NUMBER_BYTES] = {'3','4','7','6','5','R','0','3'}; // released software PCB no. (used for internal purpose only)
	const u16  u16ECUHardwareVersionNum = 0x87CD; //IIL Harware no

#endif

//Eargo 12V***********************************************************************************************************	
#if CLUSTER_ERGO_12V

	const u8 u8EcuPartNumber[ECU_PART_NUMBER_BYTES] = {'0','0','0','0','0','0','5','2','3','2','6','6'};
	const u8 u8TmlEcuHardwareNumber[TML_ECU_HARDWARE_NUMBER_BYTES] = {'2','7','5','4','5','4','2','0','0','1','2','9','_','N','R'};
	const u8 u8TmlContainerPartNumber[TML_CONTAINER_PART_NUMBER_BYTES] = {'2','7','5','4','1','6','2','6','1','2','0','2'};
	const u8 u8TmlEcuSoftwareNumber[TML_ECU_SOFTWARE_NUMBER_BYTES] = {'2','7','5','4','1','6','3','0','1','1','0','3','_','N','R'};
	const u8 u8IILPcbNo[IIL_PCB_NUMBER_BYTES] = {'3','4','7','6','8','R','0','1'}; // released software PCB no. (used for internal purpose only)
	const u16  u16ECUHardwareVersionNum = 0x87D0; //IIL Harware no

#endif
//******************************************************************************************************************

const u8 u8EcuSerialNumber[ECU_SERIAL_NUMBER_BYTES] = {'0','0','0','0','0','0','0','0','0','0','0','0'};
const u16  u16ReprogrammingCounter = 0x0000;

//Change when update software version no.
const u16  u16ECUSoftwareVersionNum = 0x0500; //V2.01  
// released software SVN no. (used for internal purpose only)
const u8 u8IILSvnNo[IIL_SVN_NUMBER_BYTES] = {'0','7','2','7'}; 





typedef struct
{

u8 serverLockStatus : 2 ; //SERVER_LOCK_TYPE
u8 securityDelayOn:1 ;
u8 securityAccessAttempts :3;
u8 securitySequence; // this is mapped to a byte in the message //SECURITY_ACCESS_TYPE
u32 securityAccessDelay; //this delay is to handle more than 3 failed attemps

}SECURITY_STRUCT;


   
//static const byte TMLAESCipherKey[17] = "TATA00Y1DEIC1111";
//global variables 

bool criticalFunctionStatus = FALSE; 
bool conditionNotCorrect = TRUE;
u8 odoProgrammingCtr = 0;

//local variables 
static bool bCheckSumVarificationDone = 0;
static u16 RoutineControlStatusPending = 0;
static u8 dataTxBuffer[MAX_LENGTH + 20];//it should actually not contain data more than MAX_DATA_LENGTH
static sJ1939_RX_MSG_TYPE diagnosticsResponse;

static u16 resetTimeTracker;
static u16 setTimeForReset;
static u8 activeDiagnosticsSession = eDefaultSession;
static bool bECUResetServiceActive = 0;
static u8 encryptedReceivedData[17];

//static const u8 TMLAESCipherKey[17] = "T0D1A0E1TYI1A1C1";  //Y1 TATA Origenal Key
#if (CLUSTER_ERGO_24V == 1 || CLUSTER_ERGO_12V == 1)
	static const u8 TMLAESCipherKey[17] = "TCIIMVLP1BPC9URS";  //BSVI EARGO
#else 
	static const u8 TMLAESCipherKey[17] = "TCIIMVIP1BLC9UUS";  //BSVI ULTRA
#endif




static SECURITY_STRUCT securityAccessInfo = {eServerLocked,FALSE,0,eNone,0};


/// @addtogroup TML_UDS_LIBRARY
/// @{
/// @defgroup TmlUdsLib  functions used 
//// 
///   
/// @addtogroup TmlUdsLib
/// @{

//local functions
static void getEncryptedData(void);
//data to be transmitted is converted into frames
static void formatMulBytesIntoFrames(void);
//data is extracted from received frames
static void formatFrameIntoMulBytes(void);

/// @} // endgroup TmlUdsLib

/// @} // endgroup TML_UDS_LIBRARY


//global variables

TEST_EQUIPMENT_CONTROL sTestEquimentControl = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


// all communication is true by default
COMM_STRUCT_TYPE commInfo = {TRUE,TRUE,TRUE,TRUE};


//global functions
void resetDiagnosticSession(void);
void setStatusDefaultSession(void);
bool isItTimeToReset(void);

/*********************************************************************//**
 *
 * Start diagnostic session 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void startDiagnosticsSession(void) 
{

	u8 i;
	bool receivedMessageError = FALSE;

	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if(singleFrameRx.numBytes != 2 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
		
	switch(singleFrameRx.dataBytes[2])
	{

		case eProgrammingSession: // this case should be on top because its negative response code has higher priority
			
				if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
				{
					activeDiagnosticsSession = eProgrammingSession;					
				}
		break;

		case eDefaultSession: 
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
			{
			
				activeDiagnosticsSession = eDefaultSession;
				
				//these should ideally happen after this response is sent

				//reset timers
				diagnosticsTimer = u32GetClockTicks(); //UDSData.timeStamp; 
				//reset diagnostics message receive buffer
				for(i=0; i<RX_QUE_SIZE ; i++)
					diagnosticsBuf[i].bDataRdy = FALSE;
				//lock if unlocked
//TBD
				securityAccessInfo.serverLockStatus = eServerLocked;
				//devControlSpeedometerExit();
				//devControlTachometerExit();
				//testEquimentControlFlags.tellTales = FALSE;
          
			}
			break;

		case eExtendedSession: 
			if(receivedMessageError == FALSE) //fall out of eDefaultSession &  eExtendedSession if there is error 
			{
				activeDiagnosticsSession = eExtendedSession;
					
			}
			break;
			
		default:
				diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
				receivedMessageError = TRUE;
			
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = StartDiagnosticSession;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 6;
		diagnosticsResponse.u8data[1] = 0x50;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = P1ServerMaxMSB;//P1servermax MSB
		diagnosticsResponse.u8data[4] = P1ServerMaxLSB;//P1servermax LSB ; 100ms
		diagnosticsResponse.u8data[5] = P2ServerMaxMSB;//P2servermax MSB
		diagnosticsResponse.u8data[6] = P2ServerMaxLSB;//P2servermax LSB ; 5000ms

	}
	diagnosticsResponse.bDataRdy = TRUE ;
	
}

/*********************************************************************//**
 *
 * ECU reset service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ECUResetService(void)
{
	
	bool receivedMessageError = FALSE;
	u16 oldRPM = 0;
	u16 speed = 0;
	
	//don't entertain if no session is active
	/*if( activeDiagnosticsSession != eDefaultSession  && activeDiagnosticsSession != eExtendedSession )
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE ) 
		return;
		
	 oldRPM = u16GetEngineSpeed();
	 speed = u16GetVehicleSpeed();
		
	// check this error conditions with the customer
   if(singleFrameRx.numBytes != 2)
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	//check engine speed and frequency speed as per diagnostics specs remove comments	 
	else if(conditionNotCorrect == TRUE || oldRPM != 0 || speed != 0 )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	switch(singleFrameRx.dataBytes[2])
	{
		case eSoftReset: 
			if( receivedMessageError == FALSE )
			{
				diagnosticsResponse.u8data[0] = 3;
				diagnosticsResponse.u8data[1] = 0x51;
				diagnosticsResponse.u8data[2] = eSoftReset;
				//reset ECU reset time tracker so that in setTimeForReset, ECU will reset
				resetTimeTracker = u16GetClockTicks();
				//as per 14229-1
				diagnosticsResponse.u8data[3] = 1;//extra second for watchdog to reset
				bECUResetServiceActive = TRUE;
				//activeDiagnosticsSession = eDefaultSession; //this happens in cpu_reset
				
				//watchdog reset will happen in main routine
				
			}
			break;

		case eHardReset :
		case eKeyOffOnReset:
		case eEnableRapidPowerShutdown:
		case eDisableRapidPowerShutdown:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
			break;

		default:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ECUReset;
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

/*********************************************************************//**
 *
 * security Access service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void securityAccessService(void)
{
	u8 i;
	static u8 seed[17];
	bool receivedMessageError = FALSE;
	
	//don't entertain if extended session is not active & //10s delay as per TML_UDS

	/*if( ( securityAccessInfo.securitySequence == eNone && singleFrameRx.dataBytes[2] > eRequestSeed )
		||( securityAccessInfo.securitySequence == eRequestSeed && multipleFramesRx.mulByteDiagRespData[3] > eSendKey )
		)
	{
		diagnosticsResponse.data[3] = ServiceNotSupported;
		receivedMessageError = TRUE;
	}
	else*/ 
	
	if( (activeDiagnosticsSession != eExtendedSession) 
		&& (activeDiagnosticsSession != eProgrammingSession) )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.securityDelayOn == TRUE 
		&& (u32GetClockTicks() - securityAccessInfo.securityAccessDelay) < 10000 )
	{
		diagnosticsResponse.u8data[3] = RequiredTimeDelayNotExpired;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.securityAccessAttempts >= 3 ) //TML_UDS
	{
		securityAccessInfo.securityAccessDelay = u32GetClockTicks();
		securityAccessInfo.securityAccessAttempts = 0;
		securityAccessInfo.securityDelayOn = TRUE;
		diagnosticsResponse.u8data[3] = ExceededNumberofAttempts;
		receivedMessageError = TRUE;
	}
	else if(securityAccessInfo.serverLockStatus == eServerLocked)
	{
		if( singleFrameRx.dataBytes[2] == eRequestSeed ) //if the number is odd, then its requesting seed
		{
			securityAccessInfo.securityDelayOn = FALSE;
			//securityAccessInfo.securityAccessAttempts = 0;

			securityAccessInfo.securitySequence = eRequestSeed;
			multipleFramesTx.numBytes= 18;
			//multipleFramesTx.timeStamp = UDSData.timeStamp;//assign the original ID
			//multipleFramesTx.numFrames = 3;
			//multipleFramesTx.waitForResponse = TRUE;

			dataTxBuffer[0] = 0x67;
			dataTxBuffer[1] = eRequestSeed;
			// Seed the random time generator
			if(u16GetClockTicks() == 0)
				srand(352487);
			else
    			srand(u16GetClockTicks());
			
			for(i = 2; i < 18; i++)
			{
				dataTxBuffer[i] = getRandomNumber;
				seed[i-2] = dataTxBuffer[i];
			}
		
			seed[16] = 0;
			for(i= 18; i<42 ; i++)
				dataTxBuffer[i]  = 0;
			
			singleFrameRx.dataBytes[2] = eNone; // so that it doesn't come in this loop next time when key is sent
			formatMulBytesIntoFrames();
			
		}
		else if( multipleFramesRx.mulByteDiagRespData[3] == eSendKey
			&& securityAccessInfo.securitySequence != eNone)//this condition may not be required
		{
			//extract the decrypted data
			getEncryptedData();
			
			data_decrypt(encryptedReceivedData,TMLAESCipherKey);
			if((memcmp(encryptedReceivedData,&seed,16)== ERR_OK) )
			{
				securityAccessInfo.serverLockStatus = eServerUnlocked;
				diagnosticsResponse.u8data[0] = 2;
				diagnosticsResponse.u8data[1] = 0x67;
				diagnosticsResponse.u8data[2] = eSendKey;
				diagnosticsResponse.bDataRdy = TRUE;
			}
			else
			{
				diagnosticsResponse.u8data[3] = InvalidKey;
				receivedMessageError = TRUE;
			}
				
		}
	}
	else if (securityAccessInfo.serverLockStatus == eServerUnlocked )
	{
		if(singleFrameRx.dataBytes[2] == eRequestSeed ) //if the number is odd, then its requesting seed
		{
	
			securityAccessInfo.securitySequence = eRequestSeed;
			multipleFramesTx.numBytes= 18;
			//multipleFramesTx.numFrames = 3;
			dataTxBuffer[0] = 0x67;
			dataTxBuffer[1] = eRequestSeed;

			for(i = 2; i < 18; i++)
			{
				dataTxBuffer[i] = 0;
				seed[i-2] = 0;
			}
			formatMulBytesIntoFrames();
			securityAccessInfo.securityAccessAttempts = 0;
		}
 	}

	if (receivedMessageError == FALSE)
	{
		//does this block need to handle sub function sendkey ?
		if(  singleFrameRx.dataBytes[2] > eSendKey  )
		{
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
		}
		
			
		else if((singleFrameRx.dataBytes[1]== SecurityAccess && singleFrameRx.numBytes != 2) 
			||
			(multipleFramesRx.mulByteDiagRespData[2]== SecurityAccess && multipleFramesRx.numBytes != 18) )
	 	{
			diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
			receivedMessageError = TRUE;
		}
		//send key comes before request seed
		else if ( securityAccessInfo.securitySequence == eNone && ( multipleFramesRx.mulByteDiagRespData[3] == eSendKey ||
			singleFrameRx.dataBytes[2] == eSendKey) )
		{
			diagnosticsResponse.u8data[3] = RequestSequenceError;
			receivedMessageError = TRUE;
		}
		else if(conditionNotCorrect == TRUE)
		{
			diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
			receivedMessageError = TRUE;
		}
	}
	if(receivedMessageError == TRUE )
	{
		for(i=4;i<8;i++)
			diagnosticsResponse.u8data[i] = 0;	
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = SecurityAccess;
		securityAccessInfo.securitySequence = eNone;
		//count attempts only if the failed-attempt -delay-timer has not started
		if(diagnosticsResponse.u8data[3] != RequiredTimeDelayNotExpired)
			securityAccessInfo.securityAccessAttempts++;
		diagnosticsResponse.bDataRdy = TRUE;
		//in case this was set before the low priority errors were detected
		multipleFramesTx.dataReady = FALSE;
	}
	

}

/*********************************************************************//**
 *
 * Communication Control Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void communicationControlService(void)
{
	bool receivedMessageError = FALSE;

	if( activeDiagnosticsSession != eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	else if(singleFrameRx.numBytes != 3 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}

	switch(singleFrameRx.dataBytes[2]) //control type
	{
		case eEnableRxTx: 
			if( receivedMessageError == FALSE )
			{
				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
				{
					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
					{
						commInfo.normalCommTxEnabled = TRUE;
						commInfo.normalCommRxEnabled = TRUE;
					}
					
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
					{
						commInfo.networkCommTxEnabled = TRUE;
						commInfo.networkCommRxEnabled = TRUE;
					}
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
					{
						commInfo.networkCommTxEnabled = TRUE;
						commInfo.networkCommRxEnabled = TRUE;
						commInfo.normalCommTxEnabled = TRUE;
						commInfo.normalCommRxEnabled = TRUE;
					}
					else //subnets not supported in this version
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
				}
				//else handle subnets
			}
			break;
		case eEnableRxDisableTx: 
			if( receivedMessageError == FALSE )
			{
				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
				{
					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
					{
						commInfo.normalCommTxEnabled = FALSE;
						commInfo.noResponse = FALSE;//respond to this message
						commInfo.normalCommRxEnabled = TRUE;
					}
					
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
					{
						commInfo.networkCommTxEnabled = FALSE;
						commInfo.noResponse = FALSE;//respond to this message
						commInfo.networkCommRxEnabled = TRUE;
					}
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
					{
						commInfo.networkCommTxEnabled = FALSE;
						commInfo.networkCommRxEnabled = TRUE;
						commInfo.normalCommTxEnabled = FALSE;
						commInfo.normalCommRxEnabled = TRUE;
						commInfo.noResponse = FALSE;//respond to this message
					}
					else //subnets not supported in this version
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
				}
				//else handle subnets
			}
			break;
		case eDisableRxEnableTx: 
			if( receivedMessageError == FALSE )
			{
				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
				{
					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
					{
						commInfo.normalCommTxEnabled = TRUE;
						commInfo.normalCommRxEnabled = FALSE;
					}
					
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
					{
						commInfo.networkCommTxEnabled = TRUE;
						commInfo.networkCommRxEnabled = FALSE;
					}
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
					{
						commInfo.networkCommTxEnabled = TRUE;
						commInfo.networkCommRxEnabled = FALSE;
						commInfo.normalCommTxEnabled = TRUE;
						commInfo.normalCommRxEnabled = FALSE;
					}
					else //subnets not supported in this version
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
				}
				//else handle subnets
			}
			break;
		case eDisableRxandTx: 
			if( receivedMessageError == FALSE )
			{
				if(((singleFrameRx.dataBytes[3] & 0xf0) >> 4) == 0)//communication type upper nibble
				{
					if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalComm )//comunication type lower nibble
					{
						commInfo.normalCommTxEnabled = FALSE;
						commInfo.noResponse = FALSE;//respond to this message
						commInfo.normalCommRxEnabled = FALSE;
					}
					
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNMComm)
					{
						commInfo.networkCommTxEnabled = FALSE;
						commInfo.noResponse = FALSE;//respond to this message
						commInfo.networkCommRxEnabled = FALSE;
					}
					else if((singleFrameRx.dataBytes[3] & 0x0f)== eNormalCommNMComm )
					{
						commInfo.networkCommTxEnabled = FALSE;
						commInfo.networkCommRxEnabled = FALSE;
						commInfo.noResponse = FALSE;//respond to this message
						commInfo.normalCommTxEnabled = FALSE;
						commInfo.normalCommRxEnabled = FALSE;
					}
					else //subnets not supported in this version
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
				}
				//else handle subnets
			}
			break;
		default:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;

	}
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}	
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = CommunicationControl;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0x68;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

/*********************************************************************//**
 *
 * Tester Present Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void TesterPresentService(void)
{
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	bool receivedMessageError = FALSE;

	if(singleFrameRx.numBytes != 2 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	
	if(singleFrameRx.dataBytes[2] == 0 || singleFrameRx.dataBytes[2] == 0x80 )
	{
		if(receivedMessageError == FALSE )
		{
			diagnosticsResponse.u8data[0] = 2;
			diagnosticsResponse.u8data[1] = 0x7e;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2] ;
			//reset the timer since the tester is present so that the current diagnostic session does not expire
			diagnosticsTimer = u32GetClockTicks();
		}
	}
	else
	{	diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
		receivedMessageError = TRUE;
	}	

	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = TesterPresent;
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}

void ControlDTCSettingServiceSingle(void)
{
	bool receivedMessageError = FALSE;
	u8 i;
	
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if( activeDiagnosticsSession != eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.numBytes != 5 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}

	switch(singleFrameRx.dataBytes[2] ) //control type
	{
		case eOn: 
			if( receivedMessageError == FALSE )
			{
				//if DTC was specified
				for(i=0;i<eNumofSupportedDTC;i++)
					if( singleFrameRx.dataBytes[3] == dtcSupportedId[i][0] 
						&&  singleFrameRx.dataBytes[4] == dtcSupportedId[i][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedId[i][2]
					)
					{
						dtcLogged[i].isEnabled = TRUE;
						dtcLogged[i].hasChanged = TRUE;
						break;
					}
				//no match was found, is the body/network group specified ?	
				if(i == eNumofSupportedDTC )
				{
					if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eBody][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eBody][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eBody][2]
					)
						{
							for(i=0;i<BODY_DTC;i++)
							{
								dtcLogged[i].isEnabled = TRUE;	
								dtcLogged[i].hasChanged = TRUE;
							}
						}
					else if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eNetwork][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eNetwork][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eNetwork][2]
					)
						{
							for(i=BODY_DTC;i<(BODY_DTC + NETWORK_DTC);i++)
							{
								dtcLogged[i].isEnabled = TRUE;
								dtcLogged[i].hasChanged = TRUE;
							}
						}
					else if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eAll][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eAll][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eAll][2]
					)

						{
							for(i=0;i<eNumofSupportedDTC;i++)
							{
								dtcLogged[i].isEnabled = TRUE;
								dtcLogged[i].hasChanged = TRUE;
							}

						}
					else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
				}
					
			}
			break;
		case eOff: 
			if( receivedMessageError == FALSE )
			{
				for(i=0;i<eNumofSupportedDTC;i++)
					if( singleFrameRx.dataBytes[3] == dtcSupportedId[i][0] 
						&&  singleFrameRx.dataBytes[4] == dtcSupportedId[i][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedId[i][2]
					)
					{
						dtcLogged[i].isEnabled = FALSE;
						dtcLogged[i].hasChanged = TRUE;
						break;
					}
				//no match was found 	
				if(i == eNumofSupportedDTC )
				{
					if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eBody][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eBody][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eBody][2]
					)
						{
							for(i=0;i<BODY_DTC;i++)
							{
								dtcLogged[i].isEnabled = FALSE;	
								dtcLogged[i].hasChanged = TRUE;
							}
						}
					else if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eNetwork][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eNetwork][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eNetwork][2]
					)
						{
							for(i=BODY_DTC;i<(BODY_DTC + NETWORK_DTC);i++)
							{
								dtcLogged[i].isEnabled = FALSE;
								dtcLogged[i].hasChanged = TRUE;
							}
						}
					else if( singleFrameRx.dataBytes[3] == dtcSupportedGroup[eAll][0]
						&&  singleFrameRx.dataBytes[4] == dtcSupportedGroup[eAll][1]
						&& singleFrameRx.dataBytes[5] == dtcSupportedGroup[eAll][2]
					)

						{
							for(i=0;i<eNumofSupportedDTC;i++)
							{
								dtcLogged[i].isEnabled = FALSE;
								dtcLogged[i].hasChanged = TRUE;
							}

						}
					else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
				}
			}
			break;
		default:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
			
	}

	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ControlDTCSetting;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC5;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];		
		vSetInternalDtcSaveFlag(TRUE);
	}

	diagnosticsResponse.bDataRdy = TRUE;
	
			
}

void ControlDTCSettingServiceMultiple(void)
{
	
	bool receivedMessageError = FALSE;
	u8 i=0,j=0,numOfDTCs;
	
	/*if(securityAccessInfo.serverLockStatus == eServerLocked)
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

		
	//Service Id bytes  
	numOfDTCs = ( multipleFramesRx.numBytes - 2 )/3;

	if( activeDiagnosticsSession != eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if( numOfDTCs > eNumofSupportedDTC )
	{
		diagnosticsResponse.u8data[3] = RequestOutOfRange;
		receivedMessageError = TRUE;
	}
	//error = numOfDTCs;//this error will decrement on every DTC match; it should be zero in the end so that negative response is not sent
	switch( multipleFramesRx.mulByteDiagRespData[3] ) //control type
	{
		case eOn: 
			if( receivedMessageError == FALSE )
			{
					
				//for all the DTCs present in the received data
				for(i=4; i < (multipleFramesRx.numBytes+2); i+=3)
					//find the supported DTC in the support DTC list
					for(j=0;j<eNumofSupportedDTC; j++)	
						if( multipleFramesRx.mulByteDiagRespData[i] == dtcSupportedId[j][0] 
							&&  multipleFramesRx.mulByteDiagRespData[i+1] == dtcSupportedId[j][1]
							&& multipleFramesRx.mulByteDiagRespData[i+2] == dtcSupportedId[j][2]
						)
						{
							dtcLogged[j].isEnabled = TRUE;
							dtcLogged[j].hasChanged = TRUE;
							//DTC matched
							numOfDTCs--;
							break;
						}
						//any of the DTCs did not match
						if(!numOfDTCs) 
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
					
				}
			
			break;
		case eOff: 
			if( receivedMessageError == FALSE )
			{
					
				//for all the DTCs present in the received data
				for(i=4; i < (multipleFramesRx.numBytes+2); i+=3)
					//find the supported DTC in the support DTC list
					for(j=0;j<eNumofSupportedDTC; j++)	
						if( multipleFramesRx.mulByteDiagRespData[i] == dtcSupportedId[j][0] 
							&&  multipleFramesRx.mulByteDiagRespData[i+1] == dtcSupportedId[j][1]
							&& multipleFramesRx.mulByteDiagRespData[i+2] == dtcSupportedId[j][2]
						)
						{
							dtcLogged[j].isEnabled = FALSE;
							dtcLogged[j].hasChanged = TRUE;
							//DTC matched
							numOfDTCs--;
							break;
						}
						//any of the DTCs did not match
						if(!numOfDTCs) 
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
					
				}
			break;
			
		default:
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
						
	}
	
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ControlDTCSetting;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC5;
		diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3];	
		vSetInternalDtcSaveFlag(TRUE);
	}
	diagnosticsResponse.bDataRdy = TRUE ;
			
}

void ClearDiagnosticsInformationService(void)
{

	bool receivedMessageError = FALSE;
	u8 i;

	/*if(securityAccessInfo.serverLockStatus == eServerLocked)
		return;*/
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
			return;
	
	if( activeDiagnosticsSession != eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}			
	else if(singleFrameRx.numBytes != 4 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else
	{
		//if DTC was specified
		for(i=0;i<eNumofSupportedDTC;i++)
			if( singleFrameRx.dataBytes[2] == dtcSupportedId[i][0] 
				&&  singleFrameRx.dataBytes[3] == dtcSupportedId[i][1]
				&& singleFrameRx.dataBytes[4] == dtcSupportedId[i][2]
			)
			{
				dtcLogged[i].statusByte.status = 0;
				dtcLogged[i].hasChanged = TRUE;
				dtcLogged[i].isLogged = FALSE;
				dtcLogged[i].nofaultCount = 0;
				break;
			}
		//no match was found, is the body/network bgroup specified ?	
		if(i == eNumofSupportedDTC )
		{
			if( singleFrameRx.dataBytes[2] == dtcSupportedGroup[eBody][0]
				&&  singleFrameRx.dataBytes[3] == dtcSupportedGroup[eBody][1]
				&& singleFrameRx.dataBytes[4] == dtcSupportedGroup[eBody][2]
				)
				{
					for(i=0;i<BODY_DTC;i++)
					{
						dtcLogged[i].statusByte.status = 0;
						dtcLogged[i].hasChanged = TRUE;
						dtcLogged[i].isLogged = FALSE;
						dtcLogged[i].nofaultCount = 0;
					}
				}
			else if( singleFrameRx.dataBytes[2] == dtcSupportedGroup[eNetwork][0]
				&&  singleFrameRx.dataBytes[3] == dtcSupportedGroup[eNetwork][1]
				&& singleFrameRx.dataBytes[4] == dtcSupportedGroup[eNetwork][2]
				)
				{
					for(i=BODY_DTC;i<(BODY_DTC + NETWORK_DTC);i++)
					{
						dtcLogged[i].statusByte.status = 0;
						dtcLogged[i].hasChanged = TRUE;
						dtcLogged[i].isLogged = FALSE;
						dtcLogged[i].nofaultCount = 0;
					}
				}
			else if( singleFrameRx.dataBytes[2] == dtcSupportedGroup[eAll][0]
				&&  singleFrameRx.dataBytes[3] == dtcSupportedGroup[eAll][1]
				&& singleFrameRx.dataBytes[4] == dtcSupportedGroup[eAll][2]
				)
				{
					for(i=0;i<eNumofSupportedDTC;i++)
					{
						dtcLogged[i].statusByte.status = 0;
						dtcLogged[i].hasChanged = TRUE;
						dtcLogged[i].isLogged = FALSE;
						dtcLogged[i].nofaultCount = 0;
					}
				}
			else
				{
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				}
		}
			
	}
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}

	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ClearDiagnosticInformation;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 1;
		diagnosticsResponse.u8data[1] = 0x54;		
		vSetInternalDtcSaveFlag(TRUE);
		//clear them all regardless of which DTCs have been cleared
		firstTestFailedDTC = eNumofSupportedDTC;
 		firstConfirmedDTC = eNumofSupportedDTC;
  		mostRecentTestFailed = eNumofSupportedDTC;
  		mostRecentConfirmedDTC = eNumofSupportedDTC;
	}
	
	diagnosticsResponse.bDataRdy = TRUE ;
}

void ReadDiagnosticTroubleInformationService(void)
{
	bool receivedMessageError = FALSE;
	bool mulByteResponse = FALSE;
	u8 i, dtcloggedFlag = FALSE;
	u16 count=0,multiByteIndex = 3;

	

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	if( activeDiagnosticsSession != eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}	
	else if( (singleFrameRx.dataBytes[2] <=  eDTCbyStatusMask  && singleFrameRx.numBytes != 3 )
		|| (singleFrameRx.dataBytes[2] > eDTCbyStatusMask && singleFrameRx.numBytes != 2) 
		)
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}

	
	switch (singleFrameRx.dataBytes[2])
	{
		case eDTCNumByMask:
			if(receivedMessageError == FALSE )
			{
				if( singleFrameRx.dataBytes[3] & DTC_STATUS_MASK_SUPPORTED )//if any of the inquired mask flags are supported, count them
				{
					for(i=0;i<eNumofSupportedDTC;i++)
					{
						if(dtcLogged[i].statusByte.status & DTC_STATUS_MASK_SUPPORTED )
							count++;
					}
					diagnosticsResponse.u8data[0] = 6;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					diagnosticsResponse.u8data[4] = 0x01;
					diagnosticsResponse.u8data[5] = (u16)count >> 8;
					diagnosticsResponse.u8data[6] = (u8)count;
				}
				else
				{
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				}
			}
			
			break;

		case eDTCbyStatusMask://DTC by Status Mask
			if(receivedMessageError == FALSE )
			{
				if( singleFrameRx.dataBytes[3] & DTC_STATUS_MASK_SUPPORTED )
				{
					for(i=0;i<eNumofSupportedDTC && multiByteIndex < MAX_DATA_LENGTH ;i++)
					{
						if(dtcLogged[i].statusByte.status & DTC_STATUS_MASK_SUPPORTED ) //confirmed DTC only
						{
							dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][0];
							dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][1];
							dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][2];
							dataTxBuffer[multiByteIndex++] = dtcLogged[i].statusByte.status;
							dtcloggedFlag = TRUE;
						}
					}
					if( dtcloggedFlag == TRUE )
					{
						if(multiByteIndex > 7)
						{
							multipleFramesTx.numBytes = multiByteIndex;
							dataTxBuffer[2] = DTC_STATUS_MASK_SUPPORTED;
							mulByteResponse = TRUE;
						}
						else
						{
							diagnosticsResponse.u8data[0] = 7;
							diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
							diagnosticsResponse.u8data[4] = dataTxBuffer[3];
							diagnosticsResponse.u8data[5] = dataTxBuffer[4];
							diagnosticsResponse.u8data[6] = dataTxBuffer[5];
							diagnosticsResponse.u8data[7] = dataTxBuffer[6];
							for(i=0;i<8;i++)
								dataTxBuffer[i] = 0;
						}
					}
					else
					{
						diagnosticsResponse.u8data[0] = 3;
						diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					}
				}
				else
				{
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;

				}
			}
			
			break;
		case eSupportedDTC:
			if(receivedMessageError == FALSE )
			{
				for(i=0;i<eNumofSupportedDTC && multiByteIndex < MAX_DATA_LENGTH ;i++)
				{
					if(dtcLogged[i].isEnabled) //confirmed DTC only
					{
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][0];
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][1];
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][2];
						dataTxBuffer[multiByteIndex++] = dtcLogged[i].statusByte.status;
						dtcloggedFlag = TRUE;
					}
				}
				if( dtcloggedFlag == TRUE )
				{
					if(multiByteIndex > 7)
					{
						multipleFramesTx.numBytes = multiByteIndex;
						dataTxBuffer[2] = DTC_STATUS_MASK_SUPPORTED;//singleFrameRx.dataBytes[3];
						mulByteResponse = TRUE;
					}
					else
					{
						diagnosticsResponse.u8data[0] = 7;
						diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
						diagnosticsResponse.u8data[4] = dataTxBuffer[3];
						diagnosticsResponse.u8data[5] = dataTxBuffer[4];
						diagnosticsResponse.u8data[6] = dataTxBuffer[5];
						diagnosticsResponse.u8data[7] = dataTxBuffer[6];
						for(i=0;i<8;i++)
							dataTxBuffer[i] = 0;
					}
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
				}

			}
			break;
		case eFirstTestFailed:
			if(receivedMessageError == FALSE )
			{
				if(firstTestFailedDTC < eNumofSupportedDTC )
				{
					diagnosticsResponse.u8data[0] = 7;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					diagnosticsResponse.u8data[4] = dtcSupportedId[firstTestFailedDTC][0];
					diagnosticsResponse.u8data[5] = dtcSupportedId[firstTestFailedDTC][1];
					diagnosticsResponse.u8data[6] = dtcSupportedId[firstTestFailedDTC][2];
					diagnosticsResponse.u8data[7] = dtcLogged[firstTestFailedDTC].statusByte.status;
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = 0xFF;
				}
			}
			break;
		case eFirstConfirmed:
			if(receivedMessageError == FALSE )
			{
				if(firstConfirmedDTC < eNumofSupportedDTC )
				{
					diagnosticsResponse.u8data[0] = 7;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					diagnosticsResponse.u8data[4] = dtcSupportedId[firstConfirmedDTC][0];
					diagnosticsResponse.u8data[5] = dtcSupportedId[firstConfirmedDTC][1];
					diagnosticsResponse.u8data[6] = dtcSupportedId[firstConfirmedDTC][2];
					diagnosticsResponse.u8data[7] = dtcLogged[firstConfirmedDTC].statusByte.status;
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = 0xFF;
				}
			}	
			break;
		case eMostRecentTestFailed:
			if(receivedMessageError == FALSE )
			{
				if(mostRecentTestFailed < eNumofSupportedDTC )
				{
					diagnosticsResponse.u8data[0] = 7;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					diagnosticsResponse.u8data[4] = dtcSupportedId[mostRecentTestFailed][0];
					diagnosticsResponse.u8data[5] = dtcSupportedId[mostRecentTestFailed][1];
					diagnosticsResponse.u8data[6] = dtcSupportedId[mostRecentTestFailed][2];
					diagnosticsResponse.u8data[7] = dtcLogged[mostRecentTestFailed].statusByte.status;
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = 0xFF;
				}
			}	
			break;
		case eMostRecentConfirmed:
			if(receivedMessageError == FALSE )
			{
				if(mostRecentConfirmedDTC < eNumofSupportedDTC )
				{
					diagnosticsResponse.u8data[0] = 7;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
					diagnosticsResponse.u8data[4] = dtcSupportedId[mostRecentConfirmedDTC][0];
					diagnosticsResponse.u8data[5] = dtcSupportedId[mostRecentConfirmedDTC][1];
					diagnosticsResponse.u8data[6] = dtcSupportedId[mostRecentConfirmedDTC][2];
					diagnosticsResponse.u8data[7] = dtcLogged[mostRecentConfirmedDTC].statusByte.status;
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = 0xFF;
				}
			}	
			break;

		case eDTCPermanentStatus://DTC by Status Mask
			if(receivedMessageError == FALSE )
			{
				
				for(i=0;i<eNumofSupportedDTC && multiByteIndex < MAX_DATA_LENGTH ;i++)
				{
					if(dtcLogged[i].statusByte.status & DTC_STATUS_MASK_PERMANENT ) //confirmed DTC only
					{
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][0];
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][1];
						dataTxBuffer[multiByteIndex++] = dtcSupportedId[i][2];
						dataTxBuffer[multiByteIndex++] = dtcLogged[i].statusByte.status;
						dtcloggedFlag = TRUE;
					}
				}
				if( dtcloggedFlag == TRUE )
				{
					if(multiByteIndex > 7)
					{
						multipleFramesTx.numBytes = multiByteIndex;
						dataTxBuffer[2] = 0xFF;
						mulByteResponse = TRUE;
					}
					else
					{
						diagnosticsResponse.u8data[0] = 7;
						diagnosticsResponse.u8data[3] = 0xFF;
						diagnosticsResponse.u8data[4] = dataTxBuffer[3];
						diagnosticsResponse.u8data[5] = dataTxBuffer[4];
						diagnosticsResponse.u8data[6] = dataTxBuffer[5];
						diagnosticsResponse.u8data[7] = dataTxBuffer[6];
						for(i=0;i<8;i++)
							dataTxBuffer[i] = 0;
					}
				}
				else
				{
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[3] = DTC_STATUS_MASK_SUPPORTED;
				}
			
			}
			break;
		default: 
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDTCInformationService;
		diagnosticsResponse.bDataRdy = TRUE;
	}
	else if (mulByteResponse == TRUE )
	{
		dataTxBuffer[0] = 0x59;
		dataTxBuffer[1] = singleFrameRx.dataBytes[2];
		//dataTxBuffer[2] = DTC_STATUS_MASK_SUPPORTED;
		formatMulBytesIntoFrames();
	}
	else
	{
		diagnosticsResponse.u8data[1] = 0x59;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.bDataRdy = TRUE;
	}

}



/*********************************************************************//**
 *
 * Routine control service used for service reminder reset
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void RoutineService()
{

	bool receivedMessageError = FALSE;
	u8 status = 0;	
	eSERVICE_REMINDER_TYPE ServiceStatus;
	u16 RoutineIdentifier =0;	
	static u16 Servicepending = 0;
	u16 speed = 0;	
	static u32 ClkTimer = 0;
	u8 temp = 0;
	u8 temp1 = 0;
	static u8 PendingResponseCount = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
	{
		commInfo.noResponse = TRUE;
	}
	
	if (commInfo.normalCommRxEnabled == FALSE )
	{
		return;
	}
	
	if( (RoutineControlStatusPending == CHECK_PROG_DEP ) || (RoutineControlStatusPending == ERASE_MEMORY) )
	{
		
			temp = bCheckDepenDencies();
			temp1 = bFlashEarseStatus();
			if( (temp == TRUE ) && (RoutineControlStatusPending == CHECK_PROG_DEP) )
		 	{
			 	diagnosticsResponse.u8data[0] = 4;
				diagnosticsResponse.u8data[1] = 0x71;
				diagnosticsResponse.u8data[2] = START_ROUTINE;
				diagnosticsResponse.u8data[3] = 0xFF;
				diagnosticsResponse.u8data[4] = 0x01;
				diagnosticsResponse.bDataRdy = TRUE;
				RoutineControlStatusPending = 0;
		 	}			
			else if((temp1 == TRUE) && (RoutineControlStatusPending == ERASE_MEMORY) )
		 	{
			 	diagnosticsResponse.u8data[0] = 4;
				diagnosticsResponse.u8data[1] = 0x71;
				diagnosticsResponse.u8data[2] = START_ROUTINE;
				diagnosticsResponse.u8data[3] = 0xFF;
				diagnosticsResponse.u8data[4] = 0x00;
				diagnosticsResponse.bDataRdy = TRUE;
				RoutineControlStatusPending =0;
				ClearFlashEraseFlag();
		 	}
			else
			{		
		      if( (u32GetClockTicks()- ClkTimer) > 4000)
		      	{
		      		ClkTimer = u32GetClockTicks();
					diagnosticsResponse.u8data[0] = 3;
					diagnosticsResponse.u8data[1] = 0x7f;
					diagnosticsResponse.u8data[2] = RoutineControlService;
					diagnosticsResponse.u8data[3] = RequestCorrectlyReceivedResponsePending;
					diagnosticsResponse.bDataRdy = TRUE;
					if(++PendingResponseCount > 2)
					{
						PendingResponseCount = 0;
						RoutineControlStatusPending =0;
					}
		      	}
			}

		return;

	}

	speed = u16GetEngineSpeed();
		
	if(securityAccessInfo.serverLockStatus == eServerLocked)
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.numBytes != 4 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else
	{
	;
	}

	RoutineIdentifier = singleFrameRx.dataBytes[3];
	RoutineIdentifier = (RoutineIdentifier << 8) | singleFrameRx.dataBytes[4];
		
	if(singleFrameRx.dataBytes[2] == START_ROUTINE )
	{
		
		if(receivedMessageError == FALSE)
		{
	 	 	switch(RoutineIdentifier)
	 	 	{
		 	 		case CHECK_PROG_DEP:
					temp = GetControllerMode();
					if(temp == eBootloaderMode)
					{
						; //do nothing in application code 
					}
					else				
					{
						RoutineControlStatusPending = CHECK_PROG_DEP;					
						
					}
						
						break;
		 	 		case ERASE_MEMORY:
											//TBD check Speedo& RPM erase memory logic to be added 
						temp = GetControllerMode();
						if(temp == eBootloaderMode)
						{
							RoutineControlStatusPending = ERASE_MEMORY;							
						}
						else				
						{
						//In aplication code erase memory not support 
						 receivedMessageError = TRUE;
						 diagnosticsResponse.u8data[3] = RequestOutOfRange; //request out of range							
						}
						break;
						case SERVICE_RESET:
											//TBD check Speedo& RPM erase memory logic to be added 
						temp = GetControllerMode();
						if(temp == eBootloaderMode)
						{
							
							status = UNSUCCESSFUL;
						}
						else				
						{
							speed = u16GetVehicleSpeed();							
							ServiceStatus = eGetServiceStatus();
							if( (speed != 0 ) && ( ServiceStatus == eServRemdNoDisp ) )
							{
								status = UNSUCCESSFUL;
							}
							else if(speed != 0)
							{
								status = UNSUCCESSFUL;
								status |= REASON1_FOR_FAILING;
								
							}
							else if(ServiceStatus == eServRemdNoDisp)
							{
								
								status = UNSUCCESSFUL;
								status |= REASON2_FOR_FAILING;
							}
							else
							{
							 vClearServiceReminder();
							 status = SUCCESSFUL;
							}
								
						}
						break;
					default:
						receivedMessageError = TRUE;
						diagnosticsResponse.u8data[3] = RequestOutOfRange; //request out of range
					break;
	 	 	}
		}

	}
	else if( (receivedMessageError == FALSE ) && (singleFrameRx.dataBytes[2] == REQUEST_ROUTINE_RESULT ) )
	{
		if(RoutineIdentifier == CRC_VERIFICATION)
		{

			temp = GetControllerMode();
			if(temp == eBootloaderMode)
			{
				status =  bCheckSumStatus();	
				bCheckSumVarificationDone = TRUE;
			}
			else
			{
			    receivedMessageError = TRUE;
			    diagnosticsResponse.u8data[3] = SubfunctionNotSupported;			
			}		 	
		}
		else
		{
			receivedMessageError = TRUE;
			diagnosticsResponse.u8data[3] = RequestOutOfRange;
		}
			
	}
	else if ( diagnosticsResponse.u8data[3] != SecurityAccessDenied )
	{
		receivedMessageError = TRUE;
		diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
	}
	else
	{
		;
	}
		
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = RoutineControlService;
	}
	else if( (speed !=0 ) && ( RoutineIdentifier != SERVICE_RESET) )
	{
		receivedMessageError = TRUE;
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
	}
		
	else
	{
		if( (singleFrameRx.dataBytes[2] == REQUEST_ROUTINE_RESULT ) || ( RoutineIdentifier == SERVICE_RESET) ) //request routine responce in 5 byte
		{
			diagnosticsResponse.u8data[0] = 5;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
			diagnosticsResponse.u8data[4] = singleFrameRx.dataBytes[4];
			diagnosticsResponse.u8data[5] = status;
		}
		else if( (RoutineControlStatusPending == CHECK_PROG_DEP ) || ( RoutineControlStatusPending == ERASE_MEMORY ) )
		{
			diagnosticsResponse.u8data[0] = 3;
			diagnosticsResponse.u8data[1] = 0x7f;
			diagnosticsResponse.u8data[2] = RoutineControlService;
			diagnosticsResponse.u8data[3] = RequestCorrectlyReceivedResponsePending;
		}
		else
		{
			diagnosticsResponse.u8data[0] = 4;
			diagnosticsResponse.u8data[1] = 0x71;
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
			diagnosticsResponse.u8data[4] = singleFrameRx.dataBytes[4];	
			
		}
		
		
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}
/*********************************************************************//**
 *
 * Link Control Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void LinkControlService(void)
{
	bool receivedMessageError = FALSE;
	

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	
	
	
   if(singleFrameRx.numBytes != 3 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}

	
	if(receivedMessageError == FALSE)	
	{
		if(singleFrameRx.dataBytes[2] == FIXED_BAUD_RATE )
		{
			switch (singleFrameRx.dataBytes[3])
			{
				
				case CAN_250K_BAUD_RATE:
					vSetCanBaudRate(e250);
				break;
				case CAN_500K_BAUD_RATE:					
					vSetCanBaudRate(e500);
				break;
				case CAN_1M_BAUD_RATE:
					vSetCanBaudRate(e1000);
				break;

				default: 
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				break;
					
			}
		}
		else
		{
		 	 	
			diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
			receivedMessageError = TRUE;
		}
		
	}//End if(receivedMessageError == FALSE)	
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = LinkControl;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 2;
		diagnosticsResponse.u8data[1] = 0xC7;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
	}
	diagnosticsResponse.bDataRdy = TRUE;


}

/*********************************************************************//**
 *
 * Clear Diagnostics Information Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
/*
void ClearDiagnosticsInformationService(void)
{

}
*/
/*********************************************************************//**
 *
 * Read Diagnostic  Information Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
/*
void ReadDiagnosticTroubleInformationService(void)
{

}
*/
/*********************************************************************//**
 *
 * Input Output Control dentifier Service multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void InputOutputControlByIdentifierServiceMul(void)
{
	bool receivedMessageError = FALSE;
	bool mulByteResponse = FALSE;
	u16 oldRPM = 0;
	u16 SpeedoSpeed = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	oldRPM = u16GetEngineSpeed();
	SpeedoSpeed = u16GetVehicleSpeed();
	if( activeDiagnosticsSession == eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}	
	else if (securityAccessInfo.serverLockStatus == eServerLocked)
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if ( oldRPM != 0 || SpeedoSpeed != 0 )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	
	if(receivedMessageError == FALSE)	
	{
		if(multipleFramesRx.mulByteDiagRespData[3] == LID_GEN_BYTE2) 
		{
			switch (multipleFramesRx.mulByteDiagRespData[4])
			{
				case LID_TELL_TALE:
					if(multipleFramesRx.mulByteDiagRespData[5] == CONTROL_WITH_TE)
					{

						if(multipleFramesRx.numBytes != (IO_CONTROL_LAMP_BYTE + EXTRA_WRITE_BYTES) )
						{
							diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
							receivedMessageError = TRUE;
						}
						else
						{
				
							InputOutputLampControl.IoControlByte[0] = multipleFramesRx.mulByteDiagRespData[6];
							InputOutputLampControl.IoControlByte[1] = multipleFramesRx.mulByteDiagRespData[7];
							InputOutputLampControl.IoControlByte[2] = multipleFramesRx.mulByteDiagRespData[8];
							InputOutputLampControl.IoControlByte[3] = multipleFramesRx.mulByteDiagRespData[9];
							InputOutputLampControl.IoControlByte[4] = multipleFramesRx.mulByteDiagRespData[10];
							InputOutputLampControl.IoControlByte[5] = multipleFramesRx.mulByteDiagRespData[11];
									
							
							sTestEquimentControl.tellTalesFlag = TRUE;						
						}
					}
					else if(multipleFramesRx.mulByteDiagRespData[5] == CONTROL_WITH_ECU)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;						
					}
					else
					{
						;
					}
				break;

				case LID_ANALOG_GAUGES:
					if(multipleFramesRx.mulByteDiagRespData[5] == CONTROL_WITH_TE)
					{

						if(multipleFramesRx.numBytes != (IO_CONTROL_GAUGES_BYTE + EXTRA_WRITE_BYTES) )
						{
							diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
							receivedMessageError = TRUE;
						}
						else
						{								
							sTestEquimentControl.IoControlFlag.u8IoControlFlagsByte = multipleFramesRx.mulByteDiagRespData[6];
							sTestEquimentControl.u8SpeedoData = multipleFramesRx.mulByteDiagRespData[7];
							sTestEquimentControl.u8TachoData = multipleFramesRx.mulByteDiagRespData[8];
							sTestEquimentControl.u8CoolantTempData = multipleFramesRx.mulByteDiagRespData[9];
							sTestEquimentControl.u8FuelData = multipleFramesRx.mulByteDiagRespData[10];
							sTestEquimentControl.u8FrontApgData = multipleFramesRx.mulByteDiagRespData[11];
							sTestEquimentControl.u8RearApgData = multipleFramesRx.mulByteDiagRespData[12];
							sTestEquimentControl.u8OilPressureData = multipleFramesRx.mulByteDiagRespData[13];
							sTestEquimentControl.u8DefData = multipleFramesRx.mulByteDiagRespData[14];						
												
						}
					}
					else if(multipleFramesRx.mulByteDiagRespData[5] == CONTROL_WITH_ECU)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;						
					}
					else
					{
						;
					}
				break;

				default: 
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
				break;	
			}
			
		}
		else
		{
			diagnosticsResponse.u8data[3] = RequestOutOfRange;
			receivedMessageError = TRUE;
		}
		
	}
	
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE  )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = InputandOutputControlbyId;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6F;
		diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3];
		diagnosticsResponse.u8data[3] = multipleFramesRx.mulByteDiagRespData[4];
	}
	
	diagnosticsResponse.bDataRdy = TRUE;


}

/*********************************************************************//**
 *
 * Input Output Control  dentifier Service
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void InputOutputControlByIdentifierService(void)
{
	bool receivedMessageError = FALSE;
	u16 oldRPM = 0;
	u16 SpeedoSpeed = 0;
	u8 i;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	oldRPM = u16GetEngineSpeed();
	SpeedoSpeed = u16GetVehicleSpeed();
	
   if( activeDiagnosticsSession == eProgrammingSession )
   {
	   diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
	   receivedMessageError = TRUE;
   }
   else if(securityAccessInfo.serverLockStatus == eServerLocked)
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if ( oldRPM != 0 || SpeedoSpeed != 0 )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	if(receivedMessageError == FALSE)	
	{
		if(singleFrameRx.dataBytes[2] == LID_GEN_BYTE2 )
		{
			switch (singleFrameRx.dataBytes[3])
			{
				case LID_TELL_TALE:
					if(singleFrameRx.dataBytes[4] == CONTROL_WITH_ECU )
					{
						if(singleFrameRx.numBytes != 4 )
						{
							diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
							receivedMessageError = TRUE;
						}
						else
						{
						   for(i =0; i < (IO_CONTROL_LAMP_BYTE-1); i++)
						   	{
								InputOutputLampControl.IoControlByte[i] = 0;
						   	}
							
							sTestEquimentControl.tellTalesFlag = FALSE;
							
						}
					}
					else if (singleFrameRx.dataBytes[4] == CONTROL_WITH_TE )
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
				break;

				case LID_ANALOG_GAUGES:
					
					 if ( singleFrameRx.dataBytes[4] == CONTROL_WITH_ECU)
					{
						if(singleFrameRx.numBytes != 4 )
						{
							diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
							receivedMessageError = TRUE;
						}
						else 
						{
							sTestEquimentControl.IoControlFlag.u8IoControlFlagsByte = 0x00;
							sTestEquimentControl.u8SpeedoData = 0x00;
							sTestEquimentControl.u8TachoData = 0x00;
							sTestEquimentControl.u8CoolantTempData = 0x00;
							sTestEquimentControl.u8FuelData =  0x00;
							sTestEquimentControl.u8FrontApgData = 0x00;
							sTestEquimentControl.u8RearApgData = 0x00;
							sTestEquimentControl.u8OilPressureData = 0x00;
							sTestEquimentControl.u8DefData = 0x00;
													  
						}
						
					}
					else
					{
						diagnosticsResponse.u8data[3] = RequestOutOfRange;
						receivedMessageError = TRUE;
					}
					break;
				default: 
					diagnosticsResponse.u8data[3] = RequestOutOfRange;
					receivedMessageError = TRUE;
					break;
			}
		}
		else
		{
			diagnosticsResponse.u8data[3] = RequestOutOfRange;
			receivedMessageError = TRUE;
		}
		
	}
	if(receivedMessageError == FALSE && conditionNotCorrect == TRUE )
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = InputandOutputControlbyId;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6F;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
	}
	diagnosticsResponse.bDataRdy = TRUE;
	
}


/*********************************************************************//**
 *
 * Control DTC Setting
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
/*
void ControlDTCSettingServiceSingle(void)
{
	
}
*/

/*********************************************************************//**
 *
 * Control DTC Setting Service Multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

/*
void ControlDTCSettingServiceMultiple(void)
{
	
			
}
*/
/*********************************************************************//**
 *
 * Read Data dentifier Service Single
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ReadDataByIdentifierServiceSingle(void)
{
	bool receivedMessageError = FALSE;
	bool mulByteResponse = FALSE;
	u8 i;
	u32 TempByte =0;
	u32 Temp1 =0;
	u32 Temp2 =0;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if(singleFrameRx.numBytes != 3 && singleFrameRx.numBytes != 5 && singleFrameRx.numBytes != 7  )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(singleFrameRx.dataBytes[2] == LID_ECU_ID_BYTE1)
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
			case LID_ECU_SERIAL_NUM:
			if( receivedMessageError == FALSE )
			{
				//ECU serial Parmeter is read only so no need to read from Memory its declare CONST
			
				multipleFramesTx.numBytes = ECU_SERIAL_NUMBER_BYTES + EXTRA_READ_BYTES;				
				for(i=0;i<ECU_SERIAL_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8EcuSerialNumber[i];
				mulByteResponse = TRUE;
			}
			break;
			
			case LID_ECU_SUPPLIER_PART_NUM:
			if( receivedMessageError == FALSE )
			{
				multipleFramesTx.numBytes = ECU_PART_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<ECU_PART_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8EcuPartNumber[i];
				mulByteResponse = TRUE;
				
			}
			break;
			
			case LID_ECU_HW_NUM:
			if( receivedMessageError == FALSE )
			{
				multipleFramesTx.numBytes = TML_ECU_HARDWARE_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<TML_ECU_HARDWARE_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8TmlEcuHardwareNumber[i];
				mulByteResponse = TRUE;
			}
			break;
			
			case LID_CONTAINER_PART_NUM:
			if( receivedMessageError == FALSE )
			{
				multipleFramesTx.numBytes = TML_CONTAINER_PART_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<TML_CONTAINER_PART_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8TmlContainerPartNumber[i];
				mulByteResponse = TRUE;
			}
			break;

				
			case LID_ECU_SW_NUM:
			if( receivedMessageError == FALSE )
			{
				multipleFramesTx.numBytes = TML_ECU_SOFTWARE_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<TML_ECU_SOFTWARE_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8TmlEcuSoftwareNumber[i];
				mulByteResponse = TRUE;
			}
			break;	

			case LID_VEHICLE_NUM:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eVehicleIdBytes);
				multipleFramesTx.numBytes = VEHICLE_ID_BYTES + EXTRA_READ_BYTES ;
				for(i=0;i<VEHICLE_ID_BYTES ;i++)
					dataTxBuffer[3+i] = u8VehicleId[i];
				mulByteResponse = TRUE;
			}
			break;
			
			case LID_VARIANT_DATASET_NUM:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eVariantDaSetIdBytes);
				multipleFramesTx.numBytes = VARIANT_DA_SET_ID_BYTES + EXTRA_READ_BYTES ;
				for(i=0;i < VARIANT_DA_SET_ID_BYTES;i++)
					dataTxBuffer[3+i] = u8VariantDaSetId[i];
				mulByteResponse = TRUE;
			}
			
			break;

			case LID_PARAMETER_PART_NUMBER:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eParameterPartNumberBytes);
				multipleFramesTx.numBytes = PARAMETER_PART_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<PARAMETER_PART_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8ParameterPartNumber[i];
				mulByteResponse = TRUE;
			}
			
			break;

			case LID_PROGRAM_SHOP_CODE:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eProgrammingShopCodeBytes);
				multipleFramesTx.numBytes = PROGRAMMING_SHOP_CODE_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<PROGRAMMING_SHOP_CODE_BYTES;i++)
					dataTxBuffer[3+i] = u8ProgrammingShopCode[i];
				mulByteResponse = TRUE;
			}
			
			break;

			case LID_DATE_OF_LAST_PROGRAM:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eDateOfLastProgrammingBytes);
				diagnosticsResponse.u8data[0] = DATE_OF_LAST_PROGRAMMING_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<DATE_OF_LAST_PROGRAMMING_BYTES;i++)
					diagnosticsResponse.u8data[4+i] = u8DateOfLastProgramming[i];
			
			}
			break;

			case LID_ECUHW_VER_NUM:
			if( receivedMessageError == FALSE )
			{
				diagnosticsResponse.u8data[0] = ECU_HARDWARE_VERSION_BYTES + EXTRA_READ_BYTES;;
				diagnosticsResponse.u8data[4] = (u16)u16ECUHardwareVersionNum>> 8;
				diagnosticsResponse.u8data[5] = (u8)u16ECUHardwareVersionNum;

						
			}
			
			break;
			case LID_ECUSW_VER_NUM:
			if( receivedMessageError == FALSE )
			{
				diagnosticsResponse.u8data[0] = TML_SOFTWARE_CALIBERATION_ID_BYTES + EXTRA_READ_BYTES;;
				diagnosticsResponse.u8data[4] = (u16)u16ECUSoftwareVersionNum >> 8;
				diagnosticsResponse.u8data[5] = (u8)u16ECUSoftwareVersionNum;

			}
			break;
			
			case LID_VEH_CONFIG_NUM:
			if( receivedMessageError == FALSE )
			{
				vGetEolData(eVehicleConfigNumberBytes);
				multipleFramesTx.numBytes = VEHICLE_CONFIG_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<VEHICLE_CONFIG_NUMBER_BYTES;i++)
					dataTxBuffer[3+i] = u8VehicleConfigNumber[i];
				mulByteResponse = TRUE;
			
			}
			break;	

			case LID_REPROGRAMMING_COUNTER:
			if( receivedMessageError == FALSE )
			{
			
				diagnosticsResponse.u8data[0] = REPROGRAMMING_COUNTER_BYTES + EXTRA_READ_BYTES;
				diagnosticsResponse.u8data[4] = (u16)u16ReprogrammingCounter >> 8;
				diagnosticsResponse.u8data[5] = (u8)u16ReprogrammingCounter;

			}
			break;

		   case LID_PCB_NO:
			if( receivedMessageError == FALSE )
			{
				multipleFramesTx.numBytes = IIL_PCB_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<IIL_PCB_NUMBER_BYTES;i++)
				{
					dataTxBuffer[3+i] = u8IILPcbNo[i];
				}
				mulByteResponse = TRUE;
			}
			break;

			case LID_SVN_NO:
			if( receivedMessageError == FALSE )
			{
			
				diagnosticsResponse.u8data[0] = IIL_SVN_NUMBER_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<IIL_SVN_NUMBER_BYTES;i++)
				{
					diagnosticsResponse.u8data[4+i] = u8IILSvnNo[i];
				}
				

			}
			break;

			
			
			default:
				diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
				receivedMessageError = TRUE;
			}			

	}
	else if(singleFrameRx.dataBytes[2]== LID_GEN_BYTE1 )
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
			case LID_VEH_OPT: 
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eVehicleContentBytes);
					multipleFramesTx.numBytes = VEHICLE_TYPE_BYTES + EXTRA_READ_BYTES;
				for(i=0;i<VEHICLE_TYPE_BYTES;i++)
					dataTxBuffer[3+i] = u8VehicleOptionContent[i];
				mulByteResponse = TRUE;
				
				}

				break;	
			
			
			case LID_AFE_CALC_UPDATE:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eAfeOptionsBytes);
					diagnosticsResponse.u8data[0] = AFE_OPTIONS_BYTES + EXTRA_READ_BYTES;
					
					diagnosticsResponse.u8data[4] = u8AfeOptions[0];
					diagnosticsResponse.u8data[5] = u8AfeOptions[1];
					
					
				}
				break;
			case LID_VEH_SPD_PPR:
				{
					vGetEolData(eVsPulsPerRevBytes);
					multipleFramesTx.numBytes = SPEED_SENSOR_BYTES + EXTRA_READ_BYTES;
					for(i=0;i<SPEED_SENSOR_BYTES;i++)
						dataTxBuffer[3+i] = u8VsPulsPerRev[i];
					mulByteResponse = TRUE;
										
				}
				break;
			case LID_ENG_SPD: 
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eEngineOverSpeedWarningBytes);
					diagnosticsResponse.u8data[0] = ( ENGINE_OVER_SPEED_BYTES -1) + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8EngineOverSpeedWarning[0];
					
				}
				
				break;	
			case LID_BUZZER_CONTROL:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eBuzzerOptionBytes);
					diagnosticsResponse.u8data[0] = BUZZER_OPTION_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8BuzzerOption[0];
					diagnosticsResponse.u8data[5] = u8BuzzerOption[1];
					diagnosticsResponse.u8data[6] = u8BuzzerOption[2];		
				}
			break;	

			case LID_FIRST_SERVICE_DUE:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eFirstService);
					diagnosticsResponse.u8data[0] = SERVICE_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8FirstService[0];
					diagnosticsResponse.u8data[5] = u8FirstService[1];
					diagnosticsResponse.u8data[6] = u8FirstService[2];
		
				}
				break;	
							
			case LID_SERVICE_INTERVAL:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eServiceInterval);
					diagnosticsResponse.u8data[0] = SERVICE_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8ServiceInterval[0];
					diagnosticsResponse.u8data[5] = u8ServiceInterval[1];
					diagnosticsResponse.u8data[6] = u8ServiceInterval[2];
		
				}
				break;						

			case LID_SERVICE_THRESHOLD:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eServiceThreshold);
					diagnosticsResponse.u8data[0] = SERVICE_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8ServiceThreshold[0];
					diagnosticsResponse.u8data[5] = u8ServiceThreshold[1];
					diagnosticsResponse.u8data[6] = u8ServiceThreshold[2];		
				}
				break;					
			case LID_COOLANT_TEMP_THRES: 
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eCoolantTempOptionBytes);
					diagnosticsResponse.u8data[0] = COOLANT_TEMP_OPTION_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8CoolantTempOption[0];
					diagnosticsResponse.u8data[5] = u8CoolantTempOption[1];
					diagnosticsResponse.u8data[6] = u8CoolantTempOption[2];					
				
					
				}
				break;
				

			case LID_FE_THRESH:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eFeIndicatorThreshBytes);
					diagnosticsResponse.u8data[0] = FE_INDICATOR_THRESH_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8FeIndicatorThresh[0];
					diagnosticsResponse.u8data[5] = u8FeIndicatorThresh[1];
					diagnosticsResponse.u8data[6] = u8FeIndicatorThresh[2];
					diagnosticsResponse.u8data[7] = u8FeIndicatorThresh[3];
					
				}
				break;

			case LID_DTE_CALC_UPDATE:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eDteMulFactorBytes);
					diagnosticsResponse.u8data[0] = DTE_MUL_FACTOR_BYTE + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8DteMulFactor[0];
								
				}
				break;
			
			case LID_OIL_PRESSURE_TALE_TELL:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eLowOilPressureThreshBytes);
					diagnosticsResponse.u8data[0] = LOW_OIL_PRESSURE_THRESH_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8LowOilPressureThresh[0];
					diagnosticsResponse.u8data[5] = u8LowOilPressureThresh[1];
								
				}
				break;				
			case LID_LOW_AIR_PRES:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eAirPressureOptionBytes);
					multipleFramesTx.numBytes = AIR_PRESSURE_OPTION_BYTES + EXTRA_READ_BYTES;
					for(i=0;i<AIR_PRESSURE_OPTION_BYTES;i++)
						dataTxBuffer[3+i] = u8AirPressureOption[i];
					mulByteResponse = TRUE;
										
				}
				break;
				
			case LID_TELL_TALE:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eTelltalesOptionBytes);

					diagnosticsResponse.u8data[0] = TELLTALES_OPTION_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8TelltalesOption[0];
					diagnosticsResponse.u8data[5] = u8TelltalesOption[1];
					diagnosticsResponse.u8data[6] = u8TelltalesOption[2];
					diagnosticsResponse.u8data[7] = u8TelltalesOption[3];				
					
					
				}
				break;	
			case DISP_LCD_SCREEN:
				if( receivedMessageError == FALSE )
				{					
					vGetEolData(eDisplayScreenConfigBytes);
					diagnosticsResponse.u8data[0] = DISPLAY_SCREEN_CONFIG_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = u8DisplayScreenConfig[0];
					diagnosticsResponse.u8data[5] = u8DisplayScreenConfig[1];
				}
				break;

			case LID_ODOMETER:			

				if( receivedMessageError == FALSE )
				{
					Temp1 = u32GetOdoValue();
					Temp2 = u8GetOdoFractionValue();
				    TempByte = Temp1 * 10 + Temp2;
					diagnosticsResponse.u8data[0] = ODO_WRITE_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = (u32)TempByte >> 16;
					diagnosticsResponse.u8data[5] = (u32)TempByte >> 8;
					diagnosticsResponse.u8data[6] = (u8)TempByte;

				}
				break;

			case LID_HOURMETER:			

				if( receivedMessageError == FALSE )
				{
					Temp1 = u32GetHoursValue();
					Temp2 = u8GetHoursFractionValue();
				    TempByte = Temp1 * 10 + Temp2;
					diagnosticsResponse.u8data[0] = ODO_WRITE_BYTES + EXTRA_READ_BYTES;
					diagnosticsResponse.u8data[4] = (u32)TempByte >> 16;
					diagnosticsResponse.u8data[5] = (u32)TempByte >> 8;
					diagnosticsResponse.u8data[6] = (u8)TempByte;

				}
				break;				
				
			default:
				diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
				receivedMessageError = TRUE;
				
		}
	}
	else
	{
		diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDataIdentifier;
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	else if (mulByteResponse == TRUE )
	{
		dataTxBuffer[0] = 0x62;
		dataTxBuffer[1] = singleFrameRx.dataBytes[2];
		dataTxBuffer[2] = singleFrameRx.dataBytes[3];
		formatMulBytesIntoFrames();
	}
	else
	{
		diagnosticsResponse.u8data[1] = 0x62;
		diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
		diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	
			
}


/*********************************************************************//**
 *
 * Read Data dentifier Service Multiple
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void ReadDataByIdentifierServiceMultiple(void)
{
	
	bool receivedMessageError = FALSE;
	bool mulByteResponse = FALSE;
	u8 i,multiByteIndex = 0,numOfIds,j;
	u32 TempByte;
	u32 Temp1 = 0;
	u32 Temp2 = 0;

	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	numOfIds = ( multipleFramesRx.numBytes - 1 )/2;
	if( numOfIds > NUM_READ_ID_SUPPORTED )
	{
		diagnosticsResponse.u8data[3] = RequestOutOfRange;
		receivedMessageError = TRUE;
	}

	else if(conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	
	multipleFramesTx.numBytes = 1;
	dataTxBuffer[multiByteIndex++] = 0x62;
	for(i=0; i < numOfIds && multiByteIndex < MAX_DATA_LENGTH ; i++)
	{
		if((multipleFramesRx.mulByteDiagRespData[2*i+3])== LID_GEN_BYTE1)
		{
			switch(multipleFramesRx.mulByteDiagRespData[2*i+4]) //first 4 bytes have: 2 numbytes + read identifier+ first byte of identifier
			{
				case LID_VEH_OPT: 
					if( receivedMessageError == FALSE )
					{
					vGetEolData(eVehicleContentBytes);
						multipleFramesTx.numBytes += VEHICLE_TYPE_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<VEHICLE_TYPE_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8VehicleOptionContent[j];
												
					}

					break;	
			
				
				case LID_AFE_CALC_UPDATE:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eAfeOptionsBytes);
						multipleFramesTx.numBytes += AFE_OPTIONS_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<AFE_OPTIONS_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8AfeOptions[j];

						
					}
					break;
				case LID_VEH_SPD_PPR:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eVsPulsPerRevBytes);
						multipleFramesTx.numBytes += SPEED_SENSOR_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<SPEED_SENSOR_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8VsPulsPerRev[j];
												
					}
					break;
				case LID_ENG_SPD: 
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eEngineOverSpeedWarningBytes);
						multipleFramesTx.numBytes += (ENGINE_OVER_SPEED_BYTES-1) + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						dataTxBuffer[multiByteIndex++] = u8EngineOverSpeedWarning[0];
					}
					break;	
				case LID_BUZZER_CONTROL:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eBuzzerOptionBytes);
						multipleFramesTx.numBytes += BUZZER_OPTION_BYTES + EXTRA_READ_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<BUZZER_OPTION_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8BuzzerOption[j];
							
					}
				break;	
				
				case LID_FIRST_SERVICE_DUE:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eFirstService);
						multipleFramesTx.numBytes += SERVICE_BYTES + EXTRA_READ_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<SERVICE_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8FirstService[j];
				
					}
					break;	
								
				case LID_SERVICE_INTERVAL:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eServiceInterval);						
						multipleFramesTx.numBytes += SERVICE_BYTES + EXTRA_READ_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<SERVICE_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8ServiceInterval[j];						
								
					}
					break;						
				
				case LID_SERVICE_THRESHOLD:
					if( receivedMessageError == FALSE )
					{

						vGetEolData(eServiceThreshold);						
						multipleFramesTx.numBytes += SERVICE_BYTES + EXTRA_READ_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<SERVICE_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8ServiceThreshold[j];	
					
					}
					break;			

			
				case LID_COOLANT_TEMP_THRES: 
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eCoolantTempOptionBytes);
						multipleFramesTx.numBytes += COOLANT_TEMP_OPTION_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<COOLANT_TEMP_OPTION_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8CoolantTempOption[j];
						
					}
					break;


				case LID_FE_THRESH:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eFeIndicatorThreshBytes);
						multipleFramesTx.numBytes += FE_INDICATOR_THRESH_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<FE_INDICATOR_THRESH_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8FeIndicatorThresh[j];
					
					}
					break;

				case LID_DTE_CALC_UPDATE:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eDteMulFactorBytes);
						multipleFramesTx.numBytes += DTE_MUL_FACTOR_BYTE + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];						
						dataTxBuffer[multiByteIndex++] = u8DteMulFactor[0];
												
					}
					break;
				

					break;	
				case LID_OIL_PRESSURE_TALE_TELL:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eLowOilPressureThreshBytes);
					multipleFramesTx.numBytes += LOW_OIL_PRESSURE_THRESH_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<LOW_OIL_PRESSURE_THRESH_BYTES;j++)
					dataTxBuffer[multiByteIndex++] = u8LowOilPressureThresh[j];					
											
				}
				break;
				case LID_LOW_AIR_PRES:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eAirPressureOptionBytes);
						multipleFramesTx.numBytes += AIR_PRESSURE_OPTION_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<AIR_PRESSURE_OPTION_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8AirPressureOption[j];
						
					}
					break;
				
				case LID_TELL_TALE:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eTelltalesOptionBytes);
						multipleFramesTx.numBytes += TELLTALES_OPTION_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						for(j=0;j<TELLTALES_OPTION_BYTES;j++)
							dataTxBuffer[multiByteIndex++] = u8TelltalesOption[j];
						
					}
					break;	
				case DISP_LCD_SCREEN:
					if( receivedMessageError == FALSE )
					{
						vGetEolData(eDisplayScreenConfigBytes);
						multipleFramesTx.numBytes += DISPLAY_SCREEN_CONFIG_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						dataTxBuffer[multiByteIndex++] = u8DisplayScreenConfig[0];
						dataTxBuffer[multiByteIndex++] = u8DisplayScreenConfig[1];
					}
					break;

				case LID_ODOMETER:	

				if( receivedMessageError == FALSE )
				{
						Temp1 = u32GetOdoValue();
						Temp2 = u8GetOdoFractionValue();
						TempByte = Temp1 * 10 + Temp2;
						multipleFramesTx.numBytes += ODO_WRITE_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
						dataTxBuffer[multiByteIndex++] = (u32)TempByte >> 16;
						dataTxBuffer[multiByteIndex++] = (u32)TempByte >> 8;
						dataTxBuffer[multiByteIndex++] = (u8)TempByte;
				    
					
				}
				break;	

				case LID_HOURMETER:			

				if( receivedMessageError == FALSE )
				{
					Temp1 = u32GetHoursValue();
					Temp2 = u8GetHoursFractionValue();
				    TempByte = Temp1 * 10 + Temp2;
					multipleFramesTx.numBytes += ODO_WRITE_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					dataTxBuffer[multiByteIndex++] = (u32)TempByte >> 16;
					dataTxBuffer[multiByteIndex++] = (u32)TempByte >> 8;
					dataTxBuffer[multiByteIndex++] = (u8)TempByte;

				}
				break;		
					
				default:
					diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
					receivedMessageError = TRUE;
					
			}
		}
		else if((multipleFramesRx.mulByteDiagRespData[2*i+3])== LID_ECU_ID_BYTE1)
		{

			switch(multipleFramesRx.mulByteDiagRespData[2*i+4]) //second u8 of the LID
			{
				case LID_ECU_SERIAL_NUM:
				if( receivedMessageError == FALSE )
				{
					
					multipleFramesTx.numBytes += ECU_SERIAL_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<ECU_SERIAL_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8EcuSerialNumber[j];
					
				}
				break;
				
				case LID_ECU_SUPPLIER_PART_NUM:
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes += ECU_PART_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<ECU_PART_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8EcuPartNumber[j];
					
					
				}
				break;
				
				case LID_ECU_HW_NUM:
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes += TML_ECU_HARDWARE_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<TML_ECU_HARDWARE_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8TmlEcuHardwareNumber[j];
					
				}
				break;
				
				case LID_CONTAINER_PART_NUM:
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes += TML_CONTAINER_PART_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<TML_CONTAINER_PART_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8TmlContainerPartNumber[j];
					
				}
				break;
	
					
				case LID_ECU_SW_NUM:
				if( receivedMessageError == FALSE )
				{
					multipleFramesTx.numBytes += TML_ECU_SOFTWARE_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<TML_ECU_SOFTWARE_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8TmlEcuSoftwareNumber[j];
					
				}
				break;
	
				case LID_VEHICLE_NUM:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eVehicleIdBytes);
					multipleFramesTx.numBytes += VEHICLE_ID_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<VEHICLE_ID_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8VehicleId[j];
					
				}
				break;
				
				case LID_VARIANT_DATASET_NUM:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eVariantDaSetIdBytes);
					multipleFramesTx.numBytes += VARIANT_DA_SET_ID_BYTES + EXTRA_READ_MUL_BYTES;
						dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<VARIANT_DA_SET_ID_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8VariantDaSetId[j];
					
				}
				break;
	
				case LID_PARAMETER_PART_NUMBER:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eParameterPartNumberBytes);
					multipleFramesTx.numBytes += PARAMETER_PART_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<PARAMETER_PART_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8ParameterPartNumber[j];
					
				}
				break;
	
				case LID_PROGRAM_SHOP_CODE:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eProgrammingShopCodeBytes);
					multipleFramesTx.numBytes += PROGRAMMING_SHOP_CODE_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<PROGRAMMING_SHOP_CODE_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8ProgrammingShopCode[j];
				
				}
				
				break;
	
				case LID_DATE_OF_LAST_PROGRAM:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eDateOfLastProgrammingBytes);
					multipleFramesTx.numBytes += DATE_OF_LAST_PROGRAMMING_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<DATE_OF_LAST_PROGRAMMING_BYTES;j++)
						dataTxBuffer[multiByteIndex++]  = u8DateOfLastProgramming[j];
				
				}
				break;
	
				case LID_ECUHW_VER_NUM:
				if( receivedMessageError == FALSE )
				{
				
					multipleFramesTx.numBytes += ECU_HARDWARE_VERSION_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					dataTxBuffer[multiByteIndex++]  = (u16)u16ECUHardwareVersionNum>> 8;
					dataTxBuffer[multiByteIndex++]  = (u8)u16ECUHardwareVersionNum;
				
				}
				
				break;
				case LID_ECUSW_VER_NUM:
				if( receivedMessageError == FALSE )
				{
				
					multipleFramesTx.numBytes += TML_SOFTWARE_CALIBERATION_ID_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					dataTxBuffer[multiByteIndex++]  = (u16)u16ECUSoftwareVersionNum >> 8;
					dataTxBuffer[multiByteIndex++]  = (u8)u16ECUSoftwareVersionNum;
				
				}
						
				break;

				case LID_VEH_CONFIG_NUM:
				if( receivedMessageError == FALSE )
				{
					vGetEolData(eVehicleConfigNumberBytes);
					multipleFramesTx.numBytes += VEHICLE_CONFIG_NUMBER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					for(j=0;j<VEHICLE_CONFIG_NUMBER_BYTES;j++)
						dataTxBuffer[multiByteIndex++] = u8VehicleConfigNumber[j];
				}
				break;	

				case LID_REPROGRAMMING_COUNTER:
				if( receivedMessageError == FALSE )
				{

					multipleFramesTx.numBytes += REPROGRAMMING_COUNTER_BYTES + EXTRA_READ_MUL_BYTES;
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+3];
					dataTxBuffer[multiByteIndex++] = multipleFramesRx.mulByteDiagRespData[2*i+4];
					dataTxBuffer[multiByteIndex++]  = (u16)u16ReprogrammingCounter >> 8;
					dataTxBuffer[multiByteIndex++]  = (u8)u16ReprogrammingCounter;
				}
				break;
				
				default:
					diagnosticsResponse.u8data[3] = SubfunctionNotSupported;
					receivedMessageError = TRUE;
					
				}			

		}
		
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = ReadDataIdentifier;
		diagnosticsResponse.bDataRdy = TRUE ;
	}
	else
	{
		formatMulBytesIntoFrames();
	}
	
			
}


/*********************************************************************//**
 *
 * Write Data Identifier Service Single
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void WriteDataByIdentifierServiceSingle(void)
{
	bool receivedMessageError = FALSE;
	u32 tempVar;
	u32 Temp1;
	u32 Temp2;
	u32 Hour;
	u8 i,j;

	
	if(commInfo.normalCommTxEnabled == FALSE )
		commInfo.noResponse = TRUE;
	
	if (commInfo.normalCommRxEnabled == FALSE )
		return;

	
	// the order of following conditions should not change; they have been arranged to maintain the priority of Negative Response Codes
	if( activeDiagnosticsSession == eProgrammingSession )
	{
		diagnosticsResponse.u8data[3] = ServiceNotSupportedInActiveSession;
		receivedMessageError = TRUE;
	}
	else if ( securityAccessInfo.serverLockStatus == eServerLocked) // check this error conditions with the customer
	{
		diagnosticsResponse.u8data[3] = SecurityAccessDenied;
		receivedMessageError = TRUE;
	}
	else if(singleFrameRx.dataReady == TRUE && singleFrameRx.numBytes > 7 )
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}  
	


	if(multipleFramesRx.dataReady == TRUE)
	{
		if(multipleFramesRx.mulByteDiagRespData[3] == LID_GEN_BYTE1 &&						
			multipleFramesRx.mulByteDiagRespData[4] == LID_VEH_OPT)
		{
			if( receivedMessageError == FALSE )
			{

						
				if(multipleFramesRx.numBytes != VEHICLE_TYPE_BYTES + EXTRA_WRITE_BYTES)
				{
					diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
					receivedMessageError = TRUE;
				}
				else
				{
						for(i=0;i<VEHICLE_TYPE_BYTES;i++)
							u8VehicleOptionContent[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eVehicleContentBytes);				
				}
			}
		} 
		 
	     else if(multipleFramesRx.mulByteDiagRespData[3] == LID_GEN_BYTE1 &&						
				multipleFramesRx.mulByteDiagRespData[4] == LID_LOW_AIR_PRES)
			{
				if( receivedMessageError == FALSE )
				{						
					if(multipleFramesRx.numBytes != AIR_PRESSURE_OPTION_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
							for(i=0;i<AIR_PRESSURE_OPTION_BYTES;i++)
								u8AirPressureOption[i] = multipleFramesRx.mulByteDiagRespData[5+i];
							vSaveEolData(eAirPressureOptionBytes);				
					}
				}
			} 
		else if(multipleFramesRx.mulByteDiagRespData[3] == LID_GEN_BYTE1 && 
			multipleFramesRx.mulByteDiagRespData[4] == LID_VEH_SPD_PPR)
			   
		   {
			   if( receivedMessageError == FALSE )
			   {					   
				   if(multipleFramesRx.numBytes != SPEED_SENSOR_BYTES + EXTRA_WRITE_BYTES)
				   {
					   diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
					   receivedMessageError = TRUE;
				   }
				   else
				   {
						
						    tempVar = (u16)multipleFramesRx.mulByteDiagRespData[5]<<8;
					   		tempVar |= multipleFramesRx.mulByteDiagRespData[6];

					   
					   
					   if( ( multipleFramesRx.mulByteDiagRespData[9] <= 5)&&
					   	   (multipleFramesRx.mulByteDiagRespData[8] >= SPEEDO_BIAS_FACTOR_MIN && multipleFramesRx.mulByteDiagRespData[8] <= SPEEDO_BIAS_FACTOR_MAX)
						   && (multipleFramesRx.mulByteDiagRespData[7] <= SPEEDO_OVER_SPEED)
						   && (tempVar >= SPEEDO_PULSES_MIN && tempVar <= SPEEDO_PULSES_MAX) )
						   {
							   for(i=0;i<SPEED_SENSOR_BYTES;i++)
							   	u8VsPulsPerRev[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						   	   vSaveEolData(eVsPulsPerRevBytes);
						   }
						   else
						   {
							   diagnosticsResponse.u8data[3] = RequestOutOfRange;
							   receivedMessageError = TRUE;
						   }
				   }
			   }
		   } 

		else if (multipleFramesRx.mulByteDiagRespData[3] == LID_ECU_ID_BYTE1)
		{
			switch(multipleFramesRx.mulByteDiagRespData[4])
			{
			
				case LID_VEHICLE_NUM:
				if( receivedMessageError == FALSE )
				{
					if(multipleFramesRx.numBytes != VEHICLE_ID_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						for(i=0;i<VEHICLE_ID_BYTES;i++)
							 u8VehicleId[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eVehicleIdBytes);
												
						
					}
				}
				break;
				
				case LID_VARIANT_DATASET_NUM:
				if( receivedMessageError == FALSE )
				{
					if(multipleFramesRx.numBytes != VARIANT_DA_SET_ID_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						for(i=0;i<VARIANT_DA_SET_ID_BYTES;i++)
							u8VariantDaSetId[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eVariantDaSetIdBytes);
					
						
					}
				}
				
				break;

			case LID_PARAMETER_PART_NUMBER:
				if( receivedMessageError == FALSE )
				{
					if(multipleFramesRx.numBytes != PARAMETER_PART_NUMBER_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
										
						for(i=0;i<PARAMETER_PART_NUMBER_BYTES;i++)
							u8ParameterPartNumber[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eParameterPartNumberBytes);
											
					}
				}
				
				break;

			case LID_PROGRAM_SHOP_CODE:
				if( receivedMessageError == FALSE )
				{
					if(multipleFramesRx.numBytes != PROGRAMMING_SHOP_CODE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						for(i=0;i<PROGRAMMING_SHOP_CODE_BYTES;i++)
							u8ProgrammingShopCode[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eProgrammingShopCodeBytes);
					
						
					}
				}
				
				break;

				case LID_VEH_CONFIG_NUM:
				if( receivedMessageError == FALSE )
				{
					if(multipleFramesRx.numBytes != VEHICLE_CONFIG_NUMBER_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						for(i=0;i<VEHICLE_CONFIG_NUMBER_BYTES;i++)
							u8VehicleConfigNumber[i] = multipleFramesRx.mulByteDiagRespData[5+i];
						vSaveEolData(eVehicleConfigNumberBytes);
											
					}
				}
				break;
			
			default:
				diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
				receivedMessageError = TRUE;
			}
		}
		else
		{
			diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
			receivedMessageError = TRUE;
		}
	}
	else if(singleFrameRx.dataBytes[2]== LID_ECU_ID_BYTE1 ) 
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{

			case LID_DATE_OF_LAST_PROGRAM:
				if( receivedMessageError == FALSE )
				{
					if(singleFrameRx.numBytes != DATE_OF_LAST_PROGRAMMING_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						for(i=0;i<DATE_OF_LAST_PROGRAMMING_BYTES;i++)
							u8DateOfLastProgramming[i] = singleFrameRx.dataBytes[4+i];
						vSaveEolData(eDateOfLastProgrammingBytes);
												
					}
				}
				break;
					
			default:
				diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
				receivedMessageError = TRUE;

		}
	}
	else if(singleFrameRx.dataBytes[2]== LID_GEN_BYTE1 )
	{
		switch(singleFrameRx.dataBytes[3] ) //second byte of the LID
		{
						
			case LID_ENG_SPD: 
				if( receivedMessageError == FALSE )
				{

					if( singleFrameRx.numBytes != ( ENGINE_OVER_SPEED_BYTES - 1 ) + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						if( singleFrameRx.dataBytes[4] <= RPM_EOL_MAX_RANGE) 
						{
							u8EngineOverSpeedWarning[0] = singleFrameRx.dataBytes[4];
							vSaveEolData(eEngineOverSpeedWarningBytes);
													
						}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
						
					}
					
					
					 
				}
				break;

			case LID_BUZZER_CONTROL: 
				if( receivedMessageError == FALSE )
				{

					if( singleFrameRx.numBytes != BUZZER_OPTION_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						 for(i=0;i<BUZZER_OPTION_BYTES;i++)
							u8BuzzerOption[i] = singleFrameRx.dataBytes[4+i];
							vSaveEolData(eBuzzerOptionBytes);						
					}				
					
					 
				}
				break;
				
			case LID_FIRST_SERVICE_DUE: 
				if( receivedMessageError == FALSE )
				{

					if( singleFrameRx.numBytes != SERVICE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						if( singleFrameRx.dataBytes[4] <= SERVICE_MAX_RANGE)
						{
							for(i=0;i<SERVICE_BYTES;i++)
								u8FirstService[i] = singleFrameRx.dataBytes[4+i];
							vSaveEolData(eFirstService);

							tempVar = (u16)u8FirstService[0]<<8;
							tempVar |= (u16)u8FirstService[1];
							tempVar = tempVar<<8;
							tempVar |= u8FirstService[2];
							sServReminder.u32FirstServiceDue = tempVar;
							
							vCalcNextServiceDue(UpdateEolServiceparameter);
						}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
						
					}				
					
					 
				}
				break;
			case LID_SERVICE_INTERVAL: 
				if( receivedMessageError == FALSE )
				{

					if( singleFrameRx.numBytes != SERVICE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						if( singleFrameRx.dataBytes[4] <= SERVICE_MAX_RANGE)
						{
							for(i=0;i<SERVICE_BYTES;i++)
								u8ServiceInterval[i] = singleFrameRx.dataBytes[4+i];
							vSaveEolData(eServiceInterval);

								tempVar = (u16)u8ServiceInterval[0]<<8;
								tempVar |= (u16)u8ServiceInterval[1];
								tempVar = tempVar<<8;
								tempVar |= u8ServiceInterval[2];
								
								sServReminder.u32ServiceInterval = tempVar;	

								vCalcNextServiceDue(UpdateEolServiceparameter);
													
						}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
						
					}				
					
					 
				}
				break;
			case LID_SERVICE_THRESHOLD: 
				if( receivedMessageError == FALSE )
				{

					if( singleFrameRx.numBytes != SERVICE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
					
						if( singleFrameRx.dataBytes[4] <= SERVICE_MAX_RANGE)
						{
							for(i=0;i<SERVICE_BYTES;i++)
							u8ServiceThreshold[i] = singleFrameRx.dataBytes[4+i];
							vSaveEolData(eServiceThreshold);

						    tempVar = (u16)u8ServiceThreshold[0]<<8;
							tempVar |= (u16)u8ServiceThreshold[1];
							tempVar = tempVar<<8;
							tempVar |= u8ServiceThreshold[2];
							
							sServReminder.u32PopUpInterval = tempVar;	
						}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
						
					}				
					
					 
				}
				break;
				
						 
			
			case LID_AFE_CALC_UPDATE:
				if( receivedMessageError == FALSE )
				{
					if( singleFrameRx.numBytes != AFE_OPTIONS_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else 
						{
							if(singleFrameRx.dataBytes[4] <= AFE_MIN_DISTANCE_TRAVEL 
							&& (singleFrameRx.dataBytes[5] >= AFE_MUL_FACTOR_MIN && singleFrameRx.dataBytes[5] <= AFE_MUL_FACTOR_MAX) )
							{
								u8AfeOptions[0] = singleFrameRx.dataBytes[4];
								u8AfeOptions[1] = singleFrameRx.dataBytes[5];
								
								vSaveEolData(eAfeOptionsBytes);
								
							}
							else
							{
								diagnosticsResponse.u8data[3] = RequestOutOfRange;
								receivedMessageError = TRUE;
							}
						}
					
				}
				break;
				/*
			case LID_VEH_SPD_PPR:
				if( receivedMessageError == FALSE )
				{
					if(singleFrameRx.numBytes != SPEED_SENSOR_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						tempVar = (u16)singleFrameRx.dataBytes[4]<<8;
						tempVar |= singleFrameRx.dataBytes[5];

						
						
						if( (singleFrameRx.dataBytes[7] >= SPEEDO_BIAS_FACTOR_MIN && singleFrameRx.dataBytes[7] <= SPEEDO_BIAS_FACTOR_MAX)
							&& (singleFrameRx.dataBytes[6] <= SPEEDO_OVER_SPEED)
							&& (tempVar >= SPEEDO_PULSES_MIN && tempVar <= SPEEDO_PULSES_MAX) )
							{
								u8VsPulsPerRev[0] = singleFrameRx.dataBytes[4];
								u8VsPulsPerRev[1] = singleFrameRx.dataBytes[5];
								u8VsPulsPerRev[2] = singleFrameRx.dataBytes[6];
								u8VsPulsPerRev[3] = singleFrameRx.dataBytes[7];
								vSaveEolData(eVsPulsPerRevBytes);
							}
							else
							{
								diagnosticsResponse.u8data[3] = RequestOutOfRange;
								receivedMessageError = TRUE;
							}
							
						
					}
					
					
				}	
				break;
				*/
				
			case LID_COOLANT_TEMP_THRES: 
				if( receivedMessageError == FALSE )
				{
					if( singleFrameRx.numBytes != COOLANT_TEMP_OPTION_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						if(singleFrameRx.dataBytes[6] > TEMP_START_RED_ZONE_MIN && singleFrameRx.dataBytes[6] < TEMP_START_RED_ZONE_MAX)
						{
							u8CoolantTempOption[0] = singleFrameRx.dataBytes[4];
							u8CoolantTempOption[1] = singleFrameRx.dataBytes[5];
							u8CoolantTempOption[2] = singleFrameRx.dataBytes[6];
							vSaveEolData(eCoolantTempOptionBytes); 	
						}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}

									
						
					}
					
				}
				
				break;
			case LID_FE_THRESH:
				if( receivedMessageError == FALSE )
				{
					
					if( singleFrameRx.numBytes != FE_INDICATOR_THRESH_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						
						if(singleFrameRx.dataBytes[4] > FE_DATA_RANGE || singleFrameRx.dataBytes[5] > FE_DATA_RANGE
						|| singleFrameRx.dataBytes[6] > FE_DATA_RANGE ||singleFrameRx.dataBytes[7] > FE_DATA_RANGE )
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
						else
						{
							u8FeIndicatorThresh[0] = singleFrameRx.dataBytes[4];
							u8FeIndicatorThresh[1] = singleFrameRx.dataBytes[5];
							u8FeIndicatorThresh[2] = singleFrameRx.dataBytes[6];
							u8FeIndicatorThresh[3] = singleFrameRx.dataBytes[7];								
							vSaveEolData(eFeIndicatorThreshBytes);
												
						}
					}
					
				}
				break;

			case LID_DTE_CALC_UPDATE:
				
				if( receivedMessageError == FALSE )
				{
					
					if( singleFrameRx.numBytes != DTE_MUL_FACTOR_BYTE + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else 
					{
					 	if(singleFrameRx.dataBytes[4] >= DTE_MUL_FACTOR_MIN && singleFrameRx.dataBytes[4] <= DTE_MUL_FACTOR_MAX )
				 		{
							u8DteMulFactor[0] = singleFrameRx.dataBytes[4];						
							vSaveEolData(eDteMulFactorBytes);
				 		}
						else
						{
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
						}
										
					}
					
					
				}
				
				break;
				
				case LID_OIL_PRESSURE_TALE_TELL:
				if( receivedMessageError == FALSE )
				{
					if( singleFrameRx.numBytes != LOW_OIL_PRESSURE_THRESH_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						u8LowOilPressureThresh[0] = singleFrameRx.dataBytes[4];
						u8LowOilPressureThresh[1] = singleFrameRx.dataBytes[5];
						
						vSaveEolData(eLowOilPressureThreshBytes);
						
					}
					
				}
				break;
					
			case DISP_LCD_SCREEN:
				if( receivedMessageError == FALSE )
				{
					
					if( singleFrameRx.numBytes != DISPLAY_SCREEN_CONFIG_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						u8DisplayScreenConfig[0] = singleFrameRx.dataBytes[4];
						u8DisplayScreenConfig[1] = singleFrameRx.dataBytes[5];
						vSaveEolData(eDisplayScreenConfigBytes);					
					}
					
				}
				break;	
				case LID_TELL_TALE:
				if( receivedMessageError == FALSE )
				{
					
					if( singleFrameRx.numBytes != TELLTALES_OPTION_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						u8TelltalesOption[0] = singleFrameRx.dataBytes[4];
						u8TelltalesOption[1] = singleFrameRx.dataBytes[5];
						u8TelltalesOption[2] = singleFrameRx.dataBytes[6];
						u8TelltalesOption[3] = singleFrameRx.dataBytes[7];
						vSaveEolData(eTelltalesOptionBytes);					
					}
					
				}
				break;	
			case LID_ODOMETER:	
				if( receivedMessageError == FALSE )
				{
					if( singleFrameRx.numBytes != ODO_WRITE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						tempVar = (u32)singleFrameRx.dataBytes[4] << 16;
						tempVar |= (u32)singleFrameRx.dataBytes[5] << 8;
						tempVar |= (u32)singleFrameRx.dataBytes[6];
						
						i = u8GetOdoUpdateByEolCounter();
						// j used to keep count value when value written < current 
						// i used to keep count value when value written > current 
						j = i >> 4;
						i = i & 0x0F;	

						Temp1 = u32GetHoursValue();
						Temp2 = u8GetHoursFractionValue();
						Hour = Temp1 * 10 + Temp2;
						
						Temp1 = u32GetOdoValue();
						Temp2 = u8GetOdoFractionValue();
					    Temp1 = Temp1 * 10 + Temp2;
						
	 					if( (tempVar > 9999999) || (tempVar < Temp1) || (i>=2) )
	 					{
	 					//User can write lower value to current odo value  if vehicle has run <500kms or < 200hrs.
	 					   if( (Temp1 > 5000 ) || ( Hour > 2000 ) || (j>=2) )
	 					   {
							diagnosticsResponse.u8data[3] = RequestOutOfRange;
							receivedMessageError = TRUE;
	 					   }
							
						}
						
						if(receivedMessageError == 0)
						{
							if( (Temp1 < 5000) && (j < 2) && (tempVar < Temp1))
							{
							    if(j>=2)
								{
									diagnosticsResponse.u8data[3] = RequestOutOfRange;
									receivedMessageError = TRUE;
								}
								else
								{
									j++;	
								}
							}
							else
							{
								if(i>=2)
								{
									diagnosticsResponse.u8data[3] = RequestOutOfRange;
									receivedMessageError = TRUE;
								}
								else
								{
									i++;	
								}
							}

							if(receivedMessageError == 0)
							{
								i = (j<<4) | i;				
								
								vSaveOdoUpdateByEolCounter(i);							
								
								vSetOdoFractionValue(tempVar%10);
								tempVar = tempVar/10;
								vSetOdoValue(tempVar);
								vSetOdoUpdateFlagStatus(TRUE);							
								vSetOdoFractionUpdateStatus(TRUE);
							}
						}
					}
				}
				break;
			case LID_HOURMETER:	
				if( receivedMessageError == FALSE )
				{
					if( singleFrameRx.numBytes != ODO_WRITE_BYTES + EXTRA_WRITE_BYTES)
					{
						diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
						receivedMessageError = TRUE;
					}
					else
					{
						tempVar = (u32)singleFrameRx.dataBytes[4] << 16;
						tempVar |= (u32)singleFrameRx.dataBytes[5] << 8;
						tempVar |= (u32)singleFrameRx.dataBytes[6];

						i = u8GetHourUpdateByEolCounter();
						// j used to keep count value when value written < current 
						// i used to keep count value when value written > current 
						j = i >> 4;
						i = i & 0x0F;

						Temp1 = u32GetHoursValue();
						Temp2 = u8GetHoursFractionValue();
				    	Hour = Temp1 * 10 + Temp2;

						Temp1 = u32GetOdoValue();
						Temp2 = u8GetOdoFractionValue();
					    Temp1 = Temp1 * 10 + Temp2;
						
	 					if(tempVar > 9999999 || tempVar < Hour || i>=2)
	 					{
	 					  //User can write lower value to current odo value  if vehicle has run <500kms or < 200hrs.
	 					   if( (Temp1 > 5000 ) || ( Hour > 2000 ) || (j>=2) )
	 					   {
								diagnosticsResponse.u8data[3] = RequestOutOfRange;
								receivedMessageError = TRUE;
	 					   	}
						}
						
						if(receivedMessageError == 0)
						{
							if( Hour < 2000 && (j < 2)&& (tempVar < Hour) )
							{
							 	if(j>=2)
								{
									diagnosticsResponse.u8data[3] = RequestOutOfRange;
									receivedMessageError = TRUE;
								}
								else
								{
									j++;	
								}
							}
							else
							{
								if(i>=2)
								{
									diagnosticsResponse.u8data[3] = RequestOutOfRange;
									receivedMessageError = TRUE;
								}
								else
								{
									i++;	
								}
							}

							if(receivedMessageError == 0)
							{
								i = (j<<4) | i;							
																				
								vSaveHourUpdateByEolCounter(i);	
								
								vSetHoursFractionValue(tempVar%10);
								tempVar = tempVar/10;
								vSetHoursValue(tempVar);
								vSetHoursUpdateStatus(TRUE);
								vSetHoursFractionUpdateStatus(TRUE);
							}
							
						}
						
					}
				}
				break;

				
			
			default:
				diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;;
				receivedMessageError = TRUE;
				
				
		}
		
	}
	
	else
	{
		diagnosticsResponse.u8data[3] = IncorrectMessageLengthOrInvalidFormat;
		receivedMessageError = TRUE;
	}
	
	//add the error : general programming failure when eeprom saving returns error
	if(receivedMessageError== FALSE && conditionNotCorrect == TRUE)
	{
		diagnosticsResponse.u8data[3] = ConditionsNotCorrect;
		receivedMessageError = TRUE;
	}
	if(receivedMessageError == TRUE )
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = NegativeResponseID;
		diagnosticsResponse.u8data[2] = WriteDatabyIdentifier;
	}
	else
	{
		diagnosticsResponse.u8data[0] = 3;
		diagnosticsResponse.u8data[1] = 0x6E;
		if( multipleFramesRx.dataReady == TRUE )
		{
			diagnosticsResponse.u8data[2] = multipleFramesRx.mulByteDiagRespData[3] ;
			diagnosticsResponse.u8data[3] = multipleFramesRx.mulByteDiagRespData[4] ;

		}
		else
		{
			diagnosticsResponse.u8data[2] = singleFrameRx.dataBytes[2];
			diagnosticsResponse.u8data[3] = singleFrameRx.dataBytes[3];
		}

	}
	diagnosticsResponse.bDataRdy = TRUE ;
	

}

/*********************************************************************//**
 *
 * Encryptoin Data
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

static void getEncryptedData(void)
{
	u8 i;
	//remove the first 4 bytes of first frame
	for(i=4;i<20;i++)
		encryptedReceivedData[i-4] = multipleFramesRx.mulByteDiagRespData[i];
	encryptedReceivedData[16]=0;
	
}


/*********************************************************************//**
 *
 * Format Mul Bytes Frames
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

static void formatMulBytesIntoFrames(void)
{
 	u8 i,j;

	multipleFramesTx.frameInProgress = eIdle;
	multipleFramesTx.status = eSegmentedTxNotStarted;
	multipleFramesTx.minSepTime = MIN_SEP_TIME; //default sepration time is 127 ms
	multipleFramesTx.ID = diagnosticsCANId;
	
	multipleFramesTx.mulByteDiagRespData[0] = 0x10;//msb number bytes are ignored in this byte
	multipleFramesTx.mulByteDiagRespData[1] = (u8)multipleFramesTx.numBytes;
	
	//calculate number of frames
	i = 6;//6 bytes in first frame
	multipleFramesTx.numFrames = 1;//first frame
	while ( multipleFramesTx.numBytes > i )
	{
		multipleFramesTx.numFrames++;
		i=i+7; //seven bytes in each frame
	}
	j = multipleFramesTx.numFrames; //tracks number of frames to be formatted;
	for(i=2;i<8;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-2];
	if(j > 0)
		j--;
	multipleFramesTx.mulByteDiagRespData[8] = 0x21; //first consecutive frame
	for(i=9;i<16 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-3];
	
	if(j > 0)
		j--;
	
	multipleFramesTx.mulByteDiagRespData[16] = 0x22; //second consecutive frame
	for(i=17;i<24 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-4];

	if(j > 0)
		j--;
	
	multipleFramesTx.mulByteDiagRespData[24] = 0x23; //third consecutive frame //not used as yet
	for(i=25;i<32 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-5];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[32] = 0x24; //fourth consecutive frame //not used as yet
	for(i=33;i<40 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-6];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[40] = 0x25; //fifth consecutive frame //not used as yet
	for(i=41;i<48 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-7];

	if(j > 0)
		j--;

	multipleFramesTx.mulByteDiagRespData[48] = 0x26; //sixth consecutive frame //not used as yet
	for(i=49;i<56 && j>0;i++)
		multipleFramesTx.mulByteDiagRespData[i] = dataTxBuffer[i-8];
	
	if(testerAddress == REQ_PHYSICAL_SA1 )
		multipleFramesTx.ID |= REQ_PHYSICAL_SA1_L;
	else
		multipleFramesTx.ID |= REQ_PHYSICAL_SA2_L;
	
	multipleFramesTx.ID |= PREFERRED_SA;
	multipleFramesTx.dataReady = TRUE;

}


/*********************************************************************//**
 *
 * send Diagnostics Response
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void sendDiagnosticsResponse (void)
{
	u8 i;
	static u8 dataByteIndex;
	u32 ID;

	//if (commInfo.noResponse == TRUE || !HSCAN_GetStateTX())
	if (commInfo.noResponse == TRUE )
		return;
	
	if( diagnosticsResponse.bDataRdy == TRUE )
	{
		
		diagnosticsResponse.sMsgID.Identifier = diagnosticsCANId; 
		if(testerAddress == REQ_PHYSICAL_SA1 )
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA1_L;
		else
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA2_L;

		diagnosticsResponse.sMsgID.Identifier |= PREFERRED_SA;
		
		vTxMsgBufProcessing(diagnosticsResponse.sMsgID.Identifier,8,diagnosticsResponse.u8data,UDS_TX);
		diagnosticsResponse.bDataRdy = FALSE;
		//if reset diagnosticsTimer here then delay counter will be reset in case of  wrong message in tester present also
		//diagnosticsTimer = clock1msTicker;
		for(i=0;i<8;i++)
		{
			diagnosticsResponse.u8data[i] = 0;
			singleFrameRx.dataBytes[i]=0;
		}
	}

	if (multipleFramesTx.dataReady == TRUE )
	{
		
		// sending first frame
		if(	multipleFramesTx.frameInProgress == eIdle && multipleFramesTx.status == eSegmentedTxNotStarted)
		{
			dataByteIndex = 0;
			vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
			diagnosticsTimer = u32GetClockTicks();
			
			multipleFramesTx.serialNum++;
			dataByteIndex = 8 * multipleFramesTx.serialNum;
			multipleFramesTx.timeStamp = u32GetClockTicks();
			multipleFramesTx.status = eWait;
			multipleFramesTx.frameInProgress = eFirst;
		}
		// sending consecutive frames
		// min separation time over & has the flow control frame from receiver cleared to send
		else if ( ( u32GetClockTicks() - multipleFramesTx.timeStamp ) > multipleFramesTx.minSepTime 
			&& (multipleFramesTx.status == eClearToSend))
		{
			// if blocksize is zero, send all the frames
			if(!multipleFramesTx.blockSize)
			{
				if ( multipleFramesTx.serialNum < multipleFramesTx.numFrames )
				{
					vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
					diagnosticsTimer = u32GetClockTicks();
					multipleFramesTx.serialNum++;
					dataByteIndex = 8 * multipleFramesTx.serialNum;
					multipleFramesTx.timeStamp = u32GetClockTicks();
					multipleFramesTx.frameInProgress = eConsecutive;
					//this has to be there for system with longer main loop:otherwise  multiframe messages are not received
					if(multipleFramesTx.serialNum >= multipleFramesTx.numFrames )
					{
						multipleFramesTx.frameInProgress = eTxCompleted;
						multipleFramesTx.status = eStopped;
						multipleFramesTx.dataReady = FALSE;
					} 
				}//this may not be required since it has been done in the if loop above but let it stay
				else //if all the frames have been sent, stop sending
				{
					multipleFramesTx.frameInProgress = eTxCompleted;
					multipleFramesTx.status = eStopped;
					multipleFramesTx.dataReady = FALSE;
				}
			}//if blocksize has been specified by the receiver, send only that number of frames
			else if (	 multipleFramesTx.serialNum < multipleFramesTx.blockSize 
				&& multipleFramesTx.blockSize <= multipleFramesTx.numFrames
				)
			{
				dataByteIndex = 8 * multipleFramesTx.serialNum;				
				vTxMsgBufProcessing(multipleFramesTx.ID,8,&multipleFramesTx.mulByteDiagRespData[dataByteIndex],UDS_TX);
				diagnosticsTimer = u32GetClockTicks();
				multipleFramesTx.serialNum++;
				dataByteIndex = 8 * multipleFramesTx.serialNum;
				multipleFramesTx.timeStamp = u32GetClockTicks();
				multipleFramesTx.frameInProgress = eConsecutive;
				
				if(multipleFramesTx.blockSize == multipleFramesTx.numFrames)//if all the frames have been sent, stop sending
				{
					multipleFramesTx.frameInProgress = eTxCompleted;
					multipleFramesTx.status = eStopped;
					multipleFramesTx.dataReady = FALSE;
				}
			}
			//wait for next flow control
			else 
			{
				//multipleFramesTx.waitForResponse = TRUE;
				multipleFramesTx.status = eWait;
				multipleFramesTx.timeStamp = u32GetClockTicks();
			}
		}
		
		//if time is out since last consecutive frame was received,  stop sending
		else if (( u32GetClockTicks() - multipleFramesTx.timeStamp ) > NEXT_CAN_FRAME_TIMEOUT 
			|| multipleFramesTx.status == eOverflowAndTerminate )
		{
			multipleFramesTx.frameInProgress = eTerminate;
			multipleFramesTx.status = eStopped;
			multipleFramesTx.dataReady = FALSE;
		}
	

	}// if all the frames were sent or if data overflow has occured, reset the structure to stop sending 
	else if ( (( dataByteIndex >= MAX_DATA_LENGTH ) || multipleFramesTx.frameInProgress >= eTxCompleted ) 
		|| (multipleFramesTx.status == eWait && (u32GetClockTicks() - multipleFramesTx.timeStamp) >  NEXT_CAN_FRAME_TIMEOUT )
	)
	{
		multipleFramesTx.frameInProgress = eIdle;
		for(i=0;i< MAX_LENGTH; i++)
		{
			multipleFramesTx.mulByteDiagRespData[i]=0;
			dataTxBuffer[i]=0;
		}
		
		//multipleFramesTx.waitForResponse = FALSE;
		// if the request message was sent but no message was received for more than NEXT_CAN_FRAME_TIMEOUT
		multipleFramesTx.status = eSegmentedTxNotStarted;
		multipleFramesTx.serialNum = 0;
		//if( securitySequence == eRequestSeed )
			//securitySequence = eNone;
	}
	if(multipleFramesRx.frameInProgress == eFirst && multipleFramesRx.status == eWait)
	{
		if(testerAddress == REQ_PHYSICAL_SA1 )
			ID = multipleFramesRx.ID | REQ_PHYSICAL_SA1_L;
		else
			ID = multipleFramesRx.ID | REQ_PHYSICAL_SA2_L;

		ID |= PREFERRED_SA;
		frameConrolDataToSend[0] = 0x30;	
		vTxMsgBufProcessing(ID,8,&frameConrolDataToSend,UDS_TX);
		multipleFramesRx.status = eClearToSend;
		diagnosticsTimer = u32GetClockTicks();
	}
	else if(multipleFramesRx.frameInProgress == eTerminate && multipleFramesRx.status == eOverflowAndTerminate)
	{
		diagnosticsResponse.sMsgID.Identifier = multipleFramesRx.ID; 
		if(testerAddress == REQ_PHYSICAL_SA1 )
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA1_L;
		else
			diagnosticsResponse.sMsgID.Identifier |= REQ_PHYSICAL_SA2_L;

		diagnosticsResponse.sMsgID.Identifier |= PREFERRED_SA;
		
		frameConrolDataToSend[0] = 0x32;//overflow sent in Frame control frame
		vTxMsgBufProcessing(diagnosticsResponse.sMsgID.Identifier,8,&frameConrolDataToSend,UDS_TX);
		multipleFramesRx.status = eClearToSend;
		diagnosticsTimer = u32GetClockTicks();
	}

	if (multipleFramesRx.frameInProgress >= eTxCompleted )
	{
		//data has been processed, so clear the buffer
		for(i=0;i< MAX_LENGTH; i++)
			multipleFramesRx.mulByteDiagRespData[i]=0;
		multipleFramesRx.frameInProgress =  eIdle;
		multipleFramesRx.status = eReserved;
		multipleFramesRx.serialNum = 0;
		multipleFramesRx.dataReady = FALSE;
		for(i=0;i<8;i++)
			singleFrameRx.dataBytes[i]=0;
	}
	
}


/*********************************************************************//**
 * init Diagnostics Session
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

//Called in Events:: CPU_Reset
void initDiagnosticsSession(void)
{
	  setStatusDefaultSession(); //set to diagnostics to default session if watchdog reset has occured

}




/*********************************************************************//**
 * Reset Diagnostic Session call this after booting is over
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void resetDiagnosticSession(void)
{
	activeDiagnosticsSession = eDefaultSession;

}

/*********************************************************************//**
 *
 * Set Status Default Session
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setStatusDefaultSession(void)
{
	activeDiagnosticsSession = eDefaultSession;

}

/*********************************************************************//**
 *check session Time out
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void sessionTimeout(void)
{
	//after 3 failed attempts,  after waiting for 10s, reset all the parameters to allow fresh access
	if( securityAccessInfo.securityDelayOn && (u32GetClockTicks() - securityAccessInfo.securityAccessDelay > SECURITY_ACCESS_DELAY)) 
	{
		securitySessionTimeOutInit();		
	}
	//if there is no message exchange
	if(u32GetClockTicks() - diagnosticsTimer > NO_MESSAGE_TIMEOUT )
	{
		activeDiagnosticsSession = eDefaultSession;
		securitySessionTimeOutInit();
	}
	
}

/*********************************************************************//**
 *security Session Time Out Init
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void securitySessionTimeOutInit(void)
{
	u8 i;
	
	securityAccessInfo.securityDelayOn = FALSE;
	multipleFramesTx.dataReady = FALSE;
	multipleFramesTx.frameInProgress = eIdle;
	securityAccessInfo.serverLockStatus = eServerLocked;
	securityAccessInfo.securitySequence = eNone; //RequestSeed msg must come again,
	securityAccessInfo.securityAccessAttempts = 0;
	for(i=0;i< MAX_LENGTH; i++)
	{
		multipleFramesTx.mulByteDiagRespData[i]=0;
		dataTxBuffer[i]=0;
		multipleFramesRx.mulByteDiagRespData[i]=0;
	}
	multipleFramesTx.frameInProgress = multipleFramesRx.frameInProgress =  eIdle;
	multipleFramesTx.status  = eSegmentedTxNotStarted;
	multipleFramesTx.serialNum = multipleFramesRx.serialNum = 0;
	multipleFramesTx.dataReady = multipleFramesRx.dataReady = FALSE;
	multipleFramesRx.status = eReserved;

	
}

/*********************************************************************//**
 *
 * ECU rest with time
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

bool isItTimeToReset(void)
{

	u16 time;
	
    time =  setTimeForReset*1000;
    if(setTimeForReset > 0 && (u16GetClockTicks() - resetTimeTracker > time))
		return TRUE;
	else
		return FALSE;
		

}

/*********************************************************************//**
 *
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void diagnosticDataInit(void)
{

	securitySessionTimeOutInit();
	conditionNotCorrect = FALSE;
	testerAddress = REQ_PHYSICAL_SA1;
	//setTimeForReset = 0; // this will be initialized on wtd reset anyway
	
}

u16 u16GetRoutineControlServiceStatus(void)
{
	return RoutineControlStatusPending;
}

bool bIsECUResetByService(void)
{
	return (bECUResetServiceActive);
}

/*********************************************************************//**
 *
 * Set Condition not Correct
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setConditionNotCorrect(bool status)
{
	conditionNotCorrect = status;
}

/*********************************************************************//**
 *
 * Get Condition  Not Correct
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

bool getConditionNotCorrect(void)
{
	return conditionNotCorrect;
}

/*********************************************************************//**
 *
 * Set CriticalFunctionStatus
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void setCriticalFunctionStatus(bool status)
{
	criticalFunctionStatus = status;
}

/*********************************************************************//**
 * Get CriticalFunctionStatus
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

bool getCriticalFunctionStatus(void)
{
	return criticalFunctionStatus;
}

u16 u16getSoftwareVersion(void)
{
 return (u16ECUSoftwareVersionNum);
}




/* END TML_14229_UDS */
