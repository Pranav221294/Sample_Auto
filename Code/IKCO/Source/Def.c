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
#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "J1939.h" 
#include "Def.h" 
#include "TellTales.h"
#include "r_cg_userdefine.h"
#include "TML_14229_UDSH.h"




// Constant declarations -----------------------------------------------------

#define WARNING_ON_TIME				1000	//ms
#define WARNING_OFF_TIME			3000	//ms
#define DEF_AVG_SIZE				8

								//J1939 data
#define DEF_ON_BAR1					0x29		
#define DEF_ON_BAR2					0x53
#define DEF_ON_BAR3					0x7D
#define DEF_ON_BAR4					0xA6
#define DEF_ON_BAR5					0xD0
#define DEF_ON_BAR6					0xED
#define DEF_HYS						0x5   //2 Level % Hys 


#define DEF_OFF_BAR1				DEF_ON_BAR1 - DEF_HYS
#define DEF_OFF_BAR2				DEF_ON_BAR2 - DEF_HYS
#define DEF_OFF_BAR3				DEF_ON_BAR3 - DEF_HYS
#define DEF_OFF_BAR4				DEF_ON_BAR4 - DEF_HYS
#define DEF_OFF_BAR5				DEF_ON_BAR5 - DEF_HYS
#define DEF_OFF_BAR6				DEF_ON_BAR6 - DEF_HYS


// local declarations --------------------------------------------------------

typedef struct
{	
	u16     u16DefSignalData[DEF_AVG_SIZE];      	 
	u8      u8DefLevel;      			 
	u8      u8PreviousDefLevel;      			 	
	u8      u8DefBarNo;
	bool    bSymboleStatus;
} DEF_DATA_TYPE;

DEF_DATA_TYPE sDEF = {0x00,0x00,0x00,0x00,0x00};


/*********************************************************************//**
 *
 * Check def CAN msg and CAN lost msg ,According to def level calculate bars.
 *
 *
 * @param      None 
 *
 * @return     None 
 *************************************************************************/
