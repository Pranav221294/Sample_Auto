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
#ifndef __Adc_H
#define __Adc_H

// Constant declarations -----------------------------------------------------

#define OPEN_SHORT_SENSE_TIME			1000	//in ms
//open ckt and short to battery are same 
#define  OPEN_CIRCUIT               	2550	// Ohm*10 
//Short to ground 
#define  SHORT_CIRCUIT              	30		// Ohm*10  
									
#define OPEN_CKT_HYS  					200		// Ohm*10
#define SHORT_CKT_HYS 					50		// Ohm*10

#define WARNING_ON_TIME					1000	//ms
#define WARNING_OFF_TIME				3000	//ms

typedef enum
{
    eAP2_CH0,
    eAP1_CH1,    
    eMIL_ADCH2,
    eBATT_VOLT_CH3,
    eOIL_PRESS_CH4,
    eFUEL_CH5,
    eNO_OF_ADC_CH
}eADC_CH_TYPE;

typedef enum
{
	eOpenCkt,	
    eShortCkt,    
    eWarningOn,
    eWarningOff
}eGauge_Lamp_TYPE;


/// @addtogroup ADC_LIBRARY
/// @{
/// @defgroup AdcLib  functions used in main file 
//// 
///   
/// @addtogroup AdcLib
/// @{

void vReadAdc();
void vSetAdcDataConversion(bool);
u16 u16GetAdcValue(eADC_CH_TYPE);
void vResetAdcSampleStatus(eADC_CH_TYPE);
bool bGetAdcSampleStatus(eADC_CH_TYPE);

/// @} // endgroup AdcLib

/// @} // endgroup ADC_LIBRARY

extern u16 u16GetClockTicks(void);
extern u32 u32GetClockTicks(void);

#endif