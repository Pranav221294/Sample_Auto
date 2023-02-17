/** ###################################################################
**     Filename  : P15765.c
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 5/4/2016, 7:06 PM
**     Contents  :
**         User source code
**
** ###################################################################*/
/*** ---------------------------------------------------------------------------
**  COPYRIGHT: Indication Instruments Limited
**              Plot 19, Sector 6
**              Industrial Layout
**              Faridabad - 121006
**
**              Copyright 2016
**
**
** ---------------------------------------------------------------------------
** ==========  ===============	==============================================
** 
** Author:  Khusboo Lalani 
*****************************************************************************/



/* MODULE P15765 */

#include "P15765H.h"
#include "TML_14229_UDSH.h"
#include "r_cg_macrodriver.h"
#include "customTypedef.h"
#include "J1939.h" 
#include "Can.h"



//global variables
u8 testerAddress;
//byte gatewayRequestAdr;
//byte gatewayResponseAdr;

sJ1939_RX_MSG_TYPE diagnosticsBuf[RX_QUE_SIZE];

u32 diagnosticsTimer;
multiple_frames multipleFramesRx = {{0},0,0,0,0,eSegmentedTxNotStarted,0,0,eIdle,FALSE,0};
multiple_frames multipleFramesTx = {{0},0,0,0,0,eSegmentedTxNotStarted,0,0,eIdle,FALSE,0};
single_frame singleFrameRx = {0,{0},0,0};
u8 frameConrolDataToSend[8]={0x30,0x0,0x1,0,0,0,0,0};
u32 diagnosticsCANId;
u32 HSCANBusOffRecoveryClk = 0;


//variables
//EOL
u8 	powerMode;
u16 UnderVoltageRecoveryTimeout = 500; //5*100ms
u16 OverVoltageRecoveryTimeout = 500;//5*100ms
u16 BusOffRecoveryTimeout = 1000;//5*200


/// @addtogroup TELL_TALE_LIBRARY
/// @{
 
/// @defgroup TellTaleHeaderLib Global functions used in Tell tale
//// 
///   
/// @addtogroup TellTaleHeaderLib
/// @{

//local variables
static p15765_MSG_STRUCT_TYPE UDSData;

//local functions
static void copySingleFrameDataToMultiFrame(void);

/// @} // endgroup TellTaleHeaderLib

/// @} // endgroup TELL_TALE_LIBRARY


