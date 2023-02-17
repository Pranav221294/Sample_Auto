#include "customTypedef.h"
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "LCDScreens.h"
#include "dataflash.h"
/*Switch Library For Screen Transition*/
#include "DiscreteSwitches.h"
#include "StepperDriver.h"
#include "GenIllumination.h" 
#include "Clock.h"
#include "GenIllumination.h"
#include "Event.h"
#include "Lcd.h"
#include "J1939.h"

#define TIME_OUT 				3500
#define TIME_OUT_TO_DEFAULT 	10000
#define TIME_OUT_SAVE_CLOCK		5000


typedef struct
{	
	u8 u8TripAB;
	u8 u8ClockSet;
	SCREEN_SET eMode;	
	ODO_RESET eOdoTripReset;
	HOUR_RESET eHourTripReset;
	//AFE_RESET eAFETripReset;
	AFE_RESET eAFETripAReset;
	AFE_RESET eAFETripBReset;
	SCREEN_SET eFeederPulse;
	u8 u8DTCMsgNumber;
	bool bDoSelfTest : 1;
	bool bSendTripResetStatus : 1;
}SCREEN_Functions;


SCREEN_SET eScreen = eTripAScreen;

SCREEN_Functions sScreenFunc = {eTripAScreen,0,0,0,0,0,0,0,0,0,0};
SCREEN_Functions sScreenSelect = {eTripAScreen,0,0,0,0,0,0,0,0,0,0};

PULSE_DATA_TYPE  sPulseData  = {0,0,0,0};
PULSE_DATA_FLAG sPulseFlagSet ={FALSE,FALSE,FALSE};

static DTC_TYPES eDtcTypes = 0U;
static SCREEN_SET ePreviousScreen = 0;

static u32 _u32ChangeMainScreen(u8 , u8 , u8 , u8 , u32 );
static void _vDisplayDispClockData();
static void _vDisplayMemClockData();
static void _vSaveDispClockData();
static void _vCheckTripRolloverStatus(void);
u8 u8GetNoOfDigit(u16);
static u8 u8GetDigit(u16);
static u16 pulses[5];
static u32 _u32ChangeMainScreen(u8 u8SetStatus, u8 u8UpStatus, u8 u8DownStatus, u8 u8ModeStatus, u32 u32TimeStamp)
{
	u16 u16VehicleSpeed = 0;
	u16 u16EngineRPM = 0;
	

	u16VehicleSpeed = u16GetVehicleSpeed();				// for manual self test
	
	u16EngineRPM    = u16GetEngineSpeed();				// for manual self test
	
	
	if((u8UpStatus == eUpDownLongPressed || u8DownStatus == eUpDownLongPressed)
		     && u16VehicleSpeed == 0 && u16EngineRPM == 0)
	{
		sScreenFunc.bDoSelfTest = TRUE;	
		u32TimeStamp = u32GetClockTicks();
	}
	else if(u8UpStatus == eLongPressed || u8DownStatus == eLongPressed) //2 sec
	{
		ePreviousScreen = eScreen;
		eScreen = eIllumination;
		u32TimeStamp = u32GetClockTicks();
	}
	else if(u8ModeStatus == eModeSetLongPressed || u8SetStatus == eModeSetLongPressed)
	{
		ePreviousScreen = eScreen;
		eScreen = eClockHour;
		u32TimeStamp = u32GetClockTicks();
	}
	else if(u8ModeStatus == eModeDownLongPressed || u8DownStatus == eModeDownLongPressed)
	{
		ePreviousScreen = eScreen;
		if(u16VehicleSpeed < 5)
		{
			eScreen = eDTC;	
		}	
		sScreenFunc.u8DTCMsgNumber = 0;
		u32TimeStamp = u32GetClockTicks();
	}
	else if( u8ModeStatus == eModeUpLongPressed || u8UpStatus == eModeUpLongPressed)
	{
		ePreviousScreen = eScreen;
		eScreen = eFeededPulse;
		vSetTopLinePriority(ePriority1);					
		vSetTopLineSubPriority(eNoSubPriority);
		vSetTopLineText(0);
		sScreenFunc.eFeederPulse = eFeededPulse;
		u32TimeStamp = u32GetClockTicks();
	}

	return (u32TimeStamp);
}


static void _vDisplayDispClockData()
{
	u8 temp1;
	u16 temp2;
	bool temp3;

	temp1 = u8GetClockMinutes();
	vSetDispMinutes(temp1);
	temp1 = u8GetClockHours();
	vSetDispHours(temp1);
	temp3 = bGetClockAmPmFlag();
	vSetDispAmPmFlag(temp3);

	temp1 = u8GetDispClockHours();
	vSetClockData((u16)temp1);
	temp1 = u8GetDispClockMinutes();
	temp2 = u16GetClockData();
	temp2 = temp2*100 + temp1;
	vSetClockData(temp2);
}


 static void _vDisplayMemClockData()
 {
	 u8 temp1;
	 u16 temp2;
	 bool temp3;
 
 
	 temp2 = u8GetClockHours();
	 temp1 = u8GetClockMinutes();
	 temp2 = temp2*100 + temp1;
	 vSetClockData(temp2);
 }

