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
#ifndef __Speedo_H
#define __Speedo_H

// Constant declarations -----------------------------------------------------
#define SPEEDO_RANGE                120U		 //km/h

// Global prototypes --------------------------------------------------------

/// @addtogroup SPEEDO_LIBRARY
/// @{
/// @defgroup SpeedoLib  functions used 
/// @addtogroup SpeedoLib
/// @{

// Global prototypes --------------------------------------------------------

void vSpeedDataProcessing(void);
bool bOverSpeedStatus(void);
u16 u16GetVehicleSpeed(void);
void vSetVehicleOverSpeed(u8);
void vSetSpeedoMaxFreq(u16);
u16 u16GetVehiclePulses(void);
void vSetSpeedoBiasFactor(u8);
void vSetSpeedoOffset(u8 );

/// @} // endgroup SpeedoLib

/// @} // endgroup SPEEDO_LIBRARY



extern u32 u32GetFreqCaptureValue(void);
extern bool bGetCaptureSignalStatus(void);
extern void vSetCaptureSignalStatus(bool);
extern void vSetAnalougeOdoEnable(bool);
extern u16 u16GetClockTicks(void);

#endif