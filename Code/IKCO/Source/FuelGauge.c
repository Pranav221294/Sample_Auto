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
#include "customTypedef.h"
#include "FuelGauge.h" 
#include "StepperDriver.h"
#include "CurveHeader.h"
#include "r_cg_userdefine.h"
#include "Eol.h"
#include "TML_14229_UDSH.h"


#define FUEL_COMMON_SUPPLY			P1.1 //for select capcitive and analouge fuel gauge 
#define DUAL_FUEL_INPUT             P5.5



// Constant declarations -----------------------------------------------------
#define FUEL_HYSTERESIS           	30U		//resistance*10
#define LOW_FUEL_LAMP_ON   			180U 	//resistance*10
#define LOW_FUEL_LAMP_OFF  			220U	//resistance*10	
#define FUEL_SIGNAL_AVG_SIZE		16U
#define FUEL_TANK2_AVG_SIZE			8U
#define WARNING_STATUS_ON_TIME		1000U
#define FULL_FUEL_CAN_DATA  		0xFAU	//J1939 data
#define FUEL_LEVEL_18	  			0x1FU	//J1939 data
#define EMPTY_FUEL  	  			0U		//J1939 data
#define FUEL_FULL_LEVEL				10000U   //100.00 
#define FUEL_MAX_RES				1800U	//resistance*10
#define FUEL_MIN_RES				100U		//resistance*10
#define FUEL_DATA_18	  			1250U	//fuel data for duel fuel tank 12.5%(1/8 postion)

//Macro used in antisloshing 

// 60 means  E to F in 10 minute i.e. 10000 level = 10 minute menas 1 level = 60 ms 
#define DAMP_TIME_NORMAL				60U	 	//ms 
#define DAMP_SIZE_NORMAL				1U		//%
#define DAMP_TIME_FAST					25U		//ms
#define DAMP_SIZE_FAST					13U		//%

#define MICROSTEPS_CHNAGE				30U 		//Microsteps



#define EMPTY_HIGH_LEVEL			170U		//Empty high level (Red zone end )for capacitive sensor ADC value for voltage 1V
#define CAP_SENSE_LAMP_OFF			187U	//Empty high level (Red zone end )for capacitive sensor warning lamp off hysterisis 1.1v ADC value 

#define FUEL_MAX_ADC				854U		//For Capacitive 5V ADC value 	
#define FUEL_MIN_ADC				84U		//For Capacitive 0.5v ADC value 

#define MAX_J1939_VALUE				250U		//data for 100% level for capacitive 

#define VOLTAGE_OFFSET				500U		//0.5v is 0% for capacitive fuel hense offset
#define SENSOR_VOLT_RANGE			45U		// total voltage rage up to 100% 5v-.5v = 4.5 *10 (SENSOR_MAX_VOLTAGE - VOLTAGE_OFFSET)/100
#define SENSOR_MAX_VOLTAGE			5000U	//voltage for 100% level
#define FUEL_LEVEL_CAN_RESOLUTION	4U	

#define OPEN_CONDITION_TRUE			33U		// Adc value for 200mV 
#define OPEN_CONDITION_FALSE		(OPEN_CONDITION_TRUE + FUEL_HYSTERESIS) 




// local declarations --------------------------------------------------------
typedef enum 
{
	ePrimaryMode =0,
	eSecondryMode,
}eDualFuelCycle;


eFuelTank2Steps eTank2Mode = eNormalTank2;
eFuelTank1Steps eTank1Mode = eNormalTank1;
eDualFuelCycle  eFuelCycleSteps = ePrimaryMode;


//ADC to VOLTAGE Lookup Table for Capacitive Sensor

typedef struct
{	
	u16     u16FuelResistance;	
	u16     u16FuelResistancePre;
	u16     u16FuelCapacitiveSensorADC;
	u32     u32FuelSignalData[FUEL_SIGNAL_AVG_SIZE];		// Array for averaging signal samples
	u16     u16FuelPreviousRes; 
	u16 	u16VoltValue;
	u8 		u8Fuellevel;
	bool	bOpenCktFlag;
	bool	bShortCktFlag;
	

} FUEL_DATA_TYPE;

typedef struct
{	
	u16     u16FuelData;
	u32     u32FuelSignalData[FUEL_TANK2_AVG_SIZE];		// Array for averaging signal samples
	u32 	u32FuelPercent;
	u8 		u8Fuellevel;
	bool	bSensorMissing;
	bool	bSensorError;
	bool	bDualFuelTankActive;

} FUEL_TANK2_TYPE;


//bSpeedoONOffWaitingTimeActive flag goes true for 2 minutes when speedo On then Off ,So keep all warning and pointer status same until its goes flase 
static bool bSpeedoONOffWaitingTimeActive = FALSE;
static bool bSpeedoONOffWaitingTank2Flag = FALSE;
static bool TankSwitchCycle = FALSE;
static bool DualSensorInputShortDetect = FALSE;

static FUEL_DATA_TYPE sFuel = { 0x00U,0x00U,0x00U,{0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U}, 
								0x00U,0x00U,0x00U,FALSE,FALSE};

