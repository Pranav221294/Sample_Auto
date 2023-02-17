#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "r_cg_userdefine.h"
#include "ServiceReminder.h"
#include "Eol.h"
#include "OdoMeter.h"
#include "HourMeter.h"



// Local declarations --------------------------------------------------------

static u32 u32Servicecounter = 0;
static u32 u32NextServiceDue = 0;
static bool	bServiceClearanceFlag;
static bool	bServiceDoneFlag;

eSERVICE_REMINDER_TYPE ServiceReminderType = eServRemdNoDisp;



/*********************************************************************//**
 *
 * Calculate service reminder and set remider type 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vServiceReminder(void)
{
 u8 NextService = 0;
 u32 CurrentValue = 0;

	//its a depend on type  of services  ODO or hours based 
	if(sIndicatorInputType.Inputsbytes.Byte3.bServiceType == eHourBasedService)
	   CurrentValue = u32GetHoursValue();
	else
		CurrentValue = u32GetOdoValue();

	//Logic for First service 
	if(CurrentValue <= sServReminder.u32FirstServiceDue)
	{	
		if(u32Servicecounter == 0) //if first service not done 
		{			
						
			 if( (u32NextServiceDue - CurrentValue) <= sServReminder.u32PopUpInterval) //check popup interval for service Icon  Continouse ON 
			{
				ServiceReminderType = eServRemdOnContinuous;
			}
			else
			{
			 	ServiceReminderType = eServRemdNoDisp;
			}
			 	

			//Service clearance if service occured
			if(ServiceReminderType ==  eServRemdOnContinuous)
			{
				if(bServiceClearanceFlag == TRUE) //This flag set by Routine  control service 
				{
					vCalcNextServiceDue(NormalServiceDueHour);
					bServiceClearanceFlag = FALSE;
					
				}
			}

		}
		else 
		{
			ServiceReminderType = eServRemdNoDisp; //Do not Dispay Service reminder 			
		}
	 
	}//End if(CurrentValue <= sServReminder.u32FirstServiceDue)
	else 
	{
		//Calculate 
		NextService = ( (CurrentValue - sServReminder.u32FirstServiceDue)/sServReminder.u32ServiceInterval ) + 1;
		
		//if Service done with in interval  
		if(u32Servicecounter > NextService )
		{
		   ServiceReminderType = eServRemdNoDisp;
		   return;
		}
	
		if(u32Servicecounter < NextService)
		{
			ServiceReminderType = eDueService;	//Display service due 
		}
		else
		{
			
			if( (u32NextServiceDue  - CurrentValue ) <= sServReminder.u32PopUpInterval)
			{
				ServiceReminderType = eServRemdOnContinuous;
			}
			else
			{
			 	ServiceReminderType = eServRemdNoDisp;
			}
									
		}
	
		//Service clearance 
		if(ServiceReminderType == eDueService)
		{
			if(bServiceClearanceFlag == TRUE)
			{
				vCalcNextServiceDue(NormalServiceDueHour);
				bServiceClearanceFlag = FALSE;				
			}
		}
		else if(ServiceReminderType ==  eServRemdOnContinuous)
		{
			if(bServiceClearanceFlag == TRUE)
			{
				vCalcNextServiceDue(NormalServiceDueHour);
				bServiceClearanceFlag = FALSE;				
			}
		}
		
	
	}//end else
	
}

/*********************************************************************//**
 *
 * Calculate next service due when set Service EOL parmeter 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vCalcNextServiceDue(bool ServiceDuetype)
{

	u32 NextService = 0;
	u32 CurrentValue = 0;

	//its a depend on type  of services  ODO or hours based 
	if(sIndicatorInputType.Inputsbytes.Byte3.bServiceType == eHourBasedService)
	   CurrentValue = u32GetHoursValue();
	else
		CurrentValue = u32GetOdoValue();
   if(CurrentValue > sServReminder.u32FirstServiceDue)
   	{
	   NextService = ( (CurrentValue - sServReminder.u32FirstServiceDue)/sServReminder.u32ServiceInterval ) + 1;
   	}
	else
	{
	 	NextService = 1;
	}

	// when service parameter update by EOL	
	if( ServiceDuetype == UpdateEolServiceparameter )
	{
		if(CurrentValue <= sServReminder.u32FirstServiceDue)
			u32NextServiceDue = sServReminder.u32FirstServiceDue;
	    else
		{		  
		  u32NextServiceDue = 	sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * NextService;
		}
		
		u32Servicecounter = 0;
	}
	else
	{

		if(u32Servicecounter == 0)
		{
			u32Servicecounter = NextService;  //Save in EEPROM
			u32NextServiceDue = sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * u32Servicecounter;			
			//If odo/Hours have greater with service occuring time 
			if( CurrentValue >= ( u32NextServiceDue - sServReminder.u32PopUpInterval ) )
			{
				 u32Servicecounter++;
				 u32NextServiceDue = sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * u32Servicecounter;			
			}
		}
		else
		{
			//Service clearance 
			if(ServiceReminderType == eDueService)
			{
				u32Servicecounter = NextService;  //Save in EEPROM
				u32NextServiceDue = 	sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * u32Servicecounter;
				if( CurrentValue >= ( u32NextServiceDue - sServReminder.u32PopUpInterval ) )
				{
					 u32Servicecounter++;
					 u32NextServiceDue = sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * u32Servicecounter;			
				}
					
			}
			else if(ServiceReminderType ==  eServRemdOnContinuous)
			{			
				u32Servicecounter = NextService + 1;  //Save in EEPROM
				u32NextServiceDue = 	sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval *  u32Servicecounter;
				if( CurrentValue >= ( u32NextServiceDue - sServReminder.u32PopUpInterval ) )
				{
					 u32Servicecounter++;
					 u32NextServiceDue = sServReminder.u32FirstServiceDue + sServReminder.u32ServiceInterval * u32Servicecounter;			
				}				
				
			}
		
		}
	
	}
	
	//for Saving calculated Next service due 
 	bServiceDoneFlag = TRUE; 
}

/*********************************************************************//**
 *
 * Set Next service due km or hours 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vSetNextDueService(u32 ServDue)
{
 	u32NextServiceDue = ServDue;
}


/*********************************************************************//**
 *
 * Return Next service due km or hours 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

u32 u32GetNextDueService()
{
	if(u32NextServiceDue > 9999999)	//Odo max range 
	    u32NextServiceDue = 9999999;
 	return u32NextServiceDue;
}

/*********************************************************************//**
 *
 *  Set service counter value 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vSetServiceCounter( u32 ServCount)
{
  u32Servicecounter = ServCount;
}

/*********************************************************************//**
 *
 *  Get service counter value 
 * 
 *
 * @param      None    
 *
 * @return     Service counter value
 *************************************************************************/

u32 u32getServiceCounter( )
{
  return u32Servicecounter;
}


/*********************************************************************//**
 *
 *  Get service reminder status 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

eSERVICE_REMINDER_TYPE eGetServiceStatus()
{
  return ServiceReminderType;
}

/*********************************************************************//**
 *
 *  Clear service reminder 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vClearServiceReminder()
{
  bServiceClearanceFlag = TRUE;
}

/*********************************************************************//**
 *
 *  Get Service Done flag
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

bool bGetServiceDoneFlag()
{
 return bServiceDoneFlag;
}

/*********************************************************************//**
 *
 *  Set Service Done flag
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vSetServiceDoneFlag(bool status)
{
	bServiceDoneFlag = status;
}



