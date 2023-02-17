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
* File Name    : r_cg_serial.c
* Version      : Applilet3 for RL78/D1A V2.03.04.01 [11 Nov 2016]
* Device(s)    : R5F10DPG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for Serial module.
* Creation Date: 19/06/2019
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
#include "r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t  * gp_csi10_rx_address;        /* csi10 receive buffer address */
volatile uint16_t   g_csi10_rx_length;          /* csi10 receive data length */
volatile uint16_t   g_csi10_rx_count;           /* csi10 receive data count */
volatile uint8_t  * gp_csi10_tx_address;        /* csi10 send buffer address */
volatile uint16_t   g_csi10_tx_length;          /* csi10 send data length */
volatile uint16_t   g_csi10_tx_count;           /* csi10 send data count */
volatile uint8_t    g_iic11_master_status_flag; /* iic11 start flag for send address check by master mode */
volatile uint8_t  * gp_iic11_tx_address;        /* iic11 send data pointer by master mode */
volatile uint16_t   g_iic11_tx_count;           /* iic11 send data size by master mode */
volatile uint8_t  * gp_iic11_rx_address;        /* iic11 receive data pointer by master mode */
volatile uint16_t   g_iic11_rx_count;           /* iic11 receive data size by master mode */
volatile uint16_t   g_iic11_rx_length;          /* iic11 receive data length by master mode */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes SAU1.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
    SAU1EN = 1U;    /* supply SAU1 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS1 = _0000_SAU_CK00_FCLK_0 | _0020_SAU_CK01_FCLK_2;
    
    R_CSI10_Create();
    R_IIC11_Create();
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Create
* Description  : This function initializes CSI10.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Create(void)
{
    ST1 |= _0001_SAU_CH0_STOP_TRG_ON;     /* disable CSI10 */
    CSIMK10 = 1U;    /* disable INTCSI10 interrupt */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
    /* Set INTCSI10 low priority */
    CSIPR110 = 1U;
    CSIPR010 = 1U;
    SIR10 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _8000_SAU_CLOCK_SELECT_CK01 | _0000_SAU_CLOCK_MODE_CKS |
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_CSI | _0000_SAU_TRANSFER_END;
    SCR10 = _8000_SAU_TRANSMISSION | _3000_SAU_TIMING_4 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
    SDR10 = _CE00_CSI10_DIVISOR;
    SO1 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1; /* output CSI10 clock value 0 */
    SO1 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;  /* output CSI10 data value 0 */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;  /* enable CSI10 output */
    /* use P51, P52, P53 as SCK01, SI01, SO01 */
    STSEL0 |= _40_CSI10_PIN_SELECTION_2;
    /* Set SO10 pin */
    P5 |= 0x08U;
    PM5 &= 0xF7U;
    /* Set SCK10 pin */
    P5 |= 0x02U;
    PM5 &= 0xFDU;
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Start
* Description  : This function starts CSI10.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Start(void)
{
    SO1 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;  /* output CSI10 clock value 0 */
    SO1 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;   /* output CSI10 data value 0 */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;   /* enable CSI10 output */
    SS1 |= _0001_SAU_CH0_START_TRG_ON;     /* enable CSI10 */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
    CSIMK10 = 0U;    /* enable INTCSI10 */
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Stop
* Description  : This function stops CSI10.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Stop(void)
{
    CSIMK10 = 1U;    /* disable INTCSI10 interrupt */
    ST1 |= _0001_SAU_CH0_STOP_TRG_ON;        /* disable CSI10 */
    SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable CSI10 output */
    CSIIF10 = 0U;    /* clear INTCSI10 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_CSI10_Send
* Description  : This function sends CSI10 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_CSI10_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_csi10_tx_count = tx_num;       /* send data count */
        gp_csi10_tx_address = tx_buf;    /* send buffer pointer */
        CSIMK10 = 1U;    /* disable INTCSI10 interrupt */
        SDR10L = *gp_csi10_tx_address;     /* started by writing data to SDR[7:0] */
        gp_csi10_tx_address++;
        g_csi10_tx_count--;
        CSIMK10 = 0U;    /* enable INTCSI10 interrupt */
    }
    
    return (status);
}

