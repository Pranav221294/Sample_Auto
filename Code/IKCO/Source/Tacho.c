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
#include "Tacho.h"
#include "TML_14229_UDSH.h"

// Constant declarations -----------------------------------------------------



#define RPM_SIGNAL_AVG              8

#define RPM_OVER_SPEED_WAR_OFF_HYS	300		//RPM
#define RPM_HYS						10		//RPM
#define TACHO_USTEPS_TO_SKIP		20 //microsteps


// local declarations --------------------------------------------------------
typedef struct
{	
	u16     u16EngineRpm;  			 
	u16     u16RpmSignalData[RPM_SIGNAL_AVG]; 
	u16		u16EngineOverSpeed;
} RPM_DATA_TYPE;

RPM_DATA_TYPE sRPM = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate 
*  Stepper target position for RPM gauge
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/
void vRpmDataProcessing()
{	 
	u32 data = 0;
	u32 temp = 0;
	static u32 PreviousMicroSteps = 0;
	u8 i = 0;
    static u8 AvgSampleCount = 0;						// Counter for averaging samples

	//When control speedo gauge by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.tachometerFlag ==  TRUE)
	{
		//Must Assign value in PreviousMicroSteps more than rpm max micro stepping 
		//So that current micro steps and previous could not match when comes in normal mode 
		PreviousMicroSteps = 0xFFFFU;
		return;
	}
			
	data = u32GetPGNdata(eJRpm);
	//Check CAN message lost
	if(bGetPGNTimeOut(eJRpm) || data >= BADDATA_2BYTE) 
	{
		data = 0;
	}
	else
	{
	//Go back if new sample is not coming		
	if(! bGetPGNDataReady(eJRpm) )
		return;	
	ClearPGNDataReady(eJRpm);
	}

	//Averging of signal 
	sRPM.u16RpmSignalData[AvgSampleCount] = (u16)data;
	
	if(++AvgSampleCount >= RPM_SIGNAL_AVG)
	{
		AvgSampleCount =0;
	}
		data = 0;
		for(i = 0; i < RPM_SIGNAL_AVG; i++)
		data = data + sRPM.u16RpmSignalData[i];
		data = data / RPM_SIGNAL_AVG;		
		sRPM.u16EngineRpm = (u16)data;
	

	//Calculate Target position 
	temp = u16GaugeMaxMicroSteps(eTachometer);
	
	temp =  (temp * (u32)sRPM.u16EngineRpm)/RPM_RANGE;

	//calculate difference between .............
	if( ((temp > PreviousMicroSteps)? (temp - PreviousMicroSteps) : (PreviousMicroSteps - temp)) > TACHO_USTEPS_TO_SKIP )
	{			 
				 PreviousMicroSteps = temp;
				 vSetTargetPosition((u16)temp , eTachometer);
	}
		
	
	
 	

}

/*********************************************************************//**
 *
 * Current engine speed of vehicle
 *
 * @param	None       
 *
 * @return	Current Engine speed       
 *************************************************************************/
u16 u16GetEngineSpeed(void)
{
static u16 PrevioursRpm = 0;

	 if( ((sRPM.u16EngineRpm > PrevioursRpm)? (sRPM.u16EngineRpm  - PrevioursRpm) : (PrevioursRpm - sRPM.u16EngineRpm)) > RPM_HYS )
	  		PrevioursRpm = sRPM.u16EngineRpm;

	 return PrevioursRpm;
}

/*********************************************************************//**
 *
 * Check Engine over speed 
 *
 * @param 	None       
 *
 * @return	Over speed warning status 
 *************************************************************************/
bool bOverEngineSpeed(void)
{
	static bool status = 0;
	
	if(sRPM.u16EngineRpm >= sRPM.u16EngineOverSpeed)
		status = TRUE;
	else if(sRPM.u16EngineRpm <= (sRPM.u16EngineOverSpeed - RPM_OVER_SPEED_WAR_OFF_HYS) )
		status = FALSE;
		
	return(status);
}

/*********************************************************************//**
 *
 * Set Engine over speed 
 *
 * @param	Engine speed   
 *
 * @return	None
 *************************************************************************/
void vSetEngineOverSpeed(u16 Enginespeed)
{
	sRPM.u16EngineOverSpeed = Enginespeed;
}

