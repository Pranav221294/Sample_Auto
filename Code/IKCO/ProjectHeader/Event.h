#ifndef __Event_H
#define __Event_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "Clock.h"
#include "HourMeter.h"
#include "OdoMeter.h"



// Sgared Enums and Variables are declared in this file.

typedef enum
{
	eNoOdoReset = 0,
	eOdoTripAReset,
	eOdoTripBReset,
	eOdoTripResetCANTx
	
}ODO_RESET;

typedef enum
{
	eNoHourReset = 0,
	eHourTripAReset,
	eHourTripBReset
	
}HOUR_RESET;

typedef enum
{
	eNoAFEReset = 0,
	eResetAFETripA,
	eResetAFETripB,
	eAFEAtLowLfcA,
	eAFEAtLowLfcB
	
}AFE_RESET;

#endif
