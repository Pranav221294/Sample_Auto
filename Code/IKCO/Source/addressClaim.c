/*****************************************************************************
**  Module: addressClaim.c
**
** ---------------------------------------------------------------------------
**  Description:
**      Handles CAN address claim and all ISO required protocols.
**
*****************************************************************************/
#include "r_cg_macrodriver.h"
#include "addressClaim.h"
#include "customTypedef.h"
#include "Can.h"
#include "r_cg_userdefine.h"

// constant declarations -----------------------------------------------------


#define NMEA2000    TRUE

#ifdef NMEA2000
    #define  J1939_ADDRESS_CLAIM_MAX_ADDRESS    252    // maximum valid address is 251
#else
    #define  J1939_ADDRESS_CLAIM_MAX_ADDRESS    254
#endif

#define  J1939_ADDRESS_CLAIM_NULL_ADDRESS   254

// My application NAME fields
#define  MY_ARBITRARY_ADDRESS_CAPABILITY    FALSE           // Can use arbitrary SA to resolve a conflict
#define  MY_INDUSTRY_GROUP                  (u8)1         	// J1939 TABLE B1: 1 = On-Highway Equipment
#define  MY_SYSTEM_INSTANCE                 (u8)0         	// First instance
#define  MY_SYSTEM                          (u8)80        	// J1939 TABLE B12: Instrumentation General System = 80
#define  MY_RESERVED_FIELD                  (u8)0         	// Reserved by SAE, should be set to 0
#define  MY_FUNCTION                        (u8)19       	// J1939 Table B11, Instrument Cluster = 19
#define  MY_FUNCTION_INSTANCE               (u8)0         	//
#define  MY_ECU_INSTANCE                    (u8)0         	// First Instance J1939 Table B11,Instrument Cluster = 19
#define  MY_MANUFACTURER_CODE               (u16)466       	// Veethree (11 bit max)
#define  MY_IDENTITY_NUMBER                 (u32)0x1FFFFF 	// 21 bits

// Macros
#define isDelayExpired    (u16GetClockTicks() >= uiDelayTimer)


typedef enum
// ISO 11783 (J1939-81)
{
    eAC_INIT = 0,
    eAC_S0_IDLE,
    eAC_T0_IDLE,
    eAC_S1_RANDOM_DELAY,
    eAC_T1_RANDOM_DELAY,
    eAC_S2_TRANSMIT,
    eAC_T2_TRANSMIT,
    eAC_S3_RXCANFRAME,
    eAC_T3_RXCANFRAME,
    eAC_S4_FETCH_NEXT_myAddress,
    eAC_T4_FETCH_NEXT_myAddress,
    eAC_S5_RXCANFRAME,
    eAC_T5_RXCANFRAME,
    eAC_S6_RANDOM_DELAY,
    eAC_T6_RANDOM_DELAY,
    eAC_S7_TRANSMIT,
    eAC_T7_TRANSMIT,
    eAC_S8_RXCANFRAME,
    eAC_T8_RXCANFRAME,
    eAC_S9_TRANSMIT,
    eAC_T9_TRANSMIT
} eAdrClaimStateType;

typedef struct
{
    eAdrClaimStateType  state;
    u16    idleState           :1;
    u16    startRandomDelay    :1;
    u16    start250MsDelay     :1;
    u16    delayComplete       :1;
    u16    selectStartAddress  :1;
    u16    fetchNextMyAddress  :1;
    u16    commandedAddress    :1;
    u16    addressClaimed      :1;
    u16    cannotClaimAddress  :1;
    u16    iWin                :1;
    u16    iLose               :1;
    u16    canMsgSent          :1;
    u16    noMessage           :1;
    u16    requestPgn          :1;
    u16    pendingRequestPgn   :1;
    u16    adrClaimContention  :1;
    u16    uiAdrClaimCounter;
    bool    bIwon;
    bool    bBumpedByHigher;
} sAddressClaimStateType;


