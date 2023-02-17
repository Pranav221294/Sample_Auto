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
#include "r_cg_adc.h"
#include "Adc.h" 


// Constant declarations -----------------------------------------------------

#define ADC_REFRESH_TIME 	  		10      //Adc Scan time in ms


// local declarations --------------------------------------------------------


typedef struct
{     
    u16   u16TimeStamp;
    u16   u16Value[eNO_OF_ADC_CH];			//hold ADC samples value of different channels
	bool  bValid;							//ADC data conversion complete flag   
	bool  bTakeAdcSample[eNO_OF_ADC_CH];		//ADC value read and fill in buffer 
	
     
} ADC_TYPE; 

ADC_TYPE sADC = {0x00,0x00,0x00,0x00};
static void _vSelectAdcChannel(u8);
 

/*********************************************************************//**
 *
 * Read ADC 5 channels one by one , ADC is reading with ADC interrupts 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

 void vReadAdc()
{	
	u16 data = 0;	
	static u8 AdcChCount = 0;
	//Check ADC referesh Time
	if( u16GetClockTicks() - sADC.u16TimeStamp  >= ADC_REFRESH_TIME)	
	{
		sADC.u16TimeStamp = u16GetClockTicks();
		
		//sADC.valid set in adc interrupt 
		if(sADC.bValid == TRUE)         
		{  
			//Read ADC data	 
			R_ADC_Get_Result(&data);  
			sADC.bValid = FALSE;	    
			sADC.u16Value[AdcChCount] = data; 
			//ADC value read and fill in buffer 
			sADC.bTakeAdcSample[AdcChCount] = TRUE;
			//Increment ADC channel counter
			if(++AdcChCount >= eNO_OF_ADC_CH)
			AdcChCount = 0;
			//Select ADC channel
			_vSelectAdcChannel(AdcChCount);
			// Check Conversion Complete flag 		
			R_ADC_Start();
		}
	}//End if( u16GetClockTicks() - sADC.u16TimeStamp  >= ADC_REFRESH_TIME)
}

/*********************************************************************//**
 *
 * Select ADC channel 
 *
 * @param      ADC channel
 *
 * @return     None 
 *************************************************************************/

static void _vSelectAdcChannel(u8 channel)
{
	ADS = channel;
}

/*********************************************************************//**
 *
 * Set ADC data convesion flag 
 *
 * @param      ADC channel
 *
 * @return     None 
 *************************************************************************/

void vSetAdcDataConversion(bool status)
{
	sADC.bValid = status;
}

/*********************************************************************//**
 *
 * Get ADC value 
 *
 * @param      Gauge type
 *
 * @return     ADC value 
 *************************************************************************/

u16 u16GetAdcValue(eADC_CH_TYPE GaugeType)
{
	return sADC.u16Value[GaugeType];
}


/*********************************************************************//**
 *
 * Get ADC sample satus 
 *
 * @param      Gauge type
 *
 * @return     Sample reading status
 *************************************************************************/

bool bGetAdcSampleStatus(eADC_CH_TYPE GaugeType)
{
	return sADC.bTakeAdcSample[GaugeType];
}


/*********************************************************************//**
 *
 * Clear ADC sample satus flag 
 *
 * @param      Gauge type
 *
 * @return     None
 *************************************************************************/

void vResetAdcSampleStatus(eADC_CH_TYPE GaugeType)
{
	sADC.bTakeAdcSample[GaugeType] = FALSE;
}




 



