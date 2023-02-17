#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "r_cg_userdefine.h"
#include "GenIllumination.h"
#include "Eol.h"


#define ILL_SENSE_TIME            	200     //Ignition ON/OFF time sense in ms
#define DIAL_IILU_DAY_MODE			0


typedef struct
{
	u8 u8DayLightIll;
	u8 u8NightIll;	
	bool bGenIlluminationUpdate;
} GEN_DATA_TYPE;

GEN_DATA_TYPE sGenIll = {0x00,0x00,0x00};


/*********************************************************************//**
 *
 * Set Illumination update status 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vSetGenIlluUpdateFlag(bool status)
{
	sGenIll.bGenIlluminationUpdate = status;
}

/*********************************************************************//**
 *
 * Get Illumination update status 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
bool bGetGenIlluUpdateFlag()
{
	return sGenIll.bGenIlluminationUpdate;
}

/********************************************************************//**
 *
 * get Day Light Illumination value 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
u8 u8GetDayLightIlluValue()
{
	return sGenIll.u8DayLightIll;
}



/*********************************************************************//**
 *
 * Set Day Light Illumination value 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vSetDayLightIlluValue(u8 value)
{
	 sGenIll.u8DayLightIll = value;
}

/*********************************************************************//**
 *
 * get Night Illumination value 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
u8 u8GetNightIlluValue()
{
	return sGenIll.u8NightIll;
}


/*********************************************************************//**
 *
 * Set Night Illumination value 
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vSetNightIlluValue(u8 value)
{
	sGenIll.u8NightIll = value;
}


/*********************************************************************//**
 *
 * check Illumination input and contol Illumination according to input
 *
 * @param      bIgnition 	IGNITION STATUS    and force fully action for illumination 
 *
 * @return     None
 *************************************************************************/

void vIlluminationControl(bool bIgnition, eILL_Status_TYPE forceIllumStatus)
{	
	
	bool IllInputStatus = 0;
	static u8 PreNightIll = eILLBARLIMITE; //set default value out of illumination setting range 
	static u8 PreDayIll = eILLBARLIMITE;	//so the at list one time function will execute while ignition off


	if(forceIllumStatus == eIlluminationOff)
	{
		DIAL_ILL_OFF;						//Dial and LCD illumination OFF
		LCD_ILL_OFF;
	}
	else if(forceIllumStatus == eIlluminationFullIntensityOn)
	{
		DIAL_ILL_100_PER;		//Dial and LCD illumination Max
		LCD_ILL_100_PER;
	}
	else
	{

		IllInputStatus = bGetIlluminationInputStatus();
		
		if(IllInputStatus == TRUE && bIgnition == TRUE)
		{
			//initialize day var while in Night mode so that when switch in day mode then ill On as per pre setting
			PreDayIll = eILLBARLIMITE;
			
			 if(PreNightIll != sGenIll.u8NightIll )
			 {
				 	PreNightIll = sGenIll.u8NightIll;
					switch(sGenIll.u8NightIll)
					{
						case 0:
							DIAL_ILL_5_PER;
							LCD_ILL_5_PER;
						break;
						case 1:
							DIAL_ILL_20_PER;
							LCD_ILL_20_PER;				
						break;
						case 2:
							DIAL_ILL_40_PER;
							LCD_ILL_40_PER;				
						break;
						case 3:
							DIAL_ILL_60_PER;
							LCD_ILL_60_PER;				
						break;
						case 4:
							DIAL_ILL_80_PER;
							LCD_ILL_80_PER;				
						break;
						case 5:
							DIAL_ILL_100_PER;
							LCD_ILL_100_PER;
						break;
				}
			 }		
		}
		else if(IllInputStatus == FALSE && bIgnition == TRUE)
		{
					
			//initialize Night var while in day mode so that when switch in night mode then ill On as per pre setting
			PreNightIll = eILLBARLIMITE; 
			
			if(PreDayIll != sGenIll.u8DayLightIll )
			{
				PreDayIll = sGenIll.u8DayLightIll;		
				switch(sGenIll.u8DayLightIll)
				{
					case 0:				
						LCD_ILL_5_PER;
						//In Day light mode Dial illumination also work if set dial illumination by EOL 
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_5_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
						   
							
					break;
					case 1:				
						LCD_ILL_20_PER;	
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_20_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
					break;
					case 2:				
						LCD_ILL_40_PER;	
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_40_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
					break;
					case 3:				
						LCD_ILL_60_PER;	
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_60_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
					break;
					case 4:				
						LCD_ILL_80_PER;	
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_80_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
					break;
					case 5:				
						LCD_ILL_100_PER;
						if(sIndicatorInputType.Inputsbytes.Byte4.bDialIlluSelection == DIAL_IILU_DAY_MODE)
						{
							DIAL_ILL_100_PER;
						}
						else
						{
						   DIAL_ILL_OFF;
						}
					break;
				}
			}
			
		}
		else if(bIgnition == FALSE)   //Use for power moding when Ignition off
		{
		  if(IllInputStatus == FALSE )
			{   LCD_ILL_OFF;
				DIAL_ILL_OFF;	
			}
		  else if(IllInputStatus == TRUE)
			{   LCD_ILL_OFF;
				DIAL_ILL_100_PER;	
			}
		  
		}
	  	
	}


}


/*********************************************************************//**
 *
 * get illumination signal status
 *
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
bool bGetIlluminationInputStatus()
{
	static bool status = FALSE;
	static u16 IllOnTimeStamp = 0;
	static u16 IllOffTimeStamp = 0;			
	
		if(ILL_INPUT == HIGH )
		{
			if( u16GetClockTicks() - IllOffTimeStamp	>= ILL_SENSE_TIME)
			 status = FALSE;
			
			IllOnTimeStamp = u16GetClockTicks() ;
		}
		else if(ILL_INPUT == LOW )
		{
			if( u16GetClockTicks() - IllOnTimeStamp	>= ILL_SENSE_TIME)
			 status = TRUE;	

			IllOffTimeStamp = u16GetClockTicks() ;
		}
		

	return status;	

  
}




