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



/* MODULE StepperDriver */

#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "StepperDriver.h"
#include "r_cg_userdefine.h"
#include "J1939.h" 
#include "r_cg_wdt.h"
#include "LCDScreens.h"
#include "TML_14229_UDSH.h"
#include "FuelGauge.h" 



// constant declarations -----------------------------------------------------

#define RPM_MAX_COUNTER				12 //6
#define SPEED_MAX_COUNTER			9
#define FUEL_MAX_COUNTER			150
#define TEMP_MAX_COUNTER			150

#define TEMP_MAX_FIRST_ENTRY		10
#define FUEL_MAX_FIRST_ENTRY		10

#define MICRO_STEPS 				20
#define SHORT_HOME_USTEPS			200
#define MOVE_SLOW_MICROSTEPS		100	//micro steps


#define AFTER_FULL_SCALE_MOVE_GAP 	700 //in ms 


const MICROSTEP_PWM_TYPE  MicroStepPWM[STEP_ARRAY_SIZE] = 
{
	{0,		255,	0x12},
	{50,	250,	0x12},
	{98,	235,	0x12},
	{142,	211,	0x12},
	{181,	178,	0x12},
	{212,	139,	0x12},
	{236,	95,		0x12},
	{250,	47,		0x12},
	{255,	0,		0x13},
	{250,	50,		0x13},
	{235,	98,		0x13},
	{211,	142,	0x13},
	{178,	181,	0x13},
	{139,	212,	0x13},
	{95,	236,	0x13},
	{47,	250,	0x13},
	{0,		255,	0x10},
	{50,	250,	0x10},
	{98,	235,	0x10},
	{142,	211,	0x10},
	{181,	178,	0x10},
	{212,	139,	0x10},
	{236,	95,		0x10},
	{250,	47,		0x10},
	{255,	0,		0x11},
	{250,	50,		0x11},
	{235,	98,		0x11},
	{211,	142,	0x11},
	{178,	181,	0x11},
	{139,	212,	0x11},
	{95,	236,	0x11},
	{47,	250,	0x11},

};


/*
d=		2.4	steps/deg
vi=		10	steps/sec
vi=		4.167	deg/sec
vi^2=		100	steps/secsec
vmaxdeg=		300	deg/sec
adeg=		1600	deg/secsec
			
vmaxsteps=		2000	steps/sec
asteps(=a)=		3840	steps/secsec

*/
#define ACCEL_DECEL_STARTUP     127

const u8 accelDecelStartUpCurve[ACCEL_DECEL_STARTUP + 1] =
{
	204 ,85	,65	,55	,48	,44	,40	,37	,35	,33	,32	,
	30	,29	,28	,27	,26	,25	,24	,24	,23	,23	,22	,
	22	,21	,21	,20	,20	,19	,19	,19	,18	,18	,18	,
	18	,17	,17	,17	,17	,16	,16	,16	,16	,16	,15	,
	15	,15	,15	,15	,15	,15	,14	,14	,14	,14	,14	,
	14	,14	,13	,13	,13	,13	,13	,13	,13	,13	,13	,
	13	,12	,12	,12	,12	,12	,12	,12	,12	,12	,12	,
	12	,12	,11	,11	,11	,11	,11	,11	,11	,11	,11	,
	11	,11	,11	,11	,11	,11	,10	,10	,10	,10	,10	,
	10	,10	,10	,10	,10	,10	,10	,10	,10	,10	,10	,
	10	,10	,10	,10	,10	,9  ,9	,9	,9	,9	,9	,
	9	,9	,9	,9	,9	,9	,9
};

 /*

 d= 	 2.4 steps/deg	 this is still wrong because pointer moves 4 steps / 1.67 deg			 
 vi=	 1	 steps/sec	 initial velocity			 
 vi=	 0.417	 deg/sec			 
 vi^2=		 1	 steps/secsec				 
 vmaxdeg=		 200 deg/sec change these values to play with the table and use the values in red			 
 adeg=		 600 deg/secsec 			 
							 
 vmaxsteps= 	 480 steps/sec				 
 asteps(=a)=	 1440	 steps/secsec				 
 
 
  */
#define ACCEL_DECEL_DISTANCE  		114 //127
#define ACCEL_DECEL_DISTANCE_SLOW  	1023

const u16 accelDecelCurveExt[ACCEL_DECEL_DISTANCE_SLOW+1] =
{

300,	115,	88,	74,	65,	59,	54,	51,	47,	45,	43,
41,39,38,36,35,34,33,32,31,31,30,29,29,28,27,27,26,
26,25,25,25,24,24,24,23,23,23,22,22,22,21,21,21,21,
21,20,20,20,20,19,19,19,19,19,19,18,18,18,18,18,18,
17,17,17,17,17,17,17,17,16,16,16,16,16,16,16,16,16,
16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,
14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,3,3,3,3,3,3,3

//last value is made 4 & 3 that will be used as it is only for self-test. 
};

// Global declarations ------------------------------------------------------

bool bStartUp = TRUE;
bool bMoveStepperMotor[eNumOfMeters]= {TRUE, TRUE, TRUE, TRUE};
bool bSelfTestStatus = 0;
STEPPER_CONTROL_BLOCK_TYPE  stepperCB[eNumOfMeters];

// local declarations -------------------------------------------------------

static bool bacceleratingStartup[eNumOfMeters]= {TRUE,TRUE, TRUE, TRUE};

// private prototypes --------------------------------------------------------

/// @addtogroup Stepper_LIBRARY
/// @{
 
/// @defgroup StepperLib Local functions used in stepper driver
//// 
///   
/// @addtogroup StepperLib
/// @{

bool _bAccelDecelStartUp(bool accelerating,eMETER_TYPE meterType);
bool _bAccelDecel(bool accelerating,eMETER_TYPE meterType);
void _vDelay(void);

/// @} // endgroup StepperLib

/// @} // endgroup Stepper_LIBRARY



