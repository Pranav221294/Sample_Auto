/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
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
#include "customTypedef.h"
#include "r_cg_macrodriver.h"  //  CPU file
#include "r_cg_userdefine.h"
#include "r_cg_wdt.h"
#include "Lcd.h"
#include "r_cg_wdt.h"
#include "r_cg_serial.h"
#include "iicDriver.h"
#include "TellTales.h"
#include "J1939.h"
#include "Amt.h"
#include "Def.h"
#include "AirPressureGauge.h" 
#include "OilPressureGauge.h"
#include "DiscreteSwitches.h"
#include "Eol.h"
#include "ServiceReminder.h"
#include "LCDScreens.h"
#include "HourMeter.h"
#include "OdoMeter.h"
#include "GenIllumination.h" 
#include "MiscFiles.h"
#include "HourMeter.h"
#include "Clock.h"
#include "Priority2Texts.h"
#include "TML_14229_UDSH.h"

// constant declarations -----------------------------------------------------
#define SERVICE_DISP_TIME 10000		//in ms

//LCD have a 240 segment and each segment build by 4 bits so total byte used 240/2 = 120
#define LCD_SEGMENT  					120
#define LCD_DEVICE_ADD       			0x70
#define CONTROL_BYTE_COMMAND 			0x80  //control byte continue, command
#define COLUMN_ADD    	     			0x13  // command for select column address
#define PARAMETR_ADD         			0xC0  //control byte last, parameter
#define PARAMETR_ADD1         			0x40  //control byte last, parameter
#define PARAMETER_ADD2          		0x20
#define SWRST 		 					0x5A
#define RESET_ADD  						0xA5
#define DISP_MODE_SET_ADD  				0x66
#define NORMAL_ADD  					0x00
#define DISP_LINE_SET_ADD  				0x6d
#define LINE_4_ADD  					0x03
#define DISP_LINE_SET_ADD1  			0x36
#define COMMON_REVERSE 					0xC5         
#define COLUMN_DIRECT_NORMAL    		0x84
#define DISP_LINE_SET_ADD2      		0x55
#define CLK_FREQ_125            		0x07
#define SUPPLY_CONTROL          		0x27
#define BIAS_ADD                		0x01
#define OSC_CKT_ON              		0xab
#define DISP_FRAME_FREQ         		0x5f
#define FREQ_200hz              		0x14
#define LCD_BIAS_SET            		0xA2
#define SUPPLY_ON               		0x25
#define DISP_ON                 		0xaf        
#define DISP_COMMAND                	0x20  // Display data command  

//Segment address module wise
#define DEF_SEG_ADD          			16		//DEF SEG ADD (seg16) 16
#define OIL_SEG_ADD          			16    	//OIL SEG ADD (seg16) 16
#define FRONT_AP_SEG_ADD     			174   	//FRONT APG SEG ADD (seg174) 174
#define REAR_AP_SEG_ADD      			180   	//REAR APG SEG ADD (seg180) 180
#define ABOVE_DEF_OIL_BOTTOM_LINE_ADD   14  	//Decorate Line
#define LINE_ABOVE_APG_ADD              34  	//Decorate Line
#define LINE_TOP_ADD                    158 	//Decorate Line
#define ABS_ESC_MIL_ADD                 174 	//Seg174 S101,S102
#define BCW_ADD                         176 	//Seg176 S103,S104,S105 //Break Wear ,Check Transmmision and Water in Fuel
#define AFC_ADD                	        194 	//Seg194 S106 //Air Filter clogged
#define DSDE_ADD                		22  	//Seg22 S22,S23,S24,S25 //DPF,SCR,Low def level,Exhaust break
#define RG_ADD                          16  	//Seg16 S26,S27 Retarder and Glow plug
#define LMH_ADD                         24 		//Seg24 LIGHT ,MEDIUM,HEAVY
#define ODO_ADD                         158 	//Seg158 ODO/HRS segment start address
#define ILL_ADD                         34  	//SEG34
#define TRIP_ADD                        36  	//SEG34
#define CLOCK_ADD                       200  	//SEG200 clock segments start from seg200
#define OAT_ADD                         208  	//SEG208 Outside ambient temperature 
#define BATT_VOLT_ADD                   194  	//SEG194 Battery Voltage
#define TPMS_STRING_ADD                 48  	//SEG48 TPMS string segment starts for SEG48 to SEG 119
#define TPMS_CHAR_ADD                   158 	//SEG158 TPMS 
#define AFE_ADD                         26  	//SEG26 AFE segment starts form S26 to SEG33
#define DEGREE_CENTIGRADE_ADD           14  	//SEG14 degree centigrade symbol S28
#define AMT_AT_ADD                      24  	//SEG24 S113,S112
#define AUTO_ADD                        34  	//SEG34 S111
#define AMT_SEG_ADD                     184 	//SEG184 to SEG194 MANUAL,CRAWAL,CREEP,POWER,ECO
#define SERVICE_ADD                     34  	//SEG34


//DEF GAUGE MACROS--------------------------------------------------
#define DEF_SYMBOLE_ON    		0b00100000  //Seg 16 & 17
#define DEF_SYMBOLE_OFF   		0b11011111  //Seg 16 & 17
#define F_SYMBOLE_ON      		0b00000001  //seg 18 & 19
#define E_SYMBOLE_ON      		0b00010001  //seg 20 & 21  E and Boundry ON
#define DEF_ON            		0b00010000  //Seg 22 & 23
 
#define DISP_DEF_1     			0b00100000 	//Seg 22 & 23 
#define DISP_DEF_2     			0b01000000 	//Seg 22 & 23 
#define DISP_DEF_3     			0b10000000 	//Seg 22 & 23 
#define DISP_DEF_4     			0b00001000 	//Seg 20 & 21 
#define DISP_DEF_5     			0b00000100 	//Seg 20 & 21 
#define DISP_DEF_6     			0b00000010 	//Seg 20 & 21 

#define CLR_DEF_1     			~DISP_DEF_1	//Seg 22 & 23 
#define CLR_DEF_2     			~DISP_DEF_2 //Seg 22 & 23 
#define CLR_DEF_3     			~DISP_DEF_3 //Seg 22 & 23 
#define CLR_DEF_4     			~DISP_DEF_4 //Seg 20 & 21 
#define CLR_DEF_5     			~DISP_DEF_5 //Seg 20 & 21 
#define CLR_DEF_6     			~DISP_DEF_6 //Seg 20 & 21 

//OIL GAUGE MACROS--------------------------------------------------
#define OIL_SYMBOLE_ON    		0b00010000  //Seg 16 & 17
#define OIL_BAR_CHAR_ON     	0b00000001  //Seg 16 & 17
#define OIL_BOUNDRY_ON    		0b00010000  //Seg 18 & 19 
#define OIL_12_ON           	0b00000010  //Seg 18 & 19 
#define OIL_10_ON           	0b00000100  //Seg 18 & 19
#define OIL_8_ON            	0b00001000  //Seg 18 & 19
#define OIL_6_ON            	0b10000000  //Seg 20 & 21
#define OIL_4_ON            	0b01000000  //Seg 20 & 21
#define OIL_2_ON            	0b00100000  //Seg 20 & 21


#define OIL_SYMBOLE_OFF   		~0b00010000  //Seg 16 & 17
#define OIL_BAR_CHAR_OFF    	~0b00000001  //Seg 16 & 17
#define OIL_BOUNDRY_OFF    		~0b00010000  //Seg 18 & 19 
#define OIL_12_OFF              ~0b00000010  //Seg 18 & 19 
#define OIL_10_OFF              ~0b00000100  //Seg 18 & 19
#define OIL_8_OFF               ~0b00001000  //Seg 18 & 19
#define OIL_6_OFF               ~0b10000000  //Seg 20 & 21
#define OIL_4_OFF               ~0b01000000  //Seg 20 & 21
#define OIL_2_OFF               ~0b00100000  //Seg 20 & 21



#define DISP_OIL_1     			0b00000010 //Seg 16 & 17
#define DISP_OIL_2     			0b00000100 //Seg 16 & 17
#define DISP_OIL_3     			0b00001000 //Seg 16 & 17
#define DISP_OIL_4     			0b10000000 //Seg 18 & 19 
#define DISP_OIL_5     			0b01000000 //Seg 18 & 19 
#define DISP_OIL_6     			0b00100000 //Seg 18 & 19  

#define CLR_OIL_1     			~DISP_OIL_1 //Seg 22 & 23 
#define CLR_OIL_2     			~DISP_OIL_2 //Seg 22 & 23 
#define CLR_OIL_3     			~DISP_OIL_3 //Seg 22 & 23 
#define CLR_OIL_4     			~DISP_OIL_4 //Seg 20 & 21 
#define CLR_OIL_5     			~DISP_OIL_5 //Seg 20 & 21 
#define CLR_OIL_6     			~DISP_OIL_6 //Seg 20 & 21 

//Front AG GAUGE MACROS --------------------------------------------------
//FRONT APG SENGMETS DETAILS
//SEG174 & SEG175 data goes in byte1 
//SEG176 & SEG177 data goes in byte2 
//SEG178 & SEG179 data goes in byte3 
//SEG180 & SEG181 data goes in byte4 
//SEG182 & SEG183 data goes in byte5

#define FAP_SYMBOLE_ON    		0b00001000  //Seg 176 & 177  //S99
#define FAP_SYMBOLE_OFF   		~0b00001000  //Seg 176 & 177  //S99
#define FAP_BOUNDRY_1_ON    	0b00000100  //Seg 176 & 177  //S98
#define FAP_BOUNDRY_2_ON    	0b00010000  //Seg 176 & 177  //S92
#define FAP_BAR_CHAR_ON       	0b00000011  //Seg 174 & 175  //S87,S67 Both BAR charactor ON
#define FAP_BOUNDRY_3_ON    	0b00000010  //Seg 178 & 179  //S89
#define FAP_BOUNDRY_4_ON    	0b00001000  //Seg 178 & 179  //S88

#define AP_15_ON               	0b00001000  //Seg 180 & 181   S86
#define AP_12_ON               	0b00000100  //Seg 180 & 181   S85
#define AP_10_ON               	0b00000010  //Seg 180 & 181   S84
#define AP_8_ON                	0b00000001  //Seg 180 & 181   S83
#define AP_6_ON                	0b00100000  //Seg 182 & 183   S82
#define AP_4_ON                	0b01000000  //Seg 182 & 183   S81
#define AP_2_ON                	0b10000000  //Seg 182 & 183   S80

#define AP_15_OFF              	~0b00001000  //Seg 180 & 181   S86


#define DISP_FAP_1     			0b00000010 //Seg 176 & 177  //S97
#define DISP_FAP_2     			0b00000001 //Seg 176 & 177  //S96
#define DISP_FAP_3     			0b00100000 //Seg 178 & 179  //S95
#define DISP_FAP_4     			0b01000000 //Seg 178 & 179  //S94 
#define DISP_FAP_5     			0b10000000 //Seg 178 & 179  //S93
#define DISP_FAP_6     			0b00000001 //Seg 178 & 179  //S91
#define DISP_FAP_7     			0b00000100 //Seg 178 & 179  //S90

#define CLR_FAP_1     			~DISP_FAP_1 
#define CLR_FAP_2     			~DISP_FAP_2 
#define CLR_FAP_3     			~DISP_FAP_3 
#define CLR_FAP_4     			~DISP_FAP_4 
#define CLR_FAP_5     			~DISP_FAP_5 
#define CLR_FAP_6     			~DISP_FAP_6 
#define CLR_FAP_7     			~DISP_FAP_7 

//REAR AG GAUGE MACROS---------------------------------------------------
//REAR APG SENGMETS DETAILS
//SEG180 & SEG181 data goes in byte1 
//SEG182 & SEG183 data goes in byte2 
//SEG184 & SEG185 data goes in byte3

#define RAP_SYMBOLE_ON     		0b00010000      //SEG184 & SEG185 S79 	  	 	
#define RAP_SYMBOLE_OFF   		~0b00010000     //SEG184 & SEG185 S79 	  	 		
#define RAP_BOUNDRY_1_ON  		0b00100000      //SEG184 & SEG185 S78 	  	 	  	
#define RAP_BOUNDRY_2_ON  		0b00000001      //SEG182 & SEG183 S72 	  	
#define RAP_BOUNDRY_3_ON  		0b00100000      //SEG180 & SEG181 S70 	
#define RAP_BOUNDRY_4_ON  		0b10000000      //SEG180 & SEG181 S68  	

#define DISP_RAP_1    			0b01000000      //SEG184 & SEG185 S77 	  	 	  	   
#define DISP_RAP_2    			0b10000000      //SEG184 & SEG185 S76 	  	 	  	     
#define DISP_RAP_3    			0b00001000      //SEG182 & SEG183 S75 	  	      
#define DISP_RAP_4    			0b00000100      //SEG182 & SEG183 S74 	  	    
#define DISP_RAP_5    			0b00000010      //SEG182 & SEG183 S73 	  	  
#define DISP_RAP_6    			0b00010000      //SEG180 & SEG181 S71 	   
#define DISP_RAP_7    			0b01000000      //SEG180 & SEG181 S69 	  

#define CLR_RAP_1    	 		~DISP_RAP_1 
#define CLR_RAP_2     			~DISP_RAP_2 
#define CLR_RAP_3     			~DISP_RAP_3 
#define CLR_RAP_4     			~DISP_RAP_4 
#define CLR_RAP_5     			~DISP_RAP_5 
#define CLR_RAP_6     			~DISP_RAP_6 
#define CLR_RAP_7     			~DISP_RAP_7 

//Boundry Lines MACROS-----------------------------------------------------
//Segments details 
//SEG14 & SEG15 data goes in Byte1
//SEG34 & SEG35 data goes in Byte2
//SEG158 & SEG159 data goes in Byte3	
#define LINE_ABOVE_DEF_OIL_ON  		0b00010000      //SEG14 & SEG15  S21
#define LINE_BOTTOM_ON 	    		0b00000001      //SEG14 & SEG15  S108
#define LINE_ABOVE_APG_ON 			0b00010000      //SEG34 & SEG35  S100
#define LINE_TOP_ON 	    		0b10000000      //SEG158 & SEG159 S159

//Tell tales macros---------------------------------------------------------
#define ABS_MIL_ON    	        	0b00000100  //S101
#define ESC_MIL_ON    	        	0b00001000  //S102 
#define BREAK_WEAK_ON    			0b10000000  //S103
#define CHECK_TRANS_ON    			0b01000000  //S104
#define WATER_IN_FUE_ON    	        0b00100000  //S105
#define AIR_FILTER_CLOGGED_ON    	0b00001000  //S106
#define DPF_ON    					0b00000001  //S22
#define SCR_ON    	        		0b00000010  //S23
#define LOW_DEF_LEVEL_ON    	    0b00000100  //S24  
#define EXHAUST_BREAK_ON    		0b00001000  //S25
#define RETARDER_ON    				0b01000000  //S26
#define GLOW_PLUG_ON    	        0b10000000  //S27 

#define ABS_MIL_OFF    	        	~0b00000100  //S101
#define ESC_MIL_OFF    	        	~0b00001000  //S102 
#define BREAK_WEAK_OFF    			~0b10000000  //S103
#define CHECK_TRANS_OFF    			~0b01000000  //S104
#define WATER_IN_FUE_OFF    	    ~0b00100000  //S105
#define AIR_FILTER_CLOGGED_OFF    	~0b00001000  //S106
#define DPF_OFF    					~0b00000001  //S22
#define SCR_OFF    	        		~0b00000010  //S23
#define LOW_DEF_LEVEL_OFF    	    ~0b00000100  //S24  
#define EXHAUST_BREAK_OFF    		~0b00001000  //S25
#define RETARDER_OFF    			~0b01000000  //S26
#define GLOW_PLUG_OFF    	        ~0b10000000  //S27 


//Light Medium and Heavy macro
#define LIGHT_ON    	        	0b00000001  //S61 
#define MEDIUM_ON    	        	0b00010000  //S60
#define HEAVY_ON    	        	0b00100000  //S59

#define LIGHT_OFF    	        	~0b00000001  //S61 
#define MEDIUM_OFF    	        	~0b00010000  //S60
#define HEAVY_OFF    	        	~0b00100000  //S59

//ODO and Hous sagment are common macros start segment Seg161
//Segments details
//Seg158 & Seg 159 data goes in Byte1
//Seg160 & Seg 161 data goes in Byte2
//Seg162 & Seg 163 data goes in Byte2
//Seg164 & Seg 165 data goes in Byte3
//Seg166 & Seg 167 data goes in Byte4
//Seg168 & Seg 169 data goes in Byte5
//Seg170 & Seg 171 data goes in Byte6
//Seg172 & Seg 173 data goes in Byte7
//Seg174 & Seg 175 data goes in Byte8

//Do not remove below comment for TRIP segment ACB.... record
/*
#define ODO_SEG_A_ON   				0b10000000  //28A
#define ODO_SEG_B_ON   				0b00001000
#define ODO_SEG_C_ON   				0b00000010
#define ODO_SEG_D_ON   				0b00010000
#define ODO_SEG_E_ON   				0b00100000
#define ODO_SEG_F_ON   				0b01000000
#define ODO_SEG_G_ON   				0b00000100
#define ODO_SEG_A_OFF   			~0b10000000
#define ODO_SEG_B_OFF   			~0b00001000
#define ODO_SEG_C_OFF   			~0b00000010
#define ODO_SEG_D_OFF   			~0b00010000
#define ODO_SEG_E_OFF   			~0b00100000
#define ODO_SEG_F_OFF   			~0b01000000
#define ODO_SEG_G_OFF   			~0b00000100
*/

#define ODO_DOT     				0b00000001	//DOT1
#define ODO_CHAR_ON    				0b10000000	//S2
#define ODO_CHAR_OFF   				~0b10000000	//S2
#define ENG_CHAR_ON    				0b00001000	//S3
#define ENG_CHAR_OFF   				~0b00001000	//S3
#define HR_ICON_ON    				0b00000100	//S4
#define HR_ICON_OFF    				~0b00000100	//S4
#define HRS_CHAR_ON     			0b00000010	//S10
#define HRS_CHAR_OFF     			~0b00000010	//S10
#define KM_ON    					0b00000001	//S11
#define KM_OFF    					~0b00000001	//S11
#define HRS_ICON_ON     			0b00000100	//S4
#define HRS_ICON_OFF     			~0b00000100	//S4


//SERVICE MECRO

#define SERVICE_TEXT_ON 			    0b00001000	//S5
#define SERVICE_SYMBOLE_ON 				0b10000000	//S18

//ILLUMINATION MACROS
#define ILL_SYMBOLE_ON 				0b01000000	//S17
#define ILL_BAR1_ON 				0b00100000	//S16
#define ILL_BAR2_ON 				0b00010000	//S15
#define ILL_BAR3_ON 				0b00000001	//S14
#define ILL_BAR4_ON 				0b00000010	//S13
#define ILL_BAR5_ON 				0b00000100	//S12

#define ILL_SYMBOLE_OFF 			~0b01000000	//S17
#define ILL_BAR1_OFF 				~0b00100000	//S16
#define ILL_BAR2_OFF 				~0b00010000	//S15
#define ILL_BAR3_OFF 				~0b00000001	//S14
#define ILL_BAR4_OFF 				~0b00000010	//S13
#define ILL_BAR5_OFF 				~0b00000100	//S12

//Do not remove below comment for TRIP segment ACB.... record
/*
//TRIP Macros SEG36 29A...
#define TRIP_SEG_A_ON   			0b00001000  
#define TRIP_SEG_B_ON   			0b10000000
#define TRIP_SEG_C_ON   			0b00100000
#define TRIP_SEG_D_ON   			0b00000001
#define TRIP_SEG_E_ON   			0b00000010
#define TRIP_SEG_F_ON   			0b00000100 
#define TRIP_SEG_G_ON   			0b01000000

#define TRIP_SEG_A_OFF   			~0b00001000  
#define TRIP_SEG_B_OFF    			~0b10000000
#define TRIP_SEG_C_OFF    			~0b00100000
#define TRIP_SEG_D_OFF    			~0b00000001
#define TRIP_SEG_E_OFF    			~0b00000010
#define TRIP_SEG_F_OFF    			~0b00000100
#define TRIP_SEG_G_OFF    			~0b01000000
*/
#define TRIP_DOT     				0b00010000		//DOT2
#define TRIP_CHAR_ON    			0b00001000  	//S6	
#define TRIP_CHAR_OFF   			~0b00001000  	//S6	
#define A_CHAR_ON   				0b00000100  	//S7
#define A_CHAR_OFF   				~0b00000100  	//S7
#define B_CHAR_ON   				0b00000010  	//S8
#define B_CHAR_OFF   				~0b00000010  	//S8
#define DTE_CHAR_ON    				0b00000001		//S9
#define DTE_CHAR_OFF   				~0b00000001		//S9
#define TRIP_HRS_CHAR_ON    		0b10000000		//S19
#define TRIP_HRS_CHAR_OFF   		~0b10000000		//S19
#define TRIP_KM_ON    				0b00010000		//S20
#define TRIP_KM_OFF    				~0b00010000		//S20
//Clock Macros , segment macro is common for BCO - battery volt,Clock,OAT
#define BCO_SEG_A_ON   				0b00000001  
#define BCO_SEG_B_ON   				0b00100000  
#define BCO_SEG_C_ON   				0b10000000    
#define BCO_SEG_D_ON   				0b00001000  
#define BCO_SEG_E_ON   				0b00000100  
#define BCO_SEG_F_ON   				0b00000010   
#define BCO_SEG_G_ON   				0b01000000  

#define BCO_SEG_A_OFF   			~0b00000001  
#define BCO_SEG_B_OFF   			~0b00100000
#define BCO_SEG_C_OFF   			~0b10000000
#define BCO_SEG_D_OFF   			~0b00001000
#define BCO_SEG_E_OFF   			~0b00000100
#define BCO_SEG_F_OFF   			~0b00000010 
#define BCO_SEG_G_OFF   			~0b01000000

#define AM_SEG_ON       			0b00000010 //S115
#define PM_SEG_ON       			0b00000100 //S116

#define AM_SEG_OFF       			~0b00000010
#define PM_SEG_OFF       			~0b00000100

#define CLOCK_DOT_ON      			0b00010000 //DOT4
#define CLOCK_DOT_OFF     			~0b00010000 //DOT4

//Battery Voltage macros
#define BATT_SYMBOLE_ON    			0b00000100 //S107
#define BATT_SYMBOLE_OFF   			~0b00000100

#define V_SYMBOLE_ON       			0b10000000
#define V_SYMBOLE_OFF      			~0b10000000


#define BATT_SEG_A_ON   			0b00010000  
#define BATT_SEG_B_ON   			0b00000010  
#define BATT_SEG_C_ON   			0b00001000    
#define BATT_SEG_D_ON   			0b10000000  
#define BATT_SEG_E_ON   			0b01000000  
#define BATT_SEG_F_ON   			0b00100000    
#define BATT_SEG_G_ON   			0b00000100    

#define BATT_SEG_A_OFF   			~0b00010000  
#define BATT_SEG_B_OFF   			~0b00000010  
#define BATT_SEG_C_OFF   			~0b00001000    
#define BATT_SEG_D_OFF   			~0b10000000  
#define BATT_SEG_E_OFF   			~0b01000000  
#define BATT_SEG_F_OFF   			~0b00100000   
#define BATT_SEG_G_OFF   			~0b00000100 


#define OAT_SYMBOLE_ON           	0b00001000 
#define OAT_SYMBOLE_OFF         	~0b00001000 
#define DEGREE_SYMBOLE_ON        	0b00000010   //S28 degree centigrade
#define DEGREE_SYMBOLE_OFF      	~0b00000010

//AFE macros
#define AFE_KG_SYMBOLE_ON           0b00010000   //M5
#define AFE_L_SYMBOLE_ON            0b00100000   //S63
#define AFE_SLASH_SYMBOLE_ON        0b01000000   //S62 km "/"
#define AFE_SLASH1_SYMBOLE_ON       0b00000001   //M4 Lit "/"
#define AFE_LIT_SYMBOLE_ON          0b00000010   //S64
#define AFE_KM_SYMBOLE_ON           0b00000100   //S65
#define AFE_SYMBOLE_ON              0b00000001   //S66
#define AFE_DOT_SYMBOLE_ON          0b00001000   //DOT3

