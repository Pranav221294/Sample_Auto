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

#ifndef _LCD_H_
#define _LCD_H_

#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "customTypedef.h"
#include "Afe.h"



typedef enum
{
	eBlank,
	ePriority1, 
	ePriority2
} eTOP_LINE_PRIORITY;
	
typedef enum
{
	eNoSubPriority,
	eSubPriority1, 
	eSubPriority2,
	eSubpriority2VariableString
} eTOP_LINE_SUB_PRIORITY;

typedef enum
{
	eSP1Blank = 0U,
	eAutoRegenOn,
	eManualRgnActive,
	eManualRegComplete,
	eManualRegAbort,
	eAMTLearning,
	eRangeInhibit,
	eSelftestProgress,
	eRampEngaged,
	eKneelingFront,
	eKneelingRear,
	eKneelingBoth,	
	eHaltRqstPriority,
	eSteeringAdjust,	
	eKneelingRight,
	eKneelingLeft,	
	eHSAActive,
	eIcEolPending,
	eTotalSP1Faults
}eSP1Faults; 

typedef enum
{
	eSP2Blank = 0U,
	eEgrStopped,
	eDefDosingStopped,
	eDefDosingFault,
	eDefQualityNotOk,
	eDefSysElecFault,
	eDefSCRElecFault,
	eNoDosingLowTemp,
	eDefLowRefill,
	eDefxx,
	eDefVLowRefill,
	eDefNearEmpty,	
	eDefEmptyRefill,
	eCheckAlternator,
	eCheckBattery,
	eRegenstopPressed,
	eAutoRegenstoped,
	eDoManualRegen,
	eStopEngine,
	eVisitWorkshop,
	eRegenRequired,
	eHSAError,
	eHaltRequest,
	eEngineOilLow,
	eEngineCoolantLow,
	eEmergencyExit,
	eRearFlapOpen,
	eSideFlapOpen,
	eDoorOpen,	
	eTotalSP2Faults
	
}eSP2Faults; 


/// @addtogroup USER_LCD
/// @{
 
/// @defgroup LcdheaderLib Global functions used in LCD files
//// 
///  
 
/// @addtogroup LcdheaderLib
/// @{


// Global prototypes --------------------------------------------------------
void vLcdInit();
void vSendData();
void vRefreshLcd();
void vLcdInit(void);


void vSetTopLinePriority(eTOP_LINE_PRIORITY eTLPriority);
eTOP_LINE_PRIORITY eGetTopLinePriority(void);

void vSetTopLineSubPriority(eTOP_LINE_SUB_PRIORITY eTLSubPriority);
eTOP_LINE_SUB_PRIORITY eGetTopLineSubPriority(void);

void vSetTopLineText(u8 );



/// @} // endgroup LcdheaderLib

/// @} // endgroup USER_LCD


#endif
