/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg_util.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains utility functions for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 10 05 2010 sh.lai
 * [ALPS00127207] [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * Fix CR ALPS00127207 : [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * [Cause] After respone of AT+CHLD? is sent the profile state is transfferred to connected and the SCO is allowed. But AP think the HFP is connected when the response of AT+CHLD? is sent to profile and request SCO immediatly. HFP will reject the SCO connection due to the state is still conencting.
 * [Solution] Change state into connected after the response of AT+CHLD? is sent to stack so that profile can accept the SCO connection request.
 *
 * 09 28 2010 sh.lai
 * [ALPS00221044] [HFP] Cannot connect HFP with Nokia BH-902
 * Fix CR ALPS00221044 : [HFP] Cannot connect HFP with Nokia BH-902.
 * [Cause] The CMGR handler was not cleared after calling CMGR_RemoveDataLink that cause stack think the HFP is still connected to another BT device after the HFP connection is removed.
 * [Solution] Call CMGR_RemoveCmgrWithDevice to clear CMGR handler after data link is removed.
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include "sys/hfgalloc.h"
#include "conmgr.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "hfg.h"

#define __BT_FILE__	BT_FILE_HFG_UTIL

#ifdef __BT_HFG_PROFILE__
extern U32 bt_hfg_get_supported_feature(void);
extern BOOL bt_hfg_check_supported_feature(U32 feature_flag);
extern U32 bt_hfg_get_supported_indicator(void);
#endif

/* Names of indicators */
const char *g_indicators[NUM_OF_HFG_IND][2] = 
{
    {"service",     "0,1"},      /* HFG_IND_SERVICE */
    {"call",          "0,1"},      /* HFG_IND_CALL */
    {"callsetup",  "0,3"},       /* HFG_IND_CALL_SETUP */
    {"callheld",    "0,2"},       /* HFG_IND_CALL_HELD */
    {"battchg",    "0,5"},       /* HFG_IND_BATTERY */
    {"signal",      "0,5"},       /* HFG_IND_SIGNAL */
    {"roam",       "0,1"},       /* HFG_IND_ROAMING */
};

/* Ranges of indicators */
/* min, max, default value */
const U8 g_indicatorRange[NUM_OF_HFG_IND][3] = 
{
    {0,1,1},      /* HFG_IND_SERVICE */
    {0,1,0},      /* HFG_IND_CALL */  
    {0,3,0},      /* HFG_IND_CALL_SETUP */
    {0,2,0},      /* HFG_IND_CALL_HELD */
    {0,5,2},      /* HFG_IND_BATTERY */
    {0,5,2},      /* HFG_IND_SIGNAL */
    {0,1,0},      /* HFG_IND_ROAMING */
};

const U32 g_DefaultIndicators = (1<<HFG_IND_SERVICE) | (1<<HFG_IND_CALL) | (1<<HFG_IND_CALL_SETUP) |
                                                (1<<HFG_IND_BATTERY) | (1<<HFG_IND_SIGNAL) | (1<<HFG_IND_ROAMING) |
                                                (1<<HFG_IND_CALL_HELD);

const HfgCHarsetType	g_SupportedCharset = HFG_CHARSET_UTF8;
const HfgCHarsetType	g_DefaultCharset = HFG_CHARSET_UTF8;

const char *g_ManufactureID = "MTK";
const char *g_ModelID = "MTK";

typedef struct _CharsetItem
{
	char 			*charset;
	HfgCHarsetType	charsetType;
}CharsetItem;

const CharsetItem g_Charsets[] = 
{	
	{"GSM", HFG_CHARSET_GSM},
	//"HEX",	/* HFG_CHARSET_HEX */
	//"IRA",	/* HFG_CHARSET_IRA */
	//"PCCP",	/* HFG_CHARSET_PCCP */
	//"PCDN",	/* HFG_CHARSET_PCDN */
	{"UCS2", HFG_CHARSET_UCS2},
	{"UTF-8", HFG_CHARSET_UTF8},
	//"8859",	/* HFG_CHARSET_8859n */
};


HfgResponse *HfgAllocResponse(void)
{
	HfgResponse *rsp = NULL;
    	if (!IsListEmpty(&HFG(freeRespList))) 
    	{
		rsp = (HfgResponse *)RemoveHeadList(&HFG(freeRespList)); 
		OS_MemSet((U8*)rsp, 0, sizeof(HfgResponse));
    	}
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_NO_FREE_RESPONSE);
	}
	return rsp;
}

void HfgFreeResponse(HfgResponse* rsp)
{
	if (!IsNodeOnList(&HFG(freeRespList), &rsp->node)) 
	{
		InsertTailList(&HFG(freeRespList), &rsp->node);
	}
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_INSERT_FREE_RESPONSE);
	}
}

const char* HfgGetCharsetDesc(HfgChannel *Channel, HfgCHarsetType charset)
{
	U8 i;
	U8 size = sizeof(g_Charsets)/sizeof(g_Charsets[0]);
	for(i = 0;i < size;i++)
	{
		if(g_Charsets[i].charsetType == charset)
			return g_Charsets[i].charset;
	}
	return NULL;
}

