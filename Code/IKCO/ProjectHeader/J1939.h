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
/*****************************************************************************
**  Module:J1939.h
**
** ---------------------------------------------------------------------------
**  Description:
**      Header file for J1939.h
**
*****************************************************************************/
#ifndef _J1939_H_
#define _J1939_H_

// constant declarations -----------------------------------------------------

#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "customTypedef.h"

// constant declarations -----------------------------------------------------
#define NO_OF_DTC_CODE        		70U //maximum is 64 DTCs

#define BADDATA						0xFEU 				
#define ERRORDATA					0XFFU

#define BADDATA_2BYTE				0xFE00U 
#define BADDATA_4BYTE				0xFE000000U

#define TEMP_UND_RANGE            	0x50U    //J1939 value at temp 60 deg 
#define TEMP_OVR_RANGE            	0xA1U    //J1939 value at temp >120 deg 

#define OAT_UND_RANGE            	0x2220U    //J1939 value at 0 deg temp
#define OAT_OVR_RANGE            	99U    	  // 99 deg Temp

#define RX_QUE_SIZE      			30U		//Buffer for receive CAN msg
#define ERR_OK           			0U		//Not used            

// public interface ---------------------------------------------------------
// (Not used ) Required ISO PGN List 
#define PGN60928                60928U           // TxRx 0xEE00 ISO Adress claim
#define PGN59392                59392U           // TxRx 0xE800 ISO Acknowledgement


// (Not used ) Proprietary PGN List
#define PGN_USER_INTERFACE      65401U           // Rx Proprietary PGN used for calibration
#define PGN_USER_INTERFACE_ext  0x18FF7900U      // 29 bit Tx Proprietary used for calibration

#define J1939_NULL_ADDRESS    	254U

#define J1939_GLOBAL_ADDRESS    255U
#define J1939_MAX_PF_PDU1FORMAT 239U             // From J1939-21 section 5.2.5 "PDU Specific (PS)
                                                // PDU1 range is 0-239 and is for PDU destination format
                                                // PDU2 range is 240-255 and is for PDU group extension values
//PGN transmmit for request
#define PGN65257				65257U			// AFE
#define PGN65257_ext            0x18FEE900U		// 29 bit ID priority 7
#define PGN65219				65219U			// TRANSMISSION RANGE LOW
#define PGN65219_ext            0x1CFEC300U		// 29 bit ID priority 7
#define PGN65088                65088U			//Trailer
#define PGN65088_ext            0x18FE4000U      // 29 bit ID priority 6


//Request PGN
#define PGN59904                59904U           // Rx 0xEA00 ISO Request PGN (data length = 3)
#define PGN59904_0x00_ext		0x18EA0017U



typedef enum
{
	eJOdoDataTx =0U,
	eJHourDataTx,
	eJOdoTripATx,
	eJTripAFuelEffKmTx,
	eJTripAFuelEffHrsTx,
	eJHourTripATx,	
	eJOdoTripBTx,
	eJTripBFuelEffKmTx,
	eJTripBFuelEffHrsTx,
	eJHourTripBTx,
	eJFuelLevelTx,
	eJDTETx,
	ejBreakWearTx,
	ejClutchWearTx,	
	ejDistanceTONextServiceTx,
	eJAfeTx,			//Request for AFE 
	eJTripResetTx,
	eJAirPressFrontTx,
	eJAirPressRearTx,
	eJSeatBeltTx,
	eJBattChargingTx,
	eJLiftAxelUpTx,
	eJGenIllTx,
	eJBattCharging2Tx,
	eJTiltCabTx,
	eJAirFilterCloggedTx,
	eJABSMalfunctionTx,
	eJServiceIndicatorTx,
	eJCapacitiveFuelTx,
	eJDualFuelLevelTx,
	eNumofTxDataTypes
} eTxDataType;

typedef enum
{
	eTx65361 = 0U,
	eTx65362,
	eTx65363,	
	eTx65364,
	eTx65365,
	eTx59904,
	eTx56832,
	eTx65198,	
	eTx65276,
	eNoOfPgnTx	
} tPGN_TxIndexerType;


