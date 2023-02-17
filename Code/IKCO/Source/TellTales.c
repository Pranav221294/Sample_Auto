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


#include "TellTales.h"
#include "customTypedef.h"
//#include "iicDriver.h"
//#include"ledDriverMPQ3326.h"
#include "J1939.h" 
#include "r_cg_macrodriver.h"  // include CPU file
#include "r_cg_userdefine.h"
#include "Speedo.h"
#include "Tacho.h"
#include "CoolantTemp.h" 
#include "FuelGauge.h" 
#include "Adc.h" 
#include "AirPressureGauge.h" 
#include"ledDriver5716.h"
#include "Eol.h"
#include "ServiceReminder.h"
#include "GenIllumination.h" 
#include "TML_14229_UDSH.h"

// Pins declarations -----------------------------------------------------

#define INPUT_TURN_LEFT		          	P6.4

#define INPUT_TURN_RIGHT	          	P6.5

#define INPUT_PARK_BRAKE		  		P6.6

#define INPUT_AIR_FILTER		  		P7.4

#define INPUT_LOW_OIL_PRES		  		P1.4

#define INPUT_CAB_TILT			  		P1.5

#define INPUT_MIL_LOW		          	P0.7

#define FRONT_FOG_HIGH                  P13.3

#define INPUT_DOOR_LOW	          	    P0.3

#define INPUT_RETARDER_HIGH	          	P13.4

#define INPUT_EXHAUST		          	P3.5

#define INPUT_MAIN_BEAM        			P3.4

#define INPUT_HIGH_BEAM                 P3.1

#define INPUT_CLUTCH_OIL_LEVEL_INPUT    P5.6

#define INPUT_ENGINE_COOLANT_LEVEL       P5.5

#define INPUT_ESC_MIL                   P5.4

#define INPUT_SEAT_BELT			  		P13.5

#define INPUT_BATT_CHARGING1            P13.2

#define INPUT_BATT_CHARGING2            P13.1

#define INPUT_BRAKE_LIQUID_LEVEL           P3.7

#define INPUT_RED_STOP_LAMP             P7.0

#define INPUT_YELLO_WRN_LAMP            P3.6

#define INPUT_ENGINE_OIL_LEVEL         P3.2 

#define INPUT_ABS_FAILURE               P0.5

#define INPUT_REVERSE_LIGHT             P7.5






 

	#define HIGH_BEAM_ON            P14 = P14 | 0x01
	#define HIGH_BEAM_OFF           P14 = P14 & 0xFE

	#define TURN_LEFT_ON			P5 = P5 | 0x80		
	#define TURN_LEFT_OFF			P5 = P5 & 0x7F

	#define TURN_RIGHT_ON			P3 = P3 | 0x01		
	#define TURN_RIGHT_OFF			P3 = P3 & 0xFE
	

	

	








// constant declarations -----------------------------------------------------
#define INTERVAL_TIME_250				250 //ms
#define INTERVAL_TIME_500				500 //ms
#define TIME_HYS						100 //ms
#define BLINK_RATE						250		//Time in ms				
#define INPUT_DEBOUNCE_TIME				200		//Time in ms
#define FUEL_WARNING_ON_TIME			1000	// in ms
#define FUEL_WARNING_OFF_TIME			3000	//in ms
#define TURN_LEFT_RIGHT_FLASH_TIME		500		//in ms
#define INDICATOR_ON_TIME				1500	//in ms
#define INDICATOR_OFF_TIME				2000	//in ms
#define INDICATOR_FLASHING_TIME			500		//in ms
#define SEAT_BELT_BUZZER_TIME 			30000 	// in ms
#define HYSTERESIS						50		//RPM
#define ENGINE_RPM						200
#define UP_DOWN_SHIFT_LAMP_BUZZER_ON	200		//In ms
#define UP_DOWN_SHIFT_LAMP_BUZZER_OFF	10000	//In ms
#define TELL_TELL_REFRESH_TIME			10		//in ms
#define MIL_PGN5080_SENSE_TIME 			10000


//*****************************************************************
/*
#define TRAILOR_ON					{ u8GSdata[0] = 0xFF; u8GSdata[1] = 0xFF;} // EXHAUST BREAK LAMP
#define MIL_ON						{ u8GSdata[3] = 0xFF; u8GSdata[4] |= 0xF0;}	// 
#define LED_FUEL_ECO_YEL_ON 		{ u8GSdata[4] |= 0x0F; u8GSdata[5] = 0xFF;}
#define LED_FUEL_ECO_RED_ON 		{ u8GSdata[7] = 0xFF; u8GSdata[8] = 0xFF;}
#define CLUCH_WEAR_ON				{ u8GSdata[9] = 0xFF; u8GSdata[10] = 0xFF;} //  CLUTCH OIL LEVEL
#define SEAT_BELT_ON				{ u8GSdata[12] = 0xFF; u8GSdata[13] |= 0xF0;} // seat belt 
#define LIFT_AXEL_FAULT_ON			{ u8GSdata[13] |= 0x0F; u8GSdata[14] = 0xFF;} // ENGINE COOLANT LEVEL
#define BATT_CHARG2_ON				{ u8GSdata[16] = 0xFF; u8GSdata[17] = 0xFF;} // REVERSE LIGHT
#define LOW_AIR_PRESSURE_ON			{ u8GSdata[18] = 0xFF; u8GSdata[19] = 0xFF;} // AIR PRESS. GUAGE-FRONT INPUT
#define LOW_BRAKE_FLUID_ON			{ u8GSdata[21] = 0xFF; u8GSdata[22] |= 0xF0;} //	
#define TPMS_LAMP_ON				{ u8GSdata[22] |= 0x0F; u8GSdata[23] = 0xFF;} // ADBLUE
#define OVER_SPEED_RPM_ON			{ u8GSdata[25] = 0xFF; u8GSdata[26] = 0xFF;} // COOLANT IN TEMPRATURE
#define OVER_SPEED_SPEEDO_ON		{ u8GSdata[27] = 0xFF; u8GSdata[28] = 0xFF;}// FUEL GUAGE
#define TILT_CAB_LOCK_ON			{ u8GSdata[30] = 0xFF; u8GSdata[31] |= 0xF0;} // TILT 	
#define LOW_OIL_PRESSURE_ON			{ u8GSdata[31] |= 0x0F; u8GSdata[32] = 0xFF;} // LOW OIL PRESSURE
#define LOW_FUEL_ON					{ u8GSdata[34] = 0xFF; u8GSdata[35] = 0xFF;} //


//***************

#define TRAILOR_OFF					{ u8GSdata[0] = 0x00; u8GSdata[1] = 0x0F;}
#define MIL_OFF						{ u8GSdata[3] = 0x00; u8GSdata[4] &= 0x0F;}

#define LED_FUEL_ECO_YEL_OFF		{ u8GSdata[4] &= 0xF0; u8GSdata[5] = 0x00;}
#define LED_FUEL_ECO_RED_OFF		{ u8GSdata[7] = 0xF0; u8GSdata[8] = 0x00;}

#define CLUCH_WEAR_OFF				{ u8GSdata[9] = 0x00; u8GSdata[10] = 0x0F;}
#define SEAT_BELT_OFF				{ u8GSdata[12] = 0x00; u8GSdata[13] &= 0x0F;}
#define LIFT_AXEL_FAULT_OFF			{ u8GSdata[13] &= 0xF0; u8GSdata[14] = 0x00;}

#define BATT_CHARG2_OFF				{ u8GSdata[16] = 0xF0; u8GSdata[17] = 0x00;}
#define LOW_AIR_PRESSURE_OFF		{ u8GSdata[18] = 0x00; u8GSdata[19] = 0x0F;}
#define LOW_BRAKE_FLUID_OFF			{ u8GSdata[21] = 0x00; u8GSdata[22] &= 0x0F;}
#define TPMS_LAMP_OFF				{ u8GSdata[22] &= 0xF0; u8GSdata[23] = 0x00;}

#define OVER_SPEED_RPM_OFF			{ u8GSdata[25] = 0xF0; u8GSdata[26] = 0x00;}


#define OVER_SPEED_SPEEDO_OFF		{ u8GSdata[27] = 0x00; u8GSdata[28] = 0x0F;}
#define TILT_CAB_LOCK_OFF			{ u8GSdata[30] = 0x00; u8GSdata[31] &= 0x0F;}
#define LOW_OIL_PRESSURE_OFF		{ u8GSdata[31] &= 0xF0; u8GSdata[32] = 0x00;}
#define LOW_FUEL_OFF				{ u8GSdata[34] = 0xF0; u8GSdata[35] = 0x00;}

*/
#define EXHAUST_BREAK_ON			{ u8GSdata[0] = 0xFF; u8GSdata[1] = 0xFF;} // EXHAUST BREAK LAMP
#define MIL_ON					{ u8GSdata[3] = 0xFF; u8GSdata[4] |= 0xF0;}	 
//#define LED_FUEL_ECO_YEL_ON 		{ u8GSdata[4] |= 0x0F; u8GSdata[5] = 0xFF;}
//#define LED_FUEL_ECO_RED_ON 		{ u8GSdata[7] = 0xFF; u8GSdata[8] = 0xFF;}
#define CLUTCH_OIL_LEVEL_ON			{ u8GSdata[9] = 0xFF; u8GSdata[10] = 0xFF;} //  CLUTCH OIL LEVEL
#define SEAT_BELT_ON				{ u8GSdata[12] = 0xFF; u8GSdata[13] |= 0xF0;} // seat belt 
#define ENGINE_COOLANT_LEVEL_ON		{ u8GSdata[13] |= 0x0F; u8GSdata[14] = 0xFF;} // ENGINE COOLANT LEVEL
#define REVERSE_LIGHT_ON			{ u8GSdata[16] = 0xFF; u8GSdata[17] = 0xFF;} // REVERSE LIGHT
#define LOW_AIR_PRESSURE_ON			{ u8GSdata[18] = 0xFF; u8GSdata[19] = 0xFF;} // AIR PRESS. GUAGE-FRONT INPUT
#define LOW_BRAKE_FLUID_ON		{ u8GSdata[21] = 0xFF; u8GSdata[22] |= 0xF0;} //	
#define ADBLUE_ON					{ u8GSdata[22] |= 0x0F; u8GSdata[23] = 0xFF;} // ADBLUE
#define COOLANT_IN_TEMPRATURE_ON	{ u8GSdata[25] = 0xFF; u8GSdata[26] = 0xFF;} // COOLANT IN TEMPRATURE
#define FUEL_GUAGE_ON				{ u8GSdata[27] = 0xFF; u8GSdata[28] = 0xFF;}// FUEL GUAGE
#define TILT_CAB_LOCK_ON			{ u8GSdata[30] = 0xFF; u8GSdata[31] |= 0xF0;} // TILT 	
#define LOW_OIL_PRESSURE_ON			{ u8GSdata[31] |= 0x0F; u8GSdata[32] = 0xFF;} // LOW OIL PRESSURE
//#define LOW_FUEL_ON				{ u8GSdata[34] = 0xFF; u8GSdata[35] = 0xFF;} //


//***************

#define EXHAUST_BREAK_OFF			{ u8GSdata[0] = 0x00; u8GSdata[1] = 0x0F;}
#define MIL_OFF					{ u8GSdata[3] = 0x00; u8GSdata[4] &= 0x0F;}

//#define LED_FUEL_ECO_YEL_OFF		{ u8GSdata[4] &= 0xF0; u8GSdata[5] = 0x00;}
//#define LED_FUEL_ECO_RED_OFF		{ u8GSdata[7] = 0xF0; u8GSdata[8] = 0x00;}

#define CLUTCH_OIL_LEVEL_OFF		{ u8GSdata[9] = 0x00; u8GSdata[10] = 0x0F;}
#define SEAT_BELT_OFF				{ u8GSdata[12] = 0x00; u8GSdata[13] &= 0x0F;}
#define ENGINE_COOLANT_LEVEL_OFF	{ u8GSdata[13] &= 0xF0; u8GSdata[14] = 0x00;}

#define REVERSE_LIGHT_OFF			{ u8GSdata[16] = 0xF0; u8GSdata[17] = 0x00;}
#define LOW_AIR_PRESSURE_OFF		{ u8GSdata[18] = 0x00; u8GSdata[19] = 0x0F;}
#define LOW_BRAKE_FLUID_OFF			{ u8GSdata[21] = 0x00; u8GSdata[22] &= 0x0F;}
#define ADBLUE_OFF					{ u8GSdata[22] &= 0xF0; u8GSdata[23] = 0x00;}

#define COOLANT_IN_TEMPRATURE_OFF	{ u8GSdata[25] = 0xF0; u8GSdata[26] = 0x00;}


#define FUEL_GUAGE_OFF				{ u8GSdata[27] = 0x00; u8GSdata[28] = 0x0F;}
#define TILT_CAB_LOCK_OFF			{ u8GSdata[30] = 0x00; u8GSdata[31] &= 0x0F;}
#define LOW_OIL_PRESSURE_OFF		{ u8GSdata[31] &= 0xF0; u8GSdata[32] = 0x00;}
//#define LOW_FUEL_OFF				{ u8GSdata[34] = 0xF0; u8GSdata[35] = 0x00;}

//*****************For Second Driver******************************************
// Add 36 constant becouse Led first driver fill buffer upto 35

//Macro For LED ON
/*
#define HIGH_TEMP_ON				{ u8GSdata[0 + 36] = 0xFF; u8GSdata[1 + 36] = 0xFF;}
#define LED_FUEL_ECO_GRN_ON			{ u8GSdata[3 + 36] = 0xFF; u8GSdata[4 + 36] |= 0xF0;}	
#define CRUISE_CONTROL_ON			{ u8GSdata[4 + 36] |= 0x0F; u8GSdata[5 + 36] = 0xFF;} //CRUISE CONTROL
#define HEST_ON						{ u8GSdata[7 + 36] = 0xFF; u8GSdata[8 + 36] = 0xFF;} //LIM/NOX
#define GEAR_DOWN_SHIFT_ON			{ u8GSdata[9 + 36] = 0xFF; u8GSdata[10 + 36] = 0xFF;} //LOW GEAR
#define STOP_ENGINE_ON				{ u8GSdata[12 + 36] = 0xFF; u8GSdata[13 + 36] |= 0xF0;} // RED STOP WARING LIGHT
#define TRANSMISSION_RANG_LOW_ON	{ u8GSdata[13 + 36] |= 0x0F; u8GSdata[14 + 36] = 0xFF;} // MAIN BEAM
#define GEAR_UP_SHIFT_ON			{ u8GSdata[16 + 36] = 0xFF; u8GSdata[17 + 36] = 0xFF;} // HIGH GEAR
#define TRANSFLUID_TEMP_ON			{ u8GSdata[18 + 36] = 0xFF; u8GSdata[19 + 36] = 0xFF;} // YELLOW WARNING LIGHT
#define ABS_TRAILOR_ON				{ u8GSdata[21 + 36] = 0xFF; u8GSdata[22 + 36] |= 0xF0;} // DOOR OPEN
#define POWER_TAKE_OFF_ON			{ u8GSdata[22 + 36] |= 0x0F; u8GSdata[23 + 36] = 0xFF;} // POWER TAKE OFF
#define PARKING_BRAKE_ON			{ u8GSdata[25 + 36] = 0xFF; u8GSdata[26 + 36] = 0xFF;} // PARKING BREAK
#define FRONT_REAR_FOG_ON			{ u8GSdata[27 + 36] = 0xFF; u8GSdata[28 + 36] = 0xFF;} // FRONT FOG
#define CHECK_ENGINE_ON				{ u8GSdata[30 + 36] = 0xFF; u8GSdata[31 + 36] |= 0xF0;} // ENGINE WARNING ADD
#define NEUTRAL_ON					{ u8GSdata[31 + 36] |= 0x0F; u8GSdata[32 + 36] = 0xFF;} // ENGINE OIL LEVEL
#define BATTERY_CHARGING1_ON		{ u8GSdata[34 + 36] = 0xFF; u8GSdata[35 + 36] = 0xFF;} // BATTERY CHARGING-1


//Macro For LED OFF

#define HIGH_TEMP_OFF				{ u8GSdata[0 + 36] = 0x00; u8GSdata[1 + 36] = 0x0F;}
#define LED_FUEL_ECO_GRN_OFF		{ u8GSdata[3 + 36] = 0x00; u8GSdata[4 + 36] &= 0x0F;}
#define CRUISE_CONTROL_OFF			{ u8GSdata[4 + 36] &= 0xF0; u8GSdata[5 + 36] = 0x00;}
#define HEST_OFF					{ u8GSdata[7 + 36] = 0xF0; u8GSdata[8 + 36] = 0x00;}
#define GEAR_DOWN_SHIFT_OFF			{ u8GSdata[9 + 36] = 0x00; u8GSdata[10 + 36] = 0x0F;}
#define STOP_ENGINE_OFF				{ u8GSdata[12 + 36] = 0x00; u8GSdata[13 + 36] &= 0x0F;}
#define TRANSMISSION_RANG_LOW_OFF	{ u8GSdata[13 + 36] &= 0xF0; u8GSdata[14 + 36] = 0x00;}
#define GEAR_UP_SHIFT_OFF			{ u8GSdata[16 + 36] = 0xF0; u8GSdata[17 + 36] = 0x00;}
#define TRANSFLUID_TEMP_OFF			{ u8GSdata[18 + 36] = 0x00; u8GSdata[19 + 36] = 0x0F;}
#define POWER_TAKE_OFF_OFF			{ u8GSdata[22 + 36] &= 0xF0; u8GSdata[23 + 36] = 0x00;}
#define ABS_TRAILOR_OFF				{ u8GSdata[21 + 36] = 0x00; u8GSdata[22 + 36] &= 0x0F;}
#define PARKING_BRAKE_OFF			{ u8GSdata[25 + 36] = 0xF0; u8GSdata[26 + 36] = 0x00;}
#define FRONT_REAR_FOG_OFF			{ u8GSdata[27 + 36] = 0x00; u8GSdata[28 + 36] = 0x0F;}
#define CHECK_ENGINE_OFF			{ u8GSdata[30 + 36] = 0x00; u8GSdata[31 + 36] &= 0x0F;}
#define NEUTRAL_OFF					{ u8GSdata[31 + 36] &= 0xF0; u8GSdata[32 + 36] = 0x00;}
#define BATTERY_CHARGING1_OFF		{ u8GSdata[34 + 36] = 0xF0; u8GSdata[35 + 36] = 0x00;}	
*/

