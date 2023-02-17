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
/************************************************************************************
**  Project : BS6Cluster
**	File	: dataflash.h
** ----------------------------------------------------------------------------------
**  Description:
**      All include files
**		EOL variables size
**		EOL variable address
**      System parameters declaration
**		System parameters Index
**		EOL Variables index
**	EOL data in FDL pool.
** ----------------------------------------------------------------------------------
**  COPYRIGHT: 
**              INDICATION INSTRUMENTS LTD.
**              PLOT-19, SECTOR 6, FARIDABAD.
**
** ----------------------------------------------------------------------------------
**
**  Revision History
**
**  Date       Name             Description
** ==========  ===============  =====================================================
** 09-10-2018  Pratyush Abhay   Initial Version.
*************************************************************************************/




#ifndef _DATA_FLASH
#define _DATA_FLASH


#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "eel_types.h"
#include "eel_user_types.h"
#include "eel_descriptor.h"
#include "eel.h"
#include "fdl.h"
#include "fdl_types.h"
#include "fdl_descriptor.h"
#include "customTypedef.h"
#include "r_cg_userdefine.h"
#include "Eol.h"

#define BOOTLOADER_IDENTIFIER 1U

//ODO 3 Byte + ODO Fr 1 Byte + TripA 2 Byte + TripA Fr 1 Byte + TripB 2 Byte + TripB Fr 1 Byte  = 10
#define ODO_SYS_PARAMETER	10

//Hour 3 Byte + Hour Fr & Second 2 Byte + Hr TripA 2 Byte + Hr TripA Fr & Second 2 Byte 
//+ Hr TripB 2 Byte + Hr TripB Fr & Second 2 Byte  = 13
#define HOUR_SYS_PARAMETER	13

//LFC Byte 4 + PreFuelConsume Byte  4 + Trip Offset Byte 2 + Trip Afevalue Byte 2 +  EEPROMOffsetFlag 1
#define AFE_A_SYS_PARAMETER	14

//LFC Byte 4 + PreFuelConsume Byte  4 + Trip Offset Byte 2 + Trip Afevalue Byte 2 +  EEPROMOffsetFlag 1
#define AFE_B_SYS_PARAMETER	14


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
extern u8 u8FirstService[SERVICE_BYTES];
extern u8 u8ServiceInterval[SERVICE_BYTES];
extern u8 u8ServiceThreshold[SERVICE_BYTES];



extern u8 u8LowOilPressureThresh[LOW_OIL_PRESSURE_THRESH_BYTES];
extern u8 u8DisplayScreenConfig[DISPLAY_SCREEN_CONFIG_BYTES];
extern u8 u8TelltalesOption[TELLTALES_OPTION_BYTES];
extern u8 u8BuzzerOption[BUZZER_OPTION_BYTES];


//***************************************ECU PARAMETERS*************************************************//

extern u8 u8DateOfLastProgramming[DATE_OF_LAST_PROGRAMMING_BYTES];
extern u8 u8VehicleId[VEHICLE_ID_BYTES];
extern u8 u8VariantDaSetId[VARIANT_DA_SET_ID_BYTES];
extern u8 u8ParameterPartNumber[PARAMETER_PART_NUMBER_BYTES];
extern u8 u8ProgrammingShopCode[PROGRAMMING_SHOP_CODE_BYTES];
extern u8 u8VehicleConfigNumber[VEHICLE_CONFIG_NUMBER_BYTES];// = {'P','R','O','G','R','A','M','C','O','U','N','T','O','F','T'};



