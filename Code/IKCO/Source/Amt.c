/** --------------------------------------------------------------------------
**  COPYRIGHT:  Indication Instruments Limited 
**              Plot No. 19,Sector6
**              Faridabad - 121006         
** --------------------------------------------------------------------------
**  IDE				: CubeSuite+(CS+)
**  Version			: V3.00.00
**  Compiler		: CA78K0R
**  Debuging Tool	: Renesas E1 programmer
**  

**---------------------------------------------------------------------------
**  Co-ordinator                Mukul Goyal
**  Software Manager		  Khusboo Lalani
**---------------------------------------------------------------------------
**  Revision History
**
**  Date        Name    			Version              Description
** ==========  =======  =======  =====================================
** 01/01/2019  Pratyush Abhay     Initial Version.       


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/


#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "J1939.h" 
#include "Amt.h"
#include "r_cg_userdefine.h"
#include "LCDScreens.h"


// Gear Values
#define NEUTRAL_DATA	0x7D	
#define TOP_GEAR		0x91
#define PARK_ON			0xFB


// AMT data from J1939 ------------------------------------------------------
// All Bit dta

#define REVERSE_CHAR	0x5852		// ('X'<<8|'R')	Reverse Ascii value of 'R'
#define NEUTRAL_CHAR	0x584E		// ('X'<<8|'N') Neutral Ascii value of 'N'
#define PARK_CHAR		0x5850		// ('X'<<8|'P')	Park  Ascii value of 'P'
#define L_GEAR			0x584C		// ('X'<<8|'L')	L gear Ascii value of 'L'

#define BLANK_CHAR		0x5858		// ('X'<<8|'X') Blank Ascii value of 'X'
#define DASH_CHAR		0x5F5F		// ('-'<<8|'-')	DashedAscii value of '-'

#define CAMERA_ENABLE  P13.1


u8  u8ShiftInhibit = FALSE;


typedef struct
{		
	u16 u16GearData;				// Gear Number Data
	bool bCreep	:1;					// Creep Status
	bool bCrawl	:1;					// Crawl Status
	eLEARN_STATUS eAMTLearning;		// Learning Status
	eAM_STATUS eAutoManual;			// Auto Manual
	eEP_STATUS eAmtEcoPower;		// Eco Power
	eEP_STATUS eAtEco;				// Eco Power
	eEP_STATUS eAtPower;			// Eco Power
	
}AMT_DATA_TYPE;

// Auto Manual Enum

AMT_DATA_TYPE sAmt = {0x5858, FALSE, FALSE, eLearnNotActive, eAMNotShown, eEPNotShown};

// private prototypes ---------------------------------------------------------------------------

/// @addtogroup AMT_LIBRARY
/// @{
 
/// @defgroup amtLib Private functions used for AMT/AT operations as Depicted in State diagram
//// 
///   
/// @addtogroup amtLib
/// @{



static void _vAutoManualDisplayProcess(void);
static void _vCreepCrawlDisplayProcess(void);


static void _vEcoPowerAMT(void);
static void _vAMTMessageProcess(void);
static void _vEcoPowerAT(void);



/// @} // endgroup amtLib

/// @} // endgroup AMT_LIBRARY




/*********************************************************************//**
 *
 * Scenario 4 from AMT State Flow Diagram
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vAutoManualDisplayProcess(void)
{
	u32 data = eAMError;
	
	if(sAmt.eAMTLearning == eLearnNotActive || sAmt.eAMTLearning == eLearnActive)
	{	
		if(bGetPGNTimeOut(eJAuto_Manual_Eco))                 
	    {
	       sAmt.eAutoManual = eAMNotShown;
		   return;
	    }
		else
		{
			data = (u8)u32GetPGNdata(eJAuto_Manual_Eco); 
		}
		
		if(data == eManual)
			sAmt.eAutoManual = eManual;
		
		else if(data == eAuto)
		{
			sAmt.eAutoManual = eAuto;
			_vEcoPowerAMT();
		}
		else
			sAmt.eAutoManual = eAMError;		
	}
	
	else
		sAmt.eAutoManual = eAMNotShown;

}

/*********************************************************************//**
 *
 * Scenario 5 from AMT State Flow Diagram
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vCreepCrawlDisplayProcess(void)
{
	u8 u8CrawlData = FALSE, u8CreepData = FALSE;
	
	u8CrawlData = (u8)u32GetPGNdata(eJCrawl);
	u8CreepData = (u8)u32GetPGNdata(eJCreep);
	
	if(u8CreepData == TRUE)
	{				
		sAmt.bCreep = TRUE;					// Display Creep
	}
	else
		sAmt.bCreep = FALSE;				// Creep Off
	
	if(u8CrawlData == TRUE)
	{
		sAmt.bCrawl = TRUE;					// Display Crawl
	}	
	else
		sAmt.bCrawl = FALSE;				// Crawl off

}

/*********************************************************************//**
 *
 * Gear Status Desicion Making
 *
 * @param      None   
 *
 * @return     None
 *************************************************************************/