//#define HIGH_TEMP_ON				{ u8GSdata[0 + 36] = 0xFF; u8GSdata[1 + 36] = 0xFF;}
//#define LED_FUEL_ECO_GRN_ON		{ u8GSdata[3 + 36] = 0xFF; u8GSdata[4 + 36] |= 0xF0;}	
#define CRUISE_CONTROL_ON			{ u8GSdata[4 + 36] |= 0x0F; u8GSdata[5 + 36] = 0xFF;} //CRUISE CONTROL
#define LIM_NOX_ON					{ u8GSdata[7 + 36] = 0xFF; u8GSdata[8 + 36] = 0xFF;} //LIM/NOX
#define LOW_GEAR_ON					{ u8GSdata[9 + 36] = 0xFF; u8GSdata[10 + 36] = 0xFF;} //LOW GEAR
#define RED_STOP_WARING_LIGHT_ON	{ u8GSdata[12 + 36] = 0xFF; u8GSdata[13 + 36] |= 0xF0;} // RED STOP WARING LIGHT
#define MAIN_BEAM_ON				{ u8GSdata[13 + 36] |= 0x0F; u8GSdata[14 + 36] = 0xFF;} // MAIN BEAM
#define HIGH_GEAR_ON				{ u8GSdata[16 + 36] = 0xFF; u8GSdata[17 + 36] = 0xFF;} // HIGH GEAR
#define YELLOW_WARNING_LIGHT_ON		{ u8GSdata[18 + 36] = 0xFF; u8GSdata[19 + 36] = 0xFF;} // YELLOW WARNING LIGHT
#define DOOR_OPEN_ON				{ u8GSdata[21 + 36] = 0xFF; u8GSdata[22 + 36] |= 0xF0;} // DOOR OPEN
#define POWER_TAKE_OFF_ON			{ u8GSdata[22 + 36] |= 0x0F; u8GSdata[23 + 36] = 0xFF;} // POWER TAKE OFF
#define PARKING_BRAKE_ON			{ u8GSdata[25 + 36] = 0xFF; u8GSdata[26 + 36] = 0xFF;} // PARKING BREAK
#define FRONT_REAR_FOG_ON			{ u8GSdata[27 + 36] = 0xFF; u8GSdata[28 + 36] = 0xFF;} // FRONT FOG
#define ENGINE_WARNING_ADD_ON		{ u8GSdata[30 + 36] = 0xFF; u8GSdata[31 + 36] |= 0xF0;} // ENGINE WARNING ADD
#define ENGINE_OIL_LEVEL_ON			{ u8GSdata[31 + 36] |= 0x0F; u8GSdata[32 + 36] = 0xFF;} // ENGINE OIL LEVEL
#define BATTERY_CHARGING1_ON		{ u8GSdata[34 + 36] = 0xFF; u8GSdata[35 + 36] = 0xFF;} // BATTERY CHARGING-1


//Macro For LED OFF

//#define HIGH_TEMP_OFF				{ u8GSdata[0 + 36] = 0x00; u8GSdata[1 + 36] = 0x0F;}
//#define LED_FUEL_ECO_GRN_OFF		{ u8GSdata[3 + 36] = 0x00; u8GSdata[4 + 36] &= 0x0F;}
#define CRUISE_CONTROL_OFF			{ u8GSdata[4 + 36] &= 0xF0; u8GSdata[5 + 36] = 0x00;}
#define LIM_NOX_OFF					{ u8GSdata[7 + 36] = 0xF0; u8GSdata[8 + 36] = 0x00;}
#define LOW_GEAR_OFF				{ u8GSdata[9 + 36] = 0x00; u8GSdata[10 + 36] = 0x0F;}
#define RED_STOP_WARING_LIGHT_OFF	{ u8GSdata[12 + 36] = 0x00; u8GSdata[13 + 36] &= 0x0F;}
#define MAIN_BEAM_OFF				{ u8GSdata[13 + 36] &= 0xF0; u8GSdata[14 + 36] = 0x00;}
#define HIGH_GEAR_OFF				{ u8GSdata[16 + 36] = 0xF0; u8GSdata[17 + 36] = 0x00;}
#define YELLOW_WARNING_LIGHT_OFF	{ u8GSdata[18 + 36] = 0x00; u8GSdata[19 + 36] = 0x0F;}
#define DOOR_OPEN_OFF				{ u8GSdata[21 + 36] = 0x00; u8GSdata[22 + 36] &= 0x0F;}
#define POWER_TAKE_OFF_OFF			{ u8GSdata[22 + 36] = 0x00; u8GSdata[23 + 36] &= 0x0F;}
#define PARKING_BRAKE_OFF			{ u8GSdata[25 + 36] = 0xF0; u8GSdata[26 + 36] = 0x00;}
#define FRONT_REAR_FOG_OFF			{ u8GSdata[27 + 36] = 0x00; u8GSdata[28 + 36] = 0x0F;}
#define ENGINE_WARNING_ADD_OFF		{ u8GSdata[30 + 36] = 0x00; u8GSdata[31 + 36] &= 0x0F;}
#define ENGINE_OIL_LEVEL_OFF		{ u8GSdata[31 + 36] &= 0xF0; u8GSdata[32 + 36] = 0x00;}
#define BATTERY_CHARGING1_OFF		{ u8GSdata[34 + 36] = 0xF0; u8GSdata[35 + 36] = 0x00;}	






//*************************************************************************

#define NO_SERVICE_REMINDER				3

typedef enum
{
  eTURN_LEFT_INPUT,
  eTURN_RIGHT_INPUT,
  ePARK_BRAKE_INPUT,
  eLOW_OIL_INPUT,
  eCAB_TILT_INPUT,
  eMIL_LOW_INPUT,
  eFRONT_FOG_INPUT,
  eDOOR_LOW_INPUT,
  eRETARDER_HIGH_INPUT,
  eEXHAUST_INPUT,
  eMAIN_BEAM_INPUT,
  eHIGH_BEAM_INPUT,
  eCLUTCH_OIL_LEVEL_INPUT,
  eENGINE_COOLANT_LEVEL_INPUT,
  eESC_MIL_INPUT,
  eSEAT_BELT_INPUT,
  eBATT_CHARGING1_INPUT,
  eBATT_CHARGING2_INPUT,
  eAIR_FILTERCLOGGED_INPUT,
  eBRAKE_LIQUID_LEVEL_INPUT,
  eRED_STOP_LAMP,
  eYELLO_WRN_LAMP,
  eENGINE_OIL_LEVEL,
  eABS_FAILURE,
  eREVERSE_LIGHT,  
  eNUM_OF_INPUTS
};


typedef enum
{                       // bit data
	eTurnLampOff = 0,      	// 000
	eTurnRight,             // 001
	eTurnLeft,            	// 010
	eTurnLeftDoubleFlag,    // 011
	eTurnRightDoubleFlag,   // 100
	eHazard,	    		// 101
	eReserve1,	    		// 110
	eReserve2,   	    	// 111
} eTurnLampdataType3bit;


typedef enum
{                       // bit data
	eLedOff = 0,        // 000
	eLedOn,             // 001
	eError2,            // 010
	eError3,            // 011
	eLampFastBlink,     // 100
	eError5,	    	// 101
	eError6,	    	// 110
	eDataNA   	    	// 111
} eOBDLampdataType3bit;
	
typedef enum
{                       // bit data
	eLampOff = 0,       // 00
	eLampOn,            // 01
	eError,             // 10
	eDataNotAvailable   // 11
} eOBDLampdataType;

	

typedef enum
{						
	eBothLampOff = 0,		
	eLeftTurnFlash = 1, 		
	eRightTurnFlash = 2,			
	eRev = 3,	
	eErrorBothFlash = 14,
} eTurnLampType;


//Type of Buzzer sound
typedef enum
{
	eLifeCriticalAlarm = 0,
	eEngineCriticalAlarm,
	eGeneralAlarm,
	eTicAlarm, 
	eTocAlarm,
	eBeepAlarm,
	eContinueAlarm, 
	eStopAlarm, 
 } eBuzzerStateType;
	
// local declarations --------------------------------------------------------
typedef struct
{
	u32 	u32TimeStamp;
	u32 	u32DebounceTime;
	u8 		u8PrevStatus;	
	u8 		u8StatusFlag;	
	u8 		u8PinStatus;
	bool	bStatusChange;

}lampInputType;

//Buzzer Flag as per tell tale indicator  
typedef	struct
{
	u8 bFRONT_AIR_PRESSUR :1;//LSB
	u8 bREAR_AIR_PRESSUR :1;
	u8 bSEAT_BELT :1;
	u8 bTILT_CAB :1;
	u8 bHTW:1;
	u8 bLFW:1;
	u8 bLOW_DEF:1;
	u8 bLOW_OIL_PRESSURE:1;
	u8 bSTOP_ENGINE:1; 
	u8 bTRANCE_FLUDE:1;
	u8 bGSO:1;
	u8 bGEAR_UP:1;
	u8 bGEAR_DOWN:1;
	u8 bBRAKE_BEAR:1;
	u8 bPARK_BREAK:1; 
	u8 bTURN_RIGHT:1;
	u8 bTURN_LEFT:1;
	u8 bBuzzerEnable:1;	
	u8 bEngineOverSpeed:1;
	u8 bLowBrakeFluid:1;
	u8 bRes4:1;
	u8 bRes5:1;
	u8 bRes6:1;
	u8 bRes7:1;
	u8 u8BuzzerSound;
}BUZZER_TYPE;


// Local declarations --------------------------------------------------------

BUZZER_TYPE	sBuzz_Control = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x00};
lampInputType	sLampInputsProp[eNUM_OF_INPUTS];
static u16 u16AlarmTimeStamp = 0;	//Common Time stamps variable for all chimes
static bool bNeturalIndicatorStatus = 0; //Netural Indictor Status used for Parking brake buzzer 
static bool bClutchWearStatus = 0;		//Keep clutch wear lamp indicator status for CAN tx
static bool bBrakeWearStatus = 0;		//Keep Brak wear lamp indicator status for CAN tx
static bool bCanLostLampFlashFlag = TRUE; //Flag use for blink lamp while CAN sigal lost
static bool bToggleFlag2Hz = TRUE;
static bool bToggleFlag1Hz = TRUE;
bool bFlasher1_3hzTimeFlashFlag = 0;
bool bFlasher2_5hzTimeFlashFlag = 0;
bool bStopEnginelampStatus = FALSE;
static bool bCheckEnginelampStatus = FALSE;
static bool bScrlampStatus = FALSE;
static bool bMilMode3Status = FALSE;
static bool bMilMode4Status = FALSE;
static bool bPtoStatus = FALSE;



typedef struct{
	
	bool bSeatBeltStatus : 1;
	bool bBatCharging1Status : 1;
	bool bLiftAxelUpStatus : 1;
	bool bGenIllStatus : 1;
	bool bBatCharging2status : 1;
	bool bCabTiltStatus : 1;
	bool bAirFilterCloggedStatus : 1;
	bool bAbsMalTrailerStatus : 1;	
	
}INDICATOR_STATUS;

INDICATOR_STATUS sIndicatorStatus = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

static u16 u16UpdateRate = 2000;		//ms
static u8 u8CheckEngStatus,u8MilStatus,u8StopEngStatus;
static u8 u8CheckEngineFlash,u8MilFlash,u8StopEngineFlash;
static bool bTimeOutFlag65226_0x00 = 0;
static bool bTimeOutFlagDm165226_0x00 = 0;
static bool bTimeOutFlag65226_0x03 = 0;
static bool bTimeOutFlag65226_0x03_DM1 = 0;
static bool bTimeOutFlag65226_0x33 =0;
static bool bTimeOutFlagDm165226_0x33 =0;

static u8 u8CheckTransFlash = 0;
static u8 u8CheckTransStatus = 0;
static bool bCheckEngineFlagRate1Hz = FALSE;
static bool bCheckEngineFlagRate2Hz = FALSE;
static bool bStopEngineFlagRate1Hz = FALSE;
static bool bStopEngineFlagRate2Hz = FALSE;
static u8 u8TpmsLampStatus = 0;




//Extern Variable-----------------------------------------------------------
IO_CONTROL_TYPE InputOutputLampControl = {0U,0U,0U,0U,0U,0U};

extern u8 u8GSdata[GS_DATA_SIZE];

// private prototypes --------------------------------------------------------
//Lamp Connected With IIC1

/// @addtogroup TELL_TALE_LIBRARY
/// @{ 
/// @defgroup TellTaleLib Private functions used in Tell tale file
//// 
///   
/// @addtogroup TellTaleLib
/// @{

static void _vTurnLeftLamp(void);
static void _vTurnRightLamp(void); 			 
static void _vMainBeamLamp(void); 	
static void _vExhaustBrakeLamp(void);  				 
static void _vClutchOilLevelLamp(void); 	
static void _vDoorOpenLamp(void);
static void _vAirFilterCloggedLamp(void);
static void _vReverseLight(void);
static void _vYellowWarningLight(void);
static void _vRedStopWarning(void);
static void _vEngineOilLevel(void);



static void _vHighBeamLamp(void);  				 
static void _vNeutral(void); 					 
static void _vCheckEngineLamp(void);		 
static void _vFrontRearLamp(void);    			 
static void _vParkBrakeLamp(void);			     
static void _vPowerTakeOffLamp(void);			 
static void _vAbsMilTrailerLamp(void); 			 
static void _vTransFluidTemp(void);			 
static void _vGearShifUpLamp(void);
static void _vGearShifDownLamp(void);
static void _vStopEngineLamp(void); 			 
static void _vCruiseControlLamp(void);	 
static void _vHestLamp(void); 					 
static void _vCoolantTempLamp(void);		 
static void _vRpmEconomy(void); 				 

//Lamp Connected With IIC2
static void _vLowAirPressureLamp(void);
static void _vLowFuelLamp(void);  				 
static void _vLowOilLamp(void);					 
static void _vTiltCabLockLamp(void); 			 
static void _vOverSpeedSpeedo(void);			 
static void _vOverSpeedRPM(void); 		 
static void _vSeatBeltLamp(void);    			 
static void _vEngineCoolantLevel(void);

   			 
static void _vMilL(void);             			 
//Lamp Direct microDriven
static void _vBattCharging1Lamp(void); 
static void _vUpdatePinStatus(void);
static void _vInputsDebounce(void);
static void _vCheckBuzzerStatus(void);
static void _vLifeCriticalAlarm(void);
static void _vEngineCriticalAlarm(void); 
static void _vGeneralAlarm(void);
static void _vTicAlarm(void);
static void _vTocAlarm(void);
static void _vTicTocAlarm(eBuzzerStateType);
static void _vBeepSound(void);
static void _vStopBuzzer(void);
static void _vOnBuzzer();
static void _vOnBuzzer3_5kHz();
static void _vOnBuzzer_4kHz();
static void _vOnBuzzer_2kHz();
static void _vReadPgn65226_0x00(void);
static void _vReadPgn65226_0x03(void);
static void _vReadPgn65226_0x33(void);

static void _vBrakeLiquidLevel(void);
void _vSetLampBilinkFlag();
void _vHazard();
void _vBattCharging2Lamp(void);
static void _vBuzzerEnableDisable(void);
static void _vReadPgn64775_DLCC1(void);
static void _vLiftAxelFaultLamp(void);
static void _vTpmsLamp(void);


/// @} // endgroup TellTaleLib

/// @} // endgroup TELL_TALE_LIBRARY



