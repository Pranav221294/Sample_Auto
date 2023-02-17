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
#include "AirPressureGauge.h" 
#include "Adc.h" 
#include "CurveHeader.h"
#include "J1939.h"
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "TellTales.h"
#include "TML_14229_UDSH.h"



// Constant declarations -----------------------------------------------------
#define  APG_SIGNAL_AVG_SIZE			16
#define AP_WARNING_STATUS_ON_TIME 		1000

#define APG_SHORT_CIRCUIT  300
#define APG_SHORT_CKT_HYS  100

// Air Pressure front & Read  ON/OFF Macro for Analogue
								
#define APG_ANALOG_ON_BAR2			1040 
#define APG_ANALOG_ON_BAR4			1590 
#define APG_ANALOG_ON_BAR6			2130 
#define APG_ANALOG_ON_BAR8			2670 
#define APG_ANALOG_ON_BAR10			3220 
#define APG_ANALOG_ON_BAR12			3760 

#define AGP_BAR_OFF_THRESH			100 


#define APG_ANALOG_OFF_BAR2			APG_ANALOG_ON_BAR2 	- AGP_BAR_OFF_THRESH
#define APG_ANALOG_OFF_BAR4			APG_ANALOG_ON_BAR4  - AGP_BAR_OFF_THRESH
#define APG_ANALOG_OFF_BAR6			APG_ANALOG_ON_BAR6 	- AGP_BAR_OFF_THRESH
#define APG_ANALOG_OFF_BAR8			APG_ANALOG_ON_BAR8  - AGP_BAR_OFF_THRESH
#define APG_ANALOG_OFF_BAR10		APG_ANALOG_ON_BAR10 - AGP_BAR_OFF_THRESH
#define APG_ANALOG_OFF_BAR12		APG_ANALOG_ON_BAR12 - AGP_BAR_OFF_THRESH


// local declarations --------------------------------------------------------
typedef struct
{	
	 
	u16     u16ApgSignalValue;							// APG ADC value	        			
	u16     u16ApgSignalData[APG_SIGNAL_AVG_SIZE];		// Array for averaging signal samples
	u16     u16ApgPreviousSignal;      				
	u8      u8ApgbarNo; 
	bool    bSymboleStatus;
	bool    bWarningStatus;
	bool	bOpenCktFlag;
	bool	bShortCktFlag;
	eAPG_BUZZER_TYPE u8ApgBuzzer;
	
	 
} APG_DATA_TYPE;

APG_DATA_TYPE sAPF = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,eNoBuzzer };
APG_DATA_TYPE sAPR = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,eNoBuzzer };

/*********************************************************************//**
 *
 * Average of ADC samples and calculate APG Level
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/

void vAPGFrontDataProcessing()
{	 
	u16 data = 0;
	u16 AdcValue = 0;
	u8 i = 0;	
	static u8   AvgCount = 0; 
	static u16 ShortCktSenseDelay =0;
	static u16 OpenCktSenseDelay =0;
/*
	//When control Air pressure gauge by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.FrontApgEnableFlag ==  TRUE)
	{
		//set true so that APG symbole display during APG gauge checking by IO control service 
		sAPF.bSymboleStatus = TRUE; 	
		/* Must assign in Previous Signal greater value to NO of BAR ,So that previous & Current value of BAR do not match when resume in normal mode 
		   and level is recalculated even without change in input
		sAPF.u16ApgPreviousSignal = 0xFFU; 
		//Set Short/OPEN ckt flag FLASE otherwise if gauge in short/Open condition then warning LED misbehave 
		sAPF.bShortCktFlag = FALSE; 		
		sAPF.bOpenCktFlag = FALSE;
		return;
	}a+
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	
	//Check ADC sample is reading or not 
	if(bGetAdcSampleStatus(eAP1_CH1) == FALSE)
		return;
	vResetAdcSampleStatus(eAP1_CH1);
	//Take ADC value	
	AdcValue = u16GetAdcValue(eAP1_CH1);
	*/
	AdcValue = u16GetAdcValue(eAP1_CH1);

	vGetCurveDataValue(eCapacitiveFuelVolt,AdcValue,&data);



//****Open /Short checking ************

	if( (APG_SHORT_CIRCUIT + APG_SHORT_CKT_HYS)  < data)
	{
		sAPF.bShortCktFlag = FALSE;
		 ShortCktSenseDelay = u16GetClockTicks();
	}	
	else if( (APG_SHORT_CIRCUIT >= data ) || sAPF.bShortCktFlag == TRUE)
	{
		if(u16GetClockTicks() - ShortCktSenseDelay > OPEN_SHORT_SENSE_TIME)
		{
			data = 0;
			sAPF.bShortCktFlag = TRUE; 	 
		}
		
	}
	