#define AFE_KG_SYMBOLE_OFF           ~0b00010000   //M5
#define AFE_L_SYMBOLE_OFF            ~0b00100000   //S63
#define AFE_SLASH_SYMBOLE_OFF        ~0b01000000   //S62 km "/"
#define AFE_SLASH1_SYMBOLE_OFF       ~0b00000001   //M4 Lit "/"
#define AFE_LIT_SYMBOLE_OFF          ~0b00000010   //S64
#define AFE_KM_SYMBOLE_OFF           ~0b00000100   //S65
#define AFE_SYMBOLE_OFF              ~0b00000001   //S66
#define AFE_DOT_SYMBOLE_OFF          ~0b00000001   //DOT3



#define AFE_SEG_A_ON   				0b00010000  
#define AFE_SEG_B_ON   				0b00100000  
#define AFE_SEG_C_ON   				0b01000000    
#define AFE_SEG_D_ON   				0b10000000  
#define AFE_SEG_E_ON   				0b00000100  
#define AFE_SEG_F_ON   				0b00000001    
#define AFE_SEG_G_ON   				0b00000010

#define AFE_SEG_A_OFF   			~0b00010000  
#define AFE_SEG_B_OFF   			~0b00100000  
#define AFE_SEG_C_OFF   			~0b01000000    
#define AFE_SEG_D_OFF   			~0b10000000  
#define AFE_SEG_E_OFF   			~0b00000100  
#define AFE_SEG_F_OFF   			~0b00000001    
#define AFE_SEG_G_OFF   			~0b00000010

//AMT/AT/AUTO/MANUAL/CRAWL/CREEP/POWER/ECO and AMT seg          Macros    
#define AMT_CHAR_ON					0b00000010 //S113  
#define AT_CHAR_ON					0b00000100 //S112
#define AUTO_CHAR_ON				0b00100000 //S111
#define MANUAL_CHAR_ON				0b00000100 //S110
#define CRAWL_CHAR_ON				0b00001000 //S109
#define CREEP_CHAR_ON				0b00100000 //S58
#define POWER_CHAR_ON				0b01000000 //S57
#define ECO_CHAR_ON					0b10000000 //S56

#define AMT_CHAR_OFF				~0b00000010 //S113  
#define AT_CHAR_OFF					~0b00000100 //S112
#define AUTO_CHAR_OFF				~0b00100000 //S111
#define MANUAL_CHAR_OFF				~0b00000100 //S110
#define CRAWL_CHAR_OFF				~0b00000010 //S109
#define CREEP_CHAR_OFF				~0b00000010 //S58
#define POWER_CHAR_OFF				~0b00000100 //S57
#define ECO_CHAR_OFF				~0b00001000 //S56


#define AMT_SEG_A_ON   				0b00010000  
#define AMT_SEG_B_ON   				0b00000001  
#define AMT_SEG_C_ON   				0b00001000    
#define AMT_SEG_D_ON   				0b00001000  
#define AMT_SEG_E_ON   				0b10000000  
#define AMT_SEG_F_ON   				0b01000000    
#define AMT_SEG_G_ON   				0b00010000
#define AMT_SEG_H_ON   				0b00000001  
#define AMT_SEG_I_ON   				0b00000010  
#define AMT_SEG_J_ON   				0b00100000    
#define AMT_SEG_K_ON   				0b00000010  
#define AMT_SEG_L_ON   				0b00100000  
#define AMT_SEG_M_ON   				0b00000100    
#define AMT_SEG_N_ON   				0b00000100
#define AMT_SEG_O_ON   				0b01000000    
#define AMT_SEG_P_ON   				0b10000000

#define AMT_SEG_A_OFF   			~0b00010000  
#define AMT_SEG_B_OFF   			~0b00000001  
#define AMT_SEG_C_OFF   			~0b00001000    
#define AMT_SEG_D_OFF   			~0b00001000  
#define AMT_SEG_E_OFF   			~0b10000000  
#define AMT_SEG_F_OFF   			~0b01000000    
#define AMT_SEG_G_OFF   			~0b00010000
#define AMT_SEG_H_OFF   			~0b00000001  
#define AMT_SEG_I_OFF   			~0b00000010  
#define AMT_SEG_J_OFF   			~0b00100000    
#define AMT_SEG_K_OFF   			~0b00000010  
#define AMT_SEG_L_OFF   			~0b00100000  
#define AMT_SEG_M_OFF   			~0b00000100    
#define AMT_SEG_N_OFF   			~0b00000100
#define AMT_SEG_O_OFF   			~0b01000000    
#define AMT_SEG_P_OFF   			~0b10000000
#define CLEAR_DATA					0x0D

//TPMS MACRO
#define TPMS_CHAR_ON   				0b01000000
#define TPMS_CHAR_OFF  				~0b01000000

//Service Reminder


// DTC FAULT CODES




#define EOL_PENDING 		0
#define SELF_TEST_PROGRESS 	1
#define NORMAL_MODE		 	2
#define CLUSTER_ENTER_SLEEP		 	3



u8 gu8TopLineMsg = 0;
u8 u8DispCount = 0;
eCLUSTER_STATE_MACHINE clusterStatuslcd;


//AMT/AT Charactor used byte for IIC
typedef enum
{ 
	eAMT_AT_CHAR_CONTROL_BYTE_COMMAND = 0,
	eAMT_AT_CHAR_COLUMN_ADD,
	eAMT_AT_CHAR_PARAMETR_ADD,
	eAMT_AT_CHAR_ADD,
	eAMT_AT_CHAR_DISP_COMMAND, 
	eAMT_AT_CHAR_BYTE1,
	eAMT_AT_CHAR_NUM_OF_BYTE
}eAMT_AT_IIC_BYTE_TYPE;

//AUTO Charactor used byte for IIC
typedef enum
{ 
	eAUTO_CHAR_CONTROL_BYTE_COMMAND = 0,
	eAUTO_CHAR_COLUMN_ADD,
	eAUTO_CHAR_PARAMETR_ADD,
	eAUTO_CHAR_ADD,
	eAUTO_CHAR_DISP_COMMAND, 
	eAUTO_CHAR_BYTE1,
	eAUTO_CHAR_NUM_OF_BYTE
}eAUTO_CHAR_IIC_BYTE_TYPE;


//AMT Area used byte for IIC(MANUAL,CRAWAL,CREEP,POWER,ECO,AMT SEG)
typedef enum
{ 
	eAMT_DATA_CONTROL_BYTE_COMMAND = 0,
	eAMT_DATA_COLUMN_ADD,
	eAMT_DATA_PARAMETR_ADD,
	eAMT_DATA_ADD,
	eAMT_DATA_DISP_COMMAND, 
	eAMT_DATA_BYTE1,
	eAMT_DATA_BYTE2,
	eAMT_DATA_BYTE3,
	eAMT_DATA_BYTE4,
	eAMT_DATA_BYTE5,
	eAMT_DATA_BYTE6,
	eAMT_DATA_NUM_OF_BYTE
}eAMT_DATA_IIC_BYTE_TYPE;

//AFE used byte for IIC
typedef enum
{ 
	eAFE_CONTROL_BYTE_COMMAND = 0,
	eAFE_COLUMN_ADD,
	eAFE_PARAMETR_ADD,
	eAFE_ADD,
	eAFE_DISP_COMMAND, 
	eAFE_BYTE1,
	eAFE_BYTE2,
	eAFE_BYTE3,
	eAFE_BYTE4,
	eAFE_NUM_OF_BYTE
}eAFE_IIC_BYTE_TYPE;

//TPMS String used byte for IIC
typedef enum
{ 
	eTPMS_STRING_CONTROL_BYTE_COMMAND = 0,
	eTPMS_STRING_COLUMN_ADD,
	eTPMS_STRING_PARAMETR_ADD,
	eTPMS_STRING_ADD,
	eTPMS_STRING_DISP_COMMAND, 
	eTPMS_STRING_BYTE1, 
	eTPMS_STRING_BYTE2,
	eTPMS_STRING_BYTE3,
	eTPMS_STRING_BYTE4,
	eTPMS_STRING_BYTE5, 
	eTPMS_STRING_BYTE6,
	eTPMS_STRING_BYTE7,
	eTPMS_STRING_BYTE8,
	eTPMS_STRING_BYTE9, 
	eTPMS_STRING_BYTE10,
	eTPMS_STRING_BYTE11,
	eTPMS_STRING_BYTE12,
	eTPMS_STRING_BYTE13, 
	eTPMS_STRING_BYTE14,
	eTPMS_STRING_BYTE15,
	eTPMS_STRING_BYTE16,
	eTPMS_STRING_BYTE17, 
	eTPMS_STRING_BYTE18,
	eTPMS_STRING_BYTE19,
	eTPMS_STRING_BYTE20,
	eTPMS_STRING_BYTE21,
	eTPMS_STRING_BYTE22,
	eTPMS_STRING_BYTE23,
	eTPMS_STRING_BYTE24,
	eTPMS_STRING_BYTE25,
	eTPMS_STRING_BYTE26, 
	eTPMS_STRING_BYTE27,
	eTPMS_STRING_BYTE28,
	eTPMS_STRING_BYTE29,
	eTPMS_STRING_BYTE30, 
	eTPMS_STRING_BYTE31,
	eTPMS_STRING_BYTE32,
	eTPMS_STRING_BYTE33,
	eTPMS_STRING_BYTE34,
	eTPMS_STRING_BYTE35,
	eTPMS_STRING_BYTE36,
	eTPMS_STRING_NUM_OF_BYTE
}eTPMS_STRING_IIC_BYTE_TYPE;

//TPMS Charactor used byte for IIC
typedef enum
{
	eTPMS_CHAR_CONTROL_BYTE_COMMAND = 0,
	eTPMS_CHAR_COLUMN_ADD,
	eTPMS_CHAR_PARAMETR_ADD,
	eTPMS_CHAR_ADD,
	eTPMS_CHAR_DISP_COMMAND, 
	eTPMS_CHAR_BYTE1,
	eTPMS_CHAR_NUM_OF_BYTE
}eTPMS_IIC_BYTE_TYPE;

// Batt volt used byte for IIC
typedef enum
{
	eBATT_VOLT_CONTROL_BYTE_COMMAND = 0,
	eBATT_VOLT_COLUMN_ADD,
	eBATT_VOLT_PARAMETR_ADD,
	eBATT_VOLT_ADD,
	eBATT_VOLT_DISP_COMMAND, 
	eBATT_VOLT_BYTE1, 
	eBATT_VOLT_BYTE2,
	eBATT_VOLT_BYTE3,
	eBATT_VOLT_BYTE4,
	eBATT_VOLT_NUM_OF_BYTE
}eBATT_VOLT_IIC_BYTE_TYPE;


//Degree centigrade symbol used byte for IIC
typedef enum
{
	eDCS_CONTROL_BYTE_COMMAND = 0,
	eDCS_COLUMN_ADD,
	eDCS_PARAMETR_ADD,
	eDCS_ADD,
	eDCS_DISP_COMMAND, 
	eDCS_BYTE1, 
	eDCS_NUM_OF_BYTE
}eDCS_IIC_BYTE_TYPE;


//Outside ambient temperature used byte for IIC
typedef enum
{ 
	eOAT_CONTROL_BYTE_COMMAND = 0,
	eOAT_COLUMN_ADD,
	eOAT_PARAMETR_ADD,
	eOAT_ADD,
	eOAT_DISP_COMMAND, 
	eOAT_BYTE1, 
	eOAT_BYTE2,
	eOAT_BYTE3, 
	eOAT_NUM_OF_BYTE
}eOAT_IIC_BYTE_TYPE;

//CLOCKused byte for IIC
typedef enum
{ 
	eCLOCK_CONTROL_BYTE_COMMAND = 0,
	eCLOCK_COLUMN_ADD,
	eCLOCK_PARAMETR_ADD,
	eCLOCK_ADD,
	eCLOCK_DISP_COMMAND, 
	eCLOCK_BYTE1, 
	eCLOCK_BYTE2,
	eCLOCK_BYTE3, 
	eCLOCK_BYTE4,
	eCLOCK_BYTE5,
	eCLOCK_NUM_OF_BYTE
}eCLOCK_IIC_BYTE_TYPE;

//Illumination used byte for IIC
typedef enum
{
	eIIL_CONTROL_BYTE_COMMAND = 0,
	eIIL_COLUMN_ADD,
	eIIL_PARAMETR_ADD,
	eIIL_ADD,
	eIIL_DISP_COMMAND, 
	eIIL_BYTE1, 
	eIIL_BYTE2,
	eIIL_NUM_OF_BYTE
}eIIL_IIC_BYTE_TYPE;

//Service reminder used byte for IIC
typedef enum
{
	eSERV_CONTROL_BYTE_COMMAND = 0,
	eSERV_COLUMN_ADD,
	eSERV_PARAMETR_ADD,
	eSERV_ADD,
	eSERV_DISP_COMMAND, 
	eSERV_BYTE1, 
	eSERV_BYTE2,
	eSERV_NUM_OF_BYTE
}eSERV_IIC_BYTE_TYPE;	

//LIGHT ,MEDIUM,HEAVY have in same segment
typedef enum
{
	eLMH_CONTROL_BYTE_COMMAND = 0,
	eLMH_COLUMN_ADD,
	eLMH_PARAMETR_ADD,
	eLMH_ADD,
	eLMH_DISP_COMMAND, 
	eLMH_BYTE1, 
	eLMH_NUM_OF_BYTE
}eLMH_IIC_BYTE_TYPE;

//For tell tale ESC and ABS becouse both have in same segment
typedef enum
{ 
	eABS_ESC_CONTROL_BYTE_COMMAND = 0,
	eABS_ESC_COLUMN_ADD,
	eABS_ESC_PARAMETR_ADD,
	eABS_ESC_MIL_ADD,
	eABS_ESC_DISP_COMMAND, 
	eABS_ESC_BYTE1, 
	eABS_ESC_NUM_OF_BYTE
}eABS_ESC_IIC_BYTE_TYPE;
//For tell tale Break Wear ,Check Transmmision and Water in Fuel becouse all have in same segment
typedef enum
{ 
	eBCW_CONTROL_BYTE_COMMAND = 0,
	eBCW_COLUMN_ADD,
	eBCW_PARAMETR_ADD,
	eBCW_ADD,
	eBCW_DISP_COMMAND, 
	eBCW_BYTE1, 
	eBCW_NUM_OF_BYTE
}eBCW_IIC_BYTE_TYPE;
// for tell tale AIR FILTER CLOGGED
typedef enum
{
	eAFC_CONTROL_BYTE_COMMAND = 0,
	eAFC_COLUMN_ADD,
	eAFC_PARAMETR_ADD,
	eAFC_ADD,
	eAFC_DISP_COMMAND, 
	eAFC_BYTE1, 
	eAFC_NUM_OF_BYTE
}eAFC_IIC_BYTE_TYPE;

//DPF,SCR,Low def level,Exhaust break
typedef enum
{
	eDSDE_CONTROL_BYTE_COMMAND = 0,
	eDSDE_COLUMN_ADD,
	eDSDE_PARAMETR_ADD,
	eDSDE_ADD,
	eDSDE_DISP_COMMAND, 
	eDSDE_BYTE1, 
	eDSDE_NUM_OF_BYTE
}eDSDE_IIC_BYTE_TYPE;

//Retarder and Glow plug
typedef enum
{
	eRG_CONTROL_BYTE_COMMAND = 0,
	eRG_COLUMN_ADD,
	eRG_PARAMETR_ADD,
	eRG_ADD,
	eRG_DISP_COMMAND, 
	eRG_BYTE1, 
	eRG_NUM_OF_BYTE
}eRG_IIC_BYTE_TYPE;



typedef enum
{
	eDEF_CONTROL_BYTE_COMMAND = 0,
	eDEF_COLUMN_ADD,
	eDEF_PARAMETR_ADD,
	eDEF_SEG_ADD,
	eDEF_DISP_COMMAND, 
	eDEF_BYTE1,
	eDEF_BYTE2,
	eDEF_BYTE3,
	eDEF_BYTE4,
	eDEF_NUM_OF_BYTE
}eDEF_IIC_BYTE_TYPE;

typedef enum
{
	eOIL_CONTROL_BYTE_COMMAND = 0,
	eOIL_COLUMN_ADD,
	eOIL_PARAMETR_ADD,
	eOIL_SEG_ADD,
	eOIL_DISP_COMMAND, 
	eOIL_BYTE1,
	eOIL_BYTE2,
	eOIL_BYTE3, 
	eOIL_NUM_OF_BYTE
}eOIL_IIC_BYTE_TYPE;

typedef enum
{ 
	eAPG_CONTROL_BYTE_COMMAND = 0,
	eAPG_COLUMN_ADD,
	eAPG_PARAMETR_ADD,
	eAPG_SEG_ADD,
	eAPG_DISP_COMMAND, 
	eAPG_BYTE1,
	eAPG_BYTE2,
	eAPG_BYTE3, 
	eAPG_BYTE4,
	eAPG_BYTE5,
	eAPG_NUM_OF_BYTE
}eAPG_IIC_BYTE_TYPE;

typedef enum
{ 
	eRAPG_CONTROL_BYTE_COMMAND = 0,
	eRAPG_COLUMN_ADD,
	eRAPG_PARAMETR_ADD,
	eRAPG_SEG_ADD,
	eRAPG_DISP_COMMAND, 
	eRAPG_BYTE1,
	eRAPG_BYTE2,
	eRAPG_BYTE3, 
	eRAPG_NUM_OF_BYTE
}eRAPG_IIC_BYTE_TYPE;

typedef enum
{	
	eLINE_CONTROL_BYTE_COMMAND1 = 0,//Start bytes for ABOVE_DEF_OIL_BOTTOM_LINE
	eLINE_COLUMN_ADD1,
	eLINE_PARAMETR_ADD1,
	eABOVE_DEF_OIL_BOTTOM_LINE_ADD,
	eLINE_DISP_COMMAND1, 
	eLINE_BYTE1,
	eLINE_NUM_OF_BYTE1
}eLINE_IIC_BYTE_TYPE1;

typedef enum
{		
	eLINE_CONTROL_BYTE_COMMAND2 = 0,//Start bytes for LINE_ABOVE_APG
	eLINE_COLUMN_ADD2,
	eLINE_PARAMETR_ADD2,
	eLINE_ABOVE_APG_ADD,
	eLINE_DISP_COMMAND2, 	 
	eLINE_BYTE2,
	eLINE_NUM_OF_BYTE2
}eLINE_IIC_BYTE_TYPE2;

typedef enum
{  		 
	eLINE_CONTROL_BYTE_COMMAND3=0,//Start bytes for LINE_TOP	
	eLINE_COLUMN_ADD3,
	eLINE_PARAMETR_ADD3,
	eLINE_TOP_ADD,
	eLINE_DISP_COMMAND3, 
	eLINE_BYTE3,	                //END
	eLINE_NUM_OF_BYTE3
}eLINE_IIC_BYTE_TYPE3;

typedef enum
{
	eODO_CONTROL_BYTE_COMMAND = 0,
	eODO_COLUMN_ADD,
	eODO_PARAMETR_ADD,
	eODO_SEG_ADD,
	eODO_DISP_COMMAND, 
	eODO_BYTE1,
	eODO_BYTE2,
	eODO_BYTE3, 
	eODO_BYTE4,
	eODO_BYTE5,
	eODO_BYTE6,
	eODO_BYTE7,
	eODO_BYTE8,
	eODO_BYTE9,
	eODO_NUM_OF_BYTE
}eODO_IIC_BYTE_TYPE;

typedef enum
{ 
	eTRIP_CONTROL_BYTE_COMMAND = 0,
	eTRIP_COLUMN_ADD,
	eTRIP_PARAMETR_ADD,
	eTRIP_SEG_ADD,
	eTRIP_DISP_COMMAND, 
	eTRIP_BYTE1,
	eTRIP_BYTE2,
	eTRIP_BYTE3, 
	eTRIP_BYTE4,
	eTRIP_BYTE5,
	eTRIP_BYTE6,
	eTRIP_NUM_OF_BYTE
}eTRIP_IIC_BYTE_TYPE;

//TPMS used byte in LCD RAM
typedef enum
{
	eTPMS_RAM_ADD_BYTE 			= TPMS_CHAR_ADD/2, //Starting byte address
	eTPMS_STRING_RAM_ADD_BYTE1 	= TPMS_STRING_ADD/2,
	eTPMS_STRING_RAM_ADD_BYTE2,
	eTPMS_STRING_RAM_ADD_BYTE3,
	eTPMS_STRING_RAM_ADD_BYTE4,
	eTPMS_STRING_RAM_ADD_BYTE5,
	eTPMS_STRING_RAM_ADD_BYTE6,
	eTPMS_STRING_RAM_ADD_BYTE7,
	eTPMS_STRING_RAM_ADD_BYTE8,
	eTPMS_STRING_RAM_ADD_BYTE9,
	eTPMS_STRING_RAM_ADD_BYTE10,
	eTPMS_STRING_RAM_ADD_BYTE11,
	eTPMS_STRING_RAM_ADD_BYTE12,
	eTPMS_STRING_RAM_ADD_BYTE13,
	eTPMS_STRING_RAM_ADD_BYTE14,
	eTPMS_STRING_RAM_ADD_BYTE15,
	eTPMS_STRING_RAM_ADD_BYTE16,
	eTPMS_STRING_RAM_ADD_BYTE17,
	eTPMS_STRING_RAM_ADD_BYTE18,
	eTPMS_STRING_RAM_ADD_BYTE19,
	eTPMS_STRING_RAM_ADD_BYTE20,
	eTPMS_STRING_RAM_ADD_BYTE21,
	eTPMS_STRING_RAM_ADD_BYTE22,
	eTPMS_STRING_RAM_ADD_BYTE23,
	eTPMS_STRING_RAM_ADD_BYTE24,
	eTPMS_STRING_RAM_ADD_BYTE25,
	eTPMS_STRING_RAM_ADD_BYTE26,
	eTPMS_STRING_RAM_ADD_BYTE27,
	eTPMS_STRING_RAM_ADD_BYTE28,
	eTPMS_STRING_RAM_ADD_BYTE29,
	eTPMS_STRING_RAM_ADD_BYTE30,
	eTPMS_STRING_RAM_ADD_BYTE31,
	eTPMS_STRING_RAM_ADD_BYTE32,
	eTPMS_STRING_RAM_ADD_BYTE33,
	eTPMS_STRING_RAM_ADD_BYTE34,
	eTPMS_STRING_RAM_ADD_BYTE35,
	eTPMS_STRING_RAM_ADD_BYTE36	    
}eTPMS_RAM_ADD_TYPE;



//AFE used byte in LCD RAM
typedef enum
{
	eAMT_AT_RAM_ADD_BYTE 	= AMT_AT_ADD/2, //Starting byte address
	eAUTO_RAM_ADD_BYTE 		= AUTO_ADD/2,
	eAMT_RAM_ADD_BYTE1		= AMT_SEG_ADD/2,
	eAMT_RAM_ADD_BYTE2,
	eAMT_RAM_ADD_BYTE3,
	eAMT_RAM_ADD_BYTE4,
	eAMT_RAM_ADD_BYTE5,
	eAMT_RAM_ADD_BYTE6 	    	    	    	   
}eAMT1_RAM_ADD_TYPE;




//AFE used byte in LCD RAM
typedef enum
{
	eAFE_RAM_ADD_BYTE1 = AFE_ADD/2, //Starting byte address
	eAFE_RAM_ADD_BYTE2, 
	eAFE_RAM_ADD_BYTE3,		  
	eAFE_RAM_ADD_BYTE4,
}eAFE_RAM_ADD_TYPE;


//Degree centigrade symbole used byte in LCD RAM
typedef enum
{
	eDCS_RAM_ADD_BYTE1 = DEGREE_CENTIGRADE_ADD/2, //Starting byte address
}eDCS_RAM_ADD_TYPE;



//DEF bytes address in LCD RAM
typedef enum
{
	eCLOCK_RAM_ADD_BYTE1 = CLOCK_ADD/2, //Starting byte address
	eCLOCK_RAM_ADD_BYTE2,
	eCLOCK_RAM_ADD_BYTE3,
	eCLOCK_RAM_ADD_BYTE4,
	eCLOCK_RAM_ADD_BYTE5
}eCLOCK_RAM_ADD_TYPE;
//DEF bytes address in LCD RAM
typedef enum
{
	eDEF_RAM_ADD_BYTE1 = DEF_SEG_ADD/2, //Starting byte address
	eDEF_RAM_ADD_BYTE2,
	eDEF_RAM_ADD_BYTE3,
	eDEF_RAM_ADD_BYTE4		
}eDEF_RAM_ADD_TYPE;