/*********************************************************************//**
 *
 * update  current status of input in buffer
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vUpdatePinStatus(void)
{
	
	sLampInputsProp[eTURN_LEFT_INPUT].u8PinStatus = INPUT_TURN_LEFT;
	sLampInputsProp[eTURN_RIGHT_INPUT].u8PinStatus = INPUT_TURN_RIGHT;
	sLampInputsProp[ePARK_BRAKE_INPUT].u8PinStatus = INPUT_PARK_BRAKE;
	sLampInputsProp[eLOW_OIL_INPUT].u8PinStatus =  INPUT_LOW_OIL_PRES;
	sLampInputsProp[eCAB_TILT_INPUT].u8PinStatus = INPUT_CAB_TILT;
	sLampInputsProp[eMIL_LOW_INPUT].u8PinStatus = INPUT_MIL_LOW;
	sLampInputsProp[eFRONT_FOG_INPUT].u8PinStatus = FRONT_FOG_HIGH;
	sLampInputsProp[eDOOR_LOW_INPUT].u8PinStatus = INPUT_DOOR_LOW;
	sLampInputsProp[eRETARDER_HIGH_INPUT].u8PinStatus = INPUT_RETARDER_HIGH;
	sLampInputsProp[eEXHAUST_INPUT].u8PinStatus = INPUT_EXHAUST;
	sLampInputsProp[eMAIN_BEAM_INPUT].u8PinStatus = INPUT_MAIN_BEAM;
	sLampInputsProp[eHIGH_BEAM_INPUT].u8PinStatus = INPUT_HIGH_BEAM;
	sLampInputsProp[eCLUTCH_OIL_LEVEL_INPUT].u8PinStatus = INPUT_CLUTCH_OIL_LEVEL_INPUT;
	sLampInputsProp[eENGINE_COOLANT_LEVEL_INPUT].u8PinStatus = INPUT_ENGINE_COOLANT_LEVEL;
	sLampInputsProp[eESC_MIL_INPUT].u8PinStatus = INPUT_ESC_MIL;
	sLampInputsProp[eSEAT_BELT_INPUT].u8PinStatus = INPUT_SEAT_BELT;
	sLampInputsProp[eBATT_CHARGING1_INPUT].u8PinStatus = INPUT_BATT_CHARGING1;
	sLampInputsProp[eBATT_CHARGING2_INPUT].u8PinStatus = INPUT_BATT_CHARGING2;
	sLampInputsProp[eAIR_FILTERCLOGGED_INPUT].u8PinStatus = INPUT_AIR_FILTER;
	sLampInputsProp[eBRAKE_LIQUID_LEVEL_INPUT].u8PinStatus = INPUT_BRAKE_LIQUID_LEVEL;
	sLampInputsProp[eRED_STOP_LAMP].u8PinStatus = INPUT_RED_STOP_LAMP;
	sLampInputsProp[eYELLO_WRN_LAMP].u8PinStatus = INPUT_YELLO_WRN_LAMP;
	sLampInputsProp[eENGINE_OIL_LEVEL].u8PinStatus = INPUT_ENGINE_OIL_LEVEL;
	sLampInputsProp[eABS_FAILURE].u8PinStatus = INPUT_ABS_FAILURE;
	sLampInputsProp[eREVERSE_LIGHT].u8PinStatus = INPUT_REVERSE_LIGHT;


}


/*********************************************************************//**
*
* Check current and previous status of pins and if input changes then update 
* current inputs after complet debouncing time.
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vInputsDebounce(void)
{

	u8 i;

	for(i = eTURN_LEFT_INPUT; i < eNUM_OF_INPUTS; i++)
	{
		if(sLampInputsProp[i].u8PinStatus == sLampInputsProp[i].u8PrevStatus)
		{
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = FALSE;
		}
		else if(sLampInputsProp[i].u8PinStatus != sLampInputsProp[i].u8PrevStatus 
		&& sLampInputsProp[i].bStatusChange == FALSE)
		{
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = TRUE;
		}

		if(u32GetClockTicks() - sLampInputsProp[i].u32TimeStamp > sLampInputsProp[i].u32DebounceTime
		&& sLampInputsProp[i].bStatusChange == TRUE)
		{
			sLampInputsProp[i].u8PrevStatus = sLampInputsProp[i].u8PinStatus;
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = FALSE;
		}
		if(sLampInputsProp[i].u8PrevStatus)
			sLampInputsProp[i].u8StatusFlag = HIGH;
		else
			sLampInputsProp[i].u8StatusFlag = LOW;
	}


}


/*********************************************************************//**
*
* Update current status of pins in previous inputs buffer
*
* @param      None    
*
* @return     None
*************************************************************************/

void vLedPrevStateInit(void)
{
	u8 i;
	_vUpdatePinStatus();

	for(i = eTURN_LEFT_INPUT; i < eNUM_OF_INPUTS ; i++ )
	{
		sLampInputsProp[i].u8PrevStatus = sLampInputsProp[i].u8PinStatus;
		sLampInputsProp[i].u32DebounceTime = INPUT_DEBOUNCE_TIME;
	}

		
}


/*********************************************************************//**
*
* ON / OFF indicators according to digital and CAN inputs
*
* @param      None    
*
* @return     None
*************************************************************************/
u8 lampOn = 0;

void vControlTellTales(void)
{
static bool flag = 1;

	if(sTestEquimentControl.tellTalesFlag == TRUE)
		{	return; }
		else
		{

			_vUpdatePinStatus();
			_vInputsDebounce(); 
            _vTurnLeftLamp();
			_vTurnRightLamp();
			_vMainBeamLamp();		
			_vExhaustBrakeLamp();
			_vClutchOilLevelLamp();
			 _vDoorOpenLamp();
			_vSeatBeltLamp();			
			_vHighBeamLamp();
			_vFrontRearLamp();
			_vMilL();
			_vLowOilLamp();
			_vParkBrakeLamp();			
			_vAirFilterCloggedLamp();
			_vTiltCabLockLamp();
			_vBattCharging1Lamp();			
			_vReverseLight();
 			_vYellowWarningLight();
 			_vRedStopWarning();
			_vEngineOilLevel();
		    _vCruiseControlLamp();
			_vEngineCoolantLevel();
			_vBrakeLiquidLevel();
			_vGearShifUpLamp();
			_vGearShifDownLamp();

		
			//_vAbsMilTrailerLamp();			
			
			//_vHestLamp();
			_vCoolantTempLamp();
			//_vRpmEconomy();
			 _vLowAirPressureLamp();
			_vLowFuelLamp();
			
			_vSetLampBilinkFlag();	
			
			_vOverSpeedSpeedo();
			_vOverSpeedRPM();
			
			
			
		
			//Must Call _vReadPgn65226_0x00 before calling Mil,Stop Eng, Check Eng
			_vReadPgn65226_0x00();
			_vReadPgn65226_0x03();
			_vReadPgn65226_0x33();			
			_vReadPgn64775_DLCC1();
			 
			//_vStopEngineLamp();
			_vCheckEngineLamp();
			_vPowerTakeOffLamp();
			
			
			//_vBattCharging2Lamp();
			//_vLiftAxelFaultLamp();
			//_vBuzzerEnableDisable();			
			//_vNeutral();	
			
			//_vTransFluidTemp();
										
			
		
			//On indicator when ignition in ON 
		// Check direct ignition becouse when ign off then glow low input indictors	
			_vCheckBuzzerStatus();				
			vLedDataSendToSpi();
			
			

}
}


void vLampsWorkInSleepMode()
{

		_vUpdatePinStatus();
		_vInputsDebounce();	
		_vHazard();
		_vCheckBuzzerStatus();	
		_vSetLampBilinkFlag();
	

}
/*********************************************************************//**
*
* Indicator ON/OFF control flag set and clear according to On/OFF time 
*
* @param      None    
*
* @return     None
*************************************************************************/

void _vSetLampBilinkFlag()
{
	static u32 CanOutTime = 0;
	static u32 clkTime = 0;
	static u32 clkTime1 = 0;
	static u32 clkTime2 = 0;
	static u32 clkTime3 = 0;
	static u32 clkTime4 = 0;

	if(u32GetClockTicks() - clkTime  > CanOutTime)
	{
		 clkTime = u32GetClockTicks();

		if(bCanLostLampFlashFlag == FALSE)
		{
			 bCanLostLampFlashFlag = TRUE;
			 CanOutTime = INDICATOR_ON_TIME;
		}
		else
		{
			bCanLostLampFlashFlag = FALSE;
			CanOutTime = INDICATOR_OFF_TIME;
		}

	}

	//1 Hz toggle flag
	if(u32GetClockTicks() - clkTime1  > 500)
	{
		 clkTime1 = u32GetClockTicks();
		if(bToggleFlag1Hz == FALSE)
		{
			 bToggleFlag1Hz = TRUE;
			
		}
		else
		{
			 bToggleFlag1Hz = FALSE;
		}

	}

	//2 Hz toggle flag
	if(u32GetClockTicks() - clkTime2  > 250)
	{
		 clkTime2 = u32GetClockTicks();
		if(bToggleFlag2Hz == FALSE)
		{
			 bToggleFlag2Hz = TRUE;
						
		}
		else
		{
			 bToggleFlag2Hz = FALSE;
			
		}

	}

	//1.36  means 82 times flash per Minute 
	if(u32GetClockTicks() - clkTime3  > 366)
	{
		 clkTime3 = u32GetClockTicks();
		if(bFlasher1_3hzTimeFlashFlag == FALSE)
		{
			 bFlasher1_3hzTimeFlashFlag = TRUE;
			
		}
		else
		{
			 bFlasher1_3hzTimeFlashFlag = FALSE;
		}

	}

	//2.53hz  means 152 times flash per Minute 
	if(u32GetClockTicks() - clkTime4  > 197)
	{
		 clkTime4 = u32GetClockTicks();
		if(bFlasher2_5hzTimeFlashFlag == FALSE)
		{
			 bFlasher2_5hzTimeFlashFlag = TRUE;
			
		}
		else
		{
			 bFlasher2_5hzTimeFlashFlag = FALSE;
		}

	}



}

/*********************************************************************//**
*
* Indicator ON/OFF control flag status
*
* @param      None    
*
* @return     Indicator ON/OFF control flag status
*************************************************************************/

bool bGetLampBlinkingFlagstatus()
{
	return bCanLostLampFlashFlag;

}





static void _vEngineOilLevel(void)
{
		

	if(sLampInputsProp[eENGINE_OIL_LEVEL].u8StatusFlag == HIGH)
	{
		ENGINE_OIL_LEVEL_ON;
	}
	else
	{
		ENGINE_OIL_LEVEL_OFF;
	}

		
		
}

static void _vRedStopWarning(void)
{
		

	if(sLampInputsProp[eRED_STOP_LAMP].u8StatusFlag == HIGH)
	{
		RED_STOP_WARING_LIGHT_ON;
	}
	else
	{
		RED_STOP_WARING_LIGHT_OFF;
	}

		
		
}



static void _vYellowWarningLight(void)
{
	

	if(sLampInputsProp[eYELLO_WRN_LAMP].u8StatusFlag == HIGH)
	{
		YELLOW_WARNING_LIGHT_ON;
	}
	else
	{
		YELLOW_WARNING_LIGHT_OFF;
	}

		
		
}


static void _vReverseLight(void)
{
	

	if(sLampInputsProp[eREVERSE_LIGHT].u8StatusFlag == LOW)
	{
		REVERSE_LIGHT_ON;
	}
	else
	{
		REVERSE_LIGHT_OFF;
	}

		
		
}


bool bAbsFailure(void)
{
		
bool status = FALSE;

	if(sLampInputsProp[eABS_FAILURE].u8StatusFlag == HIGH)
	{
		status = TRUE;
	}
	else
	{
		status = FALSE;
	}

	return(status);
		
}



/*********************************************************************//**
*
* On/OFF High beam indicator as per input
*
* @param      None    
*
* @return     None
*************************************************************************/

void _vHighBeamLamp(void)
{
		

	if(sLampInputsProp[eHIGH_BEAM_INPUT].u8StatusFlag == LOW)
	{
		HIGH_BEAM_ON;
	}
	else
	{
		HIGH_BEAM_OFF;
	}

		
		
}

static void _vReadPgn65226_0x00(void)
{

	bTimeOutFlag65226_0x00 =  bGetPGNTimeOut(eJCheckEngineSPN624);
	bTimeOutFlagDm165226_0x00 =  bGetPGNTimeOut(eJCheckEngineDM1_624);	


	//Check Engine , Mil and Stop engine have a same PGN time out 
	if(bGetPGNDataReady(eJCheckEngineSPN624) )
	{
		u8CheckEngStatus =   ((eOBDLampdataType)u32GetPGNdata(eJCheckEngineSPN624));
		u8CheckEngineFlash=	((eOBDLampdataType)u32GetPGNdata(eJCheckEngineSPN3040));
		u8MilStatus =   ((eOBDLampdataType)u32GetPGNdata(eJMilSPN1213));
		u8MilFlash=	((eOBDLampdataType)u32GetPGNdata(eJMilSPN3038));
		u8StopEngStatus =   ((eOBDLampdataType)u32GetPGNdata(eJStopEngine623));
		u8StopEngineFlash=	((eOBDLampdataType)u32GetPGNdata(eJStopEngine3039));
		ClearPGNDataReady(eJCheckEngineSPN624);
	}
	else if(bGetPGNDataReady(eJCheckEngineDM1_624) ) 
	{
		u8CheckEngStatus =   ((eOBDLampdataType)u32GetPGNdata(eJCheckEngineDM1_624));
		u8CheckEngineFlash =	((eOBDLampdataType)u32GetPGNdata(eJCheckEngineDM1_3040));
		u8MilStatus =   ((eOBDLampdataType)u32GetPGNdata(eJMilDM1_1213));
		u8MilFlash=	((eOBDLampdataType)u32GetPGNdata(eJMilDM1_3038));
		u8StopEngStatus =   ((eOBDLampdataType)u32GetPGNdata(eJStopEngineDM1_623));
		u8StopEngineFlash=	((eOBDLampdataType)u32GetPGNdata(eJStopEngineDM1_3039));
		ClearPGNDataReady(eJCheckEngineDM1_624);
	}
		


}



static void _vReadPgn65226_0x03(void)
{	
	u8 u8Temp1 = 0;
	u8 u8Temp2 = 0;
	bTimeOutFlag65226_0x03 =  bGetPGNTimeOut(eJCheckTrance);
	bTimeOutFlag65226_0x03_DM1 = bGetPGNTimeOut(eJCheckTranceDM1);	
		
	//Check Trance 
	if(bGetPGNDataReady(eJCheckTrance) )
	{
		u8Temp1 = (eOBDLampdataType)u32GetPGNdata(eJCheckTrance);
		u8Temp2 = (u8Temp1 >> 2);
		u8Temp2 = u8Temp2 & 0x03;
		u8Temp1 = u8Temp1 & 0x03;

		
		if( u8Temp1 == eError || u8Temp1 == eDataNotAvailable)
			u8CheckTransStatus = eError;
		else	
			u8CheckTransStatus =  u8Temp1;

		if( u8Temp2 == eError || u8Temp2 == eDataNotAvailable)
			u8CheckTransFlash = eError;
		else
			u8CheckTransFlash =   u8Temp2;
		
		ClearPGNDataReady(eJCheckTrance);
		
	}
	else if(bGetPGNDataReady(eJCheckTranceDM1) )
	{
		u8Temp1 = (eOBDLampdataType)u32GetPGNdata(eJCheckTranceDM1);
		u8Temp2 = (u8Temp1 >> 2);
		u8Temp2 = u8Temp2 & 0x03;
		u8Temp1 = u8Temp1 & 0x03;

		
		if( u8Temp1 == eError || u8Temp1 == eDataNotAvailable)
			u8CheckTransStatus = eError;
		else	
			u8CheckTransStatus =  u8Temp1;

		if( u8Temp2 == eError || u8Temp2 == eDataNotAvailable)
			u8CheckTransFlash = eError;
		else
			u8CheckTransFlash =   u8Temp2;
		
		ClearPGNDataReady(eJCheckTranceDM1);
	}
	


}


