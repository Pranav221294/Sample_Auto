#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h" 
#include "customTypedef.h"
#include "J1939.h" 
#include "Lcd.h"
#include "r_cg_userdefine.h"
#include "TellTales.h"
#include "Priority2Texts.h"
#include "Amt.h"

#define FIVE_SECONDS 					5000U
#define FOUR_SECONDS 					4000U
#define SIX_SECONDS 					6000U


//Defining DTC_SPNs as per Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls "
#define	DTC_SPN167	167U
#define	DTC_SPN168	168U
#define	DTC_SPN1761	1761U
#define	DTC_SPN3361	3361U
#define	DTC_SPN3362	3362U
#define	DTC_SPN3364	3364U
#define	DTC_SPN3515	3515U
#define	DTC_SPN4096	4096U
#define	DTC_SPN4331	4331U
#define	DTC_SPN5394	5394U
#define	DTC_SPN5838	5838U
#define	DTC_SPN5839	5839U
#define	DTC_SPN5841	5841U
#define	DTC_SPN5840	5840U
#define	DTC_SPN5842	5842U
#define	DTC_SPN7606	7606U
#define	DTC_SPN6802	6802U
#define	DTC_SPN7607	7607U
#define DTC_SPN3521 3521U


//Defining DTC_FMIs as per Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls "
#define	DTC_FMI1	1U
#define	DTC_FMI2	2U
#define	DTC_FMI3	3U
#define	DTC_FMI4	4U
#define	DTC_FMI5	5U

#define	DTC_FMI7	7U

#define	DTC_FMI9	9U
#define	DTC_FMI10	10U
#define	DTC_FMI11	11U
#define	DTC_FMI13	13U
#define	DTC_FMI16	16U
#define	DTC_FMI17	17U
#define	DTC_FMI18	18U
#define	DTC_FMI31	31U

// Service regen Macros for new algorithm.
#define SERVICE_REGEN_ACTIVE 	1U

#define REGEN_COMPLETE			3U
#define REGEN_FAILED			4U


// Arrays to hold Sub Priority Fault Indexes.
static eSP1Faults eSP1FaultArray[eTotalSP1Faults];
static eSP2Faults eSP2FaultArray[eTotalSP2Faults];

static void _vEtc7TextWarning(void); 
static void _vHillHolderText(void); 
static void _vDoorOpenText(void); 
static void _vCBCUWarningFault(void);
static void _vDPFRegenerationWarning(void);
static void _vDm1EMSTextWarning(void);

static eSP1Faults eSP1Index = eSP1Blank;
static eSP2Faults eSP2Index = eSP2Blank;
static bool bcheckEnginestatus =FALSE;
static bool bStopEnginestatus =FALSE;
static bool bScrstatus =FALSE;
static bool bMilMode4status =FALSE;
static bool bMilMode3status =FALSE;
static u8 u8Length = 0U;
static u32 u32TimeStamp = 0U;	



/*********************************************************************//**
*
* Read All DTCs and raise the fault flags according to availability.
*
* @param	  None 	  
*
* @return	  None
*************************************************************************/