static void _vSaveDispClockData()
{
	u8 temp1;
	u16 temp2;
	bool temp3;

	temp1 = u8GetDispClockMinutes();
	vSetMinutes(temp1);
	temp2 = u8GetDispClockHours();
	vSetHours(temp2);
	temp3 = bGetDispClockAmPmFlag();
	vSetAmPmFlag(temp3);
	
	temp2 = temp2*100 + temp1;
	vSetClockData(temp2);
}

 /*********************************************************************//**
 *
 * LCD Screen Navigation Procedure.
 *
 * @param	   None
 *			   
 *
 * @return	   None
 *************************************************************************/
 
void vScreenTransition(void)
{	
	static u8 u8ModeStatus = 1, u8SetStatus = 1, u8UpStatus = 1, u8DownStatus = 1, u8FuelType = 0, u8temp =0;	
	static u32 u32TimeStamp = 0;
	u16 u16FeededPulse = 0;
	u32 u32ElapsedTime;
	bool InputStatus = 0;
	u8 Temp = 0;	
	u16 temp2 = 0;	
	bool temp3 = 0;
	u16 u16TotalSpns=0;
	u8 noOfDigit = 0;
	u8 noDigit = 0;
	static u16 temp1 = 0;
	u16 SpeedoPulses = 0;
	static u16 Number =0;
	u8 i =0;
	
	_vCheckTripRolloverStatus(); 

	u8ModeStatus    = eGetMODESwitchStatus();        	// Get Mode switch Status 

	u8SetStatus     = eGetSETSwitchStatus();         	// Get Set switch Status 

	u8UpStatus		= eGetUPSwitchStatus();			// Get Up switch Status

	u8DownStatus	= eGetDOWNSwitchStatus();			// Get Down switch Status

	u8FuelType 	= u8GetVehicleType();

	u16FeededPulse = u16GetFeededPulse();
	
	if(u8FuelType == FUEL_TYPE_LPH)
	{
		if(u8temp == 0)
		{
			u8temp = 1;
			sScreenFunc.u8DTCMsgNumber = eTripAScreen;
			eScreen = eHourAFETripA;
			sScreenFunc.eMode = eHourAFETripA;
			sScreenFunc.u8TripAB = NULL;
			sScreenFunc.eOdoTripReset = eNoOdoReset;
			sScreenFunc.eHourTripReset = eNoHourReset;
			sScreenFunc.u8ClockSet = NULL;
		}
	}

	if(u8FuelType == FUEL_TYPE_KMPL || u8FuelType == FUEL_TYPE_KMPKG)
	{
		if(u8temp == 0)
		{
			u8temp = 1;
			sScreenFunc.u8DTCMsgNumber = eTripAScreen;
			eScreen = eTripAScreen;
            sScreenFunc.eMode = eTripAScreen;
            sScreenFunc.u8TripAB = NULL;
			sScreenFunc.eOdoTripReset = eNoOdoReset;
			sScreenFunc.eHourTripReset = eNoHourReset;
			sScreenFunc.u8ClockSet = NULL;
		}
	}
	
		switch (eScreen)
		{
			case eTripAScreen:							// Default Screen
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;
			
				_vDisplayDispClockData();
					
				if(u8ModeStatus == eShortPressed)// && u8ModeStatus < e5sPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					eScreen = eTripBScreen;
					sScreenFunc.eMode = eTripBScreen;
				}
				else if(u8SetStatus == eLongPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					sScreenFunc.eOdoTripReset = eOdoTripAReset;
					
					if(u8FuelType != FUEL_TYPE_LPH)
						sScreenFunc.eAFETripAReset = eResetAFETripA;
					else
						sScreenFunc.eAFETripAReset = eNoAFEReset;
				}

				u32TimeStamp = _u32ChangeMainScreen(u8SetStatus, u8UpStatus, u8DownStatus, u8ModeStatus, u32TimeStamp);
				
			break;
			
			case eTripBScreen:							// Trip B Screen	
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;
			
				_vDisplayDispClockData();
				
				if(u8ModeStatus == eShortPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					eScreen = eHourAFETripA;
					sScreenFunc.eMode = eHourAFETripA;
				}
				else if(u8SetStatus == eLongPressed)
				{	
					u32TimeStamp = u32GetClockTicks();
					sScreenFunc.eOdoTripReset = eOdoTripBReset;
					if(u8FuelType != FUEL_TYPE_LPH)
						sScreenFunc.eAFETripBReset = eResetAFETripB;
					else
						sScreenFunc.eAFETripBReset = eNoAFEReset;						
				}
				u32TimeStamp = _u32ChangeMainScreen(u8SetStatus, u8UpStatus, u8DownStatus, u8ModeStatus, u32TimeStamp);
				
			break;

			
			
			case eHourAFETripA:							// Hour Trip A Screen
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;

				_vDisplayDispClockData();
			
				if(u8ModeStatus == eShortPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					eScreen = eHourAFETripB;
					sScreenFunc.eMode = eHourAFETripB;
				}
			
				else if(u8SetStatus == eLongPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					sScreenFunc.eHourTripReset = eHourTripAReset;
					if(u8FuelType == FUEL_TYPE_LPH)
						sScreenFunc.eAFETripAReset = eResetAFETripA;
					else
						sScreenFunc.eAFETripAReset = eNoAFEReset;
				}
				u32TimeStamp = _u32ChangeMainScreen(u8SetStatus, u8UpStatus, u8DownStatus, u8ModeStatus, u32TimeStamp);
				
			break;	
			case eHourAFETripB:							// Hour Trip B Screen			
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;

				_vDisplayDispClockData();
				
				if(u8ModeStatus == eShortPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					eScreen = eTripAScreen;
					sScreenFunc.eMode = eTripAScreen;
				}
			
				else if(u8SetStatus == eLongPressed)
				{
					u32TimeStamp = u32GetClockTicks();
					sScreenFunc.eHourTripReset = eHourTripBReset;
					if(u8FuelType == FUEL_TYPE_LPH)
						sScreenFunc.eAFETripBReset = eResetAFETripB;
					else
						sScreenFunc.eAFETripBReset = eNoAFEReset;
				}
				u32TimeStamp = _u32ChangeMainScreen(u8SetStatus, u8UpStatus, u8DownStatus, u8ModeStatus, u32TimeStamp);
				
			break;	

			case eClockHour:							// Clock Hour Setting Screen					
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;

				if( u8GetDispClockHours() <= 12)
				{
					if(u8UpStatus == eShortPressed)
					{				
						u32TimeStamp = u32GetClockTicks();
						if(u8GetDispClockHours() == 12)
						{
							vSetDispHours(1);
						}
						else
						{
							temp1 = u8GetDispClockHours();
							temp1++;
							vSetDispHours(temp1);
						}
					}
					
					else if(u8DownStatus == eShortPressed)
					{
						u32TimeStamp = u32GetClockTicks();
						
						if(u8GetDispClockHours() == 1)
						{
							vSetDispHours(12);
						}
						else
						{
							temp1 = u8GetDispClockHours();
							temp1--;
							vSetDispHours(temp1);
						}

					}
					
					vSetUpdateData(FALSE);	
					temp2 = u8GetDispClockHours();
					temp1 = u8GetDispClockMinutes();
					temp2 = temp2*100 + temp1;
					vSetClockData(temp2);

				}

				if(u8ModeStatus == eShortPressed)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					vSetUpdateData(FALSE);	
					u32TimeStamp = u32GetClockTicks();
					eScreen = eClockMinute;
				}
				else if((u8SetStatus == eShortPressed))
				{
					vSetSensorStatus(FALSE);
					_vSaveDispClockData();
					vSetSeconds(NULL);
					vSetUpdateData(TRUE);
					u32TimeStamp = u32GetClockTicks();
					eScreen = ePreviousScreen;
				}
				u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;
				if(u8ModeStatus == eLongPressed || u32ElapsedTime > TIME_OUT_TO_DEFAULT)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					u32TimeStamp = u32GetClockTicks();
					vSetUpdateData(FALSE);					
					_vDisplayMemClockData();
					if(u32ElapsedTime > TIME_OUT_TO_DEFAULT)	
					{	
						eScreen = ePreviousScreen;	
					}
					else	
					{	
						if(ePreviousScreen > 0) 
						{	
							eScreen = ePreviousScreen-1;
						}else 
						{
							eScreen = eHourAFETripB;
						}	
					}
				}
			break;

			case eClockMinute:							// Clock Minute Setting Screen	
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;
			
				if(u8GetDispClockMinutes() < 60)
				{
					if(u8UpStatus == eShortPressed)
					{				
						u32TimeStamp = u32GetClockTicks();
						if(u8GetDispClockMinutes() == 59)
						{
							vSetDispMinutes(NULL);
						}
						else
						{
							temp1 = u8GetDispClockMinutes();
							temp1++;
							vSetDispMinutes(temp1);
						}
					}
					if(u8DownStatus == eShortPressed)
					{
						u32TimeStamp = u32GetClockTicks();
						if(u8GetDispClockMinutes() == 0)
						{
							vSetDispMinutes(59);
						}
						else
						{
							temp1 = u8GetDispClockMinutes();
							temp1--;
							vSetDispMinutes(temp1);
						}
					}
					if(u8UpStatus == eShortPressed || u8DownStatus == eShortPressed)
					{				
						u32TimeStamp = u32GetClockTicks();
					}
					
					vSetUpdateData(FALSE);	
					temp2 = u8GetDispClockHours();
					temp1 = u8GetDispClockMinutes();
					temp2 = temp2*100 + temp1;
					vSetClockData(temp2);

				}

				if(u8ModeStatus == eShortPressed)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					vSetUpdateData(FALSE);	
					u32TimeStamp = u32GetClockTicks();
					eScreen = eClockAMPM;
				}
				else if((u8SetStatus == eShortPressed))
				{
					vSetSensorStatus(FALSE);
					_vSaveDispClockData();
					vSetSeconds(NULL);
					vSetUpdateData(TRUE);
					u32TimeStamp = u32GetClockTicks();
					eScreen = ePreviousScreen;
				}
				u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;
				if(u8ModeStatus == eLongPressed || u32ElapsedTime > TIME_OUT_TO_DEFAULT)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					u32TimeStamp = u32GetClockTicks();
					vSetUpdateData(FALSE);					
					_vDisplayMemClockData();
					if(u32ElapsedTime > TIME_OUT_TO_DEFAULT)	
					{	
						eScreen = ePreviousScreen;	
					}
					else	
					{	
						if(ePreviousScreen > 0) 
						{	
							eScreen = ePreviousScreen-1;
						}else 
						{
							eScreen = eHourAFETripB;
						}	
					}
				}
			break;

			case eClockAMPM:							// Clock AM/PM Setting Screen	
				sScreenFunc.eFeederPulse = eTripAScreen;
				sScreenFunc.u8DTCMsgNumber = eTripAScreen;
				if(u8UpStatus == eShortPressed || u8DownStatus == eShortPressed)
				{				
					u32TimeStamp = u32GetClockTicks();
					temp3 = bGetDispClockAmPmFlag();
					temp3 = ~temp3;
					vSetDispAmPmFlag(temp3);
				}
				else if(u8ModeStatus == eShortPressed)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					vSetUpdateData(FALSE);
					u32TimeStamp = u32GetClockTicks();
					eScreen = eClockHour;
				}
				else if((u8SetStatus == eShortPressed))
				{
					vSetSensorStatus(FALSE);
					_vSaveDispClockData();
					vSetSeconds(NULL);
					vSetUpdateData(TRUE);
					u32TimeStamp = u32GetClockTicks();
					eScreen = ePreviousScreen;
	               // u8temp =0;
				}
				u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;
				if(u8ModeStatus == eLongPressed || u32ElapsedTime > TIME_OUT_TO_DEFAULT)
				{
					vSetSensorStatus(FALSE);
					vResetButtonTimeStamps();
					u32TimeStamp = u32GetClockTicks();
					vSetUpdateData(FALSE);					
					_vDisplayMemClockData();
					if(u32ElapsedTime > TIME_OUT_TO_DEFAULT)	
					{	
						eScreen = ePreviousScreen;	
					}
					else	
					{	
						if(ePreviousScreen > 0) 
						{	
							eScreen = ePreviousScreen-1;
						}else 
						{
							eScreen = eHourAFETripB;
						}	
					}
				}
			break;	

		case eDTC:
			/* 
				In this Case, DTC list has to be shown according to the indexing as per user changes.
				dthe user changes the idex of the dtc lists by pressing up and down button. the range
				of the DTC number will vary from 0 to the total number of SPNs calculated.
				Now these DTC number or indices are available for LCD.c to show that particular DTC from
				the list.
			*/
		
			vSetTopLinePriority(ePriority1);					
			vSetTopLineSubPriority(eNoSubPriority);
			if(u8GetDTCStatus() != 0U)
			{
				eDtcTypes = eMultiFrames;
				u16TotalSpns = u16GetDtcTotalBytes();
				if(u16TotalSpns > 5U)
				{
					u16TotalSpns = (u16TotalSpns - 2U)/4U;
					if(u16TotalSpns > MAXIMUM_NUMBER_OF_DTC)
					{
						u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
					}
					else	
					{	
						/*Do Nothing*/	
					}
				}
				else
				{
					u16TotalSpns = 0U;
				}
			}
			else
			{
				u16TotalSpns = 0U;
				sScreenFunc.u8DTCMsgNumber = 0U;
				if(!bGetPGNTimeOut(eJDM1DTC_spn1) )
				{
					eDtcTypes = eSingleFrame;
					u16TotalSpns = 1U;
				}
				else
				{
					eDtcTypes = eNoFaults;
					sScreenFunc.u8DTCMsgNumber = 0;
					//return;
				}	
			}

			if((sScreenFunc.u8DTCMsgNumber < u16TotalSpns) && (u8ModeStatus == eReleased) && eDtcTypes != eNoFaults)
			{
				if((u8UpStatus == eShortPressed) && (u8DownStatus == eReleased))
				{
					u32TimeStamp = u32GetClockTicks();
					if(sScreenFunc.u8DTCMsgNumber == (u16TotalSpns-1U))
					{
						sScreenFunc.u8DTCMsgNumber = 0U;
					}
					else
					{	
						sScreenFunc.u8DTCMsgNumber++;	
					}
				}
				else if((u8DownStatus == eShortPressed) && (u8UpStatus == eReleased))
				{
					u32TimeStamp = u32GetClockTicks();	
					if(sScreenFunc.u8DTCMsgNumber == 0U)
					{
						sScreenFunc.u8DTCMsgNumber = (u8)(u16TotalSpns-1U);
					}
					else
					{	
						sScreenFunc.u8DTCMsgNumber--;	
					}
				}
				else
				{	
					/*do nothing*/	
				}
				
			}
			else
			{	
				/*do nothing*/
			}
			
			u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;
			if(( ( (u8ModeStatus >= eLongPressed) && (u8ModeStatus != eInvalidPress))
					&& (u8SetStatus == eReleased) 
					&& (u8UpStatus == eReleased) && (u8DownStatus == eReleased)) 
					|| (u32ElapsedTime > TIME_OUT_TO_DEFAULT))
			{
				vResetDTCParameters();
				vSetTopLinePriority(eBlank);	
				eScreen = ePreviousScreen;
			}
			else	
			{	
				/*Do Nothing*/	
			}
			break;
			
		case eFeededPulse:
			/* 
				In this Case, Feeded pulses and Service Due is enabled as per user's choice.
				The user can make his/her choices using up and down button press. The data 
				"sScreenFunc.eFeederPulse" holds the enablers of Feeded Pulse and Service Due.
			*/
	
			vSetTopLinePriority(ePriority1);					
			vSetTopLineSubPriority(eNoSubPriority);
			if((u8UpStatus == eShortPressed || u8DownStatus == eShortPressed))
			{
				u32TimeStamp = u32GetClockTicks();
				if(sScreenFunc.eFeederPulse == eFeededPulse)
				{
					sScreenFunc.eFeederPulse = eSoftwareVersion;						
					vSetTopLineText(1);
				}
				else if(sScreenFunc.eFeederPulse == eSoftwareVersion)
				{
					sScreenFunc.eFeederPulse = eFeededPulse;
					vSetTopLineText(0);
				
				}
				else
				{
				//sScreenFunc.eFeederPulse = eFeededPulse;
					/*Do Nothing*/	
				}
			}
			if(u8ModeStatus == eShortPressed)
				{
				eScreen = eSetPulse;
				vSetTopLineText(3);
				SpeedoPulses = u16GetFeededPulse();
				vSetDispPulse(SpeedoPulses);
				}
		
			else
				{
			
				}
						
			
			u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;
			if((( ((u8ModeStatus >= eLongPressed) && (u8ModeStatus != eInvalidPress))
					&& (u8SetStatus == eReleased) 
					&& (u8UpStatus == eReleased) && (u8DownStatus == eReleased)) 
					|| (u32ElapsedTime > TIME_OUT_TO_DEFAULT)))
			{
				vSetTopLinePriority(eBlank);					
				vSetTopLineSubPriority(eNoSubPriority);
				eScreen = ePreviousScreen;
			}
			else
			{
				/*Do Nothing*/	
			}
			break;
			case eSetPulse:
				sScreenFunc.eFeederPulse = eSetPulse;
				sScreenSelect.eFeederPulse = eSetPulse;
		
			    SpeedoPulses = u16GetPulseSet();
				  for(i =0;i<5;i++)
				  {
				  	pulses[i]=SpeedoPulses%10;
					SpeedoPulses = SpeedoPulses/10;
				  }
					if(u8ModeStatus == eShortPressed)
					{
						eScreen = eSetFourthDigit;
					}					
					else if(u8SetStatus == eShortPressed )
					{
    					eScreen = eFeededPulse;
						 SpeedoPulses = u16GetPulseSet();
    					 if(SpeedoPulses >= 1000 && SpeedoPulses <= 60000)
							{
								u8VsPulsPerRev[0] = SpeedoPulses >> 8;
								u8VsPulsPerRev[1] = SpeedoPulses & 0x00FF; ;
								vSaveEolData(eVsPulsPerRevBytes);
								sSpeedSensor.u16PulsesPerKm = SpeedoPulses;
							}
    					u32TimeStamp = u32GetClockTicks();
						sScreenFunc.eFeederPulse = eFeededPulse;
						vSetTopLineText(0);
					}					
					else if(u8UpStatus == eShortPressed)
					{			
						
						if(++pulses[0] > 9)
							pulses[0] =0;
						
						
					}
					
					else if(u8DownStatus == eShortPressed)
					{
						
						if(pulses[0] <= 0)
							pulses[0] = 10;
						pulses[0]--;
						
					}
					SpeedoPulses = pulses[4]*10000+pulses[3]*1000+pulses[2]*100+pulses[1]*10+pulses[0];
					
					vSetDispPulse(SpeedoPulses);
				

			break;
			case eSetFourthDigit:
				  SpeedoPulses = u16GetPulseSet();
				  for(i =0;i<5;i++)
				  {
				  	pulses[i]=SpeedoPulses%10;
					SpeedoPulses = SpeedoPulses/10;
				  }
				 
				sScreenSelect.eFeederPulse = eSetFourthDigit;
				if(u8ModeStatus == eShortPressed)
					{
						eScreen = eSetThirdDigit;
					}
				else if(u8SetStatus == eShortPressed )
					{
    					eScreen = eFeededPulse;
						 SpeedoPulses = u16GetPulseSet();
    					 if(SpeedoPulses >= 1000 && SpeedoPulses <= 60000)
							{
								u8VsPulsPerRev[0] = SpeedoPulses >> 8;
								u8VsPulsPerRev[1] = SpeedoPulses & 0x00FF; ;
								vSaveEolData(eVsPulsPerRevBytes);
								sSpeedSensor.u16PulsesPerKm = SpeedoPulses;
							}
    					u32TimeStamp = u32GetClockTicks();
						sScreenFunc.eFeederPulse = eFeededPulse;
						vSetTopLineText(0);
					}					
					else if(u8UpStatus == eShortPressed)
					{			
						
						if(++pulses[1] > 9)
							pulses[1] =0;
						
						
					}
					
					else if(u8DownStatus == eShortPressed)
					{
						
						if(pulses[1] <= 0)
							pulses[1] = 10;
						pulses[1]--;
						
					}
					SpeedoPulses = pulses[4]*10000+pulses[3]*1000+pulses[2]*100+pulses[1]*10+pulses[0];
					
					vSetDispPulse(SpeedoPulses);	
				
			break;
			case eSetThirdDigit:

			      SpeedoPulses = u16GetPulseSet();
				  for(i =0;i<5;i++)
				  {
				  	pulses[i]=SpeedoPulses%10;
					SpeedoPulses = SpeedoPulses/10;
				  }
				  
				sScreenSelect.eFeederPulse = eSetThirdDigit;
				if(u8ModeStatus == eShortPressed)
					{
						eScreen = eSetSecondDigit;
					}
				else if(u8SetStatus == eShortPressed )
					{
    					eScreen = eFeededPulse;
						 SpeedoPulses = u16GetPulseSet();
    					 if(SpeedoPulses >= 1000 && SpeedoPulses <= 60000)
							{
								u8VsPulsPerRev[0] = SpeedoPulses >> 8;
								u8VsPulsPerRev[1] = SpeedoPulses & 0x00FF; ;
								vSaveEolData(eVsPulsPerRevBytes);
								sSpeedSensor.u16PulsesPerKm = SpeedoPulses;
							}
    					u32TimeStamp = u32GetClockTicks();
						sScreenFunc.eFeederPulse = eFeededPulse;
						vSetTopLineText(0);
					}					
					else if(u8UpStatus == eShortPressed)
					{			
						
						if(++pulses[2] > 9)
							pulses[2] =0;
						
						
					}
					
					else if(u8DownStatus == eShortPressed)
					{
						
						if(pulses[2] <= 0)
							pulses[2] = 10;
						pulses[2]--;
						
					}
					SpeedoPulses = pulses[4]*10000+pulses[3]*1000+pulses[2]*100+pulses[1]*10+pulses[0];
					
					vSetDispPulse(SpeedoPulses);
				
			break;
			case eSetSecondDigit:
				
				 SpeedoPulses = u16GetPulseSet();
				  for(i =0;i<5;i++)
				  {
				  	pulses[i]=SpeedoPulses%10;
					SpeedoPulses = SpeedoPulses/10;
				  }
				  
				sScreenSelect.eFeederPulse = eSetSecondDigit;
				if(u8ModeStatus == eShortPressed)
					{
						eScreen = eSetFirstDigit;
					}
				else if(u8SetStatus == eShortPressed )
					{
    					eScreen = eFeededPulse;
						 SpeedoPulses = u16GetPulseSet();
    					 if(SpeedoPulses >= 1000 && SpeedoPulses <= 60000)
							{
								u8VsPulsPerRev[0] = SpeedoPulses >> 8;
								u8VsPulsPerRev[1] = SpeedoPulses & 0x00FF; ;
								vSaveEolData(eVsPulsPerRevBytes);
								sSpeedSensor.u16PulsesPerKm = SpeedoPulses;
							}
    					u32TimeStamp = u32GetClockTicks();
						sScreenFunc.eFeederPulse = eFeededPulse;
						vSetTopLineText(0);
					}					
					else if(u8UpStatus == eShortPressed)
					{			
						
						if(++pulses[3] > 9)
							pulses[3] =0;
						
						
					}
					
					else if(u8DownStatus == eShortPressed)
					{
						
						if(pulses[3] <= 0)
							pulses[3] = 10;
						pulses[3]--;
						
					}
					
					if(pulses[4]> 5 && pulses[3] >4)
					{
					     pulses[3] = 4;
					}
					SpeedoPulses = pulses[4]*10000+pulses[3]*1000+pulses[2]*100+pulses[1]*10+pulses[0];
					
					vSetDispPulse(SpeedoPulses);
				
			break;
			case eSetFirstDigit:
				SpeedoPulses = u16GetPulseSet();
				  for(i =0;i<5;i++)
				  {
				  	pulses[i]=SpeedoPulses%10;
					SpeedoPulses = SpeedoPulses/10;
				  }
				sScreenSelect.eFeederPulse = eSetFirstDigit;
				if(u8ModeStatus == eShortPressed)
					{
						eScreen = eSetPulse;
					}
				else if(u8SetStatus == eShortPressed )
						{
							eScreen = eFeededPulse;
							 SpeedoPulses = u16GetPulseSet();
							 if(SpeedoPulses >= 1000 && SpeedoPulses <= 60000)
							{
								u8VsPulsPerRev[0] = SpeedoPulses >> 8;
								u8VsPulsPerRev[1] = SpeedoPulses & 0x00FF; ;
								vSaveEolData(eVsPulsPerRevBytes);
								sSpeedSensor.u16PulsesPerKm = SpeedoPulses;
							}
							u32TimeStamp = u32GetClockTicks();
							sScreenFunc.eFeederPulse = eFeededPulse;
							vSetTopLineText(0);
						}					
						else if(u8UpStatus == eShortPressed)
						{			
							
							if(++pulses[4] > 6)
								pulses[4] =0;
							
							
						}
						
						else if(u8DownStatus == eShortPressed)
						{
							
							if(pulses[4] <= 0)
								pulses[4] = 7;
							pulses[4]--;
							
						}
				
						
						 if(pulses[4]> 5 && pulses[3]>1)
						   pulses[4] = 5;
                                                 if(pulses[4] == 0 && pulses[3]== 0)
						   {
						    pulses[3] = 1;
						    pulses[2] = 0;
						    pulses[1] = 0;
						    pulses[0] = 0;
						   }
      						SpeedoPulses = pulses[4]*10000+pulses[3]*1000+pulses[2]*100+pulses[1]*10+pulses[0];
						
						vSetDispPulse(SpeedoPulses);
				
				
			break;
			case eIllumination:

			//Get Illumination Input status
			InputStatus = bGetIlluminationInputStatus();
			
			if(InputStatus == TRUE)
			{	
				Temp = u8GetNightIlluValue();
				if(u8UpStatus == eShortPressed)
				{	
					if(Temp < eILLBAR5)
					{
						Temp++;
						vSetNightIlluValue(Temp);
					}
					u32TimeStamp = u32GetClockTicks();				
				}				
				else if(u8DownStatus == eShortPressed)
				{
					if(Temp > eILLBAR0)
					{
						Temp--;
						vSetNightIlluValue(Temp);
					}
					u32TimeStamp = u32GetClockTicks();
				}
			}
			else
			{	
			  Temp =  u8GetDayLightIlluValue();
			  
				if(u8UpStatus == eShortPressed)
				{	
					if(Temp < eILLBAR5)
					{
						Temp++;
						vSetDayLightIlluValue(Temp);
					}
					u32TimeStamp = u32GetClockTicks();				
				}				
				else if(u8DownStatus == eShortPressed)
				{
					if(Temp > eILLBAR0)
					{
						Temp--;
						vSetDayLightIlluValue(Temp);
					}
					u32TimeStamp = u32GetClockTicks();
				}
				
			}

			u32ElapsedTime = u32GetClockTicks() - u32TimeStamp;

			if(u32ElapsedTime > TIME_OUT_SAVE_CLOCK)
			{
				vSetSensorStatus(FALSE);  
				vResetButtonTimeStamps();
				sScreenFunc.u8DTCMsgNumber = 0;
				vSetUpdateData(FALSE);
				u32TimeStamp = u32GetClockTicks();
				eScreen = ePreviousScreen;
               // u8temp =0;
			}
			break;

		}
		
}