static void _vReadPgn65226_0x33(void)
{

	bTimeOutFlag65226_0x33 =  bGetPGNTimeOut(eJTPMS_Lamp);
	bTimeOutFlagDm165226_0x33 =  bGetPGNTimeOut(eJTPMS_LampDM1_624);	

	 
	if(bGetPGNDataReady(eJTPMS_Lamp) ) // Read single fram message 
	{
		u8TpmsLampStatus =   ((eOBDLampdataType)u32GetPGNdata(eJTPMS_Lamp));
		
		ClearPGNDataReady(eJTPMS_Lamp);
	}
	else if(bGetPGNDataReady(eJTPMS_LampDM1_624) ) //Read multifam message 
	{
		u8TpmsLampStatus =   ((eOBDLampdataType)u32GetPGNdata(eJTPMS_LampDM1_624));
		
		ClearPGNDataReady(eJTPMS_LampDM1_624);
	}
		


}


		
/*********************************************************************//**
*
* Check check Transmission indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/


bool bCheckTransmissionLamp(void)
{
	u8 u8TransType = 0;
	static bool StatusFlag = 0;
	static u16 clkTime = 0; 

		if(sTestEquimentControl.tellTalesFlag == TRUE)
		{	   
		   if( ((InputOutputLampControl.IoControlLampbytes.Byte4.bCheckTransLamp == TRUE )
			   || (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			   && (sIndicatorEnable.Enablebytes.Byte2.bCheckTrans == eEnable))
		   {
			   StatusFlag = TRUE;
		   }
		   else
		   {
			   StatusFlag = FALSE;
		   }
		   
		}
		else
		{

			
			u8TransType = u8GetTransmissionType();

			if(sIndicatorEnable.Enablebytes.Byte2.bCheckTrans == eEnable && 
				(u8TransType == AT_TRANS || u8TransType == AMT_TRANS))
			{
				if(bGetPGNTimeOut(eJETC7) || (bTimeOutFlag65226_0x03 == TRUE && bTimeOutFlag65226_0x03_DM1 == TRUE)) 
				{
					if(bCanLostLampFlashFlag)
					{
						StatusFlag = TRUE;
						
					}
					else
					{
						StatusFlag = FALSE;				
					}
				}			
				else
				{
					switch (u8CheckTransStatus)
					{
						case eLedOff :
							if(u8CheckTransFlash == TRUE)
							{
								if(u16GetClockTicks() - clkTime  > INDICATOR_FLASHING_TIME)
								{
									clkTime = u16GetClockTicks();
									if(StatusFlag == TRUE)
									{
										StatusFlag = FALSE;							
									}
									else
								 	{
										StatusFlag = TRUE;							
								 	}
								}	
							}
							else
							{
								StatusFlag = FALSE;
							}
						
						break;
						case eLedOn :
							if(u8CheckTransFlash == FALSE)
							{
								StatusFlag = TRUE;
							}
							else if(u8CheckTransFlash == TRUE)
							{
								if(u16GetClockTicks() - clkTime  > INDICATOR_FLASHING_TIME)
								{
									clkTime = u16GetClockTicks();
									if(StatusFlag == TRUE)
									{
										StatusFlag = FALSE;							
									}
									else
								 	{
										StatusFlag = TRUE;							
								 	}
								}
							}
							else
							{
								StatusFlag = FALSE;
							}
						break;	
						default :	
							
							StatusFlag = FALSE;
						break;
					}			
				}
			}
			else
			{
				StatusFlag = FALSE;
			}
		}

	return(StatusFlag);

}


static void _vReadPgn64775_DLCC1(void)
{
	u8 Data =0;
	static u8 CheckEnginePreviousData = 0;
	u32 CheckEngineTimeDiffrence = 0;
	static u32 CheckEnginePreTimeStamp = 0;
	
	static u8 StopEnginePreviousData = 0;
	u32 StopEngineTimeDiffrence = 0;
	static u32 StopEnginePreTimeStamp = 0;


	if(sIndicatorInputType.Inputsbytes.Byte4.bDlcc1Dm1Selection == eDlcc1)
	{
		if(bGetPGNDataReady(eJCheckEng5078) ) 
		{
			ClearPGNDataReady(eJCheckEng5078);
			
			Data =	(eOBDLampdataType)u32GetPGNdata(eJCheckEng5078);
			if(Data != CheckEnginePreviousData) 
			{
				CheckEngineTimeDiffrence = u32GetClockTicks();
				CheckEngineTimeDiffrence = CheckEngineTimeDiffrence - CheckEnginePreTimeStamp;
				
				if(Data == eLampOn || Data == eLampOff)
				{
					if(Data != CheckEnginePreviousData)
					{				
						CheckEnginePreTimeStamp = u32GetClockTicks();
						CheckEnginePreviousData = Data;
					}
				}					

				if(CheckEngineTimeDiffrence >= (INTERVAL_TIME_250 - TIME_HYS)
					&& CheckEngineTimeDiffrence <= (INTERVAL_TIME_250 + TIME_HYS) )
				{
					bCheckEngineFlagRate2Hz = TRUE;				

				}
				else if(CheckEngineTimeDiffrence >= (INTERVAL_TIME_500 - TIME_HYS) 
					&& CheckEngineTimeDiffrence <= (INTERVAL_TIME_500 + TIME_HYS) )
				{
					bCheckEngineFlagRate1Hz = TRUE;	
				}
				else
				{
					bCheckEngineFlagRate1Hz = FALSE;
					bCheckEngineFlagRate2Hz = FALSE;
				}
			}
			else
			{
			    CheckEngineTimeDiffrence = u32GetClockTicks();
				CheckEngineTimeDiffrence = CheckEngineTimeDiffrence - CheckEnginePreTimeStamp;
				if(CheckEngineTimeDiffrence > (INTERVAL_TIME_500 + TIME_HYS) )
				{
				 	bCheckEngineFlagRate1Hz = FALSE;
					bCheckEngineFlagRate2Hz = FALSE;
					CheckEnginePreTimeStamp = u32GetClockTicks();
				}
			}
			
			//*******End Check engine DLCC1******************************************
			Data =	(eOBDLampdataType)u32GetPGNdata(eJStopEng5079);

		  	if(Data != StopEnginePreviousData)
			{
				StopEngineTimeDiffrence = u32GetClockTicks();
				StopEngineTimeDiffrence = StopEngineTimeDiffrence - StopEnginePreTimeStamp;
				
				if(Data == eLampOn || Data == eLampOff)
				{

					if(Data != StopEnginePreviousData)
					{
					    StopEnginePreTimeStamp = u32GetClockTicks();
						StopEnginePreviousData = Data;
					}
				}
				
				if(StopEngineTimeDiffrence >= (INTERVAL_TIME_250 - TIME_HYS) 
					&& StopEngineTimeDiffrence <= (INTERVAL_TIME_250 + TIME_HYS) )
				{
					bStopEngineFlagRate2Hz = TRUE;				

				}
				else if(StopEngineTimeDiffrence >= (INTERVAL_TIME_500 - TIME_HYS) 
					&& StopEngineTimeDiffrence <= (INTERVAL_TIME_500 + TIME_HYS) )
				{
					bStopEngineFlagRate1Hz = TRUE;	
				}
				else
				{
					bStopEngineFlagRate1Hz = FALSE;
					bStopEngineFlagRate2Hz = FALSE;
				}
			}
			else
			{
			    StopEngineTimeDiffrence = u32GetClockTicks();
				StopEngineTimeDiffrence = StopEngineTimeDiffrence - StopEnginePreTimeStamp;
				if(StopEngineTimeDiffrence > (INTERVAL_TIME_500 + TIME_HYS) )
				{
					bStopEngineFlagRate1Hz = FALSE;
					bStopEngineFlagRate2Hz = FALSE;	
					StopEnginePreTimeStamp = u32GetClockTicks();
				}
			}
			
		}
	}


}


/*********************************************************************//**
*
* On/OFF/FLASH Check Engine indicator as per CAN input only single frame implement 
* Multi frame is peding
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vCheckEngineLamp(void)
{	
	static bool LedStatusFlag = 0;
	static u16 clkTime = 0;	
	u8 Data = 0;

	if(sIndicatorInputType.Inputsbytes.Byte4.bDlcc1Dm1Selection == eDlcc1)
	{
     
		if(bGetPGNTimeOut(eJCheckEng5078)) 
		{
			if(bCanLostLampFlashFlag)
			{					
				ENGINE_WARNING_ADD_ON;
			}
			else					
			{					
				ENGINE_WARNING_ADD_OFF;
			}
		}
		else
		{
			if(bCheckEngineFlagRate2Hz)
			{
				//flashing lamp by 2hz
				if(bToggleFlag2Hz)					
				{					
					ENGINE_WARNING_ADD_ON;
				}
				else					
				{					
					ENGINE_WARNING_ADD_OFF;
				}	
				

			}
			else if(bCheckEngineFlagRate1Hz )
			{
				//flashing lamp by 1hz
				if(bToggleFlag1Hz)					
				{					
					ENGINE_WARNING_ADD_ON;
				}
				else					
				{					
					ENGINE_WARNING_ADD_OFF;
				}

			}
			else
			{			 
				Data =	(eOBDLampdataType)u32GetPGNdata(eJCheckEng5078);	
				
				if(Data == eLampOn)
				{
					ENGINE_WARNING_ADD_ON;
				}
				else
				{
					ENGINE_WARNING_ADD_OFF;
				}
			}
			
			
		}

	}
	else
	{

		if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)  
		{
		  	
				if(bCanLostLampFlashFlag)
				{					
					ENGINE_WARNING_ADD_ON;
				}
				else					
				{					
					 ENGINE_WARNING_ADD_OFF;
				}
				  
		}
		else
		{
			if( u8CheckEngStatus == 1 && u8CheckEngineFlash == 0 )
			{
				if(u16GetClockTicks() - clkTime  > 500)
				{
					clkTime = u16GetClockTicks() ;
					LedStatusFlag = ~LedStatusFlag;
					if(LedStatusFlag)
					{
						ENGINE_WARNING_ADD_ON;	  
					}
					else
					{
						ENGINE_WARNING_ADD_OFF;		 
					}
				}
			}		
			else if( u8CheckEngStatus == 1 && u8CheckEngineFlash == 1 )//Fast Flash
			{
				if(u16GetClockTicks() - clkTime  > 250)
				{
					clkTime = u16GetClockTicks() ;
					LedStatusFlag = ~LedStatusFlag;
					if(LedStatusFlag)
					{
						ENGINE_WARNING_ADD_ON;			  
					}
					else
					{
						ENGINE_WARNING_ADD_OFF;			 
					}
				}
			} 
			else if( u8CheckEngStatus == 1 && u8CheckEngineFlash == 3 )
			{
				ENGINE_WARNING_ADD_ON;
			}
			else 
			{
				ENGINE_WARNING_ADD_OFF;
			}
			
		}
	
	}

	//************For visit workshop Test msg display***********************
	{

		if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)  
		{
		  		bCheckEnginelampStatus = FALSE;				  
		}
		else
		{		
			
			if( (u8CheckEngStatus == 1) && ( (u8CheckEngineFlash == 0) || (u8CheckEngineFlash == 1) 
				|| (u8CheckEngineFlash == 3) ) )
			{
				bCheckEnginelampStatus = TRUE;
			}				
			else 
			{				
				bCheckEnginelampStatus = FALSE;
			}
			
		}
	
	}

	//**************************************************************

}

/*********************************************************************//**
*
* On/OFF Stop Engine indicator as per CAN input only single frame implement 
* Multi frame is peding 
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vStopEngineLamp(void)
{

	static bool LedStatusFlag = 0;
	static u16 clkTime = 0;	
	static bool StatusFlag = 0;
				
	u8 Data = 0;

	if(sIndicatorEnable.Enablebytes.Byte3.bStopEngine == eEnable)
	{
		if(sIndicatorInputType.Inputsbytes.Byte4.bDlcc1Dm1Selection == eDlcc1)
		{
		
			if(bGetPGNTimeOut(eJStopEng5079)) 
			{
				sBuzz_Control.bSTOP_ENGINE = FALSE;
				
				if(bCanLostLampFlashFlag)
				{					
					//STOP_ENGINE_ON;
				}
				else					
				{					
					//STOP_ENGINE_OFF;
				}
			}
			else
			{
						
				if(bStopEngineFlagRate2Hz)
				{
					//flashing lamp by 2hz
					sBuzz_Control.bSTOP_ENGINE = FALSE;

					
					if(bToggleFlag2Hz)					
					{					
						//STOP_ENGINE_ON;
					}
					else					
					{					
						//STOP_ENGINE_OFF;
					}				
		
				}
				else if(bStopEngineFlagRate1Hz )
				{
					//flashing lamp by 1hz
					sBuzz_Control.bSTOP_ENGINE = FALSE;

					
					if(bToggleFlag1Hz)					
					{					
						//STOP_ENGINE_ON;
					}
					else					
					{					
						//STOP_ENGINE_OFF;
					}
		
				}
				else
				{
					Data =	(eOBDLampdataType)u32GetPGNdata(eJStopEng5079); 
					if(Data == eLampOn)
					{
						//STOP_ENGINE_ON;
						sBuzz_Control.bSTOP_ENGINE = TRUE;
					}
					else
					{
						//STOP_ENGINE_OFF;
						sBuzz_Control.bSTOP_ENGINE = FALSE;
					}
				}
				
			}
		
		}
		else //check DM1 message 
		{		
			if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)
			{
				
				sBuzz_Control.bSTOP_ENGINE = FALSE;
					
				if(bCanLostLampFlashFlag)
				{				 
				 	//STOP_ENGINE_ON;
				}
				else					
				{				 
					//STOP_ENGINE_OFF;
				}
				
			}
			else
			{	
			 
				 if( u8StopEngStatus == 1 && u8StopEngineFlash == 0  )
				{
					sBuzz_Control.bSTOP_ENGINE = FALSE;
					if(u16GetClockTicks() - clkTime  > 500)
					{
						clkTime = u16GetClockTicks() ;
						LedStatusFlag = ~LedStatusFlag;
						if(LedStatusFlag)
						{
							//STOP_ENGINE_ON			  
						}
						else
						{
							//STOP_ENGINE_OFF			 
						}
					}
				}	
				else if( u8StopEngStatus == 1 && u8StopEngineFlash == 1 ) //Fast Flash
				{
					sBuzz_Control.bSTOP_ENGINE = FALSE;
					if(u16GetClockTicks() - clkTime  > 250)
					{
						clkTime = u16GetClockTicks() ;
						LedStatusFlag = ~LedStatusFlag;
						if(LedStatusFlag)
						{
							//STOP_ENGINE_ON			  
						}
						else
						{
							//STOP_ENGINE_OFF			 
						}
					}
				}
				else if( u8StopEngStatus == 1 && u8StopEngineFlash == 3 ) 
				{
					//STOP_ENGINE_ON		 
					sBuzz_Control.bSTOP_ENGINE = TRUE;
				}
				else
				{
					//STOP_ENGINE_OFF
					sBuzz_Control.bSTOP_ENGINE = FALSE;
					
				}

		  }
			
		}
		
	}
	else
	{
		//STOP_ENGINE_OFF
		sBuzz_Control.bSTOP_ENGINE = FALSE;		
	}

	//********For stop engine Text display*************************************
			
			if(bTimeOutFlag65226_0x00 == TRUE && bTimeOutFlagDm165226_0x00 == TRUE)
			{
							
				bStopEnginelampStatus = FALSE;				
			}
			else
			{		   
				 if( (u8StopEngStatus == 1) && ( (u8StopEngineFlash == 0) 
				 	|| (u8StopEngineFlash == 1) || (u8StopEngineFlash == 3) ) )
				{
					 bStopEnginelampStatus = TRUE;
				}					
				else
				{				
					bStopEnginelampStatus = FALSE;
				}

		  }
			
		//********************************************************

	if(sBuzzerEnable.Enablebytes.Byte2.bStopEngine == eDisable)	
		sBuzz_Control.bSTOP_ENGINE = FALSE;

}

/*********************************************************************//**
*
* On/OFF Mil indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/
void _vMilL(void)
{
		
		
	if(sLampInputsProp[eMIL_LOW_INPUT].u8StatusFlag == HIGH  )
	{
		MIL_ON;
	}
	else
	{
		MIL_OFF;
	}
		
	
}




/*********************************************************************//**
*
* On/OFF Front Rear indicator as per CAN input 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vFrontRearLamp(void)
{



	if(sLampInputsProp[eFRONT_FOG_INPUT].u8StatusFlag == LOW)
	{
		FRONT_REAR_FOG_ON;
	}
	else
		{
		 FRONT_REAR_FOG_OFF;
		}

	
}


/*********************************************************************//**
*
* On/OFF Park break indicator as per CAN input 
*
* @param      None    
*
* @return     None
*************************************************************************/


void _vParkBrakeLamp(void)
{
	
		
		if(sLampInputsProp[ePARK_BRAKE_INPUT].u8StatusFlag == HIGH)
		{
			PARKING_BRAKE_ON;
			sBuzz_Control.bPARK_BREAK = TRUE;
		}
		else
		{
			PARKING_BRAKE_OFF;
			sBuzz_Control.bPARK_BREAK = FALSE;
		}
	
	
}




/*********************************************************************//**
*
*  ABS trailer indicator 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vAbsMilTrailerLamp(void)
{

	u8 Data = 0;
	u8 Data1 = 0;

	if(sIndicatorEnable.Enablebytes.Byte2.bAbsMilTrailer == eEnable)
	{

		if( !bGetPGNTimeOut(eJAbsTrailer) )
		{
		  Data =	(eOBDLampdataType)u32GetPGNdata(eJAbsTrailer);
		}
	    if( !bGetPGNTimeOut(eJAbsTrailer65378) )
		{
			Data1 =	(eOBDLampdataType)u32GetPGNdata(eJAbsTrailer65378);
		}

		if(Data == eLampOn || Data1 == eLampOn)
		{
			//ABS_TRAILOR_ON;
			sIndicatorStatus.bAbsMalTrailerStatus = TRUE;
		}
		else
		{
			 //ABS_TRAILOR_OFF;
			 sIndicatorStatus.bAbsMalTrailerStatus = FALSE;
		}
		
	}
	else
		{
		 //ABS_TRAILOR_OFF;
		  sIndicatorStatus.bAbsMalTrailerStatus = FALSE;
		}
		

}

/*********************************************************************//**
*
*  Lift Axel Controller  
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vLiftAxelFaultLamp(void)
{

	u8 Data = 0;
	if(sIndicatorEnable.Enablebytes.Byte4.bLiftAxelControllerMil == eEnable)
	{
		if( bGetPGNTimeOut(ejLiftAxelFault) )
		{
		 	//LIFT_AXEL_FAULT_OFF; 
		}
		else
		{
	
			Data =	u32GetPGNdata(ejLiftAxelFault);
			if(Data == eLampOn )
			{
				//LIFT_AXEL_FAULT_ON; 
			}
			else
			{
				 //LIFT_AXEL_FAULT_OFF; 
			}
	
		}
	}

}






/*********************************************************************//**
*
* On/OFF Cruise Control indicator as per CAN input 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vCruiseControlLamp(void)
{

	u8 Data = 0;
	static u16 clkTime = 0;
	static u16 CanOutTime = 0;
	static bool StatusFlag = 0;
		

	
		if(bGetPGNTimeOut(eJCruiseControl)) 
		{		
			CRUISE_CONTROL_OFF;			
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJCruiseControl);

			switch (Data)	
			{
				case eLampOff :
					 CRUISE_CONTROL_OFF;
				break;
				case eLampOn :
					CRUISE_CONTROL_ON;
				break;
				case eError :
				case eDataNotAvailable :
				default :
					CRUISE_CONTROL_OFF 
				break;
			}
		}
	
	
}

/*********************************************************************//**
*
* On/OFF Hest  indicator as per CAN input 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vHestLamp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0;
	static u16 CanOutTime = 0;
	static bool StatusFlag = 0;

	if(sIndicatorEnable.Enablebytes.Byte4.bHest == eEnable)
	{	
		if(bGetPGNTimeOut(eJHEST)) 
		{
			
			if(bCanLostLampFlashFlag)
			{
				//HEST_ON
			}
			else						
			{
				//HEST_OFF
			}						
				
		}
		else
			{
				Data =	(eOBDLampdataType)u32GetPGNdata(eJHEST);

				switch (Data)	
				{
					case eLedOff :
						 //HEST_OFF;
					break;
					case eLedOn :
						//HEST_ON;
					break;				
					default :
						//HEST_OFF;	 
					break;
				}
			}
	}
	else
	{
		//HEST_OFF
	}

}

/*********************************************************************//**
*
* On/OFF coolent temperature indicator 
*
* @param      Lamp Status  
*
* @return     None
*************************************************************************/

