/** ###################################################################
**     Filename  : Curve.c
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 25/01/2018, 14:28
**     Contents  :
**         User source code
**
** ###################################################################*/

/* MODULE Curve */

#include "customTypedef.h"
#include "CurveHeader.h"

// constant declarations -----------------------------------------------------

// Each of these tables may be constructed using any number of entries and any
// width needed thereby providing a process to create translations at any
// level of accuracy needed.


#define ROWS_BATT_VOLT       27U
#define COLS_BATT_VOLT       2U

//Lookup table for Battery Voltage*10 to be display on LCD 
const u32 wBatt[ROWS_BATT_VOLT][COLS_BATT_VOLT] =
{
	//  ADC VALUE  BATT VOLT*10		
	{	192 	,	60	},
	{	195 	,	70	},
	{	225 	,	80	},
	{	255 	,	90	},
	{	284 	,	100 },
	{	314 	,	110 },
	{	342 	,	120 },
	{	372 	,	130 },
	{	401 	,	140 },
	{	429 	,	150 },
	{	457 	,	160 },
	{	487 	,	170 },
	{	515 	,	180 },
	{	545 	,	190 },
	{	573 	,	200 },
	{	602 	,	210 },
	{	631 	,	220 },
	{	659 	,	230 },
	{	688 	,	240 },
	{	717 	,	250 },
	{	746 	,	260 },
	{	775 	,	270 },
	{	804 	,	280 },
	{	832 	,	290 },
	{	861 	,	300 },
	{	895 	,	310 },
	{	924 	,	320 },


};


#define ROWS_RES_LEVEL       5U
#define COLS_RES_LEVEL       2U

//Lookup table for  Resistive fuel gauge to calulate level ; for broadcast over CAN 
const u32 wResLevel[ROWS_RES_LEVEL][COLS_RES_LEVEL] =
{
	//  Res*10  	Level		
	{	30 		,	0		},
	{	330 	,	2500	},
	{	700		,	5000	},
	{	1090	,	7500	},
	{	1800	,	10000	},

};


#define ROWS_RES       47U
#define COLS_RES       2U

//used for old hardware ;Lookup table for calculate Resistance ohms*10 for Fuel,AP,OP gauges 
const u32 wRes[ROWS_RES][COLS_RES] =
{

	{	0		,	0		},
	{	33		,	50		},
	{	63		,	100 	},
	{	92		,	150 	},
	{	118 	,	200 	},
	{	141 	,	250 	},
	{	164 	,	300 	},
	{	184 	,	350 	},
	{	203 	,	400 	},
	{	220 	,	450 	},
	{	237 	,	500 	},
	{	253 	,	550 	},
	{	267 	,	600 	},
	{	280 	,	650 	},
	{	293 	,	700 	},
	{	306 	,	750 	},
	{	318 	,	800 	},
	{	329 	,	850 	},
	{	339 	,	900 	},
	{	349 	,	950 	},
	{	359 	,	1000	},
	{	368 	,	1050	},
	{	376 	,	1100	},
	{	385 	,	1150	},
	{	392 	,	1200	},
	{	400 	,	1250	},
	{	406 	,	1300	},
	{	414 	,	1350	},
	{	420 	,	1400	},
	{	426 	,	1450	},
	{	432 	,	1500	},
	{	438 	,	1550	},
	{	444 	,	1600	},
	{	449 	,	1650	},
	{	455 	,	1700	},
	{	458 	,	1750	},
	{	464 	,	1800	},
	{	470 	,	1850	},
	{	474 	,	1900	},
	{	477 	,	1950	},
	{	482 	,	2000	},
	{	490 	,	2100	},
	{	497 	,	2200	},
	{	505 	,	2300	},
	{	511 	,	2400	},
	{	518 	,	2500	},
	{	525 	,	2600	},


};
//used for New hardware ;Lookup table for calculate Resistance ohms*10 for Fuel,AP,OP gauges 
const u32 wResFuel[ROWS_RES][COLS_RES] =
{

	{	0		,	0	},
	{	23		,	50	},
	{	47		,	100 },
	{	71		,	150 },
	{	91		,	200 },
	{	111 	,	250 },
	{	129 	,	300 },
	{	150 	,	350 },
	{	161 	,	400 },
	{	176 	,	450 },
	{	189 	,	500 },
	{	203 	,	550 },
	{	215 	,	600 },
	{	226 	,	650 },
	{	237 	,	700 },
	{	247 	,	750 },
	{	256 	,	800 },
	{	266 	,	850 },
	{	274 	,	900 },
	{	282 	,	950 },
	{	290 	,	1000	},
	{	299 	,	1050	},
	{	304 	,	1100	},
	{	311 	,	1150	},
	{	317 	,	1200	},
	{	324 	,	1250	},
	{	330 	,	1300	},
	{	336 	,	1350	},
	{	341 	,	1400	},
	{	347 	,	1450	},
	{	352 	,	1500	},
	{	357 	,	1550	},
	{	361 	,	1600	},
	{	366 	,	1650	},
	{	369 	,	1700	},
	{	377 	,	1750	},
	{	379 	,	1800	},
	{	383 	,	1850	},
	{	387 	,	1900	},
	{	391 	,	1950	},
	{	395 	,	2000	},
	{	401 	,	2100	},
	{	408 	,	2200	},
	{	414 	,	2300	},
	{	420 	,	2400	},
	{	426 	,	2500	},
	{	431 	,	2600	},


};
#define ROWS_CAP_LEVEL       7U
#define COLS_CAP_LEVEL       2U

