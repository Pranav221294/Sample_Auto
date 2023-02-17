/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/
#include "dataflash.h"
#include "r_cg_userdefine.h"
#include "Eol.h" 
#include "Afe.h" 
#include "OdoMeter.h"
#include "GenIllumination.h" 
#include "HourMeter.h"
#include "ServiceReminder.h"
#include "LCDScreens.h"
#include "Clock.h"
#include "Event.h"
#include "DtcCycle.h" 


//constant declarations -------------------------------------------------------------------------------
//ODO 3 Byte + ODO Fr 1 Byte + TripA 2 Byte + TripA Fr 1 Byte + TripB 2 Byte + TripB Fr 1 Byte  = 10
#define ODO_MEM_START_INDEX			0
#define ODO_MEM_END_INDEX 			2
#define ODO_FR_MEM_INDEX			3
#define ODO_TRIPA_MEM_START_INDEX	4
#define ODO_TRIPA_MEM_END_INDEX		5
#define ODO_TRIPA_FR_MEM_INDEX		6
#define ODO_TRIPB_MEM_START_INDEX	7
#define ODO_TRIPB_MEM_END_INDEX		8
#define ODO_TRIPB_FR_MEM_INDEX		9
//Hour 3 Byte + Hour Fr & Second 2 Byte + Hr TripA 2 Byte + Hr TripA Fr & Second 2 Byte 
//+ Hr TripB 2 Byte + Hr TripB Fr & Second 2 Byte  = 13
#define HOUR_MEM_START_INDEX					0
#define HOUR_MEM_END_INDEX 						2
#define HOUR_FR_SEC_MEM_START_INDEX				3
#define HOUR_FR_SEC_MEM_END_INDEX 				4
#define HOUR_TRIPA_MEM_START_INDEX				5
#define HOUR_TRIPA_MEM_END_INDEX 				6
#define HOUR_TRIPA_FR_SEC_MEM_START_INDEX		7
#define HOUR_TRIPA_FR_SEC_MEM_END_INDEX 		8
#define HOUR_TRIPB_MEM_START_INDEX				9
#define HOUR_TRIPB_MEM_END_INDEX 				10
#define HOUR_TRIPB_FR_SEC_MEM_START_INDEX		11
#define HOUR_TRIPB_FR_SEC_MEM_END_INDEX 		12

//LFC Byte 4 + PreFuelConsume Byte  4 + Trip Offset Byte 2 + Trip Afevalue Byte 2 +  EEPROMOffsetFlag 1 = 11

#define TRIPA_LFC_MEM_START_INDEX					0
#define TRIPA_LFC_MEM_END_INDEX 					3
#define TRIPA_PRE_FC_MEM_START_INDEX				4
#define TRIPA_PRE_FC_MEM_END_INDEX 					7
#define TRIPA_OFFSET_MEM_START_INDEX				8
#define TRIPA_OFFSET_MEM_END_INDEX 					9
#define TRIPA_AFE_VALUE_MEM_START_INDEX				10
#define TRIPA_AFE_VALUE_MEM_END_INDEX 				11
#define TRIPA_EEPROM_OFFSET_MEM_INDEX				12

//LFC Byte 4 + PreFuelConsume Byte  4 + Trip Offset Byte 2 + Trip Afevalue Byte 2 +  EEPROMOffsetFlag 1
#define TRIPB_LFC_MEM_START_INDEX					0
#define TRIPB_LFC_MEM_END_INDEX 					3
#define TRIPB_PRE_FC_MEM_START_INDEX				4
#define TRIPB_PRE_FC_MEM_END_INDEX 					7
#define TRIPB_OFFSET_MEM_START_INDEX				8
#define TRIPB_OFFSET_MEM_END_INDEX 					9
#define TRIPB_AFE_VALUE_MEM_START_INDEX				10
#define TRIPB_AFE_VALUE_MEM_END_INDEX 				11
#define TRIPB_EEPROM_OFFSET_MEM_INDEX				12

//local declarations ----------------------------------------------------------------------------------

static eel_status_t 		_snewEelStatus;				// Static Structure for EEl status
static u8 __far*  			_pnewVersionStringu8;		// Static pointer
static eel_request_t 		_snewEelRequestStr;			// Static Structure for EEl request
static eel_request_t 		_snewRefrEelRequestStr;		// Static Structure for EEl reference request
static eel_driver_status_t 	_snewEelDriverStatus;		// Static Structure for EEl Driver status

static fdl_status_t 		_snewFdlStatus;				// Static Structure for FDL status
static fdl_request_t 		_snewFdlRequestStr;			// Static Structure for FDL request
static fdl_request_t 		_snewRefrFdlRequestStr;		// Static Structure for FDL reference request



extern    __far const fdl_descriptor_t  _sfdlDescriptorStr;
u16 EOL_ADR_OFFSET = 0x00;

/*******************************************************************************************************
*****************************************Global System params Sturcture*********************************
									ADD MORE SYSTEM PARAMETERS IF REQUIRED
********************************************************************************************************/
typedef struct
{	
	u32 u32ClockData;
	u32 u32ServiceDueHours;	
	u32 u32ServiceCount;
	u32 u32DuplicateOdo;
	u32 u32DuplicateHour;	
	u8 u8SetDefaultFlag1;
	u8 u8SetDefaultFlag2;
	u8 u8GenralIllSetting;
	u8 u8OdoEolWriteCounter;
	u8 u8HourEolWriteCounter;	
		
}EE_DATA_TYPE;




EE_DATA_TYPE ssysParms =
{               
        0x0C000000, //u32ClockData
        40000, 		//u32ServiceDueHours 
	    0x00,      	//u32DuplicateOdo
        0x00,      	//u32DuplicateHour
        0x00,      	//u8Servicecounter
        0x00,     	//u8SetDefaultFlag1
        0x00,     	//u8SetDefaultFlag2       
        0x35,      	//u8GeneralIllSetting
        0x00,		//Odo Eol write Counter
        0x00,		//Hour Eol write Counter
        
};

static u8 u8BootloaderState = eApplicationMode;
static u8 u8OdoSystemParmeter[ODO_SYS_PARAMETER];
static u8 u8HourSystemParmeter[HOUR_SYS_PARAMETER];
static u8 u8AfeASystemParmeter[AFE_A_SYS_PARAMETER];
static u8 u8AfeBSystemParmeter[AFE_B_SYS_PARAMETER];

static void _vReadOdoSysParms();
static void _vReadHourSysParms();
static void _vReadTripAAfeSysParms();
static void _vReadTripBAfeSysParms();
static void _vSaveOdoSysParms();
static void _vSaveHourSysParms();
static void _vSaveTripAAfeSysParms();
static void _vSaveTripBAfeSysParms();
static void _vReadDtcData(eDtcDataIndex eDtcData);
static void _vSaveDtcData(eDtcDataIndex eDtcData);
static void _vGetSysParms(eSysParmsIndex eSysParms);
static void _vSaveSysParms(u32 write_data, eSysParmsIndex eSysParms);