//OIL bytes address in LCD RAM
typedef enum
{
	eOIL_RAM_ADD_BYTE1 = OIL_SEG_ADD/2, //Starting byte address
	eOIL_RAM_ADD_BYTE2,
	eOIL_RAM_ADD_BYTE3,			
}eOIL_RAM_ADD_TYPE;

//FRONT APG bytes address in LCD RAM
typedef enum
{
	eAPG_RAM_ADD_BYTE1 = FRONT_AP_SEG_ADD/2, //Starting byte address
	eAPG_RAM_ADD_BYTE2,
	eAPG_RAM_ADD_BYTE3,			
	eAPG_RAM_ADD_BYTE4,
	eAPG_RAM_ADD_BYTE5,
}eAPG_RAM_ADD_TYPE;   
//REAR APG bytes address in LCD RAM
typedef enum
{
	eRAPG_RAM_ADD_BYTE1 = REAR_AP_SEG_ADD/2, //Starting byte address
	eRAPG_RAM_ADD_BYTE2,
	eRAPG_RAM_ADD_BYTE3,	
}eRAPG_RAM_ADD_TYPE;       
//Decorted LINE bytes address in LCD RAM      
typedef enum
{
	eLINE_RAM_ADD_BYTE1 = ABOVE_DEF_OIL_BOTTOM_LINE_ADD/2,  
	eLINE_RAM_ADD_BYTE2 = LINE_ABOVE_APG_ADD/2,	
	eLINE_RAM_ADD_BYTE3 = LINE_TOP_ADD/2,	
}eLINE_RAM_ADD_TYPE;
//ODO bytes address in LCD RAM
typedef enum
{
	eODO_RAM_ADD_BYTE1 = ODO_ADD/2, //Starting byte address
	eODO_RAM_ADD_BYTE2,
	eODO_RAM_ADD_BYTE3,
	eODO_RAM_ADD_BYTE4,
	eODO_RAM_ADD_BYTE5,
	eODO_RAM_ADD_BYTE6,
	eODO_RAM_ADD_BYTE7,
	eODO_RAM_ADD_BYTE8,
	eODO_RAM_ADD_BYTE9,
}eODO_RAM_ADD_TYPE; 

//BAtt Voltage bytes address in LCD RAM
typedef enum
{
	eBATT_VOLT_RAM_ADD_BYTE1 = BATT_VOLT_ADD/2, //Starting byte address
	eBATT_VOLT_RAM_ADD_BYTE2,
	eBATT_VOLT_RAM_ADD_BYTE3,	
	eBATT_VOLT_RAM_ADD_BYTE4,
}eBATT_VOLT_RAM_ADD_TYPE;

//BAtt Voltage bytes address in LCD RAM
typedef enum
{
	eOAT_RAM_ADD_BYTE1 = OAT_ADD/2, //Starting byte address
	eOAT_RAM_ADD_BYTE2,
	eOAT_RAM_ADD_BYTE3,  
}eOAT_RAM_ADD_TYPE;

//ODO bytes address in LCD RAM
typedef enum
{
	eTRIP_RAM_ADD_BYTE1 = TRIP_ADD/2, //Starting byte address
	eTRIP_RAM_ADD_BYTE2,
	eTRIP_RAM_ADD_BYTE3,
	eTRIP_RAM_ADD_BYTE4,
	eTRIP_RAM_ADD_BYTE5,
	eTRIP_RAM_ADD_BYTE6,	
}eTRIP_RAM_ADD_TYPE;

//ILL bytes address in LCD RAM
typedef enum
{
	eILL_RAM_ADD_BYTE1 = ILL_ADD/2, //Starting byte address
	eILL_RAM_ADD_BYTE2,				
}eILL_RAM_ADD_TYPE;

//ILL bytes address in LCD RAM
typedef enum
{
	eSERVICE_RAM_ADD_BYTE1 = SERVICE_ADD/2, //Starting byte address
	eSERVICE_RAM_ADD_BYTE2,				
}eILL_RAM_ADD_TYPE;


typedef enum
{
	eBar0 = 0,
	eBar1,
	eBar2,
	eBar3,
	eBar4,
	eBar5,
	eBar6,
	eBar7,
}eBAR_TYPE;









typedef enum
{
	eTripDisplay,		
	eOdoDisplay,
	eDefBarDisplay,
	eOilBarDisplay,
	eAPRearBarDisplay,
	eAPFrontBarDisplay,
	eIllDisplay,
	eServDisplay,
	eClockDisplay,
	eOATDisplay,
	eBattVoltDisplay,
	eABSMilDisplay,
	eEscMilDisplay,
	eBreakWearDisplay,
	eCheckTransDisplay,
	eWaterInFuelDisplay,
	eAirFilterCloggedDisplay,
	eDpfDisplay,
	eScrDisplay,
	eLowDefDisplay,
	eExhaustBreakDisplay,
	eRetarderDisplay,
	eEngPreHeatDisplay,	
	eLMHDisplay,
	eDecorateBoundry1Display,
	eDecorateBoundry2Display,
	eDecorateBoundry3Display,
	eDegreeSymboleDisplay,
	eAfeDisplay,
	eAmtAtDisplay,
	eAutoDisplay,
	eAmtDataDisplay,
	eTpmsCharDisplay,
	eTpmsStringDisplay
}eLCDBlockType;

typedef struct 
{
	u32 SPNValue;
	u8 FMICode;
}DummyDTC;


// local declarations --------------------------------------------------------	

//Address of segment in LcdRAM = LCDSegmentAddress/2 
//Even segment RHS nibble and Odd segment LSB nibble two seg data merge in 1 byte
u8 u8LcdRAM[LCD_SEGMENT]=
{	
	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	

	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	

};

//Buffer to store IIC  data 
u8 u8AmtBuffer[eAMT_DATA_NUM_OF_BYTE]; 				 
u8 u8AmtAtCharBuffer[eAMT_AT_CHAR_NUM_OF_BYTE]; 	 
u8 u8AutoCharBuffer[eAUTO_CHAR_NUM_OF_BYTE]; 		 
u8 u8AfeBuffer[eAFE_NUM_OF_BYTE]; 					 
u8 u8BattVoltBuffer[eBATT_VOLT_NUM_OF_BYTE]; 		 
u8 u8TpmsCharBuffer[eTPMS_CHAR_NUM_OF_BYTE]; 		 
u8 u8TpmsStringBuffer[eTPMS_STRING_NUM_OF_BYTE]; 	 
u8 u8ClockBuffer[eCLOCK_NUM_OF_BYTE]; 				 
u8 u8OATBuffer[eOAT_NUM_OF_BYTE]; 					//Buffer for Outsite Ambiant Temperature
u8 u8DCSBuffer[eDCS_NUM_OF_BYTE]; 					//Buffer for Degree Centigrade symbole
u8 u8IllBuffer[eIIL_NUM_OF_BYTE]; 					//Buffer for Illumination 
u8 u8ServBuffer[eSERV_NUM_OF_BYTE]; 				//Buffer for Illumination 
u8 u8LMHBuffer[eLMH_NUM_OF_BYTE]; 					//Buffer for Light medium Heavy
u8 u8AbsEscBuffer[eABS_ESC_NUM_OF_BYTE]; 			//Buffer for ABS and ESC
u8 u8BCWBuffer[eBCW_NUM_OF_BYTE];     	 			//Buffer for Break Wear ,check transmmision & water in fuel
u8 u8AFCBuffer[eAFC_NUM_OF_BYTE]; 	 				//Buffer for Air filter clogged
u8 u8DSDEBuffer[eDSDE_NUM_OF_BYTE]; 	 			//Buffer for DPF,SCR,Low def level,Exhaust break
u8 u8RGBuffer[eRG_NUM_OF_BYTE]; 	 				//Buffer for Retarder and Glow plug
u8 u8DefBuffer[eDEF_NUM_OF_BYTE];
u8 u8OilBuffer[eOIL_NUM_OF_BYTE];
u8 u8FApgBuffer[eAPG_NUM_OF_BYTE];
u8 u8RApgBuffer[eRAPG_NUM_OF_BYTE];
u8 u8OdoBuffer[eODO_NUM_OF_BYTE];
u8 u8TripBuffer[eTRIP_NUM_OF_BYTE];
u8 u8DecorateBound1[eLINE_NUM_OF_BYTE1];
u8 u8DecorateBound2[eLINE_NUM_OF_BYTE2];
u8 u8DecorateBound3[eLINE_NUM_OF_BYTE3];

//Buffer for keep record of previous data so that if data change then send to IIC 
u8 u8PrevAmtBuffer[eAMT_DATA_NUM_OF_BYTE]; 				 
u8 u8PrevAmtAtCharBuffer[eAMT_AT_CHAR_NUM_OF_BYTE]; 	 
u8 u8PrevAutoCharBuffer[eAUTO_CHAR_NUM_OF_BYTE]; 		 
u8 u8PrevAfeBuffer[eAFE_NUM_OF_BYTE]; 					 
u8 u8PrevBattVoltBuffer[eBATT_VOLT_NUM_OF_BYTE]; 		 
u8 u8PrevTPMSBuffer[eTPMS_CHAR_NUM_OF_BYTE]; 			 
u8 u8PrevTpmsStringBuffer[eTPMS_STRING_NUM_OF_BYTE]; 	 
u8 u8PrevClockBuffer[eCLOCK_NUM_OF_BYTE]; 				 
u8 u8PrevOATBuffer[eOAT_NUM_OF_BYTE]; 					//Buffer for Outsite Ambiant Temperature
u8 u8PrevDCSBuffer[eDCS_NUM_OF_BYTE]; 					//Buffer for Degree Centigrade symbole
u8 u8PrevIllBuffer[eIIL_NUM_OF_BYTE]; 					//Buffer for Illumination 
u8 u8PrevServBuffer[eSERV_NUM_OF_BYTE]; 				//Buffer for SERVICE REMINDER 
u8 u8PrevLMHBuffer[eLMH_NUM_OF_BYTE]; 					//Buffer for Light medium Heavy
u8 u8PrevAbsEscBuffer[eABS_ESC_NUM_OF_BYTE]; 			//Buffer for ABS and ESC
u8 u8PrevBCWBuffer[eBCW_NUM_OF_BYTE];     	 			//Buffer for Break Wear ,check transmmision & water in fuel
u8 u8PrevAFCBuffer[eAFC_NUM_OF_BYTE]; 	 				//Buffer for Air filter clogged
u8 u8PrevDSDEBuffer[eDSDE_NUM_OF_BYTE]; 	 			//Buffer for DPF,SCR,Low def level,Exhaust break
u8 u8PrevRGBuffer[eRG_NUM_OF_BYTE]; 	 				//Buffer for Retarder and Glow plug
u8 u8PrevDefBuffer[eDEF_NUM_OF_BYTE];
u8 u8PrevOilBuffer[eOIL_NUM_OF_BYTE];
u8 u8PrevFApgBuffer[eAPG_NUM_OF_BYTE];
u8 u8PrevRApgBuffer[eRAPG_NUM_OF_BYTE];
u8 u8PrevOdoBuffer[eODO_NUM_OF_BYTE];
u8 u8PrevTripBuffer[eTRIP_NUM_OF_BYTE];
u8 u8PrevDecorateBound1[eLINE_NUM_OF_BYTE1];
u8 u8PrevDecorateBound2[eLINE_NUM_OF_BYTE2];
u8 u8PrevDecorateBound3[eLINE_NUM_OF_BYTE3];
u8 u8GearArray[2];

eTOP_LINE_PRIORITY eTopLinePriority = eBlank;
eTOP_LINE_SUB_PRIORITY eTopLineSubPriority = eNoSubPriority;

static u8 u8TopLineText = 0;

const u8 u8subpriority1[eTotalSP1Faults][18]=  
{	"",	"  AUTO REGEN ON   ", "MANUAL RGN ACTIVE ",
	"MANUAL RGN COMPLET", "MANUAL REGEN ABORT",	"   AMT LEARNING   ", 
	"  RANGE INHIBIT   ", "     WELCOME      ", "   RAMP ENGAGED   ", 
	"  KNEELING FRONT  ", "  KNEELING REAR   ", "  KNEELING BOTH   ", 
	"HALT RQST PRIORITY", " STEERING ADJUST  ", "  KNEELING RIGHT  ", 
	"  KNEELING LEFT   ", "    HSA ACTIVE    ", "  IC EOL PENDING  "
};							   			

const u8 u8SubPriority2[eTotalSP2Faults][18]=
{	"",      
	"    EGR STOPPED   ", "DEF DOSING STOPPED", " DEF DOSING FAULT ", "DEF QUALITY NOT OK", 
	"DEF SYS ELEC FAULT", "DEF/SCR SYS FAULT ", "NO DOSING LOW TEMP", " DEF LOW | REFILL ",
	"     DEF XX %     ", "DEF V LOW | REFILL", "  DEF NEAR EMPTY  ",  
	"DEF EMPTY | REFILL", " CHECK ALTERNATOR ", "  CHECK BATTERY   ", "REGEN STOP PRESSED", 
	"AUTO REGEN STOPPED", " DO MANUAL REGEN  ", "   STOP ENGINE    ", "  VISIT WORKSHOP  ", 
	"  REGEN REQUIRED  ", "    HSA ERROR     ", "   HALT REQUEST   ", "  ENGINE OIL LOW  ", 
	"ENGINE COOLANT LOW", "  EMERGENCY EXIT  ", "  REAR FLAP OPEN  ", "  SIDE FLAP OPEN  ", 
	"    DOOR OPEN     "
};
	
//u8 u8EMSVariableString[3][18]= {};
u8 u8VariableString[4][18]=    {"PULSES/KM         ", "SW VERSION V      ", 
								"     NO FAULT     ", "PULSES/KM         "};



//Collection of data  0 1 2 3..  digits making on 16 segment 
//16 bit formate P	O	J	A	C	M	K	B	E	F	L	G	D	N	I	H
const u16 u16LcdChar[] = { 

				// Char	Ascii
	0x00	,	//	0	0	null
	0x06	,	//	1	1
	0x9E51	,	//	2	2	//edit by vishnu	
	0x9AD1	,	//	3	3
	0x3D0	,	//	4	4
	0x9BC1	,	//	5	5
	0x9FC1	,	//	6	6
	0x5021	,	//	7	7
	0x9FD1	,	//	8	8
	0x9BD1	,	//	9	9

	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0xD3E6,	0,	0,	0,	0,	0,	0,	0,	
	0x240	,	//  -   45
	0,	
	0x4020	,	//  '/'	47	
	0x9D91	,	//	0	48
	0x06	,	//	1	49
	0x9E51	,	//	2	50 //edit by vishnu
	0x9AD1	,	//	3	51
	0x3D0	,	//	4	52
	0x9BC1	,	//	5	53
	0x9FC1	,	//	6	54
	0x5021	,	//	7	55
	0x9FD1	,	//	8	56
	0x9BD1	,	//	9	57	
	
	0,	0,	0,	0,	0,	0,	0,	
	
	0x17D1	,	//	A	65
	0x98D7	,	//	B	66
	0x9D01	,	//	C	67
	0x9897	,	//	D	68
	0x9F41	,	//	E	69
	0x1741	,	//	F	70
	0x9DC1	,	//	G	71
	0x7D0	,	//	H	72
	0x9807	,	//	I	73
	0x8C90	,	//	J	74
	0x728	,	//	K	75
	0x8D00	,	//	L	76
	0x25B0	,	//	M	77
	0x2598	,	//	N	78
	0x9D91	,	//	O	79
	0x1751	,	//	P	80
	0x9D99	,	//	Q	81
	0x1759	,	//	R	82
	0x9BC1	,	//	S	83
	0x1007	,	//	T	84
	0x8D90	,	//	U	85
	0x4520	,	//	V	86
	0x4598	,	//	W	87
	0x6028	,	//	X	88
	0x2024	,	//	Y	89
	0xD821	,	//	Z	90
	0x8007	,	// 	[	91
	0		,	
	0x1806	,	//	]	93
	0x0000 ,  	//  ClrSeg 94
	0,0,0,0,0,0, //100
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0x0000,
	0x0000,
	0x0000,
	0x0000, //123
	0x0006 // | 124

};

const u16 u16AMTChar[] = { 

	0x19D9, //0
	0x6000, //1
	0x15E9, //2
	0x1DA9, //3 
	0x0D30, //4
	0x1CB9, //5
	0x1CF9, //6 
	0x1901, //7
	0x1DF9, //8
	0x1DB9, //9
	
	0x8952,	//N	10
	0x1571,	//P	11
	0x9571,	//R	12
	0xD8  ,	//L	13
	
	0x420 ,	//-	14
	0x0000  //Clear Segment	15
};

//Digit for ODO segment 
					 	 	  //0	1	 2     3   4    5    6    7    8    9    //Space             
const u8 OdoDigitArray[11] = {0xFA,0x0A,0xBC,0x9E,0x4E,0xD6,0xF6,0x8A,0xFE,0xDF,0x00};

//Digit for TRIP segment  B	G	C	X	A	F	E	D

const u8 TripDigitArray[11] = {0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,0x00};

//CLOCK Digit segment 
								//0		1	 2     3   4    5    6    7    8    9   //Space    
const u8 ClockDigitArray[11] = { 0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};



/// @addtogroup USER_LCD
/// @{
 
/// @defgroup LCDLib Private functions used in LCD file
//// 
///  
 
/// @addtogroup LCDLib
/// @{


// private prototypes --------------------------------------------------------
//Lamps to be display On LCD
static void _vLightMediumHeavyDisp(void);
static void _vAbsMilDisp(void);      //CAN
static void _vEscAbsMilDisp(void);    //Analouge       
static void _vBreakWearDisp(void);      //Analouge
static void _vCheckTransmissionDisp(void); //CAN
static void _vWaterInFuelDisp(void); //CAN
static void _vAirFilterCloggedDisp(void);//Analouge
static void _vDpfDisp(void);   //CAN
static void _vScrDisp(void);    //CAN  
static void _vDefLevelDisp(void); //CAN
static void _vExhaustBreakDisp(void);  //CAN
static void _vRetarderDisp(void);   //Analouge
static void _vEnginePreHeatDisp(void);  //Engine PreHeat and Glow Pluge are same CAN
static void _vDecorateBoundry(void);
static void _vRearApgDisplay(void);
static void _vFrontApgDisplay(void);
static void _vOilGaugeDisplay(void);
static void _vDefDisplay();
static void _vOdoDisplay();
static void _vTripDisplay();
static void _vTripDataAtTransition();
static void _vClockDisplay();
static void _vClockSetting(u16,bool);
static void _vBattVoltDisplay();
static void _vOATDisplay();
static void _vOATDigitPos1(u8);
static void _vOATDigitPos2(u8);
static void _vBattVoltDigitPos1(u8);
static void _vBattVoltDigitPos2(u8);
static void _vClockDigitRamFill(u16,bool,bool);
static void _vCLOCKDigitPos2(u8);
static void _vCLOCKDigitPos3(u8);
static void _vCLOCKDigitPos4(u8);
static void _vTripDigitRamFill(u32);
static void _vOdoDigitRamFill(u32);
static void _vIllControl();
static void _vAFEDisplay(void);
static void _vAFEData(void);
static void _vAfeDigitPos1(u8);
static void _vAfeDigitPos2(u8);
static void _vAfeDigitPos3(u8);
static void _vAmtDisplay();
static void _vAmtDataDigitpos1(u8);
static void _vAmtDataDigitpos2(u8);
static void _vTopLineStringDisplay();
static void _vTopLineCharDisplay(u8,u8);
static void _vTopLinePrint(u8 u8Msg);
static void _vI2CCopyBuffer(eLCDBlockType);
static void _vDisplayServiceReminder();
static void _vSendLcdScreenDataI2c();


/// @} // endgroup LCDLib

/// @} // endgroup USER_LCD

/*********************************************************************//**
 *
 * Copy data buffer to i2cbuffer
 *
 * @param      LCD block type to be display  
 *
 * @return     None
 *************************************************************************/