static void _vProcessGear()
{
	u8 u8Gear = 0;
	static u32 u32TimeCount =0;
	static bool bLampStatusFlag = 0;

	if(u8GetTransmissionType() == AMT_TRANS || u8GetTransmissionType() == AT_TRANS)
	{
		if(bGetPGNTimeOut(eJGearValue_0x03))                 
	    {
	    	sAmt.u16GearData = DASH_CHAR;
	    }
		
		else
		{
			u8Gear = (u8)u32GetPGNdata(eJGearValue_0x03);

			if(u8GetTransmissionType() == AMT_TRANS)
			{
				if(sAmt.eAMTLearning == eCANLost )               
			      	sAmt.u16GearData = BLANK_CHAR;
			    
				if(sAmt.eAMTLearning == eLearnActive)
				{
					if(u32GetClockTicks() - u32TimeCount > 500 )
		    		{
		    			if(u8Gear == NEUTRAL_DATA)
		    			{
			    			u32TimeCount = u32GetClockTicks() ;
			    			bLampStatusFlag = ~bLampStatusFlag;
							
			    			if(bLampStatusFlag)
			    				sAmt.u16GearData = NEUTRAL_CHAR;	     				
			    			
			    			else
			    				sAmt.u16GearData = BLANK_CHAR;	 
		    			}
						
						else
							sAmt.u16GearData = DASH_CHAR;
		    		}
					
				}
				
				else if(sAmt.eAMTLearning == eLearnNotActive)
				{
					u32TimeCount = u32GetClockTicks() ;
					if(u8Gear == NEUTRAL_DATA)
						sAmt.u16GearData = NEUTRAL_CHAR;
					
					else if(u8Gear == PARK_ON)
						sAmt.u16GearData = PARK_CHAR;
					
					else if(u8Gear <= TOP_GEAR && u8Gear > NEUTRAL_DATA)
					   	sAmt.u16GearData = u8Gear - NEUTRAL_DATA;
					
					else if(u8Gear < NEUTRAL_DATA)
						sAmt.u16GearData = REVERSE_CHAR;

					else if(u8Gear > TOP_GEAR && u8Gear < PARK_ON)
						sAmt.u16GearData = TOP_GEAR - NEUTRAL_DATA;	// always show top gear i.e 0x91-0x7d	
						
					else if(u8Gear > PARK_ON)
						sAmt.u16GearData = DASH_CHAR;
					
				}
			}
			
			else if(u8GetTransmissionType() == AT_TRANS && sAmt.eAMTLearning != eCANLost )
			{
				u32TimeCount = u32GetClockTicks() ;
				if(u8Gear == NEUTRAL_DATA)
					sAmt.u16GearData = NEUTRAL_CHAR;
				
				else if(u8Gear == PARK_ON)
					sAmt.u16GearData = PARK_CHAR;
				
				else if(u8Gear <= TOP_GEAR && u8Gear > NEUTRAL_DATA)
				   	sAmt.u16GearData = u8Gear - NEUTRAL_DATA;
				
				else if(u8Gear < NEUTRAL_DATA)
					sAmt.u16GearData = REVERSE_CHAR;

				else if(u8Gear > TOP_GEAR && u8Gear < PARK_ON)
					sAmt.u16GearData = TOP_GEAR - NEUTRAL_DATA;	// always show top gear i.e 0x91-0x7d	
					
				else if(u8Gear > PARK_ON)
					sAmt.u16GearData = DASH_CHAR;		
				
			}

			else if(u8GetTransmissionType() == AT_TRANS && sAmt.eAMTLearning == eCANLost )               
		    {
		    	sAmt.u16GearData = BLANK_CHAR;
		    }
		
		}
	}
	else if(u8GetTransmissionType() == MANUAL_TRANS)
	{
		if(bGetPGNTimeOut(eJManualGear_0x21))                 
	    {
		    
		    	sAmt.u16GearData = BLANK_CHAR;
	    }
		else
		{
			u32TimeCount = u32GetClockTicks() ;
			u8Gear = (u8)u32GetPGNdata(eJManualGear_0x21);
			if(u8Gear == NEUTRAL_DATA)
				sAmt.u16GearData = NEUTRAL_CHAR;
			
			else if(u8Gear == PARK_ON)
				sAmt.u16GearData = PARK_CHAR;
			
			else if(u8Gear <= TOP_GEAR && u8Gear > NEUTRAL_DATA)
			{	
				if(sIndicatorInputType.Inputsbytes.Byte4.bLGearDisplay == 1)
				{
					if(u8Gear == 0x7E) // gear number one.
					{
						sAmt.u16GearData = L_GEAR;
					}
					else
						sAmt.u16GearData = u8Gear - NEUTRAL_DATA - 1;
				}
				else
					sAmt.u16GearData = u8Gear - NEUTRAL_DATA;
			}			   	
			
			else if(u8Gear < NEUTRAL_DATA)
				sAmt.u16GearData = REVERSE_CHAR;

			else if(u8Gear > TOP_GEAR && u8Gear < PARK_ON)	// always show top gear i.e 0x91-0x7d
				sAmt.u16GearData = TOP_GEAR - NEUTRAL_DATA ;			
			else if(u8Gear > PARK_ON)
				sAmt.u16GearData = BLANK_CHAR;					
		}			
	}

	else
		sAmt.u16GearData = BLANK_CHAR;
}