/*********************************************************************//**
*
* Handles positioning of the stepper.
* Movement is triggered by a difference in current and target position.
*
* @param    Gauge type	
*
* @return 	None
*************************************************************************/
 void vStepperPositionDriver(eMETER_TYPE meterType)
{
	u16 GapPos = 0;
	bool bManualSelfTestState= FALSE;
	u16 temp =0U;
	
	bManualSelfTestState = bGetDoSelfTestStatus();

	if(bManualSelfTestState == FALSE)
	{
		bManualSelfTestState = bIsECUResetByService();
	}

	temp = u16GetRoutineControlServiceStatus();				
	if(temp == CHECK_PROG_DEP)
	{
	 	bManualSelfTestState = TRUE;
	}
	// During self test and Ignition off RPM stepper motor move  by driver same as other stepper motor and 
	//in main processing stepper move by hit and trial method 
	if( meterType == eTachometer &&  bSelfTestStatus == TRUE && bGetIgnStatus() == TRUE && bManualSelfTestState == FALSE )
	{
		//Stepper driver enable to move smooth and fast RPM stepper .So only for RPM movement Refrence Six Stepper def DID 
		bMoveStepperMotor[eTachometer] = TRUE;
	
		if(stepperCB[meterType].s16TargetPosition > stepperCB[meterType].s16MaxNeedlePos )
			 stepperCB[meterType].s16TargetPosition = stepperCB[meterType].s16MaxNeedlePos;	
		
		if (stepperCB[eTachometer].s16TargetPosition > stepperCB[eTachometer].s16CurrentPosition)
			GapPos =  stepperCB[eTachometer].s16TargetPosition - stepperCB[eTachometer].s16CurrentPosition;
		else if (stepperCB[eTachometer].s16TargetPosition < stepperCB[eTachometer].s16CurrentPosition)
			GapPos =  stepperCB[eTachometer].s16CurrentPosition - stepperCB[eTachometer].s16TargetPosition;
			
   		//Six Stepper def DID have a 500us stepper timer interrupt and BSVI have 100us so counter digits * 5
					if(GapPos>384)
						stepperCB[eTachometer].s16CountdownReload = 10;//2;
					else if(GapPos>256)
						stepperCB[eTachometer].s16CountdownReload = 15;//3;
					else if(GapPos>128)
						stepperCB[eTachometer].s16CountdownReload = 25;//5;
					else if(GapPos>96)
						stepperCB[eTachometer].s16CountdownReload = 40;//8;
					else if(GapPos>64)
						stepperCB[eTachometer].s16CountdownReload = 75;//15;
					else if(GapPos>32)
						stepperCB[eTachometer].s16CountdownReload = 100;//20;
					else
						stepperCB[eTachometer].s16CountdownReload = 125;//25;
   
		stepperCB[eTachometer].s16CurrentTarget = stepperCB[eTachometer].s16TargetPosition;
					
	}
	
	else if( meterType == eSpdMeter &&  bSelfTestStatus == TRUE && bGetIgnStatus() == TRUE && bManualSelfTestState == FALSE )
	{
		//Stepper driver enable to move smooth and fast RPM stepper .So only for RPM movement Refrence Six Stepper def DID 
		bMoveStepperMotor[eSpdMeter] = TRUE;
	
		if(stepperCB[meterType].s16TargetPosition > stepperCB[meterType].s16MaxNeedlePos )
			 stepperCB[meterType].s16TargetPosition = stepperCB[meterType].s16MaxNeedlePos;	
		
		if (stepperCB[eSpdMeter].s16TargetPosition > stepperCB[eSpdMeter].s16CurrentPosition)
			GapPos =  stepperCB[eSpdMeter].s16TargetPosition - stepperCB[eSpdMeter].s16CurrentPosition;
		else if (stepperCB[eSpdMeter].s16TargetPosition < stepperCB[eSpdMeter].s16CurrentPosition)
			GapPos =  stepperCB[eSpdMeter].s16CurrentPosition - stepperCB[eSpdMeter].s16TargetPosition;
			
   		//Six Stepper def DID have a 500us stepper timer interrupt and BSVI have 100us so counter digits * 5
   		
					if(GapPos>320)
						stepperCB[eSpdMeter].s16CountdownReload = 20;//4;
					else if(GapPos>288)
						stepperCB[eSpdMeter].s16CountdownReload = 25;//8;
					else if(GapPos>256)
						stepperCB[eSpdMeter].s16CountdownReload = 30;//10;
					else if(GapPos>224)
						stepperCB[eSpdMeter].s16CountdownReload = 35;//15;
					else if(GapPos>192)
						stepperCB[eSpdMeter].s16CountdownReload = 40;//20;
					else if(GapPos>160)
						stepperCB[eSpdMeter].s16CountdownReload = 45;//25;
					else if(GapPos>128)
						stepperCB[eSpdMeter].s16CountdownReload = 50;//35;
					else if(GapPos>96)
						stepperCB[eSpdMeter].s16CountdownReload = 55;//45;
					else if(GapPos>64)
						stepperCB[eSpdMeter].s16CountdownReload = 70;//50;
					else if(GapPos>32)
						stepperCB[eSpdMeter].s16CountdownReload = 85;//50;
					else
						stepperCB[eSpdMeter].s16CountdownReload = 150;//60; 
						

					
   
		stepperCB[eSpdMeter].s16CurrentTarget = stepperCB[eSpdMeter].s16TargetPosition;
					
	}
	else
	{
	    enum 
		{
	        eNORMAL = 0,
	        eABRUPT_CHANGE,
	        eRAMP_DOWN
	    };
		static s16 stepperDriveState[eNumOfMeters]= {eNORMAL,eNORMAL,eNORMAL,eNORMAL};
		static s16 previousTarget[eNumOfMeters]= {-1,-1,-1,-1};
		static bool accelerating[eNumOfMeters]= {TRUE,TRUE,TRUE,TRUE};
		s16 distCur, distNew;

		if(stepperCB[meterType].s16TargetPosition > stepperCB[meterType].s16MaxNeedlePos )
			 stepperCB[meterType].s16TargetPosition = stepperCB[meterType].s16MaxNeedlePos;

		if (previousTarget[meterType]== -1)
		    previousTarget[meterType] = stepperCB[meterType].s16TargetPosition;

	    // If needle isn't moving, prepare for acceleration
	    if (stepperCB[meterType].s16CurrentTarget == stepperCB[meterType].s16CurrentPosition)
	    {
	        stepperCB[meterType].s16CountsFromStart = 0;
	        accelerating[meterType] = TRUE;

	        // Get the next position target
	        stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].s16TargetPosition;
	        previousTarget[meterType] = stepperCB[meterType].s16TargetPosition;
			bMoveStepperMotor[meterType] = FALSE;
	    }
	    else 
	    {
		   bMoveStepperMotor[meterType] = TRUE;
		   switch (stepperDriveState[meterType])
		    {
		        case eNORMAL :
					
		            // Check for a change in destination during a move	
					distCur = previousTarget[meterType] - stepperCB[meterType].s16TargetPosition;
					if(distCur < 0)
						distCur = -distCur;
		            if (distCur > MICRO_STEPS)
		            {
		                stepperDriveState[meterType] = eABRUPT_CHANGE;
						//accelerating[meterType] = FALSE;
		            }
		            else
		            {
		            	distCur = stepperCB[meterType].s16CurrentTarget - stepperCB[meterType].s16CurrentPosition;
						if(distCur < 0 )
							distCur = -distCur;
						accelerating[meterType] = _bAccelDecel(accelerating[meterType],meterType);
		            }
					
		        break;

		        case eABRUPT_CHANGE :
		            // Get distance to current and new targets
		            distCur = stepperCB[meterType].s16CurrentTarget - stepperCB[meterType].s16CurrentPosition;
		            distNew = stepperCB[meterType].s16TargetPosition - stepperCB[meterType].s16CurrentPosition;

		            // If going in the same direction and there is enough room for deceleration
		            if ((((distCur >= 0) && (distNew >= 0)) || ((distCur < 0) && (distNew < 0)))
		                && ((distCur < 0) ? (distCur <= -ACCEL_DECEL_DISTANCE) : (distCur >= ACCEL_DECEL_DISTANCE)))

					{	// Set the new position
		                stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].s16TargetPosition;
						previousTarget[meterType] = stepperCB[meterType].s16CurrentTarget;
						stepperDriveState[meterType] = eNORMAL;
					}
		            else
		            {	
		            	stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].s16CurrentPosition ;
		              	accelerating[meterType] = FALSE;
		            	stepperDriveState[meterType] = eRAMP_DOWN;
		               
						if(stepperCB[meterType].s16CurrentTarget > stepperCB[meterType].s16MaxNeedlePos )
							stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].s16MaxNeedlePos;
						else if (stepperCB[meterType].s16CurrentTarget < stepperCB[meterType].S16ZeroOffSet ) // this part was added later
							stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].S16ZeroOffSet;
					
		            }

		        break;

		        case eRAMP_DOWN :
		            if (accelerating[meterType] = _bAccelDecel(accelerating[meterType],meterType))
		            {
		                // Done
		                stepperCB[meterType].s16CurrentTarget = stepperCB[meterType].s16TargetPosition;
		                stepperDriveState[meterType] = eNORMAL;

		            }
		        break;
		    }
	    }
	}
    
}


 /*********************************************************************//**
 *
 *Controls the acceleration and deceleration of the
 * stepper motor using a lookup table indexed by distance
 * from target in counts. The accelerating flag is set false
 * here when the stepper comes up to full speed. It is set
 * true outside of this function.
 *
 * @param	 accelerating flag
 *			Gauge type
 *
 * @return	state of the accelerating flag
 *************************************************************************/

 bool _bAccelDecel(bool accelerating,eMETER_TYPE meterType)
{
    static u16 clkPeriod[eNumOfMeters] = {0,0,0,0};
    u16 newclkPeriod = 0;
    s16 distCur = 0;
	static bool FirstEntry = TRUE;
	bool moveStepperFast = FALSE;
	static u16 timestamp = 0;
	bool bManualSelfTestState = FALSE;
	u16 temp = 0U;

	bManualSelfTestState = bGetDoSelfTestStatus();
	if(bManualSelfTestState == FALSE)
	{
		bManualSelfTestState = bIsECUResetByService();
	}

	temp = u16GetRoutineControlServiceStatus();		
	
	if(temp == CHECK_PROG_DEP)
	{
	 	bManualSelfTestState = TRUE;
	}

	//FirstEntry checked for Fuel gauge so that after ignition ON pointer reached at destination fast.
	if(bSelfTestStatus == FALSE)
		timestamp = u16GetClockTicks();
	else if(u16GetClockTicks() - timestamp > 10000)
		FirstEntry = FALSE;

    // Get the absolute difference between current and target position
    if ((distCur = stepperCB[meterType].s16CurrentTarget - stepperCB[meterType].s16CurrentPosition) < 0)
        distCur = -distCur;
	

	if (accelerating)
        distCur = stepperCB[meterType].s16CountsFromStart;

	
	if( distCur <= ACCEL_DECEL_DISTANCE_SLOW )
	{
		if( bSelfTestStatus == TRUE && bGetIgnStatus() ==TRUE && bManualSelfTestState == FALSE)
		{
				
			if(meterType == eSpdMeter)
			{
				newclkPeriod = accelDecelCurveExt[distCur] * 3;		 
				if( newclkPeriod < SPEED_MAX_COUNTER )
					newclkPeriod = SPEED_MAX_COUNTER;
			}
			else if ( meterType == eTachometer )
			{
				newclkPeriod = accelDecelCurveExt[distCur] * 4; //*2 before pasco
				if( newclkPeriod < RPM_MAX_COUNTER )
					newclkPeriod = RPM_MAX_COUNTER;

				if( newclkPeriod >= 300 )						
					     newclkPeriod = 300;
				
				
												
			}	
			else if(meterType == eFuelGauge)
			{
				//To move fuel pointer fast first time after ignition ON 
				 moveStepperFast = bFuelGaugeSwitchMode();
				 if(FirstEntry == TRUE || moveStepperFast ==TRUE)
				{
				 	newclkPeriod = accelDecelCurveExt[distCur] * 3; 
					if( newclkPeriod < FUEL_MAX_FIRST_ENTRY )
						newclkPeriod = FUEL_MAX_FIRST_ENTRY;
				}
				else
				{
											
					newclkPeriod = accelDecelCurveExt[distCur] * 5;
					
					if( newclkPeriod < FUEL_MAX_COUNTER)
						newclkPeriod = FUEL_MAX_COUNTER;

					if( distCur <= MOVE_SLOW_MICROSTEPS )
					{
				     newclkPeriod = (FUEL_MAX_COUNTER*2);
					 if( distCur <= 40 ) //move slow 2 deg more
						newclkPeriod = (FUEL_MAX_COUNTER*5);
					}
					
				}
			}
			else if ( meterType == eTempGauge )
			{

			
				if(FirstEntry)
				{
					newclkPeriod = accelDecelCurveExt[distCur] * 3; 
					if( newclkPeriod < TEMP_MAX_FIRST_ENTRY )
						newclkPeriod = TEMP_MAX_FIRST_ENTRY; 
				}
				else
					{
					newclkPeriod = accelDecelCurveExt[distCur] * 3; 
					if( newclkPeriod < TEMP_MAX_COUNTER )
						newclkPeriod = TEMP_MAX_COUNTER; 
					
					if( distCur <= MOVE_SLOW_MICROSTEPS )						
					     newclkPeriod = TEMP_MAX_COUNTER * 2;
										
					}
				
									
			}
		}
		
	   	else	
		{
			newclkPeriod = accelDecelCurveExt[distCur]; 
					
		}
		
	}

    else
    {
          if( bSelfTestStatus == TRUE && bGetIgnStatus() ==TRUE && bManualSelfTestState == FALSE)
        {
			if(meterType == eSpdMeter)	
			{
			  newclkPeriod = SPEED_MAX_COUNTER;
			}
			else if(meterType == eTachometer)
				newclkPeriod = RPM_MAX_COUNTER; //8 before pasco
			else if(meterType == eFuelGauge)
			{
			  //To move fuel pointer fast first time after ignition ON
				 moveStepperFast = bFuelGaugeSwitchMode();
				if(FirstEntry == TRUE || moveStepperFast == TRUE)
					newclkPeriod = FUEL_MAX_FIRST_ENTRY;
				else					
				 	newclkPeriod = FUEL_MAX_COUNTER;
			}
			else if(meterType == eTempGauge)
				{

				if(FirstEntry)
					 newclkPeriod = TEMP_MAX_FIRST_ENTRY; 
					else
						newclkPeriod = TEMP_MAX_COUNTER;
						
				}

			
        }
	   	else			
	       newclkPeriod = accelDecelCurveExt[ACCEL_DECEL_DISTANCE_SLOW]; // max speed
	       
            
	    accelerating = FALSE;
    }

    if (clkPeriod[meterType] != newclkPeriod)
    {
        clkPeriod[meterType] = newclkPeriod;
        stepperCB[meterType].s16CountdownReload = clkPeriod[meterType];
    }
    return accelerating;
}


