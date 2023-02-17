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

/*******************************************************************************
* Library       : EEPROM Emulation Library (T02)
*
* File Name     : $Source: eelDescriptor.c $
* Lib. Version  : $RL78_EEL_LIB_VERSION_T02: V1.00 $
* Mod. Revision : $Revision: 1.8 $
* Mod. Date     : $Date: 2012/10/26 09:28:20MESZ $
* Device(s)     : RL78
* Description   : contains the user defined EEL-variable descriptor
*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/



/*==============================================================================================*/
/* compiler settings                                                                            */
/*==============================================================================================*/
#pragma name eel_descriptor
#pragma sfr

#pragma section @@CNSTL EEL_CNST

#include "dataflash.h"
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "DtcCycle.h" 



/*==============================================================================================*/
/* include files list                                                                           */
/*==============================================================================================*/
#define  __EEL_DESCRIPTOR_C
  #include  "eel_types.h"
  #include  "eel_descriptor.h"
#undef   __EEL_DESCRIPTOR_C

#include  "fdl_types.h"


/* definition of variable types registered at EEL */
#include  "eel_user_types.h"


/*==============================================================================================*/
/* import list                                                                                  */
/*==============================================================================================*/
/* empty */


/*==============================================================================================*/
/* data definition                                                                              */
/*==============================================================================================*/
/* empty */



/*********************************************************************************************************/
/*******                                                                                           *******/
/*******      B E G I N    O F    C U S T O M I Z A B L E    D E C L A R A T I O N    A R E A      *******/
/*******                                                                                           *******/
/*********************************************************************************************************/

