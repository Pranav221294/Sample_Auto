
#include "IoControlGauges.h" 
#include "StepperDriver.h"
#include "TML_14229_UDSH.h"
#include "Speedo.h"
#include "Tacho.h"
#include "FuelGauge.h" 
#include "CoolantTemp.h" 
#include "OilPressureGauge.h" 
#include "AirPressureGauge.h"
#include "Def.h" 
#include "Eol.h"
 
#define FUEL_MAX		100U //in %


void vIoControlGauges(void)
{
	u32 temp = 0U;	
	u32 data = 0U;
	u32 microsteps = 0U;
	u16 Value =0U; //value received for io control

	//Speedo*********************************************	
	if( sTestEquimentControl.IoControlFlag.bits.speedometerFlag ==  TRUE)
	{
		temp = u16GaugeMaxMicroSteps(eSpdMeter);
		Value = sTestEquimentControl.u8SpeedoData;
		temp =  (temp * (u32)Value)/SPEEDO_RANGE;

		if( temp > u16GaugeMaxMicroSteps(eSpdMeter))
		{
			temp = u16GaugeMaxMicroSteps(eSpdMeter); 
		}

		vSetTargetPosition((s16)temp,eSpdMeter);
	}
	//****************************************************

	//RPM*************************************************
	if( sTestEquimentControl.IoControlFlag.bits.tachometerFlag ==	TRUE)		
	{
		temp = u16GaugeMaxMicroSteps(eTachometer);
		Value = sTestEquimentControl.u8TachoData;
		Value = Value *50U; //multiply by resolution 
		temp =  (temp * (u32)Value)/(u16)RPM_RANGE;

		if(temp > u16GaugeMaxMicroSteps(eTachometer))
		{
			temp = u16GaugeMaxMicroSteps(eTachometer); 
		}

		vSetTargetPosition((s16)temp,eTachometer);
	}
	
	//****************************************************

	//Temp Gauge*************************************************
	if(sTestEquimentControl.IoControlFlag.bits.TempGaugeFlag == TRUE)
	{

		microsteps = u16GaugeMaxMicroSteps(eTempGauge);
		temp = microsteps;
		
		data = sTestEquimentControl.u8CoolantTempData;
		
			if(sTestEquimentControl.u8CoolantTempData <= GRAPH_FIRST_TEMP_MAX)
			{
				temp = temp/ 2U; 	// temp 60-95 deg have dial  span = Full span/2
				if(data >= TEMP_MIN)
				{
					data = data - TEMP_MIN;
				}
				else
				{
					data = 0U;
				}
				data =	( temp*data )/( GRAPH_FIRST_TEMP_MAX - TEMP_MIN ) ;  //Calculate Target position 
			}
			else if(sTestEquimentControl.u8CoolantTempData <= sCoolantTempBytes.u8CoolantTempRedZoneStart)
			{
				temp = temp/ 4U; 	// temp 90-Redzone deg have dial  span = Full span/4
				data = data - GRAPH_FIRST_TEMP_MAX;
				data =	( temp*data )/( (u32)sCoolantTempBytes.u8CoolantTempRedZoneStart - GRAPH_FIRST_TEMP_MAX ) ;	//Calculate Target position
				data = data + (microsteps/2U);
			}
			else
			{
				temp = temp/ 4U; 		// temp Redzone - 120 deg have dial  span = Full span/4
				data = data - sCoolantTempBytes.u8CoolantTempRedZoneStart;
				data =	( temp*data )/( TEMP_MAX - (u32)sCoolantTempBytes.u8CoolantTempRedZoneStart ) ;	//Calculate Target position
				data = data + ( (microsteps*3U)/4U );
			}
		
		vSetTargetPosition((s16)data,eTempGauge);
	}
	
	//****************************************************

	//Fuel Gauge*********************************************	
	if(sTestEquimentControl.IoControlFlag.bits.FuelGaugeFlag == TRUE)
	{
		temp = u16GaugeMaxMicroSteps(eFuelGauge);
		Value = sTestEquimentControl.u8FuelData;
		
		if(Value > FUEL_MAX)
		{
			Value = FUEL_MAX;
		}
		temp =  (temp * (u32)Value)/FUEL_MAX;

		if(temp > u16GaugeMaxMicroSteps(eFuelGauge))
		{
			temp = u16GaugeMaxMicroSteps(eFuelGauge); 
		}

		vSetTargetPosition((s16)temp,eFuelGauge);
	}
	//****************************************************

	//Def Gauge*********************************************	
	if(sTestEquimentControl.IoControlFlag.bits.DefEnableFlag == TRUE)
	{	
		 vSetDefBarNo(sTestEquimentControl.u8DefData);				
	}
	//****************************************************

	//Oil Pressure Gauge**************************************	
	if(sTestEquimentControl.IoControlFlag.bits.OilPressureEnableFlag == TRUE)
	{
		vSetOilPressureBarNo(sTestEquimentControl.u8OilPressureData/2U);
	}
	//****************************************************
	//Front Air Pressure Gauge**************************************	
	if(sTestEquimentControl.IoControlFlag.bits.FrontApgEnableFlag == TRUE)
	{
	   if(sTestEquimentControl.u8FrontApgData < 15U)
	   	{
			vSetFrontApgBarNo(sTestEquimentControl.u8FrontApgData/2U);
	   	}
	   else
	   	{
	   		vSetFrontApgBarNo(7U); //To display 15 bar
	   	}
	}
	//****************************************************
	//Rear Air Pressure Gauge**************************************	
	if(sTestEquimentControl.IoControlFlag.bits.RearApgEnableFlag == TRUE)
	{
		if(sTestEquimentControl.u8RearApgData < 15U)
	   	{
			vSetRearApgBarNo(sTestEquimentControl.u8RearApgData/2U);
	   	}
	   else
	   	{
	   		vSetRearApgBarNo(7U); //To display 15 bar
	   	}
	}	
	//****************************************************
	
}


