
#ifndef __LedDriver5716_H
#define __LedDriver5716_H
#include "customTypedef.h"



// Pins declarations ---------------------------------------------------------

#define LATCH			P13.6         	
#define GCLK			P5.0 
#define SCK				P5.1
#define SDI				P5.3
#define SDO				P5.2
#define BLANK			P2.7
#define ERR_DETECT_1	P2.6
#define ERR_DETECT_2	P13.0

// Constant declarations -----------------------------------------------------

#define GS_DATA_SIZE 72


/// @addtogroup LED_DRIVER_LIBRARY
/// @{
 
/// @defgroup LedDriverLib Global functions used in LCD files
//// 
///  
 
/// @addtogroup LedDriverLib
/// @{

void vLedDataSendToSpi();
void vLedDriverInit();
void vLedDriverSendEndCallback(void);
void vSetLedDriverBlankHigh(void);
void vSetLedDriverBlankLow(void);

/// @} // endgroup LedDriverLib

/// @} // endgroup LED_DRIVER_LIBRARY


#endif