/*********************************************************************//**
 *
 *Controls the acceleration and deceleration of the
 * stepper motor using a lookup table indexed by distance
 * from target in counts. The accelerating flag is set false
 * here when the stepper comes up to full speed. It is set
 * true outside of this function.
 *
 * @param	 accelerating flag
 *			Gauge type
 *
 * @return	state of the accelerating flag
 *************************************************************************/
 bool _bAccelDecelStartUp(bool accelerating,eMETER_TYPE meterType)
{
    static u16 clkPeriod[eNumOfMeters] ={ 0,0};
    u16 newclkPeriod = 0;
    s16 distCur;	

    // Get the absolute difference between current and target position
    if ((distCur = stepperCB[meterType].s16CurrentTarget - stepperCB[meterType].s16CurrentPosition) < 0)
        distCur = -distCur;

    // Set the correct count to calculate
    if (accelerating)
        distCur = stepperCB[meterType].s16CountsFromStart;
	
    if (stepperCB[meterType].s16CountsFromStart < ACCEL_DECEL_STARTUP )
    {
        newclkPeriod = accelDecelStartUpCurve[stepperCB[meterType].s16CountsFromStart];
    }
	
    else if (FULL_STEPS_START_UP - stepperCB[meterType].s16CountsFromStart < ACCEL_DECEL_STARTUP)
    {
    	//dividing by 4 removes the jump at stepper inner stop but leads to loss of steps. so removed
        newclkPeriod = accelDecelStartUpCurve[FULL_STEPS_START_UP - stepperCB[meterType].s16CountsFromStart] ;
    }
    else
    {   // max speed
        newclkPeriod = accelDecelStartUpCurve[ACCEL_DECEL_STARTUP]; 
        accelerating = FALSE;
    }	
    if (clkPeriod[meterType] != newclkPeriod)
    {
        clkPeriod[meterType] = newclkPeriod;
        stepperCB[meterType].s16CountdownReload = clkPeriod[meterType];
    }
    return accelerating;
}