/*
	if( (OPEN_CIRCUIT - OPEN_CKT_HYS)  > data)
	{
		sAPF.bOpenCktFlag = FALSE;
		OpenCktSenseDelay = u16GetClockTicks();
	}	
	else if( (OPEN_CIRCUIT <= data) || sAPF.bOpenCktFlag == TRUE)
	{		  
	    if(u16GetClockTicks() - OpenCktSenseDelay > OPEN_SHORT_SENSE_TIME)
		{
			data = 0;
			sAPF.bOpenCktFlag = TRUE;		 
		}
	    else
		{
			return;
		}
	   
	}
	*/
	
	//***** If open /short ckt found then set blink status APG lcd symbole********

	if(sAPF.bShortCktFlag  || sAPF.bOpenCktFlag)
	{
		
		if( bGetLampBlinkingFlagstatus()) //Get indicator On/OFF flag status 
		{			  
			sAPF.bSymboleStatus = TRUE;			  
		}
		else
		{
			sAPF.bSymboleStatus = FALSE;				
		}
		
	}
	else
	{
		sAPF.bSymboleStatus = TRUE;
	}
	
	//**************************************
			
	if(eGetClusterStatus() == eClusterIgnitionReset)
	{
	for(i = 0; i < APG_SIGNAL_AVG_SIZE; i++)
		sAPF.u16ApgSignalData[i]  = data;
	}

	sAPF.u16ApgSignalData[AvgCount] = data;
	//Average of ADC samples
	
	data = 0;
	for(i = 0; i < APG_SIGNAL_AVG_SIZE; i++)
		data = data + sAPF.u16ApgSignalData[i];
	data = data / APG_SIGNAL_AVG_SIZE;
		
  	if(++AvgCount >= APG_SIGNAL_AVG_SIZE)
	{
		for(i = 0; i < APG_SIGNAL_AVG_SIZE - 1; i++)
			sAPF.u16ApgSignalData[i]=  sAPF.u16ApgSignalData[i+1];
		AvgCount = APG_SIGNAL_AVG_SIZE - 1;
	}
    sAPF.u16ApgSignalValue = data;
	//Check warning Lamp Status 
	if(sAPF.bShortCktFlag == FALSE && sAPF.bOpenCktFlag == FALSE)
	{
		if(sAPF.u16ApgSignalValue <= ( (u16)sAirPressureBytes.u8LowAirPressureOnThresh *10) )
			sAPF.bWarningStatus = TRUE;
		else if(sAPF.u16ApgSignalValue >= ((u16)sAirPressureBytes.u8LowAirPressureOffThresh*10))
			sAPF.bWarningStatus = FALSE;
	}
	else
	{
		sAPF.bWarningStatus = FALSE;
	}
		
	//if previous value and current value are not same then  check update for bars
	if(sAPF.u16ApgPreviousSignal !=  (u16)sAPF.u16ApgSignalValue)
	{
		sAPF.u16ApgPreviousSignal = (u16)sAPF.u16ApgSignalValue;
		//Ascending order
		if(sAPF.u8ApgbarNo ==0 && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR2)
			sAPF.u8ApgbarNo =0;
		else if(sAPF.u8ApgbarNo <=1 && sAPF.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR2 && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR4 )
			sAPF.u8ApgbarNo =1;
		else if(sAPF.u8ApgbarNo <=2 && sAPF.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR4 && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR6 )
			sAPF.u8ApgbarNo =2;
		else if(sAPF.u8ApgbarNo <=3 && sAPF.u16ApgPreviousSignal >=  APG_ANALOG_ON_BAR6  && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR8 )
			sAPF.u8ApgbarNo =3;
		else if(sAPF.u8ApgbarNo <=4 && sAPF.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR8 && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR10 )
			sAPF.u8ApgbarNo =4;
		else if(sAPF.u8ApgbarNo <=5 && sAPF.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR10 && sAPF.u16ApgPreviousSignal < APG_ANALOG_ON_BAR12 )
			sAPF.u8ApgbarNo =5;
		else if(sAPF.u8ApgbarNo <=6 && sAPF.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR12 )
			sAPF.u8ApgbarNo =6;

		//Descending order
		else if(sAPF.u8ApgbarNo > 0 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR2)
			sAPF.u8ApgbarNo = 0;
		else if(sAPF.u8ApgbarNo > 1 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR4)
			sAPF.u8ApgbarNo = 1;
		else if(sAPF.u8ApgbarNo > 2 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR6)
			sAPF.u8ApgbarNo = 2;
		else if(sAPF.u8ApgbarNo > 3 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR8)
			sAPF.u8ApgbarNo = 3;
		else if(sAPF.u8ApgbarNo > 4 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR10)
			sAPF.u8ApgbarNo = 4;
		else if(sAPF.u8ApgbarNo > 5 && sAPF.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR12 )
			sAPF.u8ApgbarNo = 5;
	}

}