static void _vI2CCopyBuffer(eLCDBlockType blockType)
{
	u16 i;
	gsCopyI2CBufData.u8SlaveAddress = LCD_DEVICE_ADD;
        gsCopyI2CBufData.bDataReady = TRUE;
	switch( blockType )
	{
		case eTripDisplay :
				gsCopyI2CBufData.u8BlockSize = eTRIP_NUM_OF_BYTE;				
				for(i = 0 ; i < eTRIP_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8TripBuffer[i];
			break;
	
		case eOdoDisplay :
				gsCopyI2CBufData.u8BlockSize = eODO_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eODO_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)					 			
					gsCopyI2CBufData.u8Buffer[i] = u8OdoBuffer[i];
			break;
		case eOATDisplay :
				gsCopyI2CBufData.u8BlockSize = eOAT_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eOAT_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8OATBuffer[i];
			break;	
		case eBattVoltDisplay :
				gsCopyI2CBufData.u8BlockSize = eBATT_VOLT_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eBATT_VOLT_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8BattVoltBuffer[i];
			break;	
		case eClockDisplay :
				gsCopyI2CBufData.u8BlockSize = eCLOCK_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eCLOCK_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8ClockBuffer[i];
			break;	
		case eIllDisplay :
				gsCopyI2CBufData.u8BlockSize = eIIL_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eIIL_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8IllBuffer[i];
			break;
		case eServDisplay :
				gsCopyI2CBufData.u8BlockSize = eSERV_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eSERV_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8ServBuffer[i];
			break;
		case eLMHDisplay :
				gsCopyI2CBufData.u8BlockSize = eLMH_NUM_OF_BYTE;				 				
				for(i = 0 ; i < eLMH_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8LMHBuffer[i];
			break;	

		case    eEngPreHeatDisplay : //Comman Buffer for Engine Pre heat and retarder
		case 	eRetarderDisplay:
					gsCopyI2CBufData.u8BlockSize = eRG_NUM_OF_BYTE;								
					for(i = 0 ; i < eRG_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8RGBuffer[i];
				break;	

		 case	 eExhaustBreakDisplay:  //Comman Buffer for these indicators
		 case 	 eLowDefDisplay:
		 case    eScrDisplay:
		 case 	 eDpfDisplay:
					 gsCopyI2CBufData.u8BlockSize = eDSDE_NUM_OF_BYTE;							 
					 for(i = 0 ; i < eDSDE_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						 gsCopyI2CBufData.u8Buffer[i] = u8DSDEBuffer[i];
				 break;  

		case	eAirFilterCloggedDisplay:
				gsCopyI2CBufData.u8BlockSize = eAFC_NUM_OF_BYTE;							
				for(i = 0 ; i < eAFC_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8AFCBuffer[i];
			break; 
				
		case	eBreakWearDisplay:
		case	eCheckTransDisplay:
		case	eWaterInFuelDisplay:
					gsCopyI2CBufData.u8BlockSize = eBCW_NUM_OF_BYTE;							
					for(i = 0 ; i < eBCW_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8BCWBuffer[i];
				break;
		case	eABSMilDisplay:
		case	eEscMilDisplay:
					gsCopyI2CBufData.u8BlockSize = eABS_ESC_NUM_OF_BYTE;							
					for(i = 0 ; i < eABS_ESC_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8AbsEscBuffer[i];
				break;
		case	eDecorateBoundry1Display:
					gsCopyI2CBufData.u8BlockSize = eLINE_NUM_OF_BYTE1;							
					for(i = 0 ; i < eLINE_NUM_OF_BYTE1 && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8DecorateBound1[i];
				break;
		case	eDecorateBoundry2Display:
					gsCopyI2CBufData.u8BlockSize = eLINE_NUM_OF_BYTE2;							
					for(i = 0 ; i < eLINE_NUM_OF_BYTE2 && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8DecorateBound2[i];
				break;
		case	eDecorateBoundry3Display:
					gsCopyI2CBufData.u8BlockSize = eLINE_NUM_OF_BYTE3;							
					for(i = 0 ; i < eLINE_NUM_OF_BYTE3 && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8DecorateBound3[i];
				break;

		case	eAPRearBarDisplay:
					gsCopyI2CBufData.u8BlockSize = eRAPG_NUM_OF_BYTE;							
					for(i = 0 ; i < eRAPG_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8RApgBuffer[i];
				break;

		case	eAPFrontBarDisplay:
					gsCopyI2CBufData.u8BlockSize = eAPG_NUM_OF_BYTE;							
					for(i = 0 ; i < eAPG_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8FApgBuffer[i];
				break;
		 
		case	eOilBarDisplay:
					gsCopyI2CBufData.u8BlockSize = eOIL_NUM_OF_BYTE;							
					for(i = 0 ; i < eOIL_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
						gsCopyI2CBufData.u8Buffer[i] = u8OilBuffer[i];
				break;	
	
		case	eDefBarDisplay:
				gsCopyI2CBufData.u8BlockSize = eDEF_NUM_OF_BYTE;							
				for(i = 0 ; i < eDEF_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8DefBuffer[i];
			break;	
				
		case	eDegreeSymboleDisplay:
				gsCopyI2CBufData.u8BlockSize = eDCS_NUM_OF_BYTE;							
				for(i = 0 ; i < eDCS_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8DCSBuffer[i];
			break;

		case	eAfeDisplay:
				gsCopyI2CBufData.u8BlockSize = eAFE_NUM_OF_BYTE;							
				for(i = 0 ; i < eAFE_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8AfeBuffer[i];
			break;

		case	eAmtAtDisplay:
				gsCopyI2CBufData.u8BlockSize = eAMT_AT_CHAR_NUM_OF_BYTE;							
				for(i = 0 ; i < eAMT_AT_CHAR_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8AmtAtCharBuffer[i];
			break;			

		case	eAutoDisplay:
				gsCopyI2CBufData.u8BlockSize = eAUTO_CHAR_NUM_OF_BYTE;							
				for(i = 0 ; i < eAUTO_CHAR_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8AutoCharBuffer[i];
			break;			
 		case	eAmtDataDisplay:
				gsCopyI2CBufData.u8BlockSize = eAMT_DATA_NUM_OF_BYTE;							
				for(i = 0 ; i < eAMT_DATA_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8AmtBuffer[i];
			break;
 		case	eTpmsCharDisplay:
				gsCopyI2CBufData.u8BlockSize = eTPMS_CHAR_NUM_OF_BYTE;							
				for(i = 0 ; i < eTPMS_CHAR_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8TpmsCharBuffer[i];
			break;	 			
	
 		case	eTpmsStringDisplay:
				gsCopyI2CBufData.u8BlockSize = eTPMS_STRING_NUM_OF_BYTE;							
				for(i = 0 ; i < eTPMS_STRING_NUM_OF_BYTE && i < I2C_MAX_BLOCK_SIZE ; i++)
					gsCopyI2CBufData.u8Buffer[i] = u8TpmsStringBuffer[i];
			break;	
	


	}
	
	vPushI2CData();
}


 /*********************************************************************//**
 *
 * Update LCD screen one by one block wise 
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
void vRefreshLcd()
{ 
	clusterStatuslcd = eGetClusterStatus();  //Used for display in self testing 
	
	vSetEMSFaultText();
	_vDecorateBoundry();
	//_vDefDisplay();
	_vOilGaugeDisplay();
	_vFrontApgDisplay();
	_vRearApgDisplay();	
	_vEscAbsMilDisp();
	_vBreakWearDisp();
	//_vCheckTransmissionDisp(); 
	_vWaterInFuelDisp();
	//_vAirFilterCloggedDisp();
	//_vExhaustBreakDisp();
	_vDefLevelDisp();
	//_vScrDisp();
	//_vDpfDisp();
	_vRetarderDisp();
	_vEnginePreHeatDisp();
	//_vLightMediumHeavyDisp(); 
	_vOdoDisplay();
	_vTripDisplay();
	_vClockDisplay(); 
	_vBattVoltDisplay(); 
	//_vOATDisplay();
	_vIllControl();
	//_vAFEDisplay();	
	//_vAmtDisplay();
	_vTopLineStringDisplay(); 	
	//_vDisplayServiceReminder();
	

}

 
  /*********************************************************************//**
 *
 * This Function is used to Create and Send IIC data Buffer to print Messages
 *
 * @param      None    
 *
 * @return     None
 *************************************************************************/
static void _vTopLineStringDisplay()
{
	u8 i = 0U;
	u8 TempVar = 0U;
	static u8 u8PrevMsgState = 0U;
	static bool bPrevMsgState = FALSE;
	static bool bDataChanged1 = FALSE;
	static bool bDataChanged2 = FALSE;
	//	u8 u8VehicleType = 0;
	static u32 u32TimeStamp = 0U;
	u8 I2cBusQue = 0U;
	
	//For TPMScharactor display
	u8TpmsCharBuffer[eTPMS_CHAR_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8TpmsCharBuffer[eTPMS_CHAR_COLUMN_ADD] = COLUMN_ADD;
	u8TpmsCharBuffer[eTPMS_CHAR_PARAMETR_ADD] = PARAMETR_ADD;
	u8TpmsCharBuffer[eTPMS_CHAR_ADD] = TPMS_CHAR_ADD;
	u8TpmsCharBuffer[eTPMS_CHAR_DISP_COMMAND] = DISP_COMMAND;

	//u8LcdRAM[eTPMS_RAM_ADD_BYTE] |= TPMS_CHAR_ON;
	
	u8LcdRAM[eTPMS_RAM_ADD_BYTE] &= ~TPMS_CHAR_ON;

	u8TpmsCharBuffer[eTPMS_CHAR_BYTE1] = u8LcdRAM[eTPMS_RAM_ADD_BYTE];
	
	//TPMS segment dependent on LCD configuration EOL
	if(clusterStatuslcd == eClusterIgnitionReset)
	{
		/*if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayTPMS == eEnable)
		{			
			u8LcdRAM[eTPMS_RAM_ADD_BYTE] |= TPMS_CHAR_ON;
		}
		*/
		u8TpmsCharBuffer[eTPMS_CHAR_BYTE1] = u8LcdRAM[eTPMS_RAM_ADD_BYTE];
	}
		

	for(i = 0; i <eTPMS_CHAR_NUM_OF_BYTE; i++)
	{
		if (u8PrevTPMSBuffer[i] != u8TpmsCharBuffer[i])
		{
			//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
			{
				; // empty statement for this for loop
			}
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevTPMSBuffer[i] =	u8TpmsCharBuffer[i];
					bDataChanged1 = TRUE;
				}
			}
		}
	}
	if( bDataChanged1 == TRUE )
	{
	
		_vI2CCopyBuffer(eTpmsCharDisplay);
		bDataChanged1 = FALSE;
	}

	//For TPMS string display
	u8TpmsStringBuffer[eTPMS_STRING_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8TpmsStringBuffer[eTPMS_STRING_COLUMN_ADD] = COLUMN_ADD;
	u8TpmsStringBuffer[eTPMS_STRING_PARAMETR_ADD] = PARAMETR_ADD;
	u8TpmsStringBuffer[eTPMS_STRING_ADD] = TPMS_STRING_ADD;
	u8TpmsStringBuffer[eTPMS_STRING_DISP_COMMAND] = DISP_COMMAND;

	
	if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
	{
	
		_vTopLinePrint(SELF_TEST_PROGRESS);
	}
	else if(clusterStatuslcd == eClusterEnterSleep)
		{
		_vTopLinePrint(CLUSTER_ENTER_SLEEP);
		}
	else
	{
		//Remove EOL pending as per mail dated 02/07/20
		/*
	    TempVar = u8GetEolPendingStatus();
		if(TempVar == EOL_PENDING)		// if EOL not configured.
		{
		 	_vTopLinePrint(EOL_PENDING);		 
		}
		else
		{		
			_vTopLinePrint(NORMAL_MODE);
		}
		*/
		_vTopLinePrint(NORMAL_MODE);
		
	}
	
	for(i=eTPMS_STRING_BYTE1 ; i< eTPMS_STRING_NUM_OF_BYTE; i++)
		u8TpmsStringBuffer[i] = u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE1 + (i - eTPMS_STRING_BYTE1)];

	
 
	for(i = 0; i <eTPMS_STRING_NUM_OF_BYTE; i++)
	{
		if (u8PrevTpmsStringBuffer[i] != u8TpmsStringBuffer[i])
		{
		//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevTpmsStringBuffer[i] =	u8TpmsStringBuffer[i];
					bDataChanged2 = TRUE;
				}
			}
		}
	}
	if( bDataChanged2 == TRUE )
	{
		_vI2CCopyBuffer(eTpmsStringDisplay);
		bDataChanged2 = FALSE;
	}


}

/***************************************************************************//**
*
* This Function Process Message String and Prepares LcdRam bytes for displaying
*	characters.
*
* @param	  u8CharacterENUM		Character from 0-9 and A-Z	  
* @param	  u8Loop				Character placing.	
*
* @return	  None
*******************************************************************************/
static void _vTopLineCharDisplay(u8 u8CharacterENUM, u8 u8Loop)
{
	
	switch(u8Loop)	// Switch Character Places of Top Line, 0-17 places
	{
		case 0 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE35] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE36] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 1 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE33] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE34] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 2 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE31] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE32] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 3 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE29] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE30] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 4 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE27] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE28] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 5 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE25] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE26] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 6 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE23] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE24] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 7 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE21] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE22] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 8 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE19] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE20] = u16LcdChar[u8CharacterENUM] >> 8;
		break;
		case 9 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE17] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE18] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 10 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE15] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE16] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 11 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE13] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE14] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 12 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE11] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE12] = u16LcdChar[u8CharacterENUM]>>8;
		break;			
		case 13 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE9] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE10] = u16LcdChar[u8CharacterENUM]>>8;
		break;
		case 14 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE7] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE8] = u16LcdChar[u8CharacterENUM]>>8;
		break;			
		case 15 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE5] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE6] = u16LcdChar[u8CharacterENUM]>>8;
		break;			
		case 16 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE3] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE4] = u16LcdChar[u8CharacterENUM]>>8;
		break;			
		case 17 : 	u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE1] = u16LcdChar[u8CharacterENUM] & 0x00FF;
					u8LcdRAM[eTPMS_STRING_RAM_ADD_BYTE2] = u16LcdChar[u8CharacterENUM]>>8;
		break;			
		default :
			
		break;
			
	}
}

/***************************************************************************//**
*
* This Function Process Message String and Prepares LcdRam bytes for displaying
*	characters.
*
* @param	  u8MsgNo		String Index	  
*
* @return	  None
*******************************************************************************/
static void _vTopLinePrint(u8 Selftest)
{
	static u8 u8Length = 0;			// Identifies TopLine Text Character Location
	static u16 u16SpeedoPulses = 0;
	static u16 u16SpeedoPulsesDigit = 0;
	u16 u16SetSpeedoPulses = 0;
	u8 i=0, loop = 0;
	u32 TempVar = 0;
	u16 TempVar1 = 0;
	u16 TempVar2 = 0;
	u32 DTC_SPN = 0;
	u32 DueService = 0;
	u8 u8DtcNumber = 0;
	u8 ArrDisplay[6]={"      "};  //fill blank in array so that leading zero can clear in display 
	u8 IKCO[18]={"       IKCO       "};
	u8 GoodBye[18]={"     GOODBYE     "};
	static u32 Timestamp = 0;
	static u32 PrevDTCnumber = NO_DTC_FAULTS;
	static u16 Avoid_Nofault_Time=0;
	static u16 PulseSetBlinkTime=0;
	static bool TogalFlag = FALSE;
	

	// Check the Current Priority for the top line display.
	// Priority 1 Displays are Speedo Pulse/ Serv Due/ DTC check Mode
	TempVar2 = GetLcdScreenStatus();
	if(eTopLinePriority == ePriority1)
	{
		// When User Selects LCD CheckMode, Priority 1 is set and for displaying SPN and FMI 
		// following Algorithm has been used:
		// If there is fault present: Means DTCReady will be true.
		// Check Both Multi Frames and Single Frame DM1 messages.
		// Now Assign SP  Values At Indexes 4,5,6,7,8,9 and FMI values at 15,16,17 of topLine Text.
		// top Line Positions   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
		// Example				S P N   X X X X X X     F  M  I     X  X  X
		
		if(GetLcdScreenStatus() == eDTC)	// if LCD is  DTC check mode. Priority 1
		{
		
			

			u8DtcNumber = u8GetDTCMsgNumber();			// Get DTC message index
			if(eGetDtcType() == eMultiFrames) 			// Check if available DTC is Multiframe or single frame
			{

				if(PrevDTCnumber != u8DtcNumber)
				{
					TempVar = DTC_J1939[u8DtcNumber].SPNValue;
					TempVar1 = DTC_J1939[u8DtcNumber].FMICode;
					PrevDTCnumber = u8DtcNumber;
				}
				Timestamp = u32GetClockTicks();
				u8TopLineText = 2;
			}
			else if((eGetDtcType() == eSingleFrame))
			{
				TempVar = u32GetPGNdata(eJDM1DTC_spn1) | (u32GetPGNdata(eJDM1DTC_spn2)<< 8) | ( (u32GetPGNdata(eJDM1DTC_fmi) & 0xE0U)<<11 );
				TempVar1 = (u8)(u32GetPGNdata(eJDM1DTC_fmi) & 0x1FU);
				u8TopLineText = 2; // set SPN FMI text
			}
			else
			{
				if(u16GetClockTicks()- Avoid_Nofault_Time < 1000 )
				{
					return;
				}	
				u8TopLineText = 3; // set no fault text
				TempVar = 0;
				TempVar1 = 0;
			}
			
			DTC_SPN = TempVar;
			//if before 2 digit is non zero then fill array up to non zero digit 
			while(TempVar > 0U)
			{
				ArrDisplay[loop]= (u8) ( (TempVar%10U) + 0x30U );
				TempVar=TempVar/10U;
				loop++;
			}
			
			if( (DTC_SPN != 0U ) && ( TempVar1 != 0U ) )
			{				
				u8VariableString[2][4] = ArrDisplay[5];
				u8VariableString[2][5] = ArrDisplay[4];
				u8VariableString[2][6] = ArrDisplay[3];
				u8VariableString[2][7] = ArrDisplay[2];
				u8VariableString[2][8] = ArrDisplay[1];
				u8VariableString[2][9] = ArrDisplay[0];

				
				
				//********Edit by Vishnu FMI data fill in buffer******
				
				for( i = 0U; i < 3U; i++)
				{
					u8VariableString[2][15U+i] = ' ';
				}
				i = 17U; 	// position of FMI data 17 to 15
				while( (TempVar1 > 0 ) && ( i > 14U ) )
				{
					u8VariableString[2][i] = (u8)( (TempVar1%10U)+0x30U );
					TempVar1 = TempVar1/10U;
					i--;
				}				
				//*************************************	
				
			}				
		}		
		else if(GetLcdScreenStatus() == eFeededPulse)
		{
			if(u8GetFeederPulseStatus() == eFeededPulse)
			{
				u16SpeedoPulses = u16GetFeededPulse();		// Get Speedo Pulses 
				// Set Speedo Pulses at position 12,13,14,15,16 in Top Line Text
				// top Line Positions   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
				// Example				P U L S E S / K M       X  X  X  X  X     
				for( i = 0U; i < 5U; i++)
				{
				   u8VariableString[0][i+11U] =  ' ';
				}
						
				i = 15U; //last position of service data 15
				while( (u16SpeedoPulses > 0U) && (i > 10U) )
				{
					u8VariableString[0][i] = (u8)( (u16SpeedoPulses%10U)+0x30U );
					u16SpeedoPulses = u16SpeedoPulses/10U;
					i--;
				}
				//u8TopLineText = 0U;
			}
			else if(u8GetFeederPulseStatus() == eSoftwareVersion)
			{
                TempVar = u16getSoftwareVersion();

                // top Line Positions	0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
                // Example				S W   V E R S I O N 	V  0  4 	0  0
                // last position of service data 15
 
                    u8VariableString[1][13] = (TempVar >> 8) + '0';
					u8VariableString[1][15] = (TempVar >> 4) + '0';
					u8VariableString[1][15] = (u8VariableString[5][15] & 0x0F) + '0';
					u8VariableString[1][16] = (TempVar & 0x0F) + '0';
                  
                
            }
			
			else
				{
					;
				}
			
			vSetDTCMsgNumber(NULL);
		}
		else if(u8GetFeederPulseStatus() == eSetPulse)
			{	 
				u16SpeedoPulsesDigit = u8GetFeederPulseDigitStatus();
				
				// Set Speedo Pulses at position 12,13,14,15,16 in Top Line Text
				// top Line Positions   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
				// Example				P U L S E S / K M       X  X  X  X  X    
				
				
                		
            				u16SpeedoPulses = u16GetPulseSet();
							u8VariableString[3][15] = (u8)( (u16SpeedoPulses%10U)+0x30U );
							
							u16SpeedoPulses = u16SpeedoPulses/10U;
          					u8VariableString[3][14] = (u8)( (u16SpeedoPulses%10U)+0x30U );
							
          					u16SpeedoPulses = u16SpeedoPulses/10U;								
          					u8VariableString[3][13] = (u8)( (u16SpeedoPulses%10U)+0x30U );
							
							u16SpeedoPulses = u16SpeedoPulses/10U;					
              				u8VariableString[3][12] = (u8)( (u16SpeedoPulses%10U)+0x30U );
							
              				u16SpeedoPulses = u16SpeedoPulses/10U;								
              				u8VariableString[3][11] = (u8)( (u16SpeedoPulses%10U)+0x30U );
							if(b1HzToggleFlash())
							{
	              				if(u16SpeedoPulsesDigit == eSetPulse)
	              					{
	              					u8VariableString[3][15] = ' ';              					
	              					}
								if(u16SpeedoPulsesDigit == eSetFourthDigit)
	              					{
	              					u8VariableString[3][14] = ' ';
	              					
	              					}
								if(u16SpeedoPulsesDigit == eSetThirdDigit)
	              					{								
	              					u8VariableString[3][13] = ' ';
	              					
	              					}
								if(u16SpeedoPulsesDigit == eSetSecondDigit)
	              					{				
	              					u8VariableString[3][12] = ' ';
	              					
	              					}
								if(u16SpeedoPulsesDigit == eSetFirstDigit)
	              					{							
	              					u8VariableString[3][11] = ' ';              					
	              					}
							}
              				
                		}
     
	
	}
	else
	{
		Avoid_Nofault_Time = u16GetClockTicks();
	}

	// Filling RAM bytes for each character Position ie, 0-17 as per the text formed.
	
	while(u8Length < 18U)
	{
		if(Selftest == NORMAL_MODE)	// if EOL configured.
		{		
			if(eTopLinePriority == ePriority1)	// If priority is 1
			{
				_vTopLineCharDisplay(u8VariableString[u8TopLineText][u8Length], u8Length);
			}
			else
				{
				_vTopLineCharDisplay(IKCO[u8Length], u8Length);
				
				}
			
		}
		else if(Selftest == SELF_TEST_PROGRESS)
		{
			_vTopLineCharDisplay(u8subpriority1[eSelftestProgress][u8Length], u8Length);
		}
		else if(Selftest == EOL_PENDING)
		{
			_vTopLineCharDisplay(u8subpriority1[eIcEolPending][u8Length], u8Length);
		}
		else if(Selftest == CLUSTER_ENTER_SLEEP)
		{
			_vTopLineCharDisplay(GoodBye[u8Length], u8Length);
		}
		else
		{
			;
		}
		u8Length++;
			
	}
		
	if(u8Length >= 18U)
	{
		u8Length = 0U;
	}
}


/***************************************************************************//**
*
* Set TopLine Priority using this function. This Function is available globally.
*
* @param	  Top Line Priority	  
*
* @return	  None
*******************************************************************************/
void vSetTopLinePriority(eTOP_LINE_PRIORITY eTLPriority)
{
	if(eTopLinePriority == ePriority2 || eTopLinePriority == eBlank)
		eTopLinePriority = eTLPriority;
	if(eTLPriority == eBlank)
		eTopLinePriority = eTLPriority;
}

/***************************************************************************//**
*
* Get TopLine Priority using this function. This Function is available globally.
*
* @param	  None	  
*
* @return	  Top Line Priority
*******************************************************************************/
eTOP_LINE_PRIORITY eGetTopLinePriority(void)
{
	return eTopLinePriority;
}

/*********************************************************************************//**
*
* Set TopLine Sub-Priority using this function. This Function is available globally.
*
* @param	  Top Line Sub-Priority
*
* @return	  None	  
*************************************************************************************/
void vSetTopLineSubPriority(eTOP_LINE_SUB_PRIORITY eTLSubPriority)
{
	eTopLineSubPriority = eTLSubPriority;
}

/*********************************************************************************//**
*
* get TopLine Sub-Priority using this function. This Function is available globally.
*
* @param	  None	  
*
* @return	  Top Line Sub-Priority
*************************************************************************************/
eTOP_LINE_SUB_PRIORITY eGetTopLineSubPriority(void)
{
	return eTopLineSubPriority;
}


/*********************************************************************************//**
*
* get TopLine Text Index using this function. This Function is available globally.
*
* @param	  None	  
*
* @return	  Text Index
*************************************************************************************/
void vSetTopLineText(u8 TextNum)
{
	u8TopLineText = TextNum;
}