/*********************************************************************//**
 *
 *  Move all steppers together to full position and  home position ,
 *  during move Home position stop all stepper in mid ,
 *	During stepper self test if Ignition Off then move all stepper from its current position to
 *  home Position.
 *
 * @param	 None
 *
 * @return	 None
 *************************************************************************/

void vStepperSelfTest()
{
	bool IgnitionStatus = TRUE;
	u8 i = 0;
	
	bStartUp = FALSE;
	bSelfTestStatus = FALSE;
	//Move stepper to full position 
    stepperCB[eSpdMeter].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eSpdMeter].s16MaxNeedlePos = SPEEDO_MICROSTEPS_EXTRA;
	stepperCB[eSpdMeter].s16TargetPosition= SPEEDO_MICROSTEPS_EXTRA;	
	
	stepperCB[eTachometer].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTachometer].s16MaxNeedlePos = RPM_MICROSTEPS_EXTRA;
	stepperCB[eTachometer].s16TargetPosition = RPM_MICROSTEPS_EXTRA;
	
	stepperCB[eFuelGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eFuelGauge].s16MaxNeedlePos = FUEL_MICROSTEPS_EXTRA;
	stepperCB[eFuelGauge].s16TargetPosition = FUEL_MICROSTEPS_EXTRA;
	
	stepperCB[eTempGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTempGauge].s16MaxNeedlePos = TEMP_MICROSTEPS_EXTRA;
	stepperCB[eTempGauge].s16TargetPosition = TEMP_MICROSTEPS_EXTRA;
	
	for(i = eTachometer ; i < eNumOfMeters; i++)
		{
		stepperCB[i].s16CountsFromStart = 0;	//required for a slow start
		stepperCB[i].s16CurrentPosition = 0;		//Set Niddle current position to zero
		}  

	while((stepperCB[eSpdMeter].s16TargetPosition != stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition != stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition != stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition != stepperCB[eTempGauge].s16CurrentPosition
		
    	) )
    {
    	for(i = eTachometer; i < eNumOfMeters; i++)
			vStepperPositionDriver(i);

		R_WDT_Restart();

		vTaskDoneDuringSelfTesting();
		
		if(IgnitionStatus == TRUE)
		{
		 	IgnitionStatus = bCheckIgnitionOff();
		 	if(IgnitionStatus == FALSE)
		 	{
			 	for(i = eTachometer ; i < eNumOfMeters; i++)
				{
					stepperCB[i].s16TargetPosition = 0;
					stepperCB[i].s16CountsFromStart = 0;					
				}	 	 
		 	}
		}
	
    }

	if(IgnitionStatus == FALSE)
		{
			vResetCpu();		
		}