/*********************************************************************//**
 *
 * Handles Errors while EEL write or read for 3 clock cycles.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vMyErrorHandler(void)
{
	NOP();NOP();NOP();

	
}

/*********************************************************************//**
 *
 * Clears all blocks in EEL pool. This is a global function. Can be called from main.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
void vEelFormat()
{
	_snewEelRequestStr.command_enu = EEL_CMD_FORMAT;
    EEL_Execute(&_snewEelRequestStr);
    while (_snewEelRequestStr.status_enu == EEL_BUSY)
    {
      	EEL_Handler();
    }
    if (_snewEelRequestStr.status_enu != EEL_OK)
    {
      	_vMyErrorHandler();
    }	
}

/*********************************************************************//**
 *
 * Initializing EEL pool.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vEelInit()
{
	_snewEelStatus = EEL_Init();
	if(_snewEelStatus != EEL_OK)
    {
      	_vMyErrorHandler();
	}
	//__nop();
	NOP();
	EEL_Open();
	//__nop();
	NOP();
	EEL_GetDriverStatus((__near eel_driver_status_t*)&_snewEelDriverStatus);		
}


/*********************************************************************//**
 *
 * Initializing FDL pool.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vFdlInit()
{
	_snewFdlStatus = FDL_Init(&fdl_descriptor_str);
	if(_snewFdlStatus != FDL_OK)
	{
		_vMyErrorHandler();
	}
	else
	{
		NOP();
	}
	NOP();
	FDL_Open();
	NOP();
}

/*********************************************************************//**
 *
 * Initiates EEL bus. Error handling if BUSY.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vEelStartUp()
{
	_snewEelRequestStr.command_enu = EEL_CMD_UNDEFINED;
	_snewEelRequestStr.identifier_u8 = 0;
	_snewEelRequestStr.address_pu8 = 0;
	_snewEelRequestStr.status_enu = EEL_ERR_PARAMETER;
	_snewEelRequestStr.command_enu = EEL_CMD_STARTUP;
	EEL_Execute(&_snewEelRequestStr);

	while (_snewEelRequestStr.status_enu == EEL_BUSY)
	{
	        EEL_Handler();
	}
	if (_snewEelRequestStr.status_enu != EEL_OK)
	{
	        _vMyErrorHandler();
	}
	if (_snewEelRequestStr.status_enu == EEL_ERR_POOL_INCONSISTENT)
	{
	        vEelFormat();
	}
	if (_snewEelRequestStr.status_enu == EEL_ERR_POOL_EXHAUSTED)
	{
	        vEelFormat();
	}
	EEL_GetDriverStatus((__near eel_driver_status_t*)&_snewEelDriverStatus);	
}


/*********************************************************************//**
 *
 * Initiates FLASH memeory.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
void vDataFlashinit()
{
	_vFdlInit();
	_vEelInit();
	_vEelStartUp();
}

/*********************************************************************//**
 *
 * Write Eol array Data with respect to identifier.
 *
 * @param     Data Array to be written 
 * @param      Identifier 
 *
 * @return     None
 *************************************************************************/
static void _vEolWrite(u8* write_data, u8 u8id)
{
	_snewEelRequestStr.command_enu = EEL_CMD_WRITE;
	_snewEelRequestStr.address_pu8 = (__near u8*)&write_data[0];
	_snewEelRequestStr.identifier_u8 = u8id;

	do
	{
	 	EEL_Handler();
	  	EEL_Execute(&_snewEelRequestStr);
	} while (_snewEelRequestStr.status_enu == EEL_ERR_REJECTED);

	while (_snewEelRequestStr.status_enu == EEL_BUSY)
	{
	  	EEL_Handler();
	}

	if (_snewEelRequestStr.status_enu == EEL_ERR_POOL_FULL)
	{
	  _snewRefrEelRequestStr.command_enu = EEL_CMD_REFRESH;
	  _snewRefrEelRequestStr.identifier_u8 = 0;
	  _snewRefrEelRequestStr.address_pu8 = 0;
	  _snewRefrEelRequestStr.status_enu = EEL_ERR_PARAMETER;
	  do
	  {
	          EEL_Handler();
	          EEL_Execute(&_snewRefrEelRequestStr);
	  } while (_snewRefrEelRequestStr.status_enu == EEL_ERR_REJECTED);

	  while (_snewRefrEelRequestStr.status_enu == EEL_BUSY)
	  {
	          EEL_Handler();
	  }

	  if (_snewRefrEelRequestStr.status_enu != EEL_OK)
	  {
	          _vMyErrorHandler();
	  }
	}
	else if (_snewEelRequestStr.status_enu != EEL_OK)
	{
	  	_vMyErrorHandler();
	}		
}

/*********************************************************************//**
 *
 * Read the last written identifier in EEL pool.
 *
 * @param      Data Array to be written 
 * @param      System Parameter Index 
 *
 * @return     None
 *************************************************************************/
static void _vEolRead(u8* data, eSysParmsIndex eSysParms)
{
	_snewEelRequestStr.command_enu = EEL_CMD_READ;
    _snewEelRequestStr.address_pu8 = (__near eel_u8*)&data[0];
    _snewEelRequestStr.identifier_u8 = eSysParms;
    do
    {
          	EEL_Handler();
          	EEL_Execute(&_snewEelRequestStr);
    } while (_snewEelRequestStr.status_enu == EEL_ERR_REJECTED);

    while (_snewEelRequestStr.status_enu == EEL_BUSY)
    {
            EEL_Handler();
    }

    if (_snewEelRequestStr.status_enu != EEL_OK)
    {
            _vMyErrorHandler();
    }
}

/*********************************************************************//**
 *
 * Write data to EEL Pool.
 *
 * @param      Data Array to be written 
 * @param      System Parameter Identifier 
 *
 * @return     None
 *************************************************************************/
static void _vSysParmsWrite(u32 write_data, u8 u8id)
{
	_snewEelRequestStr.command_enu = EEL_CMD_WRITE;
	_snewEelRequestStr.address_pu8 = (__near u8*)&write_data;
	_snewEelRequestStr.identifier_u8 = u8id;

	do
	{
	 	EEL_Handler();
	  	EEL_Execute(&_snewEelRequestStr);
	} while (_snewEelRequestStr.status_enu == EEL_ERR_REJECTED);

	while (_snewEelRequestStr.status_enu == EEL_BUSY)
	{
	  	EEL_Handler();
	}

	if (_snewEelRequestStr.status_enu == EEL_ERR_POOL_FULL)
	{
	  _snewRefrEelRequestStr.command_enu = EEL_CMD_REFRESH;
	  _snewRefrEelRequestStr.identifier_u8 = 0;
	  _snewRefrEelRequestStr.address_pu8 = 0;
	  _snewRefrEelRequestStr.status_enu = EEL_ERR_PARAMETER;
	  do
	  {
	          EEL_Handler();
	          EEL_Execute(&_snewRefrEelRequestStr);
	  } while (_snewRefrEelRequestStr.status_enu == EEL_ERR_REJECTED);

	  while (_snewRefrEelRequestStr.status_enu == EEL_BUSY)
	  {
	          EEL_Handler();
	  }

	  if (_snewRefrEelRequestStr.status_enu != EEL_OK)
	  {
	          _vMyErrorHandler();
	  }
	}
	else if (_snewEelRequestStr.status_enu != EEL_OK)
	{
	  	_vMyErrorHandler();
	}		
}

/*********************************************************************//**
 *
 * Read the last written identifier in EEL pool.
 *
 * @param      System Parameter Identifier 
 *
 * @return     Data read from eeprom
 *************************************************************************/
static u32 _ulSysParmsRead(eSysParmsIndex eSysParms)
{
	static u32 _ulEolReadData;
	_snewEelRequestStr.command_enu = EEL_CMD_READ;
    _snewEelRequestStr.address_pu8 = (__near eel_u8*)&_ulEolReadData;
    _snewEelRequestStr.identifier_u8 = eSysParms;
    do
    {
          	EEL_Handler();
          	EEL_Execute(&_snewEelRequestStr);
    } while (_snewEelRequestStr.status_enu == EEL_ERR_REJECTED);

    while (_snewEelRequestStr.status_enu == EEL_BUSY)
    {
            EEL_Handler();
    }

    if (_snewEelRequestStr.status_enu != EEL_OK)
    {
            _vMyErrorHandler();
    }
	return _ulEolReadData;
}