/*********************************************************************//**
*
* This Function is used to Create and Send IIC data Buffer for AMT Process
*
* @param	  None  
*
* @return	  None
*************************************************************************/
static void _vAmtDisplay()
{
	u8 i = 0;
	
	u16 u16Gear = 0x5858;
	u8 u8AutoManual = 0;
	u8 u8EcoPower = 0;
	u8 u8AtEco = 0;
	u8 u8AtPower = 0;
	static bool bDataChanged1 = FALSE;
	static bool bDataChanged2 = FALSE;
	static bool bDataChanged3 = FALSE;
	u8 I2cBusQue = 0;

	u8 u8TransType = 0;

	u8TransType = u8GetTransmissionType();

	u16Gear = u16GetGearAMT();							// Get Gear Data
	u8AutoManual = eGetAutoManualStatus();				// Gear Auto Manual Data
	
	

	u8AmtAtCharBuffer[eAMT_AT_CHAR_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8AmtAtCharBuffer[eAMT_AT_CHAR_COLUMN_ADD] = COLUMN_ADD;
	u8AmtAtCharBuffer[eAMT_AT_CHAR_PARAMETR_ADD] = PARAMETR_ADD;
	u8AmtAtCharBuffer[eAMT_AT_CHAR_ADD] = AMT_AT_ADD;
	u8AmtAtCharBuffer[eAMT_AT_CHAR_DISP_COMMAND] = DISP_COMMAND;

	if(clusterStatuslcd == eClusterIgnitionReset && bGetTransmissionEnable()) //On ALL sengment during self test
	{		
		if(u8TransType == AMT_TRANS)
		{
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] |= AMT_CHAR_ON;
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AT_CHAR_OFF;
		}
		else if(u8TransType == AT_TRANS)
		{
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AMT_CHAR_OFF;
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] |= AT_CHAR_ON;
			
		}
		else // if Configure manual and others
		{
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AMT_CHAR_OFF;
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AT_CHAR_OFF;	
		}
		
	}
	else
	{
		if(u8TransType == DISABLE_TRANS || u8TransType == MANUAL_TRANS || !bGetTransmissionEnable())
		{
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AMT_CHAR_OFF;
			u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AT_CHAR_OFF;
		}
		else
		{
			if(u8TransType == AMT_TRANS)
			{
				u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] |= AMT_CHAR_ON;
				u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AT_CHAR_OFF;
			}
			else if(u8TransType == AT_TRANS)
			{
				u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] |= AT_CHAR_ON;
				u8LcdRAM[eAMT_AT_RAM_ADD_BYTE] &= AMT_CHAR_OFF;
			}
		}
	}
		 
		u8AmtAtCharBuffer[eAMT_AT_CHAR_BYTE1] = u8LcdRAM[eAMT_AT_RAM_ADD_BYTE];


		for(i = 0; i <eAMT_AT_CHAR_NUM_OF_BYTE; i++)
		{
			if (u8PrevAmtAtCharBuffer[i] != u8AmtAtCharBuffer[i])
			{
			//Check I2C buffer empty or not 
				for (I2cBusQue = 0;
					(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
					++I2cBusQue)
					; // empty statement for this for loop
				if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
				{
					if(sI2CControl[I2cBusQue].bDataReady == FALSE)
					{
						u8PrevAmtAtCharBuffer[i] =  u8AmtAtCharBuffer[i];
						bDataChanged1 = TRUE;
					}
				}
			}
		}
		   	//_vI2CCopyBuffer(eAmtAtDisplay);
	
	if( bDataChanged1 == TRUE )
	{	
		_vI2CCopyBuffer(eAmtAtDisplay);
		bDataChanged1 = FALSE;
	}
	
	 //For AUTO and Manual character display
	 u8AutoCharBuffer[eAUTO_CHAR_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	 u8AutoCharBuffer[eAUTO_CHAR_COLUMN_ADD] = COLUMN_ADD;
	 u8AutoCharBuffer[eAUTO_CHAR_PARAMETR_ADD] = PARAMETR_ADD;
	 u8AutoCharBuffer[eAUTO_CHAR_ADD] = AUTO_ADD;
	 u8AutoCharBuffer[eAUTO_CHAR_DISP_COMMAND] = DISP_COMMAND;

 	
	if(clusterStatuslcd == eClusterIgnitionReset && bGetTransmissionEnable()) //On ALL sengment during self test
	{

		if(u8TransType == AMT_TRANS)
		{
			u8LcdRAM[eAUTO_RAM_ADD_BYTE] |= AUTO_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] |= MANUAL_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= POWER_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= ECO_CHAR_ON ;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= CREEP_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] |= CRAWL_CHAR_ON;

		}
		else if(u8TransType == AT_TRANS)
		{
			u8LcdRAM[eAUTO_RAM_ADD_BYTE] &= ~AUTO_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~MANUAL_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= POWER_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= ECO_CHAR_ON ;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~CREEP_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~CRAWL_CHAR_ON;

		}
		else // if Configure manual and others
		{
			u8LcdRAM[eAUTO_RAM_ADD_BYTE] &= ~AUTO_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~MANUAL_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~POWER_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~ECO_CHAR_ON ;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~CREEP_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~CRAWL_CHAR_ON;	
		}
	
	}
	else
	{
		if(u8TransType == DISABLE_TRANS || u8TransType == MANUAL_TRANS || !bGetTransmissionEnable())
		{
			u8LcdRAM[eAUTO_RAM_ADD_BYTE] &= ~AUTO_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~MANUAL_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~POWER_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~ECO_CHAR_ON ;
			u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~CREEP_CHAR_ON;
			u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~CRAWL_CHAR_ON;		
				
		}
		else
		{	
			if(u8AutoManual == eManual)
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE1] |= MANUAL_CHAR_ON;		// Manual On
			}	
			else
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~MANUAL_CHAR_ON;	// Manual Off
			}
			
			if(u8AutoManual == eAuto)
			{
				u8LcdRAM[eAUTO_RAM_ADD_BYTE] |= AUTO_CHAR_ON;		// Auto On
			}
			else
			{
				u8LcdRAM[eAUTO_RAM_ADD_BYTE] &= ~AUTO_CHAR_ON;		// Auto Off
			}

			if(u8TransType == AMT_TRANS)
			{					
				u8EcoPower   = eGetEcoPowerStatus();				// Get Eco Power Status
				if(u8EcoPower == ePower)
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= POWER_CHAR_ON;		// Power On
				}	
				else
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~POWER_CHAR_ON;		// Power Off
				}
				
				if(u8EcoPower == eEco)
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= ECO_CHAR_ON;		// Eco On
				}
				else
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~ECO_CHAR_ON;		// Eco Off
				}
			}
			else if(u8TransType == AT_TRANS)
			{
				u8AtEco = eGetAtEcoStatus();
				u8AtPower = eGetAtPowerStatus();
				if(u8AtPower == POWER_ON)
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= POWER_CHAR_ON;		// Power On
				}	
				else
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~POWER_CHAR_ON;		// Power Off
				}
				
				if(u8AtEco == ECO_ON)
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= ECO_CHAR_ON;		// Eco On
				}
				else
				{
					u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~ECO_CHAR_ON;		// Eco Off
				}
			}


			if(bGetCrawlStatus())
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE1] |= CRAWL_CHAR_ON;		// Crawl On
				
			}	
			else
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE1] &= ~CRAWL_CHAR_ON;		// Crawl Off
			}
			
			if(bGetCreepStatus())
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE6] |= CREEP_CHAR_ON;		// Creep On
			}
			else
			{
				u8LcdRAM[eAMT_RAM_ADD_BYTE6] &= ~CREEP_CHAR_ON;		// Creep Off
			}
		}
	}

	u8AutoCharBuffer[eAUTO_CHAR_BYTE1] = u8LcdRAM[eAUTO_RAM_ADD_BYTE];
	
	for(i = 0; i <eAUTO_CHAR_NUM_OF_BYTE; i++)
	{
		if (u8PrevAutoCharBuffer[i] != u8AutoCharBuffer[i])
		{
		for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevAutoCharBuffer[i] =  u8AutoCharBuffer[i];
					bDataChanged2 = TRUE;
				}
			}
		}
	}
	if( bDataChanged2 == TRUE )
	{
	
		_vI2CCopyBuffer(eAutoDisplay);
		bDataChanged2 = FALSE;
	}	
	
	u8AmtBuffer[eAMT_DATA_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8AmtBuffer[eAMT_DATA_COLUMN_ADD] = COLUMN_ADD;
	u8AmtBuffer[eAMT_DATA_PARAMETR_ADD] = PARAMETR_ADD;
	u8AmtBuffer[eAMT_DATA_ADD] = AMT_SEG_ADD;
	u8AmtBuffer[eAMT_DATA_DISP_COMMAND] = DISP_COMMAND;

	if(clusterStatuslcd == eClusterIgnitionReset && bGetTransmissionEnable()) //On ALL sengment during self test
	{
		if(u8TransType != DISABLE_TRANS )
		{
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = 0xff;
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = 0xff;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = 0xff;
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = 0xff;
		}
		else
		{
			//Clear both digits
			_vAmtDataDigitpos1('X');						
		    _vAmtDataDigitpos2('X');
		}
	}
	else if(u8TransType == DISABLE_TRANS || !bGetTransmissionEnable())
	{
		_vAmtDataDigitpos1('X');						// Transmission Mode
		_vAmtDataDigitpos2('X');
	}
	else
	{		
		if(u16Gear >=1 && u16Gear <= 20)
		{
			if(u16Gear > 9)
			{
				_vAmtDataDigitpos1(u16Gear%10);				
				_vAmtDataDigitpos2(u16Gear/10);				// Gear Value
			}
			else
			{
				_vAmtDataDigitpos1(u16Gear);				
				_vAmtDataDigitpos2(u16Gear/10);				// Gear Value
			}
		}		
		else
		{
			_vAmtDataDigitpos2((u8)u16Gear);
			_vAmtDataDigitpos1(u16Gear>>8);						
		}
		
	}
	
	u8AmtBuffer[eAMT_DATA_BYTE1] = u8LcdRAM[eAMT_RAM_ADD_BYTE1];
	u8AmtBuffer[eAMT_DATA_BYTE2] = u8LcdRAM[eAMT_RAM_ADD_BYTE2];
	u8AmtBuffer[eAMT_DATA_BYTE3] = u8LcdRAM[eAMT_RAM_ADD_BYTE3];
	u8AmtBuffer[eAMT_DATA_BYTE4] = u8LcdRAM[eAMT_RAM_ADD_BYTE4];
	u8AmtBuffer[eAMT_DATA_BYTE5] = u8LcdRAM[eAMT_RAM_ADD_BYTE5];
	u8AmtBuffer[eAMT_DATA_BYTE6] = u8LcdRAM[eAMT_RAM_ADD_BYTE6];
	
	for(i = 0; i <eAMT_DATA_NUM_OF_BYTE; i++)
	{
		if (u8PrevAmtBuffer[i] != u8AmtBuffer[i])
		{
			//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevAmtBuffer[i] =  u8AmtBuffer[i];
					bDataChanged3 = TRUE;
				}
			}
		}
	}
   //_vI2CCopyBuffer(eAmtDataDisplay);
	if( bDataChanged3 == TRUE )
	{
	
		_vI2CCopyBuffer(eAmtDataDisplay);
		bDataChanged3 = FALSE;
	}

}

 /*********************************************************************//**
 *
 * display first digit on AMT screen area
 *
 * @param      Digit	digit to be display 
 *
 * @return     None
 *************************************************************************/
static void _vAmtDataDigitpos1(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[0]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[0] & 0x00FF;
		break;
		case 1:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[1]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[1] & 0x00FF;
		break;
		case 2:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[2]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[2] & 0x00FF;
		break;
		case 3:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[3]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[3] & 0x00FF;
		break;
		case 4:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[4]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[4] & 0x00FF;
		break;
		case 5:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[5]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[5] & 0x00FF;
		break;
		case 6:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[6]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[6] & 0x00FF;
		break;
		case 7:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[7]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[7] & 0x00FF;
		break;
		case 8:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[8]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[8] & 0x00FF;
		break;
		case 9:
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[9]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[9] & 0x00FF;
		break;	
		
		case 0x5F:									//Dash Char
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[14]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[14] & 0x00FF;
		break;		
		
		case 'X':
		                   //Clear Segment
			u8LcdRAM[eAMT_RAM_ADD_BYTE5] = u16AMTChar[15]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE4] = u16AMTChar[15] & 0x00FF;

		break;
		
	}

}

/*********************************************************************//**
*
* display second digit on AMT screen area
*
* @param	  Digit		digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vAmtDataDigitpos2(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[0]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[0] & 0x00FF;
		break;
		case 1:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[1]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[1] & 0x00FF;
		break;
		case 2:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[2]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[2] & 0x00FF;
		break;
		case 3:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[3]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[3] & 0x00FF;
		break;
		case 4:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[4]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[4] & 0x00FF;
		break;
		case 5:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[5]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[5] & 0x00FF;
		break;
		case 6:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[6]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[6] & 0x00FF;
		break;
		case 7:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[7]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[7] & 0x00FF;
		break;
		case 8:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[8]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[8] & 0x00FF;
		break;
		case 9:
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[9]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[9] & 0x00FF;
		break;

		case 'N':
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[10]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[10] & 0x00FF;
		break;
		
		case 'P':
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[11]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[11] & 0x00FF;
		break;

		case 'R':
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[12]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[12] & 0x00FF;
		break;
		
		case 'L':
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[13]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[13] & 0x00FF;
		break;
		
		case 0x5F:										//Dash Char
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[14]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[14] & 0x00FF;
		break;		
		
		case 'X':
		                                                   //Clear Segment
			u8LcdRAM[eAMT_RAM_ADD_BYTE3] = u16AMTChar[15]>>8;
			u8LcdRAM[eAMT_RAM_ADD_BYTE2] = u16AMTChar[15] & 0x00FF;

		break;


	}

}


/*********************************************************************//**
*
* display AFE screen area
*
* @param	  None	  
*
* @return	  None
*************************************************************************/
static void _vAFEDisplay()
{
	u8 i = 0;
	u8 I2cBusQue = 0;
	static bool bDataChanged = FALSE;
	
	u8AfeBuffer[eAFE_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8AfeBuffer[eAFE_COLUMN_ADD] = COLUMN_ADD;
	u8AfeBuffer[eAFE_PARAMETR_ADD] = PARAMETR_ADD;
	u8AfeBuffer[eAFE_ADD] = AFE_ADD;
	u8AfeBuffer[eAFE_DISP_COMMAND] = DISP_COMMAND;

	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayAfe == eDisable)
	{
		u8AfeBuffer[eAFE_BYTE1] = 0;
		u8AfeBuffer[eAFE_BYTE2] = 0;
		u8AfeBuffer[eAFE_BYTE3] = 0;
		u8AfeBuffer[eAFE_BYTE4] = 0;
	}
	else
	{
	    if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    	{
			u8AfeBuffer[eAFE_BYTE1] = 0xff;
			u8AfeBuffer[eAFE_BYTE2] = 0xff;
			u8AfeBuffer[eAFE_BYTE3] = 0xff;
			u8AfeBuffer[eAFE_BYTE4] = 0xff;    	
    	}
		else
		{
			_vAFEData();
			u8AfeBuffer[eAFE_BYTE1] = u8LcdRAM[eAFE_RAM_ADD_BYTE1];
			u8AfeBuffer[eAFE_BYTE2] = u8LcdRAM[eAFE_RAM_ADD_BYTE2];
			u8AfeBuffer[eAFE_BYTE3] = u8LcdRAM[eAFE_RAM_ADD_BYTE3];
			u8AfeBuffer[eAFE_BYTE4] = u8LcdRAM[eAFE_RAM_ADD_BYTE4];
		}
	}
	
	

	
	for(i = 0; i <eAFE_NUM_OF_BYTE; i++)
	{
		if (u8PrevAfeBuffer[i] != u8AfeBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevAfeBuffer[i] =  u8AfeBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{

		_vI2CCopyBuffer(eAfeDisplay);
		bDataChanged = FALSE;
	}

}


/*********************************************************************//**
*
* Defining AFE data
*
* @param	  None	  
*
* @return	  None
*************************************************************************/

static void _vAFEData(void)
{
	static u16 AfeValue = 0;
	u8 loop = 0;
	u8 ArrDisplay[3];  //3 digit data of AFE
	u8 u8FuelType = 0;
    SCREEN_SET eScreenMode = FALSE;
	
	eScreenMode = u8GetScreenMode();
    
	u8FuelType = u8GetVehicleType();   
	
	if((eScreenMode == eHourAFETripA || eScreenMode == eHourAFETripB) && u8FuelType == FUEL_TYPE_LPH)
	{
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_KG_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_L_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_SLASH_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_SLASH1_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_LIT_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_KM_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_DOT_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SYMBOLE_ON;
		
		if(eScreenMode == eHourAFETripA)
			AfeValue = u16GetDispTripAAfe();
		else if(eScreenMode == eHourAFETripB)
			AfeValue = u16GetDispTripBAfe();
	}

	else if((eScreenMode == eTripAScreen || eScreenMode == eTripBScreen) && u8FuelType == FUEL_TYPE_KMPL)
	{
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_KG_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_L_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_SLASH_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_SLASH1_SYMBOLE_OFF; 		// HR Slash
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_LIT_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_KM_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_DOT_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SYMBOLE_ON;
		
		if(eScreenMode == eTripAScreen)			
			AfeValue = u16GetDispTripAAfe();
		else if(eScreenMode == eTripBScreen)
			AfeValue = u16GetDispTripBAfe();			
	}

	else if((eScreenMode == eTripAScreen || eScreenMode == eTripBScreen) && u8FuelType == FUEL_TYPE_KMPKG)
	{
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_KG_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= ~AFE_L_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_SLASH_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_SLASH1_SYMBOLE_OFF; 		// HR Slash
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= AFE_LIT_SYMBOLE_OFF;
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] |= AFE_KM_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_DOT_SYMBOLE_ON;
		u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SYMBOLE_ON;
		
		if(eScreenMode == eTripAScreen)			
			AfeValue = u16GetDispTripAAfe();
		else if(eScreenMode == eTripBScreen)
			AfeValue = u16GetDispTripBAfe();			
	}

	if(AfeValue < 100)
	{
		if(AfeValue == 0)
		{
			_vAfeDigitPos1('-');
			_vAfeDigitPos2('-');
			_vAfeDigitPos3('-');
		}
		else
		{
			ArrDisplay[loop++]=AfeValue%10;
	    	AfeValue=AfeValue/10;
	    	ArrDisplay[loop++]=AfeValue%10;
	    	AfeValue=AfeValue/10;	
			_vAfeDigitPos1(ArrDisplay[0]);
			_vAfeDigitPos2(ArrDisplay[1]);
			_vAfeDigitPos3('X');
		}
	}
	else
		{
			ArrDisplay[loop++]=AfeValue%10;
	    	AfeValue=AfeValue/10;
	    	ArrDisplay[loop++]=AfeValue%10;
	    	AfeValue=AfeValue/10;
			ArrDisplay[loop++]=AfeValue;
			_vAfeDigitPos1(ArrDisplay[0]);
			_vAfeDigitPos2(ArrDisplay[1]);
			_vAfeDigitPos3(ArrDisplay[2]);
		}



	if((eScreenMode == eHourAFETripA || eScreenMode == eHourAFETripB) 
		&& (u8FuelType == FUEL_TYPE_KMPL ||  u8FuelType == FUEL_TYPE_KMPKG))
	{
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= 0;
	}

	if((eScreenMode == eTripAScreen || eScreenMode == eTripBScreen) 
		&& u8FuelType == FUEL_TYPE_LPH)
	{
		u8LcdRAM[eAFE_RAM_ADD_BYTE1] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= 0;
		u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= 0;
	}
}


/*********************************************************************//**
*
* display first Digit on AFE screen area
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vAfeDigitPos1(u8 AfeDigit)
{
	switch(AfeDigit)
	{
		case 0:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		case '-':
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] |= AFE_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE2] &= AFE_SEG_G_OFF;
		break;
	}


}

/*********************************************************************//**
*
* display second Digit on AFE screen area
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/

static void _vAfeDigitPos2(u8 AfeDigit)
{
	switch(AfeDigit)
	{
		case 0:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_E_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_G_OFF<<1)|1;
			break;
		case 1:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_G_OFF<<1)|1;
		break;
		case 2:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_E_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case 3:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case 4:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case 5:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;		
		case 6:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_E_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case 7:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_G_OFF<<1)|1;
		break;
		case 8:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_E_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case 9:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_F_ON<<1)|1;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		case '-':
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1);
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1);
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] |= (AFE_SEG_G_ON<<1)|1;
		break;
		default:
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_E_OFF<<1);
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_F_OFF<<1);
			u8LcdRAM[eAFE_RAM_ADD_BYTE3] &= (AFE_SEG_G_OFF<<1);
		break;
	}


}


/*********************************************************************//**
*
* display third Digit on AFE screen area
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vAfeDigitPos3(u8 AfeDigit)
{
	switch(AfeDigit)
	{
		case 0:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_E_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_A_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_B_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_C_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_D_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_F_ON;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		case '-':
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] |= AFE_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_A_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_B_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_C_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_D_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_E_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_F_OFF;
			u8LcdRAM[eAFE_RAM_ADD_BYTE4] &= AFE_SEG_G_OFF;
		break;
	}


}

/*********************************************************************//**
*
* Display OAT screen area ,OAT symbole and digits 
*
* @param	  digits to be display 	  
*
* @return	  None
*************************************************************************/

static void _vOATDisplay()
{
	u8 i = 0;
	u8 loop = 0;
	u8 ArrDisplay[2];  
	static bool bDataChanged = FALSE;
	static bool bDataChanged1 = FALSE;
	u8 I2cBusQue = 0;
	u8 OATValue = 0;

	OATValue = u8GetOatValue();

	u8OATBuffer[eOAT_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8OATBuffer[eOAT_COLUMN_ADD] = COLUMN_ADD;
	u8OATBuffer[eOAT_PARAMETR_ADD] = PARAMETR_ADD;
	u8OATBuffer[eOAT_ADD] = OAT_ADD;
	u8OATBuffer[eOAT_DISP_COMMAND] = DISP_COMMAND;

	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayOat == eEnable)
	{

		if(bGetOatDisplayStatus()) //Clear digit when OAT CAN lot
		{
			ArrDisplay [0] = 10;	//Fill value > 9 ,to clear OAT digits
			ArrDisplay [1] = 10;
		}
		else
		{
	    	if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
	    		OATValue = 88;
			
			while(loop < 2)
			{
				ArrDisplay[loop]=OATValue%10;
				OATValue=OATValue/10;
				loop++;
			}
		}
		 u8LcdRAM[eOAT_RAM_ADD_BYTE1] |= OAT_SYMBOLE_ON;
	}
	else
	{
		u8LcdRAM[eOAT_RAM_ADD_BYTE1] &= OAT_SYMBOLE_OFF;
		ArrDisplay [0] = 10;	//Fill value > 9 ,to clear OAT digits
		ArrDisplay [1] = 10;
	
	}

	 _vOATDigitPos1(ArrDisplay[1]);
	 _vOATDigitPos2(ArrDisplay[0]);
	

	u8OATBuffer[eOAT_BYTE1] = u8LcdRAM[eOAT_RAM_ADD_BYTE1];
	u8OATBuffer[eOAT_BYTE2] = u8LcdRAM[eOAT_RAM_ADD_BYTE2];
	u8OATBuffer[eOAT_BYTE3] = u8LcdRAM[eOAT_RAM_ADD_BYTE3];

	for(i = 0; i <eOAT_NUM_OF_BYTE; i++)
	{
		if (u8PrevOATBuffer[i] != u8OATBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevOATBuffer[i] =  u8OATBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eOAT_NUM_OF_BYTE;
		_vI2CCopyBuffer(eOATDisplay);
		bDataChanged = FALSE;
	}

	//for Degree centigrade sysmbole display
	u8DCSBuffer[eDCS_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8DCSBuffer[eDCS_COLUMN_ADD] = COLUMN_ADD;
	u8DCSBuffer[eDCS_PARAMETR_ADD] = PARAMETR_ADD;
	u8DCSBuffer[eDCS_ADD] = DEGREE_CENTIGRADE_ADD;
	u8DCSBuffer[eDCS_DISP_COMMAND] = DISP_COMMAND;
	
  	if(bGetOatDisplayStatus() || (sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayOat == eDisable) )
  		u8LcdRAM[eDCS_RAM_ADD_BYTE1] &= DEGREE_SYMBOLE_OFF;
	else		
		u8LcdRAM[eDCS_RAM_ADD_BYTE1] |= DEGREE_SYMBOLE_ON;
	

	u8DCSBuffer[eDCS_BYTE1] = u8LcdRAM[eDCS_RAM_ADD_BYTE1];

	for(i = 0; i <eDCS_NUM_OF_BYTE; i++)
	{
		if (u8PrevDCSBuffer[i] != u8DCSBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDCSBuffer[i] =  u8DCSBuffer[i];
					bDataChanged1 = TRUE;
				}
			}
		}
	}
	if( bDataChanged1 == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eDCS_NUM_OF_BYTE;
		_vI2CCopyBuffer(eDegreeSymboleDisplay);
		bDataChanged1 = FALSE;
	}

}


/*********************************************************************//**
*
* Display OAT first digit 
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vOATDigitPos1(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_C_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_C_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
	}
	
}


/*********************************************************************//**
*
* Display OAT second digit 
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/

static void _vOATDigitPos2(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_C_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_C_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eOAT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
	}
	
}


/*********************************************************************//**
*
* Display battery volt and symbole 
*
* @param	  digits to be display 	  
*
* @return	  None
*************************************************************************/

static void _vBattVoltDisplay()
{
	u8 i = 0;
	u8 loop = 0;
	u8 ArrDisplay[2];  //fill blank in array so that leading zero can clear in display 
	static bool bDataChanged = FALSE;
	u8 I2cBusQue = 0;
	u8 VoltData =0;

	VoltData = u8GetBatteryVolt();
	
	u8BattVoltBuffer[eBATT_VOLT_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8BattVoltBuffer[eBATT_VOLT_COLUMN_ADD] = COLUMN_ADD;
	u8BattVoltBuffer[eBATT_VOLT_PARAMETR_ADD] = PARAMETR_ADD;
	u8BattVoltBuffer[eBATT_VOLT_ADD] = BATT_VOLT_ADD;
	u8BattVoltBuffer[eBATT_VOLT_DISP_COMMAND] = DISP_COMMAND;
	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bDisplayBattery)
	{
    	if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    		VoltData = 88;
		while(loop < 2)
		{
			ArrDisplay[loop]=VoltData%10;
			VoltData=VoltData/10;
			loop++;
		}	
		 u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE1] |= BATT_SYMBOLE_ON;
		 u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE4] |= V_SYMBOLE_ON;
	}
	else
	{
		u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE1] &= BATT_SYMBOLE_OFF;
		u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE4] &= V_SYMBOLE_OFF;
		ArrDisplay[0] = 10; // Fill >9 to clear digits
		ArrDisplay[1] = 10;
	}

	
	_vBattVoltDigitPos1(ArrDisplay[0]);
	_vBattVoltDigitPos2(ArrDisplay[1]);

	u8BattVoltBuffer[eBATT_VOLT_BYTE1] = u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE1];
	u8BattVoltBuffer[eBATT_VOLT_BYTE2] = u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2];
	u8BattVoltBuffer[eBATT_VOLT_BYTE3] = u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3];
	u8BattVoltBuffer[eBATT_VOLT_BYTE4] = u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE4];

	for(i = 0; i <eBATT_VOLT_NUM_OF_BYTE; i++)
	{
		if (u8PrevBattVoltBuffer[i] != u8BattVoltBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevBattVoltBuffer[i] =  u8BattVoltBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eBATT_VOLT_NUM_OF_BYTE;
		_vI2CCopyBuffer(eBattVoltDisplay);
		bDataChanged = FALSE;
	}

}


