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

                                                                                                                           

 /// \file main.c
 /// \brief BSVI Documentation
 /// \author Copyright (c) 2012 Indication Instruments Limited. All rights reserved.
 /// \version 1.0
 /// \date 19/12/2018
 /// Revision History : Initial version
 
/*!
	\mainpage 

	\section welcome Welcome
	**  This project has 4 stepper gauges and Segmented LCD 
	
	**  1) Speedometer based on Frequency Input.
	
	**	2) RPM based on CAN input.
	
	**  3) Fuel Gauge based resistive input.
	
	**  4) Coolent Temperature based on CAN input.

	**  5) DEF gauge based on CAN input.
	
	**  6) AP Front and rear based on resistive input
	
	**   LCD Screens are:-
	
	**  1) Odometer
	
	**  2) HourMeter
	
	**  3)Front & Rear AP Gauge based resistive input.
	
	**  4)Oil pressure gauge based resistive input.
	
	**  5)DEF gauge based CAN input.
	
	**  6)TelleTales

	\section sect1 "BSVI Project" Documentation
*/


/**************************************************************************
** Project :  523265/66  BSVI
** Description: 
**  
**  This project has 4 stepper gauges and Segmented LCD 
**  1) Speedometer based on Frequency Input.
**	2) RPM based on CAN input.
**  3) Fuel Gauge based resistive input.
**  4) Coolent Temperature based on CAN input.
**  5) DEF gauge based on CAN input.
**  6) AP Front and rear based on resistive input
**   LCD Screens are:-
**  1)Odometer
**  2)HourMeter
**  3)Front & Rear AP Gauge based resistive input.
**  4)Oil pressure gauge based resistive input.
**  5)DEF gauge based CAN input.
**  6)TelleTales
**  
**  
** Device(s) used :
**                   NEC RL78\D1A group R5F10TPJ -  100 pins 
** memory map:
**                   16  KB RAM                  
**                   8  KB Data Flash           
**                   256 KB flash               
**                        
**                   16 MHz External crystal 
**                   1 CAN Module
**                   10 bit ADC
** --------------------------------------------------------------------------
**  COPYRIGHT:  Indication Instruments Limited 
**              Plot No. 19,Sector6
**              Faridabad - 121006         
** --------------------------------------------------------------------------
**  IDE				: CubeSuite+(CS+)
**  Version			: V3.00.00
**  Compiler		: CA78K0R
**  Debuging Tool	: Renesas E1 programmer
**  

**---------------------------------------------------------------------------
**  Co-ordinator                Mukul Goyal
**  Software Manager		  Khusboo Lalani
**---------------------------------------------------------------------------
**  Revision History
**
**  Date        Name    			Version              Description
** ==========  =======  =======  =====================================
** 09/18/2018  Vishnu Singh         V1.0  Initial Version.
** 23/01/2020   VJ/PA				V1.01 Fuel gauge anti sloshing update & Top line update 
** 12/02/2020  Pratyush 			V1.02 Top Lines update
** 04/06/2020  Vishnu 				Fuel gauge Max at 180ohm 

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_serial.h"

#include "r_cg_sg.h"
#include "customTypedef.h"
#include "Can.h"
#include "J1939.h" 
#include "TellTales.h"
#include "StepperDriver.h"
#include "dataflash.h"
#include "Lcd.h"
#include "iicDriver.h"
#include "Speedo.h"
#include "Tacho.h"
#include "Def.h" 
#include "CoolantTemp.h" 
#include "AirPressureGauge.h" 
#include "FuelGauge.h" 
#include "OilPressureGauge.h" 
#include "Adc.h" 
#include "Eol.h" 
#include "Amt.h"
#include "CurveHeader.h"
#include "DiscreteSwitches.h"
#include "r_cg_userdefine.h"
#include"ledDriver5716.h"
#include "P15765H.h"
#include "TML_14229_UDSH.h"
#include "Afe.h"
#include "HourMeter.h"
#include "ServiceReminder.h"
#include "addressClaim.h"
#include "OdoMeter.h"
#include "GenIllumination.h" 
#include "MiscFiles.h"
#include "CanTxData.h"
#include "OdoMeter.h"
#include "LCDScreens.h"
#include "Clock.h"
#include "interrupts.h"
#include "ramVector.h"
#include "DtcCycle.h" 
#include "Priority2Texts.h"
#include "IoControlGauges.h"

// Pins declarations -----------------------------------------------------
#define LCD_ENABLE                	P15.0


// Macro/Constant declarations -----------------------------------------------------
#define BOOTLOADER_FLASH_ADDR		0x0000d8U
#define MCU_WDT_RESET        	(RESF & 0x10) //Flag indicates if Watchdog reset has occured
#define IGN_SENSE_TIME            	100     //Ignition ON/OFF time sense in ms
#define MAIN_LOOP_TIME 				1
#define BUZZ_STOP_TIME_IN_SELF_TEST 2000	//Buzzer stop time during self test
#define LAMPS_TIME_IN_SELF_TEST     4200//4070	// In ms; this value has been adjusted such that lamp self-test lasts for 
											//exactly 5s from the time ignition is turned ON. 
											//checking steps one CRO probes connect to ignition sense and other cro probs 
											//connect to Mil lamp then turn On Ignition and measure time from Igniton Turn On to Mil Lamp turn OFF


// Global declarations --------------------------------------------------------

u32 gu32Clock1msTicker = 0;

// Local declarations --------------------------------------------------------
static u16 TimeStampDuringSelfTest =0;
static bool bProgDependenciesFlag = 0U;

typedef struct
{	
	u16     u16IgnTimeStamp;   //Time stamp for sense Ignition 
	bool 	bIgnitionStatusFlag;
	
} IGN_TYPE;
typedef unsigned char (*pt2Function)(unsigned long, unsigned short * );

//Stucture Object declaration-------------------------------------------------
IGN_TYPE sIgnition;
eCLUSTER_STATE_MACHINE clusterStatus = eClusterBoot;

// private prototypes --------------------------------------------------------

/// @addtogroup MAIN_LIBRARY
/// @{
 
/// @defgroup MainLib  functions used in main file
//// 
///   
/// @addtogroup MainLib
/// @{
//Local function 
static void _vProgDependenciesInit(void);
static void _vTasksBeforeLongHoming(void);
static void _vTasksBeforeIgnitionCheck(void);
static void _vTasksAfterIgnitionOn(void);
static void _vClusterSleep();
static void _vSetDataUpdateFlagsAtIGNOff();
static void _vMapInterrupts();
static void MY_Bootloader_Run(void);
/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/// @} // endgroup MainLib

/// @} // endgroup MAIN_LIBRARY

/*********************************************************************//**
 *
 * Main system processing 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void main(void)
{
	static bool isIgnition = FALSE; 	
	static u16 TimeChecker = 0;
	static u16 MainLoopRefreshTime = 0;
	static bool bManualSelfTestState = FALSE;
	static u16 temp = 0;
	int i =0;
		
	while (1U)
	{	
		switch (clusterStatus)
		{	
		    case eClusterBoot:
				_vTasksBeforeLongHoming(); 
			break;
			case eClusterBatteryReset:
				vLongHoming();
				vShortHoming();
			case eClusterWatchDogReset:
				//200ms delay before reading dataflash
				MainLoopRefreshTime = u16GetClockTicks();
				while(u16GetClockTicks() - MainLoopRefreshTime < 200) 
					R_WDT_Restart();
				_vTasksBeforeIgnitionCheck();
				break;	
			case eClusterIgnitionReset:
				_vTasksAfterIgnitionOn();	
			case eClusterNormalOperation:				
				vAddressClaimStateMgr();	
				vJ1939_ServiceManager();
				vSpeedDataProcessing();
				vRpmDataProcessing();
				vStepperPositionDriver(eTachometer);
				vStepperPositionDriver(eSpdMeter);				
		
				if(u16GetClockTicks() - MainLoopRefreshTime > MAIN_LOOP_TIME) 
				{
					MainLoopRefreshTime = u16GetClockTicks();						
					   
					R_WDT_Restart();				
					vScreenTransition();
					vBuzzerControl();			
					vDefGauge(); 			
					vReadAdc(); 			
					vAPGFrontDataProcessing();
					vAPGRearDataProcessing();			
					vOilPressureGauge();			
					vFuelGauge();											
					vCoolantTempDataProcessing();			
					vClockUpdate();					
					vStepperPositionDriver(eTempGauge);
					vStepperPositionDriver(eFuelGauge);
					vStart_Hour_Meter();			
					vControlTellTales();			
					//call refresh lcd in last so that all updated value can display 
		            vAMTProcessFlow();
					vBattVolt();
					vOatDataProcessing();
					vSetEMSFaultText();
					vRefreshLcd();
					vIlluminationControl(isIgnition,eIlluminationNormalMode);
					vSendDataToI2C();
					vWriteToCANTxArray();
					vCalculateAfe();
					vResetTripAfeData();
					vSaveFullSystemParameter();					
					//diagnostics block start
				    processDiagnosticsMessages();
				    sendDiagnosticsResponse();
				    sessionTimeout();					
					vRecoveryFromBusOffState();
					vServiceReminder();
					//vDTCOperationCycle();
					//vAdjustFaultTimers();
					vIoControlTellTaleLedBased();
					vIoControlGauges();						
					//ReCanInit Must call in the end so that processDiagnosticsMessages 
					//function sent responce when user change baud rate
					vReCANInit(); 
					isIgnition = bCheckIgnitionOff(); 
				}
				break;
			case eClusterForcedSelfTest:
			case eClusterEnterSleep:
				
				R_WDT_Restart();
				for(i = 0; i < 18; i++)
				{
				vRefreshLcd();
				vSendDataToI2C();
				}
				_vClusterSleep();
				vClearAllLampsBuzzerFlag();
				vLedPrevStateInit(); 
				break;
			
			case eClusterRestart:
				//Save clock data when Igniton ON So that after save clock data CPU can restart 
				vSetUpdateData(TRUE);
			    vSaveClockData();	
				NOP();NOP();NOP();NOP();NOP();
				vResetCpu();
				break;
			case eCheckProgDependencies:
				_vProgDependenciesInit();
				break;
			case eClusterSleep :	
			default:
				R_WDT_Restart();
				
				vIlluminationControl(isIgnition,eIlluminationNormalMode);
				isIgnition = bCheckIgnitionOn();
				vJ1939_ServiceManager();
				vLampsWorkInSleepMode();
				vBuzzerControl();
				//must call clockUpdate function if user want to save data on minute change 
				vClockUpdate(); 
				vSaveClockData();
				
			break;
		
		}

		if(clusterStatus == eClusterBoot )	
		{
			if(MCU_WDT_RESET != 0x10)		//is it not a watchdog reset ?
			{
				clusterStatus = eClusterBatteryReset;
				
			}
			else
			{
				clusterStatus = eClusterWatchDogReset;
			}
		}	
		else if (clusterStatus == eClusterBatteryReset || clusterStatus == eClusterWatchDogReset )					
		{					
			TimeChecker = u16GetClockTicks();				
			while(u16GetClockTicks() - TimeChecker < IGN_SENSE_TIME)				
				isIgnition = bCheckIgnitionOn();			

			if(isIgnition == TRUE)				
				clusterStatus = eClusterIgnitionReset;			
			else				
				clusterStatus = eClusterEnterSleep; 		
		}					
		else if (clusterStatus == eClusterIgnitionReset )					
		{					
			clusterStatus = eClusterNormalOperation;				
		}					
		else if ( clusterStatus == eClusterNormalOperation )					
		{
			if(isIgnition == TRUE ) 			
			{	
				bManualSelfTestState = bGetDoSelfTestStatus();
				if ( bManualSelfTestState == TRUE )	
				{
					clusterStatus = eClusterForcedSelfTest; 	
				}
				else
				{
				  bManualSelfTestState = bIsECUResetByService();
					if ( bManualSelfTestState == TRUE )	
					{
						clusterStatus = eClusterForcedSelfTest; 
					}
				}
				
				temp = u16GetRoutineControlServiceStatus();
				
				if(temp == CHECK_PROG_DEP)
				{
					clusterStatus = eCheckProgDependencies; 
				}
			}				
			else				
				clusterStatus = eClusterEnterSleep; 		
		}					
		else if ( clusterStatus == eClusterForcedSelfTest ) 				
		{					
			clusterStatus = eClusterRestart;				
		}					
		else if (clusterStatus == eClusterEnterSleep)					
		{					
			clusterStatus = eClusterSleep;				
		}					
		else if (clusterStatus == eClusterSleep )					
		{					
			if(isIgnition == TRUE ) 			
			clusterStatus = eClusterRestart;			
		}	
		
	}//End While
    /* End user code. Do not edit comment generated here */
}

