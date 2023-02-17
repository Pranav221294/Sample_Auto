
#include "r_cg_macrodriver.h"

#include "customTypedef.h"
#include "r_cg_userdefine.h"
#include "Afe.h" 
#include "Eol.h" 
#include "J1939.h"
#include "HourMeter.h"
#include "OdoMeter.h"
#include "LCDScreens.h"




#define TIPPERS_MAX_AFE_VALUE		999
#define TRUCKS_MAX_AFE_VALUE		255

 
AFE_DATA_TYPE sAfe = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0, 0, 0,0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static bool bAfeAReset = FALSE;
static bool bAfeBReset = FALSE;

/*********************************************************************//**
 *
 * Take Afe data and calculate Afe value 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vCalculateAfe()
{
	u8 u8AfeMulFactor = 0;
	u16 u16AFEMaxValue = (u16)TRUCKS_MAX_AFE_VALUE;
	u32 FuelDiffrence = 0;
	u32	TempVar = 0;
	u32 Temp1 = 0 ,Temp2 = 0;
	u8 u8FuelType = 0;
	
	AFE_RESET eAFEResetA = eNoAFEReset;
	AFE_RESET eAFEResetB = eNoAFEReset;
	
	eAFEResetA = u8GetAfeRstStatusA();
	eAFEResetB = u8GetAfeRstStatusB();
	
	u8FuelType 	= u8GetVehicleType();	
	

	u8AfeMulFactor = sAfeDataOptionBytes.u8AfeMulFactor;
	
	if(u8FuelType == FUEL_TYPE_LPH)
	{
		u16AFEMaxValue = (u16)TIPPERS_MAX_AFE_VALUE;
	}
	else
	{
		u16AFEMaxValue = (u16)TRUCKS_MAX_AFE_VALUE;
	}		

	if(eAFEResetA == eAFEAtLowLfcA)
	{
		bAfeAReset = TRUE;
	}
	else if(eAFEResetB == eAFEAtLowLfcB)
	{
		bAfeBReset = TRUE;
	}

	if(bGetPGNTimeOut(eJAfe))
	{
		sAfe.u16DispTripAAfe = sAfe.u16TripAAfe;
		sAfe.u16DispTripBAfe = sAfe.u16TripBAfe;
		sAfe.bSaveFuelConsumedAtResetA = FALSE;
		sAfe.bSaveFuelConsumedAtResetB = FALSE;
		TempVar = 0;
	}
	else
	{
		if(bGetPGNDataReady(eJAfe) )
		{
			TempVar = u32GetPGNdata(eJAfe);

			if(TempVar >= BADDATA_4BYTE)
			{
				sAfe.u16DispTripAAfe = 0;
				sAfe.u16DispTripBAfe = 0;
				return; 		
			}
			
			/////////////////////////////////////////TripA Read Count///////////////////////////////////////////////////
			/*
				ReadCount  = NULL -----------------------------> RESET or Start Condition
				ReadCount  = FIRST_READ_AFTER_RESET -----------> Offset Calculation
				ReadCount  = SECOND_READ_COUNT ----------------> AFE calculation

			*/
			if(sAfe.u8ReadCountTripA == NULL )    
			{
				if((TempVar * 10) > sAfe.u32PreFuelConsumedA)
				{
					if(bAfeAReset == TRUE)
					{
						sAfe.u8ReadCountTripA= NULL;
						sAfe.u32TripAUsedFuelAtReset = TempVar*10;
						sAfe.u32PreFuelConsumedA = TempVar*10;
						vSetAfeRstAStatus(eNoAFEReset);
					}
					/*
						When the fuel used at reset is zero then, last fuel consumption at reset is 
						equals to present fuel consumption and readcount becomes 1 for saving offset.
					*/
					if(sAfe.u32TripAUsedFuelAtReset == 0)
					{
						sAfe.u32TripAUsedFuelAtReset = TempVar*10;
						sAfe.u8ReadCountTripA = FIRST_READ_AFTER_RESET;
					}
					else	// else only increment the readcount for offset calculation. 
					{
						sAfe.u8ReadCountTripA = FIRST_READ_AFTER_RESET;
					}
					bAfeAReset = FALSE;
					
				}
				else if((TempVar * 10) < sAfe.u32PreFuelConsumedA)
				{
					if(bAfeAReset == TRUE)
					{
						bAfeAReset = FALSE;
						sAfe.u8ReadCountTripA = NULL;
						sAfe.u32TripAUsedFuelAtReset = TempVar*10;
						sAfe.u32PreFuelConsumedA = TempVar*10;
						vSetAfeRstAStatus(eNoAFEReset);
					}
					
				}
				else
				{
					NOP();
				}
			}
			else if(sAfe.u8ReadCountTripA == SECOND_READ_COUNT)
			{
				if((TempVar * 10) > sAfe.u32TotalFuelConsumed)
				{				
					if(sAfe.u8EEPROMOffsetSaveFlagA == FALSE)
						vSetEEPROMOffsetSaveFlagA(TRUE);
				}
			}
				
			/////////////////////////////////////////TripB Read Count///////////////////////////////////////////////////
			/*
				ReadCount  = NULL -----------------------------> RESET or Start Condition
				ReadCount  = FIRST_READ_AFTER_RESET -----------> Offset Calculation
				ReadCount  = SECOND_READ_COUNT ----------------> Offset Flag True and AFE calculation

			*/
			
			if(sAfe.u8ReadCountTripB == NULL )    
			{
				if((TempVar * 10) > sAfe.u32PreFuelConsumedB)
				{
					if(bAfeBReset == TRUE)
					{
						sAfe.u8ReadCountTripB = NULL;
						sAfe.u32TripBUsedFuelAtReset = TempVar*10;
						sAfe.u32PreFuelConsumedB = TempVar*10;
						vSetAfeRstBStatus(eNoAFEReset);
					}
					
					/*
						When the fuel used at reset is zero then, last fuel consumption at reset is 
						equals to present fuel consumption and readcount becomes 1 for saving offset.
					*/
					if(sAfe.u32TripBUsedFuelAtReset == 0)
					{
						sAfe.u32TripBUsedFuelAtReset = TempVar*10;
						sAfe.u8ReadCountTripB = FIRST_READ_AFTER_RESET;
					}
					else	// else only increment the readcount for offset calculation. 
					{
						sAfe.u8ReadCountTripB = FIRST_READ_AFTER_RESET;
					}
					bAfeBReset = FALSE;
					
				}
				else if((TempVar * 10) < sAfe.u32PreFuelConsumedB)
				{
					if(bAfeBReset == TRUE)
					{
						bAfeBReset = FALSE;
						sAfe.u8ReadCountTripB = NULL;
						sAfe.u32TripBUsedFuelAtReset = TempVar*10;
						sAfe.u32PreFuelConsumedB = TempVar*10;
						vSetAfeRstBStatus(eNoAFEReset);
					}
					
				}
				else
				{
					NOP();
				}
			}
			else if(sAfe.u8ReadCountTripB == SECOND_READ_COUNT)
			{
				if((TempVar * 10) > sAfe.u32TotalFuelConsumed)
				{				
					if(sAfe.u8EEPROMOffsetSaveFlagB == FALSE)
						vSetEEPROMOffsetSaveFlagB(TRUE);
				}
			}
		}
		else
			return;
		ClearPGNDataReady(eJAfe);			
	}
	
  /**************************************************************************************************************/	

	sAfe.u32TotalFuelConsumed = TempVar*10;	 // *10 for better resolution 
	
	if(u8FuelType == FUEL_TYPE_KMPL || u8FuelType == FUEL_TYPE_KMPKG) //vehicle type
	{
		//Afe Calculate for TRIP A
		//if fuel consumed value after reset is greater than fuel consumed at reset
		if(sAfe.u32TotalFuelConsumed >= sAfe.u32TripAUsedFuelAtReset)  
		{	
			// sAfe.u8EEPROMOffsetSaveFlagA flag represents the saving status of TRIPA offset in EEPROM
			// If TripB offset is saved in EEPROM then it become true. This flag is stored in EEPROM because 
			// if not saved in eeprom it again reinitiates itself to False, and we want this to be always TRUE 
			// after every reset.
			
			if(sAfe.u8EEPROMOffsetSaveFlagA == FALSE && sAfe.u8ReadCountTripA == FIRST_READ_AFTER_RESET) 
			{
				Temp1 = u32GetOdoTripAValue();
				Temp2 = u8GetOdoTripAFractionValue();
				vSetEEPROMOffsetSaveFlagA(TRUE);
				sAfe.u16TripAOffset = (Temp1 * 10) + Temp2; 
				
				// This flag is a boolean status for updating Trip Offset into EEPROM  
				// Flag becomes False at Offset Save in vSaveFullSystemParameters()			
				sAfe.bUpdateTripAOffset = TRUE;	
				
				// fuel consumption at reset is now equals to Last fuel consumed.
				sAfe.u32TripAUsedFuelAtReset = sAfe.u32TotalFuelConsumed;
				vSetUpdateAfeTripAFuelUsedAtReset(TRUE);
				
				// After Saving the offset and the fuel at reset the readcount now set for 2nd reading cycle.
				sAfe.u8ReadCountTripA = SECOND_READ_COUNT;
			}

			FuelDiffrence = sAfe.u32TotalFuelConsumed - sAfe.u32TripAUsedFuelAtReset;
		
			if(FuelDiffrence >= 50  									// * minimum fuel
				&& sAfe.u32PreFuelConsumedA < sAfe.u32TotalFuelConsumed	// * Wait for Fuel Consumed Greater 
																		//   than Previous Fuel Consumed
				&& sAfe.u8EEPROMOffsetSaveFlagA == TRUE )				// * When Offset HAs been Saved.		
				
			{
				Temp1 = u32GetOdoTripAValue();
				Temp2 = u8GetOdoTripAFractionValue();
				TempVar = (Temp1 * 10 + Temp2) - sAfe.u16TripAOffset;
				//Do afe calulation up to 3 decimal places 100 *1000
				TempVar = (TempVar*10000)/FuelDiffrence;
				//Multiply Afe mut factor up to 2 decimal place 
				TempVar = TempVar * (u32)u8AfeMulFactor;
				//remove two digit of Afe mul factor 
				TempVar = TempVar/100;

				// look second decimal place if non zero then do ceiling ;2 decimal place ex. Afe value 2.154 ;here 2 decimal place is 5 
				 Temp1 = TempVar /10;
				 Temp1 = Temp1 %10;
				 //if fuel cansume is zero and second decimal place is zero then dont do ceiling 
				 if(TempVar != 0 && Temp1 != 0) 
				 {
					 TempVar = TempVar + 100;
				 }				
				
				//remove extra 2 decimal 
				TempVar = TempVar/100;

				
				sAfe.u16TripAAfe = (TempVar);

				// If remainder of sAfe.u16TripAAfe/10 is not equals to zero means if the first decimal
				// place is not zero then increment the afe value by.1.

				sAfe.u32PreFuelConsumedA = sAfe.u32TotalFuelConsumed;
				sAfe.bUpdateTripA = TRUE;
				sAfe.bUpdatePreFuelConsumed = TRUE;	
				sAfe.bSaveFuelConsumedAtResetA = TRUE;
			}

		}
	
		if(sAfe.u32TotalFuelConsumed >= sAfe.u32TripBUsedFuelAtReset)  
		{							
			if(sAfe.u8EEPROMOffsetSaveFlagB == FALSE && sAfe.u8ReadCountTripB == FIRST_READ_AFTER_RESET) 
			{
				Temp1 = u32GetOdoTripBValue();
				Temp2 = u8GetOdoTripBFractionValue();
				vSetEEPROMOffsetSaveFlagB(TRUE);
				sAfe.u16TripBOffset = Temp1 * 10 + Temp2;  
				
				// This flag is a boolean status for updating Trip Offset into EEPROM  
				// Flag becomes False at Offset Save in vSaveFullSystemParameters()	 
				sAfe.bUpdateTripBOffset = TRUE;					
				sAfe.u32TripBUsedFuelAtReset = sAfe.u32TotalFuelConsumed;
				
				// fuel consumption at reset is now equals to Last fuel consumed.
				vSetUpdateAfeTripBFuelUsedAtReset(TRUE);
				sAfe.u8ReadCountTripB = SECOND_READ_COUNT;
			}
			//Afe Calculate for TRIP B
			FuelDiffrence = sAfe.u32TotalFuelConsumed - sAfe.u32TripBUsedFuelAtReset;
				
			if(FuelDiffrence >= 50  									// * minimum fuel
				&& sAfe.u32PreFuelConsumedB < sAfe.u32TotalFuelConsumed	// * Wait for Fuel Consumed Greater 
																		//   than Previous Fuel Consumed
				&& sAfe.u8EEPROMOffsetSaveFlagB == TRUE )				// * When Offset HAs been Saved.		
				
			{
				Temp1 = u32GetOdoTripBValue();
				Temp2 = u8GetOdoTripBFractionValue();
				TempVar = (Temp1 * 10 + Temp2) - sAfe.u16TripBOffset;
				//Do afe calulation up to 3 decimal places 100 *1000
				TempVar = (TempVar*10000)/FuelDiffrence;
				//Multiply Afe mut factor up to 2 decimal place 
				TempVar = TempVar * (u32)u8AfeMulFactor;
				//remove two digit of Afe mul factor 
				TempVar = TempVar/100;
				// look second decimal place if non zero then do ceiling ;2 decimal place ex. Afe value 2.154 ;here 2 decimal place is 5 
				 Temp1 = TempVar /10;
				 Temp1 = Temp1 %10;
				 //if fuel cansume is zero and second decimal place is zero then dont do ceiling 
				 if(TempVar != 0 && Temp1 != 0) 
				 {
					 TempVar = TempVar + 100;
				 }		
				//remove extra 2 decimal 
				TempVar = TempVar/100;				

				
				
				sAfe.u16TripBAfe = (TempVar);
				
				sAfe.u32PreFuelConsumedB = sAfe.u32TotalFuelConsumed;
				sAfe.bUpdateTripB = TRUE;
				sAfe.bUpdatePreFuelConsumed = TRUE;	
				sAfe.bSaveFuelConsumedAtResetB = TRUE;
			}
		}		
	}
	
	else if(u8FuelType == FUEL_TYPE_LPH)
	{	
		//Afe Calculate for TRIP A
		//if fuel consumed value after reset is greater than fuel consumed at reset
		if(sAfe.u32TotalFuelConsumed >= sAfe.u32TripAUsedFuelAtReset)  
		{		
			// Flag Becomes true at Offset Save
			if(sAfe.u8EEPROMOffsetSaveFlagA == FALSE && sAfe.u8ReadCountTripA == FIRST_READ_AFTER_RESET) 
			{
				sAfe.u16TripAOffset = (u16)u32GetTotalTripASeconds();
				
				// This flag is a boolean status for updating Trip Offset into EEPROM  
				// Flag becomes False at Offset Save in vSaveFullSystemParameters()	
				sAfe.bUpdateTripAOffset = TRUE;	
				sAfe.u32TripAUsedFuelAtReset = sAfe.u32TotalFuelConsumed;
				
				// fuel consumption at reset is now equals to Last fuel consumed.
				vSetUpdateAfeTripAFuelUsedAtReset(TRUE);
				sAfe.u8ReadCountTripA = SECOND_READ_COUNT;
			}
			
			FuelDiffrence = sAfe.u32TotalFuelConsumed - sAfe.u32TripAUsedFuelAtReset;	
		
			if(FuelDiffrence >= 50  									// * minimum fuel
				&& sAfe.u32PreFuelConsumedA < sAfe.u32TotalFuelConsumed	// * Wait for Fuel Consumed Greater 
																		//   than Previous Fuel COnsumed
				&& sAfe.u8EEPROMOffsetSaveFlagA == TRUE )				// * When Offset HAs been Saved.		
				
			{
				TempVar = u32GetTotalTripASeconds();	
				TempVar = TempVar - (sAfe.u16TripAOffset);
							
				//Do afe calulation up to 3 decimal places 100 *1000
				TempVar = ((FuelDiffrence*36000)/TempVar); 			
				//Multiply Afe mut factor up to 2 decimal place 
				TempVar = TempVar * (u32)u8AfeMulFactor;
				//remove two digit of Afe mul factor 
				TempVar = TempVar/100;

				// look second decimal place if non zero then do flooring ;2 decimal place ex. Afe value 2.154 ;here 2 decimal place is 5 
				 Temp1 = TempVar /10;
				 Temp1 = Temp1 %10;
				 //if fuel cansume is zero and second decimal place is zero then dont do flooring 
				 if(TempVar != 0 && Temp1 != 0) 
				 {
				   if(TempVar >= 100)
				  	{
					 TempVar = TempVar - 100;
				  	}
				 }			
				  
				//remove extra 2 decimal 
				TempVar = TempVar/100;
				
				
				
				sAfe.u16TripAAfe = TempVar;
				
				//AFE not display is Trip value is zero
				TempVar = u32GetHoursTripAValue();
				if(TempVar == 0)
				{
				 	TempVar = u8GetHoursTripAFractionValue();
					if(TempVar == 0)
					{
						sAfe.u16TripAAfe = 0;
					}
				}
					
				sAfe.u32PreFuelConsumedA = sAfe.u32TotalFuelConsumed;
				sAfe.bUpdateTripA = TRUE;
				sAfe.bUpdatePreFuelConsumed = TRUE;	
				sAfe.bSaveFuelConsumedAtResetA = TRUE;
			}			
		}

		//Afe Calculate for TRIP B
		//if fuel consumed value after reset is greater than fuel consumed at reset
		if(sAfe.u32TotalFuelConsumed >= sAfe.u32TripBUsedFuelAtReset)  
		{	
			// Flag Becomes true at Offset Save and becomes false at reset
			if(sAfe.u8EEPROMOffsetSaveFlagB == FALSE && sAfe.u8ReadCountTripB == FIRST_READ_AFTER_RESET)		
			{						
				sAfe.u16TripBOffset = (u16)u32GetTotalTripBSeconds();
				
				// This flag is a boolean status for updating Trip Offset into EEPROM  
				// Flag becomes False at Offset Save in vSaveFullSystemParameters()	
				sAfe.bUpdateTripBOffset = TRUE;				
				sAfe.u32TripBUsedFuelAtReset = sAfe.u32TotalFuelConsumed;
				
				// fuel consumption at reset is now equals to Last fuel consumed.
				vSetUpdateAfeTripBFuelUsedAtReset(TRUE);
				sAfe.u8ReadCountTripB = SECOND_READ_COUNT;
			}
			//Afe Calculate for TRIP B
			FuelDiffrence = sAfe.u32TotalFuelConsumed - sAfe.u32TripBUsedFuelAtReset;
						
			if(FuelDiffrence >= 50  									// * minimum fuel
				&& sAfe.u32PreFuelConsumedB < sAfe.u32TotalFuelConsumed	// * Wait for Fuel Consumed Greater 
																		//   than Previous Fuel COnsumed
				&& sAfe.u8EEPROMOffsetSaveFlagB == TRUE )				// * When Offset HAs been Saved.		
				
			{				
				TempVar = u32GetTotalTripBSeconds();	
				TempVar = TempVar - (sAfe.u16TripBOffset);
				//Do afe calulation up to 3 decimal places 100 *1000
				TempVar = ((FuelDiffrence*36000)/TempVar);				
				//Multiply Afe mut factor up to 2 decimal place 
				TempVar = TempVar * (u32)u8AfeMulFactor;
				//remove two digit of Afe mul factor 
				TempVar = TempVar/100;
				// look second decimal place if non zero then do flooring ;2 decimal place ex. Afe value 2.154 ;here 2 decimal place is 5 
				 Temp1 = TempVar /10;
				 Temp1 = Temp1 %10;
				 //if fuel cansume is zero and second decimal place is zero then dont do flooring 
				 if(TempVar != 0 && Temp1 != 0) 
				 {
				  if(TempVar > 100)
				  	{
					 TempVar = TempVar - 100;
				  	}
				 }			
				//remove extra 2 decimal 				  
				TempVar = TempVar/100;
				
				
				
				sAfe.u16TripBAfe = TempVar;
				
			
					

				//AFE not display is Trip value is zero
				TempVar = u32GetHoursTripBValue();
				if(TempVar == 0)
				{
				 	TempVar = u8GetHoursTripBFractionValue();
					if(TempVar == 0)
					{
						sAfe.u16TripBAfe = 0;
					}
				}
					
				sAfe.u32PreFuelConsumedB = sAfe.u32TotalFuelConsumed;
				sAfe.bUpdateTripB = TRUE;
				sAfe.bUpdatePreFuelConsumed = TRUE;	
				sAfe.bSaveFuelConsumedAtResetB = TRUE;
			}
		}
	}	

	if(sAfe.u16TripAAfe > u16AFEMaxValue) //Afe max limit
		sAfe.u16TripAAfe = u16AFEMaxValue;
	if(sAfe.u16TripBAfe > u16AFEMaxValue) //Afe max limit
		sAfe.u16TripBAfe = u16AFEMaxValue;
	sAfe.u16DispTripAAfe = sAfe.u16TripAAfe;
	sAfe.u16DispTripBAfe = sAfe.u16TripBAfe;	
}


