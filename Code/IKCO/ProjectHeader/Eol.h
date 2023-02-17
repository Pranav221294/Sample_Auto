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
#ifndef __Eol_H
#define __Eol_H


/******************************************************************************************************
**************************************EOL DATA-Byte Size***********************************************
						CHANGE THIS DATA TO ALTER EOL VARIABLE SIZE.
*******************************************************************************************************/
#define	VEHICLE_TYPE_BYTES					5U
#define	SPEED_SENSOR_BYTES					5U
#define	FE_INDICATOR_THRESH_BYTES			4U
#define	AIR_PRESSURE_OPTION_BYTES			8U
#define	COOLANT_TEMP_OPTION_BYTES			3U
#define	AFE_OPTIONS_BYTES					2U
#define	LOW_OIL_PRESSURE_THRESH_BYTES		2U
#define	DISPLAY_SCREEN_CONFIG_BYTES			2U
#define	TELLTALES_OPTION_BYTES				4U
#define	BUZZER_OPTION_BYTES					3U
#define ENGINE_OVER_SPEED_BYTES				2U
#define DTE_MUL_FACTOR_BYTE					2U
#define ODO_WRITE_BYTES						3U
#define SERVICE_BYTES						3U
#define FUEL_TANK_CAP_LEN					2U
#define CAN_SPEEDO_FACTOR_LEN				6U



#define	DATE_OF_LAST_PROGRAMMING_BYTES		4U
#define	VEHICLE_ID_BYTES					17U
#define	VARIANT_DA_SET_ID_BYTES				5U	//5th byte for eol pending
#define	VEHICLE_CONFIG_NUMBER_BYTES			15U
#define	PARAMETER_PART_NUMBER_BYTES			17U
#define	PROGRAMMING_SHOP_CODE_BYTES			5U


#define	ECU_SERIAL_NUMBER_BYTES				12U
#define	ECU_PART_NUMBER_BYTES				12U
#define	TML_ECU_HARDWARE_NUMBER_BYTES		15U
#define	TML_CONTAINER_PART_NUMBER_BYTES		12U
#define	TML_SOFTWARE_CALIBERATION_ID_BYTES	2U
#define	TML_ECU_SOFTWARE_NUMBER_BYTES		15U
#define	REPROGRAMMING_COUNTER_BYTES			2U
#define	ECU_HARDWARE_VERSION_BYTES			2U
#define IIL_SVN_NUMBER_BYTES				4U
#define IIL_PCB_NUMBER_BYTES				8U


typedef enum
{
	eDisable = 0U,
	eEnable,
}eEnable_Disable_Type;

typedef enum
{
	eCan = 0U,
	eAnalouge,
}eSignal_Type;

typedef enum
{
	eDlcc1 = 0U,
	eDm1,
}eDLCC1_DM1_Type;


typedef enum
{
    e250 = 1U,
    e500,
    e1000,
    eNumOfBauds
} eBAUD_RATE;



/**********************************************************************/
/*	VEHICLE_OPTION_CONTENT */
/**********************************************************************/

typedef union
{	
	u8 InputTypeByte[5];
	struct
	{
		struct
		{
			u8 bVehicleSpdBias :1;	//LSB
			u8 bFuelGauge :1;
			u8 bVehicleSpeedInput :1;
			u8 bOilGauge :1;
			u8 bTurnLeft:1;
			u8 bTrailer:1;
			u8 bTiltCab:1;
			u8 bParkBrake:1;	//MSB
			
		}Byte1;

		struct
		{
			u8 bEscMil :1;	//LSB
			u8 bTurnRight:1;
			u8 bTransRangeLow:1;
			u8 bLowBrakeFluid:1;
			u8 bClutchWear:1;
			u8 bRetarder:1;
			u8 bMil:1;
			u8 bLowOilPressure:1;	//MSB
			
		}Byte2;	

		struct
		{
			u8 bAirFilterClogged :1;	//LSB
			u8 bHighBeam :1;
			u8 bNeutralTellTaleInputType :1;
			u8 u8AfeUnitType:2;
			u8 u8TransmissionInputType:2;
			u8 bServiceType:1;
			
		}Byte3;	

		struct
		{
			u8 bFuelSensor:1; //LSB
			u8 bLGearDisplay:1;
			u8 bSpeedorequiredwithPTO:1;
			u8 bDlcc1Dm1Selection:1;
			u8 bDialIlluSelection:1;
			u8 bBatteryCharging1:1;
			u8 res7:1;
			u8 res8:1;	//MSB

		}Byte4;

		struct
		{
			u8 u8BrakeVariant;
			
		}Byte5;
	}Inputsbytes;
}VEHICLE_OPTION_CONTENT;