BtStatus HfgSetupCharsetTestRsp(HfgChannel *Channel)
{
	U8 count = 0;
#if 0
	U8 size = sizeof(g_Charsets)/sizeof(g_Charsets[0]);
	for(i = 0;i < size;i++)
	{	
		if(g_SupportedCharset & g_Charsets[i].charsetType)
		{
			Channel->atResults.p.charset_test.charset[count] = g_Charsets[i].charset;
			count++;
		}
	}
	Channel->atResults.p.charset_test.num = count;
#endif
	return (count > 0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}


BtStatus HfgDecodeCharset(HfgChannel *Channel, const char *charset, HfgCHarsetType *charsetType)
{
	U8 i;
	U8 size = sizeof(g_Charsets)/sizeof(g_Charsets[0]);
	
	for(i = 0;i < size;i++)
	{
		if( OS_StrCmp(g_Charsets[i].charset, charset) == 0 )
		{
			*charsetType = g_Charsets[i].charsetType;
			break;
		}
	}

	if( i >= size )
		return BT_STATUS_NOT_FOUND;
	else if ( !(g_SupportedCharset & (*charsetType)) )
		return BT_STATUS_NOT_SUPPORTED;
	else
		return BT_STATUS_SUCCESS;
}


const char* HfgGetModelID(HfgChannel *Channel)
{
	return g_ModelID;
}

const char* HfgGetManufactureID(HfgChannel *Channel)
{
	return g_ManufactureID;
}

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
void suspendOtherContexts(HfgChannel *Channel)
{
    HfgChannel *pChannel;
    pChannel = (HfgChannel*)GetHeadList(&Channel->node);
    bt_prompt_trace(MOD_BT, "[HFG] + suspendOtherContexts");
    while(pChannel != Channel)
    {
        if(&pChannel->node != &HFG(channelList))
        {
            bt_prompt_trace(MOD_BT, "[HFG] suspend context 0x%X", pChannel);
            Assert(HfgGetSubstate(pChannel) == HFG_SUBSTATE_CLOSED1);
            pChannel->bSuspended = KAL_TRUE;
            HfgDeregisterServerChannel(pChannel);
        }
        else
        {
            bt_prompt_trace(MOD_BT, "the context is &HFG(channelList)");
        }
        pChannel = (HfgChannel*)GetHeadList(&pChannel->node);
    }
    bt_prompt_trace(MOD_BT, "[HFG] + suspendOtherContexts");
}
void resumeOtherContexts(HfgChannel *Channel)
{
    HfgChannel *pChannel;
    pChannel = (HfgChannel*)GetHeadList(&Channel->node);
    bt_prompt_trace(MOD_BT, "[HFG] + resumeOtherContexts");
    while(pChannel != Channel)
    {
        if(&pChannel->node != &HFG(channelList))
        {
            bt_prompt_trace(MOD_BT, "[HFG] resume context 0x%X", pChannel);
            Assert(HfgGetSubstate(pChannel) == HFG_SUBSTATE_CLOSED1);
            Assert(pChannel->bSuspended == KAL_TRUE);
            pChannel->bSuspended = KAL_FALSE;
            Assert( BT_STATUS_SUCCESS == HfgRegisterServerChannel(pChannel) );
        }
        else
        {
            bt_prompt_trace(MOD_BT, "the context is &HFG(channelList)");
        }
        pChannel = (HfgChannel*)GetHeadList(&pChannel->node);
    }
    bt_prompt_trace(MOD_BT, "[HFG] - resumeOtherContexts");
}
#endif

BtStatus HfgChangeSubstate(HfgChannel *Channel, U8 newState)
{
    /* BtRemoteDevice *remDev; */
    U8 preState = HfgGetSubstate(Channel);
    BtStatus    status;

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANGE_STATE, HfgGetSubstate(Channel), newState);

    /* Pre-actions of changing substate */
    switch(newState)
    {
    case HFG_SUBSTATE_CLOSED1:
        /* FLOW : Enter */
        /* FLOW : Deregister server channel */
        HfgDeregisterServerChannel(Channel);
        /* FLOW : Register server channel */
        HfgRegisterServerChannel(Channel);
        /* FLOW : Disassociate ACL link */
        HfgCloseChannel(Channel);
        /* Clear channel state */
        Channel->flags = 0;
        Channel->queryFlags = 0;
        /* Set connection related flags to default value */
        Channel->version = 0;
        Channel->hfFeatures = 0;
	#ifdef __BT_HFG_PROFILE__
        Channel->nrecDisable = bt_hfg_check_supported_feature(HFG_FEATURE_ECHO_NOISE);
        Channel->ibRing = bt_hfg_check_supported_feature(HFG_FEATURE_RING_TONE);
	#else
        Channel->nrecDisable = FALSE;
        Channel->ibRing = TRUE;
	#endif
        Channel->indEnabled = FALSE;
        Channel->callId = FALSE;
        Channel->extendedErrors = FALSE;
        Channel->voiceRec = FALSE;
	 Channel->charset = g_DefaultCharset;
	 Channel->atParms.type = 0;	// Reset command type
	 Channel->atParms.bContinue = 0;	// Reset command type
	 /* Free tx/rx buffers */
	 if(Channel->atTxData)
 	 {
  	 	free_ctrl_buffer(Channel->atTxData);
		Channel->atTxData = NULL;
		Channel->atTxPacket.data = NULL;
 	 }
	 if(Channel->atRxBuffer)
 	 {
	 	free_ctrl_buffer(Channel->atRxBuffer);
		Channel->atRxBuffer = NULL;
		Channel->atRxBufDesc.buff = NULL;
 	 }
	 Channel->atRxBufDesc.readOffset = 0;
	 Channel->atRxBufDesc.writeOffset = 0;
	 Channel->atRxLen = 0;
#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(HfgGetSubstate(Channel) != HFG_SUBSTATE_CLOSED1)
        resumeOtherContexts(Channel);
#endif
        break;
    case HFG_SUBSTATE_CONN_OUT1:
    case HFG_SUBSTATE_CONN_OUT2:
        /* FLOW : Deregister server channel */
        //HfgDeregisterServerChannel(Channel);
        Channel->flags |= CHANNEL_FLAG_OUTGOING;
        break;
    case HFG_SUBSTATE_CONN_OUT3:
    	break;
    case HFG_SUBSTATE_CONN_IN1:
    	break;
    case HFG_SUBSTATE_CONN_IN2:
    	break;
    case HFG_SUBSTATE_DISC1:
    	break;
    case HFG_SUBSTATE_OPEN1:
    	break;
    case HFG_SUBSTATE_OPEN2:
    	break;
    case HFG_SUBSTATE_SLC1:
        /* SLC is created, so open SCO is allowed */
        DBG_PRINT_FLOW(("[HFG][FLOW] substate is S2 : %s", (preState == HFG_SUBSTATE_SLC2)?"Yes":"No"));
        if(preState != HFG_SUBSTATE_SLC2)
        {
            status = CMGR_RegisterScoHandler(&Channel->cmgrHandler);
            DBG_PRINT_FLOW(("[HFG][FLOW] Register SCO handler : %d", status));
        }
        break;
    case HFG_SUBSTATE_SLC2:
        break;
    default:
        return FALSE;
    }

    DBG_PRINT_FLOW( ("[HFG][FLOW]  Change state from 0x%X to 0x%X", HfgGetSubstate(Channel), newState) );
#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CLOSED1 && newState != HFG_SUBSTATE_CLOSED1)
        suspendOtherContexts(Channel);
#endif    
    /* Change to new substate */
    HfgGetSubstate(Channel) = newState;

    /* Post-actions of changing substate */
    switch(newState)
    {
    case HFG_SUBSTATE_CLOSED1:
        /* FLOW : Callback HFG_EVENT_SERVICE_DISCONNECTED */
        //remDev = CMGR_GetRemoteLink(&Channel->cmgrHandler);
	 //remDev = CMGR_GetRemoteDevice(&Channel->cmgrHandler);
        //if (remDev ==0)
        /* remDev = Channel->cmgrHandler.remDev; */
        HfgAppCallback(	Channel, 
                                 HFG_EVENT_SERVICE_DISCONNECTED,
                                 BT_STATUS_SUCCESS, 
                                 (U32)&Channel->bdAddr.addr/*remDev->bdAddr*/);
        //Channel->cmgrHandler.remDev = 0;
        //Channel->cmgrHandler.bdc = 0;
        /* FLOW : C1 Exit */
        break;
    case HFG_SUBSTATE_CONN_OUT1:
        break;
    case HFG_SUBSTATE_CONN_OUT2:
        break;
    case HFG_SUBSTATE_CONN_OUT3:
        break;
    case HFG_SUBSTATE_CONN_IN1:
        /* FLOW : Callback HFG_EVENT_AUTH_REQ */
        /* remDev = Channel->remDev; */
        HfgAppCallback(	Channel, 
        				HFG_EVENT_AUTH_REQ,
                           	BT_STATUS_SUCCESS, 
                           		(U32)&Channel->bdAddr.addr/*remDev->bdAddr*/);
        break;
    case HFG_SUBSTATE_CONN_IN2:
    	break;
    case HFG_SUBSTATE_DISC1:
    	break;
    case HFG_SUBSTATE_OPEN1:
	 /* Realloc memory for tx/rx buffer */
         if(Channel->atTxData == NULL)
         {
	 Channel->atTxData = (U8*)get_ctrl_buffer(HFG_TX_BUFFER_SIZE);
	 Channel->atBuffer.buff = Channel->atTxPacket.data = Channel->atTxData;
         }
         if(Channel->atRxBuffer == NULL)
         {
	 Channel->atRxBuffer = (U8*)get_ctrl_buffer(HFG_RECV_BUFFER_SIZE);
	 Channel->atRxBufDesc.buff = Channel->atRxBuffer;
         }
	 
        /* FLOW : Callback HFG_EVENT_RFCOMM_CONNECTED */
        /* Indicate the HFG currently negotiate with HF device */
        if(Channel->type == AG_TYPE_HF)
        {
            Channel->flags |= CHANNEL_FLAG_NEGOTIATE;
        }
        /* remDev = Channel->remDev; */
        HfgAppCallback(	Channel, 
        				HFG_EVENT_RFCOMM_CONNECTED,
                                 BT_STATUS_SUCCESS, 
                                 (U32)&Channel->bdAddr.addr /*remDev->bdAddr*/);
        break;
    case HFG_SUBSTATE_OPEN2:
    	break;
    case HFG_SUBSTATE_SLC1:
	 /* If connected profile is HSP, it will be directly set to SLC1 instead of OPEN1 */
	 if(Channel->atTxData == NULL)
 	 {
	 	Channel->atTxData = (U8*)get_ctrl_buffer(HFG_TX_BUFFER_SIZE);
	 	Channel->atBuffer.buff = Channel->atTxPacket.data = Channel->atTxData;
 	 }
	 if(Channel->atRxBuffer == NULL)
 	 {
	 	Channel->atRxBuffer = (U8*)get_ctrl_buffer(HFG_RECV_BUFFER_SIZE);
	 	Channel->atRxBufDesc.buff = Channel->atRxBuffer;
 	 }
        /* FLOW : Callback HFG_EVENT_SERVICE_CONNECTED */
        DBG_PRINT_FLOW(("[HFG][FLOW] Previous substate is S2 : %s", (preState == HFG_SUBSTATE_SLC2)?"Yes":"No"));
        if(preState != HFG_SUBSTATE_SLC2)
        {
            HfgReportServiceConnected(Channel);
        }
        break;
    case HFG_SUBSTATE_SLC2:
        break;
    default:
        return FALSE;
    }

    /*Report(("- HfgChangeSubstate: already changed to <%d>.", HfgGetSubstate(Channel)));*/
    return TRUE;
}