static FUEL_TANK2_TYPE sFuelTank2 ={0x00U,0x00U,0x00U,0x00U,0x00U,FALSE,FALSE,FALSE};

// Private prototypes --------------------------------------------------------

/// @addtogroup FUEL_GAUGE_LIBRARY
/// @{
/// @defgroup FuelLib  Private functions used 
//// 
///   
/// @addtogroup FuelLib
/// @{

static u32 _u32AntiSloshValue(void);
static u8 _vCalcFuelLevel();
static void _vDualFuelTankDataProc(void);
static u32 _u32FuelTank2AntiSlosh(void); 
static void _vCheckDualFuelTank(void);
static void _vCheckDualFuelTankIn(void);
static void _vSenseDualFuelInput(void);




/// @} // endgroup FuelLib

/// @} // endgroup FUEL_GAUGE_LIBRARY


/*********************************************************************//**
 *
 *function have a resisitive ,CAN and Capaitive fuel gauge and work one of them gauge as per 
 *configure EOL.  calculate Resistance and voltage and according to input set stepper target position
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vFuelGauge()
{	
	//this should be a global macro in EOL

	u8 i = 0U;	
	u32 LevelData, data, oldFuelPer;
	static u8 AvgCount = 0U;	
	static u16 PreviousValue = 0U;
	static u16 ShortCktSenseDelay =0U;
	static u16 OpenCktSenseDelay =0U;	
	u32 value = 0U;

	//if fuel gauge is controlled by user through UDS, then do not execute this function
	if(sTestEquimentControl.IoControlFlag.bits.FuelGaugeFlag ==  TRUE)
	{
		return;
	}


	//if fuel gauge is driven by resistive or capacitive input
	if(sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge == eAnalouge ) 
	{
		
		if(bGetAdcSampleStatus(eFUEL_CH5) == FALSE)
		{
			return;	
		}
		//so that the same ADC reading is not used again
		vResetAdcSampleStatus(eFUEL_CH5);
		data = u16GetAdcValue(eFUEL_CH5);

		// take the same ADC reading in all the buffers if this is first entry after ignition is turned on.
		// So that single reading can be used to calculate fuel level and needle position can be set to 
		//actual target position immediately after ignition is turned on
		
		if(eGetClusterStatus() == eClusterIgnitionReset)
		{	
			for(i=0U;i<FUEL_SIGNAL_AVG_SIZE;i++)
			{
				sFuel.u32FuelSignalData[i] = data;	
			}
			if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)
  			{
				_vSenseDualFuelInput();
			}
		}
		else //rc else should be put here
		{
			sFuel.u32FuelSignalData[AvgCount] = data;
			if(++AvgCount >= FUEL_SIGNAL_AVG_SIZE )
			{
				AvgCount = 0U; 
			}
		}
		
		
		
		//find the average of last 16 readings
		for(i=0U, data = 0U; i < FUEL_SIGNAL_AVG_SIZE; i++)
		{
			data += sFuel.u32FuelSignalData[i];
		}
		data = 	data /FUEL_SIGNAL_AVG_SIZE;
			
		//if fuel gauge is driven by capacitive input
		if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)		// Capacitive
		{	
			//this is used to calculate data to be transmitted over CAN
			sFuel.u16FuelCapacitiveSensorADC = data;
			
			if(sFuel.u16FuelCapacitiveSensorADC > OPEN_CONDITION_FALSE )
			{
				sFuel.bOpenCktFlag = FALSE;
				OpenCktSenseDelay = u16GetClockTicks();
			}
			else if(sFuel.u16FuelCapacitiveSensorADC < OPEN_CONDITION_TRUE ) 
			{
				if( (u16GetClockTicks() - OpenCktSenseDelay ) > OPEN_SHORT_SENSE_TIME)
			   	{
					sFuel.bOpenCktFlag = TRUE;	
				}
			}
			else 
			{
				//do nothing
			}
			// if the fault condition exists, then prepare to transmit bad data over CAN
			// and prepare the needle to return to zero. 
			if ( sFuel.bOpenCktFlag == TRUE )
			{
				sFuel.u16FuelCapacitiveSensorADC = 0U;
				sFuel.u8Fuellevel = BADDATA;
			}
			else
			{
				//Fuel Level alculation for Capacitive fuel Sensor, 
				//Using Different ADC to voltage graph to calculate data for CAN.
				//the fuel level sent over CAN is not adjusted forsloshing
				_vCalcFuelLevel();
			}
			// adjust calculated fuel level for sloshing
			oldFuelPer = _u32AntiSloshValue(); 				
			//**************Second Fuel Tank*************************************
			 _vDualFuelTankDataProc();
				
			
			//*****************************************************************
		}
		else
		{			
			//Calculate resistance
			sFuelTank2.u8Fuellevel = ERRORDATA;
			vGetCurveDataValue(eResFuelCurve,(u16)data,&data);
			
			//****Open /Short checking ************
			// set the short/open circuit flags to true if the fault exists continuously for more than 1s
			// reset the timestamp everytime reading reading is in valid range so that fluctuations are filtered out 
			if( data > (SHORT_CIRCUIT + SHORT_CKT_HYS))
			{
				sFuel.bShortCktFlag = FALSE;
				ShortCktSenseDelay = u16GetClockTicks();
			}	
			else if( data <= SHORT_CIRCUIT )
			{
				if( (u16GetClockTicks() - ShortCktSenseDelay ) > OPEN_SHORT_SENSE_TIME)
			   	{
					sFuel.bShortCktFlag = TRUE;	     
			   	}				
			}
			else 
			{	
				
			}
					
			if( data < (OPEN_CIRCUIT - OPEN_CKT_HYS) )
			{
				sFuel.bOpenCktFlag = FALSE;
				OpenCktSenseDelay = u16GetClockTicks();
			}   
			else if( OPEN_CIRCUIT <= data)
			{		  
			   if( (u16GetClockTicks() - OpenCktSenseDelay ) > OPEN_SHORT_SENSE_TIME)
			   	{
			   		sFuel.bOpenCktFlag = TRUE;	     
			   	}
			}
			else 
			{	
				
			}

			// if the fault condition exists, then prepare to transmit bad data over CAN
			// and prepare the needle to return to zero
			if( (sFuel.bOpenCktFlag == TRUE ) || (sFuel.bShortCktFlag == TRUE) )
			{
				data  = 0;
				// sensor is not connected
				sFuel.u8Fuellevel = BADDATA ; //error value as per J1939 slot table
				oldFuelPer = _u32AntiSloshValue();
			}
			else if( (sFuel.bShortCktFlag  == FALSE) && (sFuel.bOpenCktFlag == FALSE  ) )
			{				
				sFuel.u16FuelResistance = data;						

				//Above 150 ohms, the change in resistance below 2 ohms is ignored.
				// this has been done due to bad ADC resolution from 150 ohms - 180 ohms
				if( (( sFuel.u16FuelResistance > sFuel.u16FuelResistancePre)? 
					( sFuel.u16FuelResistance - sFuel.u16FuelResistancePre) : 
					(sFuel.u16FuelResistancePre - sFuel.u16FuelResistance)) > 20U)
				{			 
					sFuel.u16FuelResistancePre = sFuel.u16FuelResistance;				 	
				}
				//After 150ohm resistance update if resistance change > 2 ohms(06/09/2019)
				//So that pointer flickering not observed 			
				if(sFuel.u16FuelResistance > 1500U )
				{
					sFuel.u16FuelResistance = sFuel.u16FuelResistancePre;
				}
		
				oldFuelPer = _u32AntiSloshValue();
					
				LevelData = (oldFuelPer * 5U) / 100U;				
				LevelData = LevelData >> 1; 
				sFuel.u8Fuellevel = LevelData;
			}
			else 
			{	
				//DO NOTHING......	
			}
			
		}
		
	}
	
	else  //Fuel gauge On CAN	
	{
	 	data = u32GetPGNdata(eJFuel);	
		//Check CAN message lost; this is a fault condition
		if( bGetPGNTimeOut(eJFuel) || (data >= BADDATA) )
		{
			// so that needle returns to zero
			data = 0U;
			sFuel.bOpenCktFlag = TRUE;
		}
		else
		{
			//Go back if new message not received yet	
			if(! bGetPGNDataReady(eJFuel) )
			{
				return;	
			}
			// clear the flag so that new message can be buffered
			ClearPGNDataReady(eJFuel);				
			sFuel.bOpenCktFlag = FALSE;			
			
		}
		// clear the average buffer 	
		if(eGetClusterStatus() == eClusterIgnitionReset)
		{
			for(i = 0U; i < 8U; i++)
			{
				sFuel.u32FuelSignalData[i]  = data;	
			}
		}
		else
		{
			sFuel.u32FuelSignalData[AvgCount] = data;
		}
		// calculate moving average of 8 samples
		data = 0U;	
		for(i = 0U; i < 8U; i++)
		{
			data = data + sFuel.u32FuelSignalData[i];
		}
		
		data = data / 8U; 
		
		if(++AvgCount >= 8U)
		{
			AvgCount = 0U;
		}
		
		sFuel.u8Fuellevel = data;
		oldFuelPer = _u32AntiSloshValue(); 
	
		data = (data* 5U)/2U;

		if(sFuel.bOpenCktFlag == TRUE)
		{			
			sFuel.u8Fuellevel = BADDATA; 
		}
		else
		{
		 	sFuel.u8Fuellevel = data;
		}
		
	
	}

   if(sFuelTank2.bDualFuelTankActive)
   {
   	 oldFuelPer = sFuelTank2.u32FuelPercent;
   }
	
	//Calculate Target position 
	value = u16GaugeMaxMicroSteps(eFuelGauge);
	value =  (value * oldFuelPer)/FUEL_FULL_LEVEL;

	// why were only 2 microsteps chosen ? don't know
	if(sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge != eCan )
	{
		//Move stepper if minimum microsteps change 
		if( (( (u16)value > PreviousValue)? ( (u16)value - PreviousValue) : (PreviousValue - (u16)value)) > 2)//MICROSTEPS_CHNAGE)
		{			 
			PreviousValue = value;				 	
		}
	}
	else
	{
		PreviousValue = value;
	}

	if(eTank2Mode != eTank2Step1 && eTank1Mode != eTank1Step1)
	{
		vSetTargetPosition(PreviousValue , eFuelGauge);
	}
	
}
/******************************************************************/

