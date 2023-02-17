#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "Event.h"
#include "Clock.h"
#include "LCDScreens.h"

CLOCK_DATA_TYPE sClock = {0,0,0,0,0,0,0,FALSE,FALSE,FALSE,TRUE};					// declared in event.h

/*********************************************************************//**
 *
 * Prepare clock data for display if minute update 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vClockUpdate()
{
	static u8 TempMinute = 0xFF; // Keep TempMinute > 59 so that after CPU reset it could prepare clock data
	static u8 u8ScreenStatus = 0;

	u8ScreenStatus = GetLcdScreenStatus();
	
	if(sClock.u8Minute != TempMinute && (u8ScreenStatus < eClockHour || u8ScreenStatus > eClockAMPM))
	{	
		TempMinute = sClock.u8Minute;
		sClock.u16ClockData = sClock.u8Hour;
		sClock.u16ClockData = (sClock.u16ClockData*100) + sClock.u8Minute;
		sClock.bUpdateData = TRUE;
	}
  
}

/*********************************************************************//**
 *
 * Get Clock Data
 *
 * @param      None    
 *
 * @return     Clock Data
 *************************************************************************/

u16 u16GetClockData()
{
	return sClock.u16ClockData;
}

/*********************************************************************//**
 *
 * Get Hours Data
 *
 * @param      None    
 *
 * @return     Hours Data
 *************************************************************************/
u8 u8GetClockHours()
{
	return sClock.u8Hour;
}

/*********************************************************************//**
 *
 * Get Minutes Data
 *
 * @param      None    
 *
 * @return     Minutes Data
 *************************************************************************/

u8 u8GetClockMinutes()
{
	return sClock.u8Minute;
}

/*********************************************************************//**
 *
 * Get Display Hours Data
 *
 * @param      None    
 *
 * @return     Display Hours Data
 *************************************************************************/

u8 u8GetDispClockHours()
{
	return sClock.u8DispHour;
}

/*********************************************************************//**
 *
 * Get Display Minutes Data
 *
 * @param      None    
 *
 * @return     Display Minutes Data
 *************************************************************************/

u8 u8GetDispClockMinutes()
{
	return sClock.u8DispMinute;
}

/*********************************************************************//**
 *
 * Get Seconds Data
 *
 * @param      None    
 *
 * @return     Seconds Data
 *************************************************************************/

u8 u8GetClockSeconds()
{
	return sClock.u8Second;
}

/*********************************************************************//**
 *
 * Get Seconds on Power ON
 *
 * @param      None    
 *
 * @return     Seconds on Power ON
 *************************************************************************/

u8 u8GetClockPwrOnSecond()
{
	return sClock.u8PwrOnSecond;
}

/*********************************************************************//**
 *
 * Get Display AM/PM flag
 *
 * @param      None    
 *
 * @return     Display AM/PM flag
 *************************************************************************/

bool bGetDispClockAmPmFlag()
{
	return sClock.bDispAmPmFlag;
}

/*********************************************************************//**
 *
 * Get AM/PM flag
 *
 * @param      None    
 *
 * @return     AM/PM flag
 *************************************************************************/

bool bGetClockAmPmFlag()
{
	return sClock.bAmPmFlag;
}

/*********************************************************************//**
 *
 * Get Update Data Flag
 *
 * @param      None    
 *
 * @return     Update Data Flag
 *************************************************************************/

bool bGetClockUpdateData()
{
	return sClock.bUpdateData;
}

/*********************************************************************//**
 *
 * Get Count Flag at Power ON
 *
 * @param      None    
 *
 * @return     Count Flag at Power ON
 *************************************************************************/

bool bGetClockPowerONCountFlag()
{
	return sClock.bPowerONCountFlag;
}

/*********************************************************************//**
 *
 * Set Clock Data
 *
 * @param      Clock Data    
 *
 * @return     None
 *************************************************************************/

void vSetClockData(u16 data)
{
	sClock.u16ClockData = data;
}

/*********************************************************************//**
 *
 * Set Hour Data
 *
 * @param      Hours Data    
 *
 * @return     None
 *************************************************************************/

void vSetHours(u8 data)
{
	sClock.u8Hour = data;
}

/*********************************************************************//**
 *
 * Set Minutes Data
 *
 * @param      Minutes Data    
 *
 * @return     None
 *************************************************************************/

void vSetMinutes(u8 data)
{
	sClock.u8Minute = data;
}

/*********************************************************************//**
 *
 * Set Display Hour Data
 *
 * @param      Display Hour Data  
 *
 * @return     None
 *************************************************************************/

void vSetDispHours(u8 data)
{
	sClock.u8DispHour = data;
}

/*********************************************************************//**
 *
 * Set Display Minutes Data
 *
 * @param      Display Minutes Data    
 *
 * @return     None
 *************************************************************************/

void vSetDispMinutes(u8 data)
{
	sClock.u8DispMinute = data;
}

/*********************************************************************//**
 *
 * Set Seconds
 *
 * @param      Seconds    
 *
 * @return     None
 *************************************************************************/

void vSetSeconds(u8 data)
{
	sClock.u8Second = data;
}

/*********************************************************************//**
 *
 * Set Seconds at Power ON
 *
 * @param      Seconds at Power ON    
 *
 * @return     None

*************************************************************************/
void vSetPwrOnSecond(u8 data)
{
	sClock.u8PwrOnSecond = data;
}

/*********************************************************************//**
 *
 * Set Display AM/PM Flag
 *
 * @param      Display AM/PM   
 *
 * @return     None
 *************************************************************************/

void vSetDispAmPmFlag(bool data)
{
	sClock.bDispAmPmFlag = data;
}

/*********************************************************************//**
 *
 * Set AM/PM Flag
 *
 * @param      AM/PM Flag    
 *
 * @return     None
 *************************************************************************/

void vSetAmPmFlag(bool data)
{
	sClock.bAmPmFlag = data;
}

/*********************************************************************//**
 *
 * Set Update Data Flag
 *
 * @param      Update Data Flag    
 *
 * @return     None
 *************************************************************************/

void vSetUpdateData(bool data)
{
	sClock.bUpdateData = data;
}

/*********************************************************************//**
 *
 * Set Count Flag at Power ON
 *
 * @param      Count Flag at Power ON   
 *
 * @return     None
 *************************************************************************/

void vSetPowerONCountFlag(bool data)
{
	sClock.bPowerONCountFlag = data;
}



