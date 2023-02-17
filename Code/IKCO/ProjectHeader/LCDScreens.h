#ifndef __LCDSCREENS_H
#define __LCDSCREENS_H

#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "customTypedef.h"
#include "Event.h"


typedef enum
{
	eTripAScreen = 0,
	eTripBScreen,
	eHourAFETripA,
	eHourAFETripB,
	eClockHour,
	eClockMinute,
	eClockAMPM,
	eFeededPulse,
	eSetPulse,
	eSetFourthDigit,
	eSetThirdDigit,
	eSetSecondDigit,
	eSetFirstDigit,
	eServiceHourDue,
	eSoftwareVersion,
    eDTC,
    eIllumination,
    eTotalDTC
    
}SCREEN_SET;

typedef enum
{
	eNoFaults = 0U,
	eSingleFrame,
	eMultiFrames
    
}DTC_TYPES;
typedef struct
{	
u16    u16PulseData;					// Prepare clock data for display u32
u8	   u8FirstDigit;				 
u8	   u8SecondDigit;	
u8	   u8ThirdDigit;				 
u8	   u8FourthDigit;
} PULSE_DATA_TYPE;

typedef struct
{	
bool    bPulseSetFlag;					// Prepare clock data for display u32
bool    bPulseEntryFlag;				 
bool    bPulseSetExitFlag;	
} PULSE_DATA_FLAG;


extern u8 u8GetVehicleType(void);
extern u16 u16GetFeededPulse(void);
extern SCREEN_SET  GetLcdScreenStatus();
extern u16 u16GetVehicleSpeed(void);
extern u16 u16GetEngineSpeed(void);
extern u16 u16GetPulseSet();
extern void vSetDispPulse(u16);


/// @addtogroup LCD_SCREEN_LIBRARY
/// @{
 
/// @defgroup LcdScreenLib Global functions used in LCD files
//// 
///  
 
/// @addtogroup LcdScreenLib
/// @{


void vScreenTransition(void);

void vResetScreen(void);

// Getting various Mode types and status, reserved for future standardisation 

u8 u8GetTripType();
u8 u8GetClockSetStatus();
SCREEN_SET u8GetScreenMode();
ODO_RESET u8GetOdoTripRstStatus();
HOUR_RESET u8GetHourTripRstStatus();
AFE_RESET u8GetAfeRstStatusA();
AFE_RESET u8GetAfeRstStatusB();
SCREEN_SET u8GetFeederPulseStatus();
SCREEN_SET u8GetFeederPulseDigitStatus();

u8 u8GetDTCMsgNumber();
bool bGetDoSelfTestStatus();
bool bGetSendTripResetStatus();
DTC_TYPES eGetDtcType(void);

void vSetScreenMode(SCREEN_SET data);
void vSetTripType(u8 );
void vSetOdoTripRstStatus(ODO_RESET data);
void vSetHourTripRstStatus(HOUR_RESET data);
void vSetAfeRstAStatus(AFE_RESET data);
void vSetAfeRstBStatus(AFE_RESET data);
void vSetClockSetStatus(u8 );
void vSetFeederPulseStatus(SCREEN_SET data);
void vSetDTCMsgNumber(u8 data);
void vSetDoSelfTestStatus(bool );
void vSetSendTripResetStatus(bool );
void vSetTripAOdoResetReq(void);
void vSetTripBOdoResetReq(void);
void vSetTripAHrResetReq(void);
void vSetTripBHrResetReq(void);


/// @} // endgroup LcdScreenLib

/// @} // endgroup LCD_SCREEN_LIBRARY


#endif /* LCDSCREENS_H_ */