/*********************************************************************//**
*
* Reset All LCD screen state.
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
void vResetScreen()
{
	sScreenFunc.eMode = eTripAScreen;
	sScreenFunc.u8TripAB = NULL;
	sScreenFunc.eOdoTripReset = eNoOdoReset;
	sScreenFunc.eHourTripReset = eNoHourReset;
	sScreenFunc.u8ClockSet = NULL;
	sScreenFunc.u8DTCMsgNumber = eTripAScreen;
}


/*********************************************************************//**
*
* Get LCD screen State.
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
SCREEN_SET  GetLcdScreenStatus()
{
	return eScreen;
}


SCREEN_SET u8GetScreenMode()
{
	return sScreenFunc.eMode;
}
u8 u8GetTripType()
{
	return sScreenFunc.u8TripAB;
}
ODO_RESET u8GetOdoTripRstStatus()
{
	return sScreenFunc.eOdoTripReset;
}
HOUR_RESET u8GetHourTripRstStatus()
{
	return sScreenFunc.eHourTripReset;
}
AFE_RESET u8GetAfeRstStatusA()
{
	return sScreenFunc.eAFETripAReset;
}

AFE_RESET u8GetAfeRstStatusB()
{
	return sScreenFunc.eAFETripBReset;
}

u8 u8GetClockSetStatus()
{
	return sScreenFunc.u8ClockSet;
}
SCREEN_SET u8GetFeederPulseStatus()
{
	return sScreenFunc.eFeederPulse;
}
SCREEN_SET u8GetFeederPulseDigitStatus()
{
	return sScreenSelect.eFeederPulse;
}

u8 u8GetDTCMsgNumber()
{
	return sScreenFunc.u8DTCMsgNumber;
}

bool bGetDoSelfTestStatus()
{
	return sScreenFunc.bDoSelfTest;
}
bool bGetSendTripResetStatus()
{
	return sScreenFunc.bSendTripResetStatus;
}


void vSetScreenMode(SCREEN_SET data)
{
	sScreenFunc.eMode = data;
}
void vSetTripType(u8 data)
{
	sScreenFunc.u8TripAB = data;
}
void vSetOdoTripRstStatus(ODO_RESET data)
{
	sScreenFunc.eOdoTripReset = data;
}
void vSetHourTripRstStatus(HOUR_RESET data)
{
	sScreenFunc.eHourTripReset = data;
}
void vSetAfeRstAStatus(AFE_RESET data)
{
	sScreenFunc.eAFETripAReset = data;
}
void vSetAfeRstBStatus(AFE_RESET data)
{
	sScreenFunc.eAFETripBReset = data;
}


void vSetClockSetStatus(u8 data)
{
	sScreenFunc.u8ClockSet = data;
}
void vSetFeederPulseStatus(SCREEN_SET data)
{
	sScreenFunc.eFeederPulse = data;
}
void vSetDTCMsgNumber(u8 data)
{
	sScreenFunc.u8DTCMsgNumber = data;
}
void vSetDoSelfTestStatus(bool data)
{
	sScreenFunc.bDoSelfTest = data;
}
void vSetSendTripResetStatus(bool data)
{
	sScreenFunc.bSendTripResetStatus = data;
}

DTC_TYPES eGetDtcType(void)
{
	return eDtcTypes;
}



void vSetTripAOdoResetReq(void)
{

	static u8 u8FuelType = 0;

	u8FuelType	= u8GetVehicleType();

	sScreenFunc.eOdoTripReset = eOdoTripAReset;
	
	if(u8FuelType != FUEL_TYPE_LPH)
		sScreenFunc.eAFETripAReset = eResetAFETripA;
	else
		sScreenFunc.eAFETripAReset = eNoAFEReset;
}

void vSetTripBOdoResetReq(void)
{

	static u8 u8FuelType = 0;

	u8FuelType	= u8GetVehicleType();

	sScreenFunc.eOdoTripReset = eOdoTripBReset;
		if(u8FuelType != FUEL_TYPE_LPH)
			sScreenFunc.eAFETripBReset = eResetAFETripB;
		else
			sScreenFunc.eAFETripBReset = eNoAFEReset;	

}

void vSetTripAHrResetReq(void)
{

	static u8 u8FuelType = 0;

	u8FuelType	= u8GetVehicleType();

	sScreenFunc.eHourTripReset = eHourTripAReset;
	if(u8FuelType == FUEL_TYPE_LPH)
		sScreenFunc.eAFETripAReset = eResetAFETripA;
	else
		sScreenFunc.eAFETripAReset = eNoAFEReset;

}

void vSetTripBHrResetReq(void)
{

	static u8 u8FuelType = 0;

	u8FuelType	= u8GetVehicleType();

	sScreenFunc.eHourTripReset = eHourTripBReset;
	if(u8FuelType == FUEL_TYPE_LPH)
		sScreenFunc.eAFETripBReset = eResetAFETripB;
	else
		sScreenFunc.eAFETripBReset = eNoAFEReset;
	

}

void _vCheckTripRolloverStatus(void)
{
	bool OdoTripARollOverStatus = FALSE;
	bool OdoTripBRollOverStatus = FALSE;
	bool HourTripARollOverStatus = FALSE;
	bool HourTripBRollOverStatus = FALSE;
	u8 FuelType = 0;
	
	FuelType 	= u8GetVehicleType();
	OdoTripARollOverStatus = bGetOdoTripARolloverStatus();
	OdoTripBRollOverStatus = bGetOdoTripBRolloverStatus();
	HourTripARollOverStatus = bGetHourTripARolloverStatus();
	HourTripBRollOverStatus = bGetHourTripBRolloverStatus();
	
	if(OdoTripARollOverStatus == TRUE)
	{
		sScreenFunc.eOdoTripReset = eOdoTripAReset;
		if(FuelType != FUEL_TYPE_LPH)
		{
			sScreenFunc.eAFETripAReset = eResetAFETripA;
		}
		else
		{
			sScreenFunc.eAFETripAReset = eNoAFEReset;
		}
		vClearOdoTripARolloverStatus();
	}
	
	if(OdoTripBRollOverStatus== TRUE)
	{
		sScreenFunc.eOdoTripReset = eOdoTripBReset;
		if(FuelType != FUEL_TYPE_LPH)
		{
			sScreenFunc.eAFETripBReset = eResetAFETripB;
		}
		else
		{
			sScreenFunc.eAFETripBReset = eNoAFEReset;
		}
		vClearOdoTripBRolloverStatus();
	}
	
	if(HourTripARollOverStatus == TRUE)
	{
		sScreenFunc.eHourTripReset = eHourTripAReset;
		if(FuelType == FUEL_TYPE_LPH)
		{
			sScreenFunc.eAFETripAReset = eResetAFETripA;
		}
		else
		{
			sScreenFunc.eAFETripAReset = eNoAFEReset;
		}
		vClearHourTripARolloverStatus();
	}
	
	if(HourTripBRollOverStatus== TRUE)
	{
		sScreenFunc.eHourTripReset = eHourTripBReset;
		if(FuelType == FUEL_TYPE_LPH)
		{
			sScreenFunc.eAFETripBReset = eResetAFETripB;
		}
		else
		{
			sScreenFunc.eAFETripBReset = eNoAFEReset;
		}
		vClearHourTripBRolloverStatus();
	}

}
u16 u16GetPulseSet()
{
	return sPulseData.u16PulseData;
}
void vSetDispPulse(u16 data)
{
	sPulseData.u16PulseData = data;
}
u8 u8GetNoOfDigit(u16 data)
{
u8 Temp = 0;
u8 Temp1 = 0;
while(data > 0) 
		{
			Temp = data % 10;  
		
			data = data / 10;  
			Temp1++;
		}

return Temp1;

}
u8 u8GetDigit(u16 data)

{
	u8 Temp = 0;
	u8 Temp1 = 0;
		while(data > 0) 
			{
				Temp = data % 10;  
			
				data = data / 10;	 
			}
	
	return Temp;

}
