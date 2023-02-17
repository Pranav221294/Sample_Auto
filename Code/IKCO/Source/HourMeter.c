#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "r_cg_userdefine.h"
#include "Afe.h" 
#include "Eol.h" 
#include "J1939.h"
#include "r_cg_timer.h"
#include "Event.h" 
#include "HourMeter.h" 
#include "Tacho.h"


#define MAX_THRESH_RPM				150
#define MIN_THRESH_RPM				100
#define	SIX_MINUTES					6

HOUR_DATA_TYPE sHourMeter = {0,0,0,0,0,0,0,0,0,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};


/*********************************************************************//**
 *
 * Enable interrupts and initialize varibale and 
 * check power on reset status before doing stepper self test
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vStart_Hour_Meter(void)
{
    
    u16 u16EngineSpeed = 0;
	u16EngineSpeed = u16GetEngineSpeed();
	
    if(u16EngineSpeed >= MAX_THRESH_RPM)
    {
        sHourMeter.bHourMeterEnable = TRUE;
    }
    else if(u16EngineSpeed < MIN_THRESH_RPM)
    {
        sHourMeter.bHourMeterEnable = FALSE;
    }
}

/*********************************************************************//**
 *
 * Get Hour Meter Start Status
 *
 * @param      None    
 *
 * @return     Start Status of Hour Meter
 *************************************************************************/

bool bGetHourMeterEnableStatus(void)
{
	return (sHourMeter.bHourMeterEnable);
}

/*********************************************************************//**
 *
 * Set Hour Meter Start Status
 *
 * @param      Hour Meter Enable Status    
 *
 * @return     none
 *************************************************************************/

void vSetHourMeterEnableStatus(bool bStatus)
{
	sHourMeter.bHourMeterEnable = bStatus;
}

/*********************************************************************//**
 *
 * Get Total Engine Hour
 *
 * @param      None    
 *
 * @return     Total Engine Running Hours
 *************************************************************************/

u32 u32GetHoursValue(void)
{
   
	return (sHourMeter.u32HourMeter);
}


/*********************************************************************//**
 *
 * Get  Engine Hour Fraction Value
 *
 * @param      None    
 *
 * @return      Engine Running Hours Fraction Value
 *************************************************************************/

u8 u8GetHoursFractionValue(void)
{
   
	return (sHourMeter.u8HrFractionValue);
}


/*********************************************************************//**
 *
 * Set  Engine Hour
 *
 * @param      Engine Hour value    
 *
 * @return     None
 *************************************************************************/

void vSetHoursValue(u32 value)
{
   
	sHourMeter.u32HourMeter = value;
}


/*********************************************************************//**
 *
 * Set  Engine Hour Fraction Value
 *
 * @param      Engine Running Hours Fraction Value    
 *
 * @return    None  
 *************************************************************************/

void vSetHoursFractionValue(u8 value)
{
   
	sHourMeter.u8HrFractionValue = value;
}


/*********************************************************************//**
 *
 * Set  Engine Hour counter Value
 *
 * @param      Engine Running Hours counter Value    
 *
 * @return    None  
 *************************************************************************/

void vSetHoursCounterValue(u16 value)
{
   
	sHourMeter.u16HrMtrSeconds = value;
}


/*********************************************************************//**
 *
 * Get  Engine Hour counter Value
 *
 * @param      None
 *
 * @return    Engine Running Hours counter Value      
 *************************************************************************/

u16 u16GetHoursCounterValue()
{
   
	return sHourMeter.u16HrMtrSeconds;
}



/*********************************************************************//**
 *
 * Get  Engine Hour Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour Upgate flag status
 *************************************************************************/

bool bGetHoursUpdateStatus(void)
{
   
	return (sHourMeter.bUpdateHourMtr);
}

/*********************************************************************//**
 *
 * Get  Engine Hour Fraction Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour Fraction Upgate flag status
 *************************************************************************/

bool bGetHoursFractionUpdateStatus(void)
{
   
	return (sHourMeter.bFrUpdateHourMtr);
}


/*********************************************************************//**
 *
 * Set Engine Hour Upgate flag status
 *
 * @param      update status    
 *
 * @return      None
 *************************************************************************/

void vSetHoursUpdateStatus(bool status)
{
   
	sHourMeter.bUpdateHourMtr = status;
}

/*********************************************************************//**
 *
 * Set  Engine Hour Fraction Upgate flag status
 *
 * @param      update status  
 *
 * @return      None
 *************************************************************************/

void vSetHoursFractionUpdateStatus(bool status)
{
   
	sHourMeter.bFrUpdateHourMtr = status; 
}


/*********************************************************************//**
 *
 * Get  Engine Hour TripA value
 *
 * @param      None    
 *
 * @return     Engine Running Hours Trip A value
 *************************************************************************/

