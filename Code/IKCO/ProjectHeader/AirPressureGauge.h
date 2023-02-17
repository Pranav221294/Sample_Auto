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
#ifndef __AirPressureGauge_H
#define __AirPressureGauge_H

// Global prototypes --------------------------------------------------------

typedef enum
{
	eNoBuzzer,	
    eAPGBuzzerActive    
}eAPG_BUZZER_TYPE;

typedef enum
{
	eApgDigit10 = 1,
	eApgDigit12,
	eApgDigit15
}eAPG_BAR_TYPE;



/// @addtogroup APG_LIBRARY
/// @{
/// @defgroup ApgLib  functions used 
//// 
///   
/// @addtogroup ApgLib
/// @{

void vAPGFrontDataProcessing();
void vAPGRearDataProcessing();
u8 u8ApfBarNo(void);
u8 u8AprBarNo(void);
bool bGetFrontApgSymbolStatus(void);
bool bGetRearApgSymbolStatus(void);
u8 u8APGWarningStatus(void);
eAPG_BUZZER_TYPE eFrontApgBuzzerStatus();
eAPG_BUZZER_TYPE eRearApgBuzzerStatus();
void vSetRearApgBarNo(u8 BarNo);
void vSetFrontApgBarNo(u8 BarNo);




/// @} // endgroup ApgLib

/// @} // endgroup APG_LIBRARY


extern u32 u32GetClockTicks(void);

#endif
