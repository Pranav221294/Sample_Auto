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
/*
 * iicDriver.c
 *
 *  Created on: Oct 30, 2018
 *      Author: Rajni Rajpoot
 *      
 *      
 */

/*****************************************************************************
 **  Module: IICDriver
 **
 ** ---------------------------------------------------------------------------
 **  Description:
 **      This Module is designed for Multi Slave I2CDriver.
 **     
 **
 *****************************************************************************/



#include "customTypedef.h"
#include "iicDriver.h"
#include "r_cg_serial.h"
#include "r_cg_wdt.h"

// constant declarations -----------------------------------------------------



// local declarations --------------------------------------------------------

static u8 u8PushPtr, u8PopPtr;
static eI2CState u8i2cState; 							// used to change state of bus

I2CComControl sI2CControl[I2C_MAX_BUFFER_SIZE]; // I2c communication control structure
static bool bReadFromSlave = FALSE;

// Global declarations --------------------------------------------------------

I2CComControl gsI2CReadData;
I2CComControl gsCopyI2CBufData;
bool bIIcErroFlag = 0;


/*********************************************************************//**
 *
 * Initialize I2C buffer and set I2c status Idle
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vI2CBufferInit(void)
{
	
	u8 i =0;
	u8PushPtr = 0;
	u8PopPtr =0;
	u8i2cState = eI2CIdle;
	
	for(i = 0;i < I2C_MAX_BUFFER_SIZE; i++ )
		sI2CControl[i].bDataReady = 0x00;
}


/*********************************************************************//**
 *
 * Copy I2c Data to I2C Buffer.
 * this function has to be called after copying data into gsCopyI2CBufData
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/

void vPushI2CData(void)
{
	u8 i;

	//if( ++u8PushPtr >= I2C_MAX_BUFFER_SIZE )
		//u8PushPtr = 0;
	//Check I2C buffer empty or not 
   for (u8PushPtr = 0;
	   (u8PushPtr < I2C_MAX_BUFFER_SIZE) && (sI2CControl[u8PushPtr].bDataReady == TRUE);
	   ++u8PushPtr)
	   ; // empty statement for this for loop
   if(u8PushPtr < I2C_MAX_BUFFER_SIZE) 
   	{
	   if(sI2CControl[u8PushPtr].bDataReady == FALSE)
	   	{	
			sI2CControl[u8PushPtr].u8BlockSize = gsCopyI2CBufData.u8BlockSize;
			sI2CControl[u8PushPtr].u8SlaveAddress = gsCopyI2CBufData.u8SlaveAddress;
			sI2CControl[u8PushPtr].bDataReady = gsCopyI2CBufData.bDataReady ;
			for ( i = 0; i < gsCopyI2CBufData.u8BlockSize && i < I2C_MAX_BLOCK_SIZE ; i++)
				sI2CControl[u8PushPtr].u8Buffer[i] =  gsCopyI2CBufData.u8Buffer[i];
	   	}
   	}
}

/*********************************************************************//**
 *
 * get the state of i2c bus.
 *
 * @param      None    
 *
 * @return     I2c bus status
 *************************************************************************/

eI2CState eGetI2CBusState(void)
{
	return u8i2cState;
}


/*********************************************************************//**
 *
 * Set the state of i2c bus.
 *
 * @param     I2c Bus state
 *
 * @return     None
 *************************************************************************/

void vSetI2CBusState(eI2CState state)
{
	u8i2cState = state;
}


/*********************************************************************//**
 *
 * (Function not used )get the dataread status 
 *
 * @param     None
 *
 * @return     None
 *************************************************************************/

bool	bIsReadFromSlave(void)
{
	return bReadFromSlave ;
}


/*********************************************************************//**
 *
 * Send data on I2C bus if previous data sent successfully 
 *
 * @param     None
 *
 * @return     None
 *************************************************************************/

void vSendDataToI2C()
{	
	u8 IicdataSendCount =0;
	
	if(u8i2cState != eI2CIdle ) 
		return;

	if(++u8PopPtr >= I2C_MAX_BUFFER_SIZE )
		u8PopPtr = 0;


	if(sI2CControl[u8PopPtr].bDataReady == TRUE)
	{

		u8i2cState = eI2CTxReady;		
		R_IIC11_Master_Send(sI2CControl[u8PopPtr].u8SlaveAddress,sI2CControl[u8PopPtr].u8Buffer,sI2CControl[u8PopPtr].u8BlockSize);		
		sI2CControl[u8PopPtr].bDataReady = FALSE;
		//If during data sending I2C bus generate error then try to send same data 5 times 
		while( eI2CTxReady == eGetI2CBusState() && IicdataSendCount < 5 )
		{
			R_WDT_Restart();
			if(bIIcErroFlag == TRUE)
			{
				bIIcErroFlag = FALSE;
				R_IIC11_Master_Send(sI2CControl[u8PopPtr].u8SlaveAddress,sI2CControl[u8PopPtr].u8Buffer,sI2CControl[u8PopPtr].u8BlockSize);		
				++IicdataSendCount;
			}

		}
		//If I2c bus error not removed then set I2C bus status idle
		if(IicdataSendCount > 4)
			u8i2cState = eI2CIdle;
	}


}


/*********************************************************************//**
 * Not using this function
 * select slave and send register address to slave from which Data is received.
 * Received Block Size.	
 * blocksize and slave address has to be set in  sI2CReadData
 * before calling this function. this function has to be called more than 
 * once to get the read data in gsI2CReadData.u8Buffer.
 *
 * @param   Slave Address
 *			Resister address where from data to read
 *			No of byte to be read
 *
 * @return     None
 *************************************************************************/
void vReadFromI2C(u8 slaveAddr, u8 startRegAddr, u8 blockSize)
{
	bReadFromSlave = TRUE;

	gsI2CReadData.u8SlaveAddress = slaveAddr ; 
	gsI2CReadData.u8StartRegAddress = startRegAddr ;
	gsI2CReadData.u8BlockSize = blockSize;
	if(u8i2cState == eI2CIdle ||  u8i2cState == eI2CRead )
	{
		bReadFromSlave = FALSE;
		u8i2cState = eI2CAddressSent;
		//LedDriver_SelectSlave(gsI2CReadData.u8SlaveAddress);
		//LedDriver_SendChar(gsI2CReadData.u8StartRegAddress);
	}
	else if(u8i2cState == eI2CReadDataTransmit )
	;
		//LedDriver_RecvBlock(gsI2CReadData.u8Buffer, gsI2CReadData.u8BlockSize, &size);
}





