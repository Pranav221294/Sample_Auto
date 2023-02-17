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
 * iicDriver.h
 *
 *  Created on: Oct 30, 2018
 *      Author: Rajni Rajpoot
 */

#ifndef IICDRIVER_H_
#define IICDRIVER_H_
#include "r_cg_macrodriver.h"
#include "customTypedef.h"

// constant declarations -----------------------------------------------------

#define I2C_MAX_BLOCK_SIZE		45

#define I2C_MAX_BUFFER_SIZE		10


typedef enum
{    
	eI2CIdle  = 0 ,				//set state when transmission finished and no reading is required also when reading is over
	eI2CTxReady,			  	//set state when data is ready to transmit
	eI2CRead,					//set state if breadFromSlave is true (in Event)	
	eI2CAddressSent,        	//set state when read address has send    
	eI2CReadDataTransmit,   	//set state when transmission is successful of read  address
	eI2CError,
} eI2CState;

// Global declarations --------------------------------------------------------

typedef struct
{
 	u8		u8Buffer[I2C_MAX_BLOCK_SIZE];			//buffer for send data to i2c
	u8		u8BlockSize;							//size of data transmitted to i2c
	u8		u8SlaveAddress;							//I2c slave Address
	u8		u8StartRegAddress;						//Register address from which data is to be read
	bool	bDataReady;
}I2CComControl;

extern I2CComControl gsCopyI2CBufData;
extern I2CComControl gsI2CReadData;
extern bool bIIcErroFlag;
extern I2CComControl sI2CControl[I2C_MAX_BUFFER_SIZE]; // I2c communication control structure


// Global prototypes --------------------------------------------------------

/// @addtogroup I2C_LIBRARY
/// @{
 
/// @defgroup I2cDriverLib Global functions used in iicdriver file
//// 
///   
/// @addtogroup I2cDriverLib
/// @{


void vI2CBufferInit(void);
void vPushI2CData(void);
eI2CState eGetI2CBusState(void);
void vSetI2CBusState(eI2CState state);
bool bIsReadFromSlave(void);
void vSendDataToI2C(void);
void vReadFromI2C(u8 slaveAddr, u8 startRegAddr, u8 blockSize);


/// @} // endgroup I2cDriverLib

/// @} // endgroup I2C_LIBRARY


#endif /* IICDRIVER_H_ */