/*******************************************************************************************************
******************************************SYS DATA Enumeration******************************************
						ADD TO THIS ENUMERATION FOR MORE SYSTEM PARAMETERS

/* PARAMETERS CAN BE DELETED ACCORDING TO NEEDS.
/* NEVER DELETE "SetDefaultFlag" VARIABLES.
********************************************************************************************************/
typedef enum
{
	eBootLoaderIdentifier	= 1U	,			//Bootloader Identifier
	eOdoIdentifier					,	
	eHourIdentifier					,
	eTripAAfeIdentifier				,
	eTripBAfeIdentifier				,
	eClockData						,			//Clock data Hour,Minute,second,AMPMstatus
	eServiceDueHours				,			//Service due hours	
	eDuplicateOdoId					,
	eDuplicateHourId				,	
	eServiceCounter					,
	eSetDefaultFlag1				,			//SetDefaultFlag1
	eSetDefaultFlag2				,			//SetDefaultFlag2
	eGenIllSetting					,			//Geneara Illumination setting for day and night mode
	eOdoEolWriteCountIdentifier		,			//Odo write by EOL counter 
	eHourEolWriteCountIdentifier	,			//Hour write by EOL counter 
	eNoOfSysParaMeter				,
	
	/* if required to add new parameters, follow steps below :
	/*	* Add required parameters to eSysParmsIndex enum squencially as defined in dataflsh.h.
	/*	* Navigate to eel_descriptor.c and add required Parameters in eel_descriptor array in same sequence
	/*	  as mentioned in eSysParmsIndex enum.
	/*	  Example : (eel_u8)1,  /* id=15             */  
	/*	  			(eel_u8) is typedef, 1 is the Data Size of the variable.
	/*	NOTE : the sequence of defining enum should be same as sequence of size defined in eel_descriptor.c 
	/*	
	/*	* Navigate to eel_descriptor.h and change the value of EEL_VAR_NO to :  
	/*	  |14+(no. of added parameters)|.
	/*	  Example : If adding 4 more paramenters, then the value of EEL_VAR_NO changes to 18 (14+4).*/
	
}eSysParmsIndex;

/*******************************************************************************************************
**********************************************EOL DATA Enumeration**************************************
   Add new variables if required. the new variable should be in sequence as earlier variables defined.
********************************************************************************************************/
typedef enum
{
	eVehicleContentBytes	=	eNoOfSysParaMeter,
	eVsPulsPerRevBytes				,		
	eEngineOverSpeedWarningBytes 	,	
	eFeIndicatorThreshBytes			,	
	eAirPressureOptionBytes			,	
	eCoolantTempOptionBytes			,	
	eAfeOptionsBytes 				,	
	eDteMulFactorBytes				,	
	eLowOilPressureThreshBytes		,	
	eDisplayScreenConfigBytes		,	
	eTelltalesOptionBytes			,	
	eBuzzerOptionBytes				,
	eReProgrammingCounterBytes		,
	eDateOfLastProgrammingBytes		,	
	eVehicleIdBytes					,	
	eVariantDaSetIdBytes 			,	
	eVehicleConfigNumberBytes		,	
	eParameterPartNumberBytes		,	
	eProgrammingShopCodeBytes		,
	eFirstService					,
	eServiceInterval				,
	eServiceThreshold				,
	eFuelTankCapacity				,
	eCanSpeedoFactor				,
	eCanSpeedoFactork3k4			,
	eTotalEOLParameters
	/* Add new Variables if required*/
	
}eEolDataIndex;


typedef enum
{
	eFrontApgShortToGndIdentifier	=	eTotalEOLParameters,
	eRearApgShortToGndIdentifier,
	eFuelShortToGndIdentifier,
	eOilpressureShortToGndIdentifier,
	eCanBusOffIdentifier,
	eTotalDtcIdentifiers

}eDtcDataIndex;


//-------------Global declarations --------------------------------------------------------




/********************************************************************************************************
**********************************Public Function Prototype Declaration**********************************
*********************************************************************************************************/

/// @addtogroup DATAFLASH_LIBRARY
/// @{
/// @defgroup DataFlashLib  functions used 
//// 
///   
/// @addtogroup DataFlashLib
/// @{

void vEelFormat();
void vDataFlashinit();
void vSaveEolData(eEolDataIndex eEolData);
void vGetEolData(eEolDataIndex eEolData);
void vSetDefaultVariablesToFlash(void);
void vSaveFullSystemParameter();
void vReadSystemParameter();
void vSaveClockData();
void vResetTripAfeData();
void vSaveOdoUpdateByEolCounter(u8 value);
void vSaveHourUpdateByEolCounter(u8 value);
u8 u8GetOdoUpdateByEolCounter();
u8 u8GetHourUpdateByEolCounter();
void vSaveBootloaderState(u8 data);


/// @} // endgroup DataFlashLib

/// @} // endgroup DATAFLASH_LIBRARY


#endif