static void _vDualFuelTankDataProc(void)
{
	u32 data = 0;

	u8 i;
	static u8 AvgCount = 0;

     data = u32GetPGNdata(eJCapacitiveAft2);  
     
	 
   //Check CAN message lost; this is a fault condition
   if( bGetPGNTimeOut(eJCapacitiveAft2) || (data >= 0x27c4) )
   {
	   // so that needle returns to zero
	  
	    if(data == 0x27EC )
	   	{
	       sFuelTank2.bSensorError = TRUE;
		   sFuelTank2.u8Fuellevel = BADDATA;
	   	}
	    else
	   	{
	   		sFuelTank2.bSensorMissing = TRUE;
			sFuelTank2.u8Fuellevel = ERRORDATA;
	   	}
		 data = 0U;
   }
   else
   {
	   //Go back if new message not received yet   
	   if(! bGetSPNDataReady(eJCapacitiveAft2) )
	   {
		   return; 
	   }
	   // clear the flag so that new message can be buffered
	   vClearSPNDataReady(eJCapacitiveAft2);
	   
	   //Calulate Capacitive fuel gauge voltage/Level without any filter to broadcast over CAN ; 
	    vGetCurveDataValue(eSecondryFuelTank,data,&data);
		   
	   sFuelTank2.bSensorError = FALSE;
	   sFuelTank2.bSensorMissing = FALSE;
	   
   }

    // clear the average buffer 	
	if(eGetClusterStatus() == eClusterIgnitionReset)
	{
		for(i = 0U; i < FUEL_TANK2_AVG_SIZE; i++)
		{
			sFuelTank2.u32FuelSignalData[i]	= data; 
		}
	}
	else
	{
		sFuelTank2.u32FuelSignalData[AvgCount] = data;
	}
	// calculate moving average of 8 samples
	data = 0U;	
	for(i = 0U; i < FUEL_TANK2_AVG_SIZE; i++)
	{
		data = data + sFuelTank2.u32FuelSignalData[i];
	}
	
	data = data / FUEL_TANK2_AVG_SIZE; 
	
	if(++AvgCount >= FUEL_TANK2_AVG_SIZE)
	{
		AvgCount = 0U;
	}
	
	sFuelTank2.u16FuelData = (data/5U)*10U; // to make fuel level 100.00
	sFuelTank2.u32FuelPercent = _u32FuelTank2AntiSlosh(); 
	

	
	if( sFuelTank2.bSensorError == FALSE &&  sFuelTank2.bSensorMissing == FALSE )
	{		
		//Calculate fuel level in j1939 form
		data = (sFuelTank2.u32FuelPercent * 5U) / 100U;				
		data = data >> 1; 
		sFuelTank2.u8Fuellevel = data;
	}

	_vSenseDualFuelInput();
	_vCheckDualFuelTankIn();
	   
}