BtStatus HfgRegisterServerChannel(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL bReg = (BOOL)(Channel->rfChannel->userContext != NULL);

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_REGISTER_SERVER_CHANNEL, Channel);

    /* FLOW : Register server channel */
    DBG_PRINT_FLOW(("[HFG][FLOW] Server channel registered : %s", bReg?"Yes":"No"));
    if(bReg == FALSE)
    {
        status = RF_RegisterServerChannel(Channel->rfChannel, &HFG(hfgService)[Channel->type], 1);
        DBG_PRINT_FLOW(("[HFG][FLOW] Register server channel : %d", status));
        if(status != BT_STATUS_SUCCESS)
        {      
            	DBG_PRINT_ERROR(("[HFG][ERR] Register server channel failed : %d - (%d,%d)", status, __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
            Assert(0);
        }
        else	
        {      
            Channel->rfChannel->userContext = (void*)1;
        }
    }
    else
    {
		/* The RfChannel is already registered */
        DBG_PRINT_WARN(("[HFG][WARN] Server channel already registered - (%d,%d)", __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN,__BT_FILE__, __LINE__);
    }
    Report( ("[HFG][HfgRegisterServerChannel]  - return status==x%X", status) );
    return status;
}

BtStatus HfgDeregisterServerChannel(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL bReg = (BOOL)(Channel->rfChannel->userContext != NULL);
    
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DEREGISTER_SERVER_CHANNEL, Channel);
    
    DBG_PRINT_FLOW(("[HFG][FLOW] Server channel registered : %s", bReg?"Yes":"No"));
    /* FLOW : Deregister server channel */
	if(bReg)
    {
        status = RF_DeregisterServerChannel(Channel->rfChannel, &HFG(hfgService)[Channel->type]);
        DBG_PRINT_FLOW(("[HFG][FLOW] Deregister server channel : %d", status));
        if(status != BT_STATUS_SUCCESS)
        {
            /* It might be failed due to authentication is ongoing */
            DBG_PRINT_WARN(("[HFG][WARN] HfgDeregisterServerChannel failed. status=%d - (%d,%d)", status, __BT_FILE__, __LINE__));
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
        }
        else
        {
            Channel->rfChannel->userContext = NULL;
        }	
    }	
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN,__BT_FILE__, __LINE__);
	}
    Report( ("[HFG][HfgDeregisterServerChannel]  - return status==0x%X", status) );
    return status;
}