typedef struct
{
    u8  IdentityNumber_u81;             // lsb of identity number
    u8  IdentityNumber_u82;             // second u8 of identity number
    struct
    {
        u16  IdentityNumber_u83     :5;  // 4:0 most significant 5 bits of identity number
        u16  ManufacturerCode_u83   :3;  // 7:5 least significant 2 bits of manufacturing code
    } u83;
    u8  ManufacturerCode_u84;           // msb of manufacturing code
    struct
    {
        u16  EcuInstance              :3;  // 2:0 ECU instance
        u16  FunctionInstance         :5;  // 7:3 function instance
    } u85;
    u8  DeviceFunction;                   // function
    struct
    {
        u16  Reserved                 :1;  // 0 reserved
        u16  VehicleSystem            :7;  // 7:1 industry group
    } u87;
    struct
    {
        u16  VehicleSystemInstance    :4;  // 3:0 vehicle system instance
        u16  IndustryGroup            :3;  // 6:4 industry group
        u16  ArbitraryAddressCapable  :1;  // 7 arbitrary address capable
    } u88;
} sJ1939_NAME_FIELDS_TYPE;


typedef struct j1939_Request_Message_Struct
{
    u8  PduSpecific;
    u8  PduFormat;
    struct
    {
        u8  DataPage         :1;
        u8  Reserved         :1;
        u8  UnusedMSB        :6;
    } sBitField;
} sJ1939_Request_Message_Struct_Type;


// local declarations --------------------------------------------------------

enum J1939_ACKNOWLEDGMENT_STATE
{
    ePOSITIVE_ACKNOWLEDGMENT = 0,
    eNEGATIVE_ACKNOWLEDGMENT,
    ePGN_SUPPORTED_ACCESS_DENIED,
    ePGN_SUPPORTED_CANNOT_RESPOND
};