/*********************************************************************//**
 *
 * Checking programming depedencies return all pointer to home ,Stop buzzer and lamps 
 * then after 1 sec jump in bootloader code 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

static void _vProgDependenciesInit(void)
{

	static bool firstEntryFlag = TRUE;
	static u16 timestamp = 0U;


 	sendDiagnosticsResponse();	
 
	if(firstEntryFlag)
	{
		vSetBuzzerSoundType(BUZZER_STOP);
		vSetHourMeterEnableStatus(FALSE);
		vBuzzerControl();
		vAllLampsOff();
		vSetLedDriverBlankLow();   		//Keep low BLANK pin of LED driver so that all indicator off
		vMoveStepperToHomePosition(); 	//Move stepper to initial Position from its current position
		LCD_ENABLE = LOW;		 		//Disable LCD display 
		vIlluminationControl(FALSE,eIlluminationNormalMode);	
		
		bProgDependenciesFlag = TRUE;
		firstEntryFlag = FALSE;
		timestamp = u16GetClockTicks();
		RoutineService();
	}	
	
	//Wait Aprox 1 sec so that use response send succesfully 
	if((u16GetClockTicks() - timestamp) > 1000U)
	{
		vSaveBootloaderState(eBootloaderMode);		
		NOP();NOP();NOP();NOP();NOP();		
		MY_Bootloader_Run();
	}
}

/*********************************************************************//**
 *
 * Function used to jump in bootloader code
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void MY_Bootloader_Run(void)
{
	//__far unsigned int *ptr;
	__far u16 *ptr;
	pt2Function fp;

	ptr = (__far u16 *)( BOOTLOADER_FLASH_ADDR );

	if ( *ptr != 0xffffU )
	{
		// call user program and never return
		fp = (pt2Function) ptr;
		fp( 0U, 0U);
	}
	else
	{
		// invalid user reset address
		// some problem return
		return;
	}

	
}

/*********************************************************************//**
 *
 * controller Mode satus keep alwary Application mode in application code 
 *
 * @param      None
 *
 * @return     system clock value in u32
 *************************************************************************/

