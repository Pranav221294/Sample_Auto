/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
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
* Copyright (C) 2012, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_port.c
* Version      : Applilet3 for RL78/D1A V2.04.02.01 [21 Mar 2018]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for PORT module.
* Creation Date: 24/01/2023
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
#include "r_cg_port.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    P1 = _02_Pn1_OUTPUT_1 | _00_Pn2_OUTPUT_0;
    P2 = _00_Pn7_OUTPUT_0;
    P3 = _00_Pn0_OUTPUT_0;
    P5 = _00_Pn7_OUTPUT_0;
    P7 = _00_Pn1_OUTPUT_0;
    P8 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0 | _00_Pn4_OUTPUT_0 |
         _00_Pn5_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0;
    P9 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0 | _00_Pn4_OUTPUT_0 |
         _00_Pn5_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0;
    P13 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn6_OUTPUT_0;
    P14 = _00_Pn0_OUTPUT_0;
    P15 = _00_Pn0_OUTPUT_0;
    POM = _01_POM0_NCH_ON | _02_POM1_NCH_ON;
    ADPC = _07_ADPC_DI_ON;
    PM0 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_MODE_INPUT | _10_PMn4_NOT_USE |
          _20_PMn5_MODE_INPUT | _40_PMn6_MODE_INPUT | _80_PMn7_MODE_INPUT;
    PM1 = _01_PMn0_NOT_USE | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _08_PMn3_MODE_INPUT | _10_PMn4_MODE_INPUT |
          _20_PMn5_MODE_INPUT | _40_PMn6_NOT_USE | _80_PMn7_MODE_INPUT;
    PM2 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE |
          _20_PMn5_NOT_USE | _40_PMn6_MODE_INPUT | _00_PMn7_MODE_OUTPUT;
    PM3 = _00_PMn0_MODE_OUTPUT | _02_PMn1_MODE_INPUT | _04_PMn2_MODE_INPUT | _08_PMn3_NOT_USE | _10_PMn4_MODE_INPUT |
          _20_PMn5_MODE_INPUT | _40_PMn6_MODE_INPUT | _80_PMn7_MODE_INPUT;
    PM5 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_MODE_INPUT |
          _20_PMn5_MODE_INPUT | _40_PMn6_MODE_INPUT | _00_PMn7_MODE_OUTPUT;
    PM6 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_MODE_INPUT | _08_PMn3_MODE_INPUT | _10_PMn4_MODE_INPUT |
          _20_PMn5_MODE_INPUT | _40_PMn6_MODE_INPUT | _80_PM6_DEFAULT;
    PM7 = _01_PMn0_MODE_INPUT | _00_PMn1_MODE_OUTPUT | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_MODE_INPUT |
          _20_PMn5_MODE_INPUT | _C0_PM7_DEFAULT;
    PM8 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT |
          _00_PMn4_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT;
    PM9 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT |
          _00_PMn4_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT;
    PM13 = _00_PMn1_MODE_OUTPUT | _04_PMn2_MODE_INPUT | _08_PMn3_MODE_INPUT | _10_PMn4_MODE_INPUT |
           _20_PMn5_MODE_INPUT | _00_PMn6_MODE_OUTPUT | _80_PM13_DEFAULT;
    PM14 = _00_PMn0_MODE_OUTPUT | _FE_PM14_DEFAULT;
    PM15 = _00_PMn0_MODE_OUTPUT | _FE_PM15_DEFAULT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