/*********************************************************************//**
 *
 * Process diagnostic services
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/

void processDiagnosticsMessages(void)
{
 u16 i,j;
 static u16 mulBytesIndex;
 //p15765_MSG_STRUCT_TYPE *p14229BufPtr;
 static u8 RxQuePop;
 static u16 numBytesRx;
 
 


 for (RxQuePop = 0; RxQuePop < RX_QUE_SIZE; ++RxQuePop)
 {
	if (diagnosticsBuf[RxQuePop].bDataRdy)
	{
		for(i=0;i<8;i++)
			UDSData.msg15765.dataBytes[i]= diagnosticsBuf[RxQuePop].u8data[i];
		UDSData.timeStamp = diagnosticsBuf[RxQuePop].u32TimeStamp;
		UDSData.ID = diagnosticsBuf[RxQuePop].sMsgID.Identifier;
		diagnosticsCANId = UDSData.ID;
		
		UDSData.msg15765.NPCI.numBytes_MSB_SerialNum_FlowStatus =  UDSData.msg15765.dataBytes[0] & 0x0F;
		UDSData.msg15765.NPCI.frameType =  (UDSData.msg15765.dataBytes[0] >> 4) & 0x0F;
		UDSData.msg15765.NPCI.MulPCI.numBytesLSB = UDSData.msg15765.dataBytes[1];
		UDSData.msg15765.NPCI.MulPCI.FC.blockSize = UDSData.msg15765.dataBytes[1];
		UDSData.msg15765.NPCI.MulPCI.FC.minSepTime = UDSData.msg15765.dataBytes[2];
		// Interested in this PGN?	
	
		switch (UDSData.msg15765.NPCI.frameType)
		{
		    //case PGN65110 : UDSbufPtr = &UDSmsg65110; validMsg = TRUE    break;
		    case eSingle:
				if( multipleFramesTx.status  >= eSegmentedTxNotStarted  &&//its an unexpected PDU if segemented transmission is in progress
					UDSData.msg15765.NPCI.numBytes_MSB_SerialNum_FlowStatus < 8 
					&& UDSData.msg15765.NPCI.numBytes_MSB_SerialNum_FlowStatus > 0)  //15765-2 requirement
				{
					//copy the contents to the buffer for further processing
					singleFrameRx.numBytes = UDSData.msg15765.NPCI.numBytes_MSB_SerialNum_FlowStatus;
					singleFrameRx.timeStamp = UDSData.timeStamp;
					//copy all the bytes including the first byte (=numByte)
					for(j=0; j <= singleFrameRx.numBytes; j++)
						singleFrameRx.dataBytes[j] = UDSData.msg15765.dataBytes[j];
					
					singleFrameRx.ID = UDSData.ID ;//only the first 2 bytes of ID
					singleFrameRx.dataReady = TRUE;
					//diagnostic service byte	
					switch (singleFrameRx.dataBytes[1]) 
					{
						case StartDiagnosticSession:	
							startDiagnosticsSession(); 
							break;

						case ECUReset: 
							ECUResetService();
							break;
							
						case SecurityAccess:
							securityAccessService();
							break;
							
						case CommunicationControl:
							communicationControlService();
							break;	
							
						case TesterPresent:
							TesterPresentService();
							break;
							
						case ControlDTCSetting: //when single DTC is present
							ControlDTCSettingServiceSingle();
							break;
							
						case ReadDataIdentifier:
							//when 2 or 3 data bytes are requested to be read
							if(singleFrameRx.numBytes == 5 || singleFrameRx.numBytes == 7 )
							{
								copySingleFrameDataToMultiFrame();
								ReadDataByIdentifierServiceMultiple();
								//this flag will be checked to clear the multipleFramesRx data
								multipleFramesRx.frameInProgress = eObsolete;
							}
							else
								ReadDataByIdentifierServiceSingle();
								
							break;
							
						case WriteDatabyIdentifier:
							WriteDataByIdentifierServiceSingle();
							break;
							
						case ReadDTCInformationService:
							//ReadDiagnosticTroubleInformationService();
							break;
							
						case ClearDiagnosticInformation:
							//ClearDiagnosticsInformationService();
							break;
					
						case LinkControl:
							LinkControlService();
							break;
							
					//	case AccessingTimingParameter:
						//	AccessingTimingParService();
							//break;

						case InputandOutputControlbyId:
							InputOutputControlByIdentifierService();
							break;
						case RoutineControlService:
							RoutineService();
							break;
						
							
					} 
					sendDiagnosticsResponse();
					singleFrameRx.dataReady = FALSE;
				}
					
			break;
			// this message is received when the other ECU wants to send a message
			case eFirst:
		        if( multipleFramesTx.status >= eSegmentedTxNotStarted //its an unexpected PDU if segemented transmission is in progress
					&& UDSData.msg15765.NPCI.MulPCI.numBytesLSB > 7  
					&& UDSData.msg15765.NPCI.MulPCI.numBytesLSB  < MAX_DATA_LENGTH // number of bytes should be less than receiver buffer size
					)
	        	{
					multipleFramesRx.ID = UDSData.ID; //only the first 2 bytes of ID
					multipleFramesRx.numBytes = UDSData.msg15765.NPCI.MulPCI.numBytesLSB ;
					for(mulBytesIndex=0; mulBytesIndex<8; mulBytesIndex++)
						multipleFramesRx.mulByteDiagRespData[mulBytesIndex] = UDSData.msg15765.dataBytes[mulBytesIndex];
					multipleFramesRx.serialNum++;
					multipleFramesRx.timeStamp = UDSData.timeStamp;
					multipleFramesRx.frameInProgress = eFirst;
					multipleFramesRx.status = eWait;
					numBytesRx = 6;
	        	}
				else if (UDSData.msg15765.NPCI.MulPCI.numBytesLSB  >= MAX_DATA_LENGTH )
				{
					multipleFramesRx.frameInProgress = eTerminate;
					multipleFramesRx.status = eOverflowAndTerminate;
					numBytesRx = 0;
					mulBytesIndex = 0;
					
				}
				//send a diagnostic response
				sendDiagnosticsResponse();
			break;

			case eConsecutive:
				if( multipleFramesTx.status != eWait ) //its an unexpected PDU if segemented transmission is in progress
				{
					multipleFramesRx.frameInProgress = eConsecutive;
					
					//serial numbers do not match
					if(multipleFramesRx.serialNum != (UDSData.msg15765.dataBytes[0] & 0x0f ))
					{
						multipleFramesRx.frameInProgress = eTerminate;
						multipleFramesRx.status = eOverflowAndTerminate;
						numBytesRx = 0;
						mulBytesIndex = 0;
						break;
					}
					multipleFramesRx.serialNum++;
					multipleFramesRx.timeStamp = UDSData.timeStamp;
					//copy all the 7 data bytes into buffer. First byte is left out
					for(j = 1; j<8; j++)
						multipleFramesRx.mulByteDiagRespData[mulBytesIndex++] = UDSData.msg15765.dataBytes[j];
					numBytesRx +=7;
					if(multipleFramesRx.numBytes <= numBytesRx)
					{
						multipleFramesRx.dataReady = TRUE;
						switch (multipleFramesRx.mulByteDiagRespData[2])
						{
							case SecurityAccess:
								//if all the frames have been received
								securityAccessService();
								break;
								
							case ControlDTCSetting://when multiple DTCs are present
								if(commInfo.normalCommRxEnabled == TRUE)
									ControlDTCSettingServiceMultiple();
								break;	
								//when more than 3 data bytes are requested to be read
							case ReadDataIdentifier:
								ReadDataByIdentifierServiceMultiple();
                                break;
							case WriteDatabyIdentifier:
								WriteDataByIdentifierServiceSingle();	
								break;
							case InputandOutputControlbyId:
								InputOutputControlByIdentifierServiceMul();
								break;	
						}
						mulBytesIndex = 0;
						numBytesRx = 0;
						//this flag will be checked to clear the multipleFramesRx data
						multipleFramesRx.frameInProgress = eTxCompleted;
						sendDiagnosticsResponse();
					}
				}
			break;
			
			// this message is received when this ECU has sent the first frame
			
			case eFlowControl:
				if(multipleFramesTx.status == eWait || multipleFramesTx.frameInProgress == eFirst) //if segmented transmission is in progress
				{
					multipleFramesTx.status = UDSData.msg15765.NPCI.numBytes_MSB_SerialNum_FlowStatus;
					multipleFramesTx.blockSize = UDSData.msg15765.NPCI.MulPCI.FC.blockSize;
					
					multipleFramesTx.minSepTime = UDSData.msg15765.NPCI.MulPCI.FC.minSepTime;
					if(multipleFramesTx.minSepTime > MIN_SEP_TIME )
                        multipleFramesTx.minSepTime = MIN_SEP_TIME;
					if( multipleFramesTx.minSepTime >= 0xF1 && multipleFramesTx.minSepTime <= 0xF9 )
						multipleFramesTx.minSepTime = 1; //diversion from 15765-2-it should be 100 us - 900 us as per 15765-2
						
				}
				
			break;
			
			
		    
		}
		diagnosticsBuf[RxQuePop].bDataRdy = FALSE;
	}

 }
 

}


/*********************************************************************//**
 *
 * copy signle fram data to muti fram bruffer
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
static void copySingleFrameDataToMultiFrame(void)
{
	u8 i=0;
	
	//first byte is left blank because it contains frame type + top 4 bytes for numBytes which is not required here
	multipleFramesRx.mulByteDiagRespData[0] = 0;
	//filter out the frame type and keep the numbytes
	multipleFramesRx.mulByteDiagRespData[1] =  singleFrameRx.dataBytes[i] & 0x0f;
	for(i=1;i<8;i++)
		multipleFramesRx.mulByteDiagRespData[i+1] = singleFrameRx.dataBytes[i];
	multipleFramesRx.ID = singleFrameRx.ID;
	multipleFramesRx.numBytes = singleFrameRx.numBytes;
	multipleFramesRx.timeStamp = singleFrameRx.timeStamp;
	multipleFramesRx.dataReady = TRUE;
	

}


/*********************************************************************//**
 *
 * 
 *
 * @param	None 
 *
 * @return	None
 *************************************************************************/
void HSCANBusOffRecovery(void)
{
/*
	if(errorFlags[eDiagBusOff - eDiscreteSwitchFault] == TRUE && (u32GetClockTicks() - HSCANBusOffRecoveryClk) > BusOffRecoveryTimeout )
	{
	//	HSCAN_BusOffRecoveryRequest();
		errorFlags[eDiagBusOff - eDiscreteSwitchFault]  = FALSE;
		//save Diag Bus off DTC here
	}
	*/
	
}


/* END P15765 */