typedef enum
{
    eJWaterInFuelLamp = 0U,
	eJSCR_SPN5825,
    eJGearShiftUpDown,
    eJEnginePreHeat,
    eJCoolantTemp,
    eJLowDEFLamp,
    eJNuetral,
    eJFrontRearLampSPN2388,
    eJFrontRearLampSPN2390,
    eJPowerTakeOff,
    eJCruiseControl,
    eJDPF, 
    eJCheckEngineSPN624,
    eJCheckEngineSPN3040,
    eJMilSPN1213,
    eJMilSPN3038,
    eJDM1DTC_spn1,
    eJDM1DTC_spn2,
    eJDM1DTC_fmi,
    eJMilSPN5080,
    eJRpm,
    eJTransFluidTemp,
    eJHEST,
    eJDPFC1_SPN3702,
    eJDPFC1_SPN3703,
    eJDPFC1_SPN3704,
    eJDPFC1_SPN3705,
    eJDPFC1_SPN3707,
    eJDPFC1_SPN3708,
    eJDPFC1_SPN3710,
    eJDPFC1_SPN3711,
    eJDPFC1_SPN3712,
    eJDPFC1_SPN3716,
    eJDPFC1_SPN4175,
    eJDPFC1_SPN3700,
    eJDefLamp,
    eJStopEngine623,
    eJStopEngine3039, 
	eJETC7,
    eJCreep,
    eJPower,
    eJAuto_Manual_Eco,
    eJCrawl,
    eJTurnLeftRightTurn,   
    eJHighBeam,
    ejTrailer,
    ejParkingBrake,
    eJLowOilLamp,
    eJEscMil,
    eJAbsTrailer,
    eJTransRangeLow,
    eJExhaustBreak,
    eJAfe,
    eJFuel,
    eJCheckEngineDM1_624,
    eJCheckEngineDM1_3040,
    eJStopEngineDM1_623,
    eJStopEngineDM1_3039,
    eJMilDM1_1213,
    eJMilDM1_3038,
    eJCheckTrance,
    eJCheckTranceDM1,
    eJGearValue_0x03,
    eJManualGear_0x21,
    ejOat,
    eJEngineModeSelection,
    eJSpeedoSpn191,
    eJEngineOilPressure,
    eJRetarderRequest,
    eJAbsMil_1438,
    eJTPDT_Protocol,
    ejLearnFailDM1SPN1,
    ejLearnFailDM1SPN2,
    ejLearnFailDM1FMI,
    eJAbsTrailer65378,
	eJShiftInhibit,
    eJCheckEng5078,
    eJStopEng5079,
    eJAirFilterClogged65303,
    eJBatteryCharging1,
    eJBatteryCharging2,
    ejLiftAxelFault,   
    eJBatteryWeak2,
    eJTPMS_LampDM1_624,
    eJTPMS_Lamp,
    ///////////////////////////////////////////////////
    eJHaltRequest,
    eJRampEngaged,
    eJKneelingFront,
    eJKneelingRear,
    eJKneelingBoth,
    eJEngineOilLow,
    eJEngineCoolantLow,
    eJEmergencyExit,
    eJHaltRequestPriority,
    eJSteeringAdjust,
    eJBuzzerDisable,
    eJRearFlapOpen,
    eJSideFlapOpen,
    eJKneelingRight,
    eJKneelingLeft,
    ///////////////////////////////////////////////////
	eJDoorOpen,
	///////////////////////////////////////////////////
    eJHillHolder,
    eJIILFaultDiag,
    eJAirFilterClogged5086,
    eJCapacitiveAft2,
    eJLowBrakeFluid2931,
    eNumofRxDataTypes  
} eRxDataType;

