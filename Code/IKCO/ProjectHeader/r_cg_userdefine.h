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


/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_userdefine.h
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file includes user definition.
* Creation Date: 18/09/2018
***********************************************************************************************************************/

#ifndef _USER_DEF_H
#define _USER_DEF_H
#include "r_cg_macrodriver.h"
#include "customTypedef.h"

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/

/* Start user code for function. Do not edit comment generated here */

// constant declarations -----------------------------------------------------



#define CLUSTER_ULTRA_24V	1
#define CLUSTER_ULTRA_12V	0
#define CLUSTER_ERGO_24V	0
#define CLUSTER_ERGO_12V	0


 

#define NULL 			0x00

#define ODO_MAX_RANGE      			999999 	// decimal place not included
#define TRIPA_MAX_RANGE    			9999	// decimal place not included
#define TRIPB_MAX_RANGE    			9999	// decimal place not included
#define ODO_FRACTION_VALUE 			9

#define HOUR_MAX_RANGE      		999999 	// decimal place not included
#define HR_TRIPA_MAX_RANGE    		9999	// decimal place not included
#define HR_TRIPB_MAX_RANGE    		9999	// decimal place not included
#define HOUR_FRACTION_VALUE 		9

#define SIX_MINUTE_SECONDS			360


#define IGNITION	          		P13.7 

#define FUEL_TYPE_KMPL				0
#define FUEL_TYPE_LPH				1
#define FUEL_TYPE_KMPKG				2

#define MANUAL_TRANS				0
#define AMT_TRANS					1
#define AT_TRANS					2
#define DISABLE_TRANS				3


#define FIRST_READ_AFTER_RESET		1
#define	SECOND_READ_COUNT			2
#define OFFSET_SAVE_DONE			3

#define NO_DTC_FAULTS				0xFF
#define SINGLE_FRAME_FAULT			0x46

#define MAXIMUM_NUMBER_OF_DTC					38


 typedef enum
{
	eClusterBoot = 0,
	eClusterWatchDogReset,	
	eClusterBatteryReset,
	eClusterIgnitionReset,
	eClusterNormalOperation,
	eClusterForcedSelfTest,
	eClusterEnterSleep,
	eClusterRestart,
	eClusterSleep,
	eCheckProgDependencies
 } eCLUSTER_STATE_MACHINE;

 typedef enum
{
	eApplicationMode = 1U,
	eBootloaderMode = 2U
 } eMCU_MODE_TYPE;

// Global declarations ------------------------------------------------------

extern u32 gu32Clock1msTicker;


extern void vResetCpu();
eCLUSTER_STATE_MACHINE eGetClusterStatus();

/// @addtogroup MAIN_LIBRARY
/// @{
/// @defgroup MainLib  functions used in main file 
//// 
///   
/// @addtogroup MainLib
/// @{

// Global prototypes --------------------------------------------------------

void vWatingTimeMs(u16);
u16 u16GetClockTicks(void);
u32 u32GetClockTicks(void);
bool bCheckIgnitionOff();
bool bCheckIgnitionOn();
bool bGetIgnStatus();
u8 GetControllerMode(void);
bool bCheckDepenDencies(void);
bool bFlashEarseStatus(void);
void ClearFlashEraseFlag(void);
bool bCheckSumStatus(void);
void vClearFlashWriteStatus(void);
bool bGetSensorStatus();
void vSetSensorStatus(bool);
void vSetAllEolData(void);
bool bGetOdoTripARolloverStatus(void);
void vClearOdoTripARolloverStatus(void);
bool bGetOdoTripBRolloverStatus(void);
void vClearOdoTripBRolloverStatus(void);
bool bGetHourTripARolloverStatus(void);
void vClearHourTripARolloverStatus(void);
bool bGetHourTripBRolloverStatus(void);
void vClearHourTripBRolloverStatus(void);


/// @} // endgroup MainLib

/// @} // endgroup MAIN_LIBRARY

/* End user code. Do not edit comment generated here */
#endif