BtStatus HfgHfgFlushRespQueue(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U16 count = 0;
    
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_FLUSH_RESPONSE, Channel);
    while (!IsListEmpty(&Channel->rspQueue)) 
    {
    	HfgReportPacketCompleted(Channel, BT_STATUS_NO_CONNECTION);
	#if 0
        atResults = (AtResults*)RemoveHeadList(&Channel->rspQueue);
        if(atResults != &Channel->atResults)
        {
            /* Results sent from application */
            HfgAppCallback(Channel, HFG_EVENT_RESPONSE_COMPLETE, 
                                   BT_STATUS_NO_CONNECTION, (U32)atResults);
            callback++;
        } 
	#endif		
        count++;
    }
    DBG_PRINT_FLOW(("[HFG][FLOW] Flush response queue : flush %d entry", count));
    return status;
}


BtStatus HfgDisconnecting(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_PENDING;
    U16 substate = HfgGetSubstate(Channel);

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING, Channel, HfgGetSubstate(Channel));
    DBG_PRINT_FLOW(("[HFG][FLOW] Disconnecting : substate=0x%X", HfgGetSubstate(Channel)));

	/* Change state to disconnecting first */
	if(HfgGetSubstate(Channel) != HFG_SUBSTATE_DISC1)
	{
		DBG_PRINT_FLOW(("[HFG][FLOW] Substate is not D1."));
		HfgChangeSubstate(Channel, HFG_SUBSTATE_DISC1);
	}

    if(Channel->queryFlags & SDP_QUERY_FLAG_ONGOING)
    {
        /* SDP query is ongoing */
        DBG_PRINT_FLOW(("[HFG][FLOW] SDP query ongoing"));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_SDP);
        goto Pending;
    }

    if(Channel->linkFlags & HFG_LINK_SCO_CONNECTING)
    {
        /* SCO connection creation ongoing */
        DBG_PRINT_FLOW(("[HFG][FLOW] SCO connecting ongoing"));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_SCO_CONNECT);
        goto Pending;
    }

    if(Channel->linkFlags & HFG_LINK_SCO_DISCONNECTING)
    {
        /* SCO connection disconnecting ongoing */
        DBG_PRINT_FLOW(("[HFG][FLOW] SCO disconnecting ongoing"));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_SCO_DISCONNECT);
        goto Pending;
    }

    /*if(substate == HFG_SUBSTATE_CONN_IN2 || substate == HFG_SUBSTATE_CONN_OUT3)*/
    if(Channel->linkFlags & (HFG_LINK_CONNECTING|HFG_LINK_DISCONNECTING))
    {
        /* Waiting RFCOMM confirm */
        DBG_PRINT_FLOW(("[HFG][FLOW] Substate is I2/O3. Waiting RFCOMM confirm"));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_RFCOMM_CONFIRM);
        goto Pending;        
    }

    if(substate == HFG_SUBSTATE_CONN_OUT1 )
    {
        /* Waiting CMGR data link confirm */
        DBG_PRINT_FLOW(("[HFG][FLOW] Substate is O1. Waiting CMGR data link confirm"));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_CMGR_CONFIRM);
        goto Pending;
    }

#if 0
    if(HfgGetSubstate(Channel) != HFG_SUBSTATE_DISC1)
    {
        DBG_PRINT_FLOW(("[HFG][FLOW] Substate is not D1."));
        HfgChangeSubstate(Channel, HFG_SUBSTATE_DISC1);
    }