u8 GetControllerMode(void)
{	
 return(eApplicationMode);
}

/*********************************************************************//**
 *
 * Give check dependencies status
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bCheckDepenDencies(void)
{
 	return(bProgDependenciesFlag);
}

/*********************************************************************//**
 *
 * Flash Erase Status (Dummy function for keeping UDS service code same in Application & Bootloader)
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bFlashEarseStatus(void)
{
	return(TRUE); 

}
/*********************************************************************//**
 *
 * Clear Flash Erasing status flag
 *(Dummy function for keeping UDS service code same in Application & Bootloader)
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void ClearFlashEraseFlag(void)
{
	;
}

/*********************************************************************//**
 *
 * Return flash writing checksum status
 *(Dummy function for keeping UDS service code same in Application & Bootloader)
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bCheckSumStatus(void)
{
 return(TRUE);
}

/*********************************************************************//**
 *
 * Return flash writing status
 *(Dummy function for keeping UDS service code same in Application & Bootloader)
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bFlashWriteStatus(void)
{
 return(1);
}

/*********************************************************************//**
 *
 * Clear flash writing status
 *(Dummy function for keeping UDS service code same in Application & Bootloader)
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vClearFlashWriteStatus(void)
{
	 ;
}

/*********************************************************************//**
 *
 *Task must be perform before ignition ON
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void _vTasksBeforeIgnitionCheck()
{
	vDataFlashinit();			// Data Flash init compulsory before read flash memory 
	vSetDefaultVariablesToFlash();		//Initialize dataflash if fresh MC				
	vReadSystemParameter();	
	vSetAllEolData();
	vJ1939Init();               //J1939 buffer init before CAN init.
	vCanInit();	
	vBuzzerEnable();

}


/*********************************************************************//**
 *
 * Enable interrupts and initialize varibale and 
 * check power on reset status before doing stepper self test
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void _vTasksBeforeLongHoming(void)
{
	/* Start user code. Do not edit comment generated here */
	_vMapInterrupts();
	//This flag must be true here so that clock can count second before read flash data
	vSetPowerONCountFlag(TRUE);	
	R_TAU0_Channel5_Start();  		//TAU05 Clock timer must be start here for clock accuracy 	
	vStepperInit();		
	//Set low Illumination PWM resister before start timer other Illumination will blink	
	vIlluminationControl(TRUE,eIlluminationOff);
	R_TAU1_Channel0_Start();		//TAU10 PWM timer for Illumination control

	R_TAU0_Channel1_Start();  		//TAU01 100us timer for stepper motors 
	R_TAU0_Channel4_Start();  		//TAU04 1 ms timer for system clock
	
	EI();                           //Enable all interrupts
	R_WDT_Restart();
		
    /* End user code. Do not edit comment generated here */
}

