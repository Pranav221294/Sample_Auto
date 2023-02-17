#include "r_cg_userdefine.h"
#include "DtcCycle.h" 
#include "Adc.h" 
#include "Can.h"



#define DTC_CYCLE_TIME			40 		//in ms
#define SHORT_TO_GND			20		// increased to 20 because of noise when inputs are short to ground 2 //equivalent ADC values
#define HW_DIAG_TIME_THRESH		1000 	//in ms specified
//DTC
#define FAULT_RESET_COUNT		40


static u16 faultTimersGnd[eNumofSupportedDTC];//contains time in ms
u8 faultFlags[eNumofSupportedDTC];


static bool bSaveDatcDataFlag = FALSE; //save DTC data

u8  u8FrontApgDtcStatus[DTC_DATA_LENGTH] = {0xFF, 0xFF, 0xFF};
u8  u8RearApgDtcStatus[DTC_DATA_LENGTH] = {0xFF, 0xFF, 0xFF};
u8  u8FuelDtcStatus[DTC_DATA_LENGTH] = {0xFF, 0xFF, 0xFF};
u8  u8OilPressureDtcStatus[DTC_DATA_LENGTH] = {0xFF, 0xFF, 0xFF};
u8  u8CanBusOffDtcStatus[DTC_DATA_LENGTH] = {0xFF, 0xFF, 0xFF};



//maintain the same sequence in dtcLogged this array as defined in SUPPORTED_DTC
 DTC_STRUCT_TYPE dtcLogged[eNumofSupportedDTC+1];

static void _vResetFault(SUPPORTED_DTC );



