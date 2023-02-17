#ifndef __HourMeter_H
#define __HourMeter_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"


typedef struct
{	
	u32		u32HourMeter;					// Prepare clock data for display 
	u32		u32HourTripA;      		 
	u32		u32HourTripB; 
	u16		u16HrMtrSeconds;
	u16		u16HrTripASeconds;
	u16		u16HrTripBSeconds;
	u8      u8HrFractionValue;
	u8      u8HrTripAFractionValue;
	u8      u8HrTripBFractionValue;
	bool 	bHourMeterEnable : 1;
	bool	bUpdateHourMtr : 1;							//set when second change
	bool	bUpdateHourTripA : 1;						//set when second change
	bool	bUpdateHourTripB : 1;						//set when second change
	bool	bFrUpdateHourMtr : 1;						//set when second change
	bool	bFrUpdateHourTripA : 1;						//set when second change
	bool	bFrUpdateHourTripB : 1;						//set when second change
	
} HOUR_DATA_TYPE;


extern HOUR_DATA_TYPE sHourMeter;


/// @addtogroup HOUR_METER_LIBRARY
/// @{
/// @defgroup HourMeterLib  functions used 
//// 
///   
/// @addtogroup HourMeterLib
/// @{
void vStart_Hour_Meter(void);
bool bGetHourMeterEnableStatus(void);
void vSetHourMeterEnableStatus(bool);

u32 u32GetTotalHours(void);
u32 u32GetTotalTripASeconds(void);
u32 u32GetTotalTripBSeconds(void);

u32 u32GetHoursValue(void);
u8 u8GetHoursFractionValue(void);
void vSetHoursValue(u32);
void vSetHoursFractionValue(u8);
void vSetHoursCounterValue(u16);
u16 u16GetHoursCounterValue();
bool bGetHoursUpdateStatus(void);
bool bGetHoursFractionUpdateStatus(void);
void vSetHoursUpdateStatus(bool);
void vSetHoursFractionUpdateStatus(bool);

u32 u32GetHoursTripAValue(void);
u8 u8GetHoursTripAFractionValue(void);
void vSetHoursTripAValue(u32);
void vSetHoursTripAFractionValue(u8);
void vSetHoursTripACounterValue(u16);
u16 u16GetHoursTripACounterValue();
bool bGetHoursTripAUpdateStatus(void);
bool bGetHoursTripAFractionUpdateStatus(void);
void vSetHoursTripAUpdateStatus(bool);
void vSetHoursTripAFractionUpdateStatus(bool);

u32 u32GetHoursTripBValue(void);
u8 u8GetHoursTripBFractionValue(void);
void vSetHoursTripBValue(u32);
void vSetHoursTripBFractionValue(u8);
void vSetHoursTripBCounterValue(u16);
u16 u16GetHoursTripBCounterValue();
bool bGetHoursTripBUpdateStatus(void);
bool bGetHoursTripBFractionUpdateStatus(void);
void vSetHoursTripBUpdateStatus(bool);
void vSetHoursTripBFractionUpdateStatus(bool);

/// @} // endgroup HourMeterLib

/// @} // endgroup HOUR_METER_LIBRARY

#endif
