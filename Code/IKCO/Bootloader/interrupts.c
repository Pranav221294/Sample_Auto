
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma interrupt INTWDTI watchdog_intrpt
#pragma interrupt INTLVI lvi_intrpt
#pragma interrupt INTP0 intp0_intrpt
#pragma interrupt INTP1 intp1_intrpt
#pragma interrupt INTP2 intp2_intrpt
#pragma interrupt INTP3 intp3_intrpt
#pragma interrupt INTP4 intp4_intrpt
#pragma interrupt INTP5 intp5_intrpt
#pragma interrupt INTCLM clock_monitor_intrpt
#pragma interrupt INTCSI00 csi00_end_intrpt
#pragma interrupt INTCSI01 csi01_end_intrpt
#pragma interrupt INTDMA0 dma0_end_intrpt
#pragma interrupt INTDMA1 dma1_end_intrpt
#pragma interrupt INTRTC rtc_intrpt
#pragma interrupt INTIT interval_timer_intrpt

#pragma interrupt INTLT0 lin_uart0_tx_intrpt
#pragma interrupt INTLR0 lin_uart0_rx_intrpt
#pragma interrupt INTLS0 lin_uart0_rxerr_intrpt
#pragma interrupt INTPLR0 lin_uart0_pin_intrpt
#pragma interrupt INTSG sg_intrpt
#pragma interrupt INTTM00 timer00_intrpt
#pragma interrupt INTTM01 timer01_intrpt
#pragma interrupt INTTM02 timer02_intrpt
#pragma interrupt INTTM03 timer03_intrpt
#pragma interrupt INTAD adc_intrpt
#pragma interrupt INTLT1 lin_uart1_tx_intrpt
#pragma interrupt INTLR1 lin_uart1_rx_intrpt
#pragma interrupt INTLS1 lin_uart1_rxerr_intrpt
#pragma interrupt INTPLR1 lin_uart1_pin_intrpt
#pragma interrupt INTCSI10 csi10_end_intrpt
#pragma interrupt INTIIC11 iic11_end_intrpt
#pragma interrupt INTTM04 timer04_intrpt
#pragma interrupt INTTM05 timer05_intrpt
#pragma interrupt INTTM06 timer06_intrpt
#pragma interrupt INTTM07 timer07_intrpt
//------------------------------------------------
//#pragma interrupt INTC1ERR can1_err_intrpt
//#pragma interrupt INTC1WUP can1_wake_intrpt
#pragma interrupt INTC0ERR can0_err_intrpt
#pragma interrupt INTC0WUP can0_wake_intrpt
#pragma interrupt INTC0REC can0_rx_intrpt
#pragma interrupt INTC0TRX can0_tx_intrpt
#pragma interrupt INTTM10 timer10_intrpt
#pragma interrupt INTTM11 timer11_intrpt
#pragma interrupt INTTM12 timer12_intrpt
#pragma interrupt INTTM13 timer13_intrpt
#pragma interrupt INTMD division_end_intrpt
//#pragma interrupt INTC1REC can1_rx_intrpt
#pragma interrupt INTFL flash_prgrm_intrpt
//#pragma interrupt INTC1TRX can1_tx_intrpt
#pragma interrupt INTTM14 timer14_intrpt
#pragma interrupt INTTM15 timer15_intrpt
#pragma interrupt INTTM16 timer16_intrpt
#pragma interrupt INTTM17 timer17_intrpt
#pragma interrupt INTTM20 timer20_intrpt
#pragma interrupt INTTM21 timer21_intrpt
#pragma interrupt INTTM22 timer22_intrpt
#pragma interrupt INTTM23 timer23_intrpt
#pragma interrupt INTTM24 timer24_intrpt
#pragma interrupt INTTM26 timer26_intrpt
#pragma interrupt INTDMA2 dma2_end_intrpt
#pragma interrupt INTDMA3 dma3_end_intrpt




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "r_cg_macrodriver.h"
#include "ramVector.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__interrupt static void watchdog_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTWDTI_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lvi_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLVI_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp0_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp1_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP1_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp2_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP2_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp3_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP3_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp4_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP4_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void intp5_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTP5_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void clock_monitor_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTCLM_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void csi00_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTCSI00_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void csi01_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTCSI01_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void dma0_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTDMA0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void dma1_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTDMA1_ISR)();
    /* End user code. Do not edit comment generated here */
}
__interrupt static void rtc_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTRTC_ISR)();
    /* End user code. Do not edit comment generated here */
}
__interrupt static void interval_timer_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTIT_ISR)();
    /* End user code. Do not edit comment generated here */
}