__far const eel_u8 eel_descriptor[EEL_VAR_NO+2] =
{
	(eel_u8)(EEL_VAR_NO),  /* variable count   			*/  \
	(eel_u8)BOOTLOADER_IDENTIFIER ,	/* 1 byte bootloader identifier		*/  \
	(eel_u8)ODO_SYS_PARAMETER	,  	/* 10 bytes odo, trip a, trip b 	*/  \
	(eel_u8)HOUR_SYS_PARAMETER	,  	/* 13 bytes Hour, trip a, trip b	*/  \
	(eel_u8)AFE_A_SYS_PARAMETER	,  	/* 14 AFE TripA parameters			*/  \
	(eel_u8)AFE_B_SYS_PARAMETER	,  	/* 14 AFE TripB parameters			*/  \
	(eel_u8)4	,  	/* 4 byte clock data HH:MM:SS		*/  \
	(eel_u8)4	,  	/* 4 byte Service due hours			*/  \
	(eel_u8)4	,  	/* 4 byte Duplicate odo				*/  \
	(eel_u8)4	,  	/* 4 byte Duplicate hour			*/  \
	(eel_u8)4	,  	/* 1 byte Service Counter			*/  \
	(eel_u8)1	,  	/* 1 byte SetDefaultFlag1			*/  \
	(eel_u8)1	,  	/* 1 byte SetDefaultFlag2			*/  \
	(eel_u8)1	,  	/* 1 byte Gen Illumination			*/  \
	(eel_u8)1	,  	/* 1 byte Odo Write counter			*/  \
	(eel_u8)1	,  	/* 1 byte Hour Write counter		*/  \
	

  
	(eel_u8)VEHICLE_TYPE_BYTES	,					/*	eVehicleTypeBytes				*/	\
	(eel_u8)SPEED_SENSOR_BYTES	,					/*	eVsPulsPerRevBytes				*/	\
	(eel_u8)ENGINE_OVER_SPEED_BYTES  ,		        /*	eEngineOverSpeedWarningBytes	*/	\
	(eel_u8)FE_INDICATOR_THRESH_BYTES	,			/*	eFeIndicatorThreshBytes			*/	\
	(eel_u8)AIR_PRESSURE_OPTION_BYTES	,			/*	eAirPressureOptionBytes 		*/	\
	(eel_u8)COOLANT_TEMP_OPTION_BYTES	,			/*	eCoolantTempOptionBytes 		*/	\
	(eel_u8)AFE_OPTIONS_BYTES	,					/*	eAfeOptionsBytes				*/	\
	(eel_u8)DTE_MUL_FACTOR_BYTE	,		            /*	eDteMulFactorBytes				*/	\
	(eel_u8)LOW_OIL_PRESSURE_THRESH_BYTES	,		/*	eLowOilPressureThreshBytes		*/	\
	(eel_u8)DISPLAY_SCREEN_CONFIG_BYTES ,		    /*	eDisplayScreenConfigBytes		*/	\
	(eel_u8)TELLTALES_OPTION_BYTES	,				/*	eTelltalesOptionBytes			*/	\
	(eel_u8)BUZZER_OPTION_BYTES	,					/*	eBuzzerOptionBytes				*/	\
	(eel_u8)REPROGRAMMING_COUNTER_BYTES	,			/*	eReProgrammingCounterBytes 		*/	\
	(eel_u8)DATE_OF_LAST_PROGRAMMING_BYTES	,		/*	eDateOfLastProgrammingBytes 	*/	\
	(eel_u8)VEHICLE_ID_BYTES	,					/*	eVehicleIdBytes 				*/	\
	(eel_u8)VARIANT_DA_SET_ID_BYTES ,				/*	eVariantDaSetIdBytes			*/	\
	(eel_u8)VEHICLE_CONFIG_NUMBER_BYTES ,			/*	eVehicleConfigNumberBytes		*/	\
	(eel_u8)PARAMETER_PART_NUMBER_BYTES ,			/*	eParameterPartNumberBytes		*/	\
	(eel_u8)PROGRAMMING_SHOP_CODE_BYTES ,			/*	eProgrammingShopCodeBytes		*/	\
	(eel_u8)SERVICE_BYTES ,							/*	eFirstService					*/	\
	(eel_u8)SERVICE_BYTES ,							/*	eServiceInterval				*/	\
	(eel_u8)SERVICE_BYTES ,							/*	eServiceThreshold				*/	\
	(eel_u8)FUEL_TANK_CAP_LEN ,						/*	eFuelTankCapacity				*/	\
	(eel_u8)CAN_SPEEDO_FACTOR_LEN ,					/*	eCanSpeedoFactor K1 & K2		*/	\
	(eel_u8)CAN_SPEEDO_FACTOR_LEN ,					/*	eCanSpeedoFactor K3 & K4		*/	\
	(eel_u8)DTC_DATA_LENGTH ,						/*	eFrontApg DTC					*/	\
	(eel_u8)DTC_DATA_LENGTH ,						/*	eRearApg DTC					*/	\
	(eel_u8)DTC_DATA_LENGTH ,						/*	eFuel   DTC						*/	\
	(eel_u8)DTC_DATA_LENGTH ,						/*	eOilpressure DTC				*/	\
	(eel_u8)DTC_DATA_LENGTH ,						/*	eCan Bus off DTC				*/	\
    /*	* Add required Parameters in eelDescriptor array in same sequence
	/*	  as mentioned in eSysParmsIndex enum.
	/*	  Example : (eel_u8)1,  /* id=15             */  \
	/*	  			(eel_u8) is typedef, 1 is the Data Size of the variable.
	/*	NOTE : the sequence of defining enum should be same as sequence of size defined in eelDescriptor.c
	/*
  	/*  * Navigate to eel_descriptor.h and change the value of EEL_VAR_NO to :  
	/*	  |14+(no. of added parameters)|.
	/*	  Example : If adding 4 more paramenters, then the value of EEL_VAR_NO changes to 18 (14+4).*/

  
  (eel_u8)(0x00),            /* zero terminator  */  \
};

/*********************************************************************************************************/
/*******                                                                                           *******/
/*******      E N D    O F    C U S T O M I Z A B L E    D E C L A R A T I O N    A R E A          *******/
/*******                                                                                           *******/
/*********************************************************************************************************/




/*********************************************************************************************************/
/*******                                                                                           *******/
/*******      B E G I N    O F    U N T O U C H A B L E     D E C L A R A T I O N    A R E A       *******/
/*******                                                                                           *******/
/*********************************************************************************************************/
__far const eel_u8   eel_internal_cfg_cau08[]     = {0x40};
/*********************************************************************************************************/
/*******                                                                                           *******/
/*******      E N D    O F    U N T O U C H A B L E     D E C L A R A T I O N    A R E A           *******/
/*******                                                                                           *******/
/*********************************************************************************************************/

