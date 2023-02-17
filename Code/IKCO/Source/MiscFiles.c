
#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "J1939.h"
#include "r_cg_userdefine.h"
#include "Adc.h" 
#include "CurveHeader.h"
#include "MiscFiles.h"

#define GAUGE_AVG_SIZE				16
#define FILTER_VOLTAGE				10     //volt 1.0
#define VOLTAGE_HYS_UP				6      //volt 0.6
#define VOLTAGE_HYS_DOWN			4      //volt 0.4
#define VOLT_VARIATION_TIME 		3000



typedef struct
{  
    u16  u16Value;							//OAT Value
    u8   u8AvgCount;  						//Counter for averaging ADC sample   
    bool bClearDisplay;
} OAT_TYPE;

typedef struct
{   
	u16  u16SignalData[GAUGE_AVG_SIZE];			//Use for ADC sample averaging
    u8   u8Value;							//Current value of voltmeter
    u8   u8AvgCount;  						//Counter for averaging ADC sample   
} Voltage_TYPE;


OAT_TYPE sOat = {0x00,0x00,0x00};
Voltage_TYPE sBattVolt = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Read CAN message for OAT and Check Can lost message and Average data for OAt 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 void vOatDataProcessing()
{
	u32 data = 0;	
		
	if(bGetPGNTimeOut(ejOat))	//Check CAN message lost
	{
		data = 0;	
		sOat.bClearDisplay = TRUE;
	}
	else
	{
	//Go back if new sample is not coming		
		if(! bGetPGNDataReady(ejOat) )
			return; 
		ClearPGNDataReady(ejOat);
		
		data = u32GetPGNdata(ejOat);
		//Check out of range data 
		if(data >= BADDATA_2BYTE ) 
			sOat.bClearDisplay = TRUE;
		else
		{
			sOat.bClearDisplay = FALSE;
			if(data >= OAT_OVR_RANGE)
				data = OAT_OVR_RANGE;
		}	
		
	}
	
	sOat.u16Value = (u16)data;

}

/*********************************************************************//**
 *
 * Get OAT value  
 *
 * @param      None    
 *
 * @return     OAT value
 *************************************************************************/

u8 u8GetOatValue()
{
 return (u8)sOat.u16Value;
}

/*********************************************************************//**
 *
 * Get OAT display status  
 *
 * @param      None    
 *
 * @return     OAt display status
 *************************************************************************/

bool bGetOatDisplayStatus()
{
 return sOat.bClearDisplay;
}

/*********************************************************************//**
 *
 * Read CAN message for OAT and Check Can lost message and Average data for OAt 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

 void vBattVolt()
{
	u32 data = 0;
	u32 BattVolt =0;
	static u16 PreviousBattVolt =0;
	u8 i = 0;
	static u8 AvgCount = GAUGE_AVG_SIZE - 1;
	static bool FirstEntry = TRUE;		
	u8 fractionPart = 0;
	static bool HysFlag = 0;
	
	static u16 CrackingWatingTime =0;
	static bool VoltageUpdateFlag = 0;
 	//Check ADC sample is reading or not 
	if(bGetAdcSampleStatus(eBATT_VOLT_CH3) == FALSE)
		return;
	vResetAdcSampleStatus(eBATT_VOLT_CH3);
	//Take ADC value	
	data = u16GetAdcValue(eBATT_VOLT_CH3);

	if(VoltageUpdateFlag)
		{
		if(u16GetClockTicks() - CrackingWatingTime < VOLT_VARIATION_TIME)
			return;
		else
			VoltageUpdateFlag = FALSE;
		}
		

	if(FirstEntry)
	{
		for(i = 0; i < GAUGE_AVG_SIZE; i++)
			sBattVolt.u16SignalData[i]  = data;		
		AvgCount = GAUGE_AVG_SIZE-1;
				 
	}
		
	sBattVolt.u16SignalData[AvgCount] = data;	
		data = 0;	
	for(i = 0; i < GAUGE_AVG_SIZE; i++)
		data = data + sBattVolt.u16SignalData[i];
	
		
	if(++AvgCount >= GAUGE_AVG_SIZE)
	{
		AvgCount = 0;
		data = data / GAUGE_AVG_SIZE;	
		vGetCurveDataValue(eBattVoltCurve,data,&BattVolt);
		//This is filter when voltage up down suddenly then its works  		
		
			if( ((BattVolt > PreviousBattVolt)? (BattVolt - PreviousBattVolt) : (PreviousBattVolt - BattVolt)) > FILTER_VOLTAGE )
				{			 
				 PreviousBattVolt = BattVolt;
				 VoltageUpdateFlag = TRUE;
				 CrackingWatingTime = u16GetClockTicks();
				 if(FirstEntry == FALSE)
				 	return;
				}
		
		FirstEntry = FALSE;
		fractionPart = BattVolt%10;

		
		sBattVolt.u8Value = (u8)(BattVolt/10);
		
		if(fractionPart <= VOLTAGE_HYS_DOWN)
		{
			HysFlag = FALSE;
		}			

		if(fractionPart >= VOLTAGE_HYS_UP || HysFlag == TRUE)
		{
			sBattVolt.u8Value = sBattVolt.u8Value + 1; //Add  1.0 volt if volt greater 0.7V 
			HysFlag = TRUE;			
		}
	   
			

		
	}

}

/*********************************************************************//**
 *
 * Get Battery Voltage value
 *
 * @param      None    
 *
 * @return     battry volt
 *************************************************************************/

u8 u8GetBatteryVolt()
{
	return sBattVolt.u8Value;
}