void vSetEMSFaultText(void)
{

	
	static bool bWait = FALSE;
	static bool bRegenStopPressed = FALSE;
	
	u8 i = 0U;	
	eCLUSTER_STATE_MACHINE clusterStatusTopLineText;
	
	clusterStatusTopLineText = eGetClusterStatus();

	
	if(clusterStatusTopLineText == eClusterIgnitionReset)
	{
		return;
	}
	else
	{
		//do nothing
	}

	/*
		Check current priority before changing it to Priority 2.
		If higher priority is present then return from the function, no execution required.
	*/

	if((eGetTopLinePriority() !=  ePriority1) )
	{		
		vSetTopLinePriority(ePriority2);	
	}
	else
	{
		return;
	}

	//Read check engine , Stop engine and Mil Mode 3 & 4 status 
	//Must read these status before calling others warning  function 	
	bStopEnginestatus = bGetStopEngineStatus();
	bcheckEnginestatus = bGetCheckEngineStatus();
	bScrstatus = bGetScrLampStatus();
	bMilMode4status = bGetMilLampMode4Status();
	bMilMode3status = bGetMilLampMode3Status();
	
	// To queue all the non ems-faults, we need to monitor PGN 65303/64964/65102 timeout.
	// PGN 85098 ETC7 AMT LEARNING , RANGE INHIBIT 
	_vEtc7TextWarning();
	
	// Monitor PGN 64964 for hill holder faults.	
	_vHillHolderText(); 

	// Monitor PGN 65102 for Door Open faults.
	_vDoorOpenText(); 

	// Monitor PGN 65303 for warning faults.
	_vCBCUWarningFault();


	_vDm1EMSTextWarning();

	// If No timeOut for DPFC1 PGN......
	_vDPFRegenerationWarning();
	
	if(bWait == FALSE)
	{
		u32TimeStamp = u32GetClockTicks();
		bWait = TRUE;
	}
	else 
	{
		/*Do Nothing*/
	}

	if(eSP1Index <= eTotalSP1Faults)
	{
		// Switch Text at every 4 second Interval and if the Text in not blank.
		if(((u32GetClockTicks() - u32TimeStamp) < FOUR_SECONDS) && (eSP1FaultArray[eSP1Index] != 0))
		{
			
			vSetTopLinePriority(ePriority2);					
			vSetTopLineSubPriority(eSubPriority1);
			vSetTopLineText(eSP1FaultArray[eSP1Index]);
		}
		else	// If text is blank or 4 seconds interval finished increment the Text Index.
		{
			eSP1Index++;
			u32TimeStamp = u32GetClockTicks();
		}

		if(eSP1Index == eTotalSP1Faults)
		{
			for(i = 0u ; i < eTotalSP1Faults; i++)
			{
				eSP1FaultArray[i] = 0u;
			}
			eSP1Index++;
			vSetTopLineSubPriority(eNoSubPriority);
		}
	}
	else
	{
		
		for(i = 0 ; i < (u8)eTotalSP1Faults; i++)
		{
			if(eSP1FaultArray[i] != 0u)
			{
				eSP1Index = 0u;
				return;
			}
		}
		// Total Number of SP2 texts available is 25			
		if(eSP2Index < eTotalSP2Faults)
		{	
			if(((u32GetClockTicks() - u32TimeStamp) < FOUR_SECONDS) && (eSP2FaultArray[eSP2Index] != 0u ))
			{
				vSetTopLinePriority(ePriority2);					
				vSetTopLineSubPriority(eSubPriority2);
				vSetTopLineText(eSP2FaultArray[eSP2Index]);
			}			
			else	// If text is blank or 4 seconds interval finished increment the Text Index.
			{
				eSP2Index++;
				u32TimeStamp = u32GetClockTicks();
			}
		}
		else
		{				
						
			vSetTopLineText(eSP2FaultArray[eSP2Blank]);
			vSetTopLinePriority(eBlank);					
			vSetTopLineSubPriority(eNoSubPriority);				
			eSP2Index = 0U;
			eSP1Index = 0U;
			u8Length = 0U;
			for(i = 0 ; i < (u8)eTotalSP1Faults; i++)
			{
				eSP1FaultArray[i] = 0u;
			}
			for(i = 0 ; i < (u8)eTotalSP2Faults; i++)
			{
				eSP2FaultArray[i] = 0u;
			}
		}
		
	}
		
}

static void _vEtc7TextWarning(void)
{

	u8 TempVar1 = 0U;
	u8 u8TransType = 0;

	if((!bGetPGNTimeOut(eJETC7)) && (bGetTransmissionEnable()))
	{
		u8TransType = u8GetTransmissionType();
		TempVar1 = eGetAMTLearningStatus();
		if((TempVar1 == eLearnActive) && (u8TransType == AMT_TRANS))
		{
			eSP1FaultArray[eAMTLearning] = eAMTLearning;
		}
		else
		{
			/*do nothing*/
		}

		TempVar1 = (u8)u32GetPGNdata(eJShiftInhibit);
		
		
		if((TempVar1 == 1U) && (u8TransType == AT_TRANS))
		{	
			eSP1FaultArray[eRangeInhibit] = eRangeInhibit;	
		}
		else
		{	
			/*do nothing*/	
		}
	}
	else
	{
		;
	}


}

static void _vHillHolderText(void)
{

	u32 TempVar = 0U;

	if(!bGetPGNTimeOut(eJHillHolder))
	{
		TempVar = u32GetPGNdata(eJHillHolder);
		if((TempVar == 1U) || (TempVar == 2U))
		{
			eSP1FaultArray[eHSAActive] = eHSAActive;
		}
		else if(TempVar == 6U)
		{
			eSP2FaultArray[eHSAError] = eHSAError;
		}
		else
		{
			/*do nothing*/
		}
	}
	else
	{
		/*do nothing*/
	}
}