//Lookup table for Capacitive fuel gauge to calulate level ; for broadcast over CAN 
const u32 wCapADCLevel[ROWS_CAP_LEVEL][COLS_CAP_LEVEL] =
{
	//  ADC Value  	Level		
	{	0 		,	0		},
	{	84 		,	0		},
	{	170 	,	1200	},
	{	278		,	2500	},
	{	462		,	4850	},
	{	661		,	7500	},
	{	852		,	10000	},

};


//Lookup table for Capacitive fuel gauge to calulate level ; for broadcast over CAN 
const u32 wSecondyfuelLevel[ROWS_CAP_LEVEL][COLS_CAP_LEVEL] =
{
	//  ADC Value  	Level		
	{	0 	 		,	0			},
	{	500			,	0			}, //Empty level
	{	1070 		,	625			}, //1/8
	{	1640		,	1250		}, //1/4
	{	2750		,	2500		}, //1/2
	{	3850		,	3750		}, //3/4
	{	5000		,	5000		}, //Full

};


#define ADC_VOLT_ROWS	51U
#define ADC_VOLT_COLS	2U

//Lookup table for  Capacitive fuel gauge to calulate voltage ; for broadcast over CAN 
const u32 wAdcToVoltage[ADC_VOLT_ROWS][ADC_VOLT_COLS] =
{
	 //ADC    //Volt (mV)
	{	0	,	0		},
	{	15	,	100		},
	{	33	,	200		},
	{	50	,	300		},
	{	66	,	400		},
	{	84	,	500		},
	{	101	,	600		},
	{	117	,	700		},
	{	135	,	800		},
	{	152	,	900		},
	{	170	,	1000	},
	{	187	,	1100	},
	{	203	,	1200	},
	{	219	,	1300	},
	{	238	,	1400	},
	{	254	,	1500	},
	{	272	,	1600	},
	{	292	,	1700	},
	{	306	,	1800	},
	{	322	,	1900	},
	{	339	,	2000	},
	{	357	,	2100	},
	{	373	,	2200	},
	{	392	,	2300	},
	{	410	,	2400	},
	{	427	,	2500	},
	{	443	,	2600	},
	{	462	,	2700	},
	{	475	,	2800	},
	{	494	,	2900	},
	{	512	,	3000	},
	{	528	,	3100	},
	{	545	,	3200	},
	{	562	,	3300	},
	{	582	,	3400	},
	{	596	,	3500	},
	{	614	,	3600	},
	{	630	,	3700	},
	{	648	,	3800	},
	{	665	,	3900	},
	{	682	,	4000	},
	{	700	,	4100	},
	{	714	,	4200	},
	{	732	,	4300	},
	{	750	,	4400	},
	{	768	,	4500	},
	{	785	,	4600	},
	{	802	,	4700	},
	{	819	,	4800	},
	{	838	,	4900	},
	{	852	,	5000	},
};					



// local declarations --------------------------------------------------------
// These are variables used for the linear interprolation calculation.
// They are defined here to save stack space during use.
s32   R1;
s32   R2;
s32   R3;
s32   V1;
s32   V2;
s32   V3;

s32 overrange;

// private prototypes --------------------------------------------------------
static s16 _iGetTableValue(u32 const *, u32, u32 *, u8, u8, u8);


/*****************************************************************************
 **  Description:
 **      Returns the value from specified table.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     table ID,
 **                  resistance value
 **                  pointer to word to store the value
 **
 **  Return value:   amount of overrange as:
 **                      0   returned value is within the table range,
 **                      < 0 returned value is below the lowest value in the table,
 **                      > 0 returned value is above the highest value in the table
 **
 *****************************************************************************/