// Global vars
u8 ucMyAddress;
sJ1939_NAME_FIELDS_TYPE sMyName;
sAddressClaimStateType sAddressClaim =
{ eAC_INIT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Private vars
volatile static u8  bAnswered;
static u16 uiDelayTimer;


// private prototypes --------------------------------------------------------

static void  _vAddressClaimInitDelay(void);
static void  _vAddressClaimInit(void);
static bool  _bTxISOaddressClaim(u8 DA, u8 SA);
static bool  _bTxISOacknowledge(u8 DA, u8 SA, u32 PGN, u8 groupFnct, u8 status);
static u8    _ucGetRandomDelayTime(void);
static void  _vS0AddressClaimIdle(void);
static void  _vT0AddressClaimIdle(void);
static void  _vS1AddressClaimRandomDelay(void);
static void  _vT1AddressClaimRandomDelay(void);
static void  _vS2AddressClaimTransmit(void);
static void  _vT2AddressClaimTransmit(void);
static void  _vS3AddressClaimRxCanFrame(void);
static void  _vT3AddressClaimRxCanFrame(void);
static void  _vS4AddressClaimFetchNextmyAddress(void);
static void  _vT4AddressClaimFetchNextmyAddress(void);
static void  _vS5AddressClaimRxCanFrame(void);
static void  _vT5AddressClaimRxCanFrame(void);
static void  _vS6AddressClaimRandomDelay(void);
static void  _vT6AddressClaimRandomDelay(void);
static void  _vS7AddressClaimTransmit(void);
static void  _vT7AddressClaimTransmit(void);
static void  _vS8AddressClaimRxCanFrame(void);
static void  _vT8AddressClaimRxCanFrame(void);
static void  _vS9AddressClaimTransmit(void);
static void  _vT9AddressClaimTransmit(void);



/*****************************************************************************
**  Function name:  vAddressClaimStateMgr
**
**  Description:    Address claim state machine
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
void vAddressClaimStateMgr(void)
{
    switch (sAddressClaim.state)
    {
        case eAC_INIT :                         _vAddressClaimInit();                 break;
        case eAC_S0_IDLE :                      _vS0AddressClaimIdle();               break;
        case eAC_T0_IDLE :                      _vT0AddressClaimIdle();               break;
        case eAC_S1_RANDOM_DELAY :              _vS1AddressClaimRandomDelay();        break;
        case eAC_T1_RANDOM_DELAY :              _vT1AddressClaimRandomDelay();        break;
        case eAC_S2_TRANSMIT :                  _vS2AddressClaimTransmit();           break;
        case eAC_T2_TRANSMIT :                  _vT2AddressClaimTransmit();           break;
        case eAC_S3_RXCANFRAME :                _vS3AddressClaimRxCanFrame();         break;
        case eAC_T3_RXCANFRAME :                _vT3AddressClaimRxCanFrame();         break;
        case eAC_S4_FETCH_NEXT_myAddress :      _vS4AddressClaimFetchNextmyAddress(); break;
        case eAC_T4_FETCH_NEXT_myAddress :      _vT4AddressClaimFetchNextmyAddress(); break;
        case eAC_S5_RXCANFRAME :                _vS5AddressClaimRxCanFrame();         break;
        case eAC_T5_RXCANFRAME :                _vT5AddressClaimRxCanFrame();         break;
        case eAC_S6_RANDOM_DELAY :              _vS6AddressClaimRandomDelay();        break;
        case eAC_T6_RANDOM_DELAY :              _vT6AddressClaimRandomDelay();        break;
        case eAC_S7_TRANSMIT :                  _vS7AddressClaimTransmit();           break;
        case eAC_T7_TRANSMIT :                  _vT7AddressClaimTransmit();           break;
        case eAC_S8_RXCANFRAME :                _vS8AddressClaimRxCanFrame();         break;
        case eAC_T8_RXCANFRAME :                _vT8AddressClaimRxCanFrame();         break;
        case eAC_S9_TRANSMIT :                  _vS9AddressClaimTransmit();           break;
        case eAC_T9_TRANSMIT :                  _vT9AddressClaimTransmit();           break;
        default :
            sAddressClaim.state = eAC_INIT;
        break;
   }
}



/*****************************************************************************
**  Function name:  _vAddressClaimInit
**
**  Description:    Initialize address claim process.
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void _vAddressClaimInit(void)
{
    // Initialize the name fields
    sMyName.u88.ArbitraryAddressCapable = MY_ARBITRARY_ADDRESS_CAPABILITY;
    sMyName.u88.IndustryGroup           = MY_INDUSTRY_GROUP;
    sMyName.u88.VehicleSystemInstance   = MY_SYSTEM_INSTANCE;
    sMyName.u87.VehicleSystem           = MY_SYSTEM;
    sMyName.u87.Reserved                = 0;
    sMyName.DeviceFunction                = MY_FUNCTION;
    sMyName.u85.FunctionInstance        = MY_FUNCTION_INSTANCE;
    sMyName.u85.EcuInstance             = MY_ECU_INSTANCE;
    sMyName.ManufacturerCode_u84        = (u8)((MY_MANUFACTURER_CODE >> 3) & 0xFF);
    sMyName.u83.ManufacturerCode_u83  = (u8)(MY_MANUFACTURER_CODE & 0x07);
    sMyName.u83.IdentityNumber_u83    = (u8)((MY_IDENTITY_NUMBER >> 16) & 0x1F);
    sMyName.IdentityNumber_u82          = (u8)((MY_IDENTITY_NUMBER >> 8) & 0xFF);
    sMyName.IdentityNumber_u81          = (u8)(MY_IDENTITY_NUMBER & 0xFF);

    ucMyAddress = PREFERRED_SA;
    bAnswered  = J1939_ADDRESS_CLAIM_NULL_ADDRESS;
    sAddressClaim.uiAdrClaimCounter = 0;
    sAddressClaim.bBumpedByHigher = FALSE;

    if (ucMyAddress != J1939_ADDRESS_CLAIM_NULL_ADDRESS)
        sAddressClaim.state = eAC_S0_IDLE;
    else
        sAddressClaim.state = eAC_S6_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  vRxISOaddressClaim
**
**  Description:    Receives an address claim message
**
**  Parameters:     pointer to message
**
**  Return value:   TRUE if successful, FALSE otherwise
**
*****************************************************************************/
void vRxISOaddressClaim(sJ1939_RX_MSG_TYPE *msg)
{
    sJ1939_Id_Struct_Type *acPGN = (sJ1939_Id_Struct_Type *)&msg->sMsgID;
    u8 *myNamePtr =0;
    u8 *RxNamePtr =0;
    u8  nameCount =0;

    // Reset reception flag
    sAddressClaim.noMessage = FALSE;

    // Address claim contention
    if (acPGN->sIdField.SourceAddress == ucMyAddress)
    {
        nameCount = sizeof(sJ1939_NAME_FIELDS_TYPE);
        RxNamePtr = (u8 *)&msg->u8data[0] + nameCount;
        myNamePtr = (u8 *)&sMyName + nameCount;
        sAddressClaim.adrClaimContention = TRUE;
    }

    // Name field challenge
    do
    {
        RxNamePtr--;
        myNamePtr--;

        if (*myNamePtr != *RxNamePtr)
        {
            // Lowest value wins challenge
            if (*myNamePtr < *RxNamePtr)
            {
                sAddressClaim.iWin  = TRUE;
                sAddressClaim.iLose = FALSE;
                sAddressClaim.bIwon = TRUE;
                sAddressClaim.bBumpedByHigher = FALSE;
            }
            else
            {
                sAddressClaim.iWin  = FALSE;
                sAddressClaim.iLose = TRUE;
                sAddressClaim.bIwon = FALSE;
                sAddressClaim.bBumpedByHigher = TRUE;
            }
            if ((ucMyAddress != J1939_ADDRESS_CLAIM_NULL_ADDRESS) || (sAddressClaim.cannotClaimAddress))
            {
                // Redo the address claim - function will select the appropriate address to claim
                sAddressClaim.state = eAC_S3_RXCANFRAME;
            }
        }
    } while (nameCount-- && (*myNamePtr == *RxNamePtr));
}



/*****************************************************************************
**  Function name:  vRxISOrequest
**
**  Description:    Receives an ISO request. PGN requested is contained in the
**                  data part of the message.
**
**  Parameters:     pointer to message
**
**  Return value:   void
**
*****************************************************************************/
void vRxISOrequest(sJ1939_RX_MSG_TYPE *msg)
{
    // Set pointer to data part of message
    sJ1939_Request_Message_Struct_Type *PgnRequest = (sJ1939_Request_Message_Struct_Type *)(&msg->u8data[0]);

    union
    {
        sJ1939_Id_Struct_Type  ID;
        u32                 PGN;
    } j1939;

    // Get J1939 specific network device address who sent the message to be answered
    j1939.ID.Identifier = msg->sMsgID.Identifier;
    bAnswered = j1939.ID.sIdField.SourceAddress;

    // Extract requested 24-bit PGN number from J1939 single frame data field
    j1939.ID.sIdField.PduSpecific        = PgnRequest->PduSpecific;
    j1939.ID.sIdField.PduFormat          = PgnRequest->PduFormat;
    j1939.ID.sIdField.sBitField.DataPage  = PgnRequest->sBitField.DataPage;
    j1939.ID.sIdField.sBitField.Reserved  = PgnRequest->sBitField.Reserved;
    j1939.ID.sIdField.sBitField.Priority  = 0;
    j1939.ID.sIdField.sBitField.UnusedMSB = 0;
    j1939.ID.Identifier >>= 8;

    if (sAddressClaim.iLose == FALSE)
    {
        switch (j1939.PGN)
        {
            case PGN60928 : // Address claim request
                sAddressClaim.noMessage = FALSE;
                sAddressClaim.requestPgn = TRUE;
                sAddressClaim.pendingRequestPgn = TRUE;
            break;

            case PGN59904 : // Request
            case PGN59392 : // Acknowledgement
                _bTxISOacknowledge(bAnswered, ucMyAddress, j1939.PGN, 0, ePGN_SUPPORTED_ACCESS_DENIED);
            break;

            default :
                // If this PGN is not supported, send a negative acknowledgement
                if (!bProcessISORequest(j1939.PGN))
                    _bTxISOacknowledge(bAnswered, ucMyAddress, j1939.PGN, 255, eNEGATIVE_ACKNOWLEDGMENT);

            break;
        }
    }
    else if (j1939.PGN == PGN60928)
    {
        // J1939 request for PGN address claim message
        sAddressClaim.noMessage  = FALSE;
        sAddressClaim.requestPgn = TRUE;
    }
}



/*****************************************************************************
**  Function name:  _bTxISOacknowledge
**
**  Description:    Sends an ISO acknowledge message
**
**  Parameters:     destination address,
**                  source address,
**                  PGN
**
**  Return value:   TRUE if successful, FALSE otherwise
**
*****************************************************************************/
static bool _bTxISOacknowledge(u8 DA, u8 SA, u32 PGN, u8 groupFnct, u8 status)
{
    u8 msgData[8];
    sJ1939_Id_Struct_Type ID;

    // PGN to be acknowledged
    ID.Identifier = (PGN << 8) & 0x03FFFF00;

    // Create the data
    msgData[0] = status;
    msgData[1] = groupFnct;
    msgData[2] = 0xFF;
    msgData[3] = 0xFF;
    msgData[4] = 0xFF;
    msgData[5] = ID.sIdField.PduSpecific;
    msgData[6] = ID.sIdField.PduFormat;
    msgData[7] = (u8)ID.sIdField.sBitField.DataPage;

    // Make a Tx ID
    ID.Identifier = PGN59392 << 8;
    ID.sIdField.sBitField.Priority = 6;
    ID.sIdField.sBitField.Reserved = 0;
    ID.sIdField.PduSpecific = DA;
    ID.sIdField.SourceAddress = SA;

    // Transmit the message
  	 vTxMsgBufProcessing(ID.Identifier ,8,msgData,J1939_TX); 		// Not provide any return error for successful transmission
	return;		 
}



/*****************************************************************************
**  Function name:  _bTxISOaddressClaim
**
**  Description:    Sends an address claim message
**
**  Parameters:     destination address,
**                  source address
**
**  Return value:   TRUE if successful, FALSE otherwise
**
*****************************************************************************/
static bool _bTxISOaddressClaim(u8 DA, u8 SA)
{
    sJ1939_Id_Struct_Type acPGN;

    // Assemble the address claim 29 bit ID
    acPGN.Identifier                = PGN60928 << 8;
    acPGN.sIdField.sBitField.Priority = 6;
    acPGN.sIdField.sBitField.Reserved = 0;
    acPGN.sIdField.PduSpecific       = DA;
    acPGN.sIdField.SourceAddress     = SA;

    sAddressClaim.noMessage = FALSE;
    sAddressClaim.requestPgn = TRUE;

    // Transmit the messasge
	 vTxMsgBufProcessing(acPGN.Identifier ,8,(u8 *)&sMyName,J1939_TX); 
    return;
  
}



/*****************************************************************************
**  Function name:  _ucGetRandomDelayTime
**
**  Description:    Generates an 8 bit random number between 0 and 153 based
**                  on the contents of sMyName to be used as the random timer
**                  for the address claim proceedure.
**                  Note that the time delay of 0-153 msc is set by the SAE
**                  J1939 specification for address claim (J1939-81).
**
**  Parameters:     none
**
**  Return value:   8 bit random number between 0 and 153
**
*****************************************************************************/
static u8 _ucGetRandomDelayTime(void)
{
    u8 delay;

    // Seed the random time generator
    srand(u16GetClockTicks());

    delay = (u8)(rand() % 154);

    return (u8)delay;
}


/*****************************************************************************
**  Function name:  _vS0AddressClaimIdle
**
**  Description:    Initialize the address claim structure
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS0AddressClaimIdle(void)
{
    // Address Claim Procedures
    sAddressClaim.idleState          = FALSE;
    sAddressClaim.startRandomDelay   = FALSE;
    sAddressClaim.start250MsDelay    = FALSE;
    sAddressClaim.delayComplete      = FALSE;
    sAddressClaim.selectStartAddress = FALSE;
    sAddressClaim.commandedAddress   = FALSE;
    sAddressClaim.fetchNextMyAddress = FALSE;
    sAddressClaim.addressClaimed     = FALSE;
    sAddressClaim.cannotClaimAddress = FALSE;
    sAddressClaim.iWin               = FALSE;
    sAddressClaim.iLose              = FALSE;
    sAddressClaim.pendingRequestPgn  = FALSE;
    sAddressClaim.adrClaimContention = FALSE;
    sAddressClaim.canMsgSent         = FALSE;
    sAddressClaim.noMessage          = FALSE;
    sAddressClaim.requestPgn         = FALSE;

    // Set the next state
    sAddressClaim.state = eAC_T0_IDLE;
}


/*****************************************************************************
**  Function name:  _vT0AddressClaimIdle
**
**  Description:    Wait for the idle state indicator to go FALSE.
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT0AddressClaimIdle(void)
{
    if (sAddressClaim.idleState == FALSE)
        sAddressClaim.state = eAC_S1_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  _vS1AddressClaimRandomDelay
**
**  Description:    Set a random delay
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS1AddressClaimRandomDelay(void)
{
    // Set parameters
    sAddressClaim.selectStartAddress = TRUE;
    sAddressClaim.delayComplete      = FALSE;
    sAddressClaim.startRandomDelay   = TRUE;

    // Set delay timer
    uiDelayTimer = (u16)(u32GetClockTicks() + _ucGetRandomDelayTime());

    // Set next state
    sAddressClaim.state = eAC_T1_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  _vT1AddressClaimRandomDelay
**
**  Description:    Wait for the random delay time to expire.
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT1AddressClaimRandomDelay(void)
{
    // Check if delay time expired
    if (isDelayExpired)
    {
        // Set flag
        sAddressClaim.delayComplete = TRUE;

        // Set the next state
        sAddressClaim.state = eAC_S2_TRANSMIT;
    }
}


/*****************************************************************************
**  Function name:
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS2AddressClaimTransmit(void)
{
    // Set parameters
    sAddressClaim.startRandomDelay   = FALSE;
    sAddressClaim.start250MsDelay    = FALSE;
    sAddressClaim.addressClaimed     = FALSE;
    sAddressClaim.cannotClaimAddress = FALSE;

    // Transmit address claim message to all network devices
    sAddressClaim.canMsgSent = _bTxISOaddressClaim(J1939_GLOBAL_ADDRESS, ucMyAddress);

    // Set the next state
    sAddressClaim.state = eAC_T2_TRANSMIT;
}


/*****************************************************************************
**  Function name:  _vT2AddressClaimTransmit
**
**  Description:    Wait for address claim message to be sent
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT2AddressClaimTransmit(void)
{
    if (sAddressClaim.canMsgSent)
        sAddressClaim.state = eAC_S3_RXCANFRAME;
    else
        // Random delay and try again
        sAddressClaim.state = eAC_S1_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS3AddressClaimRxCanFrame(void)
{
    // Set parameters
    sAddressClaim.iWin      = FALSE;
    sAddressClaim.iLose     = FALSE;
    sAddressClaim.noMessage = FALSE;

    if (!sAddressClaim.start250MsDelay)
    {
        sAddressClaim.delayComplete = FALSE;
        sAddressClaim.start250MsDelay = TRUE;
        uiDelayTimer = (u16)(u32GetClockTicks() + 250);
    }

    // Set the next state
    sAddressClaim.state = eAC_T3_RXCANFRAME;
}


/*****************************************************************************
**  Function name:  _vT3AddressClaimRxCanFrame
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT3AddressClaimRxCanFrame(void)
{
    // Check the 250ms timer
    if (isDelayExpired)
        sAddressClaim.delayComplete = TRUE;

    // Determine next state
    if (sAddressClaim.iWin || sAddressClaim.bIwon)
    {
        sAddressClaim.state = eAC_S2_TRANSMIT;
        sAddressClaim.bIwon = FALSE;
    }
    else if (sAddressClaim.iLose)
        sAddressClaim.state = eAC_S4_FETCH_NEXT_myAddress;
    else if (sAddressClaim.delayComplete)
        sAddressClaim.state = eAC_S5_RXCANFRAME;
    else if (sAddressClaim.adrClaimContention)
        sAddressClaim.state = eAC_S4_FETCH_NEXT_myAddress;
    else
        sAddressClaim.state = eAC_T3_RXCANFRAME;
}


/*****************************************************************************
**  Function name:  _vS4AddressClaimFetchNextmyAddress
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS4AddressClaimFetchNextmyAddress(void)
{
    // Create a random time to use for same address claim contention
    u8 contentionRandomTime;
    srand(u16GetClockTicks());
    contentionRandomTime = (u8)(rand() % 0x20);

    // Reset the delay flag
    sAddressClaim.start250MsDelay = FALSE;

    // Two or more devices are trying to claim the same address,
    // randomly try the same address again.
    if (!((sAddressClaim.adrClaimContention) && ((u32GetClockTicks() & contentionRandomTime) == contentionRandomTime)))
    {
        // Get the next source address to be claimed except the reserved address
        if (++ucMyAddress >= J1939_ADDRESS_CLAIM_MAX_ADDRESS)
            ucMyAddress = 128;
        if ((ucMyAddress + sAddressClaim.uiAdrClaimCounter) != 0)
            ++sAddressClaim.uiAdrClaimCounter;
    }
    // This must be outside of the loop above
    sAddressClaim.adrClaimContention = FALSE;

    if (sAddressClaim.uiAdrClaimCounter >= J1939_ADDRESS_CLAIM_MAX_ADDRESS)
    {
        sAddressClaim.fetchNextMyAddress = FALSE;
        sAddressClaim.uiAdrClaimCounter = 0;
    }
    else
        sAddressClaim.fetchNextMyAddress = TRUE;

    // Set the next state
    sAddressClaim.state = eAC_T4_FETCH_NEXT_myAddress;
}


/*****************************************************************************
**  Function name:  _vT4AddressClaimFetchNextmyAddress
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT4AddressClaimFetchNextmyAddress(void)
{
    // Determine the next state
    if (sAddressClaim.fetchNextMyAddress)
        sAddressClaim.state = eAC_S2_TRANSMIT;
    else
        sAddressClaim.state = eAC_S6_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  _vS5AddressClaimRxCanFrame
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS5AddressClaimRxCanFrame(void)
{
    // Set parameters
    sAddressClaim.iWin            = FALSE;
    sAddressClaim.iLose           = FALSE;
    sAddressClaim.noMessage       = FALSE;
    sAddressClaim.requestPgn      = FALSE;
    sAddressClaim.start250MsDelay = FALSE;
    sAddressClaim.addressClaimed  = TRUE;
    sAddressClaim.uiAdrClaimCounter = 0;

    // Set the next state
    sAddressClaim.state = eAC_T5_RXCANFRAME;
}


/*****************************************************************************
**  Function name:  _vT5AddressClaimRxCanFrame
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT5AddressClaimRxCanFrame(void)
{
    // Determine next state
    if (sAddressClaim.requestPgn || sAddressClaim.iWin || sAddressClaim.pendingRequestPgn)
    {
        sAddressClaim.pendingRequestPgn = FALSE;
        sAddressClaim.state = eAC_S9_TRANSMIT;
    }
    else if (sAddressClaim.iLose)
        sAddressClaim.state = eAC_S4_FETCH_NEXT_myAddress;
}


/*****************************************************************************
**  Function name:  _vS6AddressClaimRandomDelay
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS6AddressClaimRandomDelay(void)
{
    // Set parameters
    sAddressClaim.delayComplete = FALSE;
    sAddressClaim.startRandomDelay = TRUE;
    uiDelayTimer = (u16)(u32GetClockTicks() + _ucGetRandomDelayTime());

    // Set the next state
    sAddressClaim.state = eAC_T6_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  _vT6AddressClaimRandomDelay
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT6AddressClaimRandomDelay(void)
{
    // Check the delay timer
    if (isDelayExpired)
    {
        sAddressClaim.delayComplete = TRUE;
        sAddressClaim.state = eAC_S7_TRANSMIT;
    }
}


/*****************************************************************************
**  Function name:  _vS7AddressClaimTransmit
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS7AddressClaimTransmit(void)
{
    // Set parameters
    sAddressClaim.startRandomDelay = FALSE;

    if (sAddressClaim.cannotClaimAddress)
    {
        // Try a valid address again
        ucMyAddress = PREFERRED_SA;
    }
    else
    {
        // Set to null address
        ucMyAddress = J1939_ADDRESS_CLAIM_NULL_ADDRESS;
        sAddressClaim.uiAdrClaimCounter = 0;
    }

    // Transmit to all network devices
    sAddressClaim.canMsgSent = _bTxISOaddressClaim(J1939_GLOBAL_ADDRESS, ucMyAddress);

    // Set the next state
    sAddressClaim.state = eAC_T7_TRANSMIT;
}


/*****************************************************************************
**  Function name:  _vT7AddressClaimTransmit
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT7AddressClaimTransmit(void)
{
    if (sAddressClaim.canMsgSent)
        sAddressClaim.state = eAC_S8_RXCANFRAME;
    else
        // Random delay and try again
        sAddressClaim.state = eAC_S6_RANDOM_DELAY;
}


/*****************************************************************************
**  Function name:  _vS8AddressClaimRxCanFrame
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS8AddressClaimRxCanFrame(void)
{
    // Set parameters
    sAddressClaim.requestPgn = FALSE;
    sAddressClaim.cannotClaimAddress = TRUE;

    // Set the next state
    sAddressClaim.state = eAC_T8_RXCANFRAME;
}


/*****************************************************************************
**  Function name:  _vT8AddressClaimRxCanFrame
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT8AddressClaimRxCanFrame(void)
{
    if (sAddressClaim.requestPgn)
    {
        if (sAddressClaim.cannotClaimAddress)
            sAddressClaim.state = eAC_S3_RXCANFRAME;
        else
            sAddressClaim.state = eAC_S6_RANDOM_DELAY;
    }
}


/*****************************************************************************
**  Function name:  _vS9AddressClaimTransmit
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vS9AddressClaimTransmit(void)
{
    // Transmit to all network devices
    sAddressClaim.canMsgSent = _bTxISOaddressClaim(J1939_GLOBAL_ADDRESS, ucMyAddress);

    // Set the next state
    sAddressClaim.state = eAC_T9_TRANSMIT;
}


/*****************************************************************************
**  Function name:  _vT9AddressClaimTransmit
**
**  Description:
**
**  Parameters:     void
**
**  Return value:   void
**
*****************************************************************************/
static void  _vT9AddressClaimTransmit(void)
{
    if (sAddressClaim.canMsgSent)
        sAddressClaim.state = eAC_S5_RXCANFRAME;
    else
        // Random delay and try again
        sAddressClaim.state = eAC_S1_RANDOM_DELAY;
}



/*****************************************************************************
*****************************************************************************/
bool bIsAddressClaimed(void)
{
    return sAddressClaim.addressClaimed;
}


/*****************************************************************************
*****************************************************************************/
u8 ucGetMyClaimedAddress(void)
{
    return ucMyAddress;
}

/*****************************************************************************
*****************************************************************************/
bool bHasMyClaimedAddressedBeenBumped(void)
{
    return sAddressClaim.bBumpedByHigher;
}

/*****************************************************************************
*****************************************************************************/
void vClearAddressClaimBumped(void)
{
    sAddressClaim.bBumpedByHigher = FALSE;
}
