#ifndef __Clock_H
#define __Clock_H
#include "customTypedef.h" 




typedef struct
{	
u16    u16ClockData;					// Prepare clock data for display u32
u8	   u8Hour;				 
u8	   u8Minute;	
u8	   u8DispHour;				 
u8	   u8DispMinute;
u8	   u8Second;			 
u8	   u8PwrOnSecond;					//Count second when CPU reset untill actual clock data not read
bool   bDispAmPmFlag : 1;				//If True means PM and False means AM
bool   bAmPmFlag : 1;					//If True means PM and False means AM
bool   bUpdateData : 1; 				//set when second change
bool   bPowerONCountFlag : 1;			//count power on second until this flag is true
	
} CLOCK_DATA_TYPE;


extern CLOCK_DATA_TYPE sClock;


/// @addtogroup CLOCK_LIBRARY
/// @{
/// @defgroup ClockLib  functions used 
//// 
///   
/// @addtogroup ClockLib
/// @{

void vClockUpdate();
u16 u16GetClockData();
u8 u8GetClockHours();
u8 u8GetClockMinutes();
u8 u8GetDispClockHours();
u8 u8GetDispClockMinutes();
u8 u8GetClockSeconds();
u8 u8GetClockPwrOnSecond();
bool bGetDispClockAmPmFlag();
bool bGetClockAmPmFlag();
bool bGetClockUpdateData();
bool bGetClockPowerONCountFlag();

void vSetClockData(u16 );
void vSetHours(u8 );
void vSetMinutes(u8 );
void vSetDispHours(u8 );
void vSetDispMinutes(u8 );
void vSetSeconds(u8 );
void vSetPwrOnSecond(u8 );
void vSetDispAmPmFlag(bool );
void vSetAmPmFlag(bool );
void vSetUpdateData(bool );
void vSetPowerONCountFlag(bool );

/// @} // endgroup ClockLib

/// @} // endgroup CLOCK_LIBRARY


#endif