/*********************************************************************//**
*
* Display first digit of battery volt 
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vBattVoltDigitPos1(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_C_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] |= BATT_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_C_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE3] &= BATT_SEG_G_OFF;
		break;
	}

}


/*********************************************************************//**
*
* Display second digit of battery volt 
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/

static void _vBattVoltDigitPos2(u8 Digit)
{
	switch(Digit)
	{
		case 0:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 1:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 2:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_C_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 3:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 4:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 5:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;		
		case 6:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 7:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
		case 8:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_E_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		case 9:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_A_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_B_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_C_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_D_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_F_ON;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] |= BATT_SEG_G_ON;
		break;
		default:
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_A_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_B_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_C_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_D_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_E_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_F_OFF;
			u8LcdRAM[eBATT_VOLT_RAM_ADD_BYTE2] &= BATT_SEG_G_OFF;
		break;
	}

}



/*********************************************************************//**
*
* Display clock digits and AM/PM
*
* @param	  digits to be display 
*             AM/PM status 
*
* @return	  None
*************************************************************************/

static void _vClockDisplay()
{
	u8 i = 0;
	static bool bDataChanged = FALSE;
	u8 I2cBusQue  =0;
	u16 ClockData = 0;
	bool AmPmFlag = 0;
	u8ClockBuffer[eCLOCK_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8ClockBuffer[eCLOCK_COLUMN_ADD] = COLUMN_ADD;
	u8ClockBuffer[eCLOCK_PARAMETR_ADD] = PARAMETR_ADD;
	u8ClockBuffer[eCLOCK_ADD] = CLOCK_ADD;
	u8ClockBuffer[eCLOCK_DISP_COMMAND] = DISP_COMMAND;
	u8LcdRAM[eCLOCK_RAM_ADD_BYTE3] |= CLOCK_DOT_ON;

	
	ClockData = u16GetClockData();
	AmPmFlag = bGetDispClockAmPmFlag();
	
    if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    {
     ClockData = 8888;
     _vClockSetting(ClockData, AmPmFlag);
	 u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= AM_SEG_ON;
	 u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= PM_SEG_ON;
    }
	else
	{
		_vClockSetting(ClockData, AmPmFlag);
	}

	u8ClockBuffer[eCLOCK_BYTE1] = u8LcdRAM[eCLOCK_RAM_ADD_BYTE1];
	u8ClockBuffer[eCLOCK_BYTE2] = u8LcdRAM[eCLOCK_RAM_ADD_BYTE2];
	u8ClockBuffer[eCLOCK_BYTE3] = u8LcdRAM[eCLOCK_RAM_ADD_BYTE3];
	u8ClockBuffer[eCLOCK_BYTE4] = u8LcdRAM[eCLOCK_RAM_ADD_BYTE4];
	u8ClockBuffer[eCLOCK_BYTE5] = u8LcdRAM[eCLOCK_RAM_ADD_BYTE5];

	for(i = 0; i <eCLOCK_NUM_OF_BYTE; i++)
	{
		if (u8PrevClockBuffer[i] != u8ClockBuffer[i])
		{
			//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevClockBuffer[i] =  u8ClockBuffer[i];
					bDataChanged = TRUE;
				}
			}
			
		}
	}
	if( bDataChanged == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eCLOCK_NUM_OF_BYTE;
		_vI2CCopyBuffer(eClockDisplay);
		bDataChanged = FALSE;
	}	
	
}

/*********************************************************************//**
*
* Setting Clock and Blinking Text at Hrs Mins and AM/PM
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vClockSetting(u16 ClockData, bool AmPmFlag)
{	
	
	static u16 u16TimeStamp = 0;
	static bool bStatusFlag = FALSE;
	u8 u8ScreenStatus = 0;

	u8ScreenStatus = GetLcdScreenStatus();
	
	// If Clock Hour Selected then Blink Text at Hrs 
	if(u8ScreenStatus == eClockHour)
	{		
		if(AmPmFlag)
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= PM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;
		}
		else
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= AM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;
		}
		if(u16GetClockTicks() - u16TimeStamp  > 500)
		{
			u16TimeStamp = u16GetClockTicks() ;
			bStatusFlag = ~bStatusFlag;			
		}

		_vClockDigitRamFill(ClockData,FALSE,bStatusFlag);
	}

	// If Clock Minutes Selected then Blink Text at Mins 
	else if(u8ScreenStatus == eClockMinute)
	{
		
		if(AmPmFlag)
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= PM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;
		}
		else
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= AM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;
		}
		if(u16GetClockTicks() - u16TimeStamp  > 500)
		{
			u16TimeStamp = u16GetClockTicks() ;
			bStatusFlag = ~bStatusFlag;			
		}
		_vClockDigitRamFill(ClockData,bStatusFlag,FALSE);
	}

	// If Clock AM/PM Selected then Blink Text at AM/PM 
	else if(u8ScreenStatus == eClockAMPM)
	{				
		_vClockDigitRamFill(ClockData,FALSE,FALSE);
		if(u16GetClockTicks() - u16TimeStamp  > 500)
		{
			u16TimeStamp = u16GetClockTicks() ;
			bStatusFlag = ~bStatusFlag;
		}
		if(AmPmFlag)
		{			
			if(bStatusFlag)
			{
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= PM_SEG_ON;
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;			  
			}
			else
			{
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;	 
			}
		
		}

		else
		{			
			if(bStatusFlag)
			{
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= AM_SEG_ON;
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;			  
			}
			else
			{
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;	 
			}
			
		}
	}

	else
	{
		if(AmPmFlag)
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= PM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= AM_SEG_OFF;
		}
		else
		{
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] |= AM_SEG_ON;
			u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] &= PM_SEG_OFF;
		}
		_vClockDigitRamFill(ClockData,FALSE,FALSE);
	}
}


/*********************************************************************//**
*
* Display clock first digit
*
* @param	  Clock value  
*			  Min digit display status
*			  Hour digit display status
*
* @return	  None
*************************************************************************/

static void _vClockDigitRamFill(u16 clockData,bool MinToggleFlag ,bool HourToggleFlag)
{
	u8 i =0;
	u8 loop = 0;
	u8 ArrDisplay[4];  //fill blank in array so that leading zero can clear in display


//if before 2 digit is non zero then fill array up to non zero digit 
	while(loop < 4)
	{
		ArrDisplay[loop]=clockData%10;
		clockData=clockData/10;
		loop++;
	}

	if(MinToggleFlag)
	{
		ArrDisplay[0] = 10; //10 for keep blank
		ArrDisplay[1] = 10; //10 for keep blank
	}

	if(HourToggleFlag)
	{
		ArrDisplay[2] = 10; //10 for keep blank
		ArrDisplay[3] = 10; //10 for keep blank
	}

	for (i =0 ; i < 4; i++)
	{
		switch(i)
		{
		case 0:

				u8LcdRAM[eCLOCK_RAM_ADD_BYTE4] = (ClockDigitArray[ ArrDisplay[i] ] & 0x0F) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE4] & 0xF0);
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] = (ClockDigitArray[ ArrDisplay[i] ] & 0xE0) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE5] & 0x1F);
		
		break;
		case 1:

				u8LcdRAM[eCLOCK_RAM_ADD_BYTE3] = (ClockDigitArray[ ArrDisplay[i] ] & 0x0F) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE3] & 0xF0);
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE4] = (ClockDigitArray[ ArrDisplay[i] ] & 0xE0) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE4] & 0x1F);
			
				
		break;
		case 2:

				u8LcdRAM[eCLOCK_RAM_ADD_BYTE2] = (ClockDigitArray[ ArrDisplay[i] ] & 0x0F) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE2] & 0xF0);
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE3] = (ClockDigitArray[ ArrDisplay[i] ] & 0xE0) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE3] & 0x1F);
	
			
		break;
		case 3:
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE1] = (ClockDigitArray[ ArrDisplay[i] ] & 0x0F) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE1] & 0xF0);
				u8LcdRAM[eCLOCK_RAM_ADD_BYTE2] = (ClockDigitArray[ ArrDisplay[i] ] & 0xE0) | (u8LcdRAM[eCLOCK_RAM_ADD_BYTE2] & 0x1F);

		break;
		
		default:
		
		break;
		}
	}
	
}


/*********************************************************************//**
*
* Display trip data and TRIP A and B
*
* @param	  digits to be display 	  
*
* @return	  None
*************************************************************************/

static void _vTripDisplay()
{
	u8 i = 0;
	u8 I2cBusQue =0;
	static bool bDataChanged = FALSE;
	//Fill location 0 & 1 to display 0.0
	

	u8TripBuffer[eTRIP_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8TripBuffer[eTRIP_COLUMN_ADD] = COLUMN_ADD;
	u8TripBuffer[eTRIP_PARAMETR_ADD] = PARAMETR_ADD;
	u8TripBuffer[eTRIP_SEG_ADD] = TRIP_ADD;
	u8TripBuffer[eTRIP_DISP_COMMAND] = DISP_COMMAND;


    if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    {
   		 _vTripDataAtTransition();
    	u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= TRIP_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE5] |= TRIP_DOT;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= TRIP_KM_ON;	     
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= A_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= B_CHAR_ON;	
		/*if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayDte == eEnable)
		{
			u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= DTE_CHAR_ON;	
		}		
		*/
		u8LcdRAM[eTRIP_RAM_ADD_BYTE4] |= TRIP_HRS_CHAR_ON;
	
    }
	else
	{
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= ~DTE_CHAR_ON;	//OFF DTC ; feature is package protected for future 
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= TRIP_CHAR_ON;

		_vTripDataAtTransition();

		u8LcdRAM[eTRIP_RAM_ADD_BYTE5] |= TRIP_DOT;
	}



	u8TripBuffer[eTRIP_BYTE1] = u8LcdRAM[eTRIP_RAM_ADD_BYTE1];
	u8TripBuffer[eTRIP_BYTE2] = u8LcdRAM[eTRIP_RAM_ADD_BYTE2];
	u8TripBuffer[eTRIP_BYTE3] = u8LcdRAM[eTRIP_RAM_ADD_BYTE3];
	u8TripBuffer[eTRIP_BYTE4] = u8LcdRAM[eTRIP_RAM_ADD_BYTE4];
	u8TripBuffer[eTRIP_BYTE5] = u8LcdRAM[eTRIP_RAM_ADD_BYTE5];
	u8TripBuffer[eTRIP_BYTE6] = u8LcdRAM[eTRIP_RAM_ADD_BYTE6];

	for(i = 0; i <eTRIP_NUM_OF_BYTE; i++)
	{
		if (u8PrevTripBuffer[i] != u8TripBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop

			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{

				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevTripBuffer[i] =  u8TripBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eTRIP_NUM_OF_BYTE;
		_vI2CCopyBuffer(eTripDisplay);
		bDataChanged = FALSE;
	}	
	
}

/*********************************************************************//**
*
* Display/Change trip value A/B, Hrs/Kms on Screen Transition   
*
* @param	  None	  
*
* @return	  None
*************************************************************************/

static void _vTripDataAtTransition()
{
	static u32 value = 0;
	u32 Temp1 = 0;
	u32 Temp2 = 0;	
	SCREEN_SET eScreenMode = eTripAScreen;
	
	eScreenMode = u8GetScreenMode();
	
	if(eScreenMode == eTripAScreen)
	{
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= TRIP_KM_ON;	     
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= A_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= B_CHAR_OFF;		
		u8LcdRAM[eTRIP_RAM_ADD_BYTE4] &= TRIP_HRS_CHAR_OFF;
		Temp1 = u32GetOdoTripAValue();
		Temp2 = u8GetOdoTripAFractionValue();
		value = Temp1 * 10 + Temp2;
	
	}

	else if(eScreenMode == eTripBScreen)
	{
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= TRIP_KM_ON;	     
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= ~A_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= B_CHAR_ON;		
		u8LcdRAM[eTRIP_RAM_ADD_BYTE4] &= TRIP_HRS_CHAR_OFF;
		Temp1 = u32GetOdoTripBValue();
		Temp2 = u8GetOdoTripBFractionValue();
		value = Temp1 * 10 + Temp2;
	}
	
	else if(eScreenMode == eHourAFETripA)
	{
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= A_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= B_CHAR_OFF;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= ~TRIP_KM_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE4] |= TRIP_HRS_CHAR_ON;
		Temp1 = u32GetHoursTripAValue();
		Temp2 = u8GetHoursTripAFractionValue();			
		value = Temp1*10 + Temp2;
	}

	else if(eScreenMode == eHourAFETripB)
	{
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= ~TRIP_KM_ON;	     
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] &= ~A_CHAR_ON;
		u8LcdRAM[eTRIP_RAM_ADD_BYTE6] |= B_CHAR_ON;		
		u8LcdRAM[eTRIP_RAM_ADD_BYTE4] |= TRIP_HRS_CHAR_ON;
		Temp1 = u32GetHoursTripBValue();
		Temp2 = u8GetHoursTripBFractionValue();			
		value = Temp1*10 + Temp2;		
	}
    else
    {
        value = 0;  
    }

    if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    	value = 888888;

	
	
	_vTripDigitRamFill(value);
	
}


/*********************************************************************//**
*
* Display trip first digit 
*
* @param	  digit to be display 	  
*
* @return	  None
*************************************************************************/
static void _vTripDigitRamFill(u32 value)
{
		u8 i = 0;
		u8 loop = 0;
		u8 ArrDisplay[6]={"      "};	//fill blank in array so that leading zero can clear in display 
		
		ArrDisplay[loop++]=value%10;
		value=value/10;
		ArrDisplay[loop++]=value%10;
		value=value/10;
		
		//if before 2 digit is non zero then fill array up to non zero digit 
		while(value > 0)
		{
			ArrDisplay[loop] = value%10;
			value = value/10;
			loop++;
		}
		
	   if(loop < 5 )
		{
		 for( i = loop; i < 5; i++ )
			ArrDisplay[i] = 10; // to show Blank Msb digits 
		}

			
	for( i = 0; i < 5; i++ )
	{

		switch(i)
		{
			case 0:
				u8LcdRAM[eTRIP_RAM_ADD_BYTE5] = (TripDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE5] & 0xF0);
				u8LcdRAM[eTRIP_RAM_ADD_BYTE6] = (TripDigitArray[ArrDisplay[i]] & 0xE0) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE6] & 0x1F);
				
			break;
			case 1:

				u8LcdRAM[eTRIP_RAM_ADD_BYTE4] = (TripDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE4] & 0xF0);
				u8LcdRAM[eTRIP_RAM_ADD_BYTE5] = (TripDigitArray[ArrDisplay[i]] & 0xE0) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE5] & 0x1F);
				
			break;
			case 2:

				u8LcdRAM[eTRIP_RAM_ADD_BYTE3] = (TripDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE3] & 0xF0);
				u8LcdRAM[eTRIP_RAM_ADD_BYTE4] = ((TripDigitArray[ArrDisplay[i]] & 0xE0)>>1) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE4] & 0x8F);
				
				
			break;
			case 3:

				u8LcdRAM[eTRIP_RAM_ADD_BYTE2] = (TripDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE2] & 0xF0);
				u8LcdRAM[eTRIP_RAM_ADD_BYTE3] = ((TripDigitArray[ArrDisplay[i]] & 0xE0)>>1) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE3] & 0x0F);
				
				
			break;
			case 4:
				u8LcdRAM[eTRIP_RAM_ADD_BYTE1] = (TripDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE1] & 0xF0);
				u8LcdRAM[eTRIP_RAM_ADD_BYTE2] = ((TripDigitArray[ArrDisplay[i]] & 0xE0)>>1) | (u8LcdRAM[eTRIP_RAM_ADD_BYTE2] & 0x0F);
								
			break;
			
			default:
				
			break;
		}

	}

}



/*********************************************************************//**
*
* Display Odo and Hour data
*
* @param	  digits to display
*			  Types of data ODO , Hour
*
* @return	  None
*************************************************************************/
static void _vOdoDisplay()
{	
	u8 i = 0;
	static bool bDataChanged = FALSE;
	u32 value =0;
	u32 Temp1 =0;
	u32 Temp2 =0;
	u8 I2cBusQue =0;
	SCREEN_SET eScreenMode = eTripAScreen;
	
	eScreenMode = u8GetScreenMode();
	//Fill location 0 & 1 to display 0.0

	u8OdoBuffer[eODO_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8OdoBuffer[eODO_COLUMN_ADD] = COLUMN_ADD;
	u8OdoBuffer[eODO_PARAMETR_ADD] = PARAMETR_ADD;
	u8OdoBuffer[eODO_SEG_ADD] = ODO_ADD;
	u8OdoBuffer[eODO_DISP_COMMAND] = DISP_COMMAND;

	if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    {
    	u8LcdRAM[eODO_RAM_ADD_BYTE2] |= ODO_CHAR_ON;
		u8LcdRAM[eODO_RAM_ADD_BYTE1] |= KM_ON;
		u8LcdRAM[eODO_RAM_ADD_BYTE1] |= HRS_CHAR_ON;
		u8LcdRAM[eODO_RAM_ADD_BYTE1] |= ENG_CHAR_ON;
		u8LcdRAM[eODO_RAM_ADD_BYTE3] |= ODO_DOT;
		u8LcdRAM[eODO_RAM_ADD_BYTE1] |=	HRS_ICON_ON;
		value = 8888888; //to dipplay full digit during delf test
    }
	else
	{
		// For Default Screen
		if(eScreenMode == eTripAScreen || eScreenMode == eTripBScreen)
		{
			u8LcdRAM[eODO_RAM_ADD_BYTE2] |= ODO_CHAR_ON;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] |= KM_ON;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] &= HRS_CHAR_OFF;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] &= ENG_CHAR_OFF;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] &=	~HRS_ICON_ON;

			Temp1 = u32GetOdoValue();
			Temp2 = u8GetOdoFractionValue();
			value = Temp1 * 10 + Temp2;			
			
			//When ODO reached at 999999 then do not display fraction part 
			if(Temp1 <= ODO_MAX_RANGE )
			{
				u8LcdRAM[eODO_RAM_ADD_BYTE3] |= ODO_DOT;
			}
			else
			{
				u8LcdRAM[eODO_RAM_ADD_BYTE3] &= ~ODO_DOT;
				value = value/10;				//Remove fraction part 
			}
		}

		// For Displaying Hour Meter Values
		else if(eScreenMode == eHourAFETripA || eScreenMode == eHourAFETripB)
		{
			u8LcdRAM[eODO_RAM_ADD_BYTE2] &= ~ODO_CHAR_ON;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] &=	~KM_ON;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] |=	HRS_CHAR_ON;
			u8LcdRAM[eODO_RAM_ADD_BYTE1] |=	ENG_CHAR_ON;

			if(!bGetHourMeterEnableStatus())
				u8LcdRAM[eODO_RAM_ADD_BYTE1] &=	~HRS_ICON_ON;
			else
				u8LcdRAM[eODO_RAM_ADD_BYTE1] |=	HRS_ICON_ON;
			Temp1 = u32GetHoursValue();
			Temp2 = u8GetHoursFractionValue();			
			value = Temp1*10 + Temp2;

			//When Hours reached at 999999 then do not display fraction part 
			if(Temp1 <= HOUR_MAX_RANGE )
			{
				u8LcdRAM[eODO_RAM_ADD_BYTE3] |= ODO_DOT;
			}
			else
			{
				u8LcdRAM[eODO_RAM_ADD_BYTE3] &= ~ODO_DOT;
				value = value/10;				//Remove fraction part 
			}
		}

		
	}
   
	_vOdoDigitRamFill(value);	

	u8OdoBuffer[eODO_BYTE1] = u8LcdRAM[eODO_RAM_ADD_BYTE1];
	u8OdoBuffer[eODO_BYTE2] = u8LcdRAM[eODO_RAM_ADD_BYTE2];
	u8OdoBuffer[eODO_BYTE3] = u8LcdRAM[eODO_RAM_ADD_BYTE3];
	u8OdoBuffer[eODO_BYTE4] = u8LcdRAM[eODO_RAM_ADD_BYTE4];
	u8OdoBuffer[eODO_BYTE5] = u8LcdRAM[eODO_RAM_ADD_BYTE5];
	u8OdoBuffer[eODO_BYTE6] = u8LcdRAM[eODO_RAM_ADD_BYTE6];
	u8OdoBuffer[eODO_BYTE7] = u8LcdRAM[eODO_RAM_ADD_BYTE7];
	u8OdoBuffer[eODO_BYTE8] = u8LcdRAM[eODO_RAM_ADD_BYTE8];
	u8OdoBuffer[eODO_BYTE9] = u8LcdRAM[eODO_RAM_ADD_BYTE9];


	for(i = 0; i <eODO_NUM_OF_BYTE; i++)
	{
		if (u8PrevOdoBuffer[i] != u8OdoBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevOdoBuffer[i] =  u8OdoBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		gsCopyI2CBufData.u8BlockSize = eODO_NUM_OF_BYTE;
		_vI2CCopyBuffer(eOdoDisplay);
		bDataChanged = FALSE;
	}

}


/*********************************************************************//**
*
* Fill Odo/Hour data in RAM for display to LCD
*
* @param	  digit to display
*			  
*
* @return	  None
*************************************************************************/
static void _vOdoDigitRamFill(u32 value)
{
	u8 i = 0;
    u8 loop = 0;
	u8 ArrDisplay[8]={"        "};  //fill blank in array so that leading zero can clear in display 
	
    ArrDisplay[loop++]=value%10;
	value=value/10;
	ArrDisplay[loop++]=value%10;
	value=value/10;
	
	//if before 2 digit is non zero then fill array up to non zero digit 
	while(value > 0)
	{
		ArrDisplay[loop] = value%10;
		value = value/10;
		loop++;
	}
	
   if(loop < 7 )
   	{
	 for( i = loop; i < 7; i++ )
		ArrDisplay[i] = 10; // to show Blank Msb digits 
   	}
	

	for(i = 0; i <7; i++)
	{
		switch(i)
		{
			case 0: //Position 1		 
				u8LcdRAM[eODO_RAM_ADD_BYTE3] = (OdoDigitArray[ArrDisplay[i]] & 0xF0) | (u8LcdRAM[eODO_RAM_ADD_BYTE3] & 0x01);
				u8LcdRAM[eODO_RAM_ADD_BYTE2] = (OdoDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eODO_RAM_ADD_BYTE2] & 0xF0);		
			break;
			case 1://Position 2
				u8LcdRAM[eODO_RAM_ADD_BYTE4] = (OdoDigitArray[ArrDisplay[i]] & 0xF0) | (u8LcdRAM[eODO_RAM_ADD_BYTE4] & 0x01);
				u8LcdRAM[eODO_RAM_ADD_BYTE3] = (OdoDigitArray[ArrDisplay[i]] & 0x0E) | (u8LcdRAM[eODO_RAM_ADD_BYTE3]& 0xF1);			
			break;
			case 2://Position 3
				u8LcdRAM[eODO_RAM_ADD_BYTE5] = (OdoDigitArray[ArrDisplay[i]] & 0xF0);
				u8LcdRAM[eODO_RAM_ADD_BYTE4] =  (OdoDigitArray[ArrDisplay[i]] & 0x0F) | (u8LcdRAM[eODO_RAM_ADD_BYTE4]& 0xF1);		
			break;
			case 3://Position 4
				u8LcdRAM[eODO_RAM_ADD_BYTE6] = (OdoDigitArray[ArrDisplay[i]] & 0xF0);
				u8LcdRAM[eODO_RAM_ADD_BYTE5] = (OdoDigitArray[ArrDisplay[i]] & 0x0F)| (OdoDigitArray[ArrDisplay[i-1]]& 0xF0);			
			break;
			case 4://Position 5
				u8LcdRAM[eODO_RAM_ADD_BYTE7] = (OdoDigitArray[ArrDisplay[i]] & 0xF0);
				u8LcdRAM[eODO_RAM_ADD_BYTE6] = (OdoDigitArray[ArrDisplay[i]] & 0x0F)| (OdoDigitArray[ArrDisplay[i-1]]& 0xF0);			
			break;
			case 5://Position 6
				u8LcdRAM[eODO_RAM_ADD_BYTE8] = (OdoDigitArray[ArrDisplay[i]] & 0xF0);
				u8LcdRAM[eODO_RAM_ADD_BYTE7] = (OdoDigitArray[ArrDisplay[i]] & 0x0F)| (OdoDigitArray[ArrDisplay[i-1]]& 0xF0);			
			break;		
			case 6://Position 7
				u8LcdRAM[eODO_RAM_ADD_BYTE9] = (OdoDigitArray[ArrDisplay[i]] & 0xF0) | (u8LcdRAM[eODO_RAM_ADD_BYTE9] & 0x0F);
				u8LcdRAM[eODO_RAM_ADD_BYTE8] = (OdoDigitArray[ArrDisplay[i]] & 0x0F)| (OdoDigitArray[ArrDisplay[i-1]]& 0xF0);			
			break;
					
			default:
				
			break;
		}
	}
	
}