/*********************************************************************//**
 *
 * 
 *  Description:	fault timer controlled in adjustFaultTimers, is used to log DTC here. 
 * 					All the DTCs are ADC based and they all have a timer associated with them.
 * 			 		Operation cycle is same as ignition cycle
 * 			 		Monitoring of analog inputs is formed after specific intervals in the same operation cycle. 
 * 			 		This is called monitoring cycle
 
 * 			 Status bits are arranged as below 
 * 			 0 : testFailed
 * 			 1 : testFailedThisOperatingCycle
 * 			 2 : pendingDTC
 * 			 3 : confirmedDTC
 * 			 4 : testNotCompletedSinceLastClear
 * 			 5 : testFailedSinceLastClear
 * 			 6 : testNotCompletedThisMonitoringCycle
 * 			 7 : warningIndicatorRequested
 
 * 			 Bit 0,1,3,5 are provided in our cluster
 
 * 			 Bit 0 : testFailed is This set in every monitoring cycle
 * 			 Bit 1 : testFailedThisOperatingCycle When Bit 0 is set first time in an 
 * 				   operation cycle, this bit is set. Once set in an operation cycle, it will 
 * 				   remain set irrespective of Bit 0 until next operation cycle
 * 			 Bit 3 : confirmedDTC This can be cleared by external command or by itself 
					 if the fault does not exist after 40 consecutive operation cycle
 * 			 Bit 5 : testFailedSinceLastClear This is set when bit 3 is set. It can be 
					 cleared by external command only
 
 *
 * @param	None      
 *
 * @return	NOne     
 *************************************************************************/
 void vDTCOperationCycle(void)
 {
	 
	u8 i,j,k, faultIndex;
	static u32 dtcCheckTime;
	static u16 BusOffClearTimeStamp = 0;
	//this is called after ADCrefreshtime, so that can add further delay.
	if(u32GetClockTicks() - dtcCheckTime < DTC_CYCLE_TIME )
		return;
	
	dtcCheckTime = u32GetClockTicks();
	
	for(i=0; i <= eFuelLevelShortGnd; i++)
	{	 
		j= i;
		if(j == eMilLampAdc )
		{
			j = eOilPressure;
		}
		else if(j == eBattAdc )
		{
		 	j = eFuelGaugeAdc;
		}
		
		if(u16GetAdcValue(j) < SHORT_TO_GND )
		{
			faultIndex = i;
			if(faultTimersGnd[i] >= HW_DIAG_TIME_THRESH && dtcLogged[faultIndex].isEnabled == TRUE )
			{
				if( dtcLogged[faultIndex].isLogged == FALSE )
				{
					dtcLogged[faultIndex].isLogged = TRUE;
					dtcLogged[faultIndex].hasChanged= TRUE;
					dtcLogged[faultIndex].statusByte.bits.confirmedDTC = 1;
					dtcLogged[faultIndex].statusByte.bits.testFailedSinceLastClear = 1;
					dtcLogged[faultIndex].nofaultCount = 0;// no fault should exist for 40 consecutive ignition cycles; reset to start counting afresh
					dtcLogged[faultIndex].oneTimeFaultCheck = FALSE;
					bSaveDatcDataFlag = TRUE;
				}
				else if(dtcLogged[faultIndex].nofaultCount > 0 
					&& dtcLogged[faultIndex].isEnabled == TRUE )
				{
					dtcLogged[faultIndex].nofaultCount = 0;
					dtcLogged[faultIndex].hasChanged= TRUE;
					bSaveDatcDataFlag = TRUE;
				}
			}
		
		}		
		
	}

	//check fault for CAN bus load 
	
	if(bGetCanBusOffFlagStatus() == TRUE)
	{
	   BusOffClearTimeStamp = u16GetClockTicks();
		faultIndex = eJ1939BusOff;
		if(faultTimersGnd[faultIndex] >= HW_DIAG_TIME_THRESH && dtcLogged[faultIndex].isEnabled == TRUE )
		{
		  if( dtcLogged[faultIndex].isLogged == FALSE )
			{
				dtcLogged[faultIndex].isLogged = TRUE;
				dtcLogged[faultIndex].hasChanged= TRUE;
				dtcLogged[faultIndex].statusByte.bits.confirmedDTC = 1;
				dtcLogged[faultIndex].statusByte.bits.testFailedSinceLastClear = 1;
				dtcLogged[faultIndex].nofaultCount = 0;// no fault should exist for 40 consecutive ignition cycles; reset to start counting afresh
				dtcLogged[faultIndex].oneTimeFaultCheck = FALSE;
				bSaveDatcDataFlag = TRUE;
				
			}
			else if(dtcLogged[faultIndex].nofaultCount > 0 
				&& dtcLogged[faultIndex].isEnabled == TRUE )
			{
				dtcLogged[faultIndex].nofaultCount = 0;
				dtcLogged[faultIndex].hasChanged= TRUE;
				bSaveDatcDataFlag = TRUE;
			}
		}
	
	}	
	if(bGetCanBusOffFlagStatus() == TRUE)
	{
    	if( u16GetClockTicks() - BusOffClearTimeStamp > 1000)
		{
			 vClearCanBusOffFlag();
			 BusOffClearTimeStamp = u16GetClockTicks();
		}
	}
	
}
 
 
 