/**********************************************************************/
/*	SPEED_SENSOR */
/**********************************************************************/

typedef struct
{	
	u16 u16PulsesPerKm;
	u8 u8OverSpeedWarning;
	u8 u8BiasFactor;
	u8 u8SpeedoOffset;
	
}SPEED_SENSOR;



/**********************************************************************/
/*	ENGINE_OVER_SPEED_ WARNING */
/**********************************************************************/

extern u16 u16EolEngineOverSpeed;

/**********************************************************************/
/*	FE_DATA  */
/*	Fuel Efficiency */
/**********************************************************************/

typedef struct
{
	u16 THRESH1;
	u16 THRESH2;
	u16 THRESH3;
	u16 THRESH4;
	
}FE_DATA;



/**********************************************************************/
/*	AIR_PRESSURE_BYTES  */
/**********************************************************************/

typedef struct
{
	u8 u8LowAirPressureOnThresh;
	u8 u8LowAirPressureOffThresh;
	u8 u8APNumOfBars;
	u8 u8OhmBar6;
	u8 u8OhmBar8;
	u8 u8OhmBar10;
	u8 u8OhmBar12;
	u8 u8OhmBar15;
	
}AIR_PRESSURE;

/**********************************************************************/
/*	COOLANT_TEMP_BYTES  */
/**********************************************************************/

typedef struct
{
	u8 u8CoolantTempOnThresh;
	u8 u8CoolantTempOffThresh;
	u8 u8CoolantTempRedZoneStart;
	
}COOLANT_TEMP;


/**********************************************************************/
/*	AFE_DATA_OPTIONS  */
/**********************************************************************/

typedef struct 
{
	u8 u8AfeMinDistance;
	u8 u8AfeMulFactor;
	
}AFE_DATA_OPTIONS;


/**********************************************************************/
/*	DTE_DATA  */
/* u8DteMulFactor */
/**********************************************************************/
extern u8 u8EolDteMulFactor;

/**********************************************************************/
/*	SERV_HOUR  */
/**********************************************************************/
typedef struct
{
	
	u32		u32PopUpInterval;
	u32 	u32FirstServiceDue;	
	u32 	u32ServiceInterval;
}SERVICE_REMINDER;


/**********************************************************************/
/*	LOW_OIL_PRESSURE */
/**********************************************************************/

typedef struct
{
	u8 u8LowOilPressOnThresh;
	u8 u8LowOilPressOffThresh;
	
}LOW_OIL_PRESSURE;


/**********************************************************************/
/*	LCD_SCREEN_CONFIG */
/**********************************************************************/

typedef union
{	
	u8 LcdScreenConfig[2];
	struct
	{
		struct
		{
			u8 bDisplayTPMS :1;	//LSB			
			u8 bDisplayServHrSymbol:1;
			u8 bDisplayDte :1;
			u8 bDisplayOat :1;
			u8 bDisplayAfe :1;
			u8 bDisplayDef:1;
			u8 bDisplayEngModeSel:1;
			u8 bDisplayOilPress:1;	//MSB	
			
		}Byte1;

		struct
		{
			u8 bDisplayBattery :1;	//LSB
			u8 bAirPressureBar:1;
			u8 bTransmissionDisplay:1;
			u8 bDisplayEngineModeLightHeavy:1;
			u8 res5:1;
			u8 res6:1;
			u8 res7:1;
			u8 res8:1;	//MSB
			
		}Byte2;	
		
	}LcdScreenConfigBytes;
	
}LCD_SCREEN_CONFIG;