__interrupt static void lin_uart0_tx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLT0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart0_rx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLR0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart0_rxerr_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLS0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart0_pin_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTPLR0_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void sg_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTSG_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer00_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM00_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer01_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM01_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer02_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM02_ISR)();
    /* End user code. Do not edit comment generated here */
}

//--------------------------------------------------------------------------------------
__interrupt static void timer03_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM03_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void adc_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTAD_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart1_tx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLT1_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart1_rx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLR1_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart1_rxerr_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTLS1_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void lin_uart1_pin_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTPLR1_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void csi10_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTCSI10_ISR)();
    /* End user code. Do not edit comment generated here */
}

__interrupt static void iic11_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTIIC11_ISR)();
    /* End user code. Do not edit comment generated here */
}

__interrupt static void timer04_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM04_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer05_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM05_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer06_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM06_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer07_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM07_ISR)();
    /* End user code. Do not edit comment generated here */
}


/*__interrupt static void can1_err_intrpt(void)
{
    /* Start user code. Do not edit comment generated here 
    (*RAM_INTC1ERR_ISR)();
    /* End user code. Do not edit comment generated here 
} */

/*__interrupt static void can1_wake_intrpt(void)
{
    /* Start user code. Do not edit comment generated here 
    (*RAM_INTC1WUP_ISR)();
    /* End user code. Do not edit comment generated here 
}*/


__interrupt static void can0_err_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTC0ERR_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void can0_wake_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTC0WUP_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void can0_rx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTC0REC_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void can0_tx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTC0TRX_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer10_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM10_ISR)();
    /* End user code. Do not edit comment generated here */
}

//-------------------------------------------------------------------------------------------------
__interrupt static void timer11_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM11_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer12_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM12_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer13_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM13_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void division_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTMD_ISR)();
    /* End user code. Do not edit comment generated here */
}

/*__interrupt static void can1_rx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here 
    (*RAM_INTC1REC_ISR)();
    /* End user code. Do not edit comment generated here 
}*/

__interrupt static void flash_prgrm_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTFL_ISR)();
    /* End user code. Do not edit comment generated here */
}
/*__interrupt static void can1_tx_intrpt(void)
{
    /* Start user code. Do not edit comment generated here 
    (*RAM_INTC1TRX_ISR)();
    /* End user code. Do not edit comment generated here 
}*/


__interrupt static void timer14_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM14_ISR)();
    /* End user code. Do not edit comment generated here */
}

__interrupt static void timer15_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM15_ISR)();
    /* End user code. Do not edit comment generated here */
}

__interrupt static void timer16_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM16_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer17_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM17_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer20_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM20_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer21_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM21_ISR)();
    /* End user code. Do not edit comment generated here */
}
__interrupt static void timer22_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM22_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer23_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM23_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer24_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM24_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void timer26_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTTM26_ISR)();
    /* End user code. Do not edit comment generated here */
}
__interrupt static void dma2_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTDMA2_ISR)();
    /* End user code. Do not edit comment generated here */
}


__interrupt static void dma3_end_intrpt(void)
{
    /* Start user code. Do not edit comment generated here */
    (*RAM_INTDMA3_ISR)();
    /* End user code. Do not edit comment generated here */
}