#endif
        
    if(substate == HFG_SUBSTATE_CONN_IN1)
    {
    	 /* If waiting auth response, reject the request first */
	 status = RF_RejectChannel(Channel->rfChannel);
        if(status == BT_STATUS_PENDING)
        {
        	Channel->linkFlags |= HFG_LINK_DISCONNECTING;
		goto Pending;
        }
        else
        {
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_REJECT_CHANNEL_FAIL, status);
        }
    }
        
    if(CMGR_IsAudioUp(&(Channel->cmgrHandler)))
    {
        DBG_PRINT_FLOW(("[HFG][FLOW] SCO connection existed"));
        status = CMGR_RemoveAudioLink(&Channel->cmgrHandler);
        DBG_PRINT_FLOW(("[HFG][FLOW] Close Audio channel : %d", status));
        Report(("[HFG][HfgDisconnecting] Remove audio link : return %d", status));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_REMOVE_AUDIO, status);
        if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
        {
            status = BT_STATUS_PENDING;
            Channel->linkFlags |= HFG_LINK_SCO_DISCONNECTING;
            goto Pending;
        }
    }

    DBG_PRINT_FLOW(("[HFG][FLOW] Deregister SCO handler"));
    CMGR_DeregisterScoHandler(&Channel->cmgrHandler);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_DEREGISTER_SCO);
    
    	if(Channel->linkFlags & HFG_LINK_DISCONNECTING)
    	{
		goto Pending;
	}
    	else if(Channel->linkFlags & HFG_LINK_HANDSFREE)
    {
        DBG_PRINT_FLOW(("[HFG][FLOW] RFCOMM channel existed"));
        status = RF_CloseChannel(Channel->rfChannel);
        DBG_PRINT_FLOW(("[HFG][FLOW] Close RFCOMM channel : %d", status));
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DISCONNECTING_CLOSE_RF, status);
		/* No matter the return value of RF_CloseChannel, we still have to wait close indication */
		/*if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )*/
        {
			Channel->linkFlags |= HFG_LINK_DISCONNECTING;
            goto Pending;
        }
    }

    HfgHfgFlushRespQueue(Channel);
    HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
