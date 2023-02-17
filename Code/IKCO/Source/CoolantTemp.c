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
#include "StepperDriver.h"
#include "J1939.h" 
#include "CoolantTemp.h" 
#include "Adc.h" 
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "TML_14229_UDSH.h"

// Constant declarations -----------------------------------------------------

#define TEMP_SIGNAL_AVG_SIZE		16



// local declarations --------------------------------------------------------

typedef struct
{
	bool	bCanlost;
	u8     u8TempValue;								// Value of RPM,Cooleant temp. & def level	
	u16     u16data[TEMP_SIGNAL_AVG_SIZE];			// Array for averaging signal samples	
} COOLANT_TEMP_DATA_TYPE;

COOLANT_TEMP_DATA_TYPE sTemp = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate 
 * Stepper target position for temperature gauge
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/

void vCoolantTempDataProcessing()
{
	u32 data = 0;
	u32 temp = 0;
	u32 microsteps =0;
	u8 i = 0;
	static u8 AvgCount = 0;
	static bool FirstEntry = TRUE;

	//When control Temperature gauge  by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.TempGaugeFlag ==  TRUE)
	{
		return;
	}
		
	data = u32GetPGNdata(eJCoolantTemp);
	
	//Check CAN message lost
	if(bGetPGNTimeOut(eJCoolantTemp) || data >= BADDATA)
	{
		data = 0;  //To move pointer on full scale 
	    sTemp.bCanlost = TRUE;
		
		  
	}
	else
	{   
		sTemp.bCanlost = FALSE;
	  
		//Go back if new sample is not coming
		if(! bGetPGNDataReady(eJCoolantTemp) )
			return;	
		ClearPGNDataReady(eJCoolantTemp);
		
	}

	//First entry 
	if(eGetClusterStatus() == eClusterIgnitionReset)
	{
		for(i = 0; i < TEMP_SIGNAL_AVG_SIZE; i++)
			sTemp.u16data[i]  = (u16)data;	 		
	}

	//Averging of signal 
	sTemp.u16data[AvgCount] = (u16)data;
	if(++AvgCount >= TEMP_SIGNAL_AVG_SIZE)
	{
		AvgCount = 0;
	}
	
	data = 0;
	for(i = 0; i < TEMP_SIGNAL_AVG_SIZE; i++)
	data = data + sTemp.u16data[i];
	data = data / TEMP_SIGNAL_AVG_SIZE;	 
	
	if(sTemp.bCanlost)
		sTemp.u8TempValue = 0;
	else
	  sTemp.u8TempValue = (u8)data;
	
	microsteps = u16GaugeMaxMicroSteps(eTempGauge);	
	temp = microsteps;	
	
	if(!sTemp.bCanlost)
	{
		if(sTemp.u8TempValue <= GRAPH_FIRST_TEMP_MAX)
		{
			temp = (temp* 3)/4; 	// temp 60-95 deg have dial  span = Full span/2
			if(data >= TEMP_MIN)
				data = data - TEMP_MIN;
			else
				data = 0;
			data =  ( temp*data )/( GRAPH_FIRST_TEMP_MAX - TEMP_MIN ) ;  //Calculate Target position 
		}
		else if(sTemp.u8TempValue <= sCoolantTempBytes.u8CoolantTempRedZoneStart)
		{
			temp = temp/ 4; 	// temp 80-Redzone deg have dial  span = Full span/4
			temp = temp/2;
			data = data - GRAPH_FIRST_TEMP_MAX;
			data =  ( temp*data )/( sCoolantTempBytes.u8CoolantTempRedZoneStart - GRAPH_FIRST_TEMP_MAX ) ;  //Calculate Target position 
			data = data + (microsteps*3)/4;
		}
		else
		{
			temp = temp/ 4; 		// temp Redzone - 120 deg have dial  span = Full span/4
			temp = temp/2;
			data = data - sCoolantTempBytes.u8CoolantTempRedZoneStart;
			data =	( temp*data )/( TEMP_MAX - sCoolantTempBytes.u8CoolantTempRedZoneStart ) ;	//Calculate Target position 
			data = data + (microsteps*3)/4+temp;	
		}
	}

	
	
   if(sTemp.bCanlost)   	
		vSetTargetPosition( (u16)temp , eTempGauge );
   else
   		vSetTargetPosition( (u16)data , eTempGauge );
   	
	
}


/*********************************************************************//**
 *
 * Check coolant temperature warning status
 *
 * @param      coolant temperature warning status 
 *
 * @return     None
 *************************************************************************/
bool bCoolantTempWrnStatus(void)
{
	static bool status = 0;	
	
	if(sTemp.bCanlost == TRUE)  
	{
	 	status = eOpenCkt;
	}
	else
	{	
		if(sTemp.u8TempValue >= sCoolantTempBytes.u8CoolantTempOnThresh)
		{
			status = eWarningOn;
		}
		else if(sTemp.u8TempValue <= sCoolantTempBytes.u8CoolantTempOffThresh)
		{
			status = eWarningOff;
		}
	}
	
	return(status);
}