/*********************************************************************//**
 *
 * AMT system processing in Main.c
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vAMTProcessFlow()
{
	u8 u8TransType = 0;
	u8 ETC7Data = eCANLost;
	
	sAmt.bCreep = FALSE;					// Creep Not Shown 
	sAmt.bCrawl = FALSE;					// Crawl Not Shown
	sAmt.eAMTLearning = eCANLost;			// Learning is CAN LOST
	sAmt.eAutoManual = eAMNotShown;
	sAmt.eAmtEcoPower = eEPNotShown;
	
	u8TransType = u8GetTransmissionType();

	
	if(bGetPGNTimeOut(eJETC7))                
		NOP();
	else
	{
		ETC7Data = (u8)u32GetPGNdata(eJETC7);		// Learning Data
		
		if(ETC7Data == eLearnActive)
			sAmt.eAMTLearning = eLearnActive;
		
		else 
			sAmt.eAMTLearning = eLearnNotActive;
	}
	
	
	if(u8TransType == AMT_TRANS)
	{
	
		if(sAmt.eAMTLearning != eLearnNotActive)
			NOP();  								
			
		else if(sAmt.eAMTLearning == eLearnNotActive)
		{
		
			_vAutoManualDisplayProcess();					// AUTO/MANUAL_ECO/POWER
			_vCreepCrawlDisplayProcess();					// CREEP CRAWL
		}
	}
	else if(u8TransType == AT_TRANS)
	{
	
		if(sAmt.eAMTLearning == eCANLost)
		{
			sAmt.eAtPower = eEPError;
			sAmt.eAtEco = eEPError;
		}
		
		else
		{			
			// Transmission Fluid Temp. Telltale in TellTale.c 
			// Shift Inhibit in function _vAMTMessageProcess()
			
			_vEcoPowerAT();
		}
	}

	_vProcessGear();								// GEAR CALCULATION
	//_vAMTMessageProcess();
}



/*********************************************************************//**
 *
 * Eco and Power decision for AT transmission type. Private Function
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

static void _vEcoPowerAT()
{
    u8 data1 = eEPNotShown;	
	u8 data2 = eEPNotShown;	
	data1 = (u8)u32GetPGNdata(eJPower); 
	data2 = (u8)u32GetPGNdata(eJAuto_Manual_Eco); 
	
	if(data1 == POWER_ON)
		sAmt.eAtPower = POWER_ON;
	else	
		sAmt.eAtPower = eEPError;   
	
	if(data2 == ECO_ON)
		sAmt.eAtEco = ECO_ON;	
	else	
		sAmt.eAtEco = eEPError;   
}


/*********************************************************************//**
 *
 * Eco and Power decision for AMT transmission type. Private Function
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

static void _vEcoPowerAMT()
{
    u8 data = eEPNotShown;	
	
	data = (u8)u32GetPGNdata(eJPower); 
	
	if(data == ePower)
		sAmt.eAmtEcoPower = ePower;
	
	else if(data == eEco)
		sAmt.eAmtEcoPower = eEco;
	
	else	
		sAmt.eAmtEcoPower = eEPError;   
}

/*********************************************************************//**
 *
 * Gear Status 
 *
 * @param      None    
 *
 * @return     sAmt.bManual 		Boolean Data if Manual is set or Clear
 *************************************************************************/