void vGetCurveDataValue(eCurveType curve, u32 inVal, u32 *value)
{
	int overrange; // overrange can use in future if required 

	switch (curve)
	{
	
	case eBattVoltCurve :
		overrange = _iGetTableValue((u32 *)wBatt, inVal, value, ROWS_BATT_VOLT, COLS_BATT_VOLT, 1);
		
		break;
	case eResCurve :
		overrange = _iGetTableValue((u32 *)wRes, inVal, value, ROWS_RES, COLS_RES, 1);
	
	break;
	case eResFuelCurve :		
			overrange = _iGetTableValue((u32 *)wResFuel, inVal, value, ROWS_RES, COLS_RES, 1);			
	break;
	case eResLevel :		
		overrange = _iGetTableValue((u32 *)wResLevel, inVal, value, ROWS_RES_LEVEL, COLS_RES_LEVEL, 1);
	
	break;
	case eCapacitiveFuelSensor :
		overrange = _iGetTableValue((u32 *)wCapADCLevel, inVal, value, ROWS_CAP_LEVEL, COLS_CAP_LEVEL, 1);
	
	break;
	case eCapacitiveFuelVolt :
		overrange = _iGetTableValue((u32 *)wAdcToVoltage, inVal, value, ADC_VOLT_ROWS, ADC_VOLT_COLS, 1);
	
	break;
	case eSecondryFuelTank :
		overrange = _iGetTableValue((u32 *)wSecondyfuelLevel, inVal, value, ROWS_CAP_LEVEL, COLS_CAP_LEVEL, 1);
	
	break;	
	
	default :
		*value = (u32)-1;
		overrange = 0;
		break;
	}
	
}



/*****************************************************************************
 **  Description:
 **      This functi0n finds the closest match in the table for the given
 **      value. Linear interprolation is performed between values. It will
 **      set the amount of overrange.
 **
 **      What this function will handle:
 **          Tables of any size up to 256 x 256
 **          Tables with increasing or decreasing values in any of the columns.
 **
 **      Linear Interprolation Algorithm
 **           table where,
 **                  R = the resistance or other input value
 **                  V = the converted value (pressure, temperature, etc)
 **                 R  |  V
 **                 -------
 **                 R1 | V1 (table values)
 **   (input value) R2 | V2 (the calculated output value)
 **                 R3 | V3 (table values)
 **
 **                  (R2 - R1) * (V3 - V1)
 **          V2 =    --------------------- + V1
 **                        (R3 - R1)
 **
 **      What this function will not handle:
 **          Tables with negative numbers.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     table pointer,
 **                  input value,
 **                  pointer to word for value storage
 **                  number of rows in the table,
 **                  number of columns in the table,
 **                  column index
 **
 **  Return value:   an integer indicating the percent of +- overrange
 **
 *****************************************************************************/
static s16 _iGetTableValue(u32 const *tabPtr, u32 inVal, u32 *valPtr, u8 numOfRows, u8 numOfCols, u8 colIndex)
{
	s16 i;
	bool withinTableLimits = FALSE;

	if (tabPtr[numOfCols] > tabPtr[0])
	{
		// Table first column is increasing
		if (inVal <= tabPtr[0])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[0 + colIndex];
			overrange = (((s32)inVal - (s32)tabPtr[0]) * 100) / (s32)tabPtr[0];
		}
		else if (inVal >= tabPtr[(numOfRows-1) * numOfCols])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[(numOfRows-1) * numOfCols + colIndex];
			overrange = (((s32)inVal - (s32)tabPtr[(numOfRows-1) * numOfCols]) * 100) / (s32)tabPtr[(numOfRows-1) * numOfCols];
		}
		else {
			// Set the index
			withinTableLimits = TRUE;
			i = 0;
			while (tabPtr[i * numOfCols] < inVal)
				++i;
		}
	}
	else {
		// Table first column is decreasing
		if (inVal >= tabPtr[0])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[0 + colIndex];
			overrange = (((s32)inVal - (s32)tabPtr[0]) * 100) / (s32)tabPtr[0];
		}
		else if (inVal <= tabPtr[(numOfRows-1) * numOfCols])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[(numOfRows-1) * numOfCols + colIndex];
			overrange = (((s32)inVal - (s32)tabPtr[(numOfRows-1) * numOfCols]) * 100) / (s32)tabPtr[(numOfRows-1) * numOfCols];
		}
		else {
			// Set the index
			withinTableLimits = TRUE;
			i = 0;
			while (tabPtr[i * numOfCols] > inVal)
				++i;
		}
	}

	if (withinTableLimits)
	{
		// It is not beyond the table limits
		overrange = 0;

		i *= numOfCols; // index of R1

		// Use these variables to save stack space during calculation
		R1 = (s32)tabPtr[i];
		R3 = (s32)tabPtr[i-numOfCols];
		V1 = (s32)tabPtr[i+colIndex];
		V3 = (s32)tabPtr[i-numOfCols+colIndex];
		R2 = (s32)inVal;

		V2 = ((100 * ((R2 - R1) * (V3 - V1))) / (R3 - R1)) + (100 * V1);
		*valPtr = (u32)(V2 / 100);
	}

	return (s16)overrange;
}



/* END Curve */
