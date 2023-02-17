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
#include "Speedo.h"
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "j1939.h"
#include "OdoMeter.h"
#include "TML_14229_UDSH.h"
#include "TellTales.h"



// Constant declarations -----------------------------------------------------

#define FREQ_OUT_TIME 				500   	 // in ms
#define SPEEDO_FREQ_AVG_SIZE		26
#define CAPTURE_CLOCK             	2500000  //Capture timer clock*10 value
#define	OVER_SPEED_WAR_OFF_HYS		5		 //Km
#define EXCEED_FREQ_LIMIT           32       //2k Frequency capture value
#define AVG_STEPS					5
#define NO_OF_FREQ_STEPS			4


#define NO_OF_AVG_STEPS 	5
#define AVG_STEP1 			0


// local declarations --------------------------------------------------------
typedef struct
{		
	u32     u32Frequency;      								
	u32     u32Signaldata[SPEEDO_FREQ_AVG_SIZE];    //For frequency or CAN data averaging    						
	u16     u16Value; 								//Speed value 
	u16     u16SpeedoMaxfreq;						//Speedo maximum range frequency 
	u16     u16CutOffFreq;							// speed cut of frequency 
	u16 	u16k1SpeedoFactor;						//EOL cofigurable used for ODO calculation
	u16 	u16k2SpeedoFactor;						//EOL cofigurable used for speed calulation
	u8      u8OverSpeed;							// over speed for speedo
	u8      u8SpeedOffset;							// over speed for speedo
		
} SPEEDO_DATA_TYPE;


SPEEDO_DATA_TYPE sSpeedo = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

f32 fBiasFactor;




/*********************************************************************//**
 *
 * Calculate frequency and  average it then update stepper target 
*  position. If frequency is not coming then set it to zero.
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 	
void vSpeedDataProcessing()
{	
	u32 data = 0;	
	u8 i = 0;
	u32 temp =0;
	u32 temp1 =0;
	static u16 FreqTimeOutStamp = 0;
	static u8 AvgSampleCount = 0;	
	static u8 NoOfAvgSample = 0;
	static bool speedoupdateFlag = 0;
	bool StatusFlag = 0;
		

	
	//When speedo gauge is controlled by IO control service then do not execute this function
	if(sTestEquimentControl.IoControlFlag.bits.speedometerFlag ==  TRUE)
	{
		return;
	}
  //if  "Speedo required with PTO" value 0 in eol and  if PTO lamp ON then speedo and ODO should not work
  if(sIndicatorInputType.Inputsbytes.Byte4.bSpeedorequiredwithPTO == 0)
  	{
		StatusFlag =  bGetPtoLampStatus();
		if(StatusFlag == TRUE)
		{
		 vSetAnalougeOdoEnable(FALSE);
		 vSetTargetPosition(0,eSpdMeter);
		 sSpeedo.u16Value = 0;
		 sSpeedo.u32Frequency = 0;
		  return;
		}
		vSetAnalougeOdoEnable(TRUE);
  	}
		
	if(sIndicatorInputType.Inputsbytes.Byte1.bVehicleSpeedInput == eAnalouge)
	{
		if(bGetCaptureSignalStatus())
		{
			 vSetCaptureSignalStatus(FALSE);
			if( (u16GetClockTicks() - FreqTimeOutStamp) <= FREQ_OUT_TIME )
				{
				 speedoupdateFlag = TRUE;
				}
				 
				FreqTimeOutStamp = u16GetClockTicks();	
		}
		else if((u16GetClockTicks() - FreqTimeOutStamp) >= FREQ_OUT_TIME + 10)
		{
			stepperCB[eSpdMeter].s16TargetPosition = 0;
			//Fill zero in avg array 
			for(i = 0; i < SPEEDO_FREQ_AVG_SIZE; i++) 
				sSpeedo.u32Signaldata[i] = 0;
			sSpeedo.u32Frequency = 0;
			sSpeedo.u16Value = 0;
			AvgSampleCount = 0;
			NoOfAvgSample = 0;	
			speedoupdateFlag = FALSE;
			
		}	

		//Check capture flag 	
		if(speedoupdateFlag == TRUE)
		{
			speedoupdateFlag = FALSE;
			//Calculate frequency		
			temp = u32GetFreqCaptureValue();
			//If frequency is more than 2k then go back
			if(temp < EXCEED_FREQ_LIMIT)
			{ 
			   vSetAnalougeOdoEnable(FALSE);
			   return;
			}
			//Calculate frequency 
			data = (u32)(CAPTURE_CLOCK)/temp;

			if(data > 1000)
				NoOfAvgSample = 25;
			else if(data > 500)
				NoOfAvgSample = 10;
			else if(data > 300)
				NoOfAvgSample = 5;
			else if(data > 100)
				NoOfAvgSample = 3;
			else if(data > 50)
				NoOfAvgSample = 2;
			else
				NoOfAvgSample = 1;
			
				
			//Averging of signal 
			sSpeedo.u32Signaldata[AvgSampleCount] = data;
			
			if(++AvgSampleCount >= NoOfAvgSample)
			{
				temp = 0;
				for(i = 0; i < NoOfAvgSample; i++)
				temp = temp + sSpeedo.u32Signaldata[i];
				temp = temp / NoOfAvgSample;
				
				for(i = 0; i < NoOfAvgSample - 1; i++)
				sSpeedo.u32Signaldata[i]=  sSpeedo.u32Signaldata[i+1];
				AvgSampleCount = NoOfAvgSample - 1;
				sSpeedo.u32Frequency = temp;
			}	
						
			//if frequency is coming more Max freq then stop odo meter counting 
			if(sSpeedo.u32Frequency >= sSpeedo.u16CutOffFreq  )
			{				
				vSetAnalougeOdoEnable(FALSE);
			}
			else 
			{
				vSetAnalougeOdoEnable(TRUE);
			}
			
	       //Calculate vehicle speed
			sSpeedo.u16Value = (u16)((sSpeedo.u32Frequency * SPEEDO_RANGE)/sSpeedo.u16SpeedoMaxfreq);
			
			//Calculate Target position 	
			temp1 = u16GaugeMaxMicroSteps(eSpdMeter);
			
			temp =  (temp1 * sSpeedo.u32Frequency)/sSpeedo.u16SpeedoMaxfreq;
			
	    	if(sIndicatorInputType.Inputsbytes.Byte1.bVehicleSpdBias)		
				temp *= fBiasFactor;	//Mutiply bias factor 
				
			//Add speedo offset 
			 temp = temp + (temp1/SPEEDO_RANGE)* (u32)sSpeedo.u8SpeedOffset; 
			
			if(temp > u16GaugeMaxMicroSteps(eSpdMeter))
				temp = u16GaugeMaxMicroSteps(eSpdMeter); 
			
			vSetTargetPosition((u16)temp,eSpdMeter);
			
		}
	}//end if(sIndicatorInputType.Inputsbytes.Byte1.bVehicleSpeedInput)	
/*	else
	{
	
		//Check CAN message lost
		if(bGetPGNTimeOut(eJSpeedoSpn191)) 
		{
			data = 0;
		}
		else
		{
			//Go back if new sample is not coming		
		if(! bGetPGNDataReady(eJSpeedoSpn191) )
			return;	
		ClearPGNDataReady(eJSpeedoSpn191);
		
		
		}
		data = u32GetPGNdata(eJSpeedoSpn191);		
		//Averging of signal 
		NoOfAvgSample = 8; // 8 sample for avg CAN data
		sSpeedo.u32Signaldata[AvgSampleCount] = data;

		data = 0;
		for(i = 0; i < NoOfAvgSample; i++)
			data = data + sSpeedo.u32Signaldata[i];
		
		data = data / NoOfAvgSample;

		if(++AvgSampleCount >= NoOfAvgSample)		
			AvgSampleCount = 0;

		//To be corrected when K1 & K2 factor value and resolution shared by TML
		//Calulate speed
		temp = data;
		temp = temp*(u32)sSpeedo.u16k2SpeedoFactor;		
		sSpeedo.u16Value = (u16)temp;

		//Calculate odo pulses
		//Must Set Analouge ODO False so that Analouge ODO will not work 
		vSetAnalougeOdoEnable(FALSE);
		temp = data;
		temp = temp*(u32)sSpeedo.u16k2SpeedoFactor;		
		vCANOdoUpdate((u16)temp);	
		
		//Calculate Target position 	
		temp = u16GaugeMaxMicroSteps(eSpdMeter);
		
		temp =	(temp * data)/SPEEDO_RANGE;		
		
		if(temp > u16GaugeMaxMicroSteps(eSpdMeter))
			temp = u16GaugeMaxMicroSteps(eSpdMeter); 
		
		vSetTargetPosition((u16)temp,eSpdMeter);

				
			
	}*/
	
}



