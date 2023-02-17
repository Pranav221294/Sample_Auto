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
#include "OilPressureGauge.h" 
#include "Adc.h" 
#include "CurveHeader.h"
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "TellTales.h"
#include "J1939.h"
#include "TML_14229_UDSH.h"



// Constant declarations -----------------------------------------------------

#define OIL_GAUGE_SIGNAL_AVG				16


// Oil Pressure Gauge Bar ON/OFF Macro for Analogue
									
									
#define OIL_ANALOG_ON_BAR1				480U	//ohm*10  
#define OIL_ANALOG_ON_BAR2				820U 	//ohm*10  
#define OIL_ANALOG_ON_BAR3				1160U	//ohm*10  
#define OIL_ANALOG_ON_BAR4				1510U	//ohm*10  
#define OIL_ANALOG_ON_BAR5				1840U 	//ohm*10  
  
#define ANALOG_BAR_OFF_HYS 100

#define OIL_ANALOG_OFF_BAR1				OIL_ANALOG_ON_BAR1 - ANALOG_BAR_OFF_HYS 	//ohm*10  
#define OIL_ANALOG_OFF_BAR2				OIL_ANALOG_ON_BAR2 - ANALOG_BAR_OFF_HYS  	//ohm*10  
#define OIL_ANALOG_OFF_BAR3				OIL_ANALOG_ON_BAR3 - ANALOG_BAR_OFF_HYS  	//ohm*10  
#define OIL_ANALOG_OFF_BAR4				OIL_ANALOG_ON_BAR4 - ANALOG_BAR_OFF_HYS  	//ohm*10  
#define OIL_ANALOG_OFF_BAR5				OIL_ANALOG_ON_BAR5 - ANALOG_BAR_OFF_HYS  	//ohm*10  


// Oil Gauge Bar ON/OFF Macro for CAN									
								//J1939 data									
#define OIL_CAN_ON_BAR1				0x32 //200kpa		 
#define OIL_CAN_ON_BAR2				0x64 //400kpa	  
#define OIL_CAN_ON_BAR3				0x96 //600kpa	 
#define OIL_CAN_ON_BAR4				0xC8 //800kpa	 
#define OIL_CAN_ON_BAR5				0xFA //1000kpa	  
#define HYS_OFF						0x02


#define OIL_CAN_OFF_BAR1			OIL_CAN_ON_BAR1 - HYS_OFF	 
#define OIL_CAN_OFF_BAR2			OIL_CAN_ON_BAR2 - HYS_OFF	 
#define OIL_CAN_OFF_BAR3			OIL_CAN_ON_BAR3 - HYS_OFF	 
#define OIL_CAN_OFF_BAR4			OIL_CAN_ON_BAR4 - HYS_OFF	 
#define OIL_CAN_OFF_BAR5			OIL_CAN_ON_BAR5 - HYS_OFF	 



// local declarations --------------------------------------------------------

typedef struct
{		 
	u16     u16OilSignalValue;							// APG ADC value	        			
	u16     u16OilSignalData[OIL_GAUGE_SIGNAL_AVG];		// Array for averaging signal samples
	u16     u16OilPreviousSignal;      				
	u8      u8OilbarNo; 
	bool 	bSymboleStatus;
	bool	bOpenCktFlag;
	bool	bShortCktFlag;
	 
} OIL_PRESSURE_DATA_TYPE;

OIL_PRESSURE_DATA_TYPE	sOilPressure = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*********************************************************************//**
 *
 * Average of ADC samples and calculate Oil Level
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/
 