static void _vCoolantTempLamp()
{
	u8 lampStaus = 0;
	
	lampStaus = bCoolantTempWrnStatus();

	if(lampStaus == eWarningOn)    
	{
		COOLANT_IN_TEMPRATURE_ON;//HIGH_TEMP_ON
		sBuzz_Control.bHTW = TRUE;
	}
	else if(lampStaus == eWarningOff)    
	{
		COOLANT_IN_TEMPRATURE_OFF;//HIGH_TEMP_OFF
		sBuzz_Control.bHTW = FALSE;
	}
	else if(lampStaus == eOpenCkt || lampStaus == eShortCkt)
	{
	   
			 if(bCanLostLampFlashFlag)
			 {
			   COOLANT_IN_TEMPRATURE_ON;//HIGH_TEMP_ON
			   sBuzz_Control.bHTW = FALSE;			   
			 }
			 else
			 	{
					 COOLANT_IN_TEMPRATURE_OFF;//HIGH_TEMP_OFF
					 sBuzz_Control.bHTW = FALSE;					
			 	}
		
	}	
	else
	{
		COOLANT_IN_TEMPRATURE_OFF;//HIGH_TEMP_OFF
		sBuzz_Control.bHTW = FALSE;
	}

	
	if(sBuzzerEnable.Enablebytes.Byte1.bHighTempWarning == eDisable)
		sBuzz_Control.bHTW = FALSE;


}
/*********************************************************************//**
*
* RPM economy inicators not implemented 
*
* @param      None
*
* @return     None
*************************************************************************/

static void _vRpmEconomy(void)
{
	u16 oldRPM = 0;
	static u16 timeStamp;
	
	
		if(u16GetClockTicks() - timeStamp < u16UpdateRate)
			return;
		timeStamp = u16GetClockTicks() ;

		oldRPM = u16GetEngineSpeed();

			if((oldRPM > sFeData.THRESH1 &&	oldRPM < sFeData.THRESH2)  ||
			(oldRPM > sFeData.THRESH3  && oldRPM < sFeData.THRESH4 ))
			{
				//AMBER
				if( sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableAmberLed == eEnable )
				{
					//LED_FUEL_ECO_YEL_ON;
				}
				else
				{
					//LED_FUEL_ECO_YEL_OFF;
				}
				
					//LED_FUEL_ECO_RED_OFF;
					//LED_FUEL_ECO_GRN_OFF;
			
			} 
			else if(oldRPM >= sFeData.THRESH2 && oldRPM <= sFeData.THRESH3)
			{
				 //green						
				if( sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableGreenLed == eEnable )
				{
				  	//LED_FUEL_ECO_GRN_ON;
				}
				else
				{
					//LED_FUEL_ECO_GRN_OFF;
				}
							 
				  //LED_FUEL_ECO_YEL_OFF;
				  //LED_FUEL_ECO_RED_OFF;
			 
			 
			}
			else if(oldRPM >=sFeData.THRESH4 ) 
			{
					//red
					if( sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableRedLed == eEnable)
					{
				  		//LED_FUEL_ECO_RED_ON;
					}
					else
					{
						//LED_FUEL_ECO_RED_OFF;
					}
					
					//LED_FUEL_ECO_GRN_OFF;
					//LED_FUEL_ECO_YEL_OFF;
			}
			else
			{
			//all off 
			 //LED_FUEL_ECO_YEL_OFF;
			 //LED_FUEL_ECO_GRN_OFF;
			 //LED_FUEL_ECO_RED_OFF;
			 
			}
	

}


/*********************************************************************//**
*
* ON/OFF low Air Pressure Warning indicators
*
* @param      Lamp Status  
*
* @return     None
*************************************************************************/

static void _vLowAirPressureLamp()
{
	u8 lampStaus = 0;
		
	if(sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eAirPressure && sIndicatorEnable.Enablebytes.Byte4.bLowAirPressure == eEnable && sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eEnable)
	{
		lampStaus = u8APGWarningStatus();

		if(lampStaus == eWarningOn)    
		{
			LOW_AIR_PRESSURE_ON;			
		}
		else if(lampStaus == eWarningOff)    
		{
			LOW_AIR_PRESSURE_OFF;			
		}		
		else
		{
			LOW_AIR_PRESSURE_OFF;			
		}
	}
	else
	{
		LOW_AIR_PRESSURE_OFF;		
	}

	if(sBuzzerEnable.Enablebytes.Byte1.bFrontAirPressure == eEnable)
	{
		  lampStaus = eFrontApgBuzzerStatus();
		  if(lampStaus == eAPGBuzzerActive)
		  {
		  	sBuzz_Control.bFRONT_AIR_PRESSUR = TRUE;
		  }
		  else
		  {
		  	sBuzz_Control.bFRONT_AIR_PRESSUR = FALSE;
		  }
	}

	if(sBuzzerEnable.Enablebytes.Byte1.bRearAirPressure == eEnable)
	{
		  lampStaus = eRearApgBuzzerStatus();
		  if(lampStaus == eAPGBuzzerActive)
		  {
		  	sBuzz_Control.bREAR_AIR_PRESSUR = TRUE;
		  }
		  else
		  {
		  	sBuzz_Control.bREAR_AIR_PRESSUR = FALSE;
		  }
	}

}


/*********************************************************************//**
*
* ON/OFF low fuel warning indicator 
*
* @param      Lamp Status  
*
* @return     None
*************************************************************************/

static void _vLowFuelLamp()
{
	u8 lampStaus = 0;
	
	lampStaus =  eFuelWarningStatus();

	if(lampStaus == eWarningOn)    
	{
		FUEL_GUAGE_ON;//LOW_FUEL_ON
		sBuzz_Control.bLFW = TRUE;
	}
	else if(lampStaus == eWarningOff)    
	{
		FUEL_GUAGE_OFF;//LOW_FUEL_OFF
		sBuzz_Control.bLFW = FALSE;
	}
	else if(lampStaus == eOpenCkt || lampStaus == eShortCkt)
	{
	    
			if(bCanLostLampFlashFlag)
			 {
			  FUEL_GUAGE_ON; //LOW_FUEL_ON
			   sBuzz_Control.bLFW = FALSE;			   
			 }
			 else
			 	{
					 FUEL_GUAGE_OFF;//LOW_FUEL_OFF
					 sBuzz_Control.bLFW = FALSE;
			 	}
		 	
      	
	}	
	else
	{
		FUEL_GUAGE_OFF;//LOW_FUEL_OFF
		sBuzz_Control.bLFW = FALSE;
	}

	if(sBuzzerEnable.Enablebytes.Byte1.bLowFuelWarning == eDisable)
		sBuzz_Control.bLFW = FALSE;

}
/*********************************************************************//**
*
* On/OFF Low oil indicator as per input 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vLowOilLamp(void)
{
		
	if(sLampInputsProp[eLOW_OIL_INPUT].u8StatusFlag == HIGH)
	{
		LOW_OIL_PRESSURE_ON
		sBuzz_Control.bLOW_OIL_PRESSURE = TRUE;
	}
	else
	{
		LOW_OIL_PRESSURE_OFF
		sBuzz_Control.bLOW_OIL_PRESSURE = FALSE;
	}
		
}
/*********************************************************************//**
*
* On/OFF Tild Cab lock indicator as per input 
*
* @param      None    
*
* @return     None
*************************************************************************/

void _vTiltCabLockLamp(void)
{

			
		if(sLampInputsProp[eCAB_TILT_INPUT].u8StatusFlag == HIGH)
		{
			TILT_CAB_LOCK_ON;
			sBuzz_Control.bTILT_CAB = TRUE;
			sIndicatorStatus.bCabTiltStatus = TRUE;
		}
		else
		{
			TILT_CAB_LOCK_OFF;
			sBuzz_Control.bTILT_CAB = FALSE;
			sIndicatorStatus.bCabTiltStatus = FALSE;
		}
	
	

}

/*********************************************************************//**
*
* ON/OFF over speedo warning indicator  
*
* @param      vehical speed   
*
* @return     None
*************************************************************************/

static void _vOverSpeedSpeedo()
{
	bool status = 0;

	status = bOverSpeedStatus();

	if(status) 
	{
		//OVER_SPEED_SPEEDO_ON
		sBuzz_Control.bGSO = TRUE;
	}
	else
	{
		//OVER_SPEED_SPEEDO_OFF
		sBuzz_Control.bGSO = FALSE;
	}

	if(sBuzzerEnable.Enablebytes.Byte2.bVehicleOverSpeed == eDisable)	
		sBuzz_Control.bGSO = FALSE;

}

/*********************************************************************//**
*
* On/OFF over engine warning indicator  
*
* @param      Engine Speed  
*
* @return     None
*************************************************************************/

static void _vOverSpeedRPM()
{
	bool status= 0;

    status = bOverEngineSpeed();

	if(status) 
	{
		//OVER_SPEED_RPM_ON
		sBuzz_Control.bEngineOverSpeed = TRUE;
	}
	else 
	{
		//OVER_SPEED_RPM_OFF
		sBuzz_Control.bEngineOverSpeed = FALSE;
	}

	if(sBuzzerEnable.Enablebytes.Byte3.bEngineOverSpeed == eDisable)
		sBuzz_Control.bEngineOverSpeed = FALSE;

}

/*********************************************************************//**
*
* On/OFF Clutch wear  indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/

void _vClutchOilLevelLamp(void)
{

			if(sLampInputsProp[eCLUTCH_OIL_LEVEL_INPUT].u8StatusFlag == HIGH)
			{
				CLUTCH_OIL_LEVEL_ON;				
			}
			else
			{
				CLUTCH_OIL_LEVEL_OFF;
				
			}
		


}

bool bClutchWearLampStatus()
{
 return bClutchWearStatus;
}

bool bBrakeWearLampStatus()
{
	bBrakeWearStatus = 0xff;
	return bBrakeWearStatus;
}





/*********************************************************************//**
*
* On/OFF seat belt  indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/

void _vSeatBeltLamp(void)
{
	static u16 timestamp =0;
	static bool bStatus = FALSE; //Flag make true when 30 sec time out for buzzer 
								//So that if variable rollover then buzzer not beep again while seat belt ON

	
		if(sLampInputsProp[eSEAT_BELT_INPUT].u8StatusFlag == HIGH && bStatus == FALSE)
		{
			SEAT_BELT_ON;
			sIndicatorStatus.bSeatBeltStatus = TRUE;
			if(u16GetClockTicks() - timestamp < SEAT_BELT_BUZZER_TIME)
			{
				 sBuzz_Control.bSEAT_BELT = TRUE;			
			}
			else
			{
				 sBuzz_Control.bSEAT_BELT = FALSE;
				 bStatus = TRUE;
			}
		}
		else if(sLampInputsProp[eSEAT_BELT_INPUT].u8StatusFlag == LOW)
		{
			SEAT_BELT_OFF;
			sIndicatorStatus.bSeatBeltStatus = FALSE;
			sBuzz_Control.bSEAT_BELT = FALSE;
			timestamp = u16GetClockTicks();
			if(bStatus == TRUE)
				bStatus = FALSE;
		}
	
	
}

/*********************************************************************//**
*
* get seat belt  indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetSeatBeltStatus(void)
{
	return (sIndicatorStatus.bSeatBeltStatus);
}

/*********************************************************************//**
*
* get Battery Charging 1  indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetBatChargingStatus(void)
{
	return (sIndicatorStatus.bBatCharging1Status);
}

/*********************************************************************//**
*
* get Lift Axel Up  indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetLiftAxelStatus(void)
{
	return (sIndicatorStatus.bLiftAxelUpStatus);
}

/*********************************************************************//**
*
* get General Illumination status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetGenIllStatus(void)
{

	sIndicatorStatus.bGenIllStatus = bGetIlluminationInputStatus();

	return (sIndicatorStatus.bGenIllStatus);
}

/*********************************************************************//**
*
* get Battery Charging 2 indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetBatCharging2Status(void)
{
	return (sIndicatorStatus.bBatCharging2status);
}

/*********************************************************************//**
*
* get Tilt Cab indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetTitlCabStatus(void)
{
	return (sIndicatorStatus.bCabTiltStatus);
}

/*********************************************************************//**
*
* get Air Filter Clogged indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetAirFilterCloggedStatus(void)
{
	return (sIndicatorStatus.bAirFilterCloggedStatus);
}

/*********************************************************************//**
*
* get Abs Malfunction  indicator status
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetAbsMilTrailerStatus(void)
{
	return (sIndicatorStatus.bAbsMalTrailerStatus);
}

/*********************************************************************//**
*
* Get stop engine lamp status for text warning display 
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetStopEngineStatus(void)
{
	return (bStopEnginelampStatus);
}


/*********************************************************************//**
*
* Get Check engine lamp status for text warning display 
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetCheckEngineStatus(void)
{
	return (bCheckEnginelampStatus);
}

/*********************************************************************//**
*
* Get Mil lamp Mode 3 status for text warning display 
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetMilLampMode3Status(void)
{
	return (bMilMode3Status);
}

/*********************************************************************//**
*
* Get Mil lamp Mode 4 status for text warning display 
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetMilLampMode4Status(void)
{
	return (bMilMode4Status);
}

/*********************************************************************//**
*
* Get SCR Lamp status for text warning display 
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetScrLampStatus(void)
{
	return (bScrlampStatus);
}


/*********************************************************************//**
*
* Get PTO lamp status used for speedo working when "Speedo required with PTO" clear in EOL
*
* @param      None
*
* @return     bool
*************************************************************************/

bool bGetPtoLampStatus(void)
{
	return (bPtoStatus);
}