/*********************************************************************//**
 *
 * Total Fuel Consumed
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u32	u32GetTotalFuelConsumed()
{
	return sAfe.u32TotalFuelConsumed;
}

/*********************************************************************//**
 *
 * Get Pre Fuel ConsumedA
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u32	u32GetPreFuelConsumedA()
{
	return sAfe.u32PreFuelConsumedA;
}		

/*********************************************************************//**
 *
 * Get Pre Fuel ConsumedB
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u32	u32GetPreFuelConsumedB()
{
	return sAfe.u32PreFuelConsumedB;
}		

/*********************************************************************//**
 *
 * Get TripA Used Fuel At Reset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u32	u32GetTripAUsedFuelAtReset()
{
	return sAfe.u32TripAUsedFuelAtReset;
}		

/*********************************************************************//**
 *
 * Get TripB Used Fuel At Reset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u32	u32GetTripBUsedFuelAtReset()
{
	return sAfe.u32TripBUsedFuelAtReset;
}

/*********************************************************************//**
 *
 * Get TripA Offset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16 u16GetTripAOffset()
{
	return sAfe.u16TripAOffset;
}		

/*********************************************************************//**
 *
 * Get TripB Offset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16 u16GetTripBOffset()
{
	return sAfe.u16TripBOffset;
}	

/*********************************************************************//**
 *
 * Get Trip A AFE value
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16	u16GetTripAAfe()
{
	return sAfe.u16TripAAfe;
}		

/*********************************************************************//**
 *
 * Get Trip B AFE value
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16	u16GetTripBAfe()
{
	return sAfe.u16TripBAfe;
}			


/*********************************************************************//**
 *
 * Get Display Trip B AFE value
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16 u16GetDispTripAAfe()
{
	return sAfe.u16DispTripAAfe;
}

/*********************************************************************//**
 *
 * Get Display Trip B AFE value
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u16 u16GetDispTripBAfe()
{
	return sAfe.u16DispTripBAfe;
}

/*********************************************************************//**
 *
 * Get Read Count
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u8 	u8GetReadCountTripA()
{
	return sAfe.u8ReadCountTripA;
}		

/*********************************************************************//**
 *
 * Get Read Count
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u8 	u8GetReadCountTripB()
{
	return sAfe.u8ReadCountTripB;
}

/*********************************************************************//**
 *
 * Get EEPROM Offset Save FlagA
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u8	u8GetEEPROMOffsetSaveFlagA()
{
	return sAfe.u8EEPROMOffsetSaveFlagA;
}

/*********************************************************************//**
 *
 * Get EEPROM Offset Save FlagB
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
u8	u8GetEEPROMOffsetSaveFlagB()
{
	return sAfe.u8EEPROMOffsetSaveFlagB;
}	

/*********************************************************************//**
 *
 * Get Update TripA Fuel Used At Reset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripAFuelUsedAtReset()
{
	return sAfe.bUpdateTripAFuelUsedAtReset;
}		

/*********************************************************************//**
 *
 * Get Update TripB Fuel Used At Reset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripBFuelUsedAtReset()
{
	return sAfe.bUpdateTripBFuelUsedAtReset;
}		

/*********************************************************************//**
 *
 * Get Update Pre Fuel Consumed
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdatePreFuelConsumed()
{
	return sAfe.bUpdatePreFuelConsumed;
}		


/*********************************************************************//**
 *
 * Get Update TripA
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripA()
{
	return sAfe.bUpdateTripA;
}		

/*********************************************************************//**
 *
 * Get Update TripB
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripB()
{
	return sAfe.bUpdateTripB;
}		

/*********************************************************************//**
 *
 * Get Update TripA Offset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripAOffset()
{
	return sAfe.bUpdateTripAOffset;
}		

/*********************************************************************//**
 *
 * Get Update TripB Offset
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetUpdateTripBOffset()
{
	return sAfe.bUpdateTripBOffset;
}		

/*********************************************************************//**
 *
 * Get Save Fuel Consumed At ResetA
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetSaveFuelConsumedAtResetA()
{
	return sAfe.bSaveFuelConsumedAtResetA;
}		

/*********************************************************************//**
 *
 * Get Trip B AFE value
 *
 * @param      None    
 *
 * @return     AFE value
 *************************************************************************/