static void _vCheckDualFuelTankIn(void)
{

 s16 tempVar = 0;
 s16 tempVar1 = 0;
 
	if(sFuelTank2.bDualFuelTankActive == TRUE)
	{
		 switch(eTank2Mode)
		 {
			case eNormalTank2:
					eTank2Mode = eTank2Step1;					
				break;
			 case eTank2Step1:
			 	 //set pointer to home position and wait for pointer reached at home
			 	 vSetTargetPosition(0 , eFuelGauge);
				tempVar = s16GetCurrentPosition(eFuelGauge);
				if(tempVar == 0)
			 	{
			 	 eTank2Mode = eTank2Step2;
			 	}
			 break;
			case eTank2Step2:
				//Bring pointer to target position as per input of fuel tank 2
				tempVar = s16GetCurrentPosition(eFuelGauge);
				tempVar1 = s16GetTargetPosition(eFuelGauge);
				if(tempVar == tempVar1)
				{
				 eTank2Mode = eTank2Step3;
				}
			 break;
			case eTank2Step3:
			 	 //Run in normal mode 
			 	 eFuelCycleSteps = eSecondryMode;				
			 break;
		 	
		}
	}
		else
		{
			if(eFuelCycleSteps == eSecondryMode)
			{
				 switch(eTank1Mode)
				 {
					case eNormalTank1:
						eTank1Mode = eTank1Step1;					
					break;
					 case eTank1Step1:
					 	 //set pointer to home position and wait for pointer reached at home
					 	  vSetTargetPosition(0 , eFuelGauge);
						 tempVar = s16GetCurrentPosition(eFuelGauge);
							if(tempVar == 0)
						 	{
						 	 	eTank1Mode = eTank1Step2;
						 	}
					 break;
					case eTank1Step2:
					 	 //Bring pointer to target position as per input of fuel 
					 	tempVar = s16GetCurrentPosition(eFuelGauge);
						tempVar1 = s16GetTargetPosition(eFuelGauge);
						if(tempVar == tempVar1)
						{
						 	eTank1Mode = eTank1Step3;
						}
					 break;
					case eTank1Step3:
					 	 //Run in normal mode 
					 	 eFuelCycleSteps = ePrimaryMode;
						 eTank1Mode = eNormalTank1;
				 		 eTank2Mode = eNormalTank2;
					 break;	 	
				 }
			}
			else
			{
				 eTank1Mode = eNormalTank1;
				 eTank2Mode = eNormalTank2;
			}
		}
	
}
static u32 _u32FuelTank2AntiSlosh(void)
{
	static u32 updateRate, oldFuelPer, calcUpdateRate ;
	static s16 firstEntry = -1;
	u32  newFuelPer,tempFuel1;
	u32 speed, data;	
	u16 DiffrenceFuelPer = 0U;
	u32 DampingTime = 0U;	
	u16 DampingSize = 0U;
	static u32 PreFuelPer = 0U;
	static u32 SpeedOffFuelTimeStamp = 0U;
	static bool SpeedoOnThenOffFlag = FALSE;

	if( sFuelTank2.bSensorError == TRUE || sFuelTank2.bSensorMissing == TRUE )
	{
		newFuelPer = 0U;
		oldFuelPer = 0U;	
		// this will be used to get the needle immediately to zero so that needle indication
		// and lamp status are in sync. i.e when needle turns off, needle has also started to return to 0 
		firstEntry = -1;
		calcUpdateRate = u32GetClockTicks();
	}
	else
	{
		newFuelPer = sFuelTank2.u16FuelData;
	}

	

	if(oldFuelPer >= newFuelPer )
	{
		tempFuel1 = oldFuelPer - newFuelPer;
	}
	else
	{
		tempFuel1 = newFuelPer - oldFuelPer;
	}

	speed = u16GetVehicleSpeed();

	
		// Calculate diffrence b/w current and previous fuel level %
		if( newFuelPer >= PreFuelPer )
		{
			DiffrenceFuelPer =  (u16)(newFuelPer - PreFuelPer);			
		}
		else
		{
			DiffrenceFuelPer =  (u16)(PreFuelPer - newFuelPer);					
		}
		
		PreFuelPer = newFuelPer;
		
		if(DiffrenceFuelPer > 100U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*9U; //In ms 
			DampingSize = DAMP_SIZE_NORMAL;  //.xx%
		}		
		else if(DiffrenceFuelPer > 20U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*8U; //In ms 
			DampingSize = DAMP_SIZE_NORMAL; //.xx%
		}	
		else if(DiffrenceFuelPer > 10U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*7U; //In ms 			
			DampingSize = DAMP_SIZE_NORMAL;  //.xx%
		}	
		else 
		{
			DampingTime = DAMP_TIME_NORMAL*6U; //damping time 1 hour
			DampingSize = DAMP_SIZE_NORMAL;
		}
			
	
	
	//*****************************************************************************
	
	if((speed > 1U) &&  ((u32GetClockTicks() - calcUpdateRate) > DampingTime) )
	{
		if( tempFuel1 >= DampingSize)
		{
			if( oldFuelPer >= newFuelPer )
			{
				oldFuelPer = oldFuelPer - DampingSize;
			}
			else
			{
				oldFuelPer = oldFuelPer + DampingSize;
			}
		}

		calcUpdateRate = u32GetClockTicks();
		SpeedOffFuelTimeStamp = u32GetClockTicks();
		SpeedoOnThenOffFlag = TRUE;
		bSpeedoONOffWaitingTank2Flag= FALSE;
	}
	else if( (speed <= 1) && ( (u32GetClockTicks() - calcUpdateRate) > DAMP_TIME_FAST ) )
	{
		
		if( SpeedoOnThenOffFlag == FALSE )
		{
			if(tempFuel1 >= DAMP_SIZE_FAST)
			{
				if( oldFuelPer >= newFuelPer )
				{
					oldFuelPer = oldFuelPer - DAMP_SIZE_FAST;
				}
				else
				{
					oldFuelPer = oldFuelPer + DAMP_SIZE_FAST;
				}
			}
			bSpeedoONOffWaitingTank2Flag = FALSE;
		}
		else
		{
			//2 minute time , if speed on/off then fuel should come in normal mode after 2 min
		  	if( (u32GetClockTicks() - SpeedOffFuelTimeStamp) > 120000U) 
			{
				SpeedoOnThenOffFlag = FALSE;				
			}
			else
			{
				bSpeedoONOffWaitingTank2Flag = TRUE;
			}
		}
		
		calcUpdateRate = u32GetClockTicks();
	}
	
	if (firstEntry == -1)
	{
		oldFuelPer = newFuelPer;
	}
	firstEntry = 0U;
			
	if(oldFuelPer > FUEL_FULL_LEVEL)
	{
		oldFuelPer = FUEL_FULL_LEVEL;
	}
	return oldFuelPer;

}



