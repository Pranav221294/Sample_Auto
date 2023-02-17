
#include "r_cg_macrodriver.h"
#include "DiscreteSwitches.h"
#include "r_cg_userdefine.h"
#include "customTypedef.h"

// MACRO Declarations...........................................
	// INPUT BUTTON TIME	

#define DEBOUNCE_TIME				50		

#define TIME_100MS					100         // 100 milisecs
#define TIME_2S						2000		// 2000 milisecs
#define TIME_3S						3000        // 3000 milisecs
#define TIME_5S				 		5000        // 5000 milisecs
#define TIME_10S				 	10000        // 5000 milisecs


#define	SHORT_PRESS_MIN_TIME		(u8)TIME_100MS
#define	SHORT_PRESS_MAX_TIME		(u16)TIME_2S

#define	LONG_PRESS_MIN_TIME			(u16)TIME_3S
#define	LONG_PRESS_MAX_TIME			(u16)TIME_5S

#define FAULT_TIME					(u16)TIME_10S


#define INPUT_SW_1             		P1.3 			// MODE KEY

#define INPUT_SW_2             		P0.6 			// SET KEY

#define INPUT_SW_3             		P6.3 			// UP KEY

#define INPUT_SW_4             		P6.2 			// DOWN KEY

static u32 u32ModeTimeStamp = 0;
static u32 u32SetTimeStamp = 0;
static u32 u32UpTimeStamp = 0;
static u32 u32DownTimeStamp = 0; 

/*********************************************************************//**
*
* MODE SWITCH STATUS
*
* @param 	 None	 
*
* @return	 None
*************************************************************************/
SW_STATE eGetMODESwitchStatus()
{  
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	u32 u32ElapsedTime;
	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32ModeTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32ModeTimeStamp;


	if((INPUT_SW_1 == ePressed) 
	&& (u32ElapsedTime < SHORT_PRESS_MAX_TIME )
	&& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_2 == eReleased))
	{                
		eLastState = ePressed;
	}
	
	if((INPUT_SW_1 == ePressed && INPUT_SW_3 == ePressed ) && eCombination == eReleased
	&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME)
	&& (INPUT_SW_2 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeUpLongPressed;
	}

	if((INPUT_SW_1 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME )
		&& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_2 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}

	if((INPUT_SW_1 == ePressed && INPUT_SW_2 == ePressed ) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME)
		&& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeSetLongPressed;
	}

	if((INPUT_SW_1 == ePressed && INPUT_SW_4 == ePressed ) && eCombination == eReleased
	&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME)
	&& (INPUT_SW_2 == eReleased && INPUT_SW_3 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeDownLongPressed;
	}
		
	if( eLastState == ePressed && INPUT_SW_1 == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (u32)DEBOUNCE_TIME )
			ePressType = eInvalidPress;

		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME) 
			ePressType = eShortPressed;
		
		else if(u32ElapsedTime >  FAULT_TIME) 
			ePressType = eStuckFault;

		eCombination = eReleased;
		eLastState = eReleased;
	}

	// Press time exceeds maximum time.
	else if(INPUT_SW_1 == ePressed && u32ElapsedTime >= LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;
}

/*********************************************************************//**
*
* SET SWITCH STATUS
*
* @param	  None	  
*
* @return	  None
*************************************************************************/
SW_STATE eGetSETSwitchStatus()
{ 
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	u32 u32ElapsedTime;
	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32SetTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32SetTimeStamp;	

	if((INPUT_SW_2 == ePressed) 
	&& (u32ElapsedTime < SHORT_PRESS_MAX_TIME )
	&& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
	}

	if((INPUT_SW_2 == ePressed && INPUT_SW_3 == ePressed ) && eCombination == eReleased
		& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime > LONG_PRESS_MIN_TIME)
		& (INPUT_SW_1 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eSetUpLongPressed;
	}
		
	if((INPUT_SW_2 == ePressed) && eCombination == eReleased
		& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME )
		& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}
	if((INPUT_SW_2 == ePressed && INPUT_SW_1 == ePressed ) && eCombination == eReleased
		& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime > LONG_PRESS_MIN_TIME)
		& (INPUT_SW_3 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeSetLongPressed;
	}

	if( eLastState == ePressed && INPUT_SW_2 == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (u32)DEBOUNCE_TIME )
			ePressType = eInvalidPress;
		
		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME) 
			ePressType = eShortPressed;
		
		else if(u32ElapsedTime >  FAULT_TIME) 
			ePressType = eStuckFault;
		
		eCombination = eReleased;
		eLastState = eReleased;
	}



	// Press time exceeds maximum time.
	else if(INPUT_SW_2 == ePressed && u32ElapsedTime >= LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;

}