// Stepper return to home position 

    stepperCB[eSpdMeter].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eSpdMeter].s16MaxNeedlePos = SPEEDO_MICROSTEPS_EXTRA;
	stepperCB[eSpdMeter].s16TargetPosition= SPEEDO_MICROSTEPS_EXTRA/2;
	stepperCB[eSpdMeter].s16CurrentPosition = SPEEDO_MICROSTEPS_EXTRA;
	
	stepperCB[eTachometer].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTachometer].s16MaxNeedlePos = RPM_MICROSTEPS_EXTRA;
	stepperCB[eTachometer].s16TargetPosition = RPM_MICROSTEPS_EXTRA/2;	
	stepperCB[eTachometer].s16CurrentPosition = RPM_MICROSTEPS_EXTRA;
	
	stepperCB[eFuelGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eFuelGauge].s16MaxNeedlePos = FUEL_MICROSTEPS_EXTRA;
	stepperCB[eFuelGauge].s16TargetPosition = FUEL_MICROSTEPS_EXTRA/2;	
	stepperCB[eFuelGauge].s16CurrentPosition = FUEL_MICROSTEPS_EXTRA;
	
	stepperCB[eTempGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTempGauge].s16MaxNeedlePos = TEMP_MICROSTEPS_EXTRA;
	stepperCB[eTempGauge].s16TargetPosition = TEMP_MICROSTEPS_EXTRA/2;	
	stepperCB[eTempGauge].s16CurrentPosition = TEMP_MICROSTEPS_EXTRA;	
	
	//required for a slow start
	for(i = eTachometer ; i < eNumOfMeters; i++)
		{
		stepperCB[i].s16CountsFromStart = 0;	//required for a slow start		
		}  


	while((stepperCB[eSpdMeter].s16TargetPosition != stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition != stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition != stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition != stepperCB[eTempGauge].s16CurrentPosition
		
    	) )
    {
    	for(i = eTachometer ; i < eNumOfMeters; i++)
			vStepperPositionDriver(i);

		R_WDT_Restart();
		vTaskDoneDuringSelfTesting();
		
		if(IgnitionStatus == TRUE)
		{
			 IgnitionStatus = bCheckIgnitionOff();
			 if(IgnitionStatus == FALSE)
			 {
				 	for(i = eTachometer ; i < eNumOfMeters; i++)
					{
						stepperCB[i].s16TargetPosition = 0;
					}	 	 
			 }
		}
		 
    }
	
	if(IgnitionStatus == FALSE)
		{
		  	vResetCpu();	
		}

	
	for(i = eTachometer ; i < eNumOfMeters; i++)
	{
		stepperCB[i].s16TargetPosition = 0;
		stepperCB[i].s16CountsFromStart = 0;
	}
		
	while((stepperCB[eSpdMeter].s16TargetPosition != stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition != stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition != stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition != stepperCB[eTempGauge].s16CurrentPosition
		
    	) )
    {
    	for(i = eTachometer ; i < eNumOfMeters; i++)
			vStepperPositionDriver(i);
			
		R_WDT_Restart();
		vTaskDoneDuringSelfTesting();

		if(IgnitionStatus == TRUE)
		{
			 IgnitionStatus = bCheckIgnitionOff();
		}
		
	}

	if(IgnitionStatus == FALSE)
		{
		  	vResetCpu();		 
		}
	
		bSelfTestStatus = TRUE;


}


/*********************************************************************//**
 *
 * Short Homing approx 10 degree
 *
 * @param	 None
 *
 * @return	 None
 *************************************************************************/
void vShortHoming(void)
{
	u8 i =0;
	
	bSelfTestStatus = TRUE;
	bStartUp = FALSE;
	
    stepperCB[eSpdMeter].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eSpdMeter].s16MaxNeedlePos = SPEEDO_MICROSTEPS_EXTRA;	
	stepperCB[eSpdMeter].s16CurrentPosition = SHORT_HOME_USTEPS;
	
	stepperCB[eTachometer].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTachometer].s16MaxNeedlePos = RPM_MICROSTEPS_EXTRA;		
	stepperCB[eTachometer].s16CurrentPosition = SHORT_HOME_USTEPS;
	
	stepperCB[eFuelGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eFuelGauge].s16MaxNeedlePos = FUEL_MICROSTEPS_EXTRA;	
	stepperCB[eFuelGauge].s16CurrentPosition = SHORT_HOME_USTEPS;
	
	stepperCB[eTempGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTempGauge].s16MaxNeedlePos = TEMP_MICROSTEPS_EXTRA;	
	stepperCB[eTempGauge].s16CurrentPosition = SHORT_HOME_USTEPS;	
	

	
	//required for a slow start
		for(i = eTachometer ; i < eNumOfMeters; i++)
		{
			stepperCB[i].s16CountsFromStart = 0;	//required for a slow start	
			stepperCB[i].s16TargetPosition	= 0;
		}  
	
	while((stepperCB[eSpdMeter].s16TargetPosition != stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition != stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition != stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition != stepperCB[eTempGauge].s16CurrentPosition
		
    	) )
	    {
    	for(i = eTachometer; i < eNumOfMeters; i++)
			vStepperPositionDriver(i);
			R_WDT_Restart();
		}
		

}