bool bGetSaveFuelConsumedAtResetB()
{
	return sAfe.bSaveFuelConsumedAtResetB;
}		


/*********************************************************************//**
 *
 * SetPreFuelConsumedA
 *
 * @param      PreFuelConsumedA    
 *
 * @return     None
 *************************************************************************/
void vSetPreFuelConsumedA(u32 data)
{
	sAfe.u32PreFuelConsumedA = data;	
}		

/*********************************************************************//**
 *
 * SetPreFuelConsumedB
 *
 * @param      PreFuelConsumedB   
 *
 * @return     None
 *************************************************************************/
void vSetPreFuelConsumedB(u32 data)
{
	sAfe.u32PreFuelConsumedB = data;	
}		

/*********************************************************************//**
 *
 * SetTripAUsedFuelAtReset
 *
 * @param      TripAUsedFuelAtReset    
 *
 * @return     None
 *************************************************************************/
void vSetTripAUsedFuelAtReset(u32 data)
{
	sAfe.u32TripAUsedFuelAtReset = data;	
}			 

/*********************************************************************//**
 *
 * SetTripBUsedFuelAtReset
 *
 * @param      TripBUsedFuelAtReset    
 *
 * @return     None
 *************************************************************************/
void vSetTripBUsedFuelAtReset(u32 data)
{
	sAfe.u32TripBUsedFuelAtReset = data;	
}

