
#ifndef DISCRETESWITCHES_H_
#define DISCRETESWITCHES_H_

#include "customTypedef.h"


typedef enum
{
	ePressed = 0,	
	eReleased,
	eShortPressed,
	eLongPressed,
	eModeUpLongPressed,
	eModeDownLongPressed,
	eModeSetLongPressed,
	eSetUpLongPressed,
	eUpDownLongPressed,
	eInvalidPress,
	eStuckFault
	
}SW_STATE;


extern u32 u32GetClockTicks();

/// @addtogroup SWITCHES_LIB
/// @{
 
/// @defgroup swLib Private functions used for AMT operations
//// 
///   
/// @addtogroup swLib
/// @{

SW_STATE eGetMODESwitchStatus(void);
SW_STATE eGetSETSwitchStatus(void);
SW_STATE eGetUPSwitchStatus(void);
SW_STATE eGetDOWNSwitchStatus(void);
void vResetButtonTimeStamps();
/// @} // endgroup swLib

/// @} // endgroup SWITCHES_LIB

#endif /* SWITCHES_H_ */