static u32 _u32AntiSloshValue(void)
{

	static u32 updateRate, oldFuelPer, calcUpdateRate ;
	static s16 firstEntry = -1;
	u32  newFuelPer,tempFuel1;
	u32 speed, data;	
	u16 DiffrenceFuelPer = 0U;
	u32 DampingTime = 0U;	
	u16 DampingSize = 0U;
	static u32 PreFuelPer = 0U;
	static u32 SpeedOffFuelTimeStamp = 0U;
	static bool SpeedoOnThenOffFlag = FALSE;

	if(sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge != eCan )
	{

		if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)
		{

			if( sFuel.bOpenCktFlag == TRUE )
			{
				newFuelPer = 0U;
				oldFuelPer = 0U;	
				// this will be used to get the needle immediately to zero so that needle indication
				// and lamp status are in sync. i.e when needle turns off, needle has also started to return to 0 
				firstEntry = -1;
				calcUpdateRate = u32GetClockTicks();
			}
			else
			{
				vGetCurveDataValue(eCapacitiveFuelSensor,sFuel.u16FuelCapacitiveSensorADC,&newFuelPer);
			}
		}
		else
		{
			if( (sFuel.bOpenCktFlag == TRUE )|| (sFuel.bShortCktFlag == TRUE) )
			{
				newFuelPer = 0U;
				oldFuelPer = 0U;	
				firstEntry = -1;
				calcUpdateRate = u32GetClockTicks();
			}
			else
			{				
				//Calculate % level
				vGetCurveDataValue(eResLevel, sFuel.u16FuelResistance, &newFuelPer);				
			}
		}
	}
	else
	{
			
		if(sFuel.bOpenCktFlag == TRUE) //In CAN fuel sFuel.bOpenCktFlag true is bad data and can lost 
		{
			newFuelPer = 0U;
			oldFuelPer = 0U;	
			firstEntry = -1;
			calcUpdateRate = u32GetClockTicks();				
		}
		else
		{	
			data = 	sFuel.u8Fuellevel;
			newFuelPer = data*100U;
		}

	}

	if(oldFuelPer >= newFuelPer )
	{
		tempFuel1 = oldFuelPer - newFuelPer;
	}
	else
	{
		tempFuel1 = newFuelPer - oldFuelPer;
	}

	speed = u16GetVehicleSpeed();

	//For making speed CAN and analouge same 4 times increase damping time in case of Analouge & Capacitive fuel

	if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)
	{
		// Calculate diffrence b/w current and previous fuel level %
		if( newFuelPer >= PreFuelPer )
		{
			DiffrenceFuelPer =  (u16)(newFuelPer - PreFuelPer);			
		}
		else
		{
			DiffrenceFuelPer =  (u16)(PreFuelPer - newFuelPer);					
		}
		
		PreFuelPer = newFuelPer;
		
		if(DiffrenceFuelPer > 100U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*9U; //In ms 
			DampingSize = DAMP_SIZE_NORMAL;  //.xx%
		}		
		else if(DiffrenceFuelPer > 20U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*8U; //In ms 
			DampingSize = DAMP_SIZE_NORMAL; //.xx%
		}	
		else if(DiffrenceFuelPer > 10U) //.xx %
		{
			DampingTime = DAMP_TIME_NORMAL*7U; //In ms 			
			DampingSize = DAMP_SIZE_NORMAL;  //.xx%
		}	
		else 
		{
			DampingTime = DAMP_TIME_NORMAL*6U; //damping time 1 hour
			DampingSize = DAMP_SIZE_NORMAL;
		}
			
	}
	else if(sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge == eCan )
	{
		DampingTime = DAMP_TIME_FAST;
		DampingSize = DAMP_SIZE_FAST;
	}
	else //for Analouge fuel gauge 
	{
		DampingTime = DAMP_TIME_NORMAL*6U; //damping time 1 hour
		DampingSize = DAMP_SIZE_NORMAL;
	}
	//*****************************************************************************
	
	if((speed > 1U) &&  ((u32GetClockTicks() - calcUpdateRate) > DampingTime) )
	{
		if( tempFuel1 >= DampingSize)
		{
			if( oldFuelPer >= newFuelPer )
			{
				oldFuelPer = oldFuelPer - DampingSize;
			}
			else
			{
				oldFuelPer = oldFuelPer + DampingSize;
			}
		}

		calcUpdateRate = u32GetClockTicks();
		SpeedOffFuelTimeStamp = u32GetClockTicks();
		SpeedoOnThenOffFlag = TRUE;
		bSpeedoONOffWaitingTimeActive = FALSE;
	}
	else if( (speed <= 1) && ( (u32GetClockTicks() - calcUpdateRate) > DAMP_TIME_FAST ) )
	{
		
		if( (sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge == eCan ) || (SpeedoOnThenOffFlag == FALSE) )
		{
			if(tempFuel1 >= DAMP_SIZE_FAST)
			{
				if( oldFuelPer >= newFuelPer )
				{
					oldFuelPer = oldFuelPer - DAMP_SIZE_FAST;
				}
				else
				{
					oldFuelPer = oldFuelPer + DAMP_SIZE_FAST;
				}
			}
			bSpeedoONOffWaitingTimeActive = FALSE;
		}
		else
		{
			//2 minute time , if speed on/off then fuel should come in normal mode after 2 min
		  	if( (u32GetClockTicks() - SpeedOffFuelTimeStamp) > 120000U) 
			{
				SpeedoOnThenOffFlag = FALSE;				
			}
			else
			{
				bSpeedoONOffWaitingTimeActive = TRUE;
			}
		}
		
		calcUpdateRate = u32GetClockTicks();
	}
	
	if (firstEntry == -1)
	{
		oldFuelPer = newFuelPer;
	}
	firstEntry = 0U;
			
	if(oldFuelPer > FUEL_FULL_LEVEL)
	{
		oldFuelPer = FUEL_FULL_LEVEL;
	}
	return oldFuelPer;

}

