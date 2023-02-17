
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/
/*****************************************************************************
 **  Module: J1939.c
 **
 ** ---------------------------------------------------------------------------
 **  Description:
 **      Provides all J1939 functions except address claim
 **
 *****************************************************************************/

#include "J1939.h" 
#include "addressClaim.h"
#include "Can.h"
#include "P15765H.h"
#include "TML_14229_UDSH.h"


// constant declarations -----------------------------------------------------


// J1939 PGN List (3 msb = priority
//Transmitted PGN(Not implemented transmite data)
//#define PGN64773            		64773			// J1939 PGN Direct lamp control data 
//#define PGN64773_ext            	0x18FD0500    	// 29 bit ID priority 6
//Received PGN 
// J1939 WATER IN FUEL  
#define PGN65279	              65279U         	
//#define PGN65279_0x00_ext            0x18FEFF00    	// 29 bit ID priority 6
// Gear shiftup/down
//#define PGN65279_0x21                65279         	
//#define PGN65279_0x21_ext            0x18FEFF21    	// 29 bit ID priority 6

// J1939 Aftertreatment 1 Diesel Exhaust Fluid Tank 1 Information 
#define PGN65110                65110U         	
//#define PGN65110_ext            0x18FE5600    	// 29 bit ID priority 6
#define PGN65252                65252U         	// Engine pre Heat 1
//#define PGN65252_ext            0x18FEE400    	// 29 bit ID priority 6 
#define PGN65262				65262U		  	// J1939 coolant Temperature
//#define PGN65262_ext			0x18FEEE00	  	// 29 bit ID priority 6
//Check Engine , ABS Mulfunction,Check Mulfunction and stop engine DM1 
#define PGN65226                65226U        	
//#define PGN65226_ext            0x18FECA00	  	// 29 bit ID priority 6
#define PGN65265	           65265U           //Cruise control
//#define PGN65265_0x00_ext       0x18FEF100	  	// 29 bit ID priority 6
#define PGN65265_0x21           65265U           //Parking break
//#define PGN65265_0x21_ext       0x18FEF100	  	// 29 bit ID priority 6

#define PGN61445                61445U           //Nuteral
//#define PGN61445_ext            0x18F00500      // 29 bit ID priority 6
#define PGN61444                61444U           //RPM
//#define PGN61444_ext            0x0CF00400      // 29 bit ID priority 3 
#define PGN64917                64917U           //Trans. Flud. temp.
//#define PGN64917_ext            0x18FD9500	  	// 29 bit ID priority 6
#define PGN64892                64892U           //DPF and High exhaust system temp.
//#define PGN64892_ext            0x18FD7E00      // 29 bit ID priority 6
#define PGN64775                64775U           //Malfunction indicator
//#define PGN64775_ext            0x18FD0700      // 29 bit ID priority 6
#define PGN65098                65098U           //Check Transmission 
//#define PGN65098_ext            0x18FE4A00      // 29 bit ID priority 6
#define PGN61441                61441U           //Check Transmission 
//#define PGN61441_ext            0x18F00100      // 29 bit ID priority 6
#define PGN64814                64814U           //Check Transmission 
//#define PGN64814_ext            0x0CFD2E00       // 29 bit ID priority 6
#define PGN64972				64972U			// Left/RightTurn
//#define PGN64972_ext            0x0CFDCC00		// 29 bit ID priority 3
#define PGN65089				65089U			// Front and rear lamp
//#define PGN65089_ext            0x0CFE4100		// 29 bit ID priority 3
#define PGN65103				65103U			// Left/RightTurn
//#define PGN65103_ext            0x18FE4F00		// 29 bit ID priority 6
#define PGN65276				65276U			// Fuel Gauge
//#define PGN65276_ext            0x18FEFC00		// 29 bit ID priority 6
#define PGN65269				65269U			// OAT
//#define PGN65269_ext            0x18FEF500		// 29 bit ID priority 6
#define PGN64712				64712U			// Engine Mode Selection
//#define PGN64712_ext            0x18FEF500		// 29 bit ID priority 6
#define PGN61442				61442U			// Engine Mode Selection
//#define PGN61442_ext            0x1CF00200		// 29 bit ID priority 3
#define PGN65263				65263U			// Engine oil Pressure 	

#define PGN61440				61440U			// Retarder Requesting Brake Light 

#define PGN65303				65303U			// TTWA - Proprietery 1CFF1721
#define PGN65102				65102U			// DC1 - Door Open

#define PGN65378				65378U			// VECU - Proprietery 18FF6221

#define PGN64964				64964U			// EBC5

#define PGN64998 				64998U 			//Low Brake fluid


#define PGN60416				60416U //TP_CM 
#define PGN60160				60160U //TP_DT
#define PGN60160_0x03			60160U //TP_DT
#define PGN60160_0x10			60160U //TP_DT
#define PGN60160_0x0B			60160U //TP_DT
#define PGN60160_0x33			60160U //TP_DT



#define PGN65226_0				0xCAU				
#define PGN65226_1				0xFEU
#define PGN65226_2				0U




//Transmmiting PGN
#define PGN65361				65361U			// ODO data,Engine Hour data
#define PGN65361_ext            0x18FF5117U		// 29 bit ID priority 6
// OdoTripA,TRIP A Fuel Eff. Km/L or km/Kg, TripA Fuel Eff. L/hr,HourTripA
#define PGN65362				65362U			
#define PGN65362_ext            0x18FF5217U		// 29 bit ID priority 6
// OdoTripB,TRIP B Fuel Eff. Km/L
#define PGN65363				65363U			
#define PGN65363_ext            0x18FF5317U		// 29 bit ID priority 6
// Fuel level ,DTE,BrakeWear,ClutchWear
#define PGN65364				65364U			
#define PGN65364_ext            0x18FF5417U	// 29 bit ID priority 6
//
#define PGN65365				65365U			
#define PGN65365_ext            0x18FF5517U		// 29 bit ID priority 6
//trip reset DA and SA TBD
#define PGN56832				56832U
#define PGN56832_ext            0x1CDE0017U		// 29 bit ID priority 7
// Air Pressure Front & rear
#define PGN65198				65198U			
#define PGN65198_ext            0x18FEAE17U		// 29 bit ID priority 6

#define PGN65276				65276U
#define PGN65276_ext			0x18FEFC17U


//Source Address macro

#define EMS_SA1_EXT						0x00U
#define BCM_SA_EXT						0x21U
#define ABS_SA_EXT						0x0BU
#define TCU_SA_EXT						0x03U
#define RETARDER_ECU_SA_EXT				0x10U
#define TPMS_SA_EXT						0x33U



#define RTS_TIMEOUT						1500U
#define BAM_TIMEOUT						750U

//ABS malfunction trailer
//Exhaust break PGN TBD	
 
#define BITDATA                     	0xFFU    // indicates bit data
#define SERVICE_INTERVAL            	10U      // service time interval in msec
#define SEND_ON_DEMAND              	0U

typedef enum
{
	ePGN_J1939 = 0U,
	ePGN_NMEA,
	ePGN_FAST_PACKET
} PGN_TYPE;



// local declarations --------------------------------------------------------


typedef enum
{
	eEmsSA = 0U,
	eRetarderEcuSA,
	eAbsSA,
	eTcuSA,
	eTpmsSA,
	NumberOfSA
} eSourceAddType;


typedef enum
{
	eNoNewMultiPacket,
	eTPCMRTS,
	eTPCMBAM,
	eTPDT
}MULTI_PACKET_STATE;

//variables used for multipacket data
// multipacket timestamp holds time of last frame reception
static u32 u32MultiPacketTimer[NumberOfSA];
//indicates if multipacket frames are being received
static u8 u8MultiPacketDM1[NumberOfSA] = {eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket,eNoNewMultiPacket}; 
//holds the address of multipacket being processed
static u8 u8MultiFrameSA[NumberOfSA] = {J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS,J1939_NULL_ADDRESS};	
// number of frames in the multipacket being processed
static u8 u8DM1NumFrames = 0U;
//TRUE if fresh DM1 DTCs have been received
static bool u8DTCready = FALSE;
//source address of the ECU that sent DTCs
static u8 u8DTCSA = J1939_NULL_ADDRESS;
//number of DTCs in DM1
static u8 u8DTCCount = 0U;
static u16 u16lTotalBytes = 0U;



typedef struct
{
	bool	bEnabled;          // not related to address claim
	bool    bSendOnDemand;     // used used for on demand and retry output
	u8      u8data[8];         // Data buffer
	u32     u32TimeStamp;      // Last output tim	
	
} J1939_TX_DATA_TYPE;

typedef struct
{
	u8    u8data[8];
	u32   u32TimeStamp;     	// Time of message reception
} J1939_RX_DATA_TYPE;



typedef struct
{
	u32	PGN_ext;        // J1939 29 bit ID
	u32	timeInterval;   // If != 0 then periodic output interval in msec.
	                    // If == 0, then output is on demand rather than periodic.
} J1939_TX_TYPE;

typedef struct
{
	u8	PGNindex;           // index into sPGNconstantsTx
	u8	byteIndex;          // index into the PGN message data
	u8	byteSize;           // number of bytes of data
	u8	bitMask;            // useful only for bit data
	s16	dataOffset;         // j1939 data offset
	u32	bitRes;             // j1939 bit resolution
	u32	overrangeData;      // data to output if calculation results in a value beyond acceptable range
	u32	underrangeData;      // data to output if calculation results in a value beyond acceptable range
	u32	u32TimeStamp;
} PGNdataType;

#define MAX_OVERRANGE 	20U    // maximum percent overrange

//Can transmit not implement 
typedef struct
{
	u16         PGN;
	u32         ID;         // 29 bit ID
	PGN_TYPE    PGNtype;
	u16         rate;       // in msec
} tPGNconstTxType;                                      