void vAdjustFaultTimers(void)
{	
	u8 i,j,faultIndex;
	static u32 lastTime = 1;
	u16 timeDiff ;

    if(lastTime == 1)
        lastTime =  u32GetClockTicks();
    
    timeDiff = (u16)u32GetClockTicks() - (u16)lastTime;
   for(i=0; i <= eFuelLevelShortGnd; i++)
	{	 
		j= i;
		if(j == eMilLampAdc )
		{
			j = eOilPressure;
		}
		else if(j == eBattAdc )
		{
		 	j = eFuelGaugeAdc;
		}
        if(u16GetAdcValue(j) < SHORT_TO_GND)
        {
            if(dtcLogged[faultIndex].isEnabled == TRUE )
            {
                faultTimersGnd[i] += timeDiff ;
                if(faultTimersGnd[i] >= HW_DIAG_TIME_THRESH)
                {
	                faultTimersGnd[i] = HW_DIAG_TIME_THRESH;
	                faultFlags[i] = TRUE;                                
                }
				
                dtcLogged[faultIndex].statusByte.bits.testFailed = 1;
                dtcLogged[faultIndex].statusByte.bits.testFailedThisOperatingCycle = 1;
            }
        }//for short to ground; it will be executed once
        else if(dtcLogged[faultIndex].statusByte.bits.testFailed != 0 )
    	{
        	dtcLogged[faultIndex].statusByte.bits.testFailed = 0;
    	}
        else if ( dtcLogged[faultIndex].oneTimeFaultCheck == TRUE )
        {
            if(dtcLogged[faultIndex].isLogged == TRUE && faultTimersGnd[faultIndex] == 0 
                            && dtcLogged[faultIndex].isEnabled == TRUE )
            {
                dtcLogged[faultIndex].nofaultCount++;
                dtcLogged[faultIndex].hasChanged = TRUE;
                if(dtcLogged[faultIndex].nofaultCount >= FAULT_RESET_COUNT)
                {
                	_vResetFault(faultIndex);
                }
                bSaveDatcDataFlag = TRUE;
            }
            dtcLogged[faultIndex].oneTimeFaultCheck = FALSE; 
        }
                    

        else if ( faultTimersGnd[i] != 0)
        {        

            if(faultTimersGnd[i] > timeDiff )
        	{
            	faultTimersGnd[i] -= timeDiff ;
        	}
            else
            {
            	faultTimersGnd[i] = 0;
            }
            if(  faultTimersGnd[i] == 0 )
        	{
            	faultFlags[i] = 0;
        	}
        }                                            

       lastTime = u32GetClockTicks();
				
	}
 }

 /*****************************************************************************
**  Function name:  resetFault
** 
**  Description:    Clear DTC related variables. It is called when fault has not existed for 
**				FAULT_RESET_COUNT number of cycles
**
**  Parameters:     none
**
**  Return value:   none
**
*****************************************************************************/

static void _vResetFault(SUPPORTED_DTC faultNum )
{
	dtcLogged[faultNum].statusByte.status = 0;
	dtcLogged[faultNum].isLogged = FALSE;
	dtcLogged[faultNum].hasChanged = TRUE;
	dtcLogged[faultNum].nofaultCount = 0;

}


bool bInternalDtcSaveFlagStatus()
{
	return bSaveDatcDataFlag;
}

void vSetInternalDtcSaveFlag(bool status)
{
	bSaveDatcDataFlag = status;
}


u8 u8GetNoFaultDtcCount(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].nofaultCount;
}

u8 u8GetDtcStatus(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].statusByte.status;
}

bool bGetDtcChangedFlag(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].hasChanged;
}

bool bGetDtcEnableFlag(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].isEnabled;
}

bool bGetDtcLoggedFlag(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].isLogged;
}

bool bGetDtcOneTimeCheckFlag(SUPPORTED_DTC DtcType)
{
  return dtcLogged[DtcType].oneTimeFaultCheck;
}

void SetNoFaultDtcCount(SUPPORTED_DTC DtcType , u8 count)
{
  	dtcLogged[DtcType].nofaultCount = count;
}

void SetDtcStatus(SUPPORTED_DTC DtcType , u8 DtcStatus)
{
	dtcLogged[DtcType].statusByte.status = DtcStatus;
}

void SetDtcChangedFlag(SUPPORTED_DTC DtcType , bool flag)
{
	dtcLogged[DtcType].hasChanged = flag;
}

void SetDtcEnableFlag(SUPPORTED_DTC DtcType , bool flag)
{
	dtcLogged[DtcType].isEnabled = flag;
}

void SetDtcLoggedFlag(SUPPORTED_DTC DtcType , bool flag)
{
 	dtcLogged[DtcType].isLogged = flag;
}

void SetDtcOneTimeCheckFlag(SUPPORTED_DTC DtcType , bool flag)
{
	dtcLogged[DtcType].oneTimeFaultCheck = flag;
}