/*********************************************************************//**
 *
 *Low Fuel and Open /Short circuit warning status
 *
 *
 * @param      None    
 *
 * @return     Fuel lamp status 
 *************************************************************************/

eGauge_Lamp_TYPE eFuelWarningStatus()
{
	
	static eGauge_Lamp_TYPE lampStatus = eWarningOff;
	static u16 WarningOnTimeStamp = 0U;	
	static u8 PrevLampStatus = 0U;
	
	if(eGetClusterStatus() == eClusterIgnitionReset)
	{
		WarningOnTimeStamp = u16GetClockTicks();		
	}

	if(sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge)
	{
		//keep bSpeedoONOffWaitingTimeActive false is open/short ckt detect
		if( (sFuel.bShortCktFlag == TRUE ) || (sFuel.bOpenCktFlag  == TRUE) )
		{
			bSpeedoONOffWaitingTimeActive = FALSE; 
		}
		//This flag set when speed On or OFF and clear after 2 minute in anishloshing function 
		//if flag set then don't change warning led status 
		if(bSpeedoONOffWaitingTimeActive == FALSE) 
		{
			if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor != CAPACITIVE_FUEL_GAUGE)
			{
				if( (sFuel.bShortCktFlag == TRUE ) || ( sFuel.bOpenCktFlag  == TRUE) )
				{
				 	 lampStatus = eOpenCkt;
					 WarningOnTimeStamp = u16GetClockTicks();
				}
				else
				{
					if(sFuel.u16FuelResistance <= LOW_FUEL_LAMP_ON)
				 	{
						if( (u16GetClockTicks()- WarningOnTimeStamp ) > WARNING_STATUS_ON_TIME)
						{
				 		   lampStatus = eWarningOn;
						}
				 	}
					else if(sFuel.u16FuelResistance >= LOW_FUEL_LAMP_OFF)
				 	{
				 		lampStatus = eWarningOff;
						WarningOnTimeStamp = u16GetClockTicks();
				 	}			  		
				}
			}
			else
			{ 
                // For dual fuel gauge 
                if(sFuelTank2.bDualFuelTankActive == TRUE)
			     {
					if( sFuelTank2.bSensorError == TRUE || sFuelTank2.bSensorMissing == TRUE )
					{
						lampStatus = eOpenCkt;
					 	WarningOnTimeStamp = u16GetClockTicks();
					}
					else
					{
						if((sFuelTank2.u16FuelData <= FUEL_DATA_18)&&(sFuelTank2.u8Fuellevel <= FUEL_LEVEL_18))
						{
							if( (u16GetClockTicks()- WarningOnTimeStamp ) > WARNING_STATUS_ON_TIME)
							{
								lampStatus = eWarningOn;
							}

						}
						else
						{
							lampStatus = eWarningOff;
							WarningOnTimeStamp = u16GetClockTicks();
						}
					}
					
				}
				else
				{
				    // For capacitive fuel gauge 
					if(sFuel.bOpenCktFlag  == TRUE || DualSensorInputShortDetect == TRUE)
					{
					 	 lampStatus = eOpenCkt;
						 WarningOnTimeStamp = u16GetClockTicks();
					}
					else
					{
						if(sFuel.u16FuelCapacitiveSensorADC <= EMPTY_HIGH_LEVEL)
					 	{
							if( (u16GetClockTicks()- WarningOnTimeStamp ) > WARNING_STATUS_ON_TIME)
							{
					 		   lampStatus = eWarningOn;
							}
					 	}
						else if(sFuel.u16FuelCapacitiveSensorADC > CAP_SENSE_LAMP_OFF)
					 	{
					 		lampStatus = eWarningOff;
							WarningOnTimeStamp = u16GetClockTicks();
					 	}			  		
					}

			    }
			}
		}//End bSpeedoONOffWaitingTimeActive
	}//End if(sFuel.bInputType)
	else
	{
		if(sFuel.bOpenCktFlag == TRUE )   
		{
			lampStatus = eOpenCkt;
		 	WarningOnTimeStamp = u16GetClockTicks();
		}
		else
		{
			if(sFuel.u8Fuellevel <= FUEL_LEVEL_18)
			{
				if( (u16GetClockTicks()- WarningOnTimeStamp ) > WARNING_STATUS_ON_TIME)
				{
					lampStatus = eWarningOn;
				}

			}
			else
			{
				lampStatus = eWarningOff;
				WarningOnTimeStamp = u16GetClockTicks();
			}
		}
		
	}
	PrevLampStatus = lampStatus;
 return lampStatus;

}