/*********************************************************************//**
 *
 * Read Odo Parameter Values from memory and unpacked data 
 * & assign to Odo value .ex. ODO = 0x12F934 , in zero location  Msb byte ,means byte 0x12 
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vReadOdoSysParms()
{
	u32 Temp = 0;
	u8 i,Temp1 = 0;
	
	_vEolRead	(	u8OdoSystemParmeter	,	eOdoIdentifier	);	
	 
   for(i = ODO_MEM_START_INDEX; i <= ODO_MEM_END_INDEX; i++)
   	{
		Temp = Temp <<8;
	    Temp = Temp | u8OdoSystemParmeter[i];
		
   	}
  vSetOdoValue(Temp);
  Temp1 =  u8OdoSystemParmeter[ODO_FR_MEM_INDEX];
  vSetOdoFractionValue(Temp1);

   Temp = 0;
   for(i = ODO_TRIPA_MEM_START_INDEX; i <= ODO_TRIPA_MEM_END_INDEX; i++)
   	{
		Temp = Temp <<8;
	    Temp = Temp | u8OdoSystemParmeter[i];
		
   	}
	vSetOdoTripAValue(Temp);
    Temp1 =  u8OdoSystemParmeter[ODO_TRIPA_FR_MEM_INDEX];
   vSetOdoTripAFractionValue(Temp1);
	

	Temp = 0;
	  for(i = ODO_TRIPB_MEM_START_INDEX; i <= ODO_TRIPB_MEM_END_INDEX; i++)
		{
			Temp = Temp <<8;
			Temp = Temp | u8OdoSystemParmeter[i];
			
		}
		vSetOdoTripBValue(Temp);
		Temp1 =  u8OdoSystemParmeter[ODO_TRIPB_FR_MEM_INDEX];
	   vSetOdoTripBFractionValue(Temp1);
  
}

/*********************************************************************//**
 *
 * Read Hour Parameter Values from memory.
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vReadHourSysParms()
{
	u8 i,Temp1 = 0;
	u16 Temp2 =0;
	u32 Temp = 0;
	

	_vEolRead	(	u8HourSystemParmeter	,	eHourIdentifier	);	
	
	 //Unpack hours ,Fraction & Second data and assing to releted variable 
	 for(i = HOUR_MEM_START_INDEX; i <= HOUR_MEM_END_INDEX; i++)
	 {
		  Temp = Temp <<8;
		  Temp = Temp | u8HourSystemParmeter[i];
		  
	 }
	vSetHoursValue(Temp);
	 
	 Temp2 = 0;
	 for(i = HOUR_FR_SEC_MEM_START_INDEX; i <= HOUR_FR_SEC_MEM_END_INDEX; i++)
	  {
		  Temp2 = Temp2 <<8;
		  Temp2 = Temp2 | u8HourSystemParmeter[i];		  
	  }
			
	Temp1 = Temp2 & 0x000F;
	Temp2 = (Temp2 >> 4) & 0x0FFF;
	vSetHoursFractionValue(Temp1);
	vSetHoursCounterValue(Temp2); 
	
	 //Unpack hoursTripA ,Fraction & Second data and assing to releted variable	
	 Temp = 0;
	 for(i = HOUR_TRIPA_MEM_START_INDEX; i <= HOUR_TRIPA_MEM_END_INDEX; i++)
	  {
		  Temp = Temp <<8;
		  Temp = Temp | u8HourSystemParmeter[i];
		  
	  }
	  vSetHoursTripAValue(Temp);
	 
	 Temp2 = 0;
	 for(i = HOUR_TRIPA_FR_SEC_MEM_START_INDEX; i <= HOUR_TRIPA_FR_SEC_MEM_END_INDEX; i++)
	 {
		  Temp2 = Temp2 <<8;
		  Temp2 = Temp2 | u8HourSystemParmeter[i];		  
	 }
			
	Temp1 = Temp2 & 0x000F;
	Temp2 = (Temp2 >> 4) & 0x0FFF;
	vSetHoursTripAFractionValue(Temp1);
	vSetHoursTripACounterValue(Temp2); 

		 //Unpack hoursTripB,Fraction & Second data and assing to releted variable	
	 Temp = 0;
	 for(i = HOUR_TRIPB_MEM_START_INDEX; i <= HOUR_TRIPB_MEM_END_INDEX; i++)
	 {
		  Temp = Temp <<8;
		  Temp = Temp | u8HourSystemParmeter[i];
		  
	 }
	  vSetHoursTripBValue(Temp);
	 
	 Temp2 = 0;
	 for(i = HOUR_TRIPB_FR_SEC_MEM_START_INDEX; i <= HOUR_TRIPB_FR_SEC_MEM_END_INDEX; i++)
	 {
		  Temp2 = Temp2 <<8;
		  Temp2 = Temp2 | u8HourSystemParmeter[i];		  
	 }
			
	Temp1 = Temp2 & 0x000F;
	Temp2 = (Temp2 >> 4) & 0x0FFF;
	vSetHoursTripBFractionValue(Temp1);
	vSetHoursTripBCounterValue(Temp2); 

	
}


/*********************************************************************//**
 *
 * Read TripA Afe Parameter Values from memory.
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vReadTripAAfeSysParms()
{
	u32 Temp = 0;
	u8 i,Temp1 = 0;

	_vEolRead	(	u8AfeASystemParmeter	,	eTripAAfeIdentifier	);

	 Temp = 0;
	 for(i = TRIPA_LFC_MEM_START_INDEX; i <= TRIPA_LFC_MEM_END_INDEX; i++)
	  {
		  Temp = Temp <<8;
		  Temp = Temp | u8AfeASystemParmeter[i];
		  
	  }
	 vSetTripAUsedFuelAtReset(Temp);

	Temp = 0;
	for(i = TRIPA_PRE_FC_MEM_START_INDEX; i <= TRIPA_PRE_FC_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeASystemParmeter[i];
		 
	 }	
	vSetPreFuelConsumedA(Temp);


		Temp = 0;
	for(i = TRIPA_OFFSET_MEM_START_INDEX; i <= TRIPA_OFFSET_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeASystemParmeter[i];		 
	 }	
	vSetTripAOffset((u16)Temp);

		Temp = 0;
	for(i = TRIPA_AFE_VALUE_MEM_START_INDEX; i <= TRIPA_AFE_VALUE_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeASystemParmeter[i];		 
	 }	

	 vSetTripAAfe((u16)Temp);	
	 vSetDispTripAAfe((u16)Temp);

	Temp1 = u8AfeASystemParmeter[TRIPA_EEPROM_OFFSET_MEM_INDEX];

	vSetEEPROMOffsetSaveFlagA(Temp1);
	
}

/*********************************************************************//**
 *
 * Read TripB Afe Parameter Values from memory.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vReadTripBAfeSysParms()
{
		u32 Temp = 0;
		u8 i,Temp1 = 0;

	_vEolRead	(	u8AfeBSystemParmeter	,	eTripBAfeIdentifier	);

	 Temp = 0;
	  for(i = TRIPB_LFC_MEM_START_INDEX; i <= TRIPB_LFC_MEM_END_INDEX; i++)
	  {
		  Temp = Temp <<8;
		  Temp = Temp | u8AfeBSystemParmeter[i];
		  
	  }
	 vSetTripBUsedFuelAtReset(Temp);

	Temp = 0;
	for(i = TRIPB_PRE_FC_MEM_START_INDEX; i <= TRIPB_PRE_FC_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeBSystemParmeter[i];
		 
	 }	
	vSetPreFuelConsumedB(Temp);


		Temp = 0;
	for(i = TRIPB_OFFSET_MEM_START_INDEX; i <= TRIPB_OFFSET_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeBSystemParmeter[i];		 
	 }	
	vSetTripBOffset((u16)Temp);

		Temp = 0;
	for(i = TRIPB_AFE_VALUE_MEM_START_INDEX; i <= TRIPB_AFE_VALUE_MEM_END_INDEX; i++)
	 {
		 Temp = Temp <<8;
		 Temp = Temp | u8AfeBSystemParmeter[i];		 
	 }	

	 vSetTripBAfe((u16)Temp);	
	 vSetDispTripBAfe((u16)Temp);

	Temp1 = u8AfeBSystemParmeter[TRIPB_EEPROM_OFFSET_MEM_INDEX];

	vSetEEPROMOffsetSaveFlagB(Temp1);
}

/*********************************************************************//**
 *
 * Packing Odo Parameter and save in  memory.
 * ex. ODO = 0x12F934 , in zero location goes Msb byte ,means byte 0x12 
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vSaveOdoSysParms()
{
	u32 Temp = 0;
	u8 Temp1 = 0;
	u8 num = 0;
	
	Temp = u32GetOdoValue();
	u8OdoSystemParmeter[num++] = (u8)(Temp >> 16);
	u8OdoSystemParmeter[num++] = (u8)(Temp >> 8);
	u8OdoSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetOdoFractionValue();
	u8OdoSystemParmeter[num++] = Temp1;

	Temp = u32GetOdoTripAValue();		
	u8OdoSystemParmeter[num++] = (u8)(Temp >> 8);
	u8OdoSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetOdoTripAFractionValue();
	u8OdoSystemParmeter[num++] = Temp1;
	
	Temp = u32GetOdoTripBValue();		
	u8OdoSystemParmeter[num++] = (u8)(Temp >> 8);
	u8OdoSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetOdoTripBFractionValue();
	u8OdoSystemParmeter[num++] = Temp1;

	_vEolWrite	(	u8OdoSystemParmeter	,	eOdoIdentifier	);
}
/*********************************************************************//**
 *
 * Packing hour Parameter and save in  memory.
 * ex. Hour = 0x12F934 , in zero location goes Msb byte ,means byte 0x12 
 *	  Fraction and second is merge in 2 byte so 
 * @param      None
 *
 * @return     None
 *************************************************************************/

