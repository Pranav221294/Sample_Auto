#ifndef __Afe_H
#define __Afe_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"


typedef struct
{		 
 u32	u32TotalFuelConsumed;		//Total fuel consume value 									 
 u32	u32PreFuelConsumedA;	
 u32	u32PreFuelConsumedB;
 u32	u32TripAUsedFuelAtReset;	//Update value total fuel consumed when Trip Reset or rollover 
 u32	u32TripBUsedFuelAtReset;	//Update value total fuel consumed when Trip Reset or rollover 
 u16 	u16TripAOffset;
 u16 	u16TripBOffset;
 u16	u16TripAAfe;
 u16	u16TripBAfe;
 u16	u16DispTripAAfe;
 u16	u16DispTripBAfe;
 u8 	u8ReadCountTripA;
 u8 	u8ReadCountTripB;
 u8		u8EEPROMOffsetSaveFlagA;
 u8		u8EEPROMOffsetSaveFlagB;
 bool	bUpdateTripAFuelUsedAtReset :1;
 bool	bUpdateTripBFuelUsedAtReset :1 ;
 bool	bUpdatePreFuelConsumed :1;
 bool	bUpdateTripA :1;
 bool	bUpdateTripB :1;
 bool	bUpdateTripAOffset :1;
 bool	bUpdateTripBOffset :1;
 bool	bSaveFuelConsumedAtResetA :1;
 bool	bSaveFuelConsumedAtResetB :1;
	 
} AFE_DATA_TYPE;
 
extern AFE_DATA_TYPE sAfe;

/// @addtogroup AFE_LIBRARY
/// @{
/// @defgroup AfeLib  functions used in main file 
//// 
///   
/// @addtogroup AfeLib
/// @{


u32	u32GetTotalFuelConsumed();		 									 
u32	u32GetPreFuelConsumedA();		
u32	u32GetPreFuelConsumedB();		
u32	u32GetTripAUsedFuelAtReset();			 
u32	u32GetTripBUsedFuelAtReset();
u16 u16GetTripAOffset();		
u16 u16GetTripBOffset();		
u16	u16GetTripAAfe();		
u16	u16GetTripBAfe();	
u16 u16GetDispTripAAfe();
u16 u16GetDispTripBAfe();	
u8 	u8GetReadCountTripA();		
u8 	u8GetReadCountTripB();	
u8	u8GetEEPROMOffsetSaveFlagA();		
u8	u8GetEEPROMOffsetSaveFlagB();		
bool bGetUpdateTripAFuelUsedAtReset();		
bool bGetUpdateTripBFuelUsedAtReset();		
bool bGetUpdatePreFuelConsumed();		
bool bGetUpdateTripA();		
bool bGetUpdateTripB();		
bool bGetUpdateTripAOffset();		
bool bGetUpdateTripBOffset();		
bool bGetSaveFuelConsumedAtResetA();		
bool bGetSaveFuelConsumedAtResetB();		
	 									 
void vSetPreFuelConsumedA(u32);		
void vSetPreFuelConsumedB(u32);		
void vSetTripAUsedFuelAtReset(u32);			 
void vSetTripBUsedFuelAtReset(u32);
void vSetTripAOffset(u16);		
void vSetTripBOffset(u16);		
void vSetTripAAfe(u16);		
void vSetTripBAfe(u16);
void vSetDispTripAAfe(u16);		
void vSetDispTripBAfe(u16);		
void vSetReadCountTripA(u8);
void vSetReadCountTripB(u8);	
void vSetEEPROMOffsetSaveFlagA(u8);		
void vSetEEPROMOffsetSaveFlagB(u8);		
void vSetUpdateAfeTripAFuelUsedAtReset(bool);		
void vSetUpdateAfeTripBFuelUsedAtReset(bool);		
void vSetUpdatePreFuelConsumed(bool);		
void vSetUpdateAfeTripA(bool);		
void vSetUpdateAfeTripB(bool);		
void vSetUpdateTripAOffset(bool);		
void vSetUpdateTripBOffset(bool);		
void vSetSaveFuelConsumedAtResetA(bool);		
void vSetSaveFuelConsumedAtResetB(bool);	

void vCalculateAfe();


/// @} // endgroup AfeLib

/// @} // endgroup AFE_LIBRARY

#endif