/*********************************************************************//**
*
* On/OFF Left turn indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/


void _vHazard()
{
u8 Data = 0;

	if(sIndicatorInputType.Inputsbytes.Byte1.bTurnLeft == eAnalouge && sIndicatorInputType.Inputsbytes.Byte2.bTurnRight == eAnalouge)
	{
		if(sLampInputsProp[eTURN_LEFT_INPUT].u8StatusFlag == LOW && sLampInputsProp[eTURN_RIGHT_INPUT].u8StatusFlag == LOW)
		{
			TURN_LEFT_ON;
			TURN_RIGHT_ON;
			sBuzz_Control.bTURN_LEFT = TRUE;
		}
		else
		{
			TURN_LEFT_OFF;
			TURN_RIGHT_OFF;
			sBuzz_Control.bTURN_LEFT = FALSE;
		}
	
	}
	else if(sIndicatorInputType.Inputsbytes.Byte1.bTurnLeft == eCan && sIndicatorInputType.Inputsbytes.Byte2.bTurnRight == eCan)
	{

		//Check CAN message lost
			if(bGetPGNTimeOut(eJTurnLeftRightTurn)) 
			{
				TURN_LEFT_OFF;
				TURN_RIGHT_OFF;
				sBuzz_Control.bTURN_LEFT = FALSE;
			}
			else
			{
				Data =	 u32GetPGNdata(eJTurnLeftRightTurn);
				if(Data == eHazard)
				{
					if(bFlasher1_3hzTimeFlashFlag)
					{
						TURN_LEFT_ON;
						TURN_RIGHT_ON;
						sBuzz_Control.bTURN_LEFT = TRUE;
					}
					else
					{
						TURN_LEFT_OFF;
						TURN_RIGHT_OFF;
						sBuzz_Control.bTURN_LEFT = FALSE;	
					}
				}
				else
				{
						TURN_LEFT_OFF;
						TURN_RIGHT_OFF;
						sBuzz_Control.bTURN_LEFT = FALSE;
				}
			}

	}


}



/*********************************************************************//**
*
* On/OFF Left turn indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/ 
static void _vTurnLeftLamp(void)
{

	u8 Data =0;	
	
	
		if(sIndicatorInputType.Inputsbytes.Byte1.bTurnLeft == eAnalouge)
		{
			if(sLampInputsProp[eTURN_LEFT_INPUT].u8StatusFlag == LOW)
			{
				TURN_LEFT_ON;
				sBuzz_Control.bTURN_LEFT = TRUE;
			}
			else
			{
				TURN_LEFT_OFF;
				sBuzz_Control.bTURN_LEFT = FALSE;
			}
		
		}
		else
		{
			//Check CAN message lost
			if(bGetPGNTimeOut(eJTurnLeftRightTurn)) 
			{
				TURN_LEFT_OFF;
				sBuzz_Control.bTURN_LEFT = FALSE;
			}
			else
			{
				Data =	 u32GetPGNdata(eJTurnLeftRightTurn);				

				switch(Data)
					{
						
						case eTurnLeft:
							if(bFlasher1_3hzTimeFlashFlag)
							{
							    TURN_LEFT_ON;
								sBuzz_Control.bTURN_LEFT = TRUE;
							}
							else
							{
								TURN_LEFT_OFF;
								sBuzz_Control.bTURN_LEFT = FALSE;	
							}
								
						break;
						case eTurnLeftDoubleFlag:
							if(bFlasher2_5hzTimeFlashFlag)
							{
							    TURN_LEFT_ON;
								sBuzz_Control.bTURN_LEFT = TRUE;
							}
							else
							{
								TURN_LEFT_OFF;
								sBuzz_Control.bTURN_LEFT = FALSE;	
							}
							
						break;
						
						case eHazard:
							if(bFlasher1_3hzTimeFlashFlag)
							{
							    TURN_LEFT_ON;
								sBuzz_Control.bTURN_LEFT = TRUE;
							}
							else
							{
								TURN_LEFT_OFF;
								sBuzz_Control.bTURN_LEFT = FALSE;	
							}
						break;
						default:
								TURN_LEFT_OFF;
								sBuzz_Control.bTURN_LEFT = FALSE;					
						break;
						
					}

											
			}//End else
		}
	

	if(sBuzzerEnable.Enablebytes.Byte3.bTurnLeft == eDisable)
		sBuzz_Control.bTURN_LEFT = FALSE;

}


/*********************************************************************//**
*
* On/OFF  Right turn indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/ 
static void _vTurnRightLamp(void)
{

	u8 Data =0;
	
		if(sIndicatorInputType.Inputsbytes.Byte2.bTurnRight == eAnalouge)
		{
			if(sLampInputsProp[eTURN_RIGHT_INPUT].u8StatusFlag == LOW)
			{
				TURN_RIGHT_ON;
				sBuzz_Control.bTURN_RIGHT = TRUE;
			}
			else
			{
				TURN_RIGHT_OFF;
				sBuzz_Control.bTURN_RIGHT = FALSE;
			}
		
		}
		else
		{
			//Check CAN message lost
			if(bGetPGNTimeOut(eJTurnLeftRightTurn)) 
			{
				TURN_RIGHT_OFF;
				sBuzz_Control.bTURN_RIGHT = FALSE;				 
			}
			else
			{
				Data =	  u32GetPGNdata(eJTurnLeftRightTurn);
				
				switch(Data)
				{
					
					case eTurnRight:
						if(bFlasher1_3hzTimeFlashFlag)
						{
							TURN_RIGHT_ON;
							sBuzz_Control.bTURN_RIGHT = TRUE;	
						}
						else
						{
							TURN_RIGHT_OFF;
							sBuzz_Control.bTURN_RIGHT = FALSE;						
						}					
					break;
					
					case eTurnRightDoubleFlag:
						if(bFlasher2_5hzTimeFlashFlag)
						{
							TURN_RIGHT_ON;
							sBuzz_Control.bTURN_RIGHT = TRUE;	
						}
						else
						{
							TURN_RIGHT_OFF;
							sBuzz_Control.bTURN_RIGHT = FALSE;						
						}
					break;
					case eHazard:
						if(bFlasher1_3hzTimeFlashFlag)
						{
							TURN_RIGHT_ON;
							sBuzz_Control.bTURN_RIGHT = TRUE;	
						}
						else
						{
							TURN_RIGHT_OFF;
							sBuzz_Control.bTURN_RIGHT = FALSE;						
						}	
					break;
					default:
							TURN_RIGHT_OFF;
							sBuzz_Control.bTURN_RIGHT = FALSE;					
					break;
					
				}
									
			}//End else
		}

		if(sBuzzerEnable.Enablebytes.Byte2.bTurnRight == eDisable)
			sBuzz_Control.bTURN_RIGHT = FALSE;
		
			
}

/*********************************************************************//**
*
* On/OFF  indicator as per input 
*
* @param      None
*
* @return     None
*************************************************************************/
void _vExhaustBrakeLamp(void)
{

	if(sLampInputsProp[eEXHAUST_INPUT].u8StatusFlag == LOW)
	{
		
		EXHAUST_BREAK_ON
	}
	else
	{
		
		EXHAUST_BREAK_OFF
	}
}


//Lamp to be display On LCD

/*********************************************************************//**
*
* Check ESC Mil indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/
bool bEscMilLamp(void)
{
	bool status = FALSE;
	
	if(sLampInputsProp[eESC_MIL_INPUT].u8StatusFlag == HIGH)
	{
		status = TRUE;  //ON
	}
	else
	{
		status = FALSE;//OFF
	}	
				
			

	return(status);

}




static void _vEngineCoolantLevel(void)
{
	static	bool status = 0;

		
		if(sLampInputsProp[eENGINE_COOLANT_LEVEL_INPUT].u8StatusFlag == HIGH)
		{
			ENGINE_COOLANT_LEVEL_ON;
			
		}else
		{
			ENGINE_COOLANT_LEVEL_OFF;
			
		}	
		
	
}


/*********************************************************************//**
*
* Check Break wear  indicator input 
*
* @param      None 
*
* @return     indicator status
*************************************************************************/
bool bBrakeWearLamp(void)
{
	static	bool status = 0;

		/*
		if(sLampInputsProp[eBREAK_WEAR_INPUT].u8StatusFlag == HIGH)
		{
			status = TRUE;  //ON
			sBuzz_Control.bBRAKE_BEAR = TRUE;
			
		}else
		{
			status = FALSE;//OFF
			sBuzz_Control.bBRAKE_BEAR = FALSE;
			
		}	
		
		*/
	return(status);
	
}


static void _vDoorOpenLamp(void)
{
	
	if(sLampInputsProp[eDOOR_LOW_INPUT].u8StatusFlag == HIGH)
	{
		DOOR_OPEN_ON;
	}
	else
	{
		DOOR_OPEN_OFF;
	}	
	
}



/*********************************************************************//**
*
* Check Retarder indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/
bool bRetarderLamp(void)
{
	
	
	static bool StatusFlag = 0;
	
			
			if(sLampInputsProp[eRETARDER_HIGH_INPUT].u8StatusFlag == LOW)
			{
				StatusFlag = TRUE;  //ON
			}
			else
			{
				StatusFlag = FALSE;//OFF
			}
		
	
	
	return(StatusFlag);
}
/*********************************************************************//**
*
* Check Air filter clogged indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/
static void _vAirFilterCloggedLamp(void)
{
	
	
		
				if(sLampInputsProp[eAIR_FILTERCLOGGED_INPUT].u8StatusFlag == HIGH)
				{					
				  LIM_NOX_ON;
				}
				else
				{					
					LIM_NOX_OFF;
				}
			
	
}



/*********************************************************************//**
*
* Check water in fuel indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/
bool bWaterInFuelLamp(void)
{
	
	u8 Data = 0;
	static bool status = 0;
	
			if(bGetPGNTimeOut(eJWaterInFuelLamp))  
			{
				status = FALSE;
			}
			else
			{
				Data =	(eOBDLampdataType)u32GetPGNdata(eJWaterInFuelLamp);

				switch (Data)
				{
					case eLampOff :
						status = FALSE;
					break;
					case eLampOn :
						status = TRUE;
					break;
					case eError :				
					case eDataNotAvailable :	
					default :
					status = FALSE;
					break;
				}
			}
		
	return(status);

}


/*********************************************************************//**
*
* Exhaust break input is pending  
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/

bool bExhaustBreakLamp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0; 	
	static bool status = 0;

	if(sTestEquimentControl.tellTalesFlag == TRUE)
		{		
			if( ((InputOutputLampControl.IoControlLampbytes.Byte5.bExhaustBrakeLamp == TRUE )
				|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
				&& (sIndicatorEnable.Enablebytes.Byte3.bExhaustBrake == eEnable) )
			{
				status = TRUE;	
			}
			else
			{
				status = FALSE; 
			}
		}
		else
		{

			if(sIndicatorEnable.Enablebytes.Byte3.bExhaustBrake == eEnable)
			{
				if(bGetPGNTimeOut(eJExhaustBreak))  
				{
					if(bCanLostLampFlashFlag)
					{
						status = TRUE;					 
					}
					else
					{
						status = FALSE;					
					}
				}
				else
				{
					Data =	(eOBDLampdataType)u32GetPGNdata(eJExhaustBreak);

					switch (Data)
					{
						case eLampOff :
							status = FALSE;
						break;
						case eLampOn :
							status = TRUE;
						break;
						case eError :		
						case eDataNotAvailable :	
						default :
							status = FALSE;
						break;
					}
				}
			}
			else
			{
				status = FALSE;
			}
		}
	
return(status);

}


/*********************************************************************//**
*
* Check Engine pre heat indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/

bool bEnginePreHeatLamp(void)
{
	
	u8 Data = 0;
	static bool status = 0;

	
			if(bGetPGNTimeOut(eJEnginePreHeat))  
			{
				status = FALSE;
			}
			else
			{
				Data =	(eOBDLampdataType)u32GetPGNdata(eJEnginePreHeat);

				switch (Data)
				{
					case eLampOff :
						status = FALSE;
					break;
					case eLampOn :
						status = TRUE;
					break;
					case eError :			
					case eDataNotAvailable :			       
					
					default :	
					status = FALSE;
					break;
				}
			}
		
	return(status);


}


/*********************************************************************//**
*
* Check Def level indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/

bool bDefLevelLamp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0; 
	static bool status = 0;

		if(bGetPGNTimeOut(eJDefLamp))  
		{
			 sBuzz_Control.bLOW_DEF = FALSE;

			if(bCanLostLampFlashFlag)
			{
				status = TRUE;					 
			}
			else
			{
				status = FALSE;					
			}
			
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJDefLamp);

			switch (Data)
			{
				case eLedOff :
					status = FALSE;
					sBuzz_Control.bLOW_DEF = FALSE;
				break;
				case eLedOn :
					status = TRUE;
					sBuzz_Control.bLOW_DEF = TRUE;
				break;
				case eLampFastBlink :
					if(u16GetClockTicks() - clkTime  > INDICATOR_FLASHING_TIME)
					{
						clkTime = u16GetClockTicks();
						sBuzz_Control.bLOW_DEF = FALSE;
						if(status == TRUE)
						{
							status = FALSE;							
						}
						 else
					 	{
							status = TRUE;							
					 	}
					}			
					break;		
				default :	
					status = FALSE;
					sBuzz_Control.bLOW_DEF = FALSE;
				break;
			}
		}
	

	return(status);


}

/*********************************************************************//**
*
* Check SCR indicator input  
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/

bool bScrLamp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0; 
	static bool status = 0;

	if(sTestEquimentControl.tellTalesFlag == TRUE)
	{	
		if(((InputOutputLampControl.IoControlLampbytes.Byte5.bScrFaultLamp == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte4.bScr == eEnable) )
		{
			status = TRUE;
		}
		else
		{
			status = FALSE;
		}
	}
	else
	{
			if(bGetPGNTimeOut(eJSCR_SPN5825))  //SPN 5825
			{	
			   bScrlampStatus = FALSE;
			   if(sIndicatorEnable.Enablebytes.Byte4.bScr == eEnable)
			   {
					if(bCanLostLampFlashFlag)
					{
						status = TRUE;
					}
					else
					{
						status = FALSE;
					}
			   	}
			}
			else
			{
				Data =	(eOBDLampdataType3bit)u32GetPGNdata(eJSCR_SPN5825);
				//Used for text display 
                
				if(Data == eLedOn || Data == eLampFastBlink )
				{
				   bScrlampStatus = TRUE;
				}
				else
				{
				   bScrlampStatus = FALSE;
				}
				
				//For SCR Lamp
				if(sIndicatorEnable.Enablebytes.Byte4.bScr == eEnable)
			 	{
			 		switch (Data)
					{
						case eLedOff :
							status = FALSE;							
						break;
						case eLedOn :
							status = TRUE;	
							break;	
						case eError2 :
						case eError3 :
							status = FALSE;
							break;
						case eLampFastBlink:
							if(u16GetClockTicks() - clkTime  > INDICATOR_FLASHING_TIME)
							{
								clkTime = u16GetClockTicks();
								
								if(status == TRUE)
								{
									status = FALSE;						
								}
								 else
							 	{
									status = TRUE;						
							 	}
							}			
							
							break;
						case eError5 :
						case eError6 :								
						default :	
							status = FALSE;							
						break;
					}
			 	
			 	}
			}
		
	}

	return(status);

}



/*********************************************************************//**
*
* Check DPF indicator input 
*
* @param      None 
*
* @return     Indicator status
*************************************************************************/
bool bDpfLamp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0;
	static bool status = 0;

	if(sTestEquimentControl.tellTalesFlag == TRUE)
	{	
		if(( (InputOutputLampControl.IoControlLampbytes.Byte5.bDpfilterLamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bDpf == eEnable) )
		{
			status = TRUE;
		}
		else
		{
			status = FALSE;
		}
	}
	else
	{	
		
		if(sIndicatorEnable.Enablebytes.Byte3.bDpf == eEnable)
		{
			if(bGetPGNTimeOut(eJDPF))  
			{
				if(bCanLostLampFlashFlag)
				{
					status = TRUE;
				}
				else
				{
					status = FALSE;
				}
			}
			else
			{
				Data =	(eOBDLampdataType3bit)u32GetPGNdata(eJDPF);

				switch (Data)
				{
					case eLedOff :
						status = FALSE;
						
					break;
					case eLedOn :
						status = TRUE;
						
					break;
					case eLampFastBlink :
						if(u16GetClockTicks() - clkTime  > INDICATOR_FLASHING_TIME)
						{
							clkTime = u16GetClockTicks();
							
							if(status == TRUE)
							{
								status = FALSE;						
							}
							 else
						 	{
								status = TRUE;						
						 	}
						}			
						break;		
					default :	
						status = FALSE;					
					break;
				}
			}
		}
		else
		{
			status = FALSE;
		}
	}

	return(status);



}


/*********************************************************************//**
*
* On/OFF Low Brake Fluid indicator as per input
*
* @param      None    
*
* @return     None
*************************************************************************/
void _vBrakeLiquidLevel(void)
{	

				if(sLampInputsProp[eBRAKE_LIQUID_LEVEL_INPUT].u8StatusFlag == HIGH)
				{
					LOW_BRAKE_FLUID_ON;					
					sBuzz_Control.bLowBrakeFluid = TRUE;					
				}
				else
				{
					LOW_BRAKE_FLUID_OFF;
					sBuzz_Control.bLowBrakeFluid = FALSE;
				}				
			

}



/*********************************************************************//**
*
* ON/OFF  Battery Charging  indicator as per input 
*
* @param      None 
*
* @return     None
*************************************************************************/

void _vBattCharging1Lamp(void)
{
	u8 Data = 0;
	
		if(sIndicatorInputType.Inputsbytes.Byte4.bBatteryCharging1 == eAnalouge)
		{
			if( sLampInputsProp[eBATT_CHARGING1_INPUT].u8StatusFlag == HIGH )
			{
				BATTERY_CHARGING1_ON;
				sIndicatorStatus.bBatCharging1Status = TRUE;
			}
			else
			{
				BATTERY_CHARGING1_OFF;
				sIndicatorStatus.bBatCharging1Status = FALSE;
			}	
		}
		else
		{
			if(bGetPGNTimeOut(eJBatteryCharging1))  
			{					
				BATTERY_CHARGING1_OFF;
				sIndicatorStatus.bBatCharging1Status = FALSE;					
			}
			else
			{
				Data =	(eOBDLampdataType3bit)u32GetPGNdata(eJBatteryCharging1);

				if( Data == eLampOn )
				{
					BATTERY_CHARGING1_ON;
					sIndicatorStatus.bBatCharging1Status = TRUE;

				}
				else
				{
					BATTERY_CHARGING1_OFF;
					sIndicatorStatus.bBatCharging1Status = FALSE;

				}	
			}		
		
		}	
		
}


/*********************************************************************//**
*
* ON/OFF  Battery Charging2  indicator as per Can input 
*
* @param      None 
*
* @return     None
*************************************************************************/

