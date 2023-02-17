#ifndef __ODO_METER_H
#define __ODO_METER_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"




typedef struct
{
	u32     u32OdoValue;                    
	u32     u32TripAValue;      		
	u32     u32TripBValue;         		
	u16     u16OdoPulseCount;
	u16     u16TripAPulseCount;
	u16     u16TripBPulseCount;
	u8      u8OdoFractionValue;
	u8      u8TripAFractionValue;
	u8      u8TripBFractionValue;
	bool    bOdoUpdate : 1;          			// Set when ODO update
	bool    bTripAUpdate : 1;          			// Set when TripA update
	bool    bTripBUpdate : 1;          			// Set when TripB update
	bool    bFrOdoUpdate : 1;          			// Set when Fraction Odo update
	bool    bFrTripAUpdate : 1;          		// Set when Fraction TripA update
	bool    bFrTripBUpdate : 1;          		// Set when Fraction TripB update

} ODO_DATA_TYPE;

extern ODO_DATA_TYPE sODO;


//Constant declarations ----------------------------------------------------


/// @addtogroup ODO_METER_DATA_LIBRARY
/// @{
/// @defgroup odometerdataLib  functions used 
//// 
///   
/// @addtogroup odometerdataLib
/// @{
	
void vSetOdoValue(u32);
u32 u32GetOdoValue(void);
void vSetOdoFractionValue(u8);
u8 u8GetOdoFractionValue(void);
void vSetOdoUpdateFlagStatus(bool);
bool bGetOdoUpdateFlagStatus(void);
void vSetOdoFractionUpdateStatus(bool);
bool bGetOdoFractionUpdateStatus(void);

void vSetOdoTripAValue(u32);
u32 u32GetOdoTripAValue(void);
void vSetOdoTripAFractionValue(u8 );
u8 u8GetOdoTripAFractionValue(void);
void vSetOdoTripAPulseCounter(u16);
void vSetOdoTripAUpdateStatus(bool);
bool bGetOdoTripAUpdateStatus(void);	
bool bGetOdoTripAFractionUpdateStatus(void);
void vSetOdoTripAFractionUpdateStatus(bool);

void vSetOdoTripBValue(u32);
u32 u32GetOdoTripBValue(void);
void vSetOdoTripBFractionValue(u8 );
u8 u8GetOdoTripBFractionValue(void);
void vSetOdoTripBPulseCounter(u16);
void vSetOdoTripBUpdateStatus(bool);
bool bGetOdoTripBUpdateStatus(void);
bool bGetOdoTripBFractionUpdateStatus(void);
void vSetOdoTripBFractionUpdateStatus(bool);
	
void vCANOdoUpdate(u16);



/// @} // endgroup odometerdataLib

/// @} // endgroup ODO_METER_DATA_LIBRARY

#endif