u16 u16GetGearAMT()
{
	return sAmt.u16GearData;
}


/*********************************************************************//**
 *
 * Auto/Manual Status 
 *
 * @param      None    
 *
 * @return     sAmt.bManual 		Boolean Data if Manual is set or Clear
 *************************************************************************/
eAM_STATUS eGetAutoManualStatus()
{
	return sAmt.eAutoManual;
}

/*********************************************************************//**
 *
 * POWER Status 
 *
 * @param      None    
 *
 * @return     sAmt.bPower 		Boolean Data if Power is set or Clear
 *************************************************************************/
eEP_STATUS eGetEcoPowerStatus()
{
	return sAmt.eAmtEcoPower;
}

/*********************************************************************//**
 *
 * POWER Status 
 *
 * @param      None    
 *
 * @return     sAmt.bPower 		Boolean Data if Power is set or Clear
 *************************************************************************/
eEP_STATUS eGetAtEcoStatus()
{
	return sAmt.eAtEco;
}

/*********************************************************************//**
 *
 * POWER Status 
 *
 * @param      None    
 *
 * @return     sAmt.bPower 		Boolean Data if Power is set or Clear
 *************************************************************************/
eEP_STATUS eGetAtPowerStatus()
{
	return sAmt.eAtPower;
}



/*********************************************************************//**
 *
 * AMT Learn Status 
 *
 * @param      None    
 *
 * @return     sAmt.eAMTLearning 		
 *************************************************************************/
eLEARN_STATUS eGetAMTLearningStatus()
{
	return sAmt.eAMTLearning;
}


/*********************************************************************//**
 *
 * CREEP Status 
 *
 * @param      None    
 *
 * @return     sAmt.bCreep 		Boolean Data if Creep is set or Clear
 *************************************************************************/
bool bGetCreepStatus()
{
	return sAmt.bCreep;
}

/*********************************************************************//**
 *
 * CRAWL Status 
 *
 * @param      None    
 *
 * @return     sAmt.bCrawl 		Boolean Data if Crawl is set or Clear
 *************************************************************************/
bool bGetCrawlStatus(void)
{
	return sAmt.bCrawl;
}

/*********************************************************************//**
 *
 * Inhibit message Status 
 *
 * @param      None    
 *
 * @return     sAmt.u8Msgs 		8bit Data
 *************************************************************************/
u8 u8GetShiftInhibitState()
{
	return u8ShiftInhibit;
}

void vCameraEnable()
{
  u16 gear = 0;
  gear = u16GetGearAMT();
  if(gear == REVERSE_CHAR)
  		CAMERA_ENABLE = HIGH;
  else
  		CAMERA_ENABLE = LOW;
}


