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
* File Name    : r_cg_timer.c
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for TAU module.
* Creation Date: 05/08/2019
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
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU1_ch6 pulse measurement */
extern volatile uint32_t g_tau1_ch6_width;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TAU0_Create
* Description  : This function initializes the TAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Create(void)
{
    TAU0EN = 1U;        /* supply input clock */
    TPS0 = _0000_TAU_CKM0_FCLK_0 | _0000_TAU_CKM1_FCLK_0 | _0800_TAU_CKM2_FCLK_8 | _E000_TAU_CKM3_FCLK_14;
    /* Stop all channels */
    TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON |
          _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON |
          _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON;
    /* Mask channel 0 interrupt */
    TMMK00 = 1U;        /* disable INTTM00 interrupt */
    TMIF00 = 0U;        /* clear INTTM00 interrupt flag */
    /* Mask channel 2 interrupt */
    TMMK02 = 1U;        /* disable INTTM02 interrupt */
    TMIF02 = 0U;        /* clear INTTM02 interrupt flag */
    /* Mask channel 1 interrupt */
    TMMK01 = 1U;        /* disable INTTM01 interrupt */
    TMIF01 = 0U;        /* clear INTTM01 interrupt flag */
    /* Mask channel 3 interrupt */
    TMMK03 = 1U;        /* disable INTTM03 interrupt */
    TMIF03 = 0U;        /* clear INTTM03 interrupt flag */
    /* Mask channel 4 interrupt */
    TMMK04 = 1U;        /* disable INTTM04 interrupt */
    TMIF04 = 0U;        /* clear INTTM04 interrupt flag */
    /* Mask channel 5 interrupt */
    TMMK05 = 1U;        /* disable INTTM05 interrupt */
    TMIF05 = 0U;        /* clear INTTM05 interrupt flag */
    /* Set INTTM01 low priority */
    TMPR101 = 1U;
    TMPR001 = 1U;
    /* Set INTTM03 low priority */
    TMPR103 = 1U;
    TMPR003 = 1U;
    /* Set INTTM04 low priority */
    TMPR104 = 1U;
    TMPR004 = 1U;
    /* Set INTTM05 low priority */
    TMPR105 = 1U;
    TMPR005 = 1U;
    /* Channel 0 is used as master channel for PWM output function */
    TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0800_TAU_COMBINATION_MASTER |
            _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
    TDR00 = _009F_TAU_TDR00_VALUE;
    TOM0 &= ~_0001_TAU_CH0_OUTPUT_COMBIN;
    TOL0 &= ~_0001_TAU_CH0_OUTPUT_LEVEL_L;
    TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
    /* Channel 2 is used as slave channel for PWM output function */
    TMR02 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE |
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR02 = _0050_TAU_TDR02_VALUE;
    TOM0 |= _0004_TAU_CH2_OUTPUT_COMBIN;
    TOL0 &= ~_0004_TAU_CH2_OUTPUT_LEVEL_L;
    TO0 &= ~_0004_TAU_CH2_OUTPUT_VALUE_1;
    TOE0 |= _0004_TAU_CH2_OUTPUT_ENABLE;
    /* Channel 1 used as interval timer */
    TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_MODE_INTERVAL_TIMER |
            _0001_TAU_START_INT_USED;
    TDR01 = _063F_TAU_TDR01_VALUE;
    TOM0 &= ~_0002_TAU_CH1_OUTPUT_COMBIN;
    TOL0 &= ~_0002_TAU_CH1_OUTPUT_LEVEL_L;
    TOE0 &= ~_0002_TAU_CH1_OUTPUT_ENABLE;
    /* Channel 3 used as interval timer */
    TMR03 = _C000_TAU_CLOCK_SELECT_CKM3 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_MODE_INTERVAL_TIMER |
            _0001_TAU_START_INT_USED;
    TDR03 = _E4E0_TAU_TDR03_VALUE;
    TOM0 &= ~_0008_TAU_CH3_OUTPUT_COMBIN;
    TOL0 &= ~_0008_TAU_CH3_OUTPUT_LEVEL_L;
    TOE0 &= ~_0008_TAU_CH3_OUTPUT_ENABLE;
    /* Channel 4 used as interval timer */
    TMR04 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_MODE_INTERVAL_TIMER |
            _0001_TAU_START_INT_USED;
    TDR04 = _3E7F_TAU_TDR04_VALUE;
    TOM0 &= ~_0010_TAU_CH4_OUTPUT_COMBIN;
    TOL0 &= ~_0010_TAU_CH4_OUTPUT_LEVEL_L;
    TOE0 &= ~_0010_TAU_CH4_OUTPUT_ENABLE;
    /* Channel 5 used as interval timer */
    TMR05 = _8000_TAU_CLOCK_SELECT_CKM2 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_MODE_INTERVAL_TIMER |
            _0001_TAU_START_INT_USED;
    TDR05 = _F423_TAU_TDR05_VALUE;
    TOM0 &= ~_0020_TAU_CH5_OUTPUT_COMBIN;
    TOL0 &= ~_0020_TAU_CH5_OUTPUT_LEVEL_L;
    TOE0 &= ~_0020_TAU_CH5_OUTPUT_ENABLE;
    /* Set noise filter sampling clock divisor and channels selected*/
    TNFSMP0 = _00_TAU0_NOISE_DIVISOR;
    TNFCS0 = _00_TAU0_NOISE_CHANNEL_SELECT;
    /* Set TO02 pin */
    TOS00 |= 0x10U;
    P5 &= 0xFEU;
    PM5 &= 0xFEU;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Start
* Description  : This function starts TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Start(void)
{
    TOE0 |= _0004_TAU_CH2_OUTPUT_ENABLE;
    TS0 |= _0001_TAU_CH0_START_TRG_ON | _0004_TAU_CH2_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Stop
* Description  : This function stops TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Stop(void)
{
    TT0 |= _0001_TAU_CH0_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON;
    TOE0 &= ~_0004_TAU_CH2_OUTPUT_ENABLE;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Start
* Description  : This function starts TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Start(void)
{
    TMIF01 = 0U;        /* clear INTTM01 interrupt flag */
    TMMK01 = 0U;        /* enable INTTM01 interrupt */
    TS0 |= _0002_TAU_CH1_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Stop
* Description  : This function stops TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Stop(void)
{
    TT0 |= _0002_TAU_CH1_STOP_TRG_ON;
    TMMK01 = 1U;        /* disable INTTM01 interrupt */
    TMIF01 = 0U;        /* clear INTTM01 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Start
* Description  : This function starts TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Start(void)
{
    TMIF03 = 0U;        /* clear INTTM03 interrupt flag */
    TMMK03 = 0U;        /* enable INTTM03 interrupt */
    TS0 |= _0008_TAU_CH3_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Stop
* Description  : This function stops TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Stop(void)
{
    TT0 |= _0008_TAU_CH3_STOP_TRG_ON;
    TMMK03 = 1U;        /* disable INTTM03 interrupt */
    TMIF03 = 0U;        /* clear INTTM03 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel4_Start
* Description  : This function starts TAU0 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel4_Start(void)
{
    TMIF04 = 0U;        /* clear INTTM04 interrupt flag */
    TMMK04 = 0U;        /* enable INTTM04 interrupt */
    TS0 |= _0010_TAU_CH4_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel4_Stop
* Description  : This function stops TAU0 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel4_Stop(void)
{
    TT0 |= _0010_TAU_CH4_STOP_TRG_ON;
    TMMK04 = 1U;        /* disable INTTM04 interrupt */
    TMIF04 = 0U;        /* clear INTTM04 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel5_Start
* Description  : This function starts TAU0 channel 5 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel5_Start(void)
{
    TMIF05 = 0U;        /* clear INTTM05 interrupt flag */
    TMMK05 = 0U;        /* enable INTTM05 interrupt */
    TS0 |= _0020_TAU_CH5_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel5_Stop
* Description  : This function stops TAU0 channel 5 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel5_Stop(void)
{
    TT0 |= _0020_TAU_CH5_STOP_TRG_ON;
    TMMK05 = 1U;        /* disable INTTM05 interrupt */
    TMIF05 = 0U;        /* clear INTTM05 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Create
* Description  : This function initializes the TAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Create(void)
{
    TAU1EN = 1U;        /* supply input clock */
    TPS1 = _0000_TAU_CKM0_FCLK_0 | _0060_TAU_CKM1_FCLK_6 | _0000_TAU_CKM2_FCLK_0 | _0000_TAU_CKM3_FCLK_0;
    /* Stop all channels */
    TT1 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON |
          _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON |
          _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON;
    /* Mask channel 0 interrupt */
    TMMK10 = 1U;        /* disable INTTM10 interrupt */
    TMIF10 = 0U;        /* clear INTTM10 interrupt flag */
    /* Mask channel 2 interrupt */
    TMMK12 = 1U;        /* disable INTTM12 interrupt */
    TMIF12 = 0U;        /* clear INTTM12 interrupt flag */
    /* Mask channel 4 interrupt */
    TMMK14 = 1U;        /* disable INTTM14 interrupt */
    TMIF14 = 0U;        /* clear INTTM14 interrupt flag */
    /* Mask channel 6 interrupt */
    TMMK16 = 1U;        /* disable INTTM16 interrupt */
    TMIF16 = 0U;        /* clear INTTM16 interrupt flag */
    /* Set INTTM16 level 1 priority */
    TMPR116 = 0U;
    TMPR016 = 1U;
    /* Channel 0 is used as master channel for PWM output function */
    TMR10 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0800_TAU_COMBINATION_MASTER |
            _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
    TDR10 = _01FD_TAU_TDR10_VALUE;
    TOM1 &= ~_0001_TAU_CH0_OUTPUT_COMBIN;
    TOL1 &= ~_0001_TAU_CH0_OUTPUT_LEVEL_L;
    TOE1 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
    /* Channel 2 is used as slave channel for PWM output function */
    TMR12 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE |
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR12 = _00FF_TAU_TDR12_VALUE;
    TOM1 |= _0004_TAU_CH2_OUTPUT_COMBIN;
    TOL1 &= ~_0004_TAU_CH2_OUTPUT_LEVEL_L;
    TO1 &= ~_0004_TAU_CH2_OUTPUT_VALUE_1;
    TOE1 |= _0004_TAU_CH2_OUTPUT_ENABLE;
    /* Channel 4 is used as slave channel for PWM output function */
    TMR14 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE |
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR14 = _00FF_TAU_TDR14_VALUE;
    TOM1 |= _0010_TAU_CH4_OUTPUT_COMBIN;
    TOL1 &= ~_0010_TAU_CH4_OUTPUT_LEVEL_L;
    TO1 &= ~_0010_TAU_CH4_OUTPUT_VALUE_1;
    TOE1 |= _0010_TAU_CH4_OUTPUT_ENABLE;
    /* Channel 6 is used to measure input pulse interval */
    TMR16 = _4000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0100_TAU_TRIGGER_TIMN_VALID |
            _0000_TAU_TIMN_EDGE_FALLING | _0004_TAU_MODE_CAPTURE | _0000_TAU_START_INT_UNUSED;
    TOM1 &= ~_0040_TAU_CH6_OUTPUT_COMBIN;
    TOL1 &= ~_0040_TAU_CH6_OUTPUT_LEVEL_L;
    TOE1 &= ~_0040_TAU_CH6_OUTPUT_ENABLE;
    TNFEN1 |= _40_TAU_CH6_NOISE_ON;
    /* Set noise filter sampling clock divisor and channels selected*/
    TNFSMP1 = _06_TAU1_NOISE_DIVISOR;
    TNFCS1 = _00_TAU1_NOISE_CHANNEL_SELECT;
    /* Set TO12 pin */
    TOS10 |= 0x10U;
    P0 &= 0xFBU;
    PM0 &= 0xFBU;
    /* Set TO14 pin */
    TOS11 |= 0x01U;
    P0 &= 0xEFU;
    PM0 &= 0xEFU;
    /* Set TI16 pin */
    TIS11 &= 0xCFU;  
    PM1 |= 0x40U;
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Channel0_Start
* Description  : This function starts TAU1 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Channel0_Start(void)
{
    TOE1 |= _0004_TAU_CH2_OUTPUT_ENABLE | _0010_TAU_CH4_OUTPUT_ENABLE;
    TS1 |= _0001_TAU_CH0_START_TRG_ON | _0004_TAU_CH2_START_TRG_ON | _0010_TAU_CH4_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Channel0_Stop
* Description  : This function stops TAU1 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Channel0_Stop(void)
{
    TT1 |= _0001_TAU_CH0_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON;
    TOE1 &= ~_0004_TAU_CH2_OUTPUT_ENABLE & ~_0010_TAU_CH4_OUTPUT_ENABLE;
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Channel6_Start
* Description  : This function starts TAU1 channel 6 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Channel6_Start(void)
{
    TMIF16 = 0U;        /* clear INTTM16 interrupt flag */
    TMMK16 = 0U;        /* enable INTTM16 interrupt */
    TS1 |= _0040_TAU_CH6_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Channel6_Stop
* Description  : This function stops TAU1 channel 6 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Channel6_Stop(void)
{
    TT1 |= _0040_TAU_CH6_STOP_TRG_ON;
    TMMK16 = 1U;        /* disable INTTM16 interrupt */
    TMIF16 = 0U;        /* clear INTTM16 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_TAU1_Channel6_Get_PulseWidth
* Description  : This function measures TAU1 channel 6 input pulse width.
* Arguments    : width -
*                    the address where to write the input pulse width
* Return Value : None
***********************************************************************************************************************/
void R_TAU1_Channel6_Get_PulseWidth(uint32_t * const width)
{
    /* For TAU_ch6 pulse measurement */
    *width = g_tau1_ch6_width;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