u32 u32GetHoursTripAValue(void)
{
   
	return (sHourMeter.u32HourTripA);
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripA Fraction value
 *
 * @param      None    
 *
 * @return     Engine Running Hours Trip A Fraction value
 *************************************************************************/

u8 u8GetHoursTripAFractionValue()
{
   
	return (sHourMeter.u8HrTripAFractionValue);
}


/*********************************************************************//**
 *
 * Set  Engine Hour TripA counter Value
 *
 * @param      Engine Running Hours TripA counter Value    
 *
 * @return    None  
 *************************************************************************/

void vSetHoursTripACounterValue(u16 value)
{
   
	sHourMeter.u16HrTripASeconds = value;
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripA counter Value
 *
 * @param       None
 *
 * @return    Engine Running Hours TripA counter Value     
 *************************************************************************/

u16 u16GetHoursTripACounterValue()
{
   
	return sHourMeter.u16HrTripASeconds;
}



/*********************************************************************//**
 *
 * Set  Engine Hour TripA value
 *
 * @param      Engine Running Hours Trip A value    
 *
 * @return     None
 *************************************************************************/

void vSetHoursTripAValue(u32 value)
{
   
	sHourMeter.u32HourTripA = value;
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripA Fraction value
 *
 * @param      Engine Running Hours Trip A Fraction value    
 *
 * @return     	None
 *************************************************************************/

void vSetHoursTripAFractionValue(u8 value)
{
   
	sHourMeter.u8HrTripAFractionValue = value;
}

/*********************************************************************//**
 *
 * Get  Engine Hour Trip A Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour Trip A Upgate flag status
 *************************************************************************/

bool bGetHoursTripAUpdateStatus(void)
{
   
	return (sHourMeter.bUpdateHourTripA);
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripA Fraction Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour TripA Fraction Upgate flag status
 *************************************************************************/

bool bGetHoursTripAFractionUpdateStatus(void)
{
   
	return (sHourMeter.bFrUpdateHourTripA);
}


/*********************************************************************//**
 *
 * Set Engine Hour TripA Upgate flag status
 *
 * @param      update status    
 *
 * @return      None
 *************************************************************************/

void vSetHoursTripAUpdateStatus(bool status)
{
   
	sHourMeter.bUpdateHourTripA = status;
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripA Fraction Upgate flag status
 *
 * @param      update status  
 *
 * @return      None
 *************************************************************************/

void vSetHoursTripAFractionUpdateStatus(bool status)
{
   
	sHourMeter.bFrUpdateHourTripA = status; 
}


/*********************************************************************//**
 *
 * Get  Engine Hour TripB value
 *
 * @param      None    
 *
 * @return     Engine Running Hours Trip B value
 *************************************************************************/

u32 u32GetHoursTripBValue(void)
{
   
	return (sHourMeter.u32HourTripB);
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripB Fraction value
 *
 * @param      None    
 *
 * @return     Engine Running Hours Trip B Fraction value
 *************************************************************************/

u8 u8GetHoursTripBFractionValue(void)
{
   
	return (sHourMeter.u8HrTripBFractionValue);
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripB value
 *
 * @param      Engine Running Hours Trip B value    
 *
 * @return     None
 *************************************************************************/

void vSetHoursTripBValue(u32 value)
{
   
	sHourMeter.u32HourTripB = value;
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripB Fraction value
 *
 * @param      Engine Running Hours Trip B Fraction value    
 *
 * @return     	None
 *************************************************************************/

void vSetHoursTripBFractionValue(u8 value)
{
   
	sHourMeter.u8HrTripBFractionValue = value;
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripB counter Value
 *
 * @param      Engine Running Hours TripB counter Value    
 *
 * @return    None  
 *************************************************************************/

void vSetHoursTripBCounterValue(u16 value)
{
   
	sHourMeter.u16HrTripBSeconds = value;
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripB counter Value
 *
 * @param       None
 *
 * @return    Engine Running Hours TripB counter Value     
 *************************************************************************/

u16 u16GetHoursTripBCounterValue()
{
   
	return sHourMeter.u16HrTripBSeconds;
}




/*********************************************************************//**
 *
 * Get  Engine Hour Trip B Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour Trip B Upgate flag status
 *************************************************************************/

bool bGetHoursTripBUpdateStatus(void)
{
   
	return (sHourMeter.bUpdateHourTripB);
}

/*********************************************************************//**
 *
 * Get  Engine Hour TripB Fraction Upgate flag status
 *
 * @param      None    
 *
 * @return      Engine Hour TripB Fraction Upgate flag status
 *************************************************************************/

bool bGetHoursTripBFractionUpdateStatus(void)
{
   
	return (sHourMeter.bFrUpdateHourTripB);
}


/*********************************************************************//**
 *
 * Set Engine Hour TripB Upgate flag status
 *
 * @param      update status    
 *
 * @return      None
 *************************************************************************/

void vSetHoursTripBUpdateStatus(bool status)
{
   
	sHourMeter.bUpdateHourTripB = status;
}

/*********************************************************************//**
 *
 * Set  Engine Hour TripB Fraction Upgate flag status
 *
 * @param      update status  
 *
 * @return      None
 *************************************************************************/

void vSetHoursTripBFractionUpdateStatus(bool status)
{
   
	sHourMeter.bFrUpdateHourTripB = status; 
}




/*********************************************************************//**
 *
 * Get Total Engine Hour TRIP A
 *
 * @param      None    
 *
 * @return     Total Engine Running Hours
 *************************************************************************/

u32 u32GetTotalTripASeconds(void)
{
	static u32 u32TotalSeconds = 0;

	u32TotalSeconds = (u32)(sHourMeter.u8HrTripAFractionValue * 360) + (u32)sHourMeter.u16HrTripASeconds;
	u32TotalSeconds = sHourMeter.u32HourTripA * 3600 + u32TotalSeconds;

	return (u32TotalSeconds);
}

/*********************************************************************//**
 *
 * Get Total Engine Hour TRIP B
 *
 * @param      None    
 *
 * @return     Total Engine Running Hours
 *************************************************************************/

u32 u32GetTotalTripBSeconds(void)
{
	static u32 u32TotalSeconds = 0;

	u32TotalSeconds = (u32)(sHourMeter.u8HrTripBFractionValue * 360) + (u32)sHourMeter.u16HrTripBSeconds;
	u32TotalSeconds = sHourMeter.u32HourTripB * 3600 + u32TotalSeconds;

	return (u32TotalSeconds);
}