/**********************************************************************/
/* TELL_TALE_ENABLE */
/**********************************************************************/

typedef union
{
	u8 ControlByte[4];
	
	struct
	{
		struct
		{
			u8 bTransRangeLow:1;
			u8 bTrailor:1;
			u8 bEscMil:1;		
			u8 bRetarder:1;
			u8 bTiltCab:1;
			u8 bFuelEcoEnableGreenLed:1;
			u8 bFuelEcoEnableAmberLed:1;
			u8 bFuelEcoEnableRedLed:1;//MSB
			
		}Byte1;

		struct
		{
			u8 bAirFilterClogged :1;//LSB
			u8 bNeutral :1;
			u8 bWaterInFuel:1;
			u8 bFrontRearFog:1;
			u8 bCheckTrans:1;
			u8 bGlowPlug:1;
			u8 bPowerTakeoff:1;
			u8 bAbsMilTrailer:1;			//MSB
			
		}Byte2;	

		struct
		{
			u8 bTransFluidTemp :1;	//LSB
			u8 bGsa :1;
			u8 bLowBreakFluid :1;
			u8 bStopEngine :1;
			u8 bExhaustBrake :1;
			u8 bCruiseControl :1;
			u8 bSeatBelt :1;
			u8 bDpf:1;				//MSB
			
		}Byte3;	

		struct
		{
			u8 bHest:1;				//LSB
			u8 bLowAirPressure:1;
			u8 bScr:1;
			u8 bAbsMil:1;
			u8 bClutchWear:1;
			u8 bBrakeWear:1;
			u8 bBatteryCharging2:1;
			u8 bLiftAxelControllerMil:1;	//MSB
			
		}Byte4;
		
	}Enablebytes;
} TELL_TALE_ENABLE;

/**********************************************************************/
/* BUZZER_ENABLE */
/**********************************************************************/

typedef union
{
	u8 BuzzerByte[3];
	
	struct
	{
		struct
		{
			u8 bFrontAirPressure:1;
			u8 bRearAirPressure:1;
			u8 bSeatBelt:1;
			u8 bTiltCab:1;
			u8 bHighTempWarning:1;
			u8 bLowFuelWarning:1;
			u8 bLowDef:1;
			u8 bLowOilPressure:1;//MSB
			
		}Byte1;

		struct
		{
			u8 bStopEngine :1;//LSB
			u8 bTransFluid :1;
			u8 bVehicleOverSpeed:1;
			u8 bGearUp:1;
			u8 bGearDown:1;
			u8 bBrakeWear:1;
			u8 bParkBrake:1;
			u8 bTurnRight:1;			//MSB
			
		}Byte2;	

		struct
		{
			u8 bTurnLeft :1;		//LSB
			u8 bEngineOverSpeed :1;
			u8 bLowBrakeFluid :1;
			u8 res4 :1;
			u8 res5 :1;
			u8 res6 :1;
			u8 res7 :1;
			u8 res8 :1;				//MSB
			
		}Byte3;	
		
	}Enablebytes;
} BUZZER_ENABLE;

extern u8 u8OdoWrite[3];

extern BUZZER_ENABLE sBuzzerEnable;
extern TELL_TALE_ENABLE sIndicatorEnable;
extern LCD_SCREEN_CONFIG sLcdScreenConfig;
extern LOW_OIL_PRESSURE sLowOilPressThresh;
extern AFE_DATA_OPTIONS sAfeDataOptionBytes;
extern COOLANT_TEMP sCoolantTempBytes;
extern AIR_PRESSURE sAirPressureBytes;
extern FE_DATA sFeData;
extern SPEED_SENSOR sSpeedSensor;
extern VEHICLE_OPTION_CONTENT sIndicatorInputType;
extern SERVICE_REMINDER sServReminder;