/*********************************************************************//**
 *
 * Enable all update flags on ignition off to save EEPROM parameters. 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

static void _vSetDataUpdateFlagsAtIGNOff()
{
	vSetGenIlluUpdateFlag(TRUE);
	vSetOdoUpdateFlagStatus(TRUE);	
	vSetOdoFractionUpdateStatus(TRUE);
	vSetOdoTripAUpdateStatus(TRUE);
	vSetOdoTripAFractionUpdateStatus(TRUE);
	vSetOdoTripBUpdateStatus(TRUE);
	vSetOdoTripBFractionUpdateStatus(TRUE);	

	//Hour Meter Flags//
	vSetHoursUpdateStatus(TRUE);
	vSetHoursFractionUpdateStatus(TRUE);
	vSetHoursTripAUpdateStatus(TRUE);
	vSetHoursTripAFractionUpdateStatus(TRUE);
	vSetHoursTripBUpdateStatus(TRUE);
	vSetHoursTripBFractionUpdateStatus(TRUE);	

	//AFE Flags//
	vSetUpdateAfeTripA(TRUE);
	vSetUpdateAfeTripB(TRUE);
	vSetUpdatePreFuelConsumed(TRUE);
	vSetUpdateAfeTripAFuelUsedAtReset(TRUE);
	vSetUpdateAfeTripBFuelUsedAtReset(TRUE);
	
}



/*********************************************************************//**
 *
 * ON all indicators , init LCD and do self testing 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vTasksAfterIgnitionOn()
{
	bool bIgn = TRUE;
	R_CSI10_Start();				//SPI used for LED driver 
	R_TAU0_Channel0_Start();		//TAU00 Pwm timer for Leddriver generat 4Mhz Pwm
	R_ADC_Start();
	
	LCD_ENABLE = HIGH;	//Enable LCD driver before LCD init.	
	//Set supply as per fuel selection Analouge or Capacitive 
	vSetFuelCommonSupply();
	vLedDriverInit();
	
	//Init LCD before self testing and Must Enable LCD and LED driver before LCD init.S	
	vLcdInit(); 
	
	vI2CBufferInit();
	vLedPrevStateInit(); 	
	vCheckLamps();			
	diagnosticDataInit();
	vJ1939Init();               //J1939 buffer init before CAN init.	
	R_TAU1_Channel6_Start();  	//Capture timer for Speedo
	
	//Make LCD and general illumination at 100% during self tesing 
	vIlluminationControl(TRUE,eIlluminationFullIntensityOn);		
	vSetBuzzerSoundType(CONTINUE_ALARM);
	vBuzzerControl();	 
	vSetLedDriverBlankHigh();
	
	//Buzzer stop time must init. before self test
	TimeStampDuringSelfTest = u16GetClockTicks();
	vJ1939TxInit();
	vStepperSelfTest();		
	
	

							
}

/*********************************************************************//**
 *
 * Off all indicators, stop buzzer  , return steppers to home and disable unnecessary interrupts
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vClusterSleep()
{	
	u8 i =0;
	//Disable All tx message 
	for (i = 0; i < eNoOfPgnTx; ++i)	
	{
		vJ1939_TxMsgEnableDisable(i,FALSE);
	}
	vSetBuzzerSoundType(BUZZER_STOP);
	vSetHourMeterEnableStatus(FALSE);
	vBuzzerControl();
	vAllLampsOff();
	vSetLedDriverBlankLow();   		//Keep low BLANK pin of LED driver so that all indicator off
	vMoveStepperToHomePosition(); 	//Move stepper to initial Position from its current position
	LCD_ENABLE = LOW;		 		//Disable LCD display 	
	R_IIC11_Stop();			 		//I2C for LCD
	R_CSI10_Stop();          		//SPI for LED Driver
	R_TAU1_Channel6_Stop();  		//TAU16 Capture timer for Speedo  
	R_TAU0_Channel3_Stop();  		//TAU03 For Hour Meter Counting 
	R_TAU0_Channel0_Stop();  		//TAU00 PWM for LED driver
	R_ADC_Stop();  
	vIlluminationControl(FALSE,eIlluminationNormalMode);						
	_vSetDataUpdateFlagsAtIGNOff();
	vSaveFullSystemParameter();
	vLongHoming();
	vShortHoming();
	R_TAU0_Channel1_Stop();  		//TAU01 100us timer for stepper motors 
	vDisableStepper();
}





/*********************************************************************//**
 *
 * get cluster processing status 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
eCLUSTER_STATE_MACHINE eGetClusterStatus()
{
	return clusterStatus;
}


/*********************************************************************//**
 *
 * Check Ignition status with debounce time if ignition singal 
 * have glitch then reset debounce time,call this function when ignition ON 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bCheckIgnitionOff()
{
	bool status = TRUE;

	//Start igniton off time counter and if any glitch occur in signal then reset off time sense counter
	if(IGNITION == LOW)
	{
		if( u16GetClockTicks() - sIgnition.u16IgnTimeStamp  >= IGN_SENSE_TIME)       
		status = FALSE;
	}
	else
	{
		sIgnition.u16IgnTimeStamp = u16GetClockTicks();
	}

	if(status == TRUE)
		sIgnition.bIgnitionStatusFlag = TRUE;
	else
		sIgnition.bIgnitionStatusFlag = FALSE;
	
	return(status);
}

/*********************************************************************//**
 *
 * Ignition current status 
 *
 *
 * @param      None    
 *
 * @return     Ignition Current status 
 *************************************************************************/