void vOilPressureGauge()
{	

	u16 data = 0;
	u16 AdcValue = 0;
	u8 i = 0;	
	static u8   AvgCount = 0;
	static u16 Timestamp = 0;
	static u16 ShortCktSenseDelay =0;
	static u16 OpenCktSenseDelay =0;

		//When control Oil pressure gauge by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.OilPressureEnableFlag ==  TRUE)
	{
		//Must Assign greater value of no of bar ,So that preavious & Current value could not match other wise Bar will not update
		sOilPressure.u16OilPreviousSignal = 0xFFU; 
		//Set Symbole flag true so that symbole display during control  by IO control service
		sOilPressure.bSymboleStatus = TRUE;
		return;
	}

	if(sIndicatorInputType.Inputsbytes.Byte1.bOilGauge == eAnalouge)	
	{
			//Check ADC sample is reading or not 
			if(bGetAdcSampleStatus(eOIL_PRESS_CH4) == FALSE)
				return;
			vResetAdcSampleStatus(eOIL_PRESS_CH4);
			//Take ADC value	
			AdcValue = u16GetAdcValue(eOIL_PRESS_CH4);

			vGetCurveDataValue(eResCurve,AdcValue,&data);
			
		//****Open /Short checking ***************************************
	
		if( (SHORT_CIRCUIT + SHORT_CKT_HYS)  < data)
		{
			sOilPressure.bShortCktFlag = FALSE;
			 ShortCktSenseDelay = u16GetClockTicks();
		}	
		else if( (SHORT_CIRCUIT >= data ) || sOilPressure.bShortCktFlag == TRUE)
		{
			if(u16GetClockTicks() - ShortCktSenseDelay > OPEN_SHORT_SENSE_TIME)
		   	{
		   	 	data = 0;
				sOilPressure.bShortCktFlag = TRUE;	     
		   	}
			
		}
		
	
		if( (OPEN_CIRCUIT - OPEN_CKT_HYS)  > data)
		{
			sOilPressure.bOpenCktFlag  = FALSE;
			OpenCktSenseDelay = u16GetClockTicks();
		}   
		else if( (OPEN_CIRCUIT <= data) || sOilPressure.bOpenCktFlag  == TRUE)
		{		  
		   if(u16GetClockTicks() - OpenCktSenseDelay > OPEN_SHORT_SENSE_TIME)
		   	{
		   	 	data = 0;
		   		sOilPressure.bOpenCktFlag  = TRUE;	     
		   	}
			else
				{
					return;
				}
		}
		
	  //**************************************************************
		  
		   if(sOilPressure.bShortCktFlag  || sOilPressure.bOpenCktFlag)
		   {
			   if( bGetLampBlinkingFlagstatus()) //Get indicator On/OFF flag status 
			   {			 
					sOilPressure.bSymboleStatus = TRUE;
				}
				else
			   {
				   sOilPressure.bSymboleStatus = FALSE;
			   }

		   }
		   else
		   {
			   sOilPressure.bSymboleStatus = TRUE;
		   }
			
			
		//****************************************			
			if(eGetClusterStatus() == eClusterIgnitionReset)
				{
				for(i = 0; i < OIL_GAUGE_SIGNAL_AVG; i++)
					sOilPressure.u16OilSignalData[i]  = data;
				}	
				sOilPressure.u16OilSignalData[AvgCount] = data;
			//Average of ADC samples	
				sOilPressure.u16OilSignalValue = 0;
				for(i = 0; i < OIL_GAUGE_SIGNAL_AVG; i++)
					sOilPressure.u16OilSignalValue = sOilPressure.u16OilSignalValue + sOilPressure.u16OilSignalData[i];
				sOilPressure.u16OilSignalValue = sOilPressure.u16OilSignalValue / OIL_GAUGE_SIGNAL_AVG;
				
				if(++AvgCount >= OIL_GAUGE_SIGNAL_AVG)
				{
					for(i = 0; i < OIL_GAUGE_SIGNAL_AVG - 1; i++)
						sOilPressure.u16OilSignalData[i]=  sOilPressure.u16OilSignalData[i+1];
					AvgCount = OIL_GAUGE_SIGNAL_AVG - 1;
				}

				
			if(sOilPressure.u16OilPreviousSignal !=  (u16)sOilPressure.u16OilSignalValue)
			{
				sOilPressure.u16OilPreviousSignal = sOilPressure.u16OilSignalValue;
				//Ascending inputs
				if(sOilPressure.u8OilbarNo ==0 && sOilPressure.u16OilPreviousSignal < OIL_ANALOG_ON_BAR1)
					sOilPressure.u8OilbarNo =0;
				else if(sOilPressure.u8OilbarNo <=1 && sOilPressure.u16OilPreviousSignal >= OIL_ANALOG_ON_BAR1 && sOilPressure.u16OilPreviousSignal < OIL_ANALOG_ON_BAR2 )
					sOilPressure.u8OilbarNo =1;
				else if(sOilPressure.u8OilbarNo <=2 && sOilPressure.u16OilPreviousSignal >= OIL_ANALOG_ON_BAR2 && sOilPressure.u16OilPreviousSignal < OIL_ANALOG_ON_BAR3 )
					sOilPressure.u8OilbarNo =2;
				else if(sOilPressure.u8OilbarNo <=3 && sOilPressure.u16OilPreviousSignal >= OIL_ANALOG_ON_BAR3 && sOilPressure.u16OilPreviousSignal < OIL_ANALOG_ON_BAR4 )
					sOilPressure.u8OilbarNo =3;
				else if(sOilPressure.u8OilbarNo <=4 && sOilPressure.u16OilPreviousSignal >= OIL_ANALOG_ON_BAR4 && sOilPressure.u16OilPreviousSignal < OIL_ANALOG_ON_BAR5 )
					sOilPressure.u8OilbarNo =4;
				else if(sOilPressure.u8OilbarNo <=5 && sOilPressure.u16OilPreviousSignal >= OIL_ANALOG_ON_BAR5 )
					sOilPressure.u8OilbarNo =5;
				
				
				//Descending inputs
				else if(sOilPressure.u8OilbarNo > 0 && sOilPressure.u16OilPreviousSignal <= OIL_ANALOG_OFF_BAR1)
					sOilPressure.u8OilbarNo = 0;
				else if(sOilPressure.u8OilbarNo > 1 && sOilPressure.u16OilPreviousSignal <= OIL_ANALOG_OFF_BAR2)
					sOilPressure.u8OilbarNo = 1;
				else if(sOilPressure.u8OilbarNo > 2 && sOilPressure.u16OilPreviousSignal <= OIL_ANALOG_OFF_BAR3)
					sOilPressure.u8OilbarNo = 2;
				else if(sOilPressure.u8OilbarNo > 3 && sOilPressure.u16OilPreviousSignal <= OIL_ANALOG_OFF_BAR4)
					sOilPressure.u8OilbarNo = 3;
				else if(sOilPressure.u8OilbarNo > 4 && sOilPressure.u16OilPreviousSignal <= OIL_ANALOG_OFF_BAR5)
					sOilPressure.u8OilbarNo = 4;
				
				
			}

		}
		else
			{
				//Take CAN data
				data = u32GetPGNdata(eJEngineOilPressure);
				//Check Timer stamps for CAN msg is coming or Not
				if(bGetPGNTimeOut(eJEngineOilPressure) || (u16)data >= BADDATA )	
				{
					data = 0;
					
					if( bGetLampBlinkingFlagstatus()) //Get indicator On/OFF flag status 
					{			 
						sOilPressure.bSymboleStatus = TRUE;
					}
					else
					{
						sOilPressure.bSymboleStatus = FALSE;
					}
				}
				else
				{
					sOilPressure.bSymboleStatus = TRUE;	
					//Go back if new sample is not coming
					if(! bGetPGNDataReady(eJEngineOilPressure) )
						return;	
					ClearPGNDataReady(eJEngineOilPressure);
				}

				//****************************************			
				if(eGetClusterStatus() == eClusterIgnitionReset)
				{
					for(i = 0; i < 8; i++)
						sOilPressure.u16OilSignalData[i]  = data;
				}
				
				sOilPressure.u16OilSignalData[AvgCount] = data;
			//Average of ADC samples	
				
				data = 0;
				for(i = 0; i < 8; i++)
					data = data + sOilPressure.u16OilSignalData[i];
				
				data = data / 8;
				
				if(++AvgCount >= 8)
				{					
					AvgCount = 0;
				}
				sOilPressure.u16OilSignalValue = data;

				
				if(sOilPressure.u16OilPreviousSignal !=  (u16)sOilPressure.u16OilSignalValue)
				{
					sOilPressure.u16OilPreviousSignal = sOilPressure.u16OilSignalValue;
					//Ascending inputs
					if(sOilPressure.u8OilbarNo ==0 && sOilPressure.u16OilPreviousSignal < OIL_CAN_ON_BAR1)
						sOilPressure.u8OilbarNo =0;
					else if(sOilPressure.u8OilbarNo <=1 && sOilPressure.u16OilPreviousSignal >= OIL_CAN_ON_BAR1 && sOilPressure.u16OilPreviousSignal < OIL_CAN_ON_BAR2 )
						sOilPressure.u8OilbarNo =1;
					else if(sOilPressure.u8OilbarNo <=2 && sOilPressure.u16OilPreviousSignal >= OIL_CAN_ON_BAR2 && sOilPressure.u16OilPreviousSignal < OIL_CAN_ON_BAR3 )
						sOilPressure.u8OilbarNo =2;
					else if(sOilPressure.u8OilbarNo <=3 && sOilPressure.u16OilPreviousSignal >= OIL_CAN_ON_BAR3 && sOilPressure.u16OilPreviousSignal < OIL_CAN_ON_BAR4 )
						sOilPressure.u8OilbarNo =3;
					else if(sOilPressure.u8OilbarNo <=4 && sOilPressure.u16OilPreviousSignal >= OIL_CAN_ON_BAR4 && sOilPressure.u16OilPreviousSignal < OIL_CAN_ON_BAR5 )
						sOilPressure.u8OilbarNo =4;
					else if(sOilPressure.u8OilbarNo <=5 && sOilPressure.u16OilPreviousSignal >= OIL_CAN_ON_BAR5 )
						sOilPressure.u8OilbarNo =5;
					

					//Descending inputs
					else if(sOilPressure.u8OilbarNo > 0 && sOilPressure.u16OilPreviousSignal <= OIL_CAN_OFF_BAR1)
						sOilPressure.u8OilbarNo = 0;
					else if(sOilPressure.u8OilbarNo > 1 && sOilPressure.u16OilPreviousSignal <= OIL_CAN_OFF_BAR2)
						sOilPressure.u8OilbarNo = 1;
					else if(sOilPressure.u8OilbarNo > 2 && sOilPressure.u16OilPreviousSignal <= OIL_CAN_OFF_BAR3)
						sOilPressure.u8OilbarNo = 2;
					else if(sOilPressure.u8OilbarNo > 3 && sOilPressure.u16OilPreviousSignal <= OIL_CAN_OFF_BAR4)
						sOilPressure.u8OilbarNo = 3;
					else if(sOilPressure.u8OilbarNo > 4 && sOilPressure.u16OilPreviousSignal <= OIL_CAN_OFF_BAR5)
						sOilPressure.u8OilbarNo = 4;
					
				}
			
			}
			
	

		

}

/*********************************************************************//**
*
* Oil Pressure gauge bar current status
*
*
* @param	None       
*
* @return	Oil Pressure bar number
*************************************************************************/
u8 u8OilPressureBarNo(void)
{
	return sOilPressure.u8OilbarNo;
}

/*********************************************************************//**
*
* Set Oil Pressure gauge bar 
*
*
* @param	None       
*
* @return	Oil Pressure bar number
*************************************************************************/

void vSetOilPressureBarNo(u8 BarNo)
{
	u8 NumOfBar = 0;
	// LCD have a limite to display max 5 bar only 
	 if(BarNo > 5U)
	 {
		 NumOfBar = 5U;
	 }
	 else
	 {
	 	NumOfBar = BarNo;
	 }

	 sOilPressure.u8OilbarNo = NumOfBar;
}


/*********************************************************************//**
*
* Oil Pressure gauge symbol status
*
*
* @param 	None     
*
* @return   Oil Pressure symbol status  
*************************************************************************/
bool bOilPressureSymbolStatus(void)
{
	
	return sOilPressure.bSymboleStatus;
}