/*********************************************************************//**
*
* Long homing is performed on every battery connect if any of the gauges homing 
* was not complete in previous ignition cycle. If proper homing was done for allthe  
* gauges  in previous ignition cycle, short homing shall be done on battery connect. 
* Long homing is achieved by driving the pointer from current position by 
* 300 degrees towards the internal stop.
*
* @param	  None
*
* @return   None
*************************************************************************/
 void vLongHoming(void)
{
	 u8 i = 0;
	static u32 u32SelfTestClock;
	bStartUp = TRUE;

	// Sweep the needle back to zero
	//StepperDriverTimer_SetPeriodUS(STEPPER_TIMER_HOMING); 
	stepperCB[eSpdMeter].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eFuelGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTachometer].S16ZeroOffSet = COMMON_ZERO_OFFSET;
	stepperCB[eTempGauge].S16ZeroOffSet = COMMON_ZERO_OFFSET;

	stepperCB[eSpdMeter].s16MaxNeedlePos = SPEEDO_MICROSTEPS_EXTRA;
	stepperCB[eFuelGauge].s16MaxNeedlePos = FUEL_MICROSTEPS_EXTRA;
	stepperCB[eTachometer].s16MaxNeedlePos = RPM_MICROSTEPS_EXTRA;
	stepperCB[eTempGauge].s16MaxNeedlePos = TEMP_MICROSTEPS_EXTRA;

	stepperCB[eSpdMeter].s16CurrentPosition = FULL_HOME_STEPS ;
	stepperCB[eFuelGauge].s16CurrentPosition = FULL_HOME_STEPS ;
	stepperCB[eTachometer].s16CurrentPosition = FULL_HOME_STEPS ;
	stepperCB[eTempGauge].s16CurrentPosition = FULL_HOME_STEPS ;	


	stepperCB[eSpdMeter].s16CurrentTarget = 0;
	stepperCB[eFuelGauge].s16CurrentTarget = 0;
	stepperCB[eTachometer].s16CurrentTarget = 0;
	stepperCB[eTempGauge].s16CurrentTarget = 0;


	stepperCB[eSpdMeter].s16TargetPosition= 0;
	stepperCB[eFuelGauge].s16TargetPosition = 0;
	stepperCB[eTachometer].s16TargetPosition= 0;
	stepperCB[eTempGauge].s16TargetPosition = 0;


	//required for a slow start
	stepperCB[eSpdMeter].s16CountsFromStart = 0;
	stepperCB[eFuelGauge].s16CountsFromStart = 0;
	stepperCB[eTachometer].s16CountsFromStart = 0;
	stepperCB[eTempGauge].s16CountsFromStart = 0;


	bacceleratingStartup[eSpdMeter] = TRUE;
	bacceleratingStartup[eFuelGauge] = TRUE;
	bacceleratingStartup[eTachometer] = TRUE;
	bacceleratingStartup[eTempGauge] = TRUE;
	

	for(i = eTachometer ; i < eNumOfMeters; i++)
	{
		bMoveStepperMotor[i] = TRUE;		
	} 


	while(
		stepperCB[eSpdMeter].s16TargetPosition!= stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition!= stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition!= stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition!= stepperCB[eTempGauge].s16CurrentPosition

	) 
	{
		bacceleratingStartup[eSpdMeter] = _bAccelDecelStartUp(bacceleratingStartup[eSpdMeter],eSpdMeter);
		bacceleratingStartup[eFuelGauge] = _bAccelDecelStartUp(bacceleratingStartup[eFuelGauge],eFuelGauge);
		bacceleratingStartup[eTachometer] = _bAccelDecelStartUp(bacceleratingStartup[eTachometer],eTachometer);
		bacceleratingStartup[eTempGauge] = _bAccelDecelStartUp(bacceleratingStartup[eTempGauge],eTempGauge);

		R_WDT_Restart();
	}

	u32SelfTestClock =  u32GetClockTicks();
	while( u32GetClockTicks() - u32SelfTestClock < AFTER_FULL_SCALE_MOVE_GAP)
		R_WDT_Restart();

	bStartUp = FALSE;	
}


 /*********************************************************************//**
 *
 * Send max microsteppeing 
 *
 * @param	 Gauge Type
 *
 * @return	 Gauge max Position 
 *************************************************************************/
u16 u16GaugeMaxMicroSteps(eMETER_TYPE GaugeType)
	{
		return stepperCB[GaugeType].s16MaxNeedlePos;
	}

 /*********************************************************************//**
 *
 * Set target position 
 *
 * @param	 Target position 
 *			Gauge type
 *
 * @return	 Gauge max Position 
 *************************************************************************/
void vSetTargetPosition(u16 TargetPosition , eMETER_TYPE GaugeType)
	{
		 stepperCB[GaugeType].s16TargetPosition = TargetPosition;
		 
		 if( stepperCB[GaugeType].s16TargetPosition > stepperCB[GaugeType].s16MaxNeedlePos )
		 	 stepperCB[GaugeType].s16TargetPosition = stepperCB[GaugeType].s16MaxNeedlePos;
	}

 /*********************************************************************//**
  *
  * Get niddle current position 
  *
  * @param	   Gauge type
  *
  * @return   Gauge current position 
  *************************************************************************/
 s16 s16GetCurrentPosition(eMETER_TYPE GaugeType)
 {
	  return(stepperCB[GaugeType].s16CurrentPosition);
 }

 
 /*********************************************************************//**
  *
  * Get niddle Target position 
  *
  * @param	   Gauge type
  *
  * @return   Gauge current position 
  *************************************************************************/
 s16 s16GetTargetPosition(eMETER_TYPE GaugeType)
 {
	  return(stepperCB[GaugeType].s16TargetPosition);
 }


 /*********************************************************************//**
 *
 * Return to niddle home position from current position  
 *
 * @param	 Target position 
 *			Gauge type
 *
 * @return	 Gauge max Position 
 *************************************************************************/