static void _vDoorOpenText()
{
	u32 TempVar = 0U;
	
	if(!bGetPGNTimeOut(eJDoorOpen))
	{
		TempVar = u32GetPGNdata(eJDoorOpen);
		if(TempVar == 0u)
		{
			eSP2FaultArray[eDoorOpen] = eDoorOpen;
		}
		else
		{
			/*do nothing*/
		}
	}
	else
	{
		/*do nothing*/
	}
}

static void _vCBCUWarningFault(void)
{
	u32 TempVar = 0U;

	if(!bGetPGNTimeOut(eJHaltRequest))
	{
		TempVar = u32GetPGNdata(eJHaltRequest);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eHaltRequest] = eHaltRequest;
		}
		else
		{
			/*do nothing*/
		}
		TempVar = u32GetPGNdata(eJRampEngaged);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eRampEngaged] = eRampEngaged;
		}
		else
		{
			/*do nothing*/
		}
		
		TempVar = u32GetPGNdata(eJKneelingFront);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eKneelingFront] = eKneelingFront;
		}
		else
		{
			/*do nothing*/
		}
		
		TempVar = u32GetPGNdata(eJKneelingRear);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eKneelingRear] = eKneelingRear;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJKneelingBoth);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eKneelingBoth] = eKneelingBoth;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJEngineOilLow);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eEngineOilLow] = eEngineOilLow;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJEngineCoolantLow);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eEngineCoolantLow] = eEngineCoolantLow;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJEmergencyExit);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eEmergencyExit] = eEmergencyExit;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJHaltRequestPriority);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eHaltRqstPriority] = eHaltRqstPriority;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJSteeringAdjust);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eSteeringAdjust] = eSteeringAdjust;
		}
		else
		{
			; /*do nothing*/
		}

		TempVar = u32GetPGNdata(eJRearFlapOpen);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eRearFlapOpen] = eRearFlapOpen;
		}
		else
		{	
			; /*do nothing*/	
		}

		TempVar = u32GetPGNdata(eJSideFlapOpen);
		if(TempVar == TRUE)
		{
			eSP2FaultArray[eSideFlapOpen] = eSideFlapOpen;
		}
		else
		{
			/*do nothing*/
		}
		
		TempVar = u32GetPGNdata(eJKneelingRight);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eKneelingRight] = eKneelingRight;
		}
		else
		{
			/*do nothing*/
		}

		TempVar = u32GetPGNdata(eJKneelingLeft);
		if(TempVar == TRUE)
		{
			eSP1FaultArray[eKneelingLeft] = eKneelingLeft;
		}
		else
		{
			/*do nothing*/
		}

	}
	else
	{
		; /*	Do nothing*/
	}

}