/*********************************************************************//**
 *
 * Average of ADC samples and calculate APG Level
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/

void vAPGRearDataProcessing()
{	 
	u16 data = 0;
	u16 AdcValue = 0;
	u8 i = 0;	
	static u8   AvgCount = 0;
	static u16 ShortCktSenseDelay =0;
	static u16 OpenCktSenseDelay =0;
/*
	//When control Air pressure gauge by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.RearApgEnableFlag ==  TRUE)
	{
		//set true so that APG symbole display during APG gauge checking by IO control service 
		sAPR.bSymboleStatus = TRUE;
		//Must assign in Previous Signal greater value to NO of BAR ,So that previous & Current value of BAR could not match when resume in normal mode 
		sAPR.u16ApgPreviousSignal = 0xFFU; //assign greater value of no of bar so that preavious & Current value could not match 
		//Set Short/OPEN ckt flag FLASE otherwise if gauge in short/Open condition then warning LED misbehave 
		sAPR.bShortCktFlag = FALSE;
		sAPR.bOpenCktFlag = FALSE;
		return;
	}
	
	//Check ADC sample is reading or not 
	if(bGetAdcSampleStatus(eAP2_CH0) == FALSE)
		return;
	vResetAdcSampleStatus(eAP2_CH0);	
	//Take ADC value	
	AdcValue = u16GetAdcValue(eAP2_CH0);
	*/
	AdcValue = u16GetAdcValue(eAP2_CH0);
	vGetCurveDataValue(eCapacitiveFuelVolt,AdcValue,&data);

	//****Open /Short checking ************
	
	if( (APG_SHORT_CIRCUIT + APG_SHORT_CKT_HYS)  < data)
	{
		sAPR.bShortCktFlag = FALSE;
		 ShortCktSenseDelay = u16GetClockTicks();
	}	
	else if( (APG_SHORT_CIRCUIT >= data ) || sAPR.bShortCktFlag == TRUE)
	{
		if(u16GetClockTicks() - ShortCktSenseDelay > OPEN_SHORT_SENSE_TIME)
	   	{
	   	 	data = 0;
			sAPR.bShortCktFlag = TRUE;	     
	   	}
		
	}
		
	/*
	if( (OPEN_CIRCUIT - OPEN_CKT_HYS)  > data)
	{
		sAPR.bOpenCktFlag = FALSE;
		OpenCktSenseDelay = u16GetClockTicks();
	}   
	else if( (OPEN_CIRCUIT <= data) || sAPR.bOpenCktFlag == TRUE)
	{		  
	   if(u16GetClockTicks() - OpenCktSenseDelay > OPEN_SHORT_SENSE_TIME)
	   	{
	   	 	data = 0;
	   		sAPR.bOpenCktFlag = TRUE;	     
	   	}
	   else
		{
			return;
		}
	}
	*/	
		
	//***** If open /short ckt found then set blink status Apg lcd symbole********
  
	if(sAPR.bShortCktFlag  || sAPR.bOpenCktFlag)
	{		  
		if( bGetLampBlinkingFlagstatus()) //Get indicator On/OFF flag status 
		{			 
			sAPR.bSymboleStatus = TRUE;				 
		}
		else
		{
			sAPR.bSymboleStatus = FALSE;						
		}
	}
	else
	{
	   sAPR.bSymboleStatus = TRUE;
	}
	

	//**************************************
				
	if(eGetClusterStatus() == eClusterIgnitionReset)
		{
		for(i = 0; i < APG_SIGNAL_AVG_SIZE; i++)
			sAPR.u16ApgSignalData[i]  = data;
		}	
	
	//Average of ADC samples
	sAPR.u16ApgSignalData[AvgCount] = data;
		data = 0;
		for(i = 0; i < APG_SIGNAL_AVG_SIZE; i++)
			data = data + sAPR.u16ApgSignalData[i];
		data = data / APG_SIGNAL_AVG_SIZE;
		
	if(++AvgCount >= APG_SIGNAL_AVG_SIZE)
	{
		for(i = 0; i < APG_SIGNAL_AVG_SIZE - 1; i++)
			sAPR.u16ApgSignalData[i]=  sAPR.u16ApgSignalData[i+1];
		AvgCount = APG_SIGNAL_AVG_SIZE - 1;
	}
	sAPR.u16ApgSignalValue = data;
	//Check warning Lamp Status 
	if(sAPR.bShortCktFlag == FALSE && sAPR.bOpenCktFlag == FALSE)
  	{	
		if(sAPR.u16ApgSignalValue <= (u16)sAirPressureBytes.u8LowAirPressureOnThresh *10)
			sAPR.bWarningStatus = TRUE;
		else if(sAPR.u16ApgSignalValue >= ((u16)sAirPressureBytes.u8LowAirPressureOffThresh*10))
			sAPR.bWarningStatus = FALSE;
	}
	else
	{
		sAPR.bWarningStatus = FALSE;	
	}

	
	//if previous value and current value are not same then  check update for bars
	if(sAPR.u16ApgPreviousSignal !=  sAPR.u16ApgSignalValue)
	{
	sAPR.u16ApgPreviousSignal = sAPR.u16ApgSignalValue;
	//Ascending  inputs
	if(sAPR.u8ApgbarNo ==0 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR2)
		sAPR.u8ApgbarNo =0;
	else if(sAPR.u8ApgbarNo <=1 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR2 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR4 )
		sAPR.u8ApgbarNo =1;
	else if(sAPR.u8ApgbarNo <=2 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR4 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR6 )
		sAPR.u8ApgbarNo =2;
	else if(sAPR.u8ApgbarNo <=3 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR6 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR8 )
		sAPR.u8ApgbarNo =3;
	else if(sAPR.u8ApgbarNo <=4 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR8 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR10 )
		sAPR.u8ApgbarNo =4;
	else if(sAPR.u8ApgbarNo <=5 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR10 && sAPR.u16ApgPreviousSignal < APG_ANALOG_ON_BAR12 )
		sAPR.u8ApgbarNo =5;
	else if(sAPR.u8ApgbarNo <=6 && sAPR.u16ApgPreviousSignal >= APG_ANALOG_ON_BAR12 )
		sAPR.u8ApgbarNo =6;

	//Descending  inputs

	else if(sAPR.u8ApgbarNo > 0 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR2)
		sAPR.u8ApgbarNo = 0;
	else if(sAPR.u8ApgbarNo > 1 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR4)
		sAPR.u8ApgbarNo = 1;
	else if(sAPR.u8ApgbarNo > 2 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR6 )
		sAPR.u8ApgbarNo = 2;
	else if(sAPR.u8ApgbarNo > 3 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR8 )
		sAPR.u8ApgbarNo = 3;
	else if(sAPR.u8ApgbarNo > 4 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR10 )
		sAPR.u8ApgbarNo = 4;
	else if(sAPR.u8ApgbarNo > 5 && sAPR.u16ApgPreviousSignal <= APG_ANALOG_OFF_BAR12 )
		sAPR.u8ApgbarNo = 5;

	}	

}

