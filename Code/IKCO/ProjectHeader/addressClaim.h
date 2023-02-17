/*****************************************************************************
**  Module: addressClaim.h
**
** ---------------------------------------------------------------------------
**  Description:
**      Header file for addressClaim.c
**
*****************************************************************************/
#ifndef ADDRESSCLAIM_H_
#define ADDRESSCLAIM_H_

#include "r_cg_macrodriver.h"

#include "J1939.h"

#define PREFERRED_SA          	 0x17// 0xc8            // J1939 TABLE B2..B9: Instrument cluster
// public interface ---------------------------------------------------------

// Global prototypes
void vAddressClaimStateMgr(void);
void vRxISOaddressClaim(sJ1939_RX_MSG_TYPE *);
bool bIsAddressClaimed(void);
void vRxISOrequest(sJ1939_RX_MSG_TYPE *);
u8  ucGetMyClaimedAddress(void);
bool bHasMyClaimedAddressedBeenBumped(void);
void vClearAddressClaimBumped(void);

#endif