/*********************************************************************//**
*
* Display Service Reminder
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vDisplayServiceReminder()
{
	u8 i;
	static bool bDataChanged = FALSE;
	u8 I2cBusQue =0;	
	eSERVICE_REMINDER_TYPE ServiceStatus;
	
	u8ServBuffer[eSERV_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8ServBuffer[eSERV_COLUMN_ADD] = COLUMN_ADD;
	u8ServBuffer[eSERV_PARAMETR_ADD] = PARAMETR_ADD;
	u8ServBuffer[eSERV_ADD] = SERVICE_ADD;
	u8ServBuffer[eSERV_DISP_COMMAND] = DISP_COMMAND;

	 ServiceStatus = eGetServiceStatus();
	 
    if( (ServiceStatus == eServRemdOnContinuous || ServiceStatus == eDueService 
		|| clusterStatuslcd == eClusterIgnitionReset)
		&& sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayServHrSymbol == eEnable)
	{
		u8LcdRAM[eSERVICE_RAM_ADD_BYTE1] |= SERVICE_TEXT_ON;	 
		u8LcdRAM[eSERVICE_RAM_ADD_BYTE2] |= SERVICE_SYMBOLE_ON;	
		
	}
	else
	{
		u8LcdRAM[eSERVICE_RAM_ADD_BYTE1] &= ~SERVICE_TEXT_ON;	 //Off text
		u8LcdRAM[eSERVICE_RAM_ADD_BYTE2] &= ~SERVICE_SYMBOLE_ON; //off spanner symbole		
	}
	

	u8ServBuffer[eSERV_BYTE1] = u8LcdRAM[eSERVICE_RAM_ADD_BYTE1];
	u8ServBuffer[eSERV_BYTE2] = u8LcdRAM[eSERVICE_RAM_ADD_BYTE2];

	for(i = 0; i <eSERV_NUM_OF_BYTE; i++)
	{
		if (u8PrevServBuffer[i] != u8ServBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevServBuffer[i] =  u8ServBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	
	if( bDataChanged == TRUE ){
		gsCopyI2CBufData.u8BlockSize = eSERV_NUM_OF_BYTE;
		_vI2CCopyBuffer(eServDisplay);
		bDataChanged = FALSE;
	}			


}



/*********************************************************************//**
*
* Display illumination bars
*
* @param	  illumination bar no.
*			  
*
* @return	  None
*************************************************************************/
static void _vIllControl()
{
	u8 i;
	static bool bDataChanged = FALSE;
	u8 I2cBusQue =0;
	static u16 IllTimeStamp = 0;
	SCREEN_SET ScreenStatus;
	bool InputStatus = 0;
	u8 IllBarNo = 0;

	
	u8IllBuffer[eIIL_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8IllBuffer[eIIL_COLUMN_ADD] = COLUMN_ADD;
	u8IllBuffer[eIIL_PARAMETR_ADD] = PARAMETR_ADD;
	u8IllBuffer[eIIL_ADD] = ILL_ADD;
	u8IllBuffer[eIIL_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add
	u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_SYMBOLE_ON;	 
	
		IllTimeStamp = u16GetClockTicks();
		//Get lcd screen status 
		ScreenStatus = GetLcdScreenStatus();
	//Illumination bas display only if in illumination setting mode otherwise clear all
	
		
	  if(ScreenStatus == eIllumination || clusterStatuslcd == eClusterIgnitionReset )
	  	{

			if(clusterStatuslcd == eClusterIgnitionReset)
			{
			 IllBarNo = 5;
			}
			else
			{
				InputStatus = bGetIlluminationInputStatus();
				if(InputStatus == TRUE)
					IllBarNo = u8GetNightIlluValue();
				else
					IllBarNo = u8GetDayLightIlluValue();
			}
				
			switch(IllBarNo)
			{
				case 0:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR1_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR2_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR3_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
				case 1:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR1_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR2_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR3_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
				case 2:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR1_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR2_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR3_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
				case 3:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR1_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR2_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR3_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
				case 4:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR1_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR2_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR3_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR4_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
				case 5:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR1_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] |= ILL_BAR2_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR3_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR4_ON;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] |= ILL_BAR5_ON;
				break;	
				default:
					u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR1_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR2_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR3_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
					u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				break;
			}
		
	  	}
		  else
		  	{
		  	    //clear all bar and illumination 
	  			u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR1_OFF;
				u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ILL_BAR2_OFF;
				u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR3_OFF;
				u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR4_OFF;
				u8LcdRAM[eILL_RAM_ADD_BYTE1] &= ILL_BAR5_OFF;
				u8LcdRAM[eILL_RAM_ADD_BYTE2] &= ~ILL_SYMBOLE_ON;	 
		  	}

	u8IllBuffer[eIIL_BYTE1] = u8LcdRAM[eILL_RAM_ADD_BYTE1];
	u8IllBuffer[eIIL_BYTE2] = u8LcdRAM[eILL_RAM_ADD_BYTE2];

	for(i = 0; i <eIIL_NUM_OF_BYTE; i++)
	{
		if (u8PrevIllBuffer[i] != u8IllBuffer[i])
		{
			//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevIllBuffer[i] =  u8IllBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE ){
		gsCopyI2CBufData.u8BlockSize = eIIL_NUM_OF_BYTE;
		_vI2CCopyBuffer(eIllDisplay);
		bDataChanged = FALSE;
	}			


}


/*********************************************************************//**
*
*  Display  light ,medium and heavy symbole(function incomplete )
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vLightMediumHeavyDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	u8 I2cBusQue = 0;
	u8 Level =0;

	u8LMHBuffer[eLMH_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8LMHBuffer[eLMH_COLUMN_ADD] = COLUMN_ADD;
	u8LMHBuffer[eLMH_PARAMETR_ADD] = PARAMETR_ADD;
	u8LMHBuffer[eLMH_ADD] = LMH_ADD;
	u8LMHBuffer[eLMH_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add
	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayEngModeSel)
	{

		if( clusterStatuslcd == eClusterIgnitionReset )
		{
			u8LcdRAM[LMH_ADD/2] |= LIGHT_ON;
			if(sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bDisplayEngineModeLightHeavy == eDisable)
			{
				u8LcdRAM[LMH_ADD/2] |= MEDIUM_ON;
			}
			else
			{
				u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;	
			}
			u8LcdRAM[LMH_ADD/2] |= HEAVY_ON;
		}
		else if(bGetPGNTimeOut(eJEngineModeSelection))
		{
		   	u8LcdRAM[LMH_ADD/2] &= LIGHT_OFF;
			u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;
			u8LcdRAM[LMH_ADD/2] &= HEAVY_OFF;
		}
		else
		{
			Level = (u8)u32GetPGNdata(eJEngineModeSelection);
					
			if(Level == 0)
			{
				u8LcdRAM[LMH_ADD/2] |= LIGHT_ON;
				u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;
				u8LcdRAM[LMH_ADD/2] &= HEAVY_OFF;			
			}
			else if(Level == 50)
			{
			     //Light ,Medium and Heavy display when disable if enable then only Light and Heavy display
				if(sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bDisplayEngineModeLightHeavy == eDisable)
				{
					u8LcdRAM[LMH_ADD/2] |= MEDIUM_ON;
				}
				u8LcdRAM[LMH_ADD/2] &= LIGHT_OFF;		
				u8LcdRAM[LMH_ADD/2] &= HEAVY_OFF;
			}
			else if(Level == 100 )
			{
				u8LcdRAM[LMH_ADD/2] |= HEAVY_ON;
				u8LcdRAM[LMH_ADD/2] &= LIGHT_OFF;
				u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;			
			}
			else 
			{
				u8LcdRAM[LMH_ADD/2] &= HEAVY_OFF;
				u8LcdRAM[LMH_ADD/2] &= LIGHT_OFF;
				u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;			
			}
		}
	}
	else
	{
		u8LcdRAM[LMH_ADD/2] &= LIGHT_OFF;
		u8LcdRAM[LMH_ADD/2] &= MEDIUM_OFF;
		u8LcdRAM[LMH_ADD/2] &= HEAVY_OFF;
	}
	u8LMHBuffer[eLMH_BYTE1] = u8LcdRAM[LMH_ADD/2];

	for(i = 0; i <eLMH_NUM_OF_BYTE; i++)
	{
		if (u8PrevLMHBuffer[i] != u8LMHBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevLMHBuffer[i] =  u8LMHBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eLMHDisplay);
		bDataChanged = FALSE;
	}			

}

/*********************************************************************//**
*
*  Display  Read CAN msg and Display/Clear Engine pre heat indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vEnginePreHeatDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8RGBuffer[eRG_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8RGBuffer[eRG_COLUMN_ADD] = COLUMN_ADD;
	u8RGBuffer[eRG_PARAMETR_ADD] = PARAMETR_ADD;
	u8RGBuffer[eRG_ADD] = RG_ADD;
	u8RGBuffer[eRG_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bEnginePreHeatLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte2.bGlowPlug == eEnable )
		u8LcdRAM[RG_ADD/2] |= GLOW_PLUG_ON;
	else
		u8LcdRAM[RG_ADD/2] &= GLOW_PLUG_OFF;

	u8RGBuffer[eRG_BYTE1] = u8LcdRAM[RG_ADD/2];

	for(i = 0; i <eRG_NUM_OF_BYTE; i++)
	{
		if (u8PrevRGBuffer[i] != u8RGBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevRGBuffer[i] =  u8RGBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eEngPreHeatDisplay);
		bDataChanged = FALSE;
	}	

}


/*********************************************************************//**
*
*  Display  Read retarder input status and Display/Clear indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vRetarderDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8RGBuffer[eRG_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8RGBuffer[eRG_COLUMN_ADD] = COLUMN_ADD;
	u8RGBuffer[eRG_PARAMETR_ADD] = PARAMETR_ADD;
	u8RGBuffer[eRG_ADD] = RG_ADD;
	u8RGBuffer[eRG_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bRetarderLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset)
		&& sIndicatorEnable.Enablebytes.Byte1.bRetarder == eEnable )
		u8LcdRAM[RG_ADD/2] |= RETARDER_ON;
	else
		u8LcdRAM[RG_ADD/2] &= RETARDER_OFF;

	u8RGBuffer[eRG_BYTE1] = u8LcdRAM[RG_ADD/2];

	for(i = 0; i <eRG_NUM_OF_BYTE; i++)
	{
		if (u8PrevRGBuffer[i] != u8RGBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevRGBuffer[i] =  u8RGBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eRetarderDisplay);
		bDataChanged = FALSE;
	}  

}


/*********************************************************************//**
*
*  Display  Read Exhaust Break input status and Display/Clear indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vExhaustBreakDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8DSDEBuffer[eDSDE_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8DSDEBuffer[eDSDE_COLUMN_ADD] = COLUMN_ADD;
	u8DSDEBuffer[eDSDE_PARAMETR_ADD] = PARAMETR_ADD;
	u8DSDEBuffer[eDSDE_ADD] = DSDE_ADD;
	u8DSDEBuffer[eDSDE_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bExhaustBreakLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte3.bExhaustBrake == eEnable )
		u8LcdRAM[DSDE_ADD/2] |= EXHAUST_BREAK_ON;
	else
		u8LcdRAM[DSDE_ADD/2] &= EXHAUST_BREAK_OFF;

	u8DSDEBuffer[eDSDE_BYTE1] = u8LcdRAM[DSDE_ADD/2];

	for(i = 0; i <eDSDE_NUM_OF_BYTE; i++)
	{
		if (u8PrevDSDEBuffer[i] != u8DSDEBuffer[i])
		{
			//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDSDEBuffer[i] =  u8DSDEBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	
		_vI2CCopyBuffer(eExhaustBreakDisplay);
		bDataChanged = FALSE;
		

}

/*********************************************************************//**
*
*  Display  Check Def level symbole CAN msg and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vDefLevelDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue = 0;

	u8DSDEBuffer[eDSDE_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8DSDEBuffer[eDSDE_COLUMN_ADD] = COLUMN_ADD;
	u8DSDEBuffer[eDSDE_PARAMETR_ADD] = PARAMETR_ADD;
	u8DSDEBuffer[eDSDE_ADD] = DSDE_ADD;
	u8DSDEBuffer[eDSDE_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bDefLevelLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayDef == eEnable )
		u8LcdRAM[DSDE_ADD/2] |= LOW_DEF_LEVEL_ON;
	else
		u8LcdRAM[DSDE_ADD/2] &= LOW_DEF_LEVEL_OFF;

	u8DSDEBuffer[eDSDE_BYTE1] = u8LcdRAM[DSDE_ADD/2];

	for(i = 0; i <eDSDE_NUM_OF_BYTE; i++)
	{
		if (u8PrevDSDEBuffer[i] != u8DSDEBuffer[i])
		{
			//Check I2C buffer empty or not 
			for (I2cBusQue = 0;
				(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
				++I2cBusQue)
				; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDSDEBuffer[i] =  u8DSDEBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eLowDefDisplay);
		bDataChanged = FALSE;
	}	


}


/*********************************************************************//**
*
*  Display  Check SCR symbole CAN msg and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vScrDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue = 0;

	u8DSDEBuffer[eDSDE_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8DSDEBuffer[eDSDE_COLUMN_ADD] = COLUMN_ADD;
	u8DSDEBuffer[eDSDE_PARAMETR_ADD] = PARAMETR_ADD;
	u8DSDEBuffer[eDSDE_ADD] = DSDE_ADD;
	u8DSDEBuffer[eDSDE_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add
	Lampstatus = bScrLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte4.bScr == eEnable )
		u8LcdRAM[DSDE_ADD/2] |= SCR_ON;
	else
		u8LcdRAM[DSDE_ADD/2] &= SCR_OFF;

	u8DSDEBuffer[eDSDE_BYTE1] = u8LcdRAM[DSDE_ADD/2];

	for(i = 0; i <eDSDE_NUM_OF_BYTE; i++)
	{
		if (u8PrevDSDEBuffer[i] != u8DSDEBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDSDEBuffer[i] =	u8DSDEBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eScrDisplay);
		bDataChanged = FALSE;
	}	

}


/*********************************************************************//**
*
*  Display  Check DPF symbole CAN msg and Display/Clear   indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vDpfDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8DSDEBuffer[eDSDE_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8DSDEBuffer[eDSDE_COLUMN_ADD] = COLUMN_ADD;
	u8DSDEBuffer[eDSDE_PARAMETR_ADD] = PARAMETR_ADD;
	u8DSDEBuffer[eDSDE_ADD] = DSDE_ADD;
	u8DSDEBuffer[eDSDE_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bDpfLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte3.bDpf == eEnable )
		u8LcdRAM[DSDE_ADD/2] |= DPF_ON;
	else
		u8LcdRAM[DSDE_ADD/2] &= DPF_OFF;

	u8DSDEBuffer[eDSDE_BYTE1] = u8LcdRAM[DSDE_ADD/2];

	for(i = 0; i <eDSDE_NUM_OF_BYTE; i++)
	{
		if (u8PrevDSDEBuffer[i] != u8DSDEBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDSDEBuffer[i] =	u8DSDEBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eDpfDisplay);
		bDataChanged = FALSE;
	}	

}


/*********************************************************************//**
*
*  Display  Check Air filter clogged symbole input status and Display/Clear indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vAirFilterCloggedDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;	
	u8 I2cBusQue = 0;
	
	u8AFCBuffer[eAFC_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8AFCBuffer[eAFC_COLUMN_ADD] = COLUMN_ADD;
	u8AFCBuffer[eAFC_PARAMETR_ADD] = PARAMETR_ADD;
	u8AFCBuffer[eAFC_ADD] = AFC_ADD;
	u8AFCBuffer[eAFC_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	//Lampstatus = bAirFilterCloggedLamp();

	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte2.bAirFilterClogged == eEnable )
		u8LcdRAM[AFC_ADD/2] |= AIR_FILTER_CLOGGED_ON;
	else
		u8LcdRAM[AFC_ADD/2] &= AIR_FILTER_CLOGGED_OFF;

	u8AFCBuffer[eAFC_BYTE1] = u8LcdRAM[AFC_ADD/2];

	for(i = 0; i <eAFC_NUM_OF_BYTE; i++)
	{
		if (u8PrevAFCBuffer[i] != u8AFCBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevAFCBuffer[i] =	u8AFCBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eAirFilterCloggedDisplay);
		bDataChanged = FALSE;
	}	


}


/*********************************************************************//**
*
*  Display  Check Water in fuel symbole CAN msg and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vWaterInFuelDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue = 0;
	
	u8BCWBuffer[eBCW_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8BCWBuffer[eBCW_COLUMN_ADD] = COLUMN_ADD;
	u8BCWBuffer[eBCW_PARAMETR_ADD] = PARAMETR_ADD;
	u8BCWBuffer[eBCW_ADD] = BCW_ADD;
	u8BCWBuffer[eBCW_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bWaterInFuelLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte2.bWaterInFuel == eEnable )
		u8LcdRAM[BCW_ADD/2] |= WATER_IN_FUE_ON;
	else	 	
		u8LcdRAM[BCW_ADD/2] &= WATER_IN_FUE_OFF;

	u8BCWBuffer[eBCW_BYTE1] = u8LcdRAM[BCW_ADD/2];

	for(i = 0; i <eBCW_NUM_OF_BYTE; i++)
	{
		if (u8PrevBCWBuffer[i] != u8BCWBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevBCWBuffer[i] =	u8BCWBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eWaterInFuelDisplay);
		bDataChanged = FALSE;
	}	

}

/*********************************************************************//**
*
*  Display  Check Check Transmission symbole CAN msg and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/
static void _vCheckTransmissionDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8BCWBuffer[eBCW_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8BCWBuffer[eBCW_COLUMN_ADD] = COLUMN_ADD;
	u8BCWBuffer[eBCW_PARAMETR_ADD] = PARAMETR_ADD;
	u8BCWBuffer[eBCW_ADD] = BCW_ADD;
	u8BCWBuffer[eBCW_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bCheckTransmissionLamp();


	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte2.bCheckTrans == eEnable 
		&& sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bTransmissionDisplay == eEnable
		&& (sIndicatorInputType.Inputsbytes.Byte3.u8TransmissionInputType == AT_TRANS
		|| sIndicatorInputType.Inputsbytes.Byte3.u8TransmissionInputType == AMT_TRANS))
	{
		u8LcdRAM[BCW_ADD/2] |= CHECK_TRANS_ON;
	}
	else
	{
		u8LcdRAM[BCW_ADD/2] &= CHECK_TRANS_OFF;
	}

	u8BCWBuffer[eBCW_BYTE1] = u8LcdRAM[BCW_ADD/2];

	for(i = 0; i <eBCW_NUM_OF_BYTE; i++)
	{
		if (u8PrevBCWBuffer[i] != u8BCWBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevBCWBuffer[i] =	 u8BCWBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eCheckTransDisplay);
		bDataChanged = FALSE;
	}	 

}

/*********************************************************************//**
*
*  Display  Check Break Wear input status and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/

static void _vBreakWearDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool Lampstatus = 0;
	u8 I2cBusQue =0;

	u8BCWBuffer[eBCW_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8BCWBuffer[eBCW_COLUMN_ADD] = COLUMN_ADD;
	u8BCWBuffer[eBCW_PARAMETR_ADD] = PARAMETR_ADD;
	u8BCWBuffer[eBCW_ADD] = BCW_ADD;
	u8BCWBuffer[eBCW_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add

	Lampstatus = bBrakeWearLamp();

	if( (Lampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte4.bBrakeWear == eEnable )
		u8LcdRAM[BCW_ADD/2] |= BREAK_WEAK_ON;
	else
		u8LcdRAM[BCW_ADD/2] &= BREAK_WEAK_OFF;

	u8BCWBuffer[eBCW_BYTE1] = u8LcdRAM[BCW_ADD/2];
	for(i = 0; i <eBCW_NUM_OF_BYTE; i++)
	{
		if (u8PrevBCWBuffer[i] != u8BCWBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevBCWBuffer[i] =	u8BCWBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eBreakWearDisplay);
		bDataChanged = FALSE;
	}	

}

/*********************************************************************//**
*
*  Display  Check ESC MIL input status and Display/Clear  indicator symbole
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/

static void _vEscAbsMilDisp(void)
{
	u8 i;
	bool static bDataChanged = FALSE;
	bool ESCLampstatus = 0;
	bool ABSLampstatus = 0;
	u8 I2cBusQue =0;

	u8AbsEscBuffer[eABS_ESC_CONTROL_BYTE_COMMAND] =  CONTROL_BYTE_COMMAND;
	u8AbsEscBuffer[eABS_ESC_COLUMN_ADD] = COLUMN_ADD;
	u8AbsEscBuffer[eABS_ESC_PARAMETR_ADD] = PARAMETR_ADD;
	u8AbsEscBuffer[eABS_ESC_MIL_ADD] = ABS_ESC_MIL_ADD;
	u8AbsEscBuffer[eABS_ESC_DISP_COMMAND] = DISP_COMMAND;
	//Fill data in LCD RAM buffer u8LcdRAM/2 = actual segment add
	ESCLampstatus = bEscMilLamp();
	ABSLampstatus = bAbsFailure();

	if( (ESCLampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
		&& sIndicatorEnable.Enablebytes.Byte1.bEscMil == eEnable)
		u8LcdRAM[ABS_ESC_MIL_ADD/2] |= ESC_MIL_ON;
	else
		u8LcdRAM[ABS_ESC_MIL_ADD/2] &= ~ESC_MIL_ON;

	if((ABSLampstatus == TRUE || clusterStatuslcd == eClusterIgnitionReset )
			&& sIndicatorEnable.Enablebytes.Byte4.bAbsMil == eEnable)
			u8LcdRAM[ABS_ESC_MIL_ADD/2] |= ABS_MIL_ON;
		else
			u8LcdRAM[ABS_ESC_MIL_ADD/2] &= ~ABS_MIL_ON;

	

	u8AbsEscBuffer[eABS_ESC_BYTE1] = u8LcdRAM[ABS_ESC_MIL_ADD/2];

	for(i = 0; i <eABS_ESC_NUM_OF_BYTE; i++)
	{
		if (u8PrevAbsEscBuffer[i] != u8AbsEscBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevAbsEscBuffer[i] =	u8AbsEscBuffer[i];
				bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eEscMilDisplay);
		bDataChanged = FALSE;
	}	


}


/*********************************************************************//**
*
*  Display  Display decorated boundry in LCD 
*
* @param	  None
*			  
*
* @return	  None
*************************************************************************/

static void _vDecorateBoundry(void)
{
	u8 i =0;
	bool static bDataChanged1 = FALSE;
	bool static bDataChanged2 = FALSE;
	bool static bDataChanged3 = FALSE;
	u8 I2cBusQue = 0;

	//Fill buffer for ABOVE_DEF_OIL_BOTTOM_LINE

	u8DecorateBound1[eLINE_CONTROL_BYTE_COMMAND1] =  CONTROL_BYTE_COMMAND;
	u8DecorateBound1[eLINE_COLUMN_ADD1] = COLUMN_ADD;
	u8DecorateBound1[eLINE_PARAMETR_ADD1] = PARAMETR_ADD;
	u8DecorateBound1[eABOVE_DEF_OIL_BOTTOM_LINE_ADD] = ABOVE_DEF_OIL_BOTTOM_LINE_ADD;
	u8DecorateBound1[eLINE_DISP_COMMAND1] = DISP_COMMAND;
	//Fill buffer for LINE_ABOVE_APG

	u8DecorateBound2[eLINE_CONTROL_BYTE_COMMAND2] =  CONTROL_BYTE_COMMAND;
	u8DecorateBound2[eLINE_COLUMN_ADD2] = COLUMN_ADD;
	u8DecorateBound2[eLINE_PARAMETR_ADD2] = PARAMETR_ADD;
	u8DecorateBound2[eLINE_ABOVE_APG_ADD] = LINE_ABOVE_APG_ADD;
	u8DecorateBound2[eLINE_DISP_COMMAND2] = DISP_COMMAND;
	//Fill buffer for LINE_TOP

	u8DecorateBound3[eLINE_CONTROL_BYTE_COMMAND3] =  CONTROL_BYTE_COMMAND;
	u8DecorateBound3[eLINE_COLUMN_ADD3] = COLUMN_ADD;
	u8DecorateBound3[eLINE_PARAMETR_ADD3] = PARAMETR_ADD;
	u8DecorateBound3[eLINE_TOP_ADD] = LINE_TOP_ADD;
	u8DecorateBound3[eLINE_DISP_COMMAND3] = DISP_COMMAND;	 
	//Fill data in LCD RAM buffer
	u8LcdRAM[eLINE_RAM_ADD_BYTE1] |= LINE_ABOVE_DEF_OIL_ON;
	u8LcdRAM[eLINE_RAM_ADD_BYTE1] |= LINE_BOTTOM_ON;
	u8LcdRAM[eLINE_RAM_ADD_BYTE2] |= LINE_ABOVE_APG_ON;
	u8LcdRAM[eLINE_RAM_ADD_BYTE3] |= LINE_TOP_ON;

	u8DecorateBound1[eLINE_BYTE1] = u8LcdRAM[eLINE_RAM_ADD_BYTE1];
	u8DecorateBound2[eLINE_BYTE2] = u8LcdRAM[eLINE_RAM_ADD_BYTE2];
	u8DecorateBound3[eLINE_BYTE3] = u8LcdRAM[eLINE_RAM_ADD_BYTE3];

	for(i = 0; i <eLINE_NUM_OF_BYTE1; i++)
	{
		if (u8PrevDecorateBound1[i] != u8DecorateBound1[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
				u8PrevDecorateBound1[i] = u8DecorateBound1[i];
				bDataChanged1 = TRUE;
				}
			}
		}
	}
	if( bDataChanged1 == TRUE )
	{
	    	_vI2CCopyBuffer(eDecorateBoundry1Display);
			bDataChanged1 = FALSE;
			
	}	


	for(i = 0; i <eLINE_NUM_OF_BYTE2; i++)
	{
		if (u8PrevDecorateBound2[i] != u8DecorateBound2[i])
		{
		 //Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDecorateBound2[i] = u8DecorateBound2[i];
					bDataChanged2 = TRUE;
				}
			}
		}
	}
	if( bDataChanged2 == TRUE )
	{
		_vI2CCopyBuffer(eDecorateBoundry2Display);
		bDataChanged2 = FALSE;
	}	

	for(i = 0; i <eLINE_NUM_OF_BYTE3; i++)
	{
		if (u8PrevDecorateBound3[i] != u8DecorateBound3[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDecorateBound3[i] = u8DecorateBound3[i];
					bDataChanged3 = TRUE;
				}
			}
		}
	}
	if( bDataChanged3 == TRUE )
	{
	_vI2CCopyBuffer(eDecorateBoundry3Display);
	bDataChanged3 = FALSE;
			
	}   
	
}
/*********************************************************************//**
*
* Display bars of rear Air pressure gauge 
*
* @param	  Air pressure Bar no.
*			  Air pressure indicator status 
*
* @return	  None
*************************************************************************/