const tPGNconstTxType sPGNconstantsTx[eNoOfPgnTx] = 
{                                                     
		//  PGN         	ID              	PGNtype         rate     	  
		{   PGN65361,   	PGN65361_ext,   	ePGN_J1939,      10000U    },  
		{   PGN65362,   	PGN65362_ext,   	ePGN_J1939,      10000U    }, 
		{   PGN65363,   	PGN65363_ext,   	ePGN_J1939,      10000U    }, 
		{   PGN65364,   	PGN65364_ext,   	ePGN_J1939,      10000U    }, 
		{   PGN65365,   	PGN65365_ext,   	ePGN_J1939,      1000U     },
		{   PGN59904,   	PGN59904_0x00_ext,  ePGN_J1939,      5000U     },
		{   PGN56832,  		PGN56832_ext,		ePGN_J1939,      0U		   },
		{   PGN65198,  		PGN65198_ext,		ePGN_J1939,      1000U     },
		{	PGN65276,		PGN65276_ext,		ePGN_J1939,		 1000U	   },

		
		
};
// The bit resolution in this case is the reciprocal of the
const PGNdataType sPGNdataTx[eNumofTxDataTypes] =                                     // J1939 bit resolution multiplied by 100 for integer math
{                                                                   
		//  PGNIndex    	byteIndex   byteSize    bitMask     dataOffset  bitRes  overrangeData  		
		{   eTx65361,   	0U,          4U,          0x00U,       0U,         	1U, 		0xFFU	},		// ODO data			1.1   
		{   eTx65361,   	3U,          4U,          0x00U,       0U,         	1U, 		0xFFU	},		// Engine Hour data	4.5
		{   eTx65362,   	0U,          3U,          0x00U,       0U,         	1U, 		0xFFU	},		//ODO TripA			1.1
		{   eTx65362,   	3U,          1U,          0x00U,       0U,         	1U, 		0xFFU	},		//TripA Fuel Eff Km	4.1
		{   eTx65362,   	4U,          2U,          0x00U,       0U,         	1U, 		0xFFU	},		//TripA Fuel Eff Hours5.1
		{   eTx65362,   	5U,          3U,          0x00U,       0U,         	1U, 		0xFFU	},		//HourTripA			6.5						
		{   eTx65363,   	0U,          3U,          0x00U,       0U,         	1U, 		0xFFU	},		//OdoTropB			1.1
		{   eTx65363,   	3U,          1U,          0x00U,       0U,         	1U, 		0xFFU	},		//TripB Fuel Eff Km	4.1
		{   eTx65363,   	4U,          2U,          0x00U,       0U,         	1U, 		0xFFU	},		//TripB Fuel Eff Hours5.1
		{   eTx65363,   	5U,          3U,          0x00U,       0U,         	1U, 		0xFFU	},		//HourTripB			6.5
		{   eTx65276,   	1U,          1U,          0x00U,       0U,         	4U, 		0xFFU	},		//Fuel Level %		2 SPN 96 changed from 80
		{   eTx65364,   	1U,          2U,          0x00U,       0U,         	0U, 		0xFFU	},		//DTE				1.1		
		{   eTx65364,   	2U,          1U,          0xFCU,       0U,      BITDATA, 		0xFFU	},		//BreakWear			3.1 //TBD requirement is not clear
		{   eTx65364,   	3U,          1U,          0xFCU,       0U,      BITDATA, 		0xFFU	},		//ClutchWear		4.1	00000011	
		{   eTx65364,   	4U,          3U,          0x00U,       0U,      	1U, 	    0xFFU	},		//Distance TO nex Service 5.1	18 bit
		{   eTx59904,   	0U,          3U,          0x00U,       0U,      	1U, 		0xFFU	},		//AFE request		1-3
		{   eTx56832, 		0U,          1U,          0xFCU,       0U,      BITDATA, 		0xFFU	},		//Reset	SPN 988		1.1 00000011
		{   eTx65198, 		2U,          1U,          0x00U,       0U,      	8U, 		0xFFU	},		//AP Front SPN 1087	3
		{   eTx65198, 		3U,          1U,          0x00U,       0U,      	8U, 		0xFFU	},		//AP Rear SPN 1088	4
		{	eTx65365,		0U,			 1U,		  0xFEU,	   0U,		BITDATA,		0xFFU	},		//SeatBelt 			1.1	
		{	eTx65365,		0U,			 1U,		  0xFDU,	   0U,		BITDATA,		0xFFU	},		//Batt Charging		1.2
		{	eTx65365,		0U,			 1U,		  0xFBU,	   0U,		BITDATA,		0xFFU	},		//Lift Axel Up		1.3
		{	eTx65365,		0U,			 1U,		  0xF7U,	   0U,		BITDATA,		0xFFU	},		//Gen Illumination	1.4
		{	eTx65365,		0U,			 1U,		  0xEFU,	   0U,		BITDATA,		0xFFU	},		//Batt Charging 2	1.5
		{	eTx65365,		0U,			 1U,		  0xDFU,	   0U,		BITDATA,		0xFFU	},		//Tilt Cab			1.6
		{	eTx65365,		0U,			 1U,		  0xBFU,	   0U,		BITDATA,		0xFFU	},		//Air Fltr Clogged	1.7
		{	eTx65365,		0U,			 1U,		  0x7FU,	   0U,		BITDATA,		0xFFU	},		//ABS Malfunction	1.8
		{	eTx65365,		1U,			 1U,		  0xFEU,	   0U,		BITDATA,		0xFFU	},		//Service Indicator	2.1			
		{	eTx65365,		2U,			 2U,		  0x00U,	   0U,			1U,			0xFFU	},		//CapacitiveFuelVolt  3.1
		{   eTx65276,   	6U,          1U,          0x00U,       0U,         	4U, 		0xFFU	},		//Dual Fuel tank Level % SPN 38
		
};


typedef struct
{
	u16		PGN;
	u8      u8SourceAdd;
	void	(*RxFunct)(sJ1939_RX_MSG_TYPE *);    // pointer to PGN processing method
	
} tPGNsonstRxType;