Pending:
    DBG_PRINT_FLOW(("[HFG][FLOW] Disconnecting return : %d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgParseRfcommData()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses received RFCOMM data and passes it to the AT parser.
 *
 * Return:    BT_STATUS_PENDING - More data to parse.
 *            BT_STATUS_SUCCESS - Done parsing data.
 */
BtStatus HfgParseRfcommData(HfgChannel *Channel, 
                            RfCallbackParms *parms, 
                            U16 *offset, 
                            AtCommands *atParms)
{
    BtStatus    status = BT_STATUS_FAILED;
	AtStatus    atStatus;
    U16         i;
	BOOL	bContinue = atParms->bContinue;

    Report( ("[HFG][HfgParseRfcommData]  +Channel==x%X", (U32)Channel) );
    
    for (i = *offset; i < parms->dataLen; i++) 
    {
        (*offset)++;
        if (Channel->atRxLen >= HFG_RECV_BUFFER_SIZE) 
        {
            status = BT_STATUS_FAILED;
            DBG_PRINT_AT(("[HFG][AT] CMD len > HFG_RECV_BUFFER_SIZE."));
            break;
        }

        Channel->atRxBuffer[Channel->atRxLen++] = parms->ptrs.data[i];

		if ( (!bContinue && parms->ptrs.data[i] == '\r') ||
			parms->ptrs.data[i] == 26) 
        {
            DBG_PRINT_AT(("[HFG][AT] Complete CMD"));
            /* At the end of the command */
            Channel->atRxLen--;
            Channel->atRxBuffer[Channel->atRxLen] = 0;
			Channel->atRxBufDesc.writeOffset = Channel->atRxLen;
			atStatus = AtParseCommand(Channel, Channel->atRxBuffer, Channel->atRxLen, atParms);
	     //brad
            //channel->atRxLen = 0; //not reset here
			//Channel->atRxBuffer[Channel->atRxLen] = '\r';
			//Channel->atRxLen++;
			//Channel->atRxBufDesc.readOffset++;
			if(atStatus != AT_STATUS_CONTINUE)
			{
            status = BT_STATUS_SUCCESS;
			}				
            break;
        }
		else if(parms->ptrs.data[i] == 27)
		{
			/* Clear command and reset buffer */
			atParms->type = 0;
			atParms->bContinue = FALSE;
			Channel->atRxLen = 0;
		}
    }
    Report( ("[HFG][HfgParseRfcommData] - return status==x%X", status) );
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgAppCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up the callback parameters and calls the application.
 *
 * Return:    void
 */
void HfgAppCallback(HfgChannel *Channel, HfgEvent Event, 
                    BtStatus Status, U32 data)
{
    HfgCallbackParms hfgParms;
	
    hfgParms.event = Event;
    hfgParms.p.ptr = (void *)data;
    hfgParms.status = Status;
    hfgParms.errCode = Channel->cmgrHandler.errCode;

    DBG_PRINT_CALLBACK(("[HFG][CALLBACK] Event=%d, Status=%d, data=0x%X, errCode=%d", Event, Status, data, hfgParms.errCode));
	kal_trace( TRACE_GROUP_5, BT_HFG_INFO_CALLBACK_TO_ADP, Channel, Event, Status, data);
    Channel->callback(Channel, &hfgParms);
}

/*---------------------------------------------------------------------------
 *            HfgReportServiceConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Indicates that the service level connection has been established.
 *
 * Return:    void
 */
void HfgReportPacketCompleted(HfgChannel *Channel, BtStatus status)
{
    AtResults *atResults;
    Report( ("[HFG][HfgReportPacketCompleted]  +Channel=x%X, status=%d", (U32)Channel, status) );

    DBG_PRINT_AT(("[HFG][AT] Callback to ADP : status=%d", status));
    if (!IsListEmpty(&Channel->rspQueue)) 
    {
        DBG_PRINT_AT(("[HFG][AT] rspQueue empty : No"));
        DBG_PRINT_AT(("[HFG][AT] Remove rspQueue."));
        atResults = (AtResults *)RemoveHeadList(&Channel->rspQueue);
        DBG_PRINT_AT(("[HFG][AT] Callback to ADP : CMD=%d", atResults->type));
        if(atResults->flag & AT_RESULT_FLAG_SELF_HANDLED)
        {
            DBG_PRINT_AT(("[HFG][AT] Self handled flag set : Yes"));
		HfgFreeResponse(atResults);
        }
        else
        {
            DBG_PRINT_AT(("[HFG][AT] Self handled flag set : No"));
            HfgAppCallback(Channel, HFG_EVENT_RESPONSE_COMPLETE, status, (U32)atResults);
        }
    }
    else
    {
        DBG_PRINT_AT(("[HFG][AT][ERR] rspQueue empty : Yes"));
    }

    Report( ("[HFG][HfgReportPacketCompleted]  -") );
}

/*---------------------------------------------------------------------------
 *            HfgReportServiceConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Indicates that the service level connection has been established.
 *
 * Return:    void
 */
void HfgReportServiceConnected(HfgChannel *Channel)
{
#if HFG_SNIFF_TIMER >= 0
    BtSniffInfo sniffInfo;
#endif

    Report( ("[HFG][HfgReportServiceConnected]  +Channel==x%X", (U32)Channel) );

    /* Service channel is up, tell the app */
    //Channel->state = HFG_STATE_OPEN;
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    HfgAppCallback(Channel, HFG_EVENT_SERVICE_CONNECTED, BT_STATUS_SUCCESS, 
                   (U32)&Channel->cmgrHandler.bdc->link->bdAddr);

#if HFG_SNIFF_TIMER >= 0
    if (Channel->flags & CHANNEL_FLAG_SNIFFING) {
        /* Start the sniff timer */
        sniffInfo.minInterval = HFG_SNIFF_MIN_INTERVAL;
        sniffInfo.maxInterval = HFG_SNIFF_MAX_INTERVAL;
        sniffInfo.attempt = HFG_SNIFF_ATTEMPT;
        sniffInfo.timeout = HFG_SNIFF_TIMEOUT;
        CMGR_SetSniffTimer(&Channel->cmgrHandler, &sniffInfo, 
                           HFG_SNIFF_TIMER);
    }
#endif
    Report( ("[HFG][HfgReportServiceConnected]  -") );
}



/*---------------------------------------------------------------------------
 *            HfgSetupIndicatorTestRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up the response to a Indicator Test command.
 *
 * Return:    void
 */
void HfgSetupIndicatorTestRsp(HfgChannel *Channel)
{
    U8 i, count;
    U32 Indicators = 0;

    if ( Channel == NULL )
        return;

    Report( ("[HFG][HfgSetupIndicatorTestRsp]  +Channel==x%X", (U32)Channel) );
    
    Channel->flags |= CHANNEL_FLAG_SEND_OK;
    Channel->atResults.type = AT_INDICATORS | AT_TEST;
    #if 1
    
    //Indicators = bt_hfg_get_supported_indicator();
    if(Indicators == 0)
        Indicators = g_DefaultIndicators;
    i = 0;
    count = 0;
    while(Indicators)
    {
        if(Indicators & 0x1)
        {
            if ( count < AT_MAX_INDICATORS )    //klocwork warning
            Channel->atResults.p.hf.indicators_test.ind[count].description =  g_indicators[i][0];
            if ( count < AT_MAX_INDICATORS )    //klocwork warning
            Channel->atResults.p.hf.indicators_test.ind[count].range = g_indicators[i][1];
            count++;
            Channel->IndIndex[i] = count;
            Report( ("[HFG][HfgSetupIndicatorTestRsp]  Add indicator \"%s\" range==(%s)", g_indicators[i][0], g_indicators[i][1]) );
        }
        else
        {
            Channel->IndIndex[i] = 0;
        }
        Indicators = (Indicators >> 1);
        i++;
    }
    Channel->atResults.p.hf.indicators_test.num = count;
    Report( ("[HFG][HfgSetupIndicatorTestRsp]  Add %d indicators", count) );

    #else
    #ifdef __HF_V15__
    Channel->atResults.p.hf.indicators_test.num = 7;
    #else
    Channel->atResults.p.hf.indicators_test.num = 3;
    #endif
    Channel->atResults.p.hf.indicators_test.ind[0].description = "service";
    Channel->atResults.p.hf.indicators_test.ind[0].range = "0,1";
    Channel->atResults.p.hf.indicators_test.ind[1].description = "call";
    Channel->atResults.p.hf.indicators_test.ind[1].range = "0,1";
    Channel->atResults.p.hf.indicators_test.ind[2].description = "callsetup";
    Channel->atResults.p.hf.indicators_test.ind[2].range = "0,3";
    #ifdef __HF_V15__
    Channel->atResults.p.hf.indicators_test.ind[3].description = "callheld";
    Channel->atResults.p.hf.indicators_test.ind[3].range = "0,2";
    Channel->atResults.p.hf.indicators_test.ind[4].description = "battchg";
    Channel->atResults.p.hf.indicators_test.ind[4].range = "0,5";
    Channel->atResults.p.hf.indicators_test.ind[5].description = "signal";
    Channel->atResults.p.hf.indicators_test.ind[5].range = "0,5";
    Channel->atResults.p.hf.indicators_test.ind[6].description = "roam";
    Channel->atResults.p.hf.indicators_test.ind[6].range = "0,1";
    #endif
    #endif
    Report( ("[HFG][HfgSetupIndicatorTestRsp]  -") );
}

/*---------------------------------------------------------------------------
 *            HfgSetupIndicatorReadRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up the response to a Indicator Read command.
 *
 * Return:    void
 */
void HfgSetupIndicatorReadRsp(HfgChannel *Channel)
{
    Report( ("[HFG][HfgSetupIndicatorReadRsp] +Channel==x%X", (U32)Channel) );

    Channel->flags |= CHANNEL_FLAG_SEND_OK;
    Channel->atResults.type = AT_INDICATORS | AT_READ;
    #ifdef __HF_V15__
    Channel->atResults.p.hf.indicators_read.num = 7;
    #else
    Channel->atResults.p.hf.indicators_read.num = 3;
    #endif
    Channel->atResults.p.hf.indicators_read.ind[0].value = Channel->service;
    Channel->atResults.p.hf.indicators_read.ind[1].value = Channel->call;
    Channel->atResults.p.hf.indicators_read.ind[2].value = Channel->callSetup;
    #ifdef __HF_V15__
    Channel->atResults.p.hf.indicators_read.ind[3].value = Channel->held;
    Channel->atResults.p.hf.indicators_read.ind[4].value = Channel->battery;
    Channel->atResults.p.hf.indicators_read.ind[5].value = Channel->signal;
    Channel->atResults.p.hf.indicators_read.ind[6].value = Channel->roaming;
    #endif
    Report( ("[HFG][HfgSetupIndicatorReadRsp] -") );
}

/*---------------------------------------------------------------------------
 *            HfgSetupCallHoldReadRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up the response to a Call Hold Read command.
 *
 * Return:    void
 */
void HfgSetupCallHoldReadRsp(HfgChannel *Channel)
{
    Report( ("[HFG][HfgSetupCallHoldReadRsp] +Channel==x%X", (U32)Channel) );
#ifdef __BT_HFG_PROFILE__
	if(bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
	{
            Channel->flags |= CHANNEL_FLAG_SEND_OK;
            Channel->atResults.type = AT_CALL_HOLD | AT_TEST;
            Channel->atResults.p.hf.hold_test.flags = 
                                        AT_FLAG_HOLD_RELEASE_HELD_CALLS    |
                                        AT_FLAG_HOLD_RELEASE_ACTIVE_CALLS  |
                                        AT_FLAG_HOLD_HOLD_ACTIVE_CALLS     |
                                        AT_FLAG_HOLD_ADD_HELD_CALL         |
                                        AT_FLAG_HOLD_EXPLICIT_TRANSFER;
		if(bt_hfg_check_supported_feature(HFG_FEATURE_ENHANCED_CALL_CTRL))
		{
	            	Channel->atResults.p.hf.hold_test.flags |= 
                	                        AT_FLAG_HOLD_RELEASE_SPECIFIC_CALL |
                    	                    AT_FLAG_HOLD_HOLD_SPECIFIC_CALL;
		}
	}
        else
#endif
	{
            Channel->atResults.type = AT_OK;
	}
    Report( ("[HFG][HfgSetupCallHoldReadRsp] -") );
}

/*---------------------------------------------------------------------------
 *            HfgCloseChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close ACL association and set link flag to 0
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.
 */
BtStatus HfgCloseChannel(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL bACL = (BOOL)((Channel->linkFlags & HFG_LINK_ACL)==HFG_LINK_ACL);

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CLOSE_ACL_CHANNEL, Channel);

    DBG_PRINT_FLOW(("[HFG][FLOW] ACL associated : %s", bACL?"Yes":"No"));
    /* FLOW : Disassociate ACL link */
	if (bACL) 
	//if ( CMGR_IsLinkUp(&Channel->cmgrHandler) )
    {
        status = CMGR_RemoveDataLink(&Channel->cmgrHandler);
        CMGR_RemoveCmgrWithDevice(&Channel->cmgrHandler);
        DBG_PRINT_FLOW(("[HFG][FLOW] Disassociate ACL link : %d", status));
        Channel->linkFlags = 0;
        if(status != BT_STATUS_SUCCESS)
        {
			DBG_PRINT_ERROR(("[HFG][ERR] CMGR_RemoveDataLink : %d - (%d,%d)", status, __BT_FILE__, __LINE__));
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
            Assert(0);
        }
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN,__BT_FILE__, __LINE__);
	}
	
    /* No sniff timer needed any more */
    (void)CMGR_ClearSniffTimer(&Channel->cmgrHandler);
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgOpenChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open ACL association and set set HFG_LINK_ACL to linkflag 
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.
 */
BtStatus HfgOpenChannel(HfgChannel *Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL bACL = (BOOL)((Channel->linkFlags & HFG_LINK_ACL)==HFG_LINK_ACL);

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_OPEN_ACL_CHANNEL, 
		Channel,
		Addr->addr[0],
		Addr->addr[1],
		Addr->addr[2],
		Addr->addr[3],
		Addr->addr[4],
		Addr->addr[5]);

    DBG_PRINT_FLOW(("[HFG][FLOW] ACL associated : %s", bACL?"Yes":"No"));
    if( !bACL )
    {
        status = CMGR_CreateDataLink(&Channel->cmgrHandler, Addr);
        DBG_PRINT_FLOW(("[HFG][FLOW] Associate ACL link : %d", status));
        if (status == BT_STATUS_SUCCESS)
        {
            /* Start the service search */
            Channel->linkFlags |= HFG_LINK_ACL;
			kal_trace(TRACE_GROUP_5, BT_HFG_INFO_OPEN_ACL_CHANNEL_SUCCESS);
        }
		else if(status == BT_STATUS_PENDING)
		{
			Channel->linkFlags |= HFG_LINK_ACL_CONNECTING;
		}
		else
        {
            DBG_PRINT_ERROR(("[HFG][ERR] CMGR_CreateDataLink failed. status=%d - (%d,%d)", status, __BT_FILE__, __LINE__));
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);			
		}
        }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, Channel);
    }

    Report( ("[HFG][HfgOpenChannel] - return status==x%X", status) );
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgFindChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the Hfg channel based on the remote device.
 *
 * Return:    A pointer to the hf channel, NULL if not found.
 */