/*********************************************************************//**
 *
 * Vehicle speed
 *
 * @param	None       
 *
 * @return	Current vehical speed      
 *************************************************************************/
u16 u16GetVehicleSpeed(void)
{
	return sSpeedo.u16Value;
}

/*********************************************************************//**
 *
 * Check vehicle over speed 
 *
 * @param	None       
 *
 * @return	Over speed warning status  
 *************************************************************************/
bool bOverSpeedStatus(void)
{
	static bool status = 0;
	
	if(sSpeedo.u16Value >= sSpeedo.u8OverSpeed)
		status = TRUE;
	else if(sSpeedo.u16Value <= (sSpeedo.u8OverSpeed - OVER_SPEED_WAR_OFF_HYS) )
		status = FALSE;
	return(status);
}

/*********************************************************************//**
 *
 * Set maximum frequency for speedo 
 *
 * @param	Vehicle pulses       
 *
 * @return	None
 *************************************************************************/
void vSetSpeedoMaxFreq(u16 pulses)
{
 u32 temp = 0;

  temp = pulses;
  temp = (temp*SPEEDO_RANGE)/3600; 
  
  sSpeedo.u16SpeedoMaxfreq = (u16)temp*10; // Frequency scale by 10

 //Speedo cut off freq is >10% more  
  sSpeedo.u16CutOffFreq = (sSpeedo.u16SpeedoMaxfreq + (sSpeedo.u16SpeedoMaxfreq*10)/100);
	
}

/*********************************************************************//**
 *
 * Set Vehicle over speed 
 *
 * @param	Vehicle speed   
 *
 * @return	None
 *************************************************************************/
void vSetVehicleOverSpeed(u8 speed)
{
	if(speed > OVER_SPEED_WAR_OFF_HYS )
		sSpeedo.u8OverSpeed = speed;
	else
		sSpeedo.u8OverSpeed = OVER_SPEED_WAR_OFF_HYS + 1;
}

/*********************************************************************//**
 *
 * Speedo Bias Factor 
 *
 * @param	Bias Factor
 *
 * @return	None
 *************************************************************************/
void vSetSpeedoBiasFactor(u8 BiasFactor)
{
	
	fBiasFactor = BiasFactor;
	fBiasFactor = (fBiasFactor *1)/128; //Resolution of Bias Factor
	
}


/*********************************************************************//**
 *
 * Set speedo offset
 *
 * @param	Speedo offset
 *
 * @return	None
 *************************************************************************/

void vSetSpeedoOffset(u8 speedoOffset)
{

	sSpeedo.u8SpeedOffset = speedoOffset;

}