const PGNdataType sPGNdataRx[eNumofRxDataTypes] =
{
	//  PGNindex    byteIndex  byteSize   		bitMask     dataOffset  	bitRes      overrangeData underrange   		In ms            
	{   eRx65279_0x00,	0U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				50000U	},   //  bits 00000011    WaterInFuelLamp    				SPN 97    1.1
	{	eRx65279_0x00,	1U,          1U,          0xF8U,       0,          BITDATA,    BADDATA,			0U,				50000U	},   //  bits 00000111    SCR								SPN 5825  2.1	
	{   eRx65279_0x21,	0U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 00001100    GearShiftUpDown    				SPN 5675  1.3
	{   eRx65252,   	3U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    GLOW Plug   						SPN 1081  4.1		
	{   eRx65262,   	0U,          1U,          0x00U,      -40,         1U,	 	   TEMP_OVR_RANGE,TEMP_UND_RANGE,	5000U	},   //  1 deg C/bit      CoolantTemp      	        		SPN 110    1
	{   eRx65110,   	0U,          1U,          0x00U,       0,          1U,   	   BADDATA,			0U,				5000U	},   //  0.4%/bit         DEF Gauge 	      	        	SPN 1761   1 Def have .4 resolution but we are using direct j1939 data	
	{   eRx61445,   	3U,          1U,          0x00U,       0,          1U,     	   BADDATA,			0U,				500U	},   //  1 gear value/bit Nuetral          	        		SPN 523    4
	{   eRx65088,   	1U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    FrontRearLamp			    		SPN 2388   2.1
	{   eRx65088,   	4U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    FrontRearLamp		       			SPN 2390   5.1
	{   eRx65265_0x00, 	6U,          1U,          0xE0U,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 00011111    PowerTakeOff          	    	SPN 976    7.1 5bit
	{   eRx65265_0x00, 	3U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 00000011    CruiseControl          			SPN 595    4.1
	{   eRx64892,   	0U,          1U,          0xF8U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000111    DPF          	            		SPN 3697   1.1
	{   eRx65226,   	0U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    CHECK ENGINE      	        	SPN 624    1.3
	{   eRx65226,   	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    CHECK ENGINE FLASH        		SPN 3040   2.3 
	{   eRx65226,   	0U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11000000    MIL          	        			SPN 1213   1.7   
	{   eRx65226,   	1U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //	 bits 11000000    MIL FLASH  	      	    		SPN 3038   2.7
	{   eRx65226,   	2U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc SPN1          	        	   
	{   eRx65226,   	3U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc SPN2         	        	   
	{   eRx65226,   	4U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   //  bytes 2-4        dtc fmi          	        	   
	{   eRx64775,   	0U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11000000    MIL J1939  	      	    		SPN 5080   1.7
	{   eRx61444,   	3U,          2U,          0x00U,       0,          125U,   	   BADDATA_2BYTE,	0U,				5000U	},   //	 0.125 rpm/bit     RPM  	      	        		SPN 190    4-5
	{   eRx64917,   	0U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  00110000          Trance Fluid Temp      			SPN 5345    1.5
	{   eRx64892,   	6U,          1U,          0xE3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  00011100          HEST                				SPN 3698   7.3
	{   eRx64892,   	2U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    DPF ACTIVE REGEN INhibit			SPN 3702 	3.1-3.2
	{   eRx64892,   	2U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    DPF Act. Regen. INH due to switch	SPN 3703 	3.3-3.4
	{   eRx64892,   	2U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    DPF Act. Regen. INH due to switch	SPN 3704 	3.5-3.6
	{   eRx64892,   	2U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11000000    DPF Act. Regen. INH due to switch	SPN 3705 	3.7-3.8	
	{   eRx64892,   	3U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    DPF Act. Regen. INH due to switch	SPN 3707 	4.3-4.4
	{   eRx64892,   	3U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    DPF Act. Regen. INH due to switch	SPN 3708 	4.5-4.6
	{   eRx64892,   	4U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00000011    DPF Act. Regen. INH due to switch	SPN 3710 	5.1-5.2
	{   eRx64892,   	4U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    DPF Act. Regen. INH due to switch	SPN 3711 	5.3-5.4
	{   eRx64892,   	4U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    DPF Act. Regen. INH due to switch	SPN 3712 	5.5-5.6
	{   eRx64892,   	5U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    DPF Act. Regen. INH due to switch	SPN 3716 	6.5-6.6
	{   eRx64892,   	6U,          1U,          0x1FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 11100000    DPF Act. Regen. INH Forced Status	SPN 4175 	7.6-7.8
	{   eRx64892,   	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    DPF Active Regeneration Status	SPN 3700 	2.3-2.4
	{   eRx65110,   	4U,          1U,          0x1FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  11100000          Low Def Level      	    		SPN 5245    5.6
	{   eRx65226,   	0U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    StopEngine       	        		SPN 623    1.5
	{   eRx65226,   	1U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    StopEngine       	        		SPN 3039   2.5 
	{   eRx65098,   	4U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,		   		500U	},   // 00001100            transmission mode 6				SPN 4251    5.3
	{   eRx65098,   	4U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,		   		500U	},   // 00000011            creep							SPN 4250    5.1
	{   eRx65098,   	2U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,		   		500U	},   // 00110000            ECO/ power						SPN 2537    3.5
	{   eRx65098,   	2U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,		   		500U	},   // 11000000			Auto/Manual						SPN 2536    3.7
	{   eRx65098,   	2U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,		   		500U	},   // 00001100			Crawl							SPN 2538    3.3
	{   eRx65378,   	0U,          1U,          0xF8U,       0,          BITDATA,    BADDATA,			0U,		   		2500U	},   // 00000111			Left/Right Turn					VECU - Proprietery    1.1 3Bit data	
	{   eRx65088,   	0U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,		   		5000U	},   // 11000000			High beam						SPN 2348    1.7 2Bit data
	{	eRx65378,		0U,			 1U,		  0x3FU,	   0,		   BITDATA,	   BADDATA,			0U,				2500U	},   // 11000000    		Trailer 						VECU - Proprietery	1.7 2Bit data	  
	{	eRx65265_0x00,	0U,			 1U,		  0xF3U,	   0,		   BITDATA,	   BADDATA,			0U,				500U 	},   // 00001100    		ParkingBreak					SPN 70    	1.3 2Bit data
	{	eRx64775,		1U,			 1U,		  0xCFU,	   0,		   BITDATA,	   BADDATA,			0U,				5000U	},   //	00110000       		Low Oil Lamp					SPN 5082	2.5 2Bit data
	{	eRx65103,		0U,			 1U,		  0xF0U,	   0,		   BITDATA,	   BADDATA,			0U,				500U 	},   //	00000011       		ESC Mil						SPN 1813+1814	1.1 4Bit data
	{	eRx61441,		7U,			 1U,		  0x3FU,	   0,		   BITDATA,	   BADDATA,			0U,				500U 	},   //	11000000    		ABS Trailer						SPN 1792  	8.7	2Bit data
	{	eRx65219,		0U,			 1U,		  0xF3U,	   0,		   BITDATA,	   BADDATA,			0U,				5000U	},   //	00001100    		Trans Range Low					SPN 779  	1.3	2Bit data
	{	eRx64775,		1U,			 1U,		  0xFCU,	   0,		   BITDATA,	   BADDATA,			0U,				5000U	},   // 00000011 			Exhaust Break					SPN 5081   	2.1	2Bit Data
	{	eRx65257,		4U,			 4U,		  0x00U,	   0,			5U,		   BADDATA_4BYTE,	0U,				25000U	},   // 00000000 			AFE								SPN 250   	5-8	4 Byte data
	{	eRx65276,		1U,			 1U,		  0x00U,	   0,			4U,		   BADDATA,			0U,				5000U	},   // 00000000 			Fuel gauge						SPN 96  	2 , 1 Byte data
	{   eRx60160,   	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00001100    CHECK ENGINE      	        		SPN 624    2.3
	{   eRx60160,   	2U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00001100    CHECK ENGINE FLASH        			SPN 3040   3.3
	{   eRx60160,   	1U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00110000    StopEngine       	        		SPN 623    2.5
	{   eRx60160,   	2U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00110000    StopEngine flash  	        		SPN 3039   3.5	
	{   eRx60160,   	1U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 11000000    MIL          	        			SPN 1213   2.7   
	{   eRx60160,   	2U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 11000000    MIL FLASH  	      	    		SPN 3038   3.7
	{   eRx65226_0x03, 	0U,          1U,          0xC3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00111100    CHECK Trance|Flash Rate       	 SPN 624|623   1.3
	{   eRx60160_0x03,	1U,          1U,          0xC3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00111100    DM1CHECK Trance|Flash Rate    	 SPN 624|623   2.3
	{   eRx61445_0x03,  3U,          1U,          0x00U,       0,        	1U,        BADDATA,			0U,				500U	},   // 1 gear value/bit Nuetral          	        		SPN 523    4
	{   eRx61445_0x21, 	3U,          1U,          0x00U,       0,        	1U,        BADDATA,			0U,				500U	},   //  1 gear value/bit Nuetral          	        		SPN 523    4
	{   eRx65269,   	3U,          2U,          0x00U,    -273,          3128U,      BADDATA_2BYTE,	OAT_UND_RANGE,	5000U	},   // Res 0.03125 , Offset -273C   OAT  	   	        	SPN 171    4-5
	{   eRx64712,   	6U,          1U,          0x00U,       0,          4U,   	   BADDATA,			0U,				5000U	},   //  0.4%/bit         Engine MOde Selection   	        SPN 10297   7
	{   eRx61442,	 	1U,          2U,          0x00U,       0,          125U,   	   BADDATA_2BYTE,	0U,				5000U	},   //  bits 00000011    SPEEDO          					SPN 191		4.1
	{   eRx65263, 		3U,          1U,          0x00U,       0,          1U,	   	   BADDATA,			0U,				2500U	},   //  byte 4    		  Engine oil pressure          		SPN 100		4 //SPN 100 have 4 resolution but we are using direct j1939 data so keep resolution 1
	{   eRx61440, 		3U,          1U,          0xF3U,       0,          BITDATA,	   BADDATA,			0U,				500U 	},   //  byte 4    		  Retarder Requesting Brake Light	SPN 1667	4.3
	{	eRx61441,		5U,			 1U,		  0xCFU,	   0,		   BITDATA,	   BADDATA,			0U,				500U 	},   //	11000000    	  ABS / EBS amber warning state  	SPN 1438  	6.5	2Bit data		
	{   eRx60160_0x03, 	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00001100    	TPDT		  	        		SPN 2572     1
	{   eRx65226_0x03, 	2U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   // Bytes 2,3,  		LEARN FAIL DM1 
	{   eRx65226_0x03, 	3U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   // Bytes 4,    		LEARN FAIL DM1  
	{   eRx65226_0x03, 	4U,          1U,          0x00U,       0,          1U ,  	   BADDATA_4BYTE,	0U,				5000U	},   // Bytes 4,    		LEARN FAIL DM1 
	{   eRx65378, 		1U,          1U,          0xFCU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   // Bytes 2, bits 0000011 ABS Trailer						VECU - Proprietery 2.1 2 bit
	{   eRx65098,   	1U,          1U,          0x3FU,       0,          BITDATA,    BADDATA,			0U,		   		1000U	},   // 11000000            Shift inhibit					SPN 1851    2.7
	{   eRx64775,   	0U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100    Check Engine DLCC1  	      	    		SPN 5078   1.3
	{   eRx64775,   	0U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    Stop Engine DLCC1  	      	    		SPN 5079   1.5
	{   eRx65303,   	3U,          1U,          0xFBU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000100    Air filter clogging 					TTWA - Proprietery 4.3 1 bit
	{   eRx65303,   	3U,          1U,          0xBFU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 01000000    Battery Charging1						TTWA - Proprietery 4.7 1 bit
	{   eRx65303,   	3U,          1U,          0x7FU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 10000000    Battery Charging2						TTWA - Proprietery 4.8 1 bit
	{   eRx65378, 		1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				2500U	},   // Bytes2 , bits 00001100 Lift Axel fault						VECU - Proprietery 2.3 2 bit
	{   eRx65378, 		4U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00001100    Battery Weak 2						VECU - Proprietery 5.3 2 bit
	{   eRx60160_0x33, 	1U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   // bits 00001100    TPMS DM1 Multi Fram       				SPN 624    2.3
	{   eRx65226_0x33, 	0U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00001100   TPMS DM1 SingleFram       	        	SPN 624    1.3
	{   eRx65303,   	1U,          1U,          0xFEU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000001    halt Request						TTWA - Proprietery 1 bit
	{   eRx65303,   	1U,          1U,          0xFDU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000010    ramp Engaged						TTWA - Proprietery 1 bit
	{   eRx65303,   	1U,          1U,          0xFBU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000100    kneeling front					TTWA - Proprietery 1 bit
	{   eRx65303,   	1U,          1U,          0xF7U,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00001000    kneeling rear						TTWA - Proprietery 1 bit
	{   eRx65303,   	1U,          1U,          0xEFU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00010000    kneeling both						TTWA - Proprietery 1 bit
	{   eRx65303,   	1U,          1U,          0xDFU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00100000    engine oil low					TTWA - Proprietery 1 bit
	{   eRx65303,   	2U,          1U,          0xFDU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000010    engine coolant low				TTWA - Proprietery 1 bit
	{   eRx65303,   	2U,          1U,          0xFBU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000100    emergency exit					TTWA - Proprietery 1 bit
	{   eRx65303,   	2U,          1U,          0xEFU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00010000    halt request priority				TTWA - Proprietery 1 bit
	{   eRx65303,   	3U,          1U,          0xFEU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000001    steering adjust					TTWA - Proprietery 1 bit
	{   eRx65303,   	3U,          1U,          0xFDU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000001    Buzzer Enable /Disable					TTWA - Proprietery 1 bit
	{   eRx65303,   	4U,          1U,          0xFEU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000001    rear flap open					TTWA - Proprietery 1 bit
	{   eRx65303,   	4U,          1U,          0xFDU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000010    side flap open					TTWA - Proprietery 1 bit
	{   eRx65303,   	4U,          1U,          0xFBU,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00000100    kneeling right					TTWA - Proprietery 1 bit
	{   eRx65303,   	4U,          1U,          0xF7U,       0,          BITDATA,    BADDATA,			0U,				2500U	},   //  bits 00001000    kneeling left 					TTWA - Proprietery 1 bit
	{   eRx65102,   	0U,          1U,          0xF0U,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 00001111    Door Open		 					DC1	SPN 1821	   1.1 4 bits
	{   eRx64964, 		0U,          1U,          0x1FU,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 11100000   EBC5 HILL HOLDER				    SPN 2912   1.6-1.8
	{   eRx65303_0xE1, 	0U,          4U,          0x00U,       0,          1U,  	   BADDATA_4BYTE,	0U,				5000U	},   //  IIL Proprietery for fault Diganostic						
	{   eRx64775,   	2U,          1U,          0xCFU,       0,          BITDATA,    BADDATA,			0U,				5000U	},   //  bits 00110000    Airfilter clogged    	    		SPN 5086   3.5
	{   eRx65378, 		5U,          2U,          0x00U,       0,          10U,        0x3FE,		    0U,				2500U	},   //  Bytes 2, 10 bits					VECU - Proprietery 6.1 Capacitive fuel sensor voltage -AFT2						
	{   eRx64998,   	3U,          1U,          0xF3U,       0,          BITDATA,    BADDATA,			0U,				500U	},   //  bits 00001100    Low Brake fluid    	    		SPN 2931   4.3
	
};

sJ1939_RX_MSG_TYPE   sJ1939RxQueue[RX_QUE_SIZE];
sJ1939_RX_MSG_TYPE   J1939RxData[eNoOfPgnRx]; 
J1939_TX_TYPE        J1939_Tx[eNoOfPgnTx];
 DTCdata DTC_J1939[NO_OF_DTC_CODE];
u8 u8DtcFramNum = 0U;
// Create an array corresponding to sPGNconstantsTx above for the PGN data
J1939_TX_DATA_TYPE  J1939TxData[eNoOfPgnTx]; 
static bool bSpnDataReadyStatus[eNumofRxDataTypes];
static u32 u32DtcFrameData =0;

// Private prototypes --------------------------------------------------------

/// @addtogroup J1939_LIBRARY
/// @{
 
/// @defgroup J1939Lib Private functions used in J1939 File
//// 
///   
/// @addtogroup J1939Lib
/// @{

static void _vPGN65279_0x00_Data(sJ1939_RX_MSG_TYPE *msg); 
static void _vPGN65279_0x21_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65252_Data(sJ1939_RX_MSG_TYPE *msg); 
static void _vPGN65110_DEF(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61445_Nuteral(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65088_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65265_0x00(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65265_0x00_ParkBreak(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64892_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65226_Data(sJ1939_RX_MSG_TYPE *msg); 
static void _vPGN64775_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61444_RPM(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65262_CooleantTemp(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65098_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64917_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61441_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65103_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65219_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65257_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65276_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN60160_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN60160_DM1_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN60160_DM1_0x03_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN60160_DM1_0x33_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65226_0x03_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61445_0x03_Nuteral(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61445_0x21_ManualGear(sJ1939_RX_MSG_TYPE *msg);

static void _vPGN65226_0x33_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65269_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64712_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61442_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65263_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN61440_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65303_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65102_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65378_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64964_Data(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN65303_IILPropData(sJ1939_RX_MSG_TYPE *msg);
static void _vPGN64998_Data(sJ1939_RX_MSG_TYPE *msg);





static void _vResetDM1Reception(u8 SourceAdd);
static void _J1939_RxManager(void);
static void _J1939_TxManager(void);

/// @} // endgroup J1939Lib

/// @} // endgroup J1939_LIBRARY


// prototype dependent definitions ------------------------------------------ 
// This needs to be defined after prototype definitions because they are used in this structure
const tPGNsonstRxType sPGNconstantsRx[eNoOfPgnRx] =
{
	//  PGN                 		RxFunct             				
	{   PGN65279,	    0x00U,          _vPGN65279_0x00_Data }, 	
	{   PGN65279,		0x21U,          _vPGN65279_0x21_Data }, 
	{   PGN65252,		0x00U,          _vPGN65252_Data }, 		    			
	{   PGN65110,		0x00U,          _vPGN65110_DEF },
	{   PGN61445,		0x00U,          _vPGN61445_Nuteral },		  
	{   PGN65088,		0x21U,          _vPGN65088_Data },		
	{   PGN65265,   	0x00U,          _vPGN65265_0x00 },
	{   PGN64892,		0x00U,          _vPGN64892_Data }, 		 
	{   PGN64775,		0x00U,          _vPGN64775_Data }, 
	{   PGN61444,		0x00U,          _vPGN61444_RPM }, 
	{   PGN65262,		0x00U,          _vPGN65262_CooleantTemp }, 
	{   PGN65098,		0x03U,          _vPGN65098_Data }, 
	{   PGN64917,		0x03U,          _vPGN64917_Data }, 
	{   PGN61441,		0x0BU,          _vPGN61441_Data },
	{   PGN65103,   	0x0BU,      	_vPGN65103_Data },
	{   PGN65219,   	0x21U,      	_vPGN65219_Data },
	{   PGN65257,   	0x00U,      	_vPGN65257_Data },
	{   PGN65276,   	0x00U,      	_vPGN65276_Data },
	{   PGN60160,   	0x00U,      	_vPGN60160_DM1_Data },
	{   PGN60160,  		0x03U,      	_vPGN60160_DM1_0x03_Data },
	{   PGN60160,  		0x33U,      	_vPGN60160_DM1_0x33_Data },
	{   PGN65226,		0x00U,          _vPGN65226_Data },
	{   PGN65226,		0x03U,          _vPGN65226_0x03_Data 	},
	{   PGN65226,		0x33U,          _vPGN65226_0x33_Data 	},
	{   PGN61445,		0x03U,          _vPGN61445_0x03_Nuteral },	// for AMT Source Address 0x03	
	{   PGN61445,		0x21U,          _vPGN61445_0x21_ManualGear },	// for Manual Gear Source Address 0x21
	{   PGN65269,		0x00U,          _vPGN65269_Data },
	{   PGN64712,		0x00U,          _vPGN64712_Data },
	{   PGN61442,		0x03U,          _vPGN61442_Data },
	{   PGN65263,		0x21U,          _vPGN65263_Data },
	{   PGN61440,		0x10U,          _vPGN61440_Data },
	{   PGN65303,		0x21U,          _vPGN65303_Data },
	{   PGN65102,		0x21U,          _vPGN65102_Data },
	{   PGN65378,		0x21U,          _vPGN65378_Data },
	{   PGN64964,		0x0BU,          _vPGN64964_Data },
	{   PGN65303,		0xE1U,          _vPGN65303_IILPropData },
	{   PGN64998,		0x0BU,          _vPGN64998_Data },
};
	

/*********************************************************************//**
 *
 * J1939 services main loop
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vJ1939_ServiceManager(void)
{
	static u32 serviceTime = 0U;

	if ((u32GetClockTicks() - serviceTime) >= SERVICE_INTERVAL)
	{
		// Service the J1939 received messages
		_J1939_RxManager();

		// Service the J1939 transmit messages		
		//_J1939_TxManager();

		// Reset the timer
		serviceTime = u32GetClockTicks();
	}
}


/*********************************************************************//**
 *
 * Initialize receive and transmit message structures. Set all PGN
 * message data to invalid.
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vJ1939Init(void)
{
	s16 i, j;

	// Initialize the receive queue
	for (i = 0U; i < RX_QUE_SIZE; ++i)
	{
		sJ1939RxQueue[i].bDataRdy = FALSE;
	}
	// Initialize the Tx data structure
	for (i = 0; i < eNoOfPgnTx; ++i)
	{
		J1939TxData[i].bEnabled = FALSE;
		J1939TxData[i].bSendOnDemand = FALSE;
		J1939TxData[i].u32TimeStamp = u32GetClockTicks();
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		J1939TxData[i].u8data[j] = 0xFFU;
		
	}
		
	// Initialize the Rx data structure
	for (i = 0U; i < eNoOfPgnRx; ++i)
	{
		J1939RxData[i].u32TimeStamp = u32GetClockTicks();
		J1939RxData[i].bDataOutDate = TRUE;
		J1939RxData[i].bDataRdy = FALSE;
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		{
			J1939RxData[i].u8data[j] = 0xFFU;
		}
		
	}
	for (j = 2U; j < 8U; ++j)	// Reinit of last six bytes of 65526 SA 0x00 so that DTC 65535 is never seen in DTC menu
	{
		J1939RxData[eJDM1DTC_spn1].u8data[j] = 0x00U;	
	}
}

/*********************************************************************//**
 *
 * Tx message buffer initialization
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vJ1939TxInit(void)
{
s16 i, j;
// Initialize the Tx data structure
	for (i = 0U; i < eNoOfPgnTx; ++i)
	{
		J1939TxData[i].bEnabled = TRUE;
		J1939TxData[i].bSendOnDemand = FALSE;
		J1939TxData[i].u32TimeStamp = u32GetClockTicks();
		// Set all data invalid
		for (j = 0U; j < 8U; ++j)
		{
			J1939TxData[i].u8data[j] = 0xFFU;
		}
		
	}
	
}

/*********************************************************************//**
 *
 * Processs all interested received J1939 messages
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _J1939_RxManager(void)
{
	u32 PGN,ID;	
	sJ1939_RX_MSG_TYPE *msg,*UDSbufPtr,UDSMsg;	   
   	u16  RxQuePush;
	u16 i, timeNow;
	u8 RxQuePop;
	u8 SOURCEADD;
	u8  framNum, frameIndex = 0U;	
	u8 SourceAddType = 0U;

	u8DtcFramNum = 0U;
	
	// Check the entire input queue for fresh messaages
	for (RxQuePop = 0U; RxQuePop < RX_QUE_SIZE; ++RxQuePop)
	{
		if (sJ1939RxQueue[RxQuePop].bDataRdy)
		{
			// Process fresh message
			msg = &sJ1939RxQueue[RxQuePop];
			UDSMsg = sJ1939RxQueue[RxQuePop];

			// Extract the PGN
			ID = msg->sMsgID.Identifier;
			
			PGN = (msg->sMsgID.Identifier >> 8) & 0x0003FFFFU;
			SOURCEADD = (msg->sMsgID.Identifier) & 0x000000FFU;
//****************Dignostic message *********************************

		//both diagnostics and pass through messages
		   if((UDSMsg.sMsgID.sIdField.SourceAddress == REQ_PHYSICAL_SA1 || 
			   UDSMsg.sMsgID.sIdField.SourceAddress == REQ_PHYSICAL_SA2)
			   && 
			   (UDSMsg.sMsgID.sIdField.PduSpecific == PREFERRED_SA || //0x1BDA17F1/F9
			   UDSMsg.sMsgID.sIdField.PduSpecific == REQ_FUNCTIONAL_SA) //0x1BDB33F1/F9
			)
		   {
			   // Find an available queue slot
			   for (RxQuePush = 0U;
			   (RxQuePush < RX_QUE_SIZE) && (diagnosticsBuf[RxQuePush].bDataRdy == TRUE);
			   ++RxQuePush)
			   	{
			   	 //*** DO NOTHING ***
			   	}
			   if( RxQuePush < RX_QUE_SIZE )
			   {
				   //UDSbufPtr = &diagnosticsBuf[RxQuePush];

				   diagnosticsBuf[RxQuePush].sMsgID.Identifier = ((sJ1939RxQueue[RxQuePop].sMsgID.Identifier)  & 0xffff0000U);
					for(i= 0U;i < 8U; i++)
					{
				    	diagnosticsBuf[RxQuePush].u8data[i] = sJ1939RxQueue[RxQuePop].u8data[i];
					}
					diagnosticsBuf[RxQuePush].u32TimeStamp = sJ1939RxQueue[RxQuePop].u32TimeStamp;
					diagnosticsBuf[RxQuePush].ucNumofRxBytes = sJ1939RxQueue[RxQuePop].ucNumofRxBytes;
				   diagnosticsBuf[RxQuePush].bDataRdy = TRUE;

				   
				   diagnosticsTimer = u32GetClockTicks();
				   testerAddress = UDSMsg.sMsgID.sIdField.SourceAddress;
			   }
		   }
	  

			//done only for diagnostics messages
			//	UDSbufPtr->sMsgID.Identifier = ID & 0xffff0000;
    //*****************************************************************
			if(commInfo.networkCommRxEnabled == TRUE)
			{

				if(SOURCEADD == EMS_SA1_EXT)
				{
					SourceAddType = eEmsSA;
				}
				else  if(SOURCEADD == ABS_SA_EXT)
				{
					SourceAddType = eAbsSA;
				}
				else  if(SOURCEADD == TCU_SA_EXT)
				{
					SourceAddType = eTcuSA;
				}
				else  if(SOURCEADD == RETARDER_ECU_SA_EXT)
				{
					SourceAddType = eRetarderEcuSA;
				}
				else  if(SOURCEADD == TPMS_SA_EXT)
				{
					SourceAddType = eTpmsSA;
				}
				else
				{
					// ***	 DO NOTHING ***
				}


				switch (PGN & 0xFF00U)
				{
					case PGN60416 :
						//accept only if its TP.CM_BAM 
						if(	(PGN & 0x00FF) == J1939_GLOBAL_ADDRESS 
							&& sJ1939RxQueue[RxQuePop].u8data[0] == 32U )
						{
							//is the PGN DM1 from relevant ECUs
						  	if( sJ1939RxQueue[RxQuePop].u8data[5] == PGN65226_0 
								&& sJ1939RxQueue[RxQuePop].u8data[6] == PGN65226_1
								&& sJ1939RxQueue[RxQuePop].u8data[7] == PGN65226_2
								&& ( (SOURCEADD == EMS_SA1_EXT) || (SOURCEADD == RETARDER_ECU_SA_EXT)
									|| (SOURCEADD == ABS_SA_EXT) || (SOURCEADD == TCU_SA_EXT)
									|| (SOURCEADD == TPMS_SA_EXT) ) )
							{
								//to track time to stop waiting for TP_DT
								u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
								u8MultiPacketDM1[SourceAddType] = eTPCMBAM;
								u8MultiFrameSA[SourceAddType] = SOURCEADD;

								//DTCs will be read from EMS and TCU only if the previous DTCs 
								//from either ECUs have been processed
								if( u8DTCready == FALSE 
									&& (u8MultiFrameSA[SourceAddType] == EMS_SA1_EXT /*|| u8MultiFrameSA[SourceAddType] == TCU_SA_EXT*/) 
									)
								{
									u8DTCSA = u8MultiFrameSA[SourceAddType];
									u8DM1NumFrames = sJ1939RxQueue[RxQuePop].u8data[3];
									u16lTotalBytes = sJ1939RxQueue[RxQuePop].u8data[2];
									u16lTotalBytes = (u16lTotalBytes<<8) | sJ1939RxQueue[RxQuePop].u8data[1];
								}
							}
								
						}
						break;
					case PGN60160 :
						//accept only if its TP.DT from the relevant SA
						if( (PGN & 0x00FF) == J1939_GLOBAL_ADDRESS
							&& u8MultiFrameSA[SourceAddType] != J1939_NULL_ADDRESS
							&& u8MultiFrameSA[SourceAddType] == SOURCEADD )
						{
							// accept only if this frame is continued from same dataset
							if((u32GetClockTicks() - u32MultiPacketTimer[SourceAddType]) < BAM_TIMEOUT)
							{
								//if its a first frame
								if( (sJ1939RxQueue[RxQuePop].u8data[0] == 0x01 ) && (u8MultiPacketDM1[SourceAddType] == eTPCMBAM ) )
								{
									u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
									//DTCs are read only from EMS
									if( u8DTCSA == u8MultiFrameSA[SourceAddType] )
									{
										u8DtcFramNum = 1;
										u8MultiPacketDM1[SourceAddType] = eTPDT;
										DTC_J1939[0].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[3];
										DTC_J1939[0].SPNValue |= ((u32)sJ1939RxQueue[RxQuePop].u8data[4] << 8);
										DTC_J1939[0].SPNValue |= ( (u32)(sJ1939RxQueue[RxQuePop].u8data[5] & 0xE0) << 11);
										DTC_J1939[0].FMICode = sJ1939RxQueue[RxQuePop].u8data[5] & 0x1F;
										u32DtcFrameData = sJ1939RxQueue[RxQuePop].u8data[7]; // DTC_J1939[1].SPNValue
									}
									else
									{
										_vResetDM1Reception(SourceAddType);
									}
								
								}
								else if(u8MultiPacketDM1[SourceAddType] == eTPDT )
								{
									u32MultiPacketTimer[SourceAddType] = u32GetClockTicks();
									//so that these frame is rejected for DM1 controlled lamps
									SOURCEADD = J1939_NULL_ADDRESS;
								
									framNum = sJ1939RxQueue[RxQuePop].u8data[0];
									u8DtcFramNum = framNum;
									
									//if this is the last frame
									if(framNum == u8DM1NumFrames )
									{
										u8DTCCount = (u16lTotalBytes - 2U)/4U;
										_vResetDM1Reception(SourceAddType);
										//this flag should be set to false when DTCs have been processed
										u8DTCready = TRUE;
									}
									frameIndex = 0U;
									
									// max num byte in TP.CM is 255 : 255/7 ~ 36 frames maximum
									if(framNum > 36U )
									{
										framNum = 36U;
									}
									
									if(framNum > 5U )
									{
										// calculating the index to extract DTC; the pattern repeats after every 4 frames
										frameIndex = (framNum - 2U) / 4U ;
										framNum = framNum - (frameIndex * 4U);
										frameIndex = frameIndex * 7U;
									}
								   
									switch(framNum)
									{
										case 0x2U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 1U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 1U].SPNValue = u32DtcFrameData;
												DTC_J1939[frameIndex + 1U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[1] << 8;
												DTC_J1939[frameIndex + 1U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[2] & 0xE0) << 11);
												DTC_J1939[frameIndex + 1U].FMICode = sJ1939RxQueue[RxQuePop].u8data[2] & 0x1F;
											}
											if( (frameIndex + 2U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 2U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[4];
												DTC_J1939[frameIndex + 2U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[5] << 8;
												
												DTC_J1939[frameIndex + 2U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[6] & 0xE0) << 11);
												DTC_J1939[frameIndex + 2U].FMICode = sJ1939RxQueue[RxQuePop].u8data[6] & 0x1F;
											}
										}
										break;
										
										case 0x3U:
										{
											//should not be more than number of DTCs supported
											if( (frameIndex + 3U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 3U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[1];
												DTC_J1939[frameIndex + 3U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[2] << 8;
												DTC_J1939[frameIndex + 3U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[3] & 0xE0) << 11);
												DTC_J1939[frameIndex + 3U].FMICode = sJ1939RxQueue[RxQuePop].u8data[3] & 0x1F;
											}

											if( (frameIndex + 4U) < NO_OF_DTC_CODE)
											{
												//Change sequence for remove error 											
												DTC_J1939[frameIndex + 4U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[5];
												DTC_J1939[frameIndex + 4U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[6] << 8;
												DTC_J1939[frameIndex + 4U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[7] & 0xE0) << 11);
												DTC_J1939[frameIndex + 4U].FMICode = sJ1939RxQueue[RxQuePop].u8data[7] & 0x1F;
																							
											}
										}
										break;
										
										case 0x04U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 5U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 5U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[2];
												DTC_J1939[frameIndex + 5U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[3] << 8;
												DTC_J1939[frameIndex + 5U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[4] & 0xE0) << 11);
												DTC_J1939[frameIndex + 5U].FMICode = sJ1939RxQueue[RxQuePop].u8data[4] & 0x1F;
											}
											if( (frameIndex + 6U) < NO_OF_DTC_CODE)
											{ 
												DTC_J1939[frameIndex + 6U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[6];   // 
												DTC_J1939[frameIndex + 6U].SPNValue |= ((u32)sJ1939RxQueue[RxQuePop].u8data[7] << 8);
											}
										}
										break;
										
										case 0x05U:
										{
											//should not be more than number of DTCs supported
											if((frameIndex + 6U) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 6U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[1] & 0xE0) << 11);
												DTC_J1939[frameIndex + 6U].FMICode = sJ1939RxQueue[RxQuePop].u8data[1] & 0x1F;
											}
											if( (frameIndex + 7) < NO_OF_DTC_CODE)
											{
												DTC_J1939[frameIndex + 7U].SPNValue = (u32)sJ1939RxQueue[RxQuePop].u8data[3];
												DTC_J1939[frameIndex + 7U].SPNValue |= (u32)sJ1939RxQueue[RxQuePop].u8data[4] << 8;
												DTC_J1939[frameIndex + 7U].SPNValue |= ((u32)(sJ1939RxQueue[RxQuePop].u8data[5] & 0xE0) << 11);
												DTC_J1939[frameIndex + 7U].FMICode = sJ1939RxQueue[RxQuePop].u8data[5] & 0x1F;
											}
											if( (frameIndex + 8U) < NO_OF_DTC_CODE)
											{
												u32DtcFrameData = sJ1939RxQueue[RxQuePop].u8data[7];//
												 
											}
											
										}
										break;
										
									}
								}
								// if the first frame was missed, prepare for next multipacket
								else
								{
									_vResetDM1Reception(SourceAddType);
									//so that these frame is rejected for DM1 controlled lamps
									SOURCEADD = J1939_NULL_ADDRESS;
								}
							}
							//if BAM timeout, prepare for next multipacket
							else
							{
								if(u8MultiPacketDM1[SourceAddType] == eTPDT )
									u8DTCready = TRUE;
								_vResetDM1Reception(SourceAddType);	
								//so that these frame is rejected for DM1 controlled lamps
								SOURCEADD = J1939_NULL_ADDRESS;
								
							}
						}
						else
						{
							//so that these frames are rejected for DM1 controlled lamps
							SOURCEADD = J1939_NULL_ADDRESS;
						}	
						break;
				}

		//*****************************************************************


					// if message is valid
					if ((msg->sMsgID.sIdField.PduFormat <= J1939_MAX_PF_PDU1FORMAT)
					// and it's for me
					&& ((msg->sMsgID.sIdField.PduSpecific == ucGetMyClaimedAddress())
					// or a broadcast message
					|| (msg->sMsgID.sIdField.PduSpecific == J1939_GLOBAL_ADDRESS)))
					{
						// Remove destination address from specific PGN number
						PGN ^= msg->sMsgID.sIdField.PduSpecific;

						if (bIsAddressClaimed())
						{
						// Special processing for these PGNs if already claimed an address
							switch (PGN)
							{
								case PGN59904 :
									// This request has come in after successful address claim
									// We may be getting bumped
									vRxISOrequest(&sJ1939RxQueue[RxQuePop]);
									if (bHasMyClaimedAddressedBeenBumped())
									{
									// Pause communications
									timeNow = u16GetClockTicks();
									while ((u16GetClockTicks() - timeNow) < 250U)
									R_WDT_Restart();//WDog1_Clear();
									vClearAddressClaimBumped();
									}
								break;

								case PGN60928 :
									vRxISOaddressClaim(&sJ1939RxQueue[RxQuePop]);
								break;
							}
						}
						else if (PGN == PGN59904)
						{
							// This request may be for an address claim
							vRxISOrequest(&sJ1939RxQueue[RxQuePop]);
						}
					}
					// Check all other PGN inputs
					for(i=0U; i < eNoOfPgnRx; ++i)
					{
						if(PGN == sPGNconstantsRx[i].PGN && SOURCEADD == sPGNconstantsRx[i].u8SourceAdd )
						{	
							sPGNconstantsRx[i].RxFunct(msg);
						}						
					}
					
			}//end if(commInfo.networkCommRxEnabled == TRUE)
			// Clear the queue slot
					sJ1939RxQueue[RxQuePop].bDataRdy = FALSE;
		}// end if (sJ1939RxQueue[RxQuePop].bDataRdy)
	}//end for (RxQuePop = 0; RxQuePop < RX_QUE_SIZE; ++RxQuePop)
}

/*********************************************************************//**
 *
 * Processs all transmit J1939 messages
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _J1939_TxManager(void)
{
	static u8 n = 0U;
	u8 TxReturnCode;
	bool StatusFlag = FALSE;

//If Network communication disable then dont tx CAN messaage
	if(commInfo.networkCommTxEnabled == FALSE)
	{
		return;
	}
	
	//Check CAN Transmmision Buffer availability 

	StatusFlag = bCheckCanTxBufferAvailable();
	if(StatusFlag == FALSE)
	{
		return;	//return if nuffer is not empty 
	}
	
	++n;			
	if(n >= eNoOfPgnTx)
	{
		n =0U;
	}	
	
	if (J1939TxData[n].bEnabled)
	{
		// Determine whether sending on demand or periodic
		if (((sPGNconstantsTx[n].rate != SEND_ON_DEMAND)
		&& ((u32GetClockTicks() - J1939TxData[n].u32TimeStamp) >= sPGNconstantsTx[n].rate))
		|| J1939TxData[n].bSendOnDemand)
		{
			switch (sPGNconstantsTx[n].PGNtype)
			{
				case ePGN_J1939 :
					// Engine instance is part of the PGN
					if(n == eTx59904)
					{
						vTxMsgBufProcessing(sPGNconstantsTx[n].ID,MSG_LEN_3,J1939TxData[n].u8data,J1939_TX);	
					}
					else	
					{
						vTxMsgBufProcessing(sPGNconstantsTx[n].ID,MSG_LEN,J1939TxData[n].u8data,J1939_TX);	
					}
				break;				
			}			
				// Reset the timer and on demand flag
				J1939TxData[n].u32TimeStamp = u32GetClockTicks();
				J1939TxData[n].bSendOnDemand = FALSE;
						
		}
	}
	
}

/*********************************************************************//**
 *
 * Enable Disable Transmit PGN 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
 void vJ1939_TxMsgEnableDisable(tPGN_TxIndexerType eTxPgn , bool EnableFlag)
{
	J1939TxData[eTxPgn].bEnabled = EnableFlag;
}


/*********************************************************************//**
 *
 * Enable Disable Transmit PGN 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vJ1939_TxSendOnDemandEnable(tPGN_TxIndexerType eTxPgn )
{
	J1939TxData[eTxPgn].bSendOnDemand = TRUE;
}

/*********************************************************************//**
 *
 * (Not use in code) Determines whether or not the PGN is supported for ISO requests.
 * If it is supported, it is then transmitted.
 *
 * @param      PGN    
 *
 * @return     TRUE if PGN is supported, FALSE otherwise
 *************************************************************************/
bool bProcessISORequest(u32 PGN)
{
	s16 n;
	bool foundPGN;

	// Find a matching PGN
	foundPGN = FALSE;
	n = -1;
	do 
	{
		++n;
		foundPGN = (bool)(sPGNconstantsTx[n].PGN == PGN);
	} while ((n < eNoOfPgnTx) && (!foundPGN));

	if (foundPGN)
	{
		// Set flag to force immediate output
		if(n < eNoOfPgnTx)
		{
			J1939TxData[n].bSendOnDemand = TRUE;
		}
	}

	return foundPGN;
}


/*********************************************************************//**
 *
 * (Not using transmite )Convert input data into PGN specification and deposit into the
 * PGN data for the output message.
 *
 * @param      data type,
 *             data,
 *             +- percent overrange   
 *
 * @return     None
 *************************************************************************/
void vPutPGNdata(eTxDataType dtype, u32 rawData, s16 overrange)
{
	u32 data;
	u8 *dataPtr;
	
	struct 
		{ 
		u8 byteSize;
		u8 bitMask; 
	} b;

	// Set a pointer to the PGN message data
	dataPtr = (u8 *)&J1939TxData[sPGNdataTx[dtype].PGNindex].u8data[sPGNdataTx[dtype].byteIndex];

	if (dtype < eNumofTxDataTypes)
	{
		// Valid data type - determine whether it is byte or bit data
		if (sPGNdataTx[dtype].bitRes == BITDATA)
		{
			// Bit data --------------
			// Get the bit mask and clear the bits
			b.bitMask = sPGNdataTx[dtype].bitMask;
			*dataPtr &= b.bitMask;

			// Shift the raw data bits up to the proper position
			while ((b.bitMask & 1) == 1)
			{
				rawData <<= 1;
				b.bitMask >>= 1;
			}

			// Set the bits
			*dataPtr |= rawData;
		}
		else
		{	
			// unused and needs to be modified for future use
			// Byte data ------------- 
			if (overrange >= MAX_OVERRANGE)
			{
				data = sPGNdataTx[dtype].overrangeData;
			}
			else
			{
					// Convert rawData to PGN format for integer math
				 switch (dtype)
				 	{
					 	 case eJOdoDataTx:
						 case eJOdoTripATx:
						 case eJTripAFuelEffHrsTx:
						 case eJOdoTripBTx:
						 case eJTripBFuelEffHrsTx:
						 case eJDTETx:	
						 case ejDistanceTONextServiceTx:
						 case eJCapacitiveFuelTx:
											 					
							data = ( ( (rawData*(u32)sPGNdataTx[dtype].bitRes)) - sPGNdataTx[dtype].dataOffset);

							b.byteSize = sPGNdataTx[dtype].byteSize;							
							do 
							{
		                       	if(b.byteSize == 1U)
		                       	{
			                       	if(dtype == eJCapacitiveFuelTx)
		                       	 	{
		                       	 		*dataPtr &= 0xFCU; //Add bit masking
		                       	 	}
									else
								 	{
								 		*dataPtr &= 0xF0U; //Add bit masking
								 	}
				                       	
										*dataPtr |= (u8)data;						
		                       	}
							   	else
							   	{
								 	*dataPtr++ = (u8)data;
									data >>= 8;
							   	}
								
							} while (--b.byteSize != 0U);
								
						break;
						case eJTripAFuelEffKmTx:											
					  	case eJTripBFuelEffKmTx:
							data = ( ( (rawData*(u32)sPGNdataTx[dtype].bitRes)) - sPGNdataTx[dtype].dataOffset);
							b.byteSize = sPGNdataTx[dtype].byteSize;
							
							do 
							{
	                       	 	*dataPtr++ = (u8)data;
								data >>= 8;
							   									
							} while (--b.byteSize != 0U);

						break;
					 	case eJFuelLevelTx:
						case eJDualFuelLevelTx:
							data = rawData;
							b.byteSize = sPGNdataTx[dtype].byteSize;
							
							do 
							{
	                       	 	*dataPtr++ = (u8)data;
								data >>= 8;
							   									
							} while (--b.byteSize != 0U);

						break;
						case eJAfeTx:
							data = (rawData - sPGNdataTx[dtype].dataOffset);
							b.byteSize = sPGNdataTx[dtype].byteSize;						
							do 
							{
	                       	 	*dataPtr++ = (u8)data;
								data >>= 8;
							   									
							} while (--b.byteSize != 0U);
							
						break;
						case eJHourDataTx:
							data = ( ( (rawData*(u32)sPGNdataTx[dtype].bitRes)) - sPGNdataTx[dtype].dataOffset);
							b.byteSize = sPGNdataTx[dtype].byteSize;
							do{
								if(b.byteSize== 4U)
		                       	{
			                       	*dataPtr &= 0x0F; //Add bit masking
									*dataPtr |= (u8)((data & 0x000000FU) << 4);
									*dataPtr++;
									 data >>= 4;
		                       	}
							   	else
							   	{
								 	*dataPtr++ = (u8)data;
									data >>= 8;
							   	}
							}
							while (--b.byteSize != 0U);
						break;
						case eJHourTripATx:
						case eJHourTripBTx:
							data = ( ( (rawData*(u32)sPGNdataTx[dtype].bitRes)) - sPGNdataTx[dtype].dataOffset);
							b.byteSize = sPGNdataTx[dtype].byteSize;
							do{
								if(b.byteSize== 3U)
		                       	{
			                       	*dataPtr &= 0x0F; //Add bit masking
									*dataPtr |= (u8)((data & 0x000000FU) << 4);
									*dataPtr++;
									 data >>= 4;
		                       	}
							   	else
							   	{
								 	*dataPtr++ = (u8)data;
									data >>= 8;
							   	}
							}
							while (--b.byteSize != 0U);
						break;
						case eJAirPressFrontTx:
						case eJAirPressRearTx:
							
						 	if(rawData >= BADDATA)
						 	{
								data = rawData;
						 	}
							else
							{
							   if(rawData < 7U) //7 means 15 Bar
							   {
									rawData = rawData *200U; // Convert BAR to KPA ex. 1 bar display = 1*2*100 ,
															//APG bar * 2 to make bar is 2 , 4, 6 ........
							   }
							   else
							   {
							    rawData = 15U;
							    rawData = rawData *100U;
							   }
				
												  
								data = ( (rawData/(u32)sPGNdataTx[dtype].bitRes) - sPGNdataTx[dtype].dataOffset);
							}
							b.byteSize = sPGNdataTx[dtype].byteSize;						
							do 
							{
	                       	 	*dataPtr++ = (u8)data;
								data >>= 8;
							   									
							} while (--b.byteSize != 0U);
							
						break;
					 	
					}			 

				
			}

			
		}
		
	}
	
}

/*********************************************************************//**
 *
 * Convert input data into PGN specification and deposit into the
 * PGN data for the output message
 *
 * @param      data type,
 *             data,
 *             +- percent overrange   
 *
 * @return     None
 *************************************************************************/
u32 u32GetPGNdata(eRxDataType dtype)
{
	u32 data = 0U;	
	u8 i;
	union 
	{
		u8 byteSize;
		u8 bitMask; 
	} b;

	// Get the data bit mask
	b.bitMask = sPGNdataRx[dtype].bitMask;

	if (dtype < eNumofRxDataTypes)
	{
		// Valid data type - determine whether it is byte or bit data
		if (sPGNdataRx[dtype].bitRes == BITDATA)
		{
			// Set a pointer to the PGN message data
			data = J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex];
			// Keep only the relevant bits of the data
			data &= ~(u8)b.bitMask;
			// Shift them down to the least significant bit location
			while ((b.bitMask & 1U) == 1U)
			{
				b.bitMask >>= 1;
				data >>= 1;
			}
		}
		else
		{
			// Byte data -------------
			// Get the data byte size
			b.byteSize = sPGNdataRx[dtype].byteSize;
			if(b.byteSize==1U)
			{
				data = (u8)J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex];

				if(data < sPGNdataRx[dtype].overrangeData ) 
				{				
					if(dtype == eJEngineModeSelection || dtype == eJFuel)
					{
						if(data <= sPGNdataRx[dtype].underrangeData)
							data = sPGNdataRx[dtype].underrangeData;
						data = ( (data * (u32)sPGNdataRx[dtype].bitRes)/10U) +  sPGNdataRx[dtype].dataOffset;
					}
					else
					{
					   if(data <= sPGNdataRx[dtype].underrangeData)
							data = sPGNdataRx[dtype].underrangeData;
						data = (data * (u32)sPGNdataRx[dtype].bitRes) +  sPGNdataRx[dtype].dataOffset;
					}
				}
			}
			else
			{
				for( i = 0U; i< b.byteSize; i++)
				{
					data |= (u32)J1939RxData[sPGNdataRx[dtype].PGNindex].u8data[sPGNdataRx[dtype].byteIndex + i] << (i*8);
				}

				 switch (dtype)
				 	{
				 	 case eJAfe:
					 				
							if(data < sPGNdataRx[dtype].overrangeData ) 
							{
							   if(data >= 0xffffffU) //if afe data is coming more then 3 bytes then keep it 3 bytes 
							   	{
							   		data = 0xffffffU;
							   	}
								data =  data * sPGNdataRx[dtype].bitRes;
								
							}
					 	
					 break;
							
					case ejOat:	
						if(data < sPGNdataRx[dtype].overrangeData ) 
						{
						   if(data <= sPGNdataRx[dtype].underrangeData)
						   	{
						   		data = sPGNdataRx[dtype].underrangeData;
						   	}
							data = ( (data * sPGNdataRx[dtype].bitRes) / 100000U ) +  sPGNdataRx[dtype].dataOffset;
							
						}
						break;
					case eJRpm:	
						if(data < sPGNdataRx[dtype].overrangeData ) 
						{
						   if(data <= sPGNdataRx[dtype].underrangeData)
						   	{
						   		data = sPGNdataRx[dtype].underrangeData;
						   	}
							data = ( (data * sPGNdataRx[dtype].bitRes) / 1000U ) +  sPGNdataRx[dtype].dataOffset;
							
						}
					break;
					case eJCapacitiveAft2:	
						
						data = data & 0x03FF; //AFT2 data is in 10 bit 
						if(data > 0x1F4 && data < 0x3FE) // if data is greter 5 v and lesser 3fa the keep is 5v
						{
							data = 0x1F4;
						}
						data =  (data * sPGNdataRx[dtype].bitRes)  +  sPGNdataRx[dtype].dataOffset;
					 
					break;					
					 default:
					 			
						if(data < sPGNdataRx[dtype].overrangeData ) 
						{
						   if(data <= sPGNdataRx[dtype].underrangeData)
						   	{
						   		data = sPGNdataRx[dtype].underrangeData;
						   	}
							data =  (data * sPGNdataRx[dtype].bitRes)  +  sPGNdataRx[dtype].dataOffset;
							
						}
					 
				 	  
				 	}


		}
			
		
		}
		
	}//end if (dtype < eNumofRxDataTypes)
	return data;
}

/*********************************************************************//**
 *
 * (Not using this function) Gets the latest bit data from PGN received message buffer. Bits are
 * shifted down to the least significant bits before returning.
 *
 * @param      data type to fetch  
 *
 * @return     bit data
 *************************************************************************/
u8 u8GetBitData(eRxDataType dType)
{
	// Get the data bit mask
	u8 bitMask = sPGNdataRx[dType].bitMask;

	// Get the data
	u8 data = J1939RxData[sPGNdataRx[dType].PGNindex].u8data[sPGNdataRx[dType].byteIndex];

	// Keep only the relevant bits of the data
	data &= ~(u8)bitMask;

	// Shift them down to the least significant bit location
	while ((bitMask & 1U) == 1U)
	{
		bitMask >>= 1;
		data >>= 1;
	}

	return data;
}

/*********************************************************************//**
 *
 * Copy PGN 65279 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65279_0x00_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	
	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65279_0x00].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65279_0x00].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65279_0x00].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65279_0x00].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJWaterInFuelLamp] = TRUE;
	bSpnDataReadyStatus[eJSCR_SPN5825] = TRUE;
	
}

/*********************************************************************//**
 *
 * Copy PGN 65279 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65279_0x21_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	
	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65279_0x21].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65279_0x21].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65279_0x21].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65279_0x21].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJGearShiftUpDown] = TRUE;   
	
}

 
/*********************************************************************//**
 *
 * Copy PGN 65252 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65252_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	
	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65252].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65252].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65252].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65252].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJEnginePreHeat] = TRUE;
	
}

/*********************************************************************//**
 *
 * Copy PGN 65262 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65262_CooleantTemp(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65262].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65262].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65262].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65262].bDataRdy = msg->bDataRdy;	
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJCoolantTemp] = TRUE;
}
/*********************************************************************//**
 *
 * Copy PGN 65110 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65110_DEF(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	  

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65110].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65110].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65110].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65110].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJLowDEFLamp] = TRUE;
	bSpnDataReadyStatus[eJDefLamp] = TRUE;
	  
}

/*********************************************************************//**
 *
 * Copy PGN 61445 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN61445_Nuteral(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61445].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61445].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61445].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx61445].bDataRdy = msg->bDataRdy; 
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJNuetral] = TRUE;
}

/*********************************************************************//**
 *
 * Copy PGN 61445 AMT data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN61445_0x03_Nuteral(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61445_0x03].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61445_0x03].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61445_0x03].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx61445_0x03].bDataRdy = msg->bDataRdy; 
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJGearValue_0x03] = TRUE;
}

/*********************************************************************//**
 *
 * Copy PGN 61445 AMT data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN61445_0x21_ManualGear(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61445_0x21].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61445_0x21].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61445_0x21].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx61445_0x21].bDataRdy = msg->bDataRdy; 
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJManualGear_0x21] = TRUE;
}

/*********************************************************************//**
 *
 * Copy PGN 65088 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/

static void _vPGN65088_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65088].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65088].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65088].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65088].bDataRdy = msg->bDataRdy; 
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJFrontRearLampSPN2388] = TRUE;
   	bSpnDataReadyStatus[eJFrontRearLampSPN2390] = TRUE;
	bSpnDataReadyStatus[eJHighBeam] = TRUE;
	
}


/*********************************************************************//**
 *
 * Copy PGN 65265 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65265_0x00(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65265_0x00].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65265_0x00].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65265_0x00].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65265_0x00].bDataRdy = msg->bDataRdy; 
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJPowerTakeOff] = TRUE;
	bSpnDataReadyStatus[eJCruiseControl] = TRUE;
	bSpnDataReadyStatus[ejParkingBrake] = TRUE;
}


/*********************************************************************//**
 *
 * Copy PGN 64892 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN64892_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64892].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64892].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64892].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx64892].bDataRdy = msg->bDataRdy; 
	bSpnDataReadyStatus[eJDPF] = TRUE;		
   	bSpnDataReadyStatus[eJHEST] = TRUE;
	bSpnDataReadyStatus[eJDPFC1_SPN3702] = TRUE;
	bSpnDataReadyStatus[eJDPFC1_SPN3703] = TRUE;
	bSpnDataReadyStatus[eJDPFC1_SPN4175] = TRUE;
	  
	  
}

/*********************************************************************//**
 *
 * Copy PGN 64775 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN64775_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64775].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64775].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64775].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx64775].bDataRdy = msg->bDataRdy;
	
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJMilSPN5080] = TRUE;
	bSpnDataReadyStatus[eJLowOilLamp] = TRUE;
	bSpnDataReadyStatus[eJExhaustBreak] = TRUE;
	bSpnDataReadyStatus[eJCheckEng5078] = TRUE;
	bSpnDataReadyStatus[eJStopEng5079] = TRUE;


	 
}

/*********************************************************************//**
 *
 * Copy PGN 61444 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/ 
static void _vPGN61444_RPM(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61444].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61444].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61444].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx61444].bDataRdy = msg->bDataRdy;

	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJRpm] = TRUE;
	  
}
/*********************************************************************//**
 *
 * Copy PGN 65098 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN65098_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65098].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65098].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65098].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65098].bDataRdy = msg->bDataRdy;
	
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJETC7] = TRUE;
	bSpnDataReadyStatus[eJCreep] = TRUE;
	bSpnDataReadyStatus[eJPower] = TRUE;
	bSpnDataReadyStatus[eJAuto_Manual_Eco] = TRUE;
	bSpnDataReadyStatus[eJCrawl] = TRUE;
	bSpnDataReadyStatus[eJShiftInhibit] = TRUE;
	  
}
/*********************************************************************//**
 *
 * Copy PGN 64917 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/

static void _vPGN64917_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx64917].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx64917].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64917].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx64917].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJTransFluidTemp] = TRUE;
	  
}

/*********************************************************************//**
 *
 * Copy PGN 61441 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/
static void _vPGN61441_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	
	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx61441].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx61441].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61441].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx61441].bDataRdy = msg->bDataRdy;
    //Set SPN Data ready flag 
	 bSpnDataReadyStatus[eJAbsTrailer] = TRUE; 
	 bSpnDataReadyStatus[eJAbsMil_1438] = TRUE;
}

static void _vPGN65103_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 

	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	     J1939RxData[eRx65103].u8data[i] = msg->u8data[i];
	}
	     
	  J1939RxData[eRx65103].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65103].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65103].bDataRdy = msg->bDataRdy;
	  
	  //Set SPN Data ready flag 
   	  bSpnDataReadyStatus[eJEscMil] = TRUE;
	  
}

static void _vPGN65219_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65219].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65219].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65219].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65219].bDataRdy = msg->bDataRdy;
	  	  
	  //Set SPN Data ready flag 
   	  bSpnDataReadyStatus[eJTransRangeLow] = TRUE;
	  
}

static void _vPGN65257_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65257].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65257].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65257].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65257].bDataRdy = msg->bDataRdy;
	  
	  //Set SPN Data ready flag 
	  bSpnDataReadyStatus[eJAfe] = TRUE;
}


static void _vPGN65276_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65276].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65276].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65276].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65276].bDataRdy = msg->bDataRdy;
	  //Set SPN Data ready flag 
	  bSpnDataReadyStatus[eJFuel] = TRUE;

	  
}

static void _vPGN60160_DM1_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 
	if(msg->u8data[0] == 1U)						// if frame number is 1.
	{

	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx60160].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx60160].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx60160].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx60160].bDataRdy = msg->bDataRdy;

	  //Set SPN Data ready flag 
	  bSpnDataReadyStatus[eJCheckEngineDM1_624] = TRUE;
	  bSpnDataReadyStatus[eJCheckEngineDM1_3040] = TRUE;
	  bSpnDataReadyStatus[eJStopEngineDM1_623] = TRUE;
	  bSpnDataReadyStatus[eJStopEngineDM1_3039] = TRUE;
	  bSpnDataReadyStatus[eJMilDM1_1213] = TRUE;
	  bSpnDataReadyStatus[eJMilDM1_3038] = TRUE;
	}
	  
}

static void _vPGN60160_DM1_0x03_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	
	if(msg->u8data[0] == 1U)						// if frame number is 1.
	{
	 // Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx60160_0x03].u8data[i] = msg->u8data[i];
	}
	
		J1939RxData[eRx60160_0x03].u32TimeStamp = msg->u32TimeStamp;
		J1939RxData[eRx60160_0x03].sMsgID.Identifier = msg->sMsgID.Identifier;	  
		J1939RxData[eRx60160_0x03].bDataRdy = msg->bDataRdy;
		 //Set SPN Data ready flag 
		bSpnDataReadyStatus[eJCheckTrance] = TRUE;
		bSpnDataReadyStatus[eJCheckTranceDM1] = TRUE;
		bSpnDataReadyStatus[eJTPDT_Protocol] = TRUE;

	}
	  
}

static void _vPGN60160_DM1_0x33_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 
	if(msg->u8data[0] == 1U)						// if frame number is 1.
	{

	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx60160_0x33].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx60160_0x33].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx60160_0x33].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx60160_0x33].bDataRdy = msg->bDataRdy;
	  bSpnDataReadyStatus[eJTPMS_LampDM1_624] = TRUE;
	}
	  
}

/*********************************************************************//**
 *
 * Copy PGN 65226 data from rx receiving buffer to its actual buffer
 *
 * @param      Rx Pgn type
 *
 * @return     None
 *************************************************************************/

static void _vPGN65226_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;

	for (i=0U; i < 8U; ++i)
	{
		J1939RxData[eRx65226].u8data[i] = msg->u8data[i];
	}

	J1939RxData[eRx65226].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65226].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	J1939RxData[eRx65226].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
   	bSpnDataReadyStatus[eJCheckEngineSPN624] = TRUE;
	bSpnDataReadyStatus[eJCheckEngineSPN3040] = TRUE;
	bSpnDataReadyStatus[eJMilSPN1213] = TRUE;
	bSpnDataReadyStatus[eJMilSPN3038] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_spn1] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_spn1] = TRUE;
	bSpnDataReadyStatus[eJDM1DTC_fmi] = TRUE;
	bSpnDataReadyStatus[eJStopEngine623] = TRUE;
	bSpnDataReadyStatus[eJStopEngine3039] = TRUE;
	  
}

static void _vPGN65226_0x03_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;
	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65226_0x03].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65226_0x03].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65226_0x03].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65226_0x03].bDataRdy = msg->bDataRdy;

	 //Set SPN Data ready flag 
	 bSpnDataReadyStatus[ejLearnFailDM1SPN1] = TRUE;
	 bSpnDataReadyStatus[ejLearnFailDM1SPN2] = TRUE;
	 bSpnDataReadyStatus[ejLearnFailDM1FMI] = TRUE;	 

	  
}

static void _vPGN65226_0x33_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65226_0x33].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65226_0x33].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65226_0x33].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65226_0x33].bDataRdy = msg->bDataRdy;
	  bSpnDataReadyStatus[eJTPMS_Lamp] = TRUE;
	  
}


static void _vPGN65269_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 

	// Save the latest received data
	 for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx65269].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx65269].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx65269].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx65269].bDataRdy = msg->bDataRdy;

		 //Set SPN Data ready flag 
		bSpnDataReadyStatus[ejOat] = TRUE;
	  
}



static void _vPGN64712_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 

	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx64712].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx64712].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx64712].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx64712].bDataRdy = msg->bDataRdy;

	  //Set SPN Data ready flag 
	bSpnDataReadyStatus[eJEngineModeSelection] = TRUE;
	  
}

static void _vPGN61442_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 

	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	 {
	     J1939RxData[eRx61442].u8data[i] = msg->u8data[i];
	 }
	     
	  J1939RxData[eRx61442].u32TimeStamp = msg->u32TimeStamp;
	  J1939RxData[eRx61442].sMsgID.Identifier = msg->sMsgID.Identifier;	  
	  J1939RxData[eRx61442].bDataRdy = msg->bDataRdy;

		//Set SPN Data ready flag 
		bSpnDataReadyStatus[eJSpeedoSpn191] = TRUE;
	  
}

static void _vPGN65263_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx65263].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx65263].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65263].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx65263].bDataRdy = msg->bDataRdy;

	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJEngineOilPressure] = TRUE;

}

static void _vPGN61440_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx61440].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx61440].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx61440].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx61440].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJRetarderRequest] = TRUE;	

}

static void _vPGN65303_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx65303].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx65303].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65303].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx65303].bDataRdy = msg->bDataRdy;

	
	bSpnDataReadyStatus[eJAirFilterClogged65303] = TRUE;
	bSpnDataReadyStatus[eJBatteryCharging1] = TRUE;	
	bSpnDataReadyStatus[eJBatteryCharging2] = TRUE;	
	bSpnDataReadyStatus[eJHaltRequest] = TRUE;
	bSpnDataReadyStatus[eJRampEngaged] = TRUE;
	bSpnDataReadyStatus[eJKneelingFront] = TRUE;
	bSpnDataReadyStatus[eJKneelingRear] = TRUE;
	bSpnDataReadyStatus[eJKneelingBoth] = TRUE;
	bSpnDataReadyStatus[eJEngineOilLow] = TRUE;
	bSpnDataReadyStatus[eJEngineCoolantLow] = TRUE;
	bSpnDataReadyStatus[eJEmergencyExit] = TRUE;
	bSpnDataReadyStatus[eJHaltRequestPriority] = TRUE;
	bSpnDataReadyStatus[eJSteeringAdjust] = TRUE;
	bSpnDataReadyStatus[eJBuzzerDisable] = TRUE;	
	bSpnDataReadyStatus[eJRearFlapOpen] = TRUE;
	bSpnDataReadyStatus[eJSideFlapOpen] = TRUE;
	bSpnDataReadyStatus[eJKneelingRight] = TRUE;
	bSpnDataReadyStatus[eJKneelingLeft] = TRUE;

}

static void _vPGN65378_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx65378].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx65378].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65378].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx65378].bDataRdy = msg->bDataRdy;
	
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJTurnLeftRightTurn] = TRUE;
	bSpnDataReadyStatus[ejTrailer] = TRUE;
	bSpnDataReadyStatus[eJAbsTrailer65378] = TRUE;
	bSpnDataReadyStatus[ejLiftAxelFault] = TRUE;
	bSpnDataReadyStatus[eJBatteryWeak2] = TRUE;	
	bSpnDataReadyStatus[eJCapacitiveAft2] = TRUE;	

}


static void _vPGN65102_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx65102].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx65102].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65102].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx65102].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJDoorOpen] = TRUE;

}

static void _vPGN64964_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx64964].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx64964].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64964].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx64964].bDataRdy = msg->bDataRdy;
	//Set SPN Data ready flag 
	bSpnDataReadyStatus[eJHillHolder] = TRUE;

}

static void _vPGN65303_IILPropData(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx65303_0xE1].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx65303_0xE1].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx65303_0xE1].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx65303_0xE1].bDataRdy = msg->bDataRdy;

}

