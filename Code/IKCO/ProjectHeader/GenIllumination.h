#ifndef __GEN_ILLUMINATION_H
#define __GEN_ILLUMINATION_H

#include "r_cg_macrodriver.h"
#include "customTypedef.h"

// Constant declarations 

#define ILL_INPUT                 	P1.7


typedef enum
{
	e5Percent	= 25, //10,
	e20Percent	= 100,
	e40Percent	= 200,
	e60Percent	= 300,
	e80Percent	= 400,
	e100Percent	= 510      //Max value in compare resistor TDR10
 } eILL_PERCENTAGE;
	
#define DIAL_ILL_OFF		{TDR12 = 0;}
#define DIAL_ILL_5_PER		{TDR12 = e5Percent;}
#define DIAL_ILL_20_PER		{TDR12 = e20Percent;}
#define DIAL_ILL_40_PER		{TDR12 = e40Percent;}
#define DIAL_ILL_60_PER		{TDR12 = e60Percent;}
#define DIAL_ILL_80_PER		{TDR12 = e80Percent;}
#define DIAL_ILL_100_PER	{TDR12 = e100Percent;}

#define LCD_ILL_OFF			{TDR14 = 0;}
#define LCD_ILL_5_PER		{TDR14 = e5Percent;	}
#define LCD_ILL_20_PER		{TDR14 = e20Percent;}
#define LCD_ILL_40_PER		{TDR14 = e40Percent;}
#define LCD_ILL_60_PER		{TDR14 = e60Percent;}
#define LCD_ILL_80_PER		{TDR14 = e80Percent;}
#define LCD_ILL_100_PER		{TDR14 = e100Percent;}

typedef enum
{
	eILLBAR0,	//No bar display 
	eILLBAR1,	 
	eILLBAR2,
	eILLBAR3,
	eILLBAR4,
	eILLBAR5,	//All bar display
	eILLBARLIMITE,
}eILL_TYPE;

typedef enum
{
	eIlluminationOff,
	eIlluminationFullIntensityOn,
	eIlluminationNormalMode,	// illumnation work as per Ill input and set level by switch	
}eILL_Status_TYPE;


void vIlluminationControl( bool , eILL_Status_TYPE );
bool bGetIlluminationInputStatus();
u8 u8GetDayLightIlluValue();
void vSetDayLightIlluValue(u8);
u8 u8GetNightIlluValue();
void vSetNightIlluValue(u8);
void vSetGenIlluUpdateFlag(bool);
bool bGetGenIlluUpdateFlag(void);






#endif