HfgChannel * HfgFindChannel(BtRemoteDevice *remDev)
{
    HfgChannel *hfgChannel;

    /*Report( ("[HFG][%s] +BtRemoveDevice==x%X", __func__, (U32)remDev) );*/
    
    hfgChannel = (HfgChannel *)GetHeadList(&HFG(channelList));
    while (&hfgChannel->node != &HFG(channelList)) {
        if ((hfgChannel->cmgrHandler.bdc) && 
            (CMGR_GetRemoteDevice(&hfgChannel->cmgrHandler) == remDev)) {
            break;
        }

        /* Look at the next node */
        hfgChannel = (HfgChannel *)GetNextNode(&hfgChannel->node);
    }

    if (&hfgChannel->node == &HFG(channelList)) {
        /* Not found */
        hfgChannel = 0;
    }

    /*Report( ("[HFG][%s] - return HfgChannel==x%X", __func__, hfgChannel) );*/

    return hfgChannel;
}

HfgChannel * HfgFindRegisteredChannel(RfChannel *rfChannel)
{
    	HfgChannel *Channel = NULL;

    Report( ("[HFG][HfgFindRegisteredChannel] +RfChannel==x%X", (U32)rfChannel) );

    	Channel = (HfgChannel *)GetHeadList(&HFG(channelList));
    	while (&Channel->node != &HFG(channelList)) 
	{
        if (Channel->rfChannel == rfChannel) 
            		break;

        /* Look at the next node */
        Channel = (HfgChannel *)GetNextNode(&Channel->node);
    }

    	if (&Channel->node == &HFG(channelList)) 
	{
        /* Not found */
        	Channel = NULL;
    }

    Report( ("[HFG][HfgFindRegisteredChannel] - return HfgChannel==x%X", Channel) );
    

    return Channel;
}