/*********************************************************************//**
 *
 * SetTripAOffset
 *
 * @param      TripAOffset    
 *
 * @return     None
 *************************************************************************/
void vSetTripAOffset(u16 data)
{
	sAfe.u16TripAOffset = data;	
}		

/*********************************************************************//**
 *
 * SetTripBOffset
 *
 * @param      TripBOffset    
 *
 * @return     None
 *************************************************************************/
void vSetTripBOffset(u16 data)
{
	sAfe.u16TripBOffset = data;	
}		

/*********************************************************************//**
 *
 * SetTripAAfe
 *
 * @param      TripAAfe    
 *
 * @return     None
 *************************************************************************/
void vSetTripAAfe(u16 data)
{
	sAfe.u16TripAAfe = data;	
}	

/*********************************************************************//**
 *
 * SetTripBAfe
 *
 * @param      TripBAfe    
 *
 * @return     None
 *************************************************************************/
void vSetTripBAfe(u16 data)
{
	sAfe.u16TripBAfe = data;	
}

/*********************************************************************//**
 *
 * SetDispTripAAfe
 *
 * @param      DispTripAAfe    
 *
 * @return     None
 *************************************************************************/
void vSetDispTripAAfe(u16 data)
{
	sAfe.u16DispTripAAfe = data;	
}		

