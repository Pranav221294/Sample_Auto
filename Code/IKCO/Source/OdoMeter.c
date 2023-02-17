#include "OdoMeter.h"
#include "Event.h" 
#include "r_cg_userdefine.h"
#include "Afe.h" 

ODO_DATA_TYPE sODO = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Calculate ODO for CAN speedo
 *
 * @param	Odo pulses coming form SPN 191       
 *
 * @return	NOne     
 *************************************************************************/
void vCANOdoUpdate(u16 OdoPulses)
{

static u16 DeltaOdo = 0;

	DeltaOdo = DeltaOdo + OdoPulses;

	if(DeltaOdo > 100)
	{		
		DeltaOdo = DeltaOdo - 100;	
		
			if( ++sODO.u8OdoFractionValue > ODO_FRACTION_VALUE )	  //Incement in ODO fraction part
			{
				sODO.u8OdoFractionValue = 0;
				++sODO.u32OdoValue;
				//Save Odo in main when odo <= ODO_MAX_RANGE/10 otherwise Save Odo while Ign Off
				if(sODO.u32OdoValue <= ODO_MAX_RANGE )
					sODO.bOdoUpdate = TRUE;
			}
			   

		//Increment in ODO trip A
			if( ++sODO.u8TripAFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
			{
				sODO.u8TripAFractionValue = 0;
				if( ++sODO.u32TripAValue > TRIPA_MAX_RANGE) 			//Check trip recycle to zero
				{
					sAfe.u8EEPROMOffsetSaveFlagA = FALSE;
					sODO.u32TripAValue = 0;
				}
				//sODO.bTripAUpdate = TRUE; 
			}
		
		//Increment in ODO trip B
			if( ++sODO.u8TripBFractionValue > ODO_FRACTION_VALUE )	//Incement in TRIP fraction part
			{
				sODO.u8TripBFractionValue = 0;		
				if(++sODO.u32TripBValue > TRIPB_MAX_RANGE)			//Check trip recycle to zero
				{
					sAfe.u8EEPROMOffsetSaveFlagB = FALSE;
					sODO.u32TripBValue = 0;
				}
				//sODO.bTripBUpdate = TRUE; 
			}
		
	}	
	
}



/*********************************************************************//**
 *
 * Set Odo data
 *
 *
 * @param      Odo Data 
 *
 * @return     None
 *************************************************************************/
void vSetOdoValue(u32 Value)
{
	sODO.u32OdoValue = Value;
	
}

/*********************************************************************//**
 *
 * Get Odo data
 *
 *
 * @param      None
 *
 * @return     Odo Data
 *************************************************************************/
u32 u32GetOdoValue(void)
{
	return sODO.u32OdoValue;	
}

/*********************************************************************//**
 *
 * Set Odo Fraction data
 *
 *
 * @param      Odo Fraction data 
 *
 * @return     None
 *************************************************************************/
void vSetOdoFractionValue(u8 Value)
{
	sODO.u8OdoFractionValue = Value;
	
}

/*********************************************************************//**
 *
 * Get Odo Fraction data
 *
 *
 * @param      None
 *
 * @return     Odo Fraction data
 *************************************************************************/
u8 u8GetOdoFractionValue(void)
{
	return sODO.u8OdoFractionValue;
	
}

/*********************************************************************//**
 *
 * Set Odo Update Flag
 *
 *
 * @param      Odo Update Flag
 *
 * @return     None
 *************************************************************************/
void vSetOdoUpdateFlagStatus(bool Status)
{
	sODO.bOdoUpdate = Status;
}


/*********************************************************************//**
 *
 * Get Odo Update Flag
 *
 *
 * @param      None
 *
 * @return     Odo Update Flag
 *************************************************************************/
bool bGetOdoUpdateFlagStatus(void)
{
 return sODO.bOdoUpdate ;	
}

/*********************************************************************//**
 *
 * Set Odo Fraction Update
 *
 *
 * @param      Set Odo Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoFractionUpdateStatus(bool Status)
{
	sODO.bFrOdoUpdate = Status;
}

/*********************************************************************//**
 *
 * Get Odo Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo Fraction Update
 *************************************************************************/
bool bGetOdoFractionUpdateStatus(void)
{
	return sODO.bFrOdoUpdate;
}


/*********************************************************************//**
 *
 * Set Odo TripA Value
 *
 *
 * @param      Odo TripA Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAValue(u32 Value)
{
	sODO.u32TripAValue = Value;	
}

/*********************************************************************//**
 *
 * Get Odo TripA Value
 *
 *
 * @param      None
 *
 * @return     Odo TripA Value
 *************************************************************************/
u32 u32GetOdoTripAValue(void)
{
 	return sODO.u32TripAValue;	
}

/*********************************************************************//**
 *
 * Set Odo TripA Fraction Value
 *
 *
 * @param      Odo TripA Fraction Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAFractionValue(u8 Value)
{
	sODO.u8TripAFractionValue = Value;	
}

/*********************************************************************//**
 *
 * Get Odo TripA Fraction Value
 *
 *
 * @param      None
 *
 * @return     Odo TripA Fraction Value
 *************************************************************************/
u8 u8GetOdoTripAFractionValue(void)
{
 	return sODO.u8TripAFractionValue;	
}


/*********************************************************************//**
 *
 * Set Odo TripA Update
 *
 *
 * @param      TripA Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAUpdateStatus(bool Status)
{
	sODO.bTripAUpdate = Status;	
}

/*********************************************************************//**
 *
 * Get Odo TripA Update 
 *
 *
 * @param      None
 *
 * @return     Odo TripA Update
 *************************************************************************/
bool bGetOdoTripAUpdateStatus(void)
{
	return sODO.bTripAUpdate;	
}

/*********************************************************************//**
 *
 * Get Odo TripA Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo TripA Fraction Update
 *************************************************************************/
bool bGetOdoTripAFractionUpdateStatus(void)
{
	return sODO.bFrTripAUpdate;	
}

/*********************************************************************//**
 *
 * Set Odo TripA Fraction Update
 *
 *
 * @param      Odo TripA Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAFractionUpdateStatus(bool Status)
{
	sODO.bFrTripAUpdate = Status;	
}

/*********************************************************************//**
 *
 * Set Odo TripA Pulse Counter
 *
 *
 * @param      TripA Pulse Counter
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripAPulseCounter(u16 Value)
{
	sODO.u16TripAPulseCount = Value;
	
}

/*********************************************************************//**
 *
 * Set Odo TripB Value
 *
 *
 * @param      Odo TripB Value
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBValue(u32 Value)
{
	sODO.u32TripBValue = Value;	
}

/*********************************************************************//**
 *
 * Get Odo TripB Value
 *
 *
 * @param      None
 *
 * @return     Odo TripB Value
 *************************************************************************/
u32 u32GetOdoTripBValue(void)
{
 return sODO.u32TripBValue;	
}

/*********************************************************************//**
 *
 * Set TripB Fraction
 *
 *
 * @param      Odo TripB Fraction
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBFractionValue(u8 Value)
{
	sODO.u8TripBFractionValue = Value;	
}

/*********************************************************************//**
 *
 * Get Odo TripB Fraction Value
 *
 *
 * @param      None
 *
 * @return     Odo TripB Fraction Value
 *************************************************************************/
u8 u8GetOdoTripBFractionValue(void)
{
 	return sODO.u8TripBFractionValue;	
}

/*********************************************************************//**
 *
 * Set Odo TripB Update
 *
 *
 * @param      Odo TripB Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBUpdateStatus(bool Status)
{
	sODO.bTripBUpdate = Status;	
}

/*********************************************************************//**
 *
 * Get Odo TripB Update
 *
 *
 * @param      None
 *
 * @return     Odo TripB Update
 *************************************************************************/
bool bGetOdoTripBUpdateStatus(void)
{
 return sODO.bTripBUpdate;	
}

/*********************************************************************//**
 *
 * Get Odo TripB Fraction Update
 *
 *
 * @param      None
 *
 * @return     Odo TripB Fraction Update
 *************************************************************************/
bool bGetOdoTripBFractionUpdateStatus(void)
{
	return sODO.bFrTripBUpdate;	
}

/*********************************************************************//**
 *
 * Set Odo TripB Fraction Update
 *
 *
 * @param      Odo TripB Fraction Update
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBFractionUpdateStatus(bool Status)
{
	sODO.bFrTripBUpdate = Status;	
}

/*********************************************************************//**
 *
 * Set Odo TripB Pulse Counter
 *
 *
 * @param      Odo TripB Pulse Counter
 *
 * @return     None
 *************************************************************************/
void vSetOdoTripBPulseCounter(u16 Value)
{
	sODO.u16TripBPulseCount = Value;
	
}