/*********************************************************************//**
 *
 *				   		  ADC1 | Volt1 (table values)
 *	(input value)  u16ADCValue | u16VoltValue (the calculated output value)
 *                  	  ADC2 | Volt2 (table values)
 *
 *	               			(u16ADCValue - ADC1) * (Volt2 - Volt1)
 *          u16VoltValue =   ------------------------------------- + Volt1
 *                        			(ADC2 - ADC1)
 *
 *
 *	@param      None
 *		
 *	@return     None
 *
 *************************************************************************/

static u8 _vCalcFuelLevel()
{
	
	u32 temp = 0;

	//Calulate Capacitive fuel gauge voltage/Level without any filter to broadcast over CAN ; 
	vGetCurveDataValue(eCapacitiveFuelVolt,sFuel.u16FuelCapacitiveSensorADC,&temp);
	sFuel.u16VoltValue = (u16)temp;
	if(temp> VOLTAGE_OFFSET)
	{
		temp = temp - VOLTAGE_OFFSET;	
	}
	else
	{
		temp = 0;
	}

	temp = (temp * 10U)/SENSOR_VOLT_RANGE;
	sFuel.u8Fuellevel = (u8)(temp/FUEL_LEVEL_CAN_RESOLUTION);				
			
}


/*********************************************************************//**
 *
 * Send fuel level 
 *
 *
 * @param      None    
 *
 * @return     Fuel level %
 *************************************************************************/

 u8 u8GetFuelLevel()
 {
	 return sFuel.u8Fuellevel;
 }

 /*********************************************************************//**
  *
  * Send fuel level 
  *
  *
  * @param		None	
  *
  * @return 	Fuel level %
  *************************************************************************/
 
  u8 u8GetDualFuelLevel(void)
  {
  u8 temp = 0xff;

  	if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)
  	{
  		temp =  sFuelTank2.u8Fuellevel;
  	}

  
	  return temp;
  }

 

 /*********************************************************************//**
 *
 * current fuel resistance 
 *
 *
 * @param      None    
 *
 * @return     Fuel level %
 *************************************************************************/

 u16 u16GetFuelCurrentRes()
 {
	 return sFuel.u16FuelResistance;
 }

 
 
