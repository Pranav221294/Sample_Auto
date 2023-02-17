#ifndef __ServiceReminder_H
#define __ServiceReminder_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"


//Constant declarations -----------------------------------------------------

#define  UpdateEolServiceparameter   1
#define  NormalServiceDueHour	  	 0



typedef enum
{
	eServRemdOn10Sec = 0,
	eServRemdOnContinuous,	
	eDueService,
	eServRemdNoDisp
}eSERVICE_REMINDER_TYPE;


typedef enum
{
	eHourBasedService =0,	
	eOdoBasedService
}SERVICE_TYPE;


	

/// @addtogroup SERVICE_REMINDER_LIBRARY
/// @{
/// @defgroup ServicereminderLib  functions used 
//// 
///   
/// @addtogroup ServiceReminderLib
/// @{
void vServiceReminder(void);
void vSetNextDueService( u32 );
u32 u32GetNextDueService(void);
void vSetServiceCounter( u32 );
u32 u32getServiceCounter(void);

eSERVICE_REMINDER_TYPE eGetServiceStatus(void);
void vClearServiceReminder();
bool bGetServiceDoneFlag();
void vSetServiceDoneFlag( bool );
void vCalcNextServiceDue( bool );


/// @} // endgroup ServiceReminderLib

/// @} // endgroup SERVICE_REMINDER_LIBRARY

#endif
