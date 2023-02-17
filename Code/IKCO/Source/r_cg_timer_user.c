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

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_timer_user.c
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for TAU module.
* Creation Date: 18/09/2018
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
//#pragma interrupt INTTM01 r_tau0_channel1_interrupt
//#pragma interrupt INTTM03 r_tau0_channel3_interrupt
//#pragma interrupt INTTM04 r_tau0_channel4_interrupt
//#pragma interrupt INTTM05 r_tau0_channel5_interrupt
//#pragma interrupt INTTM16 r_tau1_channel6_interrupt

/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "J1939.h"
#include "StepperDriver.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "Afe.h" 
#include "Event.h"
#include "LCDScreens.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU1_ch6 pulse measurement */
volatile uint32_t g_tau1_ch6_width = 0UL;
/* Start user code for global. Do not edit comment generated here */

// Local declarations --------------------------------------------------------


static u32 u32CaptureValue = 0;
static u16 u16SpeedoPulses = 0;
static bool bCaptureSignalStatus = 0;
static bool bOdoEnableFlag = 0;
static bool bSensorStatus = 0;
static bool bTripAOdoRolloverFlag = 0;
static bool bTripBOdoRolloverFlag =0;
static bool bTripAHourRolloverFlag = 0;
static bool bTripBHourRolloverFlag =0;

#define LCD_DIGIT_FULL 9999999


/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function is INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

//__interrupt static void r_tau0_channel4_interrupt(void)
void r_tau0_channel4_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */

	//1 ms timer for system clock
	
	++gu32Clock1msTicker;
    
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function is INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

//__interrupt static void r_tau0_channel1_interrupt(void)
void r_tau0_channel1_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */

	//100us timer interrupt 
	#define SKIP_STEPS  4

	static signed char step[eNumOfMeters]={-1,-1,-1,-1};
	short goingClockWise;
	static int countDown[eNumOfMeters];
	signed char var;
	unsigned char num;
	num = 0;
	// Do nothing if in position
	while(num < eNumOfMeters)
	{
		// num = activeMotorNo;
		if (stepperCB[num].s16CurrentTarget != stepperCB[num].s16CurrentPosition && bMoveStepperMotor[num] == TRUE)
		{
			// Using 100usec constant period clock
			if (--countDown[num] <= 0)
			{
				// Reload the count down value
				countDown[num] = stepperCB[num].s16CountdownReload;

				// Set direction flag and check
				if (!(goingClockWise = ((stepperCB[num].s16CurrentTarget - stepperCB[num].s16CurrentPosition) >= 0)))
				{
					// Going CCW so predecrement the step
					if(bStartUp == FALSE) 
					{
						// Update the absolute position
						stepperCB[num].s16CurrentPosition--;
						step[num] = step[num] - 1;
						if (step[num] < 0)
						step[num] = STEP_ARRAY_SIZE - 1;
					}
					else 
					{
						step[num] = step[num] - SKIP_STEPS; 
						stepperCB[num].s16CurrentPosition -= SKIP_STEPS;
						if (step[num] < 0)
						step[num] = STEP_ARRAY_SIZE - SKIP_STEPS + 1; //i have no idea why it works better with +1  
					}
					
					 if(stepperCB[num].s16CurrentPosition < stepperCB[num].S16ZeroOffSet)
					 	stepperCB[num].s16CurrentPosition = stepperCB[num].S16ZeroOffSet;
					
				}
				else
				{

					if(bStartUp == FALSE) 
					{
						// Update the absolute position
						stepperCB[num].s16CurrentPosition++;
						step[num] = step[num] + 1;    
					}
					else 
					{
						step[num] = step[num] + SKIP_STEPS;
						stepperCB[num].s16CurrentPosition += SKIP_STEPS;
					}
					// Post increment the step
					if (step[num] >= STEP_ARRAY_SIZE)
						step[num] = 0;
					
                    if(stepperCB[num].s16CurrentPosition > stepperCB[num].s16MaxNeedlePos)
						stepperCB[num].s16CurrentPosition = stepperCB[num].s16MaxNeedlePos;
				}
				//SPD_TablePtr = step;
				var = step[num];

				switch(num) 
				{

					case eSpdMeter :
						MCMP30 	=  MicroStepPWM[var].u16Mtr1Pwm;	//  PWM value comparition resister(sin) 
						MCMP31 	=  MicroStepPWM[var].u16Mtr2Pwm;	//  PWM value comparition resister(cos)
						MCMPC3 	=  MicroStepPWM[var].u8Polarity;
					break;

					case eTachometer : 
						MCMP10 	=  MicroStepPWM[var].u16Mtr1Pwm;	//  PWM value comparition resister(sin) 
						MCMP11 	=  MicroStepPWM[var].u16Mtr2Pwm;	//  PWM value comparition resister(cos)
						MCMPC1 	=  MicroStepPWM[var].u8Polarity; 		
					break;
					case eFuelGauge : 
						MCMP20 	=  MicroStepPWM[var].u16Mtr1Pwm;	//  PWM value comparition resister(sin) 
						MCMP21 	=  MicroStepPWM[var].u16Mtr2Pwm;	//  PWM value comparition resister(cos)
						MCMPC2 	=  MicroStepPWM[var].u8Polarity;
					break;

					case eTempGauge : 
						MCMP40 	=  MicroStepPWM[var].u16Mtr1Pwm;	//  PWM value comparition resister(sin) 
						MCMP41 	=  MicroStepPWM[var].u16Mtr2Pwm;	//  PWM value comparition resister(cos)
						MCMPC4 	=  MicroStepPWM[var].u8Polarity;				
					break;				


				};//end switch

				stepperCB[num].s16CountsFromStart++;
			}//end if (--countDown[num]
		}//end if (stepperCB[num].s16CurrentTarget 
		num++;
	}//end while


    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt 1 second Timer Interval.