static void _vRearApgDisplay()
{	
	u8 i; 
	bool static bDataChanged = FALSE;
	u8 RAPGLevel = 0;
	bool AfgSymboleStatus = 0;
	u8 I2cBusQue = 0;
	u8RApgBuffer[eRAPG_CONTROL_BYTE_COMMAND] = CONTROL_BYTE_COMMAND;
	u8RApgBuffer[eRAPG_COLUMN_ADD] = COLUMN_ADD;
	u8RApgBuffer[eRAPG_PARAMETR_ADD] = PARAMETR_ADD;
	u8RApgBuffer[eRAPG_SEG_ADD] = REAR_AP_SEG_ADD;
	u8RApgBuffer[eRAPG_DISP_COMMAND] = DISP_COMMAND;

if(sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eAirPressure 
	&& sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eEnable)
{
	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= RAP_BOUNDRY_1_ON;
	u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= RAP_BOUNDRY_2_ON;
	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= RAP_SYMBOLE_ON;
	
	AfgSymboleStatus = bGetRearApgSymbolStatus();

   if(AfgSymboleStatus)
	 u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= RAP_SYMBOLE_ON;
   else
   	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= RAP_SYMBOLE_OFF;

	RAPGLevel = u8AprBarNo();
   
	//if Apg signal Open and short then clear all bar 
		if(RAPGLevel >= BADDATA)
			RAPGLevel = eBar0;


		if(clusterStatuslcd == eClusterIgnitionReset)
    	{
	    	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= RAP_SYMBOLE_ON;
			RAPGLevel = eBar7;
		}
	  if(sAirPressureBytes.u8APNumOfBars == eApgDigit10)
		{
		
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= ~RAP_BOUNDRY_3_ON;
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= ~RAP_BOUNDRY_4_ON;
			if(RAPGLevel > eBar5 )
				RAPGLevel = eBar5;
		}		
		else if(sAirPressureBytes.u8APNumOfBars == eApgDigit15)
		{
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= RAP_BOUNDRY_3_ON;
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= RAP_BOUNDRY_4_ON;
		}	
		else 
		{
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= RAP_BOUNDRY_3_ON;
			u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= ~RAP_BOUNDRY_4_ON;				
			if(RAPGLevel > eBar6 )
				RAPGLevel = eBar6;			
		}
}
else
{
	//Clear Rear APG screen
	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= ~RAP_BOUNDRY_1_ON;
	u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= ~RAP_BOUNDRY_2_ON;
	u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= ~RAP_BOUNDRY_3_ON;
	u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= ~RAP_BOUNDRY_4_ON;	
	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= ~RAP_SYMBOLE_ON;	
	u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= RAP_SYMBOLE_OFF;
	RAPGLevel = eBar0;

}

	switch(RAPGLevel)
	{
	case eBar0:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= CLR_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= CLR_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;
	break;
	case eBar1:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] &= CLR_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;
	break;	
	case eBar2:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;	 			   
	break;
	case eBar3:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;	
	break;	
	case eBar4:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] &= CLR_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;
	break;	
	case eBar5:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;
	break;
	case eBar6:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= DISP_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] &= CLR_RAP_7;	
	break;
	case eBar7:
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_1;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE3] |= DISP_RAP_2;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_3;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_4;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE2] |= DISP_RAP_5;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= DISP_RAP_6;
		u8LcdRAM[eRAPG_RAM_ADD_BYTE1] |= DISP_RAP_7;
	break;	
	}

	u8RApgBuffer[eRAPG_BYTE1] = u8LcdRAM[eRAPG_RAM_ADD_BYTE1];
	u8RApgBuffer[eRAPG_BYTE2] = u8LcdRAM[eRAPG_RAM_ADD_BYTE2];
	u8RApgBuffer[eRAPG_BYTE3] = u8LcdRAM[eRAPG_RAM_ADD_BYTE3];

	for(i = 0; i <eRAPG_NUM_OF_BYTE; i++)
	{
		if (u8PrevRApgBuffer[i] != u8RApgBuffer[i])
		{

		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevRApgBuffer[i] = u8RApgBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eAPRearBarDisplay);
		bDataChanged = FALSE;
	}   

}

/*********************************************************************//**
*
* Display bars of Front Air pressure gauge 
*
* @param	  Air pressure Bar no.
*			  Air pressure indicator status 
*
* @return	  None
*************************************************************************/

static void _vFrontApgDisplay()
{
	
	u8 i;       
	bool static bDataChanged = FALSE;
	u8 APGLevel = 0;
	bool AfgSymboleStatus = 0;
	u8 I2cBusQue = 0;

	u8FApgBuffer[eAPG_CONTROL_BYTE_COMMAND] = CONTROL_BYTE_COMMAND;
	u8FApgBuffer[eAPG_COLUMN_ADD] = COLUMN_ADD;
	u8FApgBuffer[eAPG_PARAMETR_ADD] = PARAMETR_ADD;
	u8FApgBuffer[eAPG_SEG_ADD] = FRONT_AP_SEG_ADD;
	u8FApgBuffer[eAPG_DISP_COMMAND] = DISP_COMMAND;

	if(sIndicatorInputType.Inputsbytes.Byte5.u8BrakeVariant == eAirPressure 
		&& sLcdScreenConfig.LcdScreenConfigBytes.Byte2.bAirPressureBar == eEnable)
	{
		u8LcdRAM[eAPG_RAM_ADD_BYTE1] |= FAP_BAR_CHAR_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= FAP_BOUNDRY_1_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= FAP_BOUNDRY_2_ON;		
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] |= AP_2_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] |= AP_4_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] |= AP_6_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_8_ON;

		AfgSymboleStatus = bGetFrontApgSymbolStatus();
		
		if(AfgSymboleStatus)
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= FAP_SYMBOLE_ON;	
		else
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= FAP_SYMBOLE_OFF;	
		
		APGLevel = u8ApfBarNo();
		//if Apg signal Open and short then clear all bar 
		if(APGLevel >= BADDATA)
			APGLevel = eBar0;

        if(clusterStatuslcd == eClusterIgnitionReset) //On ALL sengment during self test
    	{
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= FAP_SYMBOLE_ON;	
    		APGLevel = eBar7;
    	}	
	  if(sAirPressureBytes.u8APNumOfBars == eApgDigit10)
		{
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_10_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_12_ON; //OFF 12
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_15_ON;	//OFF 15
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= ~FAP_BOUNDRY_3_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= ~FAP_BOUNDRY_4_ON;
			if(APGLevel > eBar5 )
				APGLevel = eBar5;
		}		
		else if(sAirPressureBytes.u8APNumOfBars == eApgDigit15)
		{
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_10_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_12_ON; 
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_15_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= FAP_BOUNDRY_3_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= FAP_BOUNDRY_4_ON;
		}	
		else 
		{
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_10_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] |= AP_12_ON; 
			u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_15_ON;	//OFF 15
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= FAP_BOUNDRY_3_ON;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= ~FAP_BOUNDRY_4_ON;
			if(APGLevel > eBar6 )
				APGLevel = eBar6;			
		}
	}
	else
	{
		u8LcdRAM[eAPG_RAM_ADD_BYTE1] &= ~FAP_BAR_CHAR_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= ~FAP_BOUNDRY_1_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= ~FAP_BOUNDRY_2_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= ~FAP_BOUNDRY_3_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= ~FAP_BOUNDRY_4_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] &= ~AP_2_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] &= ~AP_4_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE5] &= ~AP_6_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_8_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_10_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_12_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE4] &= ~AP_15_ON;
		u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= FAP_SYMBOLE_OFF;	
		APGLevel = eBar0;
	
	}

	switch(APGLevel)
	{
		case eBar0:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= CLR_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= CLR_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;
		case eBar1:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] &= CLR_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;	
		case eBar2:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;	
		break;
		case eBar3:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;	
		case eBar4:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;	
		case eBar5:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;
		case eBar6:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] &= CLR_FAP_7;
		break;
		case eBar7:
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_1;
			u8LcdRAM[eAPG_RAM_ADD_BYTE2] |= DISP_FAP_2;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_3;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_4;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_5;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_6;
			u8LcdRAM[eAPG_RAM_ADD_BYTE3] |= DISP_FAP_7;
		break;
	}

	u8FApgBuffer[eAPG_BYTE1] = u8LcdRAM[eAPG_RAM_ADD_BYTE1];
	u8FApgBuffer[eAPG_BYTE2] = u8LcdRAM[eAPG_RAM_ADD_BYTE2];
	u8FApgBuffer[eAPG_BYTE3] = u8LcdRAM[eAPG_RAM_ADD_BYTE3];
	u8FApgBuffer[eAPG_BYTE4] = u8LcdRAM[eAPG_RAM_ADD_BYTE4];
	u8FApgBuffer[eAPG_BYTE5] = u8LcdRAM[eAPG_RAM_ADD_BYTE5];

	for(i = 0; i <eAPG_NUM_OF_BYTE; i++)
	{
		if (u8PrevFApgBuffer[i] != u8FApgBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevFApgBuffer[i] = u8FApgBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eAPFrontBarDisplay);
		bDataChanged = FALSE;
	} 

}

/*********************************************************************//**
*
* Display bars of Oil pressure gauge 
*
* @param	  Oil pressure Bar no.
*			  Oil pressure indicator status 
*
* @return	  None
*************************************************************************/

static void _vOilGaugeDisplay()
{
	
	u8 i;       
	bool static bDataChanged = FALSE;
	u8 OilLevel = 0;
	bool lampStatus = 0;
	u8 I2cBusQue = 0;

	u8OilBuffer[eOIL_CONTROL_BYTE_COMMAND] = CONTROL_BYTE_COMMAND;
	u8OilBuffer[eOIL_COLUMN_ADD] = COLUMN_ADD;
	u8OilBuffer[eOIL_PARAMETR_ADD] = PARAMETR_ADD;
	u8OilBuffer[eOIL_SEG_ADD] = OIL_SEG_ADD;
	u8OilBuffer[eOIL_DISP_COMMAND] = DISP_COMMAND;

	if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayOilPress)
	{
		u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= OIL_BAR_CHAR_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= OIL_BOUNDRY_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= OIL_12_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= OIL_10_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= OIL_8_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] |= OIL_6_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] |= OIL_4_ON;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] |= OIL_2_ON;

		if(clusterStatuslcd == eClusterIgnitionReset) //Used for display in self testing 
		{
			 u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= OIL_SYMBOLE_ON;
			 OilLevel = eBar5;
		}
		else
		{

			lampStatus = bOilPressureSymbolStatus();

			if(lampStatus == TRUE)
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= OIL_SYMBOLE_ON;
			else
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= OIL_SYMBOLE_OFF;

			OilLevel = u8OilPressureBarNo();
		}
 	}
	else
	{
		//Clear display of OIL pressure 
		
		u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= OIL_BAR_CHAR_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= OIL_BOUNDRY_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= OIL_12_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= OIL_10_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= OIL_8_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] &= OIL_6_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] &= OIL_4_OFF;
		u8LcdRAM[eOIL_RAM_ADD_BYTE3] &= OIL_2_OFF;

		u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= OIL_SYMBOLE_OFF;
		OilLevel = eBar0;
	}
	
	

	switch(OilLevel)
	{
		case eBar0:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;
		break;
		case eBar1:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;
		break;	
		case eBar2:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] &= CLR_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;		
		break;
		case eBar3:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;
		break;	
		case eBar4:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;
		break;	
		case eBar5:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] &= CLR_OIL_6;		
		break;
		case eBar6:
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_1;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_2;
			u8LcdRAM[eOIL_RAM_ADD_BYTE1] |= DISP_OIL_3;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_4;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_5;
			u8LcdRAM[eOIL_RAM_ADD_BYTE2] |= DISP_OIL_6;
		break;
	}

	u8OilBuffer[eOIL_BYTE1] = u8LcdRAM[eOIL_RAM_ADD_BYTE1];
	u8OilBuffer[eOIL_BYTE2] = u8LcdRAM[eOIL_RAM_ADD_BYTE2];
	u8OilBuffer[eOIL_BYTE3] = u8LcdRAM[eOIL_RAM_ADD_BYTE3];

	for(i = 0; i <eOIL_NUM_OF_BYTE; i++)
	{
		if (u8PrevOilBuffer[i] != u8OilBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevOilBuffer[i] = u8OilBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eOilBarDisplay);
		bDataChanged = FALSE;
	} 

}

/*********************************************************************//**
*
* Display bars of gauge gauge 
*
* @param	  Def Bar no.
*			  Def indicator status 
*
* @return	  None
*************************************************************************/

static void _vDefDisplay()
{
	
	u8 i;       
	bool static bDataChanged = FALSE;
	u8 DefLevel = 0;
	bool SymbolStatus =0;
	u8 I2cBusQue = 0;

	u8DefBuffer[eDEF_CONTROL_BYTE_COMMAND] = CONTROL_BYTE_COMMAND;
	u8DefBuffer[eDEF_COLUMN_ADD] = COLUMN_ADD;
	u8DefBuffer[eDEF_PARAMETR_ADD] = PARAMETR_ADD;
	u8DefBuffer[eDEF_SEG_ADD] = DEF_SEG_ADD;
	u8DefBuffer[eDEF_DISP_COMMAND] = DISP_COMMAND;

   if(sLcdScreenConfig.LcdScreenConfigBytes.Byte1.bDisplayDef)
	{
		if(clusterStatuslcd == eClusterIgnitionReset) //Used for display in self testing 
		{
			u8LcdRAM[eDEF_RAM_ADD_BYTE2] |= F_SYMBOLE_ON;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= E_SYMBOLE_ON;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DEF_ON;	
			u8LcdRAM[eDEF_RAM_ADD_BYTE1] |= DEF_SYMBOLE_ON;
			DefLevel = eBar6;
		}
		else
		{
			u8LcdRAM[eDEF_RAM_ADD_BYTE2] |= F_SYMBOLE_ON;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= E_SYMBOLE_ON;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DEF_ON;	

			SymbolStatus = bGetDefSymbolStatus();
			if(SymbolStatus)
				u8LcdRAM[eDEF_RAM_ADD_BYTE1] |= DEF_SYMBOLE_ON;
			else
				u8LcdRAM[eDEF_RAM_ADD_BYTE1] &= DEF_SYMBOLE_OFF;
			DefLevel = u8DefBarNo();
		}
	}
   else
   	{	//Clear Def Screen
   		u8LcdRAM[eDEF_RAM_ADD_BYTE2] &= ~F_SYMBOLE_ON;
		u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= ~E_SYMBOLE_ON;
		u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= ~DEF_ON;
		DefLevel = eBar0;
		u8LcdRAM[eDEF_RAM_ADD_BYTE1] &= DEF_SYMBOLE_OFF;
   	}
	
	

	switch(DefLevel)
	{
		case eBar0:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_3;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_4;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;
		case eBar1:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_3;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_4;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;	
		case eBar2:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_2;				
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] &= CLR_DEF_3;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_4;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;
		case eBar3:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_3;				
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_4;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;	
		case eBar4:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_3;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_4;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;	
		case eBar5:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_3;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_4;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_5;
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] &= CLR_DEF_6;
		break;
		case eBar6:
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_1;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_2;
			u8LcdRAM[eDEF_RAM_ADD_BYTE4] |= DISP_DEF_3;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_4;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_5;	       
			u8LcdRAM[eDEF_RAM_ADD_BYTE3] |= DISP_DEF_6;
		break;
	}

	u8DefBuffer[eDEF_BYTE1] = u8LcdRAM[eDEF_RAM_ADD_BYTE1];
	u8DefBuffer[eDEF_BYTE2] = u8LcdRAM[eDEF_RAM_ADD_BYTE2];
	u8DefBuffer[eDEF_BYTE3] = u8LcdRAM[eDEF_RAM_ADD_BYTE3];
	u8DefBuffer[eDEF_BYTE4] = u8LcdRAM[eDEF_RAM_ADD_BYTE4];

	for(i = 0; i <eDEF_NUM_OF_BYTE; i++)
	{
		if (u8PrevDefBuffer[i] != u8DefBuffer[i])
		{
		//Check I2C buffer empty or not 
		for (I2cBusQue = 0;
			(I2cBusQue < I2C_MAX_BUFFER_SIZE) && (sI2CControl[I2cBusQue].bDataReady == TRUE);
			++I2cBusQue)
			; // empty statement for this for loop
			if(I2cBusQue < I2C_MAX_BUFFER_SIZE)
			{
				if(sI2CControl[I2cBusQue].bDataReady == FALSE)
				{
					u8PrevDefBuffer[i] = u8DefBuffer[i];
					bDataChanged = TRUE;
				}
			}
		}
	}
	if( bDataChanged == TRUE )
	{
		_vI2CCopyBuffer(eDefBarDisplay);
		bDataChanged = FALSE;
	} 

}

/*********************************************************************//**
*
* LCD initialization ,LCD have a driver S1D15104  funtion R_IIC11_Master_Send don't call 
* from any where except iicdriver.c file
*
* @param	  Def Bar no.
*			  Def indicator status 
*
* @return	  None
*************************************************************************/

void vLcdInit(void)
{
	int i;
	u8 Buffer[120];

	//Set I2c transmit flag 
	vSetI2CBusState(eI2CTxReady);       
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = SWRST;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = RESET_ADD;	 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);
	vWatingTimeMs(100);     //100ms relay required 
	//Wait until transmit flag set , it will clear in IIC interrupt
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();
	
	bIIcErroFlag =FALSE;     
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = DISP_MODE_SET_ADD;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = NORMAL_ADD;	 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE;     
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = DISP_LINE_SET_ADD;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = LINE_4_ADD;	 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);		 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();
	
	bIIcErroFlag =FALSE;     
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = DISP_LINE_SET_ADD1;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = NORMAL_ADD;	 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);			 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE;    
	vSetI2CBusState(eI2CTxReady);     
	Buffer[0] = NORMAL_ADD;
	Buffer[1] = COMMON_REVERSE;		
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,2);	
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = NORMAL_ADD;
	Buffer[1] = COLUMN_DIRECT_NORMAL;		
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,2);		 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE;     
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = DISP_LINE_SET_ADD2;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = CLK_FREQ_125;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);		 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE;    
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = SUPPLY_CONTROL;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = BIAS_ADD;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4); 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = NORMAL_ADD;
	Buffer[1] = OSC_CKT_ON;	 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,2);			 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = DISP_FRAME_FREQ;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = FREQ_200hz;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);			 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = CONTROL_BYTE_COMMAND;
	Buffer[1] = LCD_BIAS_SET;
	Buffer[2] = PARAMETR_ADD1;
	Buffer[3] = BIAS_ADD;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,4);				 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = NORMAL_ADD;
	Buffer[1] = SUPPLY_ON;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,2);		 
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0] = NORMAL_ADD;
	Buffer[1] = DISP_ON;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,2);	
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	Buffer[0]  = PARAMETER_ADD2;
	for(i= 1; i< 120; i++)
	Buffer[i]  = 0x00;		 
	R_IIC11_Master_Send(LCD_DEVICE_ADD,&Buffer,120);
	while( eI2CTxReady == eGetI2CBusState() &&  bIIcErroFlag == FALSE)
		R_WDT_Restart();		 

	 _vSendLcdScreenDataI2c();        
}

static void _vSendLcdScreenDataI2c()
{
	vRefreshLcd();
	
	//BOUNDRY LINE-------------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8DecorateBound1,eLINE_NUM_OF_BYTE1);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();		

		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8DecorateBound2,eLINE_NUM_OF_BYTE2);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();

	//DEF-------------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8DefBuffer,eDEF_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();	

	//OIL PRESSURE-------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8OilBuffer,eOIL_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();
		
	//FRONT AIR PRESSURE-------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8FApgBuffer,eAPG_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();
	//REAR AIR PRESSURE-------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8RApgBuffer,eRAPG_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();		
	//ABS ESC Indicator-------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AbsEscBuffer,eABS_ESC_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();
	//Brake wear ,Check transmssion and wate in fuel Indicator-----------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8BCWBuffer,eBCW_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();

	//Air filter cogged Indicator-----------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AFCBuffer,eAFC_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();	
	//DPF ,SCR,Low Def Level and Exaust break Indicator-----------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8DSDEBuffer,eDSDE_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();
	//Retarder and Glow plug Indicator-----------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8RGBuffer,eRG_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();

	//Light medium and heavi Indicator-----------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8LMHBuffer,eLMH_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();

	//ODO DISPLAY---------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8OdoBuffer,eODO_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();		

	//TRIP DISPLAY---------------------------------------------------------------------		
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);
         
			R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8TripBuffer,eTRIP_NUM_OF_BYTE);
		while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
			R_WDT_Restart();	

	//CLOCK DISPLAY--------------------------------------------------------------------- 	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8ClockBuffer,eCLOCK_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	//BATTERY VOLTAGE DISPLAY---------------------------------------------------------------------	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8BattVoltBuffer,eBATT_VOLT_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	//OAT DISPLAY---------------------------------------------------------------------	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8OATBuffer,eOAT_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8DCSBuffer,eDCS_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();


	//ILLUMINATION  DISPLAY---------------------------------------------------------------------	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8IllBuffer,eIIL_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();	
	//AFE  DISPLAY---------------------------------------------------------------------	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AfeBuffer,eAFE_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	//AT/AMT   DISPLAY---------------------------------------------------------------------	
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AmtAtCharBuffer,eAMT_AT_CHAR_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

		bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AutoCharBuffer,eAUTO_CHAR_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8AmtBuffer,eAMT_DATA_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();
		//TOP LINE DISPLAY--------------------------------------------------------------------- 

		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);

		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8TpmsCharBuffer,eTPMS_CHAR_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	
		bIIcErroFlag =FALSE; 
		vSetI2CBusState(eI2CTxReady);

		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8TpmsStringBuffer,eTPMS_STRING_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	//SERVICE REMINDER   DISPLAY ------------------------------------------------------- 
	bIIcErroFlag =FALSE; 
	vSetI2CBusState(eI2CTxReady);
	 
		R_IIC11_Master_Send(LCD_DEVICE_ADD,&u8ServBuffer,eSERV_NUM_OF_BYTE);
	while( eI2CTxReady == eGetI2CBusState() &&	bIIcErroFlag == FALSE)
		R_WDT_Restart();

	

}