/*********************************************************************//**
*
* Air Pressure gauge Front bar current status
*
*
* @param	None      
*
* @return	Front APG bar number        
*************************************************************************/
u8 u8ApfBarNo(void)
{

	if(sAirPressureBytes.u8APNumOfBars == eApgDigit10)
	  {
		  if(sAPF.u8ApgbarNo > 5)
			  sAPF.u8ApgbarNo = 5; 
	  }
	  else if(sAirPressureBytes.u8APNumOfBars == eApgDigit12)
	  {
		  if(sAPF.u8ApgbarNo > 6)
			  sAPF.u8ApgbarNo = 6; 
	  }
	  else if(sAirPressureBytes.u8APNumOfBars == eApgDigit15)
	  {
		  if(sAPF.u8ApgbarNo > 7)
			  sAPF.u8ApgbarNo = 7; 
	  }



	if(sAPF.bShortCktFlag  || sAPF.bOpenCktFlag)
	{
		sAPF.u8ApgbarNo = BADDATA;
	}
	/*
	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eDisable
	||sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eHydraulic)
	{
	 sAPF.u8ApgbarNo = ERRORDATA;
	}
*/
	return sAPF.u8ApgbarNo;
}

/*********************************************************************//**
*
* Air Pressure gauge Rear bar current status
*
*
* @param	None      
*
* @return	Rear APG bar number      
*************************************************************************/
u8 u8AprBarNo(void)
{

   if(sAirPressureBytes.u8APNumOfBars == eApgDigit10)
   	{
   		if(sAPR.u8ApgbarNo > 5)
			sAPR.u8ApgbarNo = 5; 
   	}
    else if(sAirPressureBytes.u8APNumOfBars == eApgDigit12)
   	{
   		if(sAPR.u8ApgbarNo > 6)
			sAPR.u8ApgbarNo = 6; 
   	}
	else if(sAirPressureBytes.u8APNumOfBars == eApgDigit15)
   	{
   		if(sAPR.u8ApgbarNo > 7)
			sAPR.u8ApgbarNo = 7; 
   	}

	if(sAPR.bShortCktFlag  || sAPR.bOpenCktFlag)
	{
		sAPR.u8ApgbarNo = BADDATA;
	}
/*
		if(sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eDisable
		||sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eHydraulic)
		{
		 sAPR.u8ApgbarNo = ERRORDATA;
		}
*/
	return sAPR.u8ApgbarNo;
}