/*********************************************************************//**
 *
 * SetDispTripBAfe
 *
 * @param      DispTripBAfe    
 *
 * @return     None
 *************************************************************************/
void vSetDispTripBAfe(u16 data)
{
	sAfe.u16DispTripBAfe = data;	
}	

/*********************************************************************//**
 *
 * SetReadCount
 *
 * @param      ReadCount    
 *
 * @return     None
 *************************************************************************/
void vSetReadCountTripA(u8 data)
{
	sAfe.u8ReadCountTripA = data;	
}	

/*********************************************************************//**
 *
 * SetReadCount
 *
 * @param      ReadCount    
 *
 * @return     None
 *************************************************************************/
void vSetReadCountTripB(u8 data)
{
	sAfe.u8ReadCountTripB = data;	
}	

/*********************************************************************//**
 *
 * SetEEPROMOffsetSaveFlagA
 *
 * @param      EEPROMOffsetSaveFlagA    
 *
 * @return     None
 *************************************************************************/
void vSetEEPROMOffsetSaveFlagA(u8 data)
{
	sAfe.u8EEPROMOffsetSaveFlagA = data;	
}		

/*********************************************************************//**
 *
 * SetEEPROMOffsetSaveFlagB
 *
 * @param      EEPROMOffsetSaveFlagB    
 *
 * @return     None
 *************************************************************************/