/*********************************************************************//**
 *
 * Send Volt Value 
 *
 *
 * @param      None    
 *
 * @return     Volt value in mV
 *************************************************************************/

 u16 u16GetVoltValue()
 {
 
	if( (sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == eDisable )
	 	|| ( sIndicatorInputType.Inputsbytes.Byte1.bFuelGauge == eCan ) )
	{
 		sFuel.u16VoltValue = 0xFFFU; //Tx gauge not available
	}
 
	 return sFuel.u16VoltValue;
 }


/*********************************************************************//**
 *
 * Set fuel Common Supply for selection capacitive and analouge fuel gauge 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vSetFuelCommonSupply()
{
	if(sIndicatorInputType.Inputsbytes.Byte4.bFuelSensor == CAPACITIVE_FUEL_GAUGE)
	{
		FUEL_COMMON_SUPPLY = FALSE;
	}
	else 
	{
		FUEL_COMMON_SUPPLY = TRUE;
	}
}

/*********************************************************************//**
 *
 * Sense dual fuel input 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vSenseDualFuelInput(void)
{
	static u32 TimeStampSwPressed =0;
	static u32 TimeStampSwReleased =0;
	static u32 TimeStampSw1Min =0;
	static u8 mscount = 0;
	

	
		if(DUAL_FUEL_INPUT == HIGH)
		{
			TimeStampSwReleased = u32GetClockTicks();
			if(DualSensorInputShortDetect == FALSE)
			{
				if(eFuelCycleSteps == ePrimaryMode )
				{
					if(u32GetClockTicks()-TimeStampSwPressed >500)
					{                  
					    sFuelTank2.bDualFuelTankActive = TRUE; 					    
					    TimeStampSw1Min = u32GetClockTicks();
					}
				}
				else 
				{
					//For 1min delay genration 
					if(u32GetClockTicks()-TimeStampSw1Min >60000)		
					{ 
						TimeStampSw1Min = u32GetClockTicks();
						sFuelTank2.bDualFuelTankActive = FALSE; 
						DualSensorInputShortDetect = TRUE;
					}
				}
			}
		}
		else
		{ 
			TimeStampSw1Min =u32GetClockTicks();
			TimeStampSwPressed = u32GetClockTicks();
			if(u32GetClockTicks()-TimeStampSwReleased >500)
			{
				if( (eFuelCycleSteps == eSecondryMode ) || (DualSensorInputShortDetect == TRUE) )
				{
					sFuelTank2.bDualFuelTankActive = FALSE;
					DualSensorInputShortDetect = FALSE;
				}
				
			}
		}
	
	
}

/*********************************************************************//**
 *
 * Fuel gaue switch from primary to secondry mode steps status for moving stepper fast
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bFuelGaugeSwitchMode(void)
{

 bool status = FALSE;
 
  if( (eTank2Mode == eTank2Step1) || (eTank2Mode == eTank2Step2)
 	 || (eTank1Mode == eTank1Step1) || (eTank1Mode == eTank1Step2)
 	)
	{
	 status = TRUE;
	}

	return status;
}