void vMoveStepperToHomePosition()
{
	u8 i =0;
	//Return all steppers to home position from its current position 
		stepperCB[eTempGauge].s16TargetPosition = 0;
		stepperCB[eFuelGauge].s16TargetPosition = 0;
		stepperCB[eTachometer].s16TargetPosition = 0;
		stepperCB[eSpdMeter].s16TargetPosition= 0;
		stepperCB[eTachometer].s16CurrentTarget = 0;
		stepperCB[eSpdMeter].s16CurrentTarget = 0;
	
		//required for a slow start
		
		for(i = eTachometer; i < eNumOfMeters; i++)
			{
			 stepperCB[i].s16CountdownReload= 0;
			 stepperCB[i].s16CountsFromStart = 0;
			}
	
		while((stepperCB[eSpdMeter].s16TargetPosition != stepperCB[eSpdMeter].s16CurrentPosition
		|| stepperCB[eTachometer].s16TargetPosition != stepperCB[eTachometer].s16CurrentPosition
		|| stepperCB[eFuelGauge].s16TargetPosition != stepperCB[eFuelGauge].s16CurrentPosition
		|| stepperCB[eTempGauge].s16TargetPosition != stepperCB[eTempGauge].s16CurrentPosition
	
		) )
		{
			vStepperPositionDriver(eSpdMeter);
			vStepperPositionDriver(eTachometer);
			vStepperPositionDriver(eFuelGauge);
			vStepperPositionDriver(eTempGauge);
			R_WDT_Restart();
	
	
		} //end while


}


/*********************************************************************//**
*
* initialize stepper driver for SM1,SM2,SM3 and SM4 
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vStepperInit(void)
{	
	PM9	   = 0x00;			//Stepper connected on SM4 & SM3 module. Make port pins as outputs
	PM8	   = 0x00;			//Stepper connected on SM2 & SM1 module. Make port pins as outputs
	PER1   = PER1 | 0X20;	//SFR used by stepper motor controller/driver can be read and written
	MCNTC0 = 0x03;  	    //Stepper driver Enable and count range from 00 to FF

	MCMPC4 = 0x10;
	MCMP40 = 0x00;
	MCMP41 = 0x00;

	MCMPC3 = 0x10;
	MCMP30 = 0x00;
	MCMP31 = 0x00;

	MCMPC2 = 0x10;
	MCMP20 = 0x00;
	MCMP21 = 0x00;

	MCMPC1 = 0x10;
	MCMP10 = 0x00;
	MCMP11 = 0x00;     

	SMPC = 0x0F; 	 

	CAE = 1;         //Stepper Motor Controller/Driver operation is enabled
	PCE = 1;		 //Timer counter is enabled.

}

/*********************************************************************//**
*
* Disable stepper driver 
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vDisableStepper()
{
	SMPC = 0x00; 
	P8 = 0x00;
	P9 = 0x00;
}

/*********************************************************************//**
*
* Function Not used
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vStepperSM1(u8 ZpdSM1DetectFlag, u8 ZpdSM2DetectFlag, u8 ZpdSM3DetectFlag, u8 ZpdSM4DetectFlag)
{
	/*
	
	if(ZpdSM1DetectFlag == 0)
	{
		P8 &= 0xF0;	 
		P8 |= 0x04;	
	}
	if(ZpdSM2DetectFlag == 0)
	{	
		P8 &= 0x0F;	 
		P8 |= 0x40;	
	}
	if(ZpdSM3DetectFlag == 0)
	{
		P9 &= 0xF0;	 
		P9 |= 0x04;	
	}
	if(ZpdSM4DetectFlag == 0)
	{	
		P9 &= 0x0F;	 
		P9 |= 0x40;		
	}
	*/
}

/*********************************************************************//**
*
* Function Not used
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vStepperSM2(u8 ZpdSM1DetectFlag, u8 ZpdSM2DetectFlag, u8 ZpdSM3DetectFlag, u8 ZpdSM4DetectFlag)
{
	/*
	if(ZpdSM1DetectFlag == 0)
	{
		P8 &= 0xF0;
		P8 |= 0x01;	
	}
	if(ZpdSM2DetectFlag == 0)
	{
		P8 &= 0x0F;
		P8 |= 0x10;		
	}
	if(ZpdSM3DetectFlag == 0)
	{
		P9 &= 0xF0;
		P9 |= 0x01;	
	}
	if(ZpdSM4DetectFlag == 0)
	{
		P9 &= 0x0F;
		P9 |= 0x10;	
	}	
       */
}
/*********************************************************************//**
*
* Function Not used
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vStepperSM3(u8 ZpdSM1DetectFlag, u8 ZpdSM2DetectFlag, u8 ZpdSM3DetectFlag, u8 ZpdSM4DetectFlag)
{/*
      
	if(ZpdSM1DetectFlag == 0)
	{
		P8 &= 0xF0;
		P8 |= 0x08;	
	}
	if(ZpdSM2DetectFlag == 0)
	{
		P8 &= 0x0F;
		P8 |= 0x80;	
	}
	if(ZpdSM3DetectFlag == 0)
	{
		P9 &= 0xF0;
		P9 |= 0x08;
	}
	if(ZpdSM4DetectFlag == 0)
	{
		P9 &= 0x0F;
		P9 |= 0x80;	
	}
	*/
}