/*---------------------------------------------------------------------------
 *            HfgAllocRfChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocate an RF channel (make it "in use").
 *
 * Return:    A pointer to the RF channel, NULL if not found.
 */
RfChannel *HfgAllocRfChannel(void)
{
    RfChannel *rfChannel = 0;

    Report(("+ HfgAllocRfChannel."));
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_ALLOC_RF_CHANNEL);
	
    if (!IsListEmpty(&HFG(freeRfChannelList))) 
    {
        rfChannel = (RfChannel *)RemoveHeadList(&HFG(freeRfChannelList)); 
    }

    if (rfChannel) 
    {
        rfChannel->callback = HfgRfCallback;
        rfChannel->maxFrameSize = RF_MAX_FRAME_SIZE;
        rfChannel->priority = RF_DEFAULT_PRIORITY;
    }
    else
    {
        /* Allocate fail */
        DBG_PRINT_ERROR(("[HFG][ERR] Alloc RfChannel failed - (%d,%d)", __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR, __BT_FILE__, __LINE__);
    }

    Report(("- HfgAllocRfChannel."));
    return rfChannel;
}

/*---------------------------------------------------------------------------
 *            HfgFreeRfChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Frees an RF channel (makes it available).
 *
 * Return:    void
 */
void HfgFreeRfChannel(RfChannel *rfChannel)
{
    Report(("+ HfgFreeRfChannel."));
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_FREE_RF_CHANNEL, rfChannel);

    if (!IsNodeOnList(&HFG(freeRfChannelList), &rfChannel->node)) 
    {
    		/* clear the userContext before insert to free channel list */
    		rfChannel->userContext = NULL;
        InsertTailList(&HFG(freeRfChannelList), &rfChannel->node);
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR, __BT_FILE__, __LINE__);
	}
    Report(("- HfgFreeRfChannel."));
}

BtStatus HfgEnableSniffTimerMode(CmgrHandler *Handler) 
{
	Report(("+ HfgEnableSniffTimerMode."));
    CMGR_ActivateSniffTimer(Handler);           
/*    
    sniffInfo.minInterval = MTK_SNIFF_MIN_INTERVAL;
    sniffInfo.maxInterval = MTK_SNIFF_MAX_INTERVAL;
    sniffInfo.attempt = MTK_SNIFF_ATTEMPT;
    sniffInfo.timeout = MTK_SNIFF_TIMEOUT;
    CMGR_SetSniffTimer(Handler, &sniffInfo, 
                   MTK_SNIFF_TIMER);
*/    
	Report(("- HfgEnableSniffTimerMode."));
    return BT_STATUS_SUCCESS;
}


BtStatus HfgDisableSniffTimerMode(CmgrHandler *Handler)  
{
    BtStatus    status = BT_STATUS_SUCCESS;

    Report(("+ HfgDisableSniffTimerMode."));

    CMGR_ClearSniffTimer(Handler);
    Report(("- HfgDisableSniffTimerMode."));
    return status;
}

#if defined(BTMTK_ON_WISE)
void bt_hfg_send_sco_connect_req(module_type src_mod_id, kal_uint16 connect_id)
{
    kal_prompt_trace(MOD_BT, "[BT][WARN] bt_hfg_send_sco_connect_req : Media should not call this function in wisdom");
/*
    bt_hfg_sco_connect_req_struct* msg_p = 
        (bt_hfg_sco_connect_req_struct*)construct_local_para(sizeof(bt_hfg_sco_connect_req_struct), TD_CTRL);

    msg_p->connection_id = connect_id;
*/
}

void bt_hfg_send_sco_disconnect_req(module_type src_mod_id, kal_uint16 connect_id)
{
    kal_prompt_trace(MOD_BT, "[BT][WARN] bt_hfg_send_sco_disconnect_req : Media should not call this function in wisdom");
}
#endif

void HfgCheckNegotiateDone(HfgChannel *Channel)
{
    Report(("HfgCheckNegotiateDone"));
    if (Channel->flags & CHANNEL_FLAG_NEG_DONE) 
    {
        	/* Negotiation is complete */
        	Channel->flags &= ~(CHANNEL_FLAG_NEG_DONE | 
                            CHANNEL_FLAG_NEGOTIATE);

        /* Service channel is up, tell the app */
        /* FLOW : Negotiation done */
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN1)
        {
            DBG_PRINT_FLOW(("[HFG][FLOW] Negotiation done"));
            HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC1);
        }
        else
        {
            Report(("HfgPacketHldr: Unexpected state (%d) when SLC is up.", HfgGetSubstate(Channel)));
            Assert(0);
            HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC1);
        }
    }
}