* Description  : This function is INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

//__interrupt static void r_tau0_channel3_interrupt(void)
void r_tau0_channel3_interrupt(void)
{
    
    /* End user code. Do not edit comment generated here */
}

/*****************************************************************************
* Function Name: r_tau0_channel2_interrupt
* Description  : This function is INTTM02 interrupt service routine.
* Arguments    : None
* Return Value : None
******************************************************************************/
//__interrupt static void r_tau0_channel5_interrupt(void)
void r_tau0_channel5_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */

//Count power on second ,count until clock data not read from internal flash
	if(sClock.bPowerONCountFlag)
		sClock.u8PwrOnSecond++;
	else
	{
		
		if(++sClock.u8Second >= 60)	
		{
			sClock.u8Second = 0;		
			if(++sClock.u8Minute >= 60)
			{
				sClock.u8Minute = 0;
				if(++sClock.u8Hour > 12)
				{
					sClock.u8Hour = 1;
					
				}
				if(sClock.u8Hour == 12)
					sClock.bAmPmFlag = ~sClock.bAmPmFlag;
			}
		}
	}

	//**************************	Hour Meter Interrupt routine   **********************************//

	if(sHourMeter.bHourMeterEnable)
	{
		if(++sHourMeter.u16HrMtrSeconds >= SIX_MINUTE_SECONDS)
		{				
			sHourMeter.bFrUpdateHourMtr = TRUE;
			sHourMeter.u16HrMtrSeconds = 0;
			if(sHourMeter.u32HourMeter < LCD_DIGIT_FULL) //Locked Hour meter after reached Max value 9999999
			{
				if( ++sHourMeter.u8HrFractionValue > HOUR_FRACTION_VALUE)
				{
					sHourMeter.u8HrFractionValue = 0;
					++sHourMeter.u32HourMeter;
					
					//Save Hours in main when Hours <= HOUR_MAX_RANGE/10 otherwise Save Hours while Ign Off
					if(sHourMeter.u32HourMeter <= HOUR_MAX_RANGE )
						sHourMeter.bUpdateHourMtr = TRUE;
				}
			}
		}

		
		if((++sHourMeter.u16HrTripASeconds) >= SIX_MINUTE_SECONDS)
		{	        
			sHourMeter.bFrUpdateHourTripA = TRUE;
			sHourMeter.u16HrTripASeconds = 0;
			
			if( ++sHourMeter.u8HrTripAFractionValue > HOUR_FRACTION_VALUE)
			{
				sHourMeter.u8HrTripAFractionValue = 0;
				if( ++sHourMeter.u32HourTripA > HR_TRIPA_MAX_RANGE ) 
				{
					sHourMeter.u32HourTripA = 0;
					sAfe.u8EEPROMOffsetSaveFlagA = FALSE;
					bTripAHourRolloverFlag = TRUE;
				}
				sHourMeter.bUpdateHourTripA = TRUE;
			}
		}

		
		if(++sHourMeter.u16HrTripBSeconds >= SIX_MINUTE_SECONDS)
		{
			sHourMeter.u16HrTripBSeconds = 0;			
			sHourMeter.bFrUpdateHourTripB = TRUE;
			
			if( ++sHourMeter.u8HrTripBFractionValue > HOUR_FRACTION_VALUE)
			{
				sHourMeter.u8HrTripBFractionValue = 0;
				
				if( ++sHourMeter.u32HourTripB > HR_TRIPB_MAX_RANGE) 
				{
					sHourMeter.u32HourTripB = 0;
					sAfe.u8EEPROMOffsetSaveFlagB = FALSE;
					bTripBHourRolloverFlag = TRUE;
				}
				sHourMeter.bUpdateHourTripB = TRUE;
			}
		}
	}
     /* End user code. Do not edit comment generated here */
}
   
