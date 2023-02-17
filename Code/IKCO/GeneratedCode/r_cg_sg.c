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
* File Name    : r_cg_sg.c
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for SG module.
* Creation Date: 19/03/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sg.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SG_Create
* Description  : This function initializes the sound generator module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SG_Create(void)
{
    SGEN = 1U;                             /* supply sound generator clock */
    SG0PWM = 0x0000U;                      /* clear SG0PWM register */ 
    SG0CTL = _00_SG_OPERATION_DISABLE;     /* disable sound generator operation */
    SGMK = 1U;                             /* disable INTSG interrupt */
    SGIF = 0U;                             /* clear INTSG interrupt flag */
    PCKSEL |= _01_SG_fCLK_HALF;
    SG0CTL = _10_SG_OPERATION_ENABLE | _00_SG_OUT_FREQ | _00_SG_ALD_DISABLE;
    SG0FL = _004F_SG0FL_VALUE;
    SG0FH = _0017_SG0FH_VALUE;
    SGSEL |= _00_SG_PIN_P73_P72;
    /* Set SG output pin */
    P7 &= 0xF3U;
    PM7 &= 0xF3U;
}

/***********************************************************************************************************************
* Function Name: R_SG_Start
* Description  : This function starts sound generator module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SG_Start(void)
{
    SG0PWM = _0048_SG0PWM_VALUE;  /* start sound generator */
}

/***********************************************************************************************************************
* Function Name: R_SG_Stop
* Description  : This function stops sound generator module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SG_Stop(void)
{
    SG0PWM = 0x0000U;  /* stop sound generator */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
