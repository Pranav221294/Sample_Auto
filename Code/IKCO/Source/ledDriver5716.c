
#include"ledDriver5716.h"
#include "customTypedef.h"
#include "r_cg_wdt.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"


static bool bSpiLatchSequence = FALSE;
static bool bSpiDataSend = 0;
void delayus();
//Led driver Initilaization Buffer
		
const u8 u8LedDriverInitBuffer[GS_DATA_SIZE] = 
			{
               
               0x00,0x00,0x00,	 
               0x00,0x00,0x00,	 
			   0x00,0x00,0x00,	 
               0x80,0x01,0x88,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,	
         	   0xff,0xFF,0xff,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,	
         	   0xff,0xFF,0xff,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,
               0x00,0x00,0x00,	 
               0x00,0x00,0x00,	 
			   0x00,0x00,0x00,	 
               0x80,0x01,0x88,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,	
         	   0xff,0xFF,0xff,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,	
         	   0xff,0xFF,0xff,
			   0xff,0xFF,0xff,	
               0xff,0xFF,0xff,
         	
			   };
			   
// Led Driver data buffer to store tell-tale status

u8 u8GSdata[GS_DATA_SIZE] = 
			{
              0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
         	  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
			  0x00,0x00,0x00,	
              0x00,0x00,0x00,	
         	  0x00,0x00,0x00,
            
			};




/*****************************************************************************
**  Function name:  vLedDriverInit
**
**  Description:    Initialize Led buffer data ,Led driver driven by SPI communication by ch. SCI10
**
**  Parameters:     none
**
**  Return value:   none
**
*****************************************************************************/

void vLedDriverInit()
{
	bool flag  = FALSE;
	u16 LedDriverDataSentTime =0;

	BLANK = LOW;	//keep blank low during initialization so that all lamp ON becomes together
	LATCH = HIGH;
	delayus();
	bSpiLatchSequence = TRUE;
	
	R_CSI10_Send(&u8LedDriverInitBuffer,GS_DATA_SIZE);


	//Wait here untill led driver initialization buffer sent
	LedDriverDataSentTime = u16GetClockTicks(); 
		while(flag == FALSE )
			{
			 R_WDT_Restart();
			 
			  if(u16GetClockTicks() - LedDriverDataSentTime > 2000)
				  flag = TRUE;
			  else
			  	flag = bSpiDataSend;
			}
}

/*****************************************************************************
**  Function name:  vLedDataSendToSpi
**
**  Description:   Send tell-tales data store in u8GSdata buffer to SPI 
**
**  Parameters:     none
**
**  Return value:   none
**
*****************************************************************************/

void vLedDataSendToSpi()
{

	if(bSpiDataSend == FALSE)
		return;	
	LATCH = LOW;
	NOP();NOP();
	bSpiDataSend = FALSE;
	R_CSI10_Send(&u8GSdata,GS_DATA_SIZE);

	if(eGetClusterStatus() == eClusterNormalOperation)
		BLANK = HIGH;
	
	
}

/***********************************************************************************************************************
* Function Name: vLedDriverSendEndCallback
* Description  : This function is a callback function of LedDriverInterrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
 void vLedDriverSendEndCallback(void)
{
    /* Start user code. Do not edit comment generated here */
    
	
	bSpiDataSend = TRUE;
	
	if(bSpiLatchSequence == TRUE)	
	{
		LATCH = LOW;
		delayus();		//Delay required b/w in latch transaction otherwise led drive malfunction 
		LATCH = HIGH;
		delayus();
		bSpiLatchSequence = FALSE;
	}
	else
	{
		LATCH = HIGH;
		delayus();
		LATCH = LOW;
		delayus();
	}
	
	/* End user code. Do not edit comment generated here */
}

void delayus()
{
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
}
/*****************************************************************************
**  Function name:  vSetLedDriverBlankHigh
**
**  Description:   Set Led driver BLANK pin to HIGH
**
**  Parameters:     None
**
**  Return value:   none
**
*****************************************************************************/

void vSetLedDriverBlankHigh()
{

bool flag  = FALSE;
u16 LedDriverDataSentTime =0;
	//Disable Indicator bilink 1 time so we wait here until data not reached 
	//to led driver then set BLANK pin high
	
		LedDriverDataSentTime = u16GetClockTicks(); 
		while(flag == FALSE )
			{
			 R_WDT_Restart();
			 
			  if(u16GetClockTicks() - LedDriverDataSentTime > 2000)
				  flag = TRUE;
			  else
			  	flag = bSpiDataSend;
			}

	BLANK = HIGH;
}


/*****************************************************************************
**  Function name:  vSetLedDriverBlankLow
**
**  Description:   Set Led driver BLANK pin to HIGH
**
**  Parameters:     None
**
**  Return value:   none
**
*****************************************************************************/

void vSetLedDriverBlankLow()
{
	BLANK = LOW;
}