bool bGetIgnStatus()
{
 return sIgnition.bIgnitionStatusFlag;
}

/*********************************************************************//**
 *
 * Check Ignition status with debounce time if ignition singal 
 * have glitch then reset debounce time ,call this function when ignition OFF
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bCheckIgnitionOn()
{
	bool status = FALSE;
	
	//Start igniton ON time counter and if any glitch occur in signal then reset On time sense counter

	if(IGNITION == HIGH)
	{
		if( u16GetClockTicks() - sIgnition.u16IgnTimeStamp  >= IGN_SENSE_TIME)
		status = TRUE;
	}
	else
	{
		sIgnition.u16IgnTimeStamp = u16GetClockTicks();
	}

	if(status == TRUE)
		sIgnition.bIgnitionStatusFlag = TRUE;
	else
		sIgnition.bIgnitionStatusFlag = FALSE;

	return(status);
}




/*********************************************************************//**
 *
 * Funtion use for do special task during self testing and its
 * calling from stepper driver while doing self testing
 *
 * @param      None
 *
 * @return     None 
 *************************************************************************/
void vTaskDoneDuringSelfTesting()
{
	bool ignstatus = 0;
	static bool entryflag = 0;
	static u16 buzzerStopTimeStamp = 0;
	
	vSendDataToI2C();
	vReadAdc();	
	vAddressClaimStateMgr();
	//Call this function during selfTesting So that j1939 buffer fill by CAN msg and no CAN msg lost
	vJ1939_ServiceManager();
	
	//Off all tell tale if ignition Off during self test
	ignstatus = bCheckIgnitionOff();	
	if(ignstatus == FALSE)
  	{
		vAllLampsOff();
		
  	}
	else
	{
		if(u16GetClockTicks() - TimeStampDuringSelfTest >= LAMPS_TIME_IN_SELF_TEST)
		{
	   		vControlTellTales();
		}
	}
	
	if( (u16GetClockTicks() - TimeStampDuringSelfTest  > BUZZ_STOP_TIME_IN_SELF_TEST ) && entryflag == FALSE )	
	{
		vSetBuzzerSoundType(BUZZER_STOP);		
		entryflag = TRUE;
	}
	vBuzzerControl();	
	
}