static void _vSaveHourSysParms()
{
	u32 Temp = 0;	
	u16 Temp2 =0;
	u8 Temp1 = 0;
	u8 num = 0;
	//Hour and hour fraction + second data packing 	
	Temp = u32GetHoursValue();
	u8HourSystemParmeter[num++] = (u8)(Temp >> 16);
	u8HourSystemParmeter[num++] = (u8)(Temp >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetHoursFractionValue();
	Temp2 = u16GetHoursCounterValue();
	Temp2 = (u16)Temp1 | (Temp2 << 4);
	u8HourSystemParmeter[num++] = (u8)(Temp2 >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp2);
	
	//Hour Trip A and hour Trip A fraction + second data packing 
	Temp = u32GetHoursTripAValue();	
	u8HourSystemParmeter[num++] = (u8)(Temp >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetHoursTripAFractionValue();
	Temp2 = u16GetHoursTripACounterValue();
	Temp2 = (u16)Temp1 | (Temp2 << 4);
	u8HourSystemParmeter[num++] = (u8)(Temp2 >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp2);
	
	//Hour Trip B and hour Trip B fraction + second data packing 
	Temp = u32GetHoursTripBValue();	
	u8HourSystemParmeter[num++] = (u8)(Temp >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp);
	Temp1 = u8GetHoursTripBFractionValue();
	Temp2 = u16GetHoursTripBCounterValue();
	Temp2 = (u16)Temp1 | (Temp2 << 4);
	u8HourSystemParmeter[num++] = (u8)(Temp2 >> 8);
	u8HourSystemParmeter[num++] = (u8)(Temp2);

	_vEolWrite	(	u8HourSystemParmeter	,	eHourIdentifier	);
}


/*********************************************************************//**
 *
 * Read TripA Afe Parameter Values from memory.
 *
 * @param      None 
 *
 * @return     None
 *************************************************************************/
static void _vSaveTripAAfeSysParms()
{

	u32 Temp = 0;	
	u16 Temp2 =0;
	u8 Temp1 = 0;
	u8 num = 0;

	//TRIP A LFC AFE data packing 
	Temp = u32GetTripAUsedFuelAtReset();	
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 24);
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 16);
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeASystemParmeter[num++] = (u8)(Temp);
	//TRIP A Pre Fuel Consuption  AFE data packing 
	Temp = u32GetPreFuelConsumedA();
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 24);
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 16);
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeASystemParmeter[num++] = (u8)(Temp);
	//TRIP A Offset AFE data packing 
	Temp = u16GetTripAOffset();	
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeASystemParmeter[num++] = (u8)(Temp);
	//TRIP A AFE value packing 
	Temp = u16GetTripAAfe();	
	u8AfeASystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeASystemParmeter[num++] = (u8)(Temp);
	//AFE EEROM offset value packing 
	//save offset saving flag into eeprom after first reading cycle is complete.
	if(u8GetEEPROMOffsetSaveFlagA() && u8GetReadCountTripA() == SECOND_READ_COUNT)
	{		
		u8AfeASystemParmeter[num++] = TRUE;
		vSetReadCountTripA(OFFSET_SAVE_DONE);
	}

	_vEolWrite	(	u8AfeASystemParmeter	,	eTripAAfeIdentifier	);
}

/*********************************************************************//**
 *
 * Read TripB Afe Parameter Values from memory.
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
static void _vSaveTripBAfeSysParms()
{
	u32 Temp = 0;	
	u16 Temp2 =0;
	u8 Temp1 = 0;
	u8 num = 0;

	//TRIP B LFC AFE data packing 
	Temp = u32GetTripBUsedFuelAtReset();	
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 24);
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 16);
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeBSystemParmeter[num++] = (u8)(Temp);
	//TRIP B Pre Fuel Consuption  AFE data packing 
	Temp = u32GetPreFuelConsumedB();	
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 24);
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 16);
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeBSystemParmeter[num++] = (u8)(Temp);
	//TRIP B Offset AFE data packing 
	Temp = u16GetTripBOffset();	
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeBSystemParmeter[num++] = (u8)(Temp);
	//TRIP B AFE value packing 
	Temp = u16GetTripBAfe();	
	u8AfeBSystemParmeter[num++] = (u8)(Temp >> 8);
	u8AfeBSystemParmeter[num++] = (u8)(Temp);
	//AFE EEROM offset value packing 
	//save offset saving flag into eeprom after first reading cycle is complete.
	if(u8GetEEPROMOffsetSaveFlagB() && u8GetReadCountTripB() == SECOND_READ_COUNT)
	{		
		u8AfeBSystemParmeter[num++] = TRUE;
		vSetReadCountTripB(OFFSET_SAVE_DONE);
	}

		_vEolWrite	(	u8AfeBSystemParmeter	,	eTripBAfeIdentifier	);
}


/*********************************************************************//**
 *
 * Read System Parameter Values from memory.
 *
 * @param      System Parameter Identifier 
 *
 * @return     None
 *************************************************************************/