/*********************************************************************//**
*
* Function Not used
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vStepperSM4(u8 ZpdSM1DetectFlag, u8 ZpdSM2DetectFlag, u8 ZpdSM3DetectFlag, u8 ZpdSM4DetectFlag)
{
	/*
	if(ZpdSM1DetectFlag == 0)
	{
		P8 &= 0xF0;
		P8 |= 0x02;	 
	}
	if(ZpdSM2DetectFlag == 0)
	{
		P8 &= 0x0F;
		P8 |= 0x20;	 
	}
	if(ZpdSM3DetectFlag == 0)
	{
		P9 &= 0xF0;
		P9 |= 0x02;	 
	}
	if(ZpdSM4DetectFlag == 0)
	{
		P9 &= 0x0F;
		P9 |= 0x20;	 
	}       
	
	*/
      
}

/*********************************************************************//**
*
* Function Not used
*
* @param    None	
*
* @return 	None
*************************************************************************/
void vZPD()
{
/*	u16 i=0;
	u8 temp_var;     
	u8 ZpdSM1DetectFlag = 0;
	u8 ZpdSM2DetectFlag = 0;
	u8 ZpdSM3DetectFlag = 0;
	u8 ZpdSM4DetectFlag = 0;

	P8 = 0x00;
	P9 = 0x00;
	CMPCTL = 0x09;   //ZPD Flag Detection Clock Setting Register Fclk/9

	do
	{
		vStepperSM1(ZpdSM1DetectFlag, ZpdSM2DetectFlag, ZpdSM3DetectFlag,ZpdSM4DetectFlag);
		_vDelay();
		vStepperSM2(ZpdSM1DetectFlag, ZpdSM2DetectFlag, ZpdSM3DetectFlag,ZpdSM4DetectFlag);
		_vDelay();
		vStepperSM3(ZpdSM1DetectFlag, ZpdSM2DetectFlag, ZpdSM3DetectFlag,ZpdSM4DetectFlag);
		_vDelay();
//[CcnvCA78K0R] 		PM8.3 = 1;		//C- as input
		__BIT8( PM8, 3 ) = 1;		//C- as input
//[CcnvCA78K0R] 		PM8.7 = 1;		//C- as input
		__BIT8( PM8, 7 ) = 1;		//C- as input
//[CcnvCA78K0R] 		PM9.3 = 1;		//C- as input
		__BIT8( PM9, 3 ) = 1;		//C- as input
//[CcnvCA78K0R] 		PM9.7 = 1;		//C- as input
		__BIT8( PM9, 7 ) = 1;		//C- as input
		ZPDEN = 0x00;   	//Disables ZPD operation.
		ZPDS0 = 0x88;		//Analog input selection
		ZPDS0 |= 0x22;		//voltage level set
		ZPDS1 = 0x88;		//Analog input selection
		ZPDS1 |= 0x22;		//voltage level set	
		ZPDEN = 0x0F;		//Enable ZPD for 4 stepper 
		for(i=0;i<0xff;i++);	//comparator stabilization time
		MCMPC1 = 0x00;		//Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC2 = 0x00;		//Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC3 = 0x00;		//Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC4 = 0x00;		//Disable writing to ZPD bit from the comparator (No 0-point detection)
		NOP();NOP();NOP();            
		MCMPC1 = 0x40;		//Enable writing to ZPD bit from the comparator (0-point detection)
		MCMPC2 = 0x40;		//Enable writing to ZPD bit from the comparator (0-point detection)
		MCMPC3 = 0x40;		//Enable writing to ZPD bit from the comparator (0-point detection)
		MCMPC4 = 0x40;		//Enable writing to ZPD bit from the comparator (0-point detection)
		vStepperSM4(ZpdSM1DetectFlag, ZpdSM2DetectFlag, ZpdSM3DetectFlag,ZpdSM4DetectFlag);
		_vDelay();
		temp_var = MCMPC1 & 0x20; // Induced zpd voltage detection
		if(temp_var)
		{
			P8 &= 0xF0;
			ZpdSM1DetectFlag = 1;// ZPD detected, break out of while loop
		}	
		temp_var = MCMPC2 & 0x20; // Induced zpd voltage detection
		if(temp_var)
		{
			P8 &= 0x0F;
			ZpdSM2DetectFlag = 1;// ZPD detected, break out of while loop
		}	
		temp_var = MCMPC3 & 0x20; // Induced zpd voltage detection
		if(temp_var)
		{
			P9 &= 0xF0;
			ZpdSM3DetectFlag = 1;// ZPD detected, break out of while loop
		}	
		temp_var = MCMPC4 & 0x20; // Induced zpd voltage detection
		if(temp_var)
		{
			P9 &= 0x0F;
			ZpdSM4DetectFlag = 1;// ZPD detected, break out of while loop
		}		
		MCMPC1 = 0x00;     //Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC2 = 0x00;     //Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC3 = 0x00;     //Disable writing to ZPD bit from the comparator (No 0-point detection)
		MCMPC4 = 0x00;     //Disable writing to ZPD bit from the comparator (No 0-point detection)
		ZPDS0 = 0x00;      //Disables Analouge input selection .
		ZPDS1 = 0x00;      //Disables Analouge input selection .
		ZPDEN = 0x00;      //Disables ZPD operation.
//[CcnvCA78K0R] 		P8.3  = 0;P8.7  = 0; P9.3  = 0;P9.7  = 0;
		__BIT8( P8, 3 )  = 0;__BIT8( P8, 7 )  = 0; __BIT8( P9, 3 )  = 0;__BIT8( P9, 7 )  = 0;
//[CcnvCA78K0R] 		PM8.3 = 0;PM8.7 = 0; PM9.3 = 0;PM9.7 = 0;
		__BIT8( PM8, 3 ) = 0;__BIT8( PM8, 7 ) = 0; __BIT8( PM9, 3 ) = 0;__BIT8( PM9, 7 ) = 0;

	}while(ZpdSM1DetectFlag != 1 || ZpdSM2DetectFlag != 1 || ZpdSM3DetectFlag != 1 || ZpdSM4DetectFlag != 1);
		NOP();
*/	
}

/*********************************************************************//**
*
* Not used ,Delay for Hardware zero position detection for stepper
*
* @param    None	
*
* @return 	None
*************************************************************************/
void _vDelay(void)
{
    unsigned int i,j;
    for(j=0;j<2;j++)
    {     
	    for(i=0;i<950;i++)
	    {
	        R_WDT_Restart();
	        
	    }
    }
}