/*********************************************************************//**
 *
 * 1ms System clock
 *
 * @param      None
 *
 * @return     system clock value in u32
 *************************************************************************/
u32 u32GetClockTicks(void)
{
	return gu32Clock1msTicker;
}

/*********************************************************************//**
 *
 * 1ms System clock
 *
 * @param      None
 *
 * @return     system clock value in u16
 *************************************************************************/
u16 u16GetClockTicks(void)
{
	return (u16)gu32Clock1msTicker;
}

/*********************************************************************//**
 * This function hold CPU processing in it as per passing time in ms.
 * 
 *
 * @param      time in ms
 *
 * @return     None 
 *************************************************************************/
void vWatingTimeMs(u16 WaitTime)
{
	u16 WatingClk = 0;
	WatingClk = u16GetClockTicks();
	while((u16GetClockTicks() - WatingClk) < WaitTime)
	{
		R_WDT_Restart(); 
	}
}

/*****************************************************************************

**  Function name:  _vMapInterrupts
**
**  Description:    Mapped Interrupts to vector table
**
**  Parameters:     None
**
**  Return value:   None
**
*****************************************************************************/
static void _vMapInterrupts()
{
	RAM_INTTM01_ISR = &r_tau0_channel1_interrupt;
	RAM_INTTM03_ISR = &r_tau0_channel3_interrupt;
	RAM_INTTM04_ISR = &r_tau0_channel4_interrupt;
	RAM_INTTM05_ISR = &r_tau0_channel5_interrupt;
	RAM_INTTM16_ISR = &r_tau1_channel6_interrupt;
		
	RAM_INTAD_ISR = &r_adc_interrupt;
	RAM_INTCSI10_ISR = &r_csi10_interrupt;
	
	RAM_INTIIC11_ISR = &r_iic11_interrupt;
	
	RAM_INTWDTI_ISR = &r_wdt_interrupt;
	
	RAM_INTC0REC_ISR = &MY_Rx_MsgBuf_Processing;
	RAM_INTC0TRX_ISR = &MY_Tx_MsgBuf;
	RAM_INTC0ERR_ISR = &MY_ISR_CAN_Error;
	RAM_INTC0WUP_ISR = &MY_ISR_CAN_Wakeup;	


}
/* End user code. Do not edit comment generated here */