static void _vGetSysParms(eSysParmsIndex eSysParms)
{
	
	switch(eSysParms)
	{
	case	eBootLoaderIdentifier:					
			u8BootloaderState = (u8)_ulSysParmsRead(eBootLoaderIdentifier);
	break;
	case	eDuplicateOdoId:					
			ssysParms.u32DuplicateOdo = _ulSysParmsRead(eDuplicateOdoId);
	break;
	case	eDuplicateHourId:					
			ssysParms.u32DuplicateHour = _ulSysParmsRead(eDuplicateHourId);
	break;

	case	eHourEolWriteCountIdentifier:					
			ssysParms.u8HourEolWriteCounter = (u8)_ulSysParmsRead(eHourEolWriteCountIdentifier);
	break;
	case	eOdoEolWriteCountIdentifier:					
			ssysParms.u8OdoEolWriteCounter = (u8)_ulSysParmsRead(eOdoEolWriteCountIdentifier);
	break;		
	case	eSetDefaultFlag1:					
			ssysParms.u8SetDefaultFlag1 = (u8)_ulSysParmsRead(eSetDefaultFlag1);// 1 byte return value for u8SetDefaultFlag1
	break;							
	case	eSetDefaultFlag2:					
			ssysParms.u8SetDefaultFlag2 = (u8)_ulSysParmsRead(eSetDefaultFlag2);// 1 byte return value for u8SetDefaultFlag2
	break;
	case	eGenIllSetting:					
			ssysParms.u8GenralIllSetting = (u8)_ulSysParmsRead(eGenIllSetting);// 1 byte return value for u8SetDefaultFlag2
	break;
	case	eClockData:					
			ssysParms.u32ClockData = _ulSysParmsRead(eClockData);
	break;
	case	eServiceCounter:					
			ssysParms.u32ServiceCount = _ulSysParmsRead(eServiceCounter);
	break;
	case	eServiceDueHours:					
			ssysParms.u32ServiceDueHours = _ulSysParmsRead(eServiceDueHours);
	break;
	
	default:
	break;

	}
	
}

/*********************************************************************//**
 *
 * Write System parameters at specific Address.
 *
 * @param      Data to be written.
 * @param      System Parameter Identifier 
 *
 * @return     None
 *************************************************************************/
static void _vSaveSysParms(u32 write_data, eSysParmsIndex eSysParms)
{
	
	switch(eSysParms)
	{	
	case eBootLoaderIdentifier: 				
		 _vSysParmsWrite((u8)write_data,	eBootLoaderIdentifier);
	break;
	case eDuplicateOdoId:					
		 _vSysParmsWrite(	write_data, 	eDuplicateOdoId);
	break;
	case eDuplicateHourId:					
		 _vSysParmsWrite(	write_data, 	eDuplicateHourId);
	break;

	case eHourEolWriteCountIdentifier:					
		 _vSysParmsWrite((u8)write_data,	eHourEolWriteCountIdentifier);
	break;
	case eOdoEolWriteCountIdentifier:					
		 _vSysParmsWrite((u8)write_data,	eOdoEolWriteCountIdentifier);
	break;
	case eSetDefaultFlag1:					
		 _vSysParmsWrite((u8)write_data,	eSetDefaultFlag1);
	break;							
	case eSetDefaultFlag2:					
		 _vSysParmsWrite((u8)write_data,	eSetDefaultFlag2);
	break;
	case eClockData:					
		 _vSysParmsWrite(write_data,	eClockData);
	break;	
	case	eGenIllSetting:					
			 _vSysParmsWrite(write_data,	eGenIllSetting);
	break;
	case	eServiceCounter:					
			 _vSysParmsWrite(write_data,	eServiceCounter);
	break;
	case	eServiceDueHours:					
			 _vSysParmsWrite(write_data,	eServiceDueHours);
	break;	

	default:
	break;
	}
	
}

/*********************************************************************//**
 *
 * Read DTC data with respect to the eDtcData Index Specified
 *
 * @param      eDtcData Index Specified
 *
 * @return     None
 *************************************************************************/
 
static void _vReadDtcData(eDtcDataIndex eDtcData)
{
	
	switch(eDtcData)
	{
		case	eFrontApgShortToGndIdentifier:						
				_vEolRead	(	u8FrontApgDtcStatus	,	eFrontApgShortToGndIdentifier);
		break;	
		case	eRearApgShortToGndIdentifier:						
				_vEolRead	(	u8RearApgDtcStatus	,	eRearApgShortToGndIdentifier);
		break;	
		case	eFuelShortToGndIdentifier:						
				_vEolRead	(	u8FuelDtcStatus	,	eFuelShortToGndIdentifier);
		break;	
		case	eOilpressureShortToGndIdentifier:						
				_vEolRead	(	u8OilPressureDtcStatus	,	eOilpressureShortToGndIdentifier);
		break;		
		case	eCanBusOffIdentifier:						
				_vEolRead	(	u8CanBusOffDtcStatus	,	eCanBusOffIdentifier);
		break;
	}
	
}


/*********************************************************************//**
 *
 * Write DTC data  to memory specified by eEolData Index.
 *
 * @param      eEolData Index Specified
 *
 * @return     None
 *************************************************************************/
static void _vSaveDtcData(eDtcDataIndex eDtcData)
{

	switch(eDtcData)
	{
		case	eFrontApgShortToGndIdentifier:						
				_vEolWrite	(	u8FrontApgDtcStatus	,	eFrontApgShortToGndIdentifier);
		break;	
		case	eRearApgShortToGndIdentifier:						
				_vEolWrite	(	u8RearApgDtcStatus	,	eRearApgShortToGndIdentifier);
		break;	
		case	eFuelShortToGndIdentifier:						
				_vEolWrite	(	u8FuelDtcStatus	,	eFuelShortToGndIdentifier);
		break;	
		case	eOilpressureShortToGndIdentifier:						
				_vEolWrite	(	u8OilPressureDtcStatus	,	eOilpressureShortToGndIdentifier);
		break;		
		case	eCanBusOffIdentifier:						
				_vEolWrite	(	u8CanBusOffDtcStatus	,	eCanBusOffIdentifier);
		break;
	}


}


/*********************************************************************//**
 *
 * Read EOL data with respect to the eEolData Index Specified
 *
 * @param      eEolData Index Specified
 *
 * @return     None
 *************************************************************************/
void vGetEolData(eEolDataIndex eEolData)
{
	
	switch(eEolData)
	{
		case	eVehicleContentBytes:						
				_vEolRead	(	u8VehicleOptionContent	,	eVehicleContentBytes	);
		break;							
		case	eVsPulsPerRevBytes:						
				_vEolRead	(	u8VsPulsPerRev	,	eVsPulsPerRevBytes	);
		break;							
		case	eEngineOverSpeedWarningBytes :						
				_vEolRead	(	u8EngineOverSpeedWarning	,	eEngineOverSpeedWarningBytes	);
		break;							
		case	eFeIndicatorThreshBytes:						
				_vEolRead	(	u8FeIndicatorThresh	,	eFeIndicatorThreshBytes	);
		break;							
		case	eAirPressureOptionBytes:						
				_vEolRead	(	u8AirPressureOption	,	eAirPressureOptionBytes	);
		break;							
		case	eCoolantTempOptionBytes:						
				_vEolRead	(	u8CoolantTempOption	,	eCoolantTempOptionBytes	);
		break;							
		case	eAfeOptionsBytes :						
				_vEolRead	(	u8AfeOptions	,	eAfeOptionsBytes	);
		break;							
		case	eDteMulFactorBytes:						
				_vEolRead	(	u8DteMulFactor	,	eDteMulFactorBytes	);
		break;						
									
		case	eLowOilPressureThreshBytes:						
				_vEolRead	(	u8LowOilPressureThresh	,	eLowOilPressureThreshBytes	);
		break;							
		case	eDisplayScreenConfigBytes:						
				_vEolRead	(	u8DisplayScreenConfig	,	eDisplayScreenConfigBytes	);
		break;							
		case	eTelltalesOptionBytes:						
				_vEolRead	(	u8TelltalesOption	,	eTelltalesOptionBytes	);
		break;	
		case	eBuzzerOptionBytes:						
				_vEolRead	(	u8BuzzerOption	,	eBuzzerOptionBytes	);
		break;	
		case	eDateOfLastProgrammingBytes:						
				_vEolRead	(	u8DateOfLastProgramming	,	eDateOfLastProgrammingBytes	);
		break;							
		case	eVehicleIdBytes:						
				_vEolRead	(	u8VehicleId	,	eVehicleIdBytes	);
		break;							
		case	eVariantDaSetIdBytes :						
				_vEolRead	(	u8VariantDaSetId	,	eVariantDaSetIdBytes	);
		break;							
									
		case	eVehicleConfigNumberBytes:						
				_vEolRead	(	u8VehicleConfigNumber	,	eVehicleConfigNumberBytes	);
		break;							
		case	eParameterPartNumberBytes:						
				_vEolRead	(	u8ParameterPartNumber	,	eParameterPartNumberBytes	);
		break;							
		case	eProgrammingShopCodeBytes:						
				_vEolRead	(	u8ProgrammingShopCode	,	eProgrammingShopCodeBytes	);
		break;	
		case	eFirstService:						
				_vEolRead	(	u8FirstService	,	eFirstService	);
		break;
		case	eServiceInterval:						
				_vEolRead	(	u8ServiceInterval	,	eServiceInterval	);
		break;	
		case	eServiceThreshold:						
				_vEolRead	(	u8ServiceThreshold	,	eServiceThreshold	);
		break;			

		default:
		break;
 }
	
}