void _vBattCharging2Lamp(void)
{
	eOBDLampdataType Data = 0u;
	eOBDLampdataType Data1 = 0u;

	if(sIndicatorEnable.Enablebytes.Byte4.bBatteryCharging2 == eEnable)
	{

		if( !bGetPGNTimeOut(eJBatteryCharging2) )
		{
		  	Data =	(eOBDLampdataType)u32GetPGNdata(eJBatteryCharging2);
		}
		else
		{

		}
	    if( !bGetPGNTimeOut(eJBatteryWeak2) )
		{
			Data1 =	(eOBDLampdataType)u32GetPGNdata(eJBatteryWeak2);
		}
		else
		{

		}
		
		if((Data == eLampOn) || (Data1 == eLampOn))
		{
			//BATT_CHARG2_ON;
			sIndicatorStatus.bBatCharging2status = TRUE;
		}
		else
		{
			//BATT_CHARG2_OFF;
			sIndicatorStatus.bBatCharging2status = FALSE;
		}	
		
	}
	else
	{
			//BATT_CHARG2_OFF;
			sIndicatorStatus.bBatCharging2status = FALSE;
	}
}


/*********************************************************************//**
 *
 * Buzzer enable and dissable 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vBuzzerEnableDisable(void)
{	

	eOBDLampdataType Data = 0u;
		if(bGetPGNTimeOut(eJBuzzerDisable))  
		{					
							
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJBuzzerDisable);
			
			if(Data == eLampOn)
			{
				vBuzzerEnable();
			}
			else
			{
				vBuzzerDisable();			
			}
		}



}


/*********************************************************************//**
*
* OFF  Economy Leds
*
* @param      None 
*
* @return     None
*************************************************************************/
void vEconomyLedOFF(void)
{
	
	 //LED_FUEL_ECO_YEL_OFF;
	 //LED_FUEL_ECO_GRN_OFF;
	 //LED_FUEL_ECO_RED_OFF;

	vLedDataSendToSpi();
}


/*********************************************************************//**
*
* OFF All indicators 
*
* @param      None 
*
* @return     None
*************************************************************************/
void vAllLampsOff(void)
{
	u8 i;
	
	


	HIGH_BEAM_OFF;
	TURN_LEFT_OFF;
	TURN_RIGHT_OFF;


	
	vSetLedDriverBlankLow();   //Keep low BLANK pin of LED driver so that all indicator off		
	for(i = 0; i < GS_DATA_SIZE; i++)
	 u8GSdata[i] = 0x00;

	vLedDataSendToSpi();
}


/*********************************************************************//**
*
* ON All indicators 
*
* @param      None 
*
* @return     None
*************************************************************************/
void vAllLampsOn(void)
{
	u8 i;


	HIGH_BEAM_ON;
	TURN_LEFT_ON;
	TURN_RIGHT_ON;

	
	for(i = 0; i < GS_DATA_SIZE; i++)
	 u8GSdata[i] = 0xFF;

	vLedDataSendToSpi();
}

/*********************************************************************//**
*
* Off disabled Indicators
*
* @param      None 
*
* @return     None
*************************************************************************/

void vCheckLamps(void)
{
	u8 i;

	HIGH_BEAM_ON;
	TURN_LEFT_ON;
	TURN_RIGHT_ON;

	
	for(i = 0; i < GS_DATA_SIZE; i++)
	 u8GSdata[i] = 0xFF;

	vLedDataSendToSpi();

}

/*********************************************************************//**
*
* Control lamps by input output control service ,control only Led lamps  
*
* @param      None    
*
* @return     None
*************************************************************************/