/*********************************************************************//**
*
* Set Air Pressure gauge Rear bar. Used in IO control
*
*
* @param	None      
*
* @return	None     
*************************************************************************/

void vSetRearApgBarNo(u8 BarNo)
{
	//APG can display up to 7 Bar,so  APG bar no should not more than 7 
	u8 NumOfBar = 0U;
	if(BarNo > 7U)
	{
		NumOfBar = 7U;
	}
	else
	{
		NumOfBar = BarNo;
	}

	sAPR.u8ApgbarNo = NumOfBar;
}

/*********************************************************************//**
*
* Set Air Pressure gauge Front bar. Used in IO control
*
*
* @param	None      
*
* @return	None     
*************************************************************************/

void vSetFrontApgBarNo(u8 BarNo)
{
	u8 NumOfBar = 0U;
	if(BarNo > 7U)
	{
		NumOfBar = 7U;
	}
	else
	{
		NumOfBar = BarNo;
	}

	sAPF.u8ApgbarNo = NumOfBar;
}



/*********************************************************************//**
*
* Get Front Apg Symbole status
*
*
* @param	None      
*
* @return	Front Apg Symbole status
*************************************************************************/
bool bGetFrontApgSymbolStatus()
{
	return sAPF.bSymboleStatus;
}

/*********************************************************************//**
*
* Get Rear Apg Symbole status
*
*
* @param	None      
*
* @return	Rear Apg Symbole status
*************************************************************************/
bool bGetRearApgSymbolStatus()
{
	return sAPR.bSymboleStatus;
}

/*********************************************************************//**
*
* Get Apg Warning lamp status
*
*
* @param	None      
*
* @return	Apg Warning lamp status
*************************************************************************/
u8 u8APGWarningStatus()
{
 	static u8 LampStatus =0;
	static u16 WarningOnTimeStamp = 0;
	
	
	if(sAPF.bWarningStatus == TRUE || sAPR.bWarningStatus == TRUE)
	{
		
		if(u16GetClockTicks()- WarningOnTimeStamp > AP_WARNING_STATUS_ON_TIME)
		{
			LampStatus = eWarningOn;
			//This logic added becouse in EOL sheet buzzer disable separate bits for APF & APR
			if(sAPF.bWarningStatus == TRUE)
			{
			 	sAPF.u8ApgBuzzer = eAPGBuzzerActive;
			}
			else
			{
				sAPF.u8ApgBuzzer = eNoBuzzer;
			}

			if(sAPR.bWarningStatus == TRUE)
			{
			 	sAPR.u8ApgBuzzer = eAPGBuzzerActive;
			}
			else
			{
				sAPR.u8ApgBuzzer = eNoBuzzer;
			}			
		}
	}
	else
	{
		LampStatus = eWarningOff;
		sAPF.u8ApgBuzzer = eNoBuzzer;
		sAPR.u8ApgBuzzer = eNoBuzzer;
		WarningOnTimeStamp = u16GetClockTicks();
	}
		
	return LampStatus;	
}

/*********************************************************************//**
*
* Get Front APG Buzzer status 
*
*
* @param	None      
*
* @return	Front APG Buzzer status 
*************************************************************************/
eAPG_BUZZER_TYPE eFrontApgBuzzerStatus()
{
 return sAPF.u8ApgBuzzer;
}

/*********************************************************************//**
*
* Get Rear APG Buzzer status 
*
*
* @param	None      
*
* @return	Rear APG Buzzer status 
*************************************************************************/
eAPG_BUZZER_TYPE eRearApgBuzzerStatus()
{
 return sAPR.u8ApgBuzzer;
}