/*********************************************************************//**
 *
 * Write data in EOL global Variables to memory specified by eEolData Index.
 *
 * @param      eEolData Index Specified
 *
 * @return     None
 *************************************************************************/
void vSaveEolData(eEolDataIndex eEolData)
{
	
	switch(eEolData)
	{
		case	eVehicleContentBytes:						
				_vEolWrite	(	u8VehicleOptionContent	,	eVehicleContentBytes	);
		break;							
		case	eVsPulsPerRevBytes:						
				_vEolWrite	(	u8VsPulsPerRev	,	eVsPulsPerRevBytes	);
		break;									
		case	eEngineOverSpeedWarningBytes :						
				_vEolWrite	(	u8EngineOverSpeedWarning	,	eEngineOverSpeedWarningBytes	);
		break;							
		case	eFeIndicatorThreshBytes:						
				_vEolWrite	(	u8FeIndicatorThresh	,	eFeIndicatorThreshBytes	);
		break;							
		case	eAirPressureOptionBytes:						
				_vEolWrite	(	u8AirPressureOption	,	eAirPressureOptionBytes	);
		break;							
		case	eCoolantTempOptionBytes:						
				_vEolWrite	(	u8CoolantTempOption	,	eCoolantTempOptionBytes	);
		break;							
		case	eAfeOptionsBytes :						
				_vEolWrite	(	u8AfeOptions	,	eAfeOptionsBytes	);
		break;							
		case	eDteMulFactorBytes:						
				_vEolWrite	(	u8DteMulFactor	,	eDteMulFactorBytes	);
		break;														
									
		case	eLowOilPressureThreshBytes:						
				_vEolWrite	(	u8LowOilPressureThresh	,	eLowOilPressureThreshBytes	);
		break;							
		case	eDisplayScreenConfigBytes:						
				_vEolWrite	(	u8DisplayScreenConfig	,	eDisplayScreenConfigBytes	);
		break;							
		case	eTelltalesOptionBytes:						
				_vEolWrite	(	u8TelltalesOption	,	eTelltalesOptionBytes	);
		break;
		case	eBuzzerOptionBytes:						
				_vEolWrite	(	u8BuzzerOption	,	eBuzzerOptionBytes	);
		break;						
								
		case	eDateOfLastProgrammingBytes:						
				_vEolWrite	(	u8DateOfLastProgramming	,	eDateOfLastProgrammingBytes	);
		break;	
		case	eVehicleIdBytes:						
				_vEolWrite	(	u8VehicleId	,	eVehicleIdBytes	);
		break;							
		case	eVariantDaSetIdBytes :						
				_vEolWrite	(	u8VariantDaSetId	,	eVariantDaSetIdBytes	);
		break;							
									
		case	eVehicleConfigNumberBytes:						
				_vEolWrite	(	u8VehicleConfigNumber	,	eVehicleConfigNumberBytes	);
		break;							
		case	eParameterPartNumberBytes:						
				_vEolWrite	(	u8ParameterPartNumber	,	eParameterPartNumberBytes	);
		break;							
		case	eProgrammingShopCodeBytes:						
				_vEolWrite	(	u8ProgrammingShopCode	,	eProgrammingShopCodeBytes	);
		break;	
		case	eFirstService:	
				_vEolWrite	(	u8FirstService	,	eFirstService	);
				
		break;
		case	eServiceInterval:						
				_vEolWrite	(	u8ServiceInterval	,	eServiceInterval	);
		break;	
		case	eServiceThreshold:						
				_vEolWrite	(	u8ServiceThreshold	,	eServiceThreshold	);
		break;		

		default:
		break;

 }
	
}