void vSetEEPROMOffsetSaveFlagB(u8 data)
{
	sAfe.u8EEPROMOffsetSaveFlagB = data;	
}		

/*********************************************************************//**
 *
 * SetUpdateTripAFuelUsedAtReset
 *
 * @param      UpdateTripAFuelUsedAtReset    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateAfeTripAFuelUsedAtReset(bool data)
{
	sAfe.bUpdateTripAFuelUsedAtReset = data;	
}	

/*********************************************************************//**
 *
 * SetUpdateTripBFuelUsedAtReset
 *
 * @param      UpdateTripBFuelUsedAtReset    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateAfeTripBFuelUsedAtReset(bool data)
{
	sAfe.bUpdateTripBFuelUsedAtReset = data;	
}		

/*********************************************************************//**
 *
 * SetUpdatePreFuelConsumed
 *
 * @param      UpdatePreFuelConsumed    
 *
 * @return     None
 *************************************************************************/
void vSetUpdatePreFuelConsumed(bool data)
{
	sAfe.bUpdatePreFuelConsumed = data;	
}	

/*********************************************************************//**
 *
 * SetUpdateTripA
 *
 * @param      UpdateTripA    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateAfeTripA(bool data)
{
	sAfe.bUpdateTripA = data;	
}		

/*********************************************************************//**
 *
 * SetUpdateTripB
 *
 * @param      UpdateTripB    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateAfeTripB(bool data)
{
	sAfe.bUpdateTripB = data;	
}		

/*********************************************************************//**
 *
 * SetUpdateTripAOffset
 *
 * @param      UpdateTripAOffset    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateTripAOffset(bool data)
{
	sAfe.bUpdateTripAOffset = data;	
}		

/*********************************************************************//**
 *
 * SetUpdateTripBOffset
 *
 * @param      UpdateTripBOffset    
 *
 * @return     None
 *************************************************************************/
void vSetUpdateTripBOffset(bool data)
{
	sAfe.bUpdateTripBOffset = data;	
}	

/*********************************************************************//**
 *
 * SetSaveFuelConsumedAtResetA
 *
 * @param      SaveFuelConsumedAtResetA    
 *
 * @return     None
 *************************************************************************/
void vSetSaveFuelConsumedAtResetA(bool data)
{
	sAfe.bSaveFuelConsumedAtResetA = data;	
}	

/*********************************************************************//**
 *
 * SetSaveFuelConsumedAtResetB
 *
 * @param      SaveFuelConsumedAtResetB    
 *
 * @return     None
 *************************************************************************/
void vSetSaveFuelConsumedAtResetB(bool data)
{
	sAfe.bSaveFuelConsumedAtResetB = data;	
}	


