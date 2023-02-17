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
#include "r_cg_macrodriver.h"
#include "customTypedef.h" 
#include "Eol.h" 
#include "Speedo.h"
#include "Tacho.h"
#include "dataflash.h"
#include "r_cg_userdefine.h"

#define RPM_RES			25
#define SERV_HRS_RES	50
#define SERV_DIST_RES	1000

static u8 u8FuelType = 0;
static u8 u8EolDteMulFactor = 0;
static u16 u16EolEngineOverSpeed = 0;
static u8 u8OdoWrite[3] = {0,0,0};
eBAUD_RATE eBaudRateIndex = e250;



//********************************EOL PARAMETER INIT AS PER ANNEXURE A**********************************//

u8  u8VehicleOptionContent[VEHICLE_TYPE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFE, 0x00}; //0102
u8	u8VsPulsPerRev[SPEED_SENSOR_BYTES] = {0x1D, 0x28, 100, 0x80,0x02};	//012C
u8	u8EngineOverSpeedWarning[ENGINE_OVER_SPEED_BYTES] = {80 , 0x00};	//012D
u8	u8FeIndicatorThresh[FE_INDICATOR_THRESH_BYTES] = {0x28, 0x30, 0x44, 0x50}; //0139
u8	u8AirPressureOption[AIR_PRESSURE_OPTION_BYTES] = {105, 116, 2, 116, 150, 168, 190, 210 }; //0141
u8	u8CoolantTempOption[COOLANT_TEMP_OPTION_BYTES] = {0x69, 0x64, 0x69}; //0135
u8	u8AfeOptions[AFE_OPTIONS_BYTES] = {0x05, 0x64}; //0146
u8	u8DteMulFactor[DTE_MUL_FACTOR_BYTE] = {0x64,0x00}; //0147
u8	u8LowOilPressureThresh[LOW_OIL_PRESSURE_THRESH_BYTES] = {0x6f, 0x73};
u8	u8DisplayScreenConfig[DISPLAY_SCREEN_CONFIG_BYTES] = {0xff, 0xff}; //0106
u8	u8TelltalesOption[TELLTALES_OPTION_BYTES] = {0xff, 0xff, 0xff, 0xff};//0103
u8	u8BuzzerOption[BUZZER_OPTION_BYTES] = {0xff, 0xff, 0xff}; //0149
u8	u8FirstService[SERVICE_BYTES] = {0x00,0x9c, 0x40}; //0151
u8	u8ServiceInterval[SERVICE_BYTES] = {0x01,0x86, 0xA0}; //0152
u8	u8ServiceThreshold[SERVICE_BYTES] = {0x00,0x01, 0xF4};//0153
u8	u8FuelTankCapacity[FUEL_TANK_CAP_LEN] = {0,0}; //0134//first byte Tank capcity and second Dead Valume
u8	u8CanSpeedoFactor[CAN_SPEEDO_FACTOR_LEN] = {0,0,0x0A,0,0x03,0xE8}; //0155

//***************************************ECU PARAMETERS*************************************************//

u8	u8DateOfLastProgramming[DATE_OF_LAST_PROGRAMMING_BYTES] = {0x00, 0x00, 0x00, 0x00};

u8	u8VehicleId[VEHICLE_ID_BYTES] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

u8	u8VariantDaSetId[VARIANT_DA_SET_ID_BYTES] = {0x00, 0x00, 0x00, 0x00, 0x00};

u8	u8ParameterPartNumber[PARAMETER_PART_NUMBER_BYTES] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

u8	u8ProgrammingShopCode[PROGRAMMING_SHOP_CODE_BYTES] = {'0', '0', '0', '0', '0'};

u8  u8VehicleConfigNumber[VEHICLE_CONFIG_NUMBER_BYTES] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

u8  u8ReprogrammingCounter[REPROGRAMMING_COUNTER_BYTES] = {0 , 0};


BUZZER_ENABLE sBuzzerEnable = {0,0,0};
TELL_TALE_ENABLE sIndicatorEnable = {0, 0, 0, 0};
LCD_SCREEN_CONFIG sLcdScreenConfig = {0, 0};
LOW_OIL_PRESSURE sLowOilPressThresh = {0, 0};
AFE_DATA_OPTIONS sAfeDataOptionBytes = {0, 0};
COOLANT_TEMP sCoolantTempBytes = {0, 0, 0};
AIR_PRESSURE sAirPressureBytes = {0, 0, 0, 0, 0, 0, 0, 0};
FE_DATA sFeData = {0, 0, 0, 0};
SPEED_SENSOR sSpeedSensor = {0, 0, 0, 0};
VEHICLE_OPTION_CONTENT sIndicatorInputType = {0, 0, 0, 0, 0};
SERVICE_REMINDER sServReminder = {0, 0, 0};


/*********************************************************************//**
 *
 * Set CAN BaudRate
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/

void vSetCanBaudRate(eBAUD_RATE BaudRate)
{
	eBaudRateIndex = BaudRate;
}

/*********************************************************************//**
 *
 * Get CAN baud Rate
 *
 *
 * @param      None 
 *
 * @return     Pulse Value 
 *************************************************************************/