/***********************************************************************************************************************
* Function Name: r_tau1_channel6_interrupt
* Description  : This function is INTTM16 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_tau1_channel6_interrupt(void)
void r_tau1_channel6_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */

	//Capture timer is running 250000Hz and capture every falling edge
	if ((TSR16 & _0001_TAU_OVERFLOW_OCCURS) == 1U)    /* overflow occurs */
	{            
		g_tau1_ch6_width = (uint32_t)(TDR16 + 1UL) + 0x10000UL;
	}
	else
	{
		g_tau1_ch6_width = (uint32_t)(TDR16 + 1UL);	
	}
	u32CaptureValue = g_tau1_ch6_width;
	bCaptureSignalStatus = TRUE;
	bSensorStatus = TRUE;
	if(bOdoEnableFlag)
	{
		if( ++sODO.u16OdoPulseCount >= u16SpeedoPulses)
		{
			sODO.u16OdoPulseCount = 0;			  
			if(sODO.u32OdoValue < LCD_DIGIT_FULL) //Odo locked after reaching Max value 
			{
				if( ++sODO.u8OdoFractionValue > ODO_FRACTION_VALUE )      //Incement in ODO fraction part
				{
					sODO.u8OdoFractionValue = 0;
					++sODO.u32OdoValue;
					//Save Odo in main when odo <= ODO_MAX_RANGE/10 otherwise Save Odo while Ign Off
					if(sODO.u32OdoValue <= ODO_MAX_RANGE )
						sODO.bOdoUpdate = TRUE;
				}
			}
			   

		}

		if( ++sODO.u16TripAPulseCount >= u16SpeedoPulses)
		{
			sODO.u16TripAPulseCount = 0;
			if( ++sODO.u8TripAFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
			{
				sODO.u8TripAFractionValue = 0;
				if( ++sODO.u32TripAValue > TRIPA_MAX_RANGE)         	//Check trip recycle to zero
				{
					sAfe.u8EEPROMOffsetSaveFlagA = FALSE;
					sODO.u32TripAValue = 0;
					bTripAOdoRolloverFlag = TRUE;
					
				}
				//sODO.bTripAUpdate = TRUE; 
			}
		}
		
		if( ++sODO.u16TripBPulseCount >= u16SpeedoPulses)
		{
			sODO.u16TripBPulseCount = 0;
			if( ++sODO.u8TripBFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
			{
				sODO.u8TripBFractionValue = 0;	    
				if(++sODO.u32TripBValue > TRIPB_MAX_RANGE)  		//Check trip recycle to zero
				{
					sAfe.u8EEPROMOffsetSaveFlagB = FALSE;
					sODO.u32TripBValue = 0;
					bTripBOdoRolloverFlag = TRUE;
				}
				//sODO.bTripBUpdate = TRUE; 
			}

		}
	}//End if(bOdoEnableFlag)
  /* End user code. Do not edit comment generated here */
}


u32 u32GetFreqCaptureValue(void)
{
	return u32CaptureValue;
}

bool bGetCaptureSignalStatus(void)
{
	return bCaptureSignalStatus;
}

bool bGetSensorStatus(void)
{
	return bSensorStatus;
}

void vSetSensorStatus(bool StatusFlag)
{
	bSensorStatus = StatusFlag;
}

void vSetCaptureSignalStatus(bool StatusFlag)
{
	bCaptureSignalStatus = StatusFlag;
}

void vSetSpeedoPulses(u16 pulses)
{
	u16SpeedoPulses = pulses/10;
}

void vSetAnalougeOdoEnable(bool status)
{
	bOdoEnableFlag = status;
}


bool bGetOdoTripARolloverStatus(void)
{
	return bTripAOdoRolloverFlag;
}
void vClearOdoTripARolloverStatus(void)
{
	bTripAOdoRolloverFlag = FALSE;
 }
bool bGetOdoTripBRolloverStatus(void)
{
	return bTripBOdoRolloverFlag;
}
void vClearOdoTripBRolloverStatus(void)
{
	bTripBOdoRolloverFlag = FALSE;
}
bool bGetHourTripARolloverStatus(void)
{
	return bTripAHourRolloverFlag;
}
void vClearHourTripARolloverStatus(void)
{
	bTripAHourRolloverFlag = FALSE;
 }
bool bGetHourTripBRolloverStatus(void)
{
	return bTripBHourRolloverFlag;
}
void vClearHourTripBRolloverStatus(void)
{
	bTripBHourRolloverFlag = FALSE;
}



