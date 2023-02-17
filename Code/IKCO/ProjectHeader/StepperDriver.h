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


#ifndef __StepperDriver_H
#define __StepperDriver_H
#include "customTypedef.h"   

// constant declarations -----------------------------------------------------

typedef struct
{
    u16    u16Mtr1Pwm;
    u16    u16Mtr2Pwm;
    u8     u8Polarity;

}  MICROSTEP_PWM_TYPE;

typedef enum
{
	eTachometer = 0, 
	eSpdMeter,
	eFuelGauge,
	eTempGauge,
	eNumOfMeters
} eMETER_TYPE;

#define STEPS_PER_DEGREE                19
#define STEP_ARRAY_SIZE         		32
 
#define FULL_STEPS_START_UP 			1632

#define COMMON_ZERO_OFFSET				0

#define SPEEDO_PULSE                    7464
#define SPEEDO_DIAL_SPAN				212L	
#define SPEED_METER_USTEPS				(u32)(SPEEDO_DIAL_SPAN * STEPS_PER_DEGREE)  
#define SPEEDO_MICROSTEPS_EXTRA			(SPEED_METER_USTEPS + 90)


#define RPM_DIAL_SPAN					212L 
#define RPM_METER_USTEPS				(u32)(RPM_DIAL_SPAN * STEPS_PER_DEGREE)  
#define RPM_MICROSTEPS_EXTRA			(RPM_METER_USTEPS + 70)

#define FUEL_DIAL_SPAN					240
#define FUEL_METER_USTEPS				(FUEL_DIAL_SPAN * STEPS_PER_DEGREE)  
#define FUEL_MICROSTEPS_EXTRA			(FUEL_METER_USTEPS + 100)


#define TEMP_DIAL_SPAN					240
#define TEMP_METER_USTEPS				(TEMP_DIAL_SPAN * STEPS_PER_DEGREE)  
#define TEMP_MICROSTEPS_EXTRA			(TEMP_METER_USTEPS + 70)

#define FULL_HOME_STEPS 				6160	//320 degree

// Global declarations ------------------------------------------------------

typedef struct
{
    s16     S16ZeroOffSet;         // Counts from the hard stop to the zero position
    s16     s16MaxNeedlePos;       // Maximum gauge needle position
    s16     s16CountsFromStart;    // Number of counts from the starting postion
    s16     s16CurrentPosition;    // Position in degrees from the origin
    s16     s16TargetPosition;     // Target position in steps from the origin
    s16     s16CurrentTarget;
    s16     s16CountdownReload;    // Reload countdown value for a 100usec clock

} STEPPER_CONTROL_BLOCK_TYPE;


extern STEPPER_CONTROL_BLOCK_TYPE  stepperCB[eNumOfMeters];
extern const MICROSTEP_PWM_TYPE MicroStepPWM[STEP_ARRAY_SIZE] ;
extern bool bStartUp;
extern bool bMoveStepperMotor[eNumOfMeters];
 
// Global prototypes --------------------------------------------------------

/// @addtogroup Stepper_LIBRARY
/// @{
 
/// @defgroup StepperHeaderLib Global functions used in Stepper Driver
//// 
///   
/// @addtogroup StepperHeaderLib
/// @{


void vStepperPositionDriver(eMETER_TYPE meterType);
void vZPD();
void vStepperInit(void);
void vStepperSelfTest(void);
void vTaskDoneDuringSelfTesting();
void vLongHoming();
void vDisableStepper();
void vShortHoming(void);
u16 u16GaugeMaxMicroSteps(eMETER_TYPE);
void vSetTargetPosition(u16,eMETER_TYPE );
void vMoveStepperToHomePosition();
s16 s16SetCurrentPosition(eMETER_TYPE GaugeType);
s16 s16GetTargetPosition(eMETER_TYPE GaugeType);


/// @} // endgroup StepperHeaderLib

/// @} // endgroup Stepper_LIBRARY


/* END StepperDriver */

#endif

