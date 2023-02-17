
#include "customTypedef.h"
#include "J1939.h"
#include "FuelGauge.h" 
#include "AirPressureGauge.h" 
#include "TellTales.h"
#include "Eol.h" 
#include "Afe.h" 
#include "r_cg_userdefine.h"
#include "LCDScreens.h"
#include "ServiceReminder.h"



/*********************************************************************//**
 *
 * Write CAN data to tx buffer
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 void vWriteToCANTxArray(void)
{
	u32 TempVar = 0U;
	u32 Temp1 = 0U;
	u32 Temp2 = 0U;
	u8 u8FuelType = 0U;
		
	u8FuelType = u8GetVehicleType();
	//ODO data prepare for transmmiting over CAN 
	Temp1 = u32GetOdoValue();
	Temp2 = u8GetOdoFractionValue();
	TempVar = (Temp1 * 10U) + Temp2;	
	vPutPGNdata(eJOdoDataTx,TempVar,0U);
	//ODO Trip A data prepare for transmmiting over CAN 
	Temp1 = u32GetOdoTripAValue();
	Temp2 = u8GetOdoTripAFractionValue();
	TempVar = (Temp1 * 10U) + Temp2;	
	vPutPGNdata(eJOdoTripATx,TempVar,0U);
	//ODO Trip B data prepare for transmmiting over CAN 
	Temp1 = u32GetOdoTripBValue();
	Temp2 = u8GetOdoTripBFractionValue();
	TempVar = (Temp1 * 10U) + Temp2;	
	vPutPGNdata(eJOdoTripBTx,TempVar,0U);
	
	//AFE tx data**************************************
	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayAfe == eEnable)
	{
		// L/Hr Afe tx 
		if(u8FuelType == FUEL_TYPE_LPH)
		{
			TempVar = u16GetDispTripAAfe();	
			vPutPGNdata(eJTripAFuelEffHrsTx,TempVar,0U); 
			TempVar = u16GetDispTripBAfe();	
			vPutPGNdata(eJTripBFuelEffHrsTx,TempVar,0U); 
		}
		else //If AFE L/Hr disable then tx 0xffffffff for Afe Trip A & B
		{
			TempVar = 0xFFFFFFFFU; 
			vPutPGNdata(eJTripAFuelEffHrsTx,TempVar,0U); 		
			vPutPGNdata(eJTripBFuelEffHrsTx,TempVar,0U); 	
		}
		//Km/L Afe Tx
		if(u8FuelType == FUEL_TYPE_KMPL || u8FuelType == FUEL_TYPE_KMPKG)
	 	{
			TempVar = u16GetDispTripAAfe();
			vPutPGNdata(eJTripAFuelEffKmTx,TempVar,0U); 
			TempVar = u16GetDispTripBAfe();
			vPutPGNdata(eJTripBFuelEffKmTx,TempVar,0U);
	 	}
		else //If AFE Km/l disable then tx 0xffffffff for Afe Trip A & B
		{
			TempVar = 0xFFFFFFFFU;
			vPutPGNdata(eJTripAFuelEffKmTx,TempVar,0U);		
			vPutPGNdata(eJTripBFuelEffKmTx,TempVar,0U);
		}
	}
	else //If both AFE Km/l or L/Hr disable then tx 0xffffffff for Afe Trip A & B
	{
		TempVar = 0xFFFFFFFFU;
		vPutPGNdata(eJTripAFuelEffHrsTx,TempVar,0U); 		
		vPutPGNdata(eJTripBFuelEffHrsTx,TempVar,0U);
		vPutPGNdata(eJTripAFuelEffKmTx,TempVar,0U);		
		vPutPGNdata(eJTripBFuelEffKmTx,TempVar,0U);	
	}
	//*****TRIP Reset status ************************************	
	if(u8GetOdoTripRstStatus() == eOdoTripResetCANTx)
	{
		TempVar = 1U;
		vPutPGNdata(eJTripResetTx,TempVar,0U);
		// Enable send on demand 
		vJ1939_TxSendOnDemandEnable(eTx56832);
		vSetOdoTripRstStatus(eNoOdoReset);		
	}
	
	//*****Air pressure Front & Rear  ************************************		
	TempVar = u8ApfBarNo();
	
	vPutPGNdata(eJAirPressFrontTx,TempVar,0U);
	
	TempVar = u8AprBarNo();
	vPutPGNdata(eJAirPressRearTx,TempVar,0U);

	//*****Hour ,TRAP _A & TRIP_B *************************************	

	Temp1 = u32GetHoursTripAValue();
	Temp2 = u8GetHoursTripAFractionValue();	
	TempVar = (Temp1*10U) + Temp2;	
	vPutPGNdata(eJHourTripATx,TempVar,0U);

	Temp1 = u32GetHoursTripBValue();
	Temp2 = u8GetHoursTripBFractionValue();	
	TempVar = (Temp1*10U) + Temp2;
	vPutPGNdata(eJHourTripBTx,TempVar,0U);
	
	Temp1 = u32GetHoursValue();
	Temp2 = u8GetHoursFractionValue();	
	TempVar =  (Temp1*10U) + Temp2;	
	vPutPGNdata(eJHourDataTx,TempVar,0U);

	// vPutPGNdata(eJDTETx,TempVar,0); //reserve for future

	//*****Lamp Status   *********************************************	

	TempVar = bClutchWearLampStatus();
	vPutPGNdata(ejClutchWearTx,TempVar,0U);

	TempVar = bBrakeWearLampStatus();
	vPutPGNdata(ejBreakWearTx,TempVar,0U);

	TempVar = bGetSeatBeltStatus();
	vPutPGNdata(eJSeatBeltTx,TempVar,0U);
	
	TempVar = bGetBatChargingStatus();
	vPutPGNdata(eJBattChargingTx,TempVar,0U);

	TempVar = bGetGenIllStatus();
	vPutPGNdata(eJGenIllTx,TempVar,0U);

	TempVar = bGetBatCharging2Status();
	vPutPGNdata(eJBattCharging2Tx,TempVar,0U);
	
	TempVar = bGetTitlCabStatus();
	vPutPGNdata(eJTiltCabTx,TempVar,0U);
		
	TempVar = bGetAirFilterCloggedStatus();
	vPutPGNdata(eJAirFilterCloggedTx,TempVar,0U);
	
	TempVar = bGetAbsMilTrailerStatus();
	vPutPGNdata(eJABSMalfunctionTx,TempVar,0U);

	//****Tx Service remnder warning status & Due service data*********************
	TempVar = (eSERVICE_REMINDER_TYPE)eGetServiceStatus();
	if( (TempVar < eServRemdNoDisp ) 
		&&  (sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayServHrSymbol == eEnable) )
	{
		TempVar = 1U;
	}
	else
	{
		TempVar = 0U;	// if service reminder not occure or disable then tx 0
	}
	vPutPGNdata(eJServiceIndicatorTx,TempVar,0U);
	// Tx service due data
	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayServHrSymbol == eEnable)
	{
		TempVar = u32GetNextDueService();
		if(sIndicatorInputType.Inputsbytes.Byte3.bServiceType == eHourBasedService)
		{
		 	Temp1 = u32GetHoursValue();
			if(Temp1 <= TempVar)
			{
				TempVar = TempVar - Temp1;
			}
			else
			{
			 	TempVar = 0U;	
			}
				
		}else
		{
			Temp1 = u32GetOdoValue();
			if(Temp1 <= TempVar)
			{
				TempVar = TempVar - Temp1;
			}
			else
			{
			 TempVar = 0U;	
			}
		}
	}
	else
	{
		TempVar = 0xFFFFFFFFU; //if service disable then tx 
	}
	
	vPutPGNdata(ejDistanceTONextServiceTx,TempVar,0U);

	//*******fuel data********************************************
	TempVar = u8GetDualFuelLevel();
	vPutPGNdata(eJDualFuelLevelTx,TempVar,0U);
	TempVar =  u8GetFuelLevel();
	vPutPGNdata(eJFuelLevelTx,TempVar,0U);
	Temp1 = TempVar;	
	TempVar = u16GetVoltValue();
	if(TempVar != 0xFFFU) //if value is 0xfff means gauge is disable 
	{
		if(Temp1 == 0xFEU) //if Capacitive fuel gauge is open ckt then tx 0 
		{
		  TempVar = 0U;
		}
		else
		{
	       TempVar = TempVar/10U;
		}
	}
	vPutPGNdata(eJCapacitiveFuelTx,TempVar,0U);	
	
	
	//*******Request for AFE data	 ************************************
	vPutPGNdata(eJAfeTx,PGN65257,0U);
		

 }