void vDefGauge(void)
{	
	u32 data;
	u8 i = 0;
	static u8 u8AvgSampleCount = 0;	

		//When control DEF gauge by IO control service then not need to execute this function
	if(sTestEquimentControl.IoControlFlag.bits.FrontApgEnableFlag ==  TRUE)
	{
		/*Change Previous Signal to out of range value, so that previous & Current value of BAR do not match when resume in normal mode 
		   and level is recalculated even without change in input*/
		sDEF.u8PreviousDefLevel = 0xFFU; 
		//set true so that DEF symbole display during DEF gauge checking by IO control service 
		sDEF.bSymboleStatus = TRUE;
		return;
	}
	
	//Take CAN data
	data = u32GetPGNdata(eJLowDEFLamp);
	//Check Timer stamps for CAN msg is coming or Not
	if(bGetPGNTimeOut(eJLowDEFLamp) || (u16)data >= BADDATA )	
	{
		data = 0;
		
		if( bGetLampBlinkingFlagstatus()) //Get indicator On/OFF flag status 
		{			 
			sDEF.bSymboleStatus = TRUE;
		}
		else
		{
			sDEF.bSymboleStatus = FALSE;
		}
	}
	else
	{
		sDEF.bSymboleStatus = TRUE;	
		//Go back if new sample is not coming
		if(! bGetPGNDataReady(eJLowDEFLamp) )
			return;	
		ClearPGNDataReady(eJLowDEFLamp);
	}
	//Averging of CAN data

	if(eGetClusterStatus() == eClusterIgnitionReset)
	{
		for(i = 0; i < DEF_AVG_SIZE; i++)
			sDEF.u16DefSignalData[i]  = (u16)data;	
		u8AvgSampleCount = DEF_AVG_SIZE -1;
	}
	
	sDEF.u16DefSignalData[u8AvgSampleCount] = (u16)data;
	if(++u8AvgSampleCount >= DEF_AVG_SIZE)
	{
		data = 0;
		for(i = 0; i < DEF_AVG_SIZE; i++)
		data = data + sDEF.u16DefSignalData[i];
		data = data / DEF_AVG_SIZE;	 
		for(i = 0; i < DEF_AVG_SIZE - 1; i++)
		sDEF.u16DefSignalData[i]=  sDEF.u16DefSignalData[i+1];
		u8AvgSampleCount = DEF_AVG_SIZE - 1;
		
		 sDEF.u8DefLevel = (u8)data;
	}


	
		if(sDEF.u8PreviousDefLevel != sDEF.u8DefLevel)
		{
			sDEF.u8PreviousDefLevel = sDEF.u8DefLevel;
			//Ascending inputs
			if(sDEF.u8DefBarNo ==0 && sDEF.u8PreviousDefLevel < DEF_ON_BAR1)
			sDEF.u8DefBarNo =0;
			else if(sDEF.u8DefBarNo <=1 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR1 && sDEF.u8PreviousDefLevel < DEF_ON_BAR2 )
			sDEF.u8DefBarNo =1;
			else if(sDEF.u8DefBarNo <=2 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR2 && sDEF.u8PreviousDefLevel < DEF_ON_BAR3 )
			sDEF.u8DefBarNo =2;
			else if(sDEF.u8DefBarNo <=3 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR3 && sDEF.u8PreviousDefLevel < DEF_ON_BAR4 )
			sDEF.u8DefBarNo =3;
			else if(sDEF.u8DefBarNo <=4 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR4 && sDEF.u8PreviousDefLevel < DEF_ON_BAR5 )
			sDEF.u8DefBarNo =4;
			else if(sDEF.u8DefBarNo <=5 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR5 && sDEF.u8PreviousDefLevel < DEF_ON_BAR6 )
			sDEF.u8DefBarNo =5;
			else if(sDEF.u8DefBarNo <=6 && sDEF.u8PreviousDefLevel >= DEF_ON_BAR6 )
			sDEF.u8DefBarNo =6;

			//Descending inputs
			else if(sDEF.u8DefBarNo > 0 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR1)
			sDEF.u8DefBarNo = 0;
			else if(sDEF.u8DefBarNo > 1 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR2)
			sDEF.u8DefBarNo = 1;
			else if(sDEF.u8DefBarNo > 2 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR3)
			sDEF.u8DefBarNo = 2;
			else if(sDEF.u8DefBarNo > 3 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR4)
			sDEF.u8DefBarNo = 3;
			else if(sDEF.u8DefBarNo > 4 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR5)
			sDEF.u8DefBarNo = 4;
			else if(sDEF.u8DefBarNo > 5 && sDEF.u8PreviousDefLevel <= DEF_OFF_BAR6)
			sDEF.u8DefBarNo = 5;
		}

	

}

/*********************************************************************//**
*
* Def bar current status
*
*
* @param      Def bar number 
*
* @return     None 
*************************************************************************/
bool u8DefBarNo(void)
{
	return sDEF.u8DefBarNo;
}

/*********************************************************************//**
*
* Set Def bar current status; used for IO control service
*
*
* @param      Def bar number 
*
* @return     None 
*************************************************************************/
void vSetDefBarNo(u8 BarNo)
{
	//LCD have a limit to display up to 6 Bar,So DEF bar no. should not more than 6 
	u8 NumOfBar = 0;
	if(BarNo > 6U)
	{
		NumOfBar = 6U;
	}
	else
	{
		NumOfBar = BarNo;
	}
	sDEF.u8DefBarNo = NumOfBar;
}


/*********************************************************************//**
*
* Get Def Symbole status
*
*
* @param	None      
*
* @return	Def Symbole status
*************************************************************************/
bool bGetDefSymbolStatus()
{
	return sDEF.bSymboleStatus;
}