/***********************************************************************************************************************
* Function Name: R_IIC11_Create
* Description  : This function initializes IIC11.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_Create(void)
{
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON;    /* disable IIC11 */
    IICMK11 = 1U;    /* disable INTIIC11 interrupt */
    IICIF11 = 0U;    /* clear INTIIC11 interrupt flag */
    /* Set INTIIC11 low priority */
    IICPR111 = 1U;
    IICPR011 = 1U;
    SIR11 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* error flag clear */
    SMR11 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS |
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_EDGE_FALL | _0004_SAU_IIC | _0000_SAU_TRANSFER_END;
    SCR11 = _0000_SAU_TIMING_1 | _0000_SAU_MSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR11 = _9E00_IIC11_DIVISOR;
    SO1 |= _0200_SAU_CH1_CLOCK_OUTPUT_1 | _0002_SAU_CH1_DATA_OUTPUT_1;
    /* use P60, P61 as SCL11, SDA11 */
    STSEL1 |= _00_IIC11_PIN_SELECTION_1;
    /* Set SCL11, SDA11 pin */
    P6 |= 0x03U;
    PM6 &= 0xFCU;
}

/***********************************************************************************************************************
* Function Name: R_IIC11_Master_Send
* Description  : This function starts transferring data for IIC11 in master mode.
* Arguments    : adr -
*                    set address for select slave
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{    
    g_iic11_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear IIC11 master status flag */
    adr &= 0xFEU;    /* send mode */
    g_iic11_master_status_flag = _01_SAU_IIC_SEND_FLAG;    /* set master status flag */
    SCR11 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
    SCR11 |= _8000_SAU_TRANSMISSION;
    /* Set paramater */
    g_iic11_tx_count = tx_num;
    gp_iic11_tx_address = tx_buf; 

    /* Start condition */
    R_IIC11_StartCondition();
    IICIF11 = 0U;    /* clear INTIIC11 interrupt flag */
    IICMK11 = 0U;    /* enable INTIIC11 */
    SDR11L = adr;
}

/***********************************************************************************************************************
* Function Name: R_IIC11_Master_Receive
* Description  : This function starts receiving data for IIC11 in master mode.
* Arguments    : adr -
*                    set address for select slave
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    g_iic11_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear master status flag */
    adr |= 0x01U;    /* receive mode */
    g_iic11_master_status_flag = _02_SAU_IIC_RECEIVE_FLAG;   /* set master status flag */
    SCR11 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
    SCR11 |= _8000_SAU_TRANSMISSION;
    /* Set parameter */
    g_iic11_rx_length = rx_num;
    g_iic11_rx_count = 0U;
    gp_iic11_rx_address = rx_buf;
    /* Start condition */
    R_IIC11_StartCondition();
    IICIF11 = 0U;    /* clear INTIIC11 interrupt flag */
    IICMK11 = 0U;    /* enable INTIIC11 */
    SDR11L = adr;
}

/***********************************************************************************************************************
* Function Name: R_IIC11_Stop
* Description  : This function stops the IIC11 operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_Stop(void)
{
    /* Stop transfer */
    IICMK11 = 1U;    /* disable INTIIC11 */
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON;    /* disable IIC11 */
    IICIF11 = 0U;    /* clear INTIIC11 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_IIC11_StartCondition
* Description  : This function starts IIC11 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_StartCondition(void)
{
    uint8_t w_count;
    SO1 &= ~_0002_SAU_CH1_DATA_OUTPUT_1;    /* clear IIC11 SDA */
    
    /* Change the waiting time according to the system */
    for (w_count = 0U; w_count <= IIC11_WAITTIME; w_count++)
    {
        NOP();
    }
    
    SOE1 |= _0002_SAU_CH1_OUTPUT_ENABLE;    /* enable IIC11 out */
    SO1 &= ~_0200_SAU_CH1_CLOCK_OUTPUT_1;   /* clear IIC11 SCL */
    SS1 |= _0002_SAU_CH1_START_TRG_ON;      /* enable IIC11 */
}

/***********************************************************************************************************************
* Function Name: R_IIC11_StopCondition
* Description  : This function stops IIC11 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC11_StopCondition(void)
{
    uint8_t w_count;
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON;       /* disable IIC11 */
    SOE1 &= ~_0002_SAU_CH1_OUTPUT_ENABLE;   /* disable IIC11 out */
    SO1 &= ~_0002_SAU_CH1_DATA_OUTPUT_1;    /* clear IIC11 SDA */
    SO1 |= _0200_SAU_CH1_CLOCK_OUTPUT_1;    /* set IIC11 SCL */
    
    /* Change the waiting time according to the system */
    for (w_count = 0U; w_count <= IIC11_WAITTIME; w_count++)
    {
        NOP();
    }
    
    SO1 |= _0002_SAU_CH1_DATA_OUTPUT_1;     /* set IIC11 SDA */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