/*********************************************************************//**
*
* UP SWITCH STATUS
*
* @param	   None    
*
* @return	   None
*************************************************************************/
SW_STATE eGetUPSwitchStatus()
{
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	u32 u32ElapsedTime;
	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32UpTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32UpTimeStamp;

	if((INPUT_SW_3 == ePressed) 
	&& (u32ElapsedTime < SHORT_PRESS_MAX_TIME )
	&& (INPUT_SW_2 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
	}
		
	if((INPUT_SW_3 == ePressed && INPUT_SW_2 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME)
		&& (INPUT_SW_1 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eSetUpLongPressed;
	}

	if((INPUT_SW_3 == ePressed && INPUT_SW_1 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME && u32ElapsedTime < LONG_PRESS_MAX_TIME)
		&& (INPUT_SW_2 == eReleased && INPUT_SW_4 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeUpLongPressed;
	}
		
	if((INPUT_SW_3 == ePressed && INPUT_SW_4 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME)
		&& (INPUT_SW_1 == eReleased && INPUT_SW_2 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eUpDownLongPressed;
	}
		
	if((INPUT_SW_3 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME)
		&& (INPUT_SW_2 == eReleased && INPUT_SW_4 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}

	if( eLastState == ePressed && INPUT_SW_3 == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (u32)DEBOUNCE_TIME )
			ePressType = eInvalidPress;

		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME) 
			ePressType = eShortPressed;
		
		else if(u32ElapsedTime >  FAULT_TIME) 
			ePressType = eStuckFault;

		eCombination = eReleased;
		eLastState = eReleased;
	}



	// Press time exceeds maximum time.
	else if(INPUT_SW_3 == ePressed && u32ElapsedTime > LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;
}
  
/*********************************************************************//**
*
* DOWN SWITCH STATUS
*
* @param	  None	  
*
* @return	  None
*************************************************************************/
SW_STATE eGetDOWNSwitchStatus()
{
	static SW_STATE eLastState = eReleased;
	static SW_STATE eCombination = eReleased;
	u32 u32ElapsedTime;
	SW_STATE ePressType = eReleased;

	if(eLastState == eReleased)
	{
		u32DownTimeStamp = u32GetClockTicks();
	}

	u32ElapsedTime = u32GetClockTicks() - u32DownTimeStamp;


	if((INPUT_SW_4 == ePressed) 
	&& (u32ElapsedTime < SHORT_PRESS_MAX_TIME)
	&& (INPUT_SW_2 == eReleased && INPUT_SW_3 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
	}
		
	if((INPUT_SW_4 == ePressed && INPUT_SW_3 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME)
		&& (INPUT_SW_1 == eReleased && INPUT_SW_2 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eUpDownLongPressed;
	}

	if((INPUT_SW_4 == ePressed && INPUT_SW_1 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME)
		&& (INPUT_SW_3 == eReleased && INPUT_SW_2 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eModeDownLongPressed;
	}
		
	if((INPUT_SW_4 == ePressed) && eCombination == eReleased
		&& (u32ElapsedTime > LONG_PRESS_MIN_TIME)
		&& (INPUT_SW_2 == eReleased && INPUT_SW_3 == eReleased && INPUT_SW_1 == eReleased))
	{                
		eLastState = ePressed;
		eCombination = ePressed;
		return eLongPressed;
	}

	if( eLastState == ePressed && INPUT_SW_4 == eReleased)
	{
		// Looking for invalid press.
		if(u32ElapsedTime <= (u32)DEBOUNCE_TIME )
			ePressType = eInvalidPress;

		else if(u32ElapsedTime >  SHORT_PRESS_MIN_TIME && u32ElapsedTime <  SHORT_PRESS_MAX_TIME) 
			ePressType = eShortPressed;

		else if(u32ElapsedTime >  FAULT_TIME) 
			ePressType = eStuckFault;

		eLastState = eReleased;
		eCombination = eReleased;
	}

	else if(INPUT_SW_4 == ePressed && u32ElapsedTime > LONG_PRESS_MAX_TIME)
	{
		ePressType = eStuckFault;
		eLastState = ePressed;
	}

	return ePressType;

}

/*********************************************************************//**
*
* Resets all buttons press time
*
* @param 	 None	 
*
* @return	 None
*************************************************************************/
void vResetButtonTimeStamps()
{
	u32DownTimeStamp = u32GetClockTicks();
	u32UpTimeStamp = u32GetClockTicks();
	u32ModeTimeStamp = u32GetClockTicks();
	u32SetTimeStamp = u32GetClockTicks();
}
  