void vIoControlTellTaleLedBased(void)
{

	if(sTestEquimentControl.tellTalesFlag == FALSE)
	{	return;	}
	else	{		}
	
	//Stop All buzzer 
	sBuzz_Control.u8BuzzerSound = BUZZER_STOP;

	
		if( ((InputOutputLampControl.IoControlLampbytes.Byte1.bTransRangeLow == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte1.bTransRangeLow == eEnable))
		{
			//TRANSMISSION_RANG_LOW_ON;	
		}
		else
		{
			//TRANSMISSION_RANG_LOW_OFF;		
		}
	
		if(((InputOutputLampControl.IoControlLampbytes.Byte3.bNeutral == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte2.bNeutral == eEnable))
		{
			//NEUTRAL_ON;
		}
		else
		{
			//NEUTRAL_OFF;	
		}
		
			if(((InputOutputLampControl.IoControlLampbytes.Byte4.bPowerTakeOff == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte2.bPowerTakeoff == eEnable))
		{
			POWER_TAKE_OFF_ON;
		}
		else
		{
			POWER_TAKE_OFF_OFF;	
		}	
		
			if(((InputOutputLampControl.IoControlLampbytes.Byte4.bGearShiftUp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bGsa == eEnable))
		{
			//GEAR_UP_SHIFT_ON;
		}
		else
		{
			//GEAR_UP_SHIFT_OFF;
		}

		
		if(((InputOutputLampControl.IoControlLampbytes.Byte5.bGearShiftDownLamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bGsa == eEnable))
		{
			//GEAR_DOWN_SHIFT_ON;
		}
		else
		{
			//GEAR_DOWN_SHIFT_OFF;
		}
		//TPMS will add when TML will advice 
		/*  
		if( (InputOutputLampControl.IoControlLampbytes.Byte6.bTpmsLamp == TRUE )
			 || (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
		{
			TPMS_LAMP_ON;
		}
		else
		{
			TPMS_LAMP_OFF;	
		}
		*/
		if(((InputOutputLampControl.IoControlLampbytes.Byte4.bTranceFluid == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bTransFluidTemp == eEnable))
		{
			//TRANSFLUID_TEMP_ON;
		}
		else
		{
			//TRANSFLUID_TEMP_OFF;
		}
		
		
		if( (InputOutputLampControl.IoControlLampbytes.Byte1.bTurnLeft == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			TURN_LEFT_ON;
		}
		else
		{
			TURN_LEFT_OFF;
		}

	

		if(((InputOutputLampControl.IoControlLampbytes.Byte1.bTrailer == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte1.bTrailor == eEnable))
		{
			//TRAILOR_ON;	
		}
		else
		{
			//TRAILOR_OFF;	
		}
		

		if( (InputOutputLampControl.IoControlLampbytes.Byte1.bHighBeam == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			HIGH_BEAM_ON;	
		}
		else
		{
			HIGH_BEAM_OFF;	
		}
		

		if( ((InputOutputLampControl.IoControlLampbytes.Byte1.bSeatBelt == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bSeatBelt == eEnable))
		{
			SEAT_BELT_ON;	
		}
		else
		{
			SEAT_BELT_OFF;	
		}
		

		if( (InputOutputLampControl.IoControlLampbytes.Byte1.bCoolantTempWarning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			//HIGH_TEMP_ON;	
		}
		else
		{
			//HIGH_TEMP_OFF;
		}
		
		
		if(((InputOutputLampControl.IoControlLampbytes.Byte2.bClutchWear == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte4.bClutchWear== eEnable))
		{
			//CLUCH_WEAR_ON;
		}
		else
		{
			//CLUCH_WEAR_OFF;
		}
		
		
		if( (InputOutputLampControl.IoControlLampbytes.Byte2.bVehicleOverSpeedoWarning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
		{
			//OVER_SPEED_SPEEDO_ON;
		}
		else
		{
			//OVER_SPEED_SPEEDO_OFF;
		}
		
		
		if( ( (InputOutputLampControl.IoControlLampbytes.Byte2.bAirPressureWarning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& ( (sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eAirPressure )
			&&   (sIndicatorEnable.Enablebytes.Byte4.bLowAirPressure == eEnable )
			&&   (sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eEnable ) )  )
		{
			LOW_AIR_PRESSURE_ON;	
		}
		else
		{
			LOW_AIR_PRESSURE_OFF;	
		}
		
		
		if( (InputOutputLampControl.IoControlLampbytes.Byte2.bFuelGaugeWaning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			//LOW_FUEL_ON;	
		}
		else
		{
			//LOW_FUEL_OFF;	
		}
		
		
		if( (InputOutputLampControl.IoControlLampbytes.Byte2.bBatterCharging1 == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ) )
		{
			BATTERY_CHARGING1_ON;	
		}
		else
		{
			BATTERY_CHARGING1_OFF;	
		}
		
		
		if(((InputOutputLampControl.IoControlLampbytes.Byte2.bLowBrakeFluid == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bLowBreakFluid == eEnable))
		{
			//LOW_BRAKE_FLUID_ON;
		}
		else
		{
			//LOW_BRAKE_FLUID_OFF;	
		}
		
		
		if( (InputOutputLampControl.IoControlLampbytes.Byte2.bMil == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			//MIL_ON;	
		}
		else
		{
			//MIL_OFF;
		}
		
		if(((InputOutputLampControl.IoControlLampbytes.Byte3.bTiltCabLock == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte1.bTiltCab == eEnable))
		{
			TILT_CAB_LOCK_ON;	
		}
		else
		{
			TILT_CAB_LOCK_OFF;	
		}
		

		if( (InputOutputLampControl.IoControlLampbytes.Byte3.bParkBrake == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			PARKING_BRAKE_ON;	
		}
		else
		{
			PARKING_BRAKE_OFF;	
		}
		

		if( (InputOutputLampControl.IoControlLampbytes.Byte3.bTurnRight == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			TURN_RIGHT_ON;
		}
		else
		{
			TURN_RIGHT_OFF;
		}
		

		if( (InputOutputLampControl.IoControlLampbytes.Byte3.bOilPressureWarning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			LOW_OIL_PRESSURE_ON;
		}
		else
		{
			LOW_OIL_PRESSURE_OFF;	
		}
		


		

		if( (InputOutputLampControl.IoControlLampbytes.Byte3.bCheckEngine == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
		{
			//CHECK_ENGINE_ON;
		}
		else
		{
			//CHECK_ENGINE_OFF;
		}

		if(((InputOutputLampControl.IoControlLampbytes.Byte4.bFrontRearFogLamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte2.bFrontRearFog == eEnable))
		{
			FRONT_REAR_FOG_ON;
		}
		else
		{
			FRONT_REAR_FOG_OFF;	
		}
		

		
		
		if(((InputOutputLampControl.IoControlLampbytes.Byte4.bAbsMilTrailer == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte2.bAbsMilTrailer == eEnable))
		{
			//ABS_TRAILOR_ON;
		}
		else
		{
			//ABS_TRAILOR_OFF;
		}
		

		if( ( (InputOutputLampControl.IoControlLampbytes.Byte5.bStopEngineLamp == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bStopEngine == eEnable))
		{
			//STOP_ENGINE_ON;
		}
		else
		{
			//STOP_ENGINE_OFF;
		}

		if( ((InputOutputLampControl.IoControlLampbytes.Byte5.bCruiseControlLamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE ))
			&& (sIndicatorEnable.Enablebytes.Byte3.bCruiseControl == eEnable))
		{
			CRUISE_CONTROL_ON;
		}
		else
		{
			CRUISE_CONTROL_OFF;	
		}

		if(( (InputOutputLampControl.IoControlLampbytes.Byte5.bHestLamp == TRUE) 
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& (sIndicatorEnable.Enablebytes.Byte4.bHest== eEnable))
		{
			//HEST_ON;
		}
		else
		{
			//HEST_OFF;
		}

			
	
		if( ((InputOutputLampControl.IoControlLampbytes.Byte6.bLiftControllerAxleLamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& (sIndicatorEnable.Enablebytes.Byte4.bLiftAxelControllerMil == eEnable))
		{
			//LIFT_AXEL_FAULT_ON;
		}
		else
		{
			//LIFT_AXEL_FAULT_OFF;
		}	

		if( ((InputOutputLampControl.IoControlLampbytes.Byte6.bBatteryCharging2Lamp == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE) )
			&& (sIndicatorEnable.Enablebytes.Byte4.bBatteryCharging2== eEnable))
		{
			//BATT_CHARG2_ON;
		}
		else
		{
			//BATT_CHARG2_OFF;
		}	

		if( (InputOutputLampControl.IoControlLampbytes.Byte6.bEngineOverSpeedWarning == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
		{
			//OVER_SPEED_RPM_ON;
		}
		else
		{
			//OVER_SPEED_RPM_OFF;
		}
		
		if(((InputOutputLampControl.IoControlLampbytes.Byte6.bEconomyLampAmber == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& (sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableAmberLed == eEnable))
		{
			//LED_FUEL_ECO_YEL_ON;
		}
		else
		{
			//LED_FUEL_ECO_YEL_OFF;
		}
		
		if((( InputOutputLampControl.IoControlLampbytes.Byte6.bEconomyLampGreen == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& (sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableGreenLed == eEnable))
		{
			//LED_FUEL_ECO_GRN_ON;
		}
		else
		{
			//LED_FUEL_ECO_GRN_OFF;
		}
		
		if( ((InputOutputLampControl.IoControlLampbytes.Byte6.bEconomyLampRed == TRUE )
			|| (InputOutputLampControl.IoControlLampbytes.Byte6.bAllTellON == TRUE))
			&& (sIndicatorEnable.Enablebytes.Byte1.bFuelEcoEnableRedLed == eEnable))
		{
			//LED_FUEL_ECO_RED_ON;
		}
		else
		{
			//LED_FUEL_ECO_RED_OFF;
		}

			
	vLedDataSendToSpi();
	
}


/*********************************************************************//**
*
* Set buzzer On/OFF status bit wise in variable 
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vCheckBuzzerStatus()
{

	//Check Life Critical Alarms Low Air Pressure,Tilt Cab,Seat belt,GSO
	if(sBuzz_Control.bFRONT_AIR_PRESSUR == TRUE || sBuzz_Control.bREAR_AIR_PRESSUR == TRUE 
		|| sBuzz_Control.bTILT_CAB == TRUE || sBuzz_Control.bSEAT_BELT == TRUE
		||sBuzz_Control.bGSO == TRUE || sBuzz_Control.bLowBrakeFluid == TRUE )
	{ 
		sBuzz_Control.u8BuzzerSound |= LIFE_CRITICAL_ALARM;   
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~LIFE_CRITICAL_ALARM;
	}

	//Check EngineCritical Alarms HTW,LFW,Low Def,Low Oil Pressure,Stop Engine,TransFluid  

	if(sBuzz_Control.bHTW == TRUE || sBuzz_Control.bLFW == TRUE || sBuzz_Control.bLOW_DEF == TRUE 
		|| sBuzz_Control.bSTOP_ENGINE == TRUE || sBuzz_Control.bTRANCE_FLUDE == TRUE  
		|| sBuzz_Control.bLOW_OIL_PRESSURE == TRUE || sBuzz_Control.bEngineOverSpeed == TRUE
		)
	{ 
		sBuzz_Control.u8BuzzerSound |= ENGINE_CRITICAL_ALARM;   
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~ENGINE_CRITICAL_ALARM;
	}


	//Check  Genral Alarms,Brake Wear,Park Wear,
	if( sBuzz_Control.bBRAKE_BEAR == TRUE 
		|| sBuzz_Control.bPARK_BREAK == TRUE  )
	{ 
		sBuzz_Control.u8BuzzerSound |= GENRAL_ALARM;   
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~GENRAL_ALARM;
	}

//Beep Alarm for GPO ,Up
	if(sBuzz_Control.bGEAR_UP == TRUE || sBuzz_Control.bGEAR_DOWN == TRUE )
	{ 
		sBuzz_Control.u8BuzzerSound |= BEEP_ALARM;   
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~BEEP_ALARM;
	}



	//Tic Toc Alarm Left and Right Turn

	if(sBuzz_Control.bTURN_LEFT== TRUE || sBuzz_Control.bTURN_RIGHT== TRUE )
	{ 
		sBuzz_Control.u8BuzzerSound |= TIC_ALARM; 
		sBuzz_Control.u8BuzzerSound &= ~TOC_ALARM;
	}
	else
	{
		sBuzz_Control.u8BuzzerSound &= ~TIC_ALARM;
		sBuzz_Control.u8BuzzerSound |= TOC_ALARM;
	}

}


/*********************************************************************//**
 *
 * Handles system alarms as per priority 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vBuzzerControl()
{

	eBuzzerStateType BuzzerSoundType;

	if(sBuzz_Control.bBuzzerEnable)
	{
		// Set Buzzer priority order Here
		if(sBuzz_Control.u8BuzzerSound & (u8)LIFE_CRITICAL_ALARM)
			BuzzerSoundType = eLifeCriticalAlarm;     
		else if(sBuzz_Control.u8BuzzerSound & (u8)ENGINE_CRITICAL_ALARM)
			BuzzerSoundType = eEngineCriticalAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (u8)GENRAL_ALARM)
			BuzzerSoundType = eGeneralAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (u8)BEEP_ALARM)
			BuzzerSoundType = eBeepAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (u8)TIC_ALARM)
			BuzzerSoundType = eTicAlarm; 
		else if(sBuzz_Control.u8BuzzerSound & (u8)TOC_ALARM)
			BuzzerSoundType = eTocAlarm;
		else if(sBuzz_Control.u8BuzzerSound & (u8)CONTINUE_ALARM)
			BuzzerSoundType = eContinueAlarm;		
		else 
			BuzzerSoundType = eStopAlarm;  
	}
	else
		{
			BuzzerSoundType = eStopAlarm;	
		}


	switch(BuzzerSoundType)
	{
		case eLifeCriticalAlarm:
			_vLifeCriticalAlarm();
		break;

		case eEngineCriticalAlarm:
			_vEngineCriticalAlarm();
		break;
		case eGeneralAlarm:
			_vGeneralAlarm();
		break;
		case eTicAlarm:     
		case eTocAlarm:
			_vTicTocAlarm(BuzzerSoundType);
		break;    

		case eStopAlarm:
			_vStopBuzzer();
		break;
		case eContinueAlarm:
			_vOnBuzzer();
		break;	
		case eBeepAlarm:
			_vBeepSound();
		break;		
		default:
			_vStopBuzzer();
		break;
	}   	
	
}

/*********************************************************************//**
 *
 * Clear All buzzer status flag 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vClearAllLampsBuzzerFlag()
{
sBuzz_Control.bFRONT_AIR_PRESSUR = FALSE;
sBuzz_Control.bREAR_AIR_PRESSUR = FALSE;
sBuzz_Control.bSEAT_BELT = FALSE;
sBuzz_Control.bTILT_CAB = FALSE;
sBuzz_Control.bHTW = FALSE;
sBuzz_Control.bLFW = FALSE;
sBuzz_Control.bLOW_DEF = FALSE;
sBuzz_Control.bLOW_OIL_PRESSURE = FALSE;
sBuzz_Control.bSTOP_ENGINE = FALSE;
sBuzz_Control.bTRANCE_FLUDE = FALSE;
sBuzz_Control.bGSO = FALSE;
sBuzz_Control.bGEAR_UP = FALSE;
sBuzz_Control.bGEAR_DOWN = FALSE;
sBuzz_Control.bBRAKE_BEAR = FALSE;
sBuzz_Control.bPARK_BREAK = FALSE;
sBuzz_Control.bTURN_RIGHT = FALSE;
sBuzz_Control.bTURN_LEFT = FALSE;
sBuzz_Control.bEngineOverSpeed = FALSE;
sBuzz_Control.bLowBrakeFluid = FALSE;
sBuzz_Control.bBuzzerEnable = TRUE;


}


/*********************************************************************//**
 *
 * Life critical Alarm 500ms On and 500ms Off
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vLifeCriticalAlarm()
{ 
	static bool isBuzzerOn = FALSE;
	static u16 u16AlarmTimeStamp = 0;
	
	if( u16GetClockTicks() - u16AlarmTimeStamp >= 500)
	{ 
		if(isBuzzerOn)
		{
			_vOnBuzzer_4kHz();
		}
		else
		{
			_vStopBuzzer();
		}
		isBuzzerOn = (bool)!isBuzzerOn;  
		u16AlarmTimeStamp = u16GetClockTicks();     
	}
	
}

/*********************************************************************//**
 *
 * Engine critical Alarm 260ms On - 160ms Off and 260ms On -400ms off 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vEngineCriticalAlarm()
{

static bool isBuzzerOn = FALSE;
static u16 u16AlarmTimeStamp = 0;

	if( u16GetClockTicks() - u16AlarmTimeStamp >= 500)
	{ 
		if(isBuzzerOn)
		{
			_vOnBuzzer3_5kHz();
		}
		else
		{
			_vStopBuzzer();
		}
		isBuzzerOn = (bool)!isBuzzerOn;  
		u16AlarmTimeStamp = u16GetClockTicks(); 	
	}

}

/*********************************************************************//**
 *
 * General Alarm 200ms On - 200ms Off and 200ms On -600ms off 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vGeneralAlarm()
{

static bool isBuzzerOn = FALSE;
static u16 u16AlarmTimeStamp = 0;

	if( u16GetClockTicks() - u16AlarmTimeStamp >= 500)
	{ 
		if(isBuzzerOn)
		{
			_vOnBuzzer(); 
		}
		else
		{
			_vStopBuzzer();
		}
		isBuzzerOn = (bool)!isBuzzerOn;  
		u16AlarmTimeStamp = u16GetClockTicks(); 	
	}

}

static void _vBeepSound()
{

	
	static u8 count = 0;
	static u16 u16AlarmTimeStamp = 0;
	
		if(count == 0)
		{
			_vOnBuzzer();					
			u16AlarmTimeStamp = u16GetClockTicks();
			count = 1;
		}
		else if(u16GetClockTicks() - u16AlarmTimeStamp > 200)
		{
			_vStopBuzzer();					
			count = 0;
		}
			

}


/*********************************************************************//**
 *
 * Tic Toc sound generate
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vTicTocAlarm(eBuzzerStateType BuzzerSound)
{
 
	static u16 Chimerate = 0; 
	static eBuzzerStateType PreviousStatus = eTocAlarm;

	if( PreviousStatus != BuzzerSound)
	{
		PreviousStatus = BuzzerSound;
		if(BuzzerSound == eTicAlarm)
		{

			_vTicAlarm();	
		}
		else if(BuzzerSound == eTocAlarm)
		{
			_vTocAlarm();
		}

		Chimerate = 30;
		u16AlarmTimeStamp = u16GetClockTicks();     

	} 
	else if( u16GetClockTicks() - u16AlarmTimeStamp >= Chimerate)
	{ 
		_vStopBuzzer();
	}

}

/*********************************************************************//**
 *
 * Buzzer chime continuous 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vOnBuzzer()
{	
	
	SG0FL = 0x27;    //2000K freq
	SG0FH = 0x32; 
	SG0PWM = 0x14;   //50% Duty 
}

/*********************************************************************//**
 *
 * Buzzer chime continuous 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 static void _vOnBuzzer_2kHz()
{	
	
	SG0FL = 0x27;    //2000K freq
	SG0FH = 0x32; 
	SG0PWM = 0x14;   //50% Duty 
}

/*********************************************************************//**
 *
 * Buzzer chime continuous 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 static void _vOnBuzzer_4kHz()
{	
	
	SG0FL = 0x27;    //2000K freq
	SG0FH = 0x18; 
	SG0PWM = 0x28;   //50% Duty 
}

 /*********************************************************************//**
 *
 * Buzzer chime continuous 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 static void _vOnBuzzer3_5kHz()
{	
	
	SG0FL = 0x56;    //2000K freq
	SG0FH = 0x0c; 
	SG0PWM = 0x2C;   //50% Duty 
}


/*********************************************************************//**
 *
 * Buzzer chime Stop 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vStopBuzzer()
{	
	SG0FL = 0x00;    //2000K freq
	SG0FH = 0x00;
	SG0PWM = 0x00;

}

/*********************************************************************//**
 *
 * Generate Toc sound
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vTocAlarm()
{
     
     SG0FL = 0x35;    //Set Freq. 2100k
     SG0FH = 0x29;
	 SG0PWM = 0x22; //TOC set 90% duty
}


/*********************************************************************//**
 *
 * Generate TIC sound
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vTicAlarm()
{
     
     SG0FL = 0x35;    //1800K
     SG0FH = 0x32;
	 SG0PWM = 0x22;  //TIC duty 90%
}


/*********************************************************************//**
 *
 * Buzzer enable
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vBuzzerEnable()
{
	
	sBuzz_Control.bBuzzerEnable = TRUE;

}


/*********************************************************************//**
 *
 * Buzzer disable
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vBuzzerDisable()
{
	
	sBuzz_Control.bBuzzerEnable = FALSE;

}


/*********************************************************************//**
 *
 * Set Buzzer sound type like life critical ,engine critical ,continuous etc..
 *
 * @param      Sound Type    
 *
 * @return     None
 *************************************************************************/
void vSetBuzzerSoundType(u8 SoundType)
{	
	sBuzz_Control.u8BuzzerSound = SoundType;

}


void _vTpmsLamp(void)
{
			
	if(bTimeOutFlag65226_0x33 == TRUE && bTimeOutFlagDm165226_0x33 == TRUE)
	{
			 
		//TPMS_LAMP_OFF;	
	}
	else
	{		
		 if( u8TpmsLampStatus == 1)
		{			
			//TPMS_LAMP_ON;		
		}	
		else
		{
			//TPMS_LAMP_OFF;			
		}

	}

}



/*********************************************************************//**
*
* On/OFF Gear Shift UP  indicator as per CAN input 
* 00 - indicator(s) inactive
* 01 - upshift indicator active
* 10 - downshift indicator active
* 11 - don't care/take no action
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vGearShifUpLamp(void)
{
	u8 Data = 0;
	static u16 IndicatorTimeStamp = 0;
	static u16 IndicatorBuzzerTimerOut = 0;
	static bool BuzzerFlagstatus  = 0;
	
	
		if(bGetPGNTimeOut(eJGearShiftUpDown))  
		{
			sBuzz_Control.bGEAR_UP = FALSE;
			HIGH_GEAR_OFF;//GEAR_UP_SHIFT_OFF;
			//Keep it zero so that buzzer On instantly when indication ON 
			IndicatorBuzzerTimerOut = 0;
			BuzzerFlagstatus = 0;			
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJGearShiftUpDown);

			switch (Data)
			{				
				case eLampOn :								
					HIGH_GEAR_ON;//GEAR_UP_SHIFT_ON					
					if(u16GetClockTicks() - IndicatorTimeStamp  > IndicatorBuzzerTimerOut)
					{						
						IndicatorTimeStamp = u16GetClockTicks();						
						BuzzerFlagstatus = ~BuzzerFlagstatus;
						
						if(BuzzerFlagstatus)
						{			
							sBuzz_Control.bGEAR_UP = TRUE;	
							IndicatorBuzzerTimerOut = UP_DOWN_SHIFT_LAMP_BUZZER_ON;
						}
						else
						{				
							sBuzz_Control.bGEAR_UP = FALSE;
							IndicatorBuzzerTimerOut = UP_DOWN_SHIFT_LAMP_BUZZER_OFF;
						}		
						
					}
					
				break;
				case eError :
					
				case eDataNotAvailable :			
				default :								
					HIGH_GEAR_OFF;//GEAR_UP_SHIFT_OFF
					sBuzz_Control.bGEAR_UP = FALSE;
					//Keep it zero so that buzzer On instantly when indication ON 
					IndicatorBuzzerTimerOut = 0;
					BuzzerFlagstatus = 0;
				break;
			}

		}
	

}


/*********************************************************************//**
*
* On/OFF Gear Shift Down  indicator as per CAN input 
* 00 - indicator(s) inactive
* 01 - upshift indicator active
* 10 - downshift indicator active
* 11 - don't care/take no action
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vGearShifDownLamp(void)
{
	u8 Data = 0;
	static u16 IndicatorTimeStamp = 0;
	static u16 IndicatorBuzzerTimerOut = 0;
	static bool BuzzerFlagstatus  = 0;


		
		if(bGetPGNTimeOut(eJGearShiftUpDown))  
		{
			LOW_GEAR_OFF;//GEAR_DOWN_SHIFT_OFF
			sBuzz_Control.bGEAR_DOWN = FALSE;
			//Keep it zero so that buzzer On instantly when indication ON 
			IndicatorBuzzerTimerOut = 0;
			BuzzerFlagstatus = 0;			
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJGearShiftUpDown);

			switch (Data)
			{
				
				case eError : //On gear shift down lamp while bit 10
					LOW_GEAR_ON;//GEAR_DOWN_SHIFT_ON 					
					if(u16GetClockTicks() - IndicatorTimeStamp  > IndicatorBuzzerTimerOut)
					{						
						IndicatorTimeStamp = u16GetClockTicks();						
						BuzzerFlagstatus = ~BuzzerFlagstatus;
						
						if(BuzzerFlagstatus)
						{			
							sBuzz_Control.bGEAR_DOWN = TRUE;							
							IndicatorBuzzerTimerOut = UP_DOWN_SHIFT_LAMP_BUZZER_ON;
						}
						else
						{				
							sBuzz_Control.bGEAR_DOWN = FALSE;
							IndicatorBuzzerTimerOut = UP_DOWN_SHIFT_LAMP_BUZZER_OFF;
						}		
						
					}
					
				break;
				case eDataNotAvailable :			
				default :
					LOW_GEAR_OFF;//GEAR_DOWN_SHIFT_OFF 					
					sBuzz_Control.bGEAR_DOWN = FALSE;
					//Keep it zero so that buzzer On instantly when indication ON 
					IndicatorBuzzerTimerOut = 0;
					BuzzerFlagstatus = 0;
				break;
			}

		}
	
		
}

/*********************************************************************//**
*
* On/OFF Trans Fluid Temp indicator as per CAN input only single frame implement 
* Multi frame is peding 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vTransFluidTemp(void)
{
	u8 Data = 0;
	static u16 clkTime = 0;	
	static bool StatusFlag = 0;
	
	u8 u8TransType = 0;
		
	u8TransType = u8GetTransmissionType();

	if( (sIndicatorEnable.Enablebytes.Byte3.bTransFluidTemp == eEnable )
		&& ( sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bTransmissionDisplay == eEnable ) 
		&&	(u8TransType == AT_TRANS ))	
	{
		if(bGetPGNTimeOut(eJTransFluidTemp) && bGetPGNTimeOut(eJETC7)) 
		{	
			sBuzz_Control.bTRANCE_FLUDE = FALSE;
			if(bCanLostLampFlashFlag)
			{				
			 	//TRANSFLUID_TEMP_ON;
			}
			else					
			{						 
				//TRANSFLUID_TEMP_OFF;
			}			
		}
		else if(!bGetPGNTimeOut(eJTransFluidTemp) && bGetPGNTimeOut(eJETC7))
		{
			//TRANSFLUID_TEMP_OFF;
			sBuzz_Control.bTRANCE_FLUDE = FALSE;
		}
		else if(bGetPGNTimeOut(eJTransFluidTemp) && !bGetPGNTimeOut(eJETC7))
		{
			//TRANSFLUID_TEMP_OFF;
			sBuzz_Control.bTRANCE_FLUDE = FALSE;
		}
		else
		{
			Data =	(eOBDLampdataType)u32GetPGNdata(eJTransFluidTemp);

			switch (Data)	
			{
				case eLampOff :
					 //TRANSFLUID_TEMP_OFF;
					 sBuzz_Control.bTRANCE_FLUDE = FALSE;
				break;
				case eLampOn :
					//TRANSFLUID_TEMP_ON;
					sBuzz_Control.bTRANCE_FLUDE = TRUE;
				break;

				case eDataNotAvailable :
				default :
					 //TRANSFLUID_TEMP_OFF;
					 sBuzz_Control.bTRANCE_FLUDE = FALSE;
				break;
			}
		}
	}
	else
	{
		//TRANSFLUID_TEMP_OFF;
		sBuzz_Control.bTRANCE_FLUDE = FALSE;
	}

	if(sBuzzerEnable.Enablebytes.Byte2.bTransFluid == eDisable)	
		sBuzz_Control.bTRANCE_FLUDE = FALSE;
		
	}	


/*********************************************************************//**
*
* On/OFF Power take off indicator as per CAN input 
*
* @param      None    
*
* @return     None
*************************************************************************/

static void _vPowerTakeOffLamp(void)
{

	u8 Data = 0;
	
	
		if(bGetPGNTimeOut(eJPowerTakeOff)) 
		{		 
			POWER_TAKE_OFF_OFF;	
			bPtoStatus = FALSE;
		}
		else
		{
			Data =	u32GetPGNdata(eJPowerTakeOff);

			if(Data >= 0x01 && Data <= 0x14)
			{
				POWER_TAKE_OFF_ON;
					bPtoStatus = TRUE;
			}
			else
			{
				POWER_TAKE_OFF_OFF;
				bPtoStatus = FALSE;
			}
					
			
		}


}


/*********************************************************************//**
*
* On/OFF Netural indicator as per CAN input
*
* @param      None    
*
* @return     None
*************************************************************************/
static void _vNeutral(void)
{		
	u8 data = 0;

	if(sIndicatorEnable.Enablebytes.Byte2.bNeutral == eEnable)	
	{
		if(sIndicatorInputType.Inputsbytes.Byte3.bNeutralTellTaleInputType == eAnalouge)
		{
			//NEUTRAL_OFF;
			bNeturalIndicatorStatus = FALSE;
		}
		else
		{
		  	if(bGetPGNTimeOut(eJNuetral)) 
			{
				//NEUTRAL_OFF;
				bNeturalIndicatorStatus = FALSE;
						
			}  		 
			else
			{
				data = (u8)u32GetPGNdata(eJNuetral);
			
				if(data == 0x7D)
				{
					//NEUTRAL_ON;
					bNeturalIndicatorStatus = TRUE;	//This flag used for parkbrake buzzer control
				}
				else
				{
					//NEUTRAL_OFF;
					bNeturalIndicatorStatus = FALSE;	//This flag used for parkbrake buzzer control
				}
			}
		}
	}	
	else
	{
		//NEUTRAL_OFF;
		bNeturalIndicatorStatus = FALSE;	//This flag used for parkbrake buzzer control
	}
		
}

/*********************************************************************//**
*
* On/OFF Trans range indicator as per input 
*
* @param      None    
*
* @return     None
*************************************************************************/
void _vMainBeamLamp(void)
{


		
			if(sLampInputsProp[eMAIN_BEAM_INPUT].u8StatusFlag == LOW)
			{
				MAIN_BEAM_ON;
			}
			else
			{
				MAIN_BEAM_OFF;
			}
		
		

}


bool b1HzToggleFlash(void)
{
 return(bToggleFlag1Hz);
}