static void _vPGN64998_Data(sJ1939_RX_MSG_TYPE *msg)
{
	s16 i;	 
	
	// Save the latest received data
	for (i=0U; i < 8U; ++i)
	{
	 J1939RxData[eRx64998].u8data[i] = msg->u8data[i];
	}
	 
	J1939RxData[eRx64998].u32TimeStamp = msg->u32TimeStamp;
	J1939RxData[eRx64998].sMsgID.Identifier = msg->sMsgID.Identifier;   
	J1939RxData[eRx64998].bDataRdy = msg->bDataRdy;

}




void vResetDTCParameters(void)
{
	u8DTCSA = J1939_NULL_ADDRESS;
	u8DTCready = FALSE;
	u8DTCCount = 0U;
}

u8 u8GetDTCStatus(void)
{
	return u8DTCready;
}

u8 u8GetDTCCount(void)
{
	return u8DTCCount;
}

u8 u8GetDTCSA(void)
{
	return u8DTCSA;
}

u8 u8GetDtcFramNum()
{
	return u8DtcFramNum;
}

u16 u16GetDtcTotalBytes()
{
	return u16lTotalBytes;
}

static void _vResetDM1Reception(u8 SourceAdd)
{
	//reset the DTC parameters at the end of multipacket reception
	u8MultiPacketDM1[SourceAdd] = eNoNewMultiPacket; 
	u8MultiFrameSA[SourceAdd] = J1939_NULL_ADDRESS;	
	
}