static void _vDm1EMSTextWarning(void)
{

	static u8 u8CheckAWLStatus = 0U;
	static u8 u8CheckAWLFlash = 0U;
	static u8 u8StopEngStatus = 0U;
	static u8 u8StopEngineFlash = 0U; ////Edit by vishnu
	
	u32 TempVar = 0U;
	u8 TempVar1 = 0U;
	//u8 i = 0U;
	u16 u16TotalSpns = 0U;

	// Check whether Multiframe status is TRUE bGetDTCStatus() or 
	// Single Frame TimeOUT is FALSE !bGetPGNTimeOut(eJDM1DTC_spn1)
	if( ( u8GetDTCStatus()) || ( !bGetPGNTimeOut(eJDM1DTC_spn1) ) )
	{	
		// If any of the above condition is True, that means PCOdes are available on DM1

		// if Multiframe status is True.
		if(u8GetDTCStatus())
		{	
			u16TotalSpns = u16GetDtcTotalBytes(); // Get Total Number of Data BYtes in mutiframe
			
			if(u16TotalSpns > 8U) // If total number bytes is more than 8(for a valid multiframe)
			{
				u16TotalSpns = (u16TotalSpns - 2U)/4U;   // Calculate total no. of SPN and FMI combination
				if(u16TotalSpns > MAXIMUM_NUMBER_OF_DTC) // Capping total number to 38
				{
					u16TotalSpns = MAXIMUM_NUMBER_OF_DTC;
				}
			}
			else // If multifrmae is not available then Single might be available.
			{
				u16TotalSpns = 0U; // Keep this zero to count only one SPN and FMI.
			}
		}
		

		// Searching the DTC_J1939 Structure to compare SPN FMI combination with Respective Pcodes 
		// and Queuing them in the array eSP2FaultArray. This has been kept in while loop because
		// it will take very less time to scan maximum Pcodes that is 38 Nos. and After Gathering all 
		// the Faults, this while loop will be executed after all the queued faults will be displayed.
		while(u8Length <= (u8)u16TotalSpns)
		{
			R_WDT_Restart();
		
			if(!bGetPGNTimeOut(eJCheckEngineDM1_624))
			{	
				if((u16TotalSpns > 0U) && (u8Length < (u8)u16TotalSpns))
				{
					TempVar = DTC_J1939[u8Length].SPNValue;
					TempVar1 = DTC_J1939[u8Length].FMICode;
				}
				else
				{
					TempVar = 0U;
					TempVar1 = 0U;
					u8Length = MAXIMUM_NUMBER_OF_DTC;
				}				
			}
			else if(!bGetPGNTimeOut(eJDM1DTC_spn1))			// No single Frame TimeOut PGN 65226 SA 00
			{
				// Extract SPN value from DM1 single frame.
				TempVar = (u32GetPGNdata(eJDM1DTC_spn1)) | (u32GetPGNdata(eJDM1DTC_spn2)<< 8) | ((u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U)<<11);				
				// Get FMI data from DM1 single frame.
				TempVar1 = (u8)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
			}
			else
			{ 
				;/* do nothing*/
			}

			// Identifing the SPN-FMI combination as suggested in 
			// Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls ".
			//-------------------------------------------------------------------------------------
			// Switching SPN and Identifing FMIs results in corresponding text values to their 
			// respective indexes in eSP2FaultArray[].

			 if( (bStopEnginestatus == TRUE ) || (bcheckEnginestatus == TRUE ) || (bScrstatus == TRUE) 
				|| (bMilMode3status == TRUE) || (bMilMode4status == TRUE) )
				{
					
					switch(TempVar)
					{
						case DTC_SPN5838:
							
							if(TempVar1 == DTC_FMI7)
							{
								eSP2FaultArray[eEgrStopped] = eEgrStopped;						
							}
							
						break;

						case DTC_SPN5840:
							
							if(TempVar1 == DTC_FMI7)
							{
								eSP2FaultArray[eDefDosingStopped] = eDefDosingStopped;	
								eSP2FaultArray[eDefDosingFault] = eDefDosingFault;	
							}
							else if(TempVar1 == DTC_FMI31)
							{
								eSP2FaultArray[eDefDosingStopped] = eDefDosingStopped;						
							}
							else
							{
							 ;
							}
							
						break;
							
					case DTC_SPN5394:
						
						if( TempVar1 == DTC_FMI5 | TempVar1 == DTC_FMI7 )
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;					
						}
						
					break;

					case DTC_SPN3362:
						
						if( TempVar1 == DTC_FMI31 )
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;					
						}
						
					break;
					case DTC_SPN3361:
						
						if( TempVar1 == DTC_FMI3 || TempVar1 == DTC_FMI4 )
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;					
						}
						
					break;
					case DTC_SPN5839:
						
						if( TempVar1 == DTC_FMI31 )
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;					
						}
						
					break;

					case DTC_SPN4331:
						
						if( TempVar1 == DTC_FMI16 || TempVar1 == DTC_FMI18 )
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;					
						}
						
					break;
					case DTC_SPN5841:
						
						if( TempVar1 == DTC_FMI31 )
						{
							eSP2FaultArray[eDefQualityNotOk] = eDefQualityNotOk;				
						}
						
					break;
					
					case DTC_SPN5842:
						
						if( TempVar1 == DTC_FMI31 )
						{
							eSP2FaultArray[eDefSCRElecFault] = eDefSCRElecFault;					
						}
						
					break;
					case DTC_SPN3364:
						
						if( TempVar1 == DTC_FMI3 || TempVar1 == DTC_FMI4 || TempVar1 == DTC_FMI2 
							|| TempVar1 == DTC_FMI9 || TempVar1 == DTC_FMI13 || TempVar1 == DTC_FMI11)
						{
							eSP2FaultArray[eDefSysElecFault] = eDefSysElecFault;			
						}
						
					break;	
					case DTC_SPN1761:
						
						if( TempVar1 == DTC_FMI3 || TempVar1 == DTC_FMI4 || TempVar1 == DTC_FMI9 || TempVar1 == DTC_FMI13 || TempVar1 == DTC_FMI10 )
						{
							eSP2FaultArray[eDefSysElecFault] = eDefSysElecFault;			
						}
												
					break;				
					case DTC_SPN3515:
						
						if( TempVar1 == DTC_FMI3 || TempVar1 == DTC_FMI4)
						{
							eSP2FaultArray[eDefSysElecFault] = eDefSysElecFault;			
						}
						
					break;	

					case DTC_SPN3521:
						
						if(TempVar1 == DTC_FMI18)
						{
							eSP2FaultArray[eDefQualityNotOk] = eDefQualityNotOk;			
						}
						
						if(TempVar1 == DTC_FMI31)
						{
							eSP2FaultArray[eDefDosingFault] = eDefDosingFault;			
						}
						
					break;

					case DTC_SPN7607:
						
						if( TempVar1 == DTC_FMI1)
						{
							eSP2FaultArray[eNoDosingLowTemp] = eNoDosingLowTemp;		
						}
						
					break;
					case DTC_SPN6802:
						
						if( TempVar1 == DTC_FMI31)
						{
							eSP2FaultArray[eNoDosingLowTemp] = eNoDosingLowTemp;		
						}
						
					break;					
					
					default:
						;
					break;
					}
			}
		// These warnings are not dependent on SCR/AWL/RSL and Mil
			switch(TempVar)
			{
				case DTC_SPN168:
						
						if(TempVar1 == DTC_FMI16)
						{
							eSP2FaultArray[eCheckAlternator] = eCheckAlternator;
						}				
				       if(TempVar1 == DTC_FMI18)
						{
							eSP2FaultArray[eCheckBattery] = eCheckBattery;
						}
					   
				break;				
				case DTC_SPN167: 
						
						if((TempVar1 == DTC_FMI16) || (TempVar1 == DTC_FMI18))
						{
							eSP2FaultArray[eCheckAlternator] = eCheckAlternator;
						}
						
				break;					
				case DTC_SPN1761:						
						
						if( TempVar1 == DTC_FMI11)
						{
							eSP2FaultArray[eDefLowRefill] = eDefLowRefill;
							eSP2FaultArray[eDefxx] = eDefxx;
						}
						if( TempVar1 == DTC_FMI18)
						{
							eSP2FaultArray[eDefVLowRefill] = eDefVLowRefill;
							eSP2FaultArray[eDefxx] = eDefxx;
						}
						if( TempVar1 == DTC_FMI1 )
						{
							eSP2FaultArray[eDefEmptyRefill] = eDefEmptyRefill;
							
						}
				break;

				case DTC_SPN7606:
						
						if( TempVar1 == DTC_FMI17)
						{
							eSP2FaultArray[eDefLowRefill] = eDefLowRefill;		
							eSP2FaultArray[eDefxx] = eDefxx;	
						}
						if( TempVar1 == DTC_FMI18)
						{
							eSP2FaultArray[eDefVLowRefill] = eDefVLowRefill;		
							eSP2FaultArray[eDefxx] = eDefxx;	
						}
						if( TempVar1 == DTC_FMI1)
						{
							eSP2FaultArray[eDefNearEmpty] = eDefNearEmpty;	
							eSP2FaultArray[eDefxx] = eDefxx;	
						}
						
					break;

				case DTC_SPN4096:
					
					if( TempVar1 == DTC_FMI31)
					{
						eSP2FaultArray[eDefEmptyRefill] = eDefEmptyRefill;		
					}
					
				break;
				default:
					;
				break;
			}
			
			
			if( (u8Length >= (u8)u16TotalSpns) && (u8Length != 0U) )
			{
				vClearDTC();
			}
			u8Length++;
		}
	}

	
	if(bStopEnginestatus == TRUE) 
	{
		eSP2FaultArray[eStopEngine] = eStopEngine;
	}
	else
	{
		eSP2FaultArray[eStopEngine] = eSP2Blank;
	}
	
	