typedef enum
{
     
	eRx65279_0x00 = 0U,	
	eRx65279_0x21,
	eRx65252,
	eRx65110,
	eRx61445,
	eRx65088,	
	eRx65265_0x00,
	eRx64892,
	eRx65226,
	eRx64775,
	eRx61444,	 
	eRx65262,
	eRx65098,
	eRx64917,
	eRx61441,
	eRx65103,
	eRx65219,
	eRx65257,
	eRx65276,
	eRx60160,
	eRx60160_0x03,
	eRx60160_0x33,
	eRx65226_0x03,
	eRx65226_0x33,
	eRx61445_0x03,
	eRx61445_0x21,
	eRx65269,
	eRx64712,
	eRx61442,
	eRx65263,
	eRx61440,
	eRx65303,
	eRx65303_0xE1,
	eRx65378,
	eRx65102,
	eRx64964,
	eRx64998,
	eNoOfPgnRx
	
} tPGN_RxIndexerType;                                                

// Identifier format
// bits | 28 27 26 | 25 | 24 | 23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 09 08 | 07 06 05 04 03 02 01 00 |
//      | priority | R  | DP | PDU format (PF)         | PDU specific (PS)       | Source address          |
//                 |<-------------------------- PGN Id ------------------------->|

//NEC MCU have a architecture little endian so change stucture according to it.
typedef union
{
    u32  Identifier;
    struct
    {
	u8 SourceAddress;            	// 0 - 7
	u8 PduSpecific;              	// 8 - 15
	u8 PduFormat;                	// 16 - 23
        struct
        {
            u8 DataPage       :1;    // 24
            u8 Reserved       :1;    // 25
            u8 Priority       :3;    // 26 - 28
            u8 UnusedMSB      :3;    // 29 - 31
        } sBitField;
        
    } sIdField;
} sJ1939_Id_Struct_Type;

// J1939 message buffer structure
typedef struct
{
    sJ1939_Id_Struct_Type   sMsgID;           // Message ID
    u32                     u32TimeStamp;     // Time of message reception
    u32                     u32ValidDataTS;   // Time stamp of last valid data
                                              // The message processor will copy the timestamp into this variable
                                              // if at least one of the datums was valid
    bool                    bDataRdy;         // True = fresh data for processing
    u8                      ucNumofRxBytes;   // 8 or less received
    u8                      u8data[8];
	bool				 	bDataOutDate;
} sJ1939_RX_MSG_TYPE;

typedef struct 
{
	u32 SPNValue;
	u8 FMICode;
}DTCdata;


extern sJ1939_RX_MSG_TYPE   sJ1939RxQueue[RX_QUE_SIZE];
extern sJ1939_RX_MSG_TYPE   J1939RxData[eNoOfPgnRx];


// Global prototypes --------------------------------------------------------

/// @addtogroup J1939_LIBRARY
/// @{
 
/// @defgroup J1939HeaderLib Global functions used in j1939 file
//// 
///   
/// @addtogroup J1939HeaderLib
/// @{

void vResetDTCParameters(void);
u8 u8GetDTCStatus(void);
u8 u8GetDTCCount(void);
u8 u8GetDTCSA(void);
u8 u8GetDtcFramNum();
u16 u16GetDtcTotalBytes();
void vJ1939Init(void);
void vJ1939_ServiceManager(void);
bool bProcessISORequest(u32 PGN);
void vPutPGNdata(eTxDataType dtype, u32 rawData, s16 overrange);
u8 u8GetBitData(eRxDataType dType);
u32  u32GetPGNdata(eRxDataType dType);
void vJ1939OnFullRx(void);
bool bGetPGNTimeOut(eRxDataType dType);
bool bGetPGNDataReady(eRxDataType dType);
void ClearPGNDataReady(eRxDataType dType);
void vJ1939_TxMsgEnableDisable(tPGN_TxIndexerType eTxPgn , bool EnableFlag);
void vJ1939TxInit(void);
void vClearDTC(void);
bool bGetSPNDataReady(eRxDataType dType);
void vClearSPNDataReady(eRxDataType dtype);
void vJ1939_TxSendOnDemandEnable(tPGN_TxIndexerType eTxPgn );



/// @} // endgroup J1939HeaderLib

/// @} // endgroup J1939_LIBRARY


#endif