/*****************************************************************************
 **  Description:
 **      Get data ready Of each PGN.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     data type,
 **                  
 **                  
 **  Return value:   True if msg Received ,False if not
 **
 *****************************************************************************/
bool bGetPGNDataReady(eRxDataType dtype)
{
	
	return J1939RxData[sPGNdataRx[dtype].PGNindex].bDataRdy;
}

/*****************************************************************************
 **  Description:
 **     Clear data ready Of each PGN.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     data type,
 **                  
 **                  
 **  Return value:   Non
 **
 *****************************************************************************/
void ClearPGNDataReady(eRxDataType dtype)
{
	J1939RxData[sPGNdataRx[dtype].PGNindex].bDataRdy = FALSE;
	
}


/*****************************************************************************
 **  Description:
 **      Get SPN data ready status
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     data type,
 **                  
 **                  
 **  Return value:   True if msg Received ,False if not
 **
 *****************************************************************************/
bool bGetSPNDataReady(eRxDataType dtype)
{
	
	return bSpnDataReadyStatus[dtype];
}

/*****************************************************************************
 **  Description:
 **     Clear data ready status of SPN
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     data type,
 **                  
 **                  
 **  Return value:   Non
 **
 *****************************************************************************/
void vClearSPNDataReady(eRxDataType dtype)
{
	bSpnDataReadyStatus[dtype] = FALSE;
	
}





