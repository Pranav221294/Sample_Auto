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
/** ---------------------------------------------------------------------------
 **  COPYRIGHT: Indication Instruments Ltd
 **             Faridabad Haryana
 **             India
 **		   2018	
 ** ---------------------------------------------------------------------------
 **
 **  Revision History
 **
 **  Date       			Name             						Description
 ** ==========  ===============  ==============================================
 ** 
 ** 19/9/2017	Rajni Rajpoot       	Initial Version of CustomTyprDef
 ** 5/9/2017		Rajni Rajpoot	      	Comment typedef bool for freescale controller as its already
 **                                 			define in PE_Types.h
 ** 							       extern commom variable in this file only.
 ** 10/4/2018	Vishnu Jat		Removed external variable. Used typedef instead of #define
 **				Khusboo Lalani
 **10/04/2018  Vishnu  Jat              Correct format of typedef
 ** ***********************************************************************************/                    

#ifndef 	_CUSTOM_TYPE_DEF_H
#define         _CUSTOM_TYPE_DEF_H
#include "r_cg_macrodriver.h"

typedef 	unsigned char		u8;
typedef 	unsigned int		u16;
typedef 	unsigned long		u32;
typedef 	signed char			s8;
typedef 	signed int			s16;
typedef 	signed long			s32;
typedef		float				f32;
typedef		double				f64;
typedef		unsigned char 	    bool;


#define     TRUE            1
#define     FALSE           0
#define     HIGH            1
#define     LOW             0



#endif


