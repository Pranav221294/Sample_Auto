/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_sg.h
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for SG module.
* Creation Date: 19/03/2019
***********************************************************************************************************************/

#ifndef SG_H
#define SG_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Peripheral enable register 1 (PER1)
*/
/* Control of sound generator input clock (SGEN) */
#define _00_SG_CLOCK_STOP           (0x00U)    /* stop supply of input clock */
#define _10_SG_CLOCK_SUPPLY         (0x10U)    /* supply input clock */
/* 
    Peripheral clock select register (PCKSEL)
*/
/* SG clock source selection (SGCLKSEL) */
#define _00_SG_fCLK                 (0x00U)    /* fCLK is supplied */
#define _01_SG_fCLK_HALF            (0x01U)    /* fCLK/2 is supplied */
/* 
    Sound generator control register (SG0CTL)
*/
/* Sound generator operation enable/disable specification (SG0PWR) */
#define _00_SG_OPERATION_DISABLE    (0x00U)    /* disable sound generator operation */
#define _10_SG_OPERATION_ENABLE     (0x10U)    /* enable sound generator operation */
/* SG0 output mode selection (SG0OS) */
#define _00_SG_OUT_FREQ             (0x00U)    /* selects frequency output at SGO/SGOF */
#define _02_SG_OUT_FREQ_AMP         (0x02U)    /* selects frequency and amplitude mixed output at SGO/SGOF */
/* Automatic logarithmic decrement of amplitude (SG0ALDS) */
#define _00_SG_ALD_DISABLE          (0x00U)    /* automatic logarithmic decrement deactivated */
#define _01_SG_ALD_ENABLE           (0x01U)    /* automatic logarithmic decrement activated */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* 
    Frequency register SG0FL (SG0FL[8:0])
*/
#define _004F_SG0FL_VALUE           (0x004FU)
/* 
    Frequency register SG0FH (SG0FH[8:0])
*/
#define _0017_SG0FH_VALUE           (0x0017U)
/* 
    Amplitude register (SG0PWM)
*/
#define _0048_SG0PWM_VALUE          (0x0048U)
/* 
    Sound generator and PCL pin select register (SGSEL)
*/
#define _00_SG_PIN_P73_P72          (0x00U)    /* SGO/SGOF P73, SGOA P72 are assigned */
#define _01_SG_PIN_P93_P92          (0x01U)    /* SGO/SGOF P93, SGOA P92 are assigned */
#define _02_SG_PIN_P135_P134        (0x02U)    /* SGO/SGOF P135, SGOA P134 are assigned */
#define _04_SG_PIN_P73              (0x04U)    /* SGO/SGOF P73 is assigned */
#define _05_SG_PIN_P93              (0x05U)    /* SGO/SGOF P93 is assigned */
#define _06_SG_PIN_P135             (0x06U)    /* SGO/SGOF P135 is assigned */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_SG_Create(void);
void R_SG_Start(void);
void R_SG_Stop(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