/*****************************************************************************
 **  Description:
 **      Check the timeout Of each PGN.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     data type,
 **                  
 **                  
 **  Return value:   True if timeout ,False if not
 **
 *****************************************************************************/
bool bGetPGNTimeOut(eRxDataType dtype)
{
	if (dtype < eNumofRxDataTypes)
	{
		if((u32GetClockTicks() - J1939RxData[sPGNdataRx[dtype].PGNindex].u32TimeStamp) >= sPGNdataRx[dtype].u32TimeStamp)
		{
			J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate =TRUE;
		}
		else
		{
			J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate =FALSE;
		}
	}	
	return J1939RxData[sPGNdataRx[dtype].PGNindex].bDataOutDate;
}


/*********************************************************************//**
 *
 * This method is called from the CAN receive interrupt.
 * It will find an available queue slot and save the message for later
 * processing. If there is no room in the queue, the message will be
 * dropped.
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vJ1939OnFullRx(void)
{
	u32	MsgBuf_address;
	u8	rx_msg_DLC;	
	u32 rx_msg_ID;
	u8  rx_buffer_number;
	u8	rx_data_cnt;
	
//	sJ1939_Id_Struct_Type ID;
	s16 RxQuePush;

	
	//  Find an available queue slot
	for (RxQuePush = 0U;
	(RxQuePush < RX_QUE_SIZE) && (sJ1939RxQueue[RxQuePush].bDataRdy == TRUE);
	++RxQuePush)
	{
		// empty statement for this for loop
	}

	if( RxQuePush < RX_QUE_SIZE )
	{
		//  Ignore message if no room in the queue
		if (sJ1939RxQueue[RxQuePush].bDataRdy == FALSE)
		{
		
			C0INTS = 0x0002;	
			rx_buffer_number = C0LIPT;						// Get receive message buffer number
			MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * rx_buffer_number));		// Set CAN message buffer[n] register address
			rx_data_cnt = 0U;
			

			// Time stamp this message regardless of buffer availability
			sJ1939RxQueue[RxQuePush].u32TimeStamp = u32GetClockTicks();
			
			// Get the ID
			*((u32 *)(MsgBuf_address + 0x0e)) = 0x0004;		// Clear DN bit
			rx_msg_DLC = *((u8 *)(MsgBuf_address + 0x08));	// Get receive message data length
			rx_msg_ID = (*((uint32_t *)(MsgBuf_address + 0x0c))<< 16 )  | (*((uint32_t *)(MsgBuf_address + 0x0A)));
			rx_msg_ID &= ~(0x80000000U);                                    // clear the IDE bit
			

			// Save the ID
			sJ1939RxQueue[RxQuePush].sMsgID.Identifier = rx_msg_ID;
			
				// Get the byte count
				sJ1939RxQueue[RxQuePush].ucNumofRxBytes = rx_msg_DLC;

				// Deposit the data
				for(rx_data_cnt = 0 ;((rx_data_cnt < rx_msg_DLC) && (rx_data_cnt < 8)) ; rx_data_cnt++)
				{
					sJ1939RxQueue[RxQuePush].u8data[rx_data_cnt] = *((u8 *)(MsgBuf_address + (0x01 * rx_data_cnt)));	
				}
				// Set data valid
				sJ1939RxQueue[RxQuePush].bDataRdy = TRUE;
			
		}
	}
	else
	{
	   //if message receiving buffer not available empty room then clear CAN interrupt 
		C0INTS = 0x0002;
		rx_buffer_number = C0LIPT;
		// Set CAN message buffer[n] register address
		MsgBuf_address = (MSGBUF_BASE_ADD + (0x10 * rx_buffer_number));		
		*((u32 *)(MsgBuf_address + 0x0e)) = 0x0004;		// Clear DN bit
	}

}


void vClearDTC(void)
{
	u8 i = 0;
	vResetDTCParameters();

}