/*********************************************************************//**
 *
 * This Function sets all the required variables to DATA FLASH.
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vSetDefaultVariablesToFlash(void)
{
 u8 i =0;
	 _vGetSysParms(eSetDefaultFlag1); 												   // Reading flag variable of eeprom
	 _vGetSysParms(eSetDefaultFlag2);												   // Reading flag variable of eeprom		 
	 if(ssysParms.u8SetDefaultFlag1 != 0xCC && ssysParms.u8SetDefaultFlag2 != 0xCD)
	 {
			 // If condition is false the code will again read for valid datas, if not 
			  //  found all the necessary variables will be initialized. 
			    
				_vGetSysParms(eSetDefaultFlag1);
				_vGetSysParms(eSetDefaultFlag2);
				if(ssysParms.u8SetDefaultFlag1 != 0xCC && ssysParms.u8SetDefaultFlag2 != 0xCD)
				{
					ssysParms.u8SetDefaultFlag1 = 0xCC;
        			ssysParms.u8SetDefaultFlag2 = 0xCD;	
					//Initialize default value 
					for(i = 0; i < ODO_SYS_PARAMETER; i++)
					{
						u8OdoSystemParmeter[i] = 0;
					}
					for(i = 0; i < HOUR_SYS_PARAMETER; i++)
					{
						u8HourSystemParmeter[i] = 0;
					}
					for(i = 0; i < AFE_A_SYS_PARAMETER; i++)
					{
						u8AfeASystemParmeter[i] = 0;
					}	
					for(i = 0; i < AFE_B_SYS_PARAMETER; i++)
					{
						u8AfeBSystemParmeter[i] = 0;
					}
					_vSysParmsWrite(0u, eBootLoaderIdentifier);
					_vEolWrite	(	u8OdoSystemParmeter		,	eOdoIdentifier	);
					_vEolWrite	(	u8HourSystemParmeter	,	eHourIdentifier	);
					_vEolWrite	(	u8AfeASystemParmeter	,	eTripAAfeIdentifier	);
					_vEolWrite	(	u8AfeBSystemParmeter	,	eTripBAfeIdentifier	);
					
					_vSysParmsWrite(ssysParms.u32ClockData, eClockData);
					_vSysParmsWrite(ssysParms.u32ServiceDueHours, eServiceDueHours);
					
					_vSysParmsWrite(ssysParms.u32DuplicateOdo, eDuplicateOdoId);
					_vSysParmsWrite(ssysParms.u32DuplicateHour, eDuplicateHourId);
					_vSysParmsWrite((u8)ssysParms.u8GenralIllSetting, eGenIllSetting);
					_vSysParmsWrite(ssysParms.u32ServiceCount, eServiceCounter);
					_vSysParmsWrite(ssysParms.u8OdoEolWriteCounter, eOdoEolWriteCountIdentifier);
					_vSysParmsWrite(ssysParms.u8HourEolWriteCounter, eHourEolWriteCountIdentifier);
					
					_vEolWrite	(	u8VehicleOptionContent	,	eVehicleContentBytes	);
					_vEolWrite	(	u8VsPulsPerRev	,	eVsPulsPerRevBytes	);
					_vEolWrite	(	u8EngineOverSpeedWarning	,	eEngineOverSpeedWarningBytes	);
					_vEolWrite	(	u8FeIndicatorThresh	,	eFeIndicatorThreshBytes	);
					_vEolWrite	(	u8AirPressureOption	,	eAirPressureOptionBytes	);
					_vEolWrite	(	u8CoolantTempOption	,	eCoolantTempOptionBytes	);
					_vEolWrite	(	u8AfeOptions	,	eAfeOptionsBytes	);
					_vEolWrite	(	u8DteMulFactor	,	eDteMulFactorBytes	);
					_vEolWrite	(	u8LowOilPressureThresh	,	eLowOilPressureThreshBytes	);
					_vEolWrite	(	u8DisplayScreenConfig	,	eDisplayScreenConfigBytes	);
					_vEolWrite	(	u8TelltalesOption	,	eTelltalesOptionBytes	);
					_vEolWrite	(	u8BuzzerOption	,	eBuzzerOptionBytes	);					
					
					_vEolWrite	(	u8DateOfLastProgramming	,	eDateOfLastProgrammingBytes	);
					_vEolWrite	(	u8VehicleId	,	eVehicleIdBytes	);
					_vEolWrite	(	u8VariantDaSetId	,	eVariantDaSetIdBytes	);
			
					_vEolWrite	(	u8VehicleConfigNumber	,	eVehicleConfigNumberBytes	);
					_vEolWrite	(	u8ParameterPartNumber	,	eParameterPartNumberBytes	);
					_vEolWrite	(	u8ProgrammingShopCode	,	eProgrammingShopCodeBytes	);
					_vEolWrite	(	u8FirstService	,	eFirstService	);
					_vEolWrite	(	u8ServiceInterval	,	eServiceInterval	);
					_vEolWrite	(	u8ServiceThreshold	,	eServiceThreshold	);
					_vEolWrite	(	u8FuelTankCapacity	,	eFuelTankCapacity	);							
					_vEolWrite	(	u8CanSpeedoFactor	,	eCanSpeedoFactor	);
					_vEolWrite	(	u8CanSpeedoFactor	,	eCanSpeedoFactork3k4);
										//set default DTC setting 
					_vEolWrite	(	u8FrontApgDtcStatus	,	eFrontApgShortToGndIdentifier);
					_vEolWrite	(	u8RearApgDtcStatus	,	eRearApgShortToGndIdentifier);
					_vEolWrite	(	u8FuelDtcStatus	,	eFuelShortToGndIdentifier);
					_vEolWrite	(	u8OilPressureDtcStatus	,	eOilpressureShortToGndIdentifier);
					_vEolWrite	(	u8CanBusOffDtcStatus	,	eCanBusOffIdentifier);
					_vSysParmsWrite((u8)ssysParms.u8SetDefaultFlag1, eSetDefaultFlag1);				// DO NOT MODIFY
        			_vSysParmsWrite((u8)ssysParms.u8SetDefaultFlag2, eSetDefaultFlag2);				// DO NOT MODIFY

				}
	 }



}


/*********************************************************************//**
 *
 * Read  system parameters and clock data from internal flash 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vReadSystemParameter()
{
	 u8 sec,min,hour;
	 u8 i = 0;
	 u16 Tempvar;
	 u8 Temp1 =0;
	 u16 Temp2 =0;

   for(i = eClockData; i < eNoOfSysParaMeter; i++)
		_vGetSysParms(i);
  
	_vReadOdoSysParms();
   	_vReadHourSysParms();
	_vReadTripAAfeSysParms();
	_vReadTripBAfeSysParms();
	
	// gu8NightIll store in LSB nibble and gu8DayLightIll store in LSB nibble
	Tempvar = ssysParms.u8GenralIllSetting;
	Temp1 = Tempvar & 0x0F;
	Temp2 = Tempvar >> 4;
	vSetDayLightIlluValue(Temp1);
	vSetNightIlluValue((u8)Temp2);
	//Set service counter value
	vSetServiceCounter(ssysParms.u32ServiceCount);
	vSetNextDueService(ssysParms.u32ServiceDueHours);

	//Clock data	
	hour = (u8)((ssysParms.u32ClockData>>24) & 0x000000FF);	
	min  = (u8)((ssysParms.u32ClockData>>16) & 0x000000FF);
	sec = (u8)((ssysParms.u32ClockData>>8)& 0x000000FF);
	
	Temp1 = (u8)(ssysParms.u32ClockData)& 0x000000FF;
	vSetAmPmFlag((u8)Temp1);
	//Add  power on second in actual clock time 
	sec += u8GetClockPwrOnSecond();
	if( sec >= 60)
	{
		 sec = sec-60;
		 if( ++min >= 60 )
		 	{
			 	 min = 0;
				 if( ++hour > 12 )
				 	{
					 	hour = 1;
						Temp1 = bGetClockAmPmFlag();
						vSetAmPmFlag((u8)Temp1);
				 	}
		 	}
	}

	vSetHours(hour);
	vSetMinutes(min);
	vSetSeconds(sec);

	Temp1 = u8GetClockHours();
	vSetDispHours(Temp1);

	Temp1 = u8GetClockMinutes();
	vSetDispMinutes(Temp1);

	Temp1 = bGetClockAmPmFlag();
	vSetDispAmPmFlag(Temp1);
	
	//This flag must be false after clock data read and clock variable initialize
	vSetPowerONCountFlag(FALSE);

 
}


/*********************************************************************//**
 *
 * Save system parmeters in interal Flash, this function call one's while Igintion OFF
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vSaveFullSystemParameter()
{
	u8 Temp1 =0;
	u16 Temp2 =0;
	u32 Temp3 =0;
	
	if( bGetServiceDoneFlag() )
	{
		
		ssysParms.u32ServiceCount = u32getServiceCounter();
		_vSaveSysParms( ssysParms.u32ServiceCount, eServiceCounter);
		
		ssysParms.u32ServiceDueHours = u32GetNextDueService();
		_vSaveSysParms( ssysParms.u32ServiceDueHours, eServiceDueHours);

		
		vSetServiceDoneFlag(FALSE);
	}

	if( bGetGenIlluUpdateFlag() )
	{
		vSetGenIlluUpdateFlag(FALSE);
		// gu8NightIll store in LSB nibble and gu8DayLightIll store in LSB nibble
		Temp1 = u8GetDayLightIlluValue();
		Temp2 = u8GetNightIlluValue();
		ssysParms.u8GenralIllSetting =  ( (Temp2 << 4) | Temp1 );
		_vSaveSysParms( ssysParms.u8GenralIllSetting, eGenIllSetting);
	}

	
	if(bGetOdoUpdateFlagStatus() || bGetOdoTripAUpdateStatus() || bGetOdoTripBUpdateStatus())
	{
		vSetOdoUpdateFlagStatus(FALSE);
		vSetOdoTripAUpdateStatus(FALSE);
		vSetOdoTripBUpdateStatus(FALSE);
		_vSaveOdoSysParms();
	}
	//Hour packet save in memory in every 6 minute 
	if(bGetHoursFractionUpdateStatus() || bGetHoursTripAUpdateStatus() || bGetHoursTripBUpdateStatus()) 
	{
		vSetHoursFractionUpdateStatus(FALSE);
		vSetHoursTripAUpdateStatus(FALSE);
		vSetHoursTripBUpdateStatus(FALSE);
		_vSaveHourSysParms();
	}
	
	if( bGetUpdatePreFuelConsumed() || bGetUpdateTripAFuelUsedAtReset()
		|| bGetUpdateTripBFuelUsedAtReset()|| bGetUpdateTripAOffset() 
		|| bGetUpdateTripBOffset()|| (u8GetEEPROMOffsetSaveFlagA() && u8GetReadCountTripA() == SECOND_READ_COUNT)
		|| (u8GetEEPROMOffsetSaveFlagB() && u8GetReadCountTripB() == SECOND_READ_COUNT))
	{
		vSetUpdatePreFuelConsumed(FALSE);
		vSetUpdateAfeTripAFuelUsedAtReset(FALSE);
		vSetUpdateAfeTripBFuelUsedAtReset(FALSE);
		vSetUpdateTripAOffset(FALSE);
		vSetUpdateTripBOffset(FALSE);
		vSetUpdateAfeTripA(FALSE);
		vSetUpdateAfeTripB(FALSE);
		_vSaveTripAAfeSysParms();
		_vSaveTripBAfeSysParms();
	}

	vSaveClockData();
 
}

void vSaveBootloaderState(u8 data)
{
	u8BootloaderState = data;
	_vSaveSysParms(u8BootloaderState, eBootLoaderIdentifier);
}


void vSaveOdoUpdateByEolCounter(u8 value)
{		
		_vSaveSysParms( value, eOdoEolWriteCountIdentifier);
}

void vSaveHourUpdateByEolCounter(u8 value)
{
	_vSaveSysParms( value, eHourEolWriteCountIdentifier);
}

u8 u8GetOdoUpdateByEolCounter()
{
	return ssysParms.u8OdoEolWriteCounter;
}

u8 u8GetHourUpdateByEolCounter()
{
	return ssysParms.u8HourEolWriteCounter;
}


/*********************************************************************//**
 *
 * Save  clock data if update second 
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vSaveClockData()
{
	
	u32 tempClockdata = 0;
	u8 Temp1 = 0;
	bool Temp2 = 0;
	u8 Temp3 = 0;
	
	if(bGetClockUpdateData())
	{
		vSetUpdateData(FALSE);
		
		Temp1 = u8GetClockHours(); 
		tempClockdata = Temp1;
		
		Temp1 = u8GetClockMinutes();
		tempClockdata = (tempClockdata<<8) |Temp1;
		
		Temp1 = u8GetClockSeconds();
		tempClockdata = (tempClockdata<<8) |Temp1;
		
		Temp2 = bGetClockAmPmFlag();
		tempClockdata = (tempClockdata<<8) |Temp2;
		
		ssysParms.u32ClockData = tempClockdata;
		_vSaveSysParms(ssysParms.u32ClockData, eClockData);
		
	}	
	
}

/*********************************************************************//**
 *
 * Save system parmeters in interal Flash, this function call one's while Igintion OFF
 * 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vResetTripAfeData()
{	
	u8 Temp1 = 0;
	u16 Temp2 = 0;
	u32 Temp3 = 0;
	bool Temp4 = 0;
	
	ODO_RESET eOdoTripRstStatus = eNoOdoReset;
	HOUR_RESET eHourTripRstStatus = eHourTripAReset;
	AFE_RESET eAfeResetStatusA = eNoAFEReset;
	AFE_RESET eAfeResetStatusB = eNoAFEReset;

	eOdoTripRstStatus = u8GetOdoTripRstStatus();
	
	eHourTripRstStatus = u8GetHourTripRstStatus();
	
	eAfeResetStatusA = u8GetAfeRstStatusA();
	eAfeResetStatusB = u8GetAfeRstStatusB();

	if(eOdoTripRstStatus == eOdoTripAReset)
	{
		// Write OdoTripA-------------------------------------				
		vSetOdoTripAValue(0);
		vSetOdoTripAUpdateStatus(TRUE);		
		// Write OdoTripA Fraction----------------------------		
		vSetOdoTripAFractionValue(0);
		vSetOdoTripAPulseCounter(0);
		vSetOdoTripAFractionUpdateStatus(TRUE);			    
		vSetOdoTripRstStatus(eOdoTripResetCANTx);
		vSetSendTripResetStatus(TRUE);
	}
	
	if(eOdoTripRstStatus == eOdoTripBReset)
	{
		// Write OdoTripB-------------------------------------
		vSetOdoTripBValue(0);
		vSetOdoTripBUpdateStatus(TRUE);	
		
		// Write OdoTripB Fraction----------------------------
		vSetOdoTripBFractionValue(0);
		vSetOdoTripBPulseCounter(0);
		vSetOdoTripBFractionUpdateStatus(TRUE);
		vSetOdoTripRstStatus(eOdoTripResetCANTx);
		vSetSendTripResetStatus(TRUE);
	}

	if(eHourTripRstStatus == eHourTripAReset)
	{
		// Write Hour Meter TripA-----------------------------
		vSetHoursTripAValue(0);
		vSetHoursTripACounterValue(0);
		vSetHoursTripAUpdateStatus(TRUE);
		
		// Write Hour Meter TripA Fraction---------------------------
		vSetHoursTripAFractionValue(0);
		vSetHoursTripAFractionUpdateStatus(TRUE);
		vSetHourTripRstStatus(eNoHourReset);
	}
	if(eHourTripRstStatus == eHourTripBReset)
	{
		// Write Hour Meter TripB------------------------------------
		vSetHoursTripBValue(0);
		vSetHoursTripBCounterValue(0);
		vSetHoursTripBUpdateStatus(TRUE);

		// Write Hour Meter TripB Fraction---------------------------
		vSetHoursTripBFractionValue(0);
		vSetHoursTripBFractionUpdateStatus(TRUE);		
        vSetHourTripRstStatus(eNoHourReset);
	}

	if(eAfeResetStatusA == eResetAFETripA)
	{
		vSetTripAAfe(NULL);					// Set AFE TripA Data
		Temp2 = u16GetTripAAfe();			// Get AFE TripA Data
		vSetDispTripAAfe(Temp2);			// Set Display AFE TripA Data
		vSetUpdateAfeTripA(TRUE);
		Temp3 = u32GetTotalFuelConsumed();
		vSetTripAUsedFuelAtReset(Temp3);
		vSetUpdateAfeTripAFuelUsedAtReset(TRUE);
		vSetReadCountTripA(NULL);
		vSetTripAOffset(NULL);
		vSetUpdateTripAOffset(TRUE);
		vSetEEPROMOffsetSaveFlagA(FALSE);	
		vSetAfeRstAStatus(eAFEAtLowLfcA);
		NOP();
	}
	
	if(eAfeResetStatusB == eResetAFETripB)
	{
		vSetTripBAfe(NULL);
		Temp2 = u16GetTripBAfe();
		vSetDispTripBAfe(Temp2);
		vSetUpdateAfeTripB(TRUE);
		Temp3 = u32GetTotalFuelConsumed();
		vSetTripBUsedFuelAtReset(Temp3);
		vSetUpdateAfeTripBFuelUsedAtReset(TRUE);
		vSetReadCountTripB(NULL);
		vSetTripBOffset(NULL);
		vSetUpdateTripBOffset(TRUE);
		vSetEEPROMOffsetSaveFlagB(FALSE);
		vSetAfeRstBStatus(eAFEAtLowLfcB);
		NOP();
	}

}