eBAUD_RATE eGetCanBaudRate()
{
	return eBaudRateIndex;
}

/*********************************************************************//**
 *
 * Return PULSE value.
 *
 *
 * @param      None 
 *
 * @return     Pulse Value 
 *************************************************************************/

u16 u16GetFeededPulse()
{
	return sSpeedSensor.u16PulsesPerKm;
}

/*********************************************************************//**
 *
 * Set Buzzer enable disable setting
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLBuzzerSetting()
{
	vGetEolData(eBuzzerOptionBytes);
	
	sBuzzerEnable.BuzzerByte[0] = u8BuzzerOption[0];
	sBuzzerEnable.BuzzerByte[1] = u8BuzzerOption[1];
	sBuzzerEnable.BuzzerByte[2] = u8BuzzerOption[2];	
	
}


/*********************************************************************//**
 *
 * Set Vehicle Options
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLVehicleOptionContentData()
{
	vGetEolData(eVehicleContentBytes);
	
	sIndicatorInputType.InputTypeByte[0] = u8VehicleOptionContent[0];
	sIndicatorInputType.InputTypeByte[1] = u8VehicleOptionContent[1];
	sIndicatorInputType.InputTypeByte[2] = u8VehicleOptionContent[2];
	sIndicatorInputType.InputTypeByte[3] = u8VehicleOptionContent[3];
	sIndicatorInputType.InputTypeByte[4] = u8VehicleOptionContent[4];
}

/*********************************************************************//**
 *
 * Set Speed Sensor
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLSpeedSensorData()
{
 	u16 tempvar = 0;
 
	vGetEolData(eVsPulsPerRevBytes);

	tempvar = (u16)u8VsPulsPerRev[0]<<8;
	tempvar  |= u8VsPulsPerRev[1];
	sSpeedSensor.u16PulsesPerKm = tempvar;
	
	//sSpeedSensor.u16PulsesPerKm = u8VsPulsPerRev[1]<<8 | u8VsPulsPerRev[0];
	sSpeedSensor.u8OverSpeedWarning = u8VsPulsPerRev[2];
	sSpeedSensor.u8BiasFactor = u8VsPulsPerRev[3];
	sSpeedSensor.u8SpeedoOffset = u8VsPulsPerRev[4];
	
	//Set Speedo EOL parameters
	vSetSpeedoMaxFreq(sSpeedSensor.u16PulsesPerKm);
	//For Odo increment 
	vSetSpeedoPulses(sSpeedSensor.u16PulsesPerKm);
	vSetVehicleOverSpeed(sSpeedSensor.u8OverSpeedWarning);
	vSetSpeedoBiasFactor(sSpeedSensor.u8BiasFactor);
	vSetSpeedoOffset(sSpeedSensor.u8SpeedoOffset);

}

/*********************************************************************//**
 *
 * Set Engine Over Speed
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/


void vSetEOLEngOverSpeedData()
{
	vGetEolData(eEngineOverSpeedWarningBytes);
	
	u16EolEngineOverSpeed = ((u16)u8EngineOverSpeedWarning[0] * RPM_RES);
	
	//Set RPM EOL parameters
	vSetEngineOverSpeed(u16EolEngineOverSpeed);
}


/*********************************************************************//**
 *
 * Set FE data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLFeData()
{
	vGetEolData(eFeIndicatorThreshBytes);

	sFeData.THRESH1 =  ((u16)u8FeIndicatorThresh[0] * RPM_RES);
	sFeData.THRESH2 =  ((u16)u8FeIndicatorThresh[1] * RPM_RES);
	sFeData.THRESH3 =  ((u16)u8FeIndicatorThresh[2] * RPM_RES);
	sFeData.THRESH4 =  ((u16)u8FeIndicatorThresh[3] * RPM_RES);
}

/*********************************************************************//**
 *
 * Set Air Pressure Data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLAirPressData()
{
	vGetEolData(eAirPressureOptionBytes);
	
	sAirPressureBytes.u8LowAirPressureOnThresh = u8AirPressureOption[0];
	sAirPressureBytes.u8LowAirPressureOffThresh = u8AirPressureOption[1];
	sAirPressureBytes.u8APNumOfBars = u8AirPressureOption[2];
	sAirPressureBytes.u8OhmBar6 = u8AirPressureOption[3];
	sAirPressureBytes.u8OhmBar8 = u8AirPressureOption[4];
	sAirPressureBytes.u8OhmBar10 = u8AirPressureOption[5];	
	sAirPressureBytes.u8OhmBar12 = u8AirPressureOption[6];	
	sAirPressureBytes.u8OhmBar15 = u8AirPressureOption[7];	
}

/*********************************************************************//**
 *
 * Set Coolant Temperture Data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLCoolantTempData()
{
	vGetEolData(eCoolantTempOptionBytes);

	sCoolantTempBytes.u8CoolantTempOnThresh = u8CoolantTempOption[0];
	sCoolantTempBytes.u8CoolantTempOffThresh = u8CoolantTempOption[1];
	sCoolantTempBytes.u8CoolantTempRedZoneStart = u8CoolantTempOption[2];
}

/*********************************************************************//**
 *
 * Set AFE data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLAfeData()
{
	vGetEolData(eAfeOptionsBytes);
	
	sAfeDataOptionBytes.u8AfeMinDistance = u8AfeOptions[0];
	sAfeDataOptionBytes.u8AfeMulFactor = u8AfeOptions[1];
}

/*********************************************************************//**
 *
 * Set DTE data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLDteMulFactorData()
{
	vGetEolData(eDteMulFactorBytes);
	u8EolDteMulFactor = u8DteMulFactor[0];
}

/*********************************************************************//**
 *
 * Set Service Hour Reminder data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLServHrReminderData() 
{

	u32 tempvar = 0;
	
	   vGetEolData(eVsPulsPerRevBytes);
	
	   tempvar = (u16)u8VsPulsPerRev[0]<<8;
	   tempvar	|= u8VsPulsPerRev[1];
	   sSpeedSensor.u16PulsesPerKm = tempvar;

	vGetEolData(eFirstService);
	vGetEolData(eServiceInterval);
	vGetEolData(eServiceThreshold);	

	tempvar = (u16)u8FirstService[0]<<8;
	tempvar |= (u16)u8FirstService[1];
	tempvar = tempvar<<8;
	tempvar |= u8FirstService[2];
	sServReminder.u32FirstServiceDue = tempvar;

	tempvar = (u16)u8ServiceInterval[0]<<8;
	tempvar |= (u16)u8ServiceInterval[1];
	tempvar = tempvar<<8;
	tempvar |= u8ServiceInterval[2];
	sServReminder.u32ServiceInterval = tempvar;

	
	tempvar = (u16)u8ServiceThreshold[0]<<8;
	tempvar |= (u16)u8ServiceThreshold[1];
	tempvar = tempvar<<8;
	tempvar |= u8ServiceThreshold[2];
	sServReminder.u32PopUpInterval = tempvar;

	
}

/*********************************************************************//**
 *
 * Set Oil Pressure Data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLLowOilPressureData()
{
	vGetEolData(eLowOilPressureThreshBytes);
	
	sLowOilPressThresh.u8LowOilPressOnThresh = u8LowOilPressureThresh[0];
	sLowOilPressThresh.u8LowOilPressOffThresh = u8LowOilPressureThresh[1];
}

/*********************************************************************//**
 *
 * Set LCD Configuration Data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLLcdScreenConfigData()
{
	vGetEolData(eDisplayScreenConfigBytes);
	
	sLcdScreenConfig.LcdScreenConfig[0] = u8DisplayScreenConfig[0];
	sLcdScreenConfig.LcdScreenConfig[1] = u8DisplayScreenConfig[1];
}

/*********************************************************************//**
 *
 * Set Tell Tale data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetEOLTellTaleControlData()
{
	vGetEolData(eTelltalesOptionBytes);
	
	sIndicatorEnable.ControlByte[0] = u8TelltalesOption[0];
	sIndicatorEnable.ControlByte[1] = u8TelltalesOption[1];
	sIndicatorEnable.ControlByte[2] = u8TelltalesOption[2];
	sIndicatorEnable.ControlByte[3] = u8TelltalesOption[3];
	
}

/*********************************************************************//**
 *
 * Set all EOL data
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

void vSetAllEolData()
{
	vSetEOLVehicleOptionContentData();
	vSetEOLSpeedSensorData();
	vSetEOLEngOverSpeedData();
	vSetEOLFeData();
	vSetEOLAirPressData();
	vSetEOLCoolantTempData();
	vSetEOLAfeData();
	vSetEOLDteMulFactorData();
	vSetEOLServHrReminderData();
	vSetEOLLowOilPressureData();
	vSetEOLLcdScreenConfigData();
	vSetEOLTellTaleControlData();
	vSetEOLBuzzerSetting();
	vSetCanBaudRate(e250);
	vGetEolData(eVariantDaSetIdBytes);
}

/*********************************************************************//**
 *
 * Get Vehicle Type
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

u8 u8GetVehicleType()
{
	//u8FuelType = FUEL_TYPE_LPH;
	u8FuelType = FUEL_TYPE_KMPL;
	return sIndicatorInputType.Inputsbytes.Byte3.u8AfeUnitType; 
}

/*********************************************************************//**
 *
 * Get Transmission Type
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

u8 u8GetTransmissionType()
{
	return sIndicatorInputType.Inputsbytes.Byte3.u8TransmissionInputType; 
}

/*********************************************************************//**
 *
 * Get Transmission Enable
 *
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/

bool bGetTransmissionEnable()
{
	return sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bTransmissionDisplay;
}

/*********************************************************************//**
 *
 * Get EOL pending status 
 *
 *
 * @param      None 
 *
 * @return     EOL pending status if 0x00 means EOL pending 
 *************************************************************************/

u8 u8GetEolPendingStatus()
{
	return u8VariantDaSetId[4];
}

