#ifndef __DTC_CYCLE_H
#define __DTC_CYCLE_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"

#define DTC_DATA_LENGTH	 3

typedef enum
{
	eRearApg,
	eFrontApg,
	eMilLampAdc,
	eBattAdc,
	eOilPressure,
	eFuelGaugeAdc,	
	eNumOfADCchans
} eADC_TYPE;

//DTC related
typedef enum
{
eRearAirPresShortGnd,
eFrontAirPresShortGnd,
eOilPressureShortGnd,
eFuelLevelShortGnd,
eJ1939BusOff,
eNumofSupportedDTC
}SUPPORTED_DTC;

#define BODY_DTC		16
#define NETWORK_DTC 	7
#define NON_ADC_DTC		9

//don't store the ID in eeprom. whenever any dtc is logged, set islogged using index=SUPPORTED_DTC
//DTC is cleared if the fault does not exist for 40 consecutive ignition cycles.
typedef struct
{
u8 hasChanged:1; //when this bit is set, write the data to memory
u8 isEnabled:1;
u8 isLogged:1;
u8 oneTimeFaultCheck:1;
union{
	u8 status;
	struct
	{
		u8 testFailed:1; //bit 0
		u8 testFailedThisOperatingCycle:1;
		u8 pendingDTC:1;
		u8 confirmedDTC:1;
		u8 testNotCompletedSinceLastClear:1;
		u8 testFailedSinceLastClear:1;
		u8 testNotCompletedThisMonitoringCycle:1;
		u8 warningIndicatorRequested:1;//bit 7
	}bits;

}statusByte;
u8 nofaultCount; //records for how many ignition cycles fault has not existed
//u8 dummy; // make it a structure of 4 bytes; it will occupy 1 sector in eeprom
}DTC_STRUCT_TYPE;


extern  DTC_STRUCT_TYPE dtcLogged[eNumofSupportedDTC+1];
extern u8  u8FrontApgDtcStatus[DTC_DATA_LENGTH];
extern u8  u8RearApgDtcStatus[DTC_DATA_LENGTH];
extern u8  u8FuelDtcStatus[DTC_DATA_LENGTH];
extern u8  u8OilPressureDtcStatus[DTC_DATA_LENGTH];
extern u8  u8CanBusOffDtcStatus[DTC_DATA_LENGTH];


//Constant declarations ----------------------------------------------------


/// @addtogroup UDS_DTC_FUNCTION_LIBRARY
/// @{
/// @defgroup DtcFunctionLib  functions used 
//// 
///   
/// @addtogroup DtcFunctionLib
/// @{

void vDTCOperationCycle(void);
void vAdjustFaultTimers(void);
void vSetInternalDtcSaveFlag(bool);
bool bInternalDtcSaveFlagStatus(void);



/// @} // endgroup DtcFunctionLib

/// @} // endgroup UDS_DTC_FUNCTION_LIBRARY

#endif