/*
	if( ( bcheckEnginestatus == TRUE )  || (bStopEnginestatus == TRUE) || (bMilMode4status == TRUE) )
	{
		eSP2FaultArray[eVisitWorkshop] = eVisitWorkshop;
	}
	else 
	{
		eSP2FaultArray[eVisitWorkshop] = eSP2Blank;
	}
*/

}

static void _vDPFRegenerationWarning(void)
{

 static bool bRegenStopPressed = FALSE;
 static u32 u32TimeStampSP2 = 0U;
 static u32 u32RegenCompleteTime = 0U;
 static u32 u32RegenFailedTime = 0U;
 static u8 u8Length = 0U;
 static u8 u8SPN4175Status = 0U;
 static u8 u8ServiceRegenState = 0U;
 u8 i =0;
 u8 Data3697 = 0;
 u8 Data3700 = 0;
 u8 Data4175 = 0;
 u8 Data3703 = 0;
 u8 Data3702 = 0;
 u8 Data3704 = 0;
 u8 Data3705 = 0;
 u8 Data3707 = 0;
 u8 Data3708 = 0;
 u8 Data3710 = 0;
 u8 Data3711 = 0;
 u8 Data3712 = 0;
 u8 Data3716 = 0;

	if(!bGetPGNTimeOut(eJDPFC1_SPN4175))
	{	
		Data3697 = (u8)u32GetPGNdata(eJDPF); //read spn 3697 data
		Data3700 = (u8)u32GetPGNdata(eJDPFC1_SPN3700);
		Data4175 = (u8)u32GetPGNdata(eJDPFC1_SPN4175);
		Data3703 = (u8)u32GetPGNdata(eJDPFC1_SPN3703);
		Data3702 = (u8)u32GetPGNdata(eJDPFC1_SPN3702);		
		Data3704 = (u8)u32GetPGNdata(eJDPFC1_SPN3704);
		Data3705 = (u8)u32GetPGNdata(eJDPFC1_SPN3705);
		Data3707 = (u8)u32GetPGNdata(eJDPFC1_SPN3707);
		Data3708 = (u8)u32GetPGNdata(eJDPFC1_SPN3708);
		Data3710 = (u8)u32GetPGNdata(eJDPFC1_SPN3710);
		Data3711 = (u8)u32GetPGNdata(eJDPFC1_SPN3711);
		Data3712 = (u8)u32GetPGNdata(eJDPFC1_SPN3712);
		Data3716 = (u8)u32GetPGNdata(eJDPFC1_SPN3716);
		

        //************** Regen Required text *********************
		if(Data3697 == 1)
		{
			eSP2FaultArray[eRegenRequired] = eRegenRequired;
		}
		else
		{
			eSP2FaultArray[eRegenRequired] = eSP2Blank;
		}
		//************** Do Manual Regen text *********************
		if( ( (Data3697 == 4) || (Data3697 == 5)  ) 
			&& ( (Data4175 == 0) && (Data3700 != 1) ) )
		{
			eSP2FaultArray[eDoManualRegen] = eDoManualRegen;
		}
		else
		{
		 	eSP2FaultArray[eDoManualRegen] = eSP2Blank;
		}
		//************** Auto Regen On text *********************
		if(Data3700 == 1U && Data4175 == 00U) 
		{	
			eSP1FaultArray[eAutoRegenOn] = eAutoRegenOn;			
		}
		else
		{
		     eSP1FaultArray[eAutoRegenOn] = eSP1Blank;
		}

		//************** Regen stop Pressed text *********************
		if(Data3703 == 1U )
		{
			 eSP2FaultArray[eRegenstopPressed] = eRegenstopPressed;
			
			if(Data3697 == 1U )
			{
			 	eSP2FaultArray[eAutoRegenstoped] = eAutoRegenstoped;
			}
			else
			{
			 	eSP2FaultArray[eAutoRegenstoped] = eSP2Blank;
			}
		}
		else
		{
			eSP2FaultArray[eRegenstopPressed] = eSP2Blank;
		}		
			

		// Checking the transition for Service Regeneration Fault.
		// Refer Document "BSVI_IC_LCD-Text Message -V2_05022020.xlsx ".
		
		
		if(Data4175 == 1U) 			// SPN 4175 is 0b001 
		{
			eSP1FaultArray[eManualRgnActive] = eManualRgnActive;
			u32RegenCompleteTime = u32GetClockTicks();
			u32RegenFailedTime = u32GetClockTicks();
			u8SPN4175Status = SERVICE_REGEN_ACTIVE; // Service regen status is TRUE.
		}		
		else if((Data4175 == 0U) && (u8SPN4175Status == SERVICE_REGEN_ACTIVE))	// SPN 4175 transitions from // 0b001 to 0b000
		{	
			for(i =0;i<eTotalSP2Faults;i++)
			{
			 eSP2FaultArray[i] = eSP2Blank;
			}
			if(((u32GetClockTicks() - u32RegenCompleteTime) <= FIVE_SECONDS) 			
				&& ((u32GetClockTicks() - u32RegenFailedTime) <= FIVE_SECONDS))		// For 2 Seconds keep monitoring 
			{																				
				if( (Data3703 != 1) && (Data3704 != 1) && (Data3705 != 1) && (Data3708 != 1)
					&& (Data3710 != 1) && (Data3711 != 1) && (Data3712 != 1) && (Data3716 != 1)	 && (Data3707 != 1) )										// if Zero then KEEP status as 
				{															// RegenComplete
					u8ServiceRegenState = REGEN_COMPLETE;
					u32RegenFailedTime = u32GetClockTicks();				// Update RegenFailed TimeStamp 					
				} 		
				else if( (Data3703 == 1) || (Data3704 == 1) || (Data3705 == 1) || (Data3708 == 1)
					|| (Data3710 == 1) || (Data3711 == 1) || (Data3712 == 1) || (Data3716 == 1)	|| (Data3707 == 1)	 )	
				{
					u8ServiceRegenState = REGEN_FAILED;						
					u32RegenCompleteTime = u32GetClockTicks();				// Update RegenComplete TimeStamp
				}
			}
		}
	  
	
		// This Code determines that after monitoring for 2 Seconds which state was fixed for 2 Seconds.
		// To decide The timestamp for Regen Failed and Complete is compared and Service Regen State is 
		// compared with REGEN_COMPLETE and REGEN_FAILED.
		if((u8ServiceRegenState == REGEN_COMPLETE) && ((u32GetClockTicks() - u32RegenCompleteTime) > FIVE_SECONDS)) 
		{			
			u8ServiceRegenState = NULL;
			u8SPN4175Status = NULL;
			u32RegenCompleteTime = u32GetClockTicks();
			eSP1FaultArray[eManualRegComplete] = eManualRegComplete;
			eSP1FaultArray[eManualRgnActive] = eSP1Blank;	
			eSP1FaultArray[eManualRegAbort] = eSP1Blank;
			eSP1Index = eManualRegComplete;
			u32TimeStamp = u32GetClockTicks();
		}
		else if((u8ServiceRegenState == REGEN_FAILED) && ((u32GetClockTicks() - u32RegenFailedTime) > FIVE_SECONDS)) 
		{
			u8ServiceRegenState = NULL;
			u8SPN4175Status = NULL;
			u32RegenFailedTime = u32GetClockTicks();
			eSP1FaultArray[eManualRegComplete] = eSP1Blank;
			eSP1FaultArray[eManualRgnActive] = eSP1Blank;	
			eSP1FaultArray[eManualRegAbort] = eManualRegAbort;
			eSP1Index = eManualRegAbort;
			u32TimeStamp = u32GetClockTicks();
		}else {/*Do Nothing*/}

		// Clear All faults related to Regen service afetr it is Displayed once or Echoed Once by
		
		if((u32GetClockTicks() - u32RegenCompleteTime) > FOUR_SECONDS)
		{
			eSP1FaultArray[eManualRegComplete] = eSP1Blank;
		}
		else if((u32GetClockTicks() - u32RegenFailedTime) > FOUR_SECONDS)
		{
			eSP1FaultArray[eManualRegAbort] = eSP1Blank;
		}else {/*Do Nothing*/}
		// Checking the transition for Regeneration Stop Pressed.
		// Refer Document "BSVI_IC_LCD-Text Message -V1.1_09112019.xls ".
		
		
	}
	else
	{
		// Keep all the Texts Blank
		
		eSP1FaultArray[eManualRgnActive] = eSP1Blank;
		eSP1FaultArray[eManualRegComplete] = eSP1Blank;
		eSP1FaultArray[eManualRegAbort] = eSP1Blank;
		eSP1FaultArray[eAutoRegenOn] = eSP1Blank;
		eSP2FaultArray[eRegenstopPressed] = eSP2Blank;
		eSP2FaultArray[eRegenRequired] = eSP2Blank;
		u8ServiceRegenState = NULL;
		bRegenStopPressed = FALSE;
		u8SPN4175Status = NULL;
		
	}

 
}


