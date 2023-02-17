
// start RAM location
#pragma section @@DATAL RAM_VECT AT 0FC300H //0FDF00H   // i.e start adress of RAM
void (*RAM_INTWDTI_ISR)();
void (*RAM_INTLVI_ISR)();
void (*RAM_INTP0_ISR)();
void (*RAM_INTP1_ISR)();
void (*RAM_INTP2_ISR)();
void (*RAM_INTP3_ISR)();
void (*RAM_INTP4_ISR)();
void (*RAM_INTP5_ISR)();
void (*RAM_INTCLM_ISR)();
void (*RAM_INTCSI00_ISR)();
void (*RAM_INTCSI01_ISR)();
void (*RAM_INTDMA0_ISR)();
void (*RAM_INTDMA1_ISR)();
void (*RAM_INTRTC_ISR)();
void (*RAM_INTIT_ISR)();

void (*RAM_INTLT0_ISR)();
void (*RAM_INTLR0_ISR)();
void (*RAM_INTLS0_ISR)();
void (*RAM_INTPLR0_ISR)();
void (*RAM_INTSG_ISR)();
void (*RAM_INTTM00_ISR)();
void (*RAM_INTTM01_ISR)();
void (*RAM_INTTM02_ISR)();
void (*RAM_INTTM03_ISR)();

void (*RAM_INTAD_ISR)();
void (*RAM_INTLT1_ISR)();
void (*RAM_INTLR1_ISR)();
void (*RAM_INTLS1_ISR)();
void (*RAM_INTPLR1_ISR)();
void (*RAM_INTCSI10_ISR)();
void (*RAM_INTIIC11_ISR)();
void (*RAM_INTTM04_ISR)();
void (*RAM_INTTM05_ISR)();
void (*RAM_INTTM06_ISR)();
void (*RAM_INTTM07_ISR)();
//------------------------------------------------------------------------------------------
//void (*RAM_INTC1ERR_ISR)();
//void (*RAM_INTC1WUP_ISR)();
void (*RAM_INTC0ERR_ISR)();
void (*RAM_INTC0WUP_ISR)();
void (*RAM_INTC0REC_ISR)();
void (*RAM_INTC0TRX_ISR)();
void (*RAM_INTTM10_ISR)();

void (*RAM_INTTM11_ISR)();
void (*RAM_INTTM12_ISR)();
void (*RAM_INTTM13_ISR)();
void (*RAM_INTMD_ISR)();
//void (*RAM_INTC1REC_ISR)();
void (*RAM_INTFL_ISR)();
//void (*RAM_INTC1TRX_ISR)();

void (*RAM_INTTM14_ISR)();
void (*RAM_INTTM15_ISR)();
void (*RAM_INTTM16_ISR)();
void (*RAM_INTTM17_ISR)();
void (*RAM_INTTM20_ISR)();
void (*RAM_INTTM21_ISR)();
void (*RAM_INTTM22_ISR)();
void (*RAM_INTTM23_ISR)();
void (*RAM_INTTM24_ISR)();
void (*RAM_INTTM26_ISR)();
void (*RAM_INTDMA2_ISR)();
void (*RAM_INTDMA3_ISR)();