/********************************************************************************************************
**************************************Extern CAN EOL Parameters******************************************
							REINIT THESE VARIABLES IN OTHER SOURCE FILES.
					    CHANGE BYTE SIZE OF EOL VARIABLES IN eel_descriptor.h
*********************************************************************************************************/

extern u8 u8VehicleOptionContent[VEHICLE_TYPE_BYTES];
extern u8 u8VsPulsPerRev[SPEED_SENSOR_BYTES];

extern u8 u8EngineOverSpeedWarning[ENGINE_OVER_SPEED_BYTES];				// single byte data, value will get multiplied by 25

extern u8 u8FeIndicatorThresh[FE_INDICATOR_THRESH_BYTES];	// Each byte gets multiplied by 25
extern u8 u8AirPressureOption[AIR_PRESSURE_OPTION_BYTES];
extern u8 u8CoolantTempOption[COOLANT_TEMP_OPTION_BYTES];

extern u8 u8AfeOptions[AFE_OPTIONS_BYTES];		// byte 0 gets multiplied by 0.1 and Byte 1 with 0.01
extern u8 u8DteMulFactor[DTE_MUL_FACTOR_BYTE];						// single byte data, value will get multiplied by 0.01

extern u8 u8LowOilPressureThresh[LOW_OIL_PRESSURE_THRESH_BYTES];
extern u8 u8DisplayScreenConfig[DISPLAY_SCREEN_CONFIG_BYTES];
extern u8 u8TelltalesOption[TELLTALES_OPTION_BYTES];
extern u8 u8BuzzerOption[BUZZER_OPTION_BYTES];
extern u8 u8OdoWriteData[ODO_WRITE_BYTES];

extern u8 u8FuelTankCapacity[FUEL_TANK_CAP_LEN];
extern u8 u8CanSpeedoFactor[CAN_SPEEDO_FACTOR_LEN];

//***************************************ECU PARAMETERS*************************************************//

extern u8 u8DateOfLastProgramming[DATE_OF_LAST_PROGRAMMING_BYTES];
extern u8 u8VehicleId[VEHICLE_ID_BYTES];
extern u8 u8VariantDaSetId[VARIANT_DA_SET_ID_BYTES];
extern u8 u8ParameterPartNumber[PARAMETER_PART_NUMBER_BYTES];
extern u8 u8ProgrammingShopCode[PROGRAMMING_SHOP_CODE_BYTES];
extern u8  u8VehicleConfigNumber[VEHICLE_CONFIG_NUMBER_BYTES];// = {'P','R','O','G','R','A','M','C','O','U','N','T','O','F','T'};





// Global prototypes --------------------------------------------------------

extern void vSetSpeedoPulses(u16);
extern void vSetFuelGaugeInputType(bool);

/// @addtogroup EOL_LIBRARY
/// @{
/// @defgroup EOLLib  functions used 
//// 
///   
/// @addtogroup EOLLib
/// @{


void vEolInit(void);
u8 u8GetVehicleType(void);
u8 u8GetTransmissionType(void);
bool bGetTransmissionEnable(void);

u16 u16GetFeededPulse(void);

void vSetEOLVehicleOptionContentData(void);
void vSetEOLSpeedSensorData(void);
void vSetEOLEngOverSpeedData(void);
void vSetEOLFeData(void);
void vSetEOLAirPressData(void);
void vSetEOLCoolantTempData(void);
void vSetEOLAfeData(void);
void vSetEOLDteMulFactorData(void);
void vSetEOLDefLevelData(void);
void vSetEOLServHrReminderData(void);
void vSetEOLLowOilPressureData(void);
void vSetEOLLcdScreenConfigData(void);
void vSetEOLTellTaleControlData(void);
void vSetEOLBuzzerSetting(void);
eBAUD_RATE eGetCanBaudRate(void);
void vSetCanBaudRate(eBAUD_RATE);
u8 u8GetEolPendingStatus(void);



/// @} // endgroup EOLLib

/// @} // endgroup EOL_LIBRARY

#endif

