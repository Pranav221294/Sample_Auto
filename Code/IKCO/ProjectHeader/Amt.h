#ifndef __AMT_H
#define __AMT_H
#include "customTypedef.h"
#include "Eol.h"
#include "TellTales.h"

extern u32 u32GetClockTicks(void);

// Learn Status Enum
typedef enum
{
	eLearnNotActive = 0,
	eLearnActive,
	eLearningFail,
	eCANLost,
	
}eLEARN_STATUS;

// Auto Manual Enum	
typedef enum
{
	eManual = 0,
	eAuto,
	eAMError,
	eAMNotShown
	
}eAM_STATUS;


// Eco Power Enum

typedef enum
{
	ePower = 0,
	eEco = 1,
	eEPError,
	eEPNotShown
}eEP_STATUS;

// AMT messages Enum

typedef enum
{
	eLearnOkMsg = 1,
	eLearnFailMsg,
	eAMTCanErrorMsg,
	eAMTCanLostMsg,
	eATCanLostMsg,
	eShiftInhibitMsg,
	eTotalAMTMsg
	
}eAMT_MSGs;

#define ECO_ON				0x01		// ONLY WHEN 'AT TRANS'
#define POWER_ON			0x01		// ONLY WHEN 'AT TRANS'


// private prototypes --------------------------------------------------------

/// @addtogroup AMT_LIBRARY
/// @{
 
/// @defgroup amtLib Private functions used for AMT operations
//// 
///   
/// @addtogroup amtLib
/// @{

u16 u16GetGearAMT(void);
eAM_STATUS eGetAutoManualStatus(void);
eEP_STATUS eGetEcoPowerStatus(void);
eEP_STATUS eGetAtEcoStatus();
eEP_STATUS eGetAtPowerStatus();

bool bGetCreepStatus(void);
bool bGetCrawlStatus(void);
void vAMTProcessFlow(void);

eLEARN_STATUS eGetAMTLearningStatus(void);

bool u8GetShiftInhibitState(void);
void vCameraEnable();


/// @} // endgroup amtLib

/// @} // endgroup AMT_LIBRARY



#endif

