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

#ifndef __TellTales_H
#define __TellTales_H
#include "customTypedef.h"

// constant declarations -----------------------------------------------------

#define LIFE_CRITICAL_ALARM		0x01
#define ENGINE_CRITICAL_ALARM	0x02
#define GENRAL_ALARM			0x04
#define TIC_ALARM				0x08
#define TOC_ALARM				0x10
#define CONTINUE_ALARM			0x20
#define BEEP_ALARM				0x40
#define BUZZER_STOP				0x00

#define IO_CONTROL_LAMP_BYTE 	( 6U + 1U ) // 1 extra byte for control option 


typedef enum
{
	eAirPressure = 0,
	eHydraulic,
}eBRAKE_VARIANT;

typedef union
{
	u8 IoControlByte[6];
	
	struct
	{
		struct
		{
			bool bTurnLeft:1;//LSB
			bool bTransRangeLow:1;//On LCD
			bool bTrailer:1;
			bool bHighBeam:1;
			bool bSeatBelt:1;
			bool bEscMil:1;		//On LCD
			bool bCoolantTempWarning:1;
			bool bBrakeWear:1;//MSB	//On LCD
			
		}Byte1;

		struct
		{
			bool bClutchWear:1;//LSB
			bool bRetarder:1;				//On LCD
			bool bVehicleOverSpeedoWarning:1;
			bool bAirPressureWarning:1;
			bool bFuelGaugeWaning:1;
			bool bBatterCharging1:1;
			bool bLowBrakeFluid:1;
			bool bMil:1;//MSB
				
			
		}Byte2;	

		struct
		{
			bool bTiltCabLock:1;//LSB
			bool bParkBrake:1;
			bool bTurnRight:1;
			bool bOilPressureWarning:1;
			bool bAirFilterClogged:1; 	//On LCD
			bool bNeutral:1;
			bool bCheckEngine:1;
			bool bWaterInFuel:1;//MSB		//On LCD
			
		}Byte3;	

		struct
		{
			bool bAbsMil:1;			//LSB //On LCD
			bool bFrontRearFogLamp:1;
			bool bCheckTransLamp:1;
			bool bGlowPlug:1;					//On LCD
			bool bPowerTakeOff:1;
			bool bAbsMilTrailer:1;
			bool bTranceFluid:1;
			bool bGearShiftUp:1;		//MSB
			
		}Byte4;
		struct
		{
			bool bGearShiftDownLamp:1;//LSB
			bool bStopEngineLamp:1;
			bool bExhaustBrakeLamp:1;
			bool bCruiseControlLamp:1;
			bool bDefLamp:1;
			bool bScrFaultLamp:1;				//On LCD
			bool bDpfilterLamp:1;					//On LCD
			bool bHestLamp:1;//MSB
			
		}Byte5;		
		struct
		{
			bool bTpmsLamp:1;//LSB
			bool bLiftControllerAxleLamp:1;
			bool bBatteryCharging2Lamp:1;
			bool bEngineOverSpeedWarning:1;
			bool bEconomyLampAmber:1;
			bool bEconomyLampGreen:1;
			bool bEconomyLampRed:1;
			bool bAllTellON:1;//MSB
			
		}Byte6;				
		
	}IoControlLampbytes;
} IO_CONTROL_TYPE;

extern IO_CONTROL_TYPE InputOutputLampControl;


// Global declarations --------------------------------------------------------


// Global prototypes ---------------------------------------------------------

/// @addtogroup TELL_TALE_LIBRARY
/// @{
 
/// @defgroup TellTaleHeaderLib Global functions used in Tell tale
//// 
///   
/// @addtogroup TellTaleHeaderLib
/// @{
bool b1HzToggleFlash(void);

bool bAbsFailure(void);

bool bEscMilLamp(void);     
bool bBrakeWearLamp(void);       
bool bRetarderLamp(void); 
bool bWaterInFuelLamp(void);  
bool bAbsMilLamp(void);       
bool bCheckTransmissionLamp(void);  
bool bEnginePreHeatLamp(void);   
bool bExhaustBreakLamp(void);   
bool bDefLevelLamp(void);  
bool bScrLamp(void);        
bool bDpfLamp(void);      
void vControlTellTales(void);
void vAllLampsOff(void);
void vEconomyLedOFF(void);
void vAllLampsOn(void);
void vLedPrevStateInit(void);
u8 u8GetGearTellTale(void);
void vBuzzerControl(void);
void vBuzzerDisable(void);
void vBuzzerEnable(void);
void vSetBuzzerSoundType(u8);
bool bClutchWearLampStatus(void);
bool bBrakeWearLampStatus();
void vCheckLamps(void);

bool bGetSeatBeltStatus(void);
bool bGetBatChargingStatus(void);
bool bGetLiftAxelStatus(void);
bool bGetGenIllStatus(void);
bool bGetBatCharging2Status(void);
bool bGetTitlCabStatus(void);
bool bGetAirFilterCloggedStatus(void);
bool bGetAbsMilTrailerStatus(void);
bool bGetCapacitiveFuelStatus(void);
bool bGetLampBlinkingFlagstatus();
void vLampsWorkInSleepMode();
void vClearAllLampsBuzzerFlag();
void vIoControlTellTaleLedBased(void);
bool bGetMilLampMode4Status(void);
bool bGetMilLampMode3Status(void);
bool bGetCheckEngineStatus(void);
bool bGetStopEngineStatus(void);
bool bGetScrLampStatus(void);
bool bGetPtoLampStatus(void);



/// @} // endgroup TellTaleHeaderLib

/// @} // endgroup TELL_TALE_LIBRARY

#endif

