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
 *     $Workfile:hfg.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains API functions for the Hands-free Audio
 *              Gateway SDK.
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
 * Jun 16 2009 mtk80029
 * [MAUI_01869480] [BT HFP] revise code related to audio connection request
 * 
 *
 * Jun 2 2009 mtk80029
 * [MAUI_01837169] [BT] remove kal_prompt_trace
 * add trace
 * 
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"


#define __BT_FILE__	BT_FILE_HFG

/***********************************************
* extern global variable
***********************************************/
extern const U8 g_indicatorRange[NUM_OF_HFG_IND][3];
extern const char *g_indicators[NUM_OF_HFG_IND][2];

/*---------------------------------------------------------------------------
 *            hfg_init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize HF Gateway SDK.  Initialize state and other variables.
 *
 * Return:    TRUE - Memory successfully initialized.
 */
BOOL hfg_init(void)
{
    U16 i;
    BOOL ret = TRUE;

    DBG_PRINT_API(("[HFG][API] + hfg_init"));
	kal_trace(TRACE_GROUP_5,BT_HFG_API_INIT);
	
    /* Initialize context memory */
    if (!(HfgAlloc())) 
    {
        DBG_PRINT_ERROR(("[HFG][ERR] HfgAlloc Fail - (%s,%d)", __BT_FILE__, __LINE__));
        kal_trace(TRACE_GROUP_5, BT_HFG_ERROR, __BT_FILE__, __LINE__);
        ret = FALSE;
        goto exit;
    }

    /* Initialize lists */
    InitializeListHead(&HFG(channelList));
    InitializeListHead(&HFG(freeRfChannelList));
    for (i = 0; i < NUM_BT_DEVICES * 2 + 2; i++) 
    {
        HFG(rfChannels)[i].userContext = 0;
        InsertTailList(&HFG(freeRfChannelList), &HFG(rfChannels)[i].node);
#ifndef __BT_2_1_SIMPLE_PAIRING__        
        RF_ChannelInit(&(HFG(rfChannels)[i]), BT_SEC_HIGH);
#else
        RF_ChannelInit(&(HFG(rfChannels)[i]), BT_SEC_MED);
#endif
    }

	/* Init free response list */
	InitializeListHead(&HFG(freeRespList));
	for (i = 0; i < (sizeof(HFG(freeResp))/sizeof(HFG(freeResp)[0])); i++)
	{
		InsertTailList(&HFG(freeRespList), &HFG(freeResp)[i].node);
	}
	
    if (AT_Init(&HFG(atContext)) != AT_STATUS_OK) 
    {
        DBG_PRINT_ERROR(("[HFG][ERR] AT_Init Fail - (%s,%d)", __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR, __BT_FILE__, __LINE__);
        ret = FALSE;
        goto exit;
    }

exit:
    DBG_PRINT_API(("[HFG][API] - hfg_init : return %s", ret ? "TRUE" : "FALSE"));
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            hfg_register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes all necessary data structures, registers with
 *            RFCOMM and SDP. 
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_register(HfgChannel *Channel, HfgCallback Callback, BOOL bHeadset)
{
    BtStatus   status = BT_STATUS_SUCCESS;

    DBG_PRINT_API(("[HFG][API] + hfg_register(Channel=0x%X, Callback=0x%X, bHeadset=%d)", (U32)Channel, (U32)Callback, bHeadset?1:0));
	kal_trace(TRACE_GROUP_5, BT_HFG_API_REGISTER, Channel, Callback, bHeadset ? 1 : 0);

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    OS_LockStack();

    /* Register this channel in the list */
    if (!IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        /* Clear the channel structure */
        OS_MemSet((U8*)Channel, 0, sizeof(HfgChannel));

        /* Set channel type, audio gateway of handsfree or headset */
        Channel->type = bHeadset ? AG_TYPE_HS : AG_TYPE_HF;

        /* Register SDP record. if HFG(hfgSdpRecord).num is 0 that 
            means HFG SDP is not registered yet */
        if (HFG(hfgSdpRecord)[Channel->type].num == 0) 
        {
            /* This is the fist registration with RFCOMM */
            HFG(hfgService)[Channel->type].serviceId = 0;

            /* Register with SDP if this is the first channel */
            status = HfgRegisterSdpServices( (BOOL)(Channel->type==AG_TYPE_HS) );

            if (status != BT_STATUS_SUCCESS) 
            {
                Report(("hfg_register: HfgRegisterSdpServices result failed. (%d).", status));
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
                goto error;
            }

            #if BT_SECURITY == XA_ENABLED
            /* Also, register security records */
            HFG(hfgSecRec)[Channel->type].id =  SEC_RFCOMM_ID;
            HFG(hfgSecRec)[Channel->type].channel = HFG(hfgService)[Channel->type].serviceId;
            HFG(hfgSecRec)[Channel->type].level = HFG_SECURITY_SETTINGS;    
            status = SEC_Register(&(HFG(hfgSecRec)[Channel->type]));
            if (status != BT_STATUS_SUCCESS) 
            {
                DBG_PRINT_ERROR(("[HFG][ERR] SEC_Register fail : %d - (%s,%d)", status, __BT_FILE__, __LINE__));
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
                /* Failed security registration, deregister SDP record */
                (void)HfgDeregisterSdpServices((BOOL)(Channel->type==AG_TYPE_HS));
                goto error;
            }
            #endif			
        }

        /* Each HfgChannel allocates a RfChannel and register to RFCOMM  */
        if(status == BT_STATUS_SUCCESS)
        {
            /* Register a channel to receive Handsfree connections */
            Channel->rfChannel = HfgAllocRfChannel();
            if (Channel->rfChannel) 
            {
                status = HfgRegisterServerChannel(Channel);
            } 
            else 
            {
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR, __BT_FILE__, __LINE__);
                /* No channels left to register */
                status = BT_STATUS_NO_RESOURCES;
            }
            
            if( status != BT_STATUS_SUCCESS )
            {
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
                /* Register server channel fail */
                if(Channel->rfChannel)
                {
                    HfgFreeRfChannel(Channel->rfChannel);
                    Channel->rfChannel = NULL;
                }
                if( HFG(regCount[Channel->type]) == 0 )
                {
                    SEC_Unregister(&(HFG(hfgSecRec)[Channel->type]));
                    HfgDeregisterSdpServices((BOOL)(Channel->type==AG_TYPE_HS));
                }
                goto error;
            }
        }

        if (status == BT_STATUS_SUCCESS) 
        {
            /* Register the HF Audio Gateway channel */
            Channel->callback = Callback;
            /* Channel->atTxPacket.data = Channel->atTxData; */
            Channel->atTxPacket.data = Channel->atTxData = NULL;
            Channel->atTxPacket.priority_type = BTP_FLAG_HIGH_PRIPORTY;
            /* Initialize indicators */
            Channel->service = 1;//Home/Roam network available			
            Channel->call = 0;//no call active
            Channel->callSetup = 0;//not currently in call setup

            Channel->held = 0;
            Channel->signal = 0;
            Channel->battery = 0;

            /* Initialize default state */
            Channel->nrecDisable = FALSE;
            Channel->ibRing = TRUE;

		//Channel->atRxBufDesc.buff = Channel->atRxBuffer;
		/* Channel->atRxBufDesc.buffSize = sizeof(Channel->atRxBuffer);*/
		Channel->atRxBufDesc.buff = Channel->atRxBuffer = NULL;
		Channel->atRxBufDesc.buffSize = HFG_RECV_BUFFER_SIZE;

            /* Insert the channel on the list */
            InsertTailList(&HFG(channelList), &Channel->node);
            InitializeListHead(&Channel->rspQueue);
            (void)CMGR_RegisterHandler(&Channel->cmgrHandler, HfgServiceConnectionCallback);
            HFG(regCount[Channel->type]++);
			kal_trace(TRACE_GROUP_5, BT_HFG_REGISTER_HFG_SUCCESS, Channel);
        }
    }
    else 
    {
        DBG_PRINT_ERROR(("[HFG][ERR] hfg_register: The channel is already in use. (%d,%d)", __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_ALREADY_REGISTERED, Channel);
        /* Already registered */
        status = BT_STATUS_IN_USE;
        goto error;
    }
error:
    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_register : return status=%d", status));
    return status;
}


/*---------------------------------------------------------------------------
 *            hfg_deregister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters with RFCOMM and SDP. 
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_deregister(HfgChannel *Channel)
{
    BtStatus   status = BT_STATUS_SUCCESS;

    DBG_PRINT_API(("[HFG][API] + hfg_deregister(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

	kal_trace(TRACE_GROUP_5, BT_HFG_API_DEREGISTER, Channel);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    OS_LockStack();		
    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        /* If  state is not closed, return BT_STATUS_CONNECTION_EXIST */
        if(HfgGetSubstate(Channel) != HFG_SUBSTATE_CLOSED1)
        {
            kal_trace(TRACE_GROUP_5, BT_HFG_DEFER_DEREGISTERATION, Channel, HfgGetSubstate(Channel));
            if(HfgGetState(Channel) != HFG_STATE_DISCONNECTING)
            {
                HfgDisconnecting(Channel);
            }
            status = BT_STATUS_CONNECTION_EXIST;
        }
        else
        {
        
        Assert(HFG(regCount[Channel->type]));
        /* If the RfChannel is registered to RFCOMM, then deregister from RFCOMM */
        status = HfgDeregisterServerChannel(Channel);
		if (status != BT_STATUS_SUCCESS) 
        {
            /* Can not deregister server channel */
            Report(("hfg_deregister: HfgFreeRfChannel failed. status=%d.", status));
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
        }
		/* No matter the deregister is successful or not, we should free the rfchannel */
		if(Channel->rfChannel)
		{
			/* A HF channel was unregistered, free it */
			HfgFreeRfChannel(Channel->rfChannel);
			Channel->rfChannel = NULL;
		}
	
        (void)CMGR_DeregisterHandler(&Channel->cmgrHandler);
        RemoveEntryList(&Channel->node);
        
        if (!(--HFG(regCount[Channel->type]))) 
        {
            /* Deregister with SDP */
            (void)HfgDeregisterSdpServices( (BOOL)(Channel->type==AG_TYPE_HS) );

            #if BT_SECURITY == XA_ENABLED
            /* Remove Security Record */
            SEC_Unregister(&(HFG(hfgSecRec)[Channel->type]));
            #endif
        }
    }
    }
    else 
    {
        Report(("hfg_deregister: Channel is not registered."));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
        /* Channel is not registered */
        status = BT_STATUS_NOT_FOUND;
    }
    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_deregister : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_create_service_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens a service level connection. 
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_create_service_link(HfgChannel *Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_create_service_link(Channel=ox%X, Addr=0x%02X%02X%02X%02X%02X%02X)", 
    		                 (U32)Channel, 
    		                 Addr->addr[0], 
    		                 Addr->addr[1], 
    		                 Addr->addr[2], 
    		                 Addr->addr[3], 
    		                 Addr->addr[4], 
    		                 Addr->addr[5]));
    kal_trace(TRACE_GROUP_5, BT_HFG_API_CREATE_SERVICE_LINK, 
				Channel,
				Addr->addr[0], 
				Addr->addr[1], 
				Addr->addr[2], 
				Addr->addr[3], 
				Addr->addr[4], 
				Addr->addr[5]);
//    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    
#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif

    OS_LockStack();

    /* See if the channel is registered */
    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CLOSED1)
        {
            status = HfgDeregisterServerChannel(Channel);
            if(status == BT_STATUS_SUCCESS)
		{
			/* FLOW : Create/Associate ACL link */
                    status = HfgOpenChannel(Channel, Addr);
            		if (status == BT_STATUS_SUCCESS)
			{
				/* FLOW : Start SDP query */
				status = HfgStartServiceQuery(Channel, BSQM_FIRST);
				if(status == BT_STATUS_PENDING)
				{
                                HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_OUT2);
				}
				else
				{
					BtStatus intStatus;
						kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
                                HfgCloseChannel(Channel);
                                	intStatus = HfgRegisterServerChannel(Channel);
                                	if( intStatus != BT_STATUS_SUCCESS )
						{
						kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, intStatus);
                                    Assert(0);
				}
            		}
				}
			else if(status == BT_STATUS_PENDING)
			{
                        HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_OUT1);
			}
			else
			{
					BtStatus intStatus;
					kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
                        /* Keep substate in C1 */
					intStatus = HfgRegisterServerChannel(Channel);
					if( intStatus != BT_STATUS_SUCCESS )
					{
						kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, intStatus);
                        Assert(0);
                }
            }
			}
            else
            {
                    /* we shall check if the HfgDeregisterServerChannel is failed 
                    due to the authentication is ongoing */
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
		}
        } 
        else 
        {
            /* Channel is already in use */
            DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
            status = BT_STATUS_IN_USE;
        }
    }
    else
    {
        DBG_PRINT_ERROR(("[HFG][ERR] Channel is not registered. Channel=0x%X - (%d,%d)", (U32)Channel, __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
    }

    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_create_service_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_disconnect_service_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close a service level connection. 
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_disconnect_service_link(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_disconnect_service_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    kal_trace(TRACE_GROUP_5, BT_HFG_API_DISCONNECT_SERVICE_LINK, Channel, HfgGetSubstate(Channel));
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");        
        return BT_STATUS_RESTRICTED;
    }
#endif

    OS_LockStack();

    /* See if the channel is registered */
    if (IsNodeOnList(&HFG(channelList), &Channel->node))
    {
        switch(HfgGetSubstate(Channel))
        {
        case HFG_SUBSTATE_OPEN1:
        case HFG_SUBSTATE_OPEN2:
        case HFG_SUBSTATE_SLC1:
        case HFG_SUBSTATE_SLC2:
            status = HfgDisconnecting(Channel);
            break;
         /* Below is unexpected substates 
          * Unexpected state handling */
        case HFG_SUBSTATE_CLOSED1:
            status = BT_STATUS_NO_CONNECTION;
            break;
        case HFG_SUBSTATE_DISC1:
            /* Tell caller the disconnection is already in progress */
            status = BT_STATUS_IN_PROGRESS;
            break;
	  case HFG_SUBSTATE_CONN_OUT1:
	  case HFG_SUBSTATE_CONN_OUT2:
	  case HFG_SUBSTATE_CONN_OUT3:
	  case HFG_SUBSTATE_CONN_IN1:
	  case HFG_SUBSTATE_CONN_IN2:
	  	/* Prevent AP want to cancel the connecting */
		status = HfgDisconnecting(Channel);
	  	break;
        default:
            DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
            status = BT_STATUS_BUSY;
            break;
        }	
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
	}
    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_disconnect_service_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_create_audio_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates an audio (SCO) link to the audio gateway.
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_create_audio_link(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_create_audio_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    
    kal_trace(TRACE_GROUP_5, BT_HFG_API_CREATE_AUDIO_LINK, Channel, HfgGetSubstate(Channel), CMGR_IsAudioUp(&Channel->cmgrHandler) ? 1 : 0);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
    
#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif
    
    OS_LockStack();

    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC1)
        {
            /* FLOW : Create Audio channel */
            if( CMGR_IsAudioUp(&Channel->cmgrHandler) )
            {
                Report(("hfg_create_audio_link: Audio connection already exists."));
				kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AUDIO_EXIST);
                /* Audio connection exist, return success directly */
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = CMGR_CreateAudioLink(&Channel->cmgrHandler);
                DBG_PRINT_FLOW(("[HFG][FLOW] Create Audio channel : %d", status));
                if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
                {
                    status = BT_STATUS_PENDING;
                    Channel->linkFlags |= HFG_LINK_SCO_CONNECTING;
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC2);
                }
				else
				{
					kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
				}
            }			
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC2)
        {
            DBG_PRINT_FLOW(("[HFG][FLOW] Audio creation ongoing : %s",(Channel->linkFlags | HFG_LINK_SCO_CONNECTING)?"Yes":"No"));
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN, __BT_FILE__, __LINE__);
            /* If state is in S2 */
            if(Channel->linkFlags | HFG_LINK_SCO_CONNECTING)
            {
                Report(("hfg_create_audio_link: Audio creation is ongoing."));
				kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AUDIO_CONENCTING);
                /* Audio creation is ongoing */
                status = BT_STATUS_IN_PROGRESS;
                DBG_PRINT_FLOW(("[HFG][FLOW] Return in progress"));
            }
            else
            {
                if (Channel->linkFlags | HFG_LINK_SCO_DISCONNECTING)
                {
                    Report(("hfg_create_audio_link: Audio disconnection ongoing."));
					kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AUDIO_DISCONENCTING);
                    /* Audio disconnecting is ongoing */
                    status = BT_STATUS_FAILED;
                }
                DBG_PRINT_FLOW(("[HFG][FLOW] Return Fail"));
            }
        }
        else
        {
            Report(("hfg_create_audio_link: No connection."));
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN, __BT_FILE__, __LINE__);
            status = BT_STATUS_NO_CONNECTION;
        }
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
	}

    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_create_audio_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_disconnect_audio_link()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Releases the audio connection with the audio gateway.
 *
 * Return:    (See header file)
 *
 */
BtStatus hfg_disconnect_audio_link(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_disconnect_audio_link(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    kal_trace(TRACE_GROUP_5, BT_HFG_API_DISCONNECT_AUDIO_LINK, Channel, HfgGetSubstate(Channel), CMGR_IsAudioUp(&Channel->cmgrHandler) ? 1 : 0);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);


#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif
    
    OS_LockStack();

    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC1)
        {
            if( !CMGR_IsAudioUp(&Channel->cmgrHandler) )
            {
                Report(("hfg_disconnect_audio_link: No audio connection."));
                /* No audio connection, return success directly */
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                Report(("hfg_disconnect_audio_link: Remove audio link."));
                status = CMGR_RemoveAudioLink(&Channel->cmgrHandler);
                DBG_PRINT_FLOW(("[HFG][FLOW] Close Audio channel : %d", status));
                if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
                {
                    status = BT_STATUS_PENDING;
                    Channel->linkFlags |= HFG_LINK_SCO_DISCONNECTING;
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC2);
                }
            }
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC2)
        {
            DBG_PRINT_FLOW(("[HFG][FLOW] Audio disconnecting ongoing : %s",(Channel->linkFlags | HFG_LINK_SCO_DISCONNECTING)?"Yes":"No"));
            if(Channel->linkFlags | HFG_LINK_SCO_DISCONNECTING)
            {
                Report(("hfg_disconnect_audio_link: Audio disconnection in progress."));
                /* Audio disconnection in progress */
                status = BT_STATUS_IN_PROGRESS;
                DBG_PRINT_FLOW(("[HFG][FLOW] Return in progress"));
            }
            else 
            {
                if(Channel->linkFlags | HFG_LINK_SCO_CONNECTING)
                {
                    Report(("hfg_disconnect_audio_link: Audio connection in progress, so return fail."));
                    /* Audio connection in progress, so return fail */
                    status = BT_STATUS_FAILED;
                }
                DBG_PRINT_FLOW(("[HFG][FLOW] Return Fail"));
            }
        }
        else
        {
            Report(("hfg_disconnect_audio_link: No connection or Busy."));
			kal_trace(TRACE_GROUP_5, BT_HFG_INFO_NO_AUDIO_CONNECTION);
            if( HfgGetState(Channel) != HFG_STATE_SLC)
                status = BT_STATUS_NO_CONNECTION;
            else
                status = BT_STATUS_BUSY;
        }
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
	}

    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_disconnect_audio_link : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_accept_connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept incoming connection request.
 *
 * Return:    (See header file)
 */
BtStatus hfg_accept_connect(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    DBG_PRINT_API(("[HFG][API] + hfg_accept_connect(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    
    kal_trace(TRACE_GROUP_5, BT_HFG_API_ACCEPT_CONNECT, Channel);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif
	
    if (!IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
        return BT_STATUS_NOT_FOUND;
    }
	
    if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_IN1)
    {
        /* status = HfgOpenChannel(Channel, &Channel->remDev->bdAddr); */
	 status = HfgOpenChannel(Channel, &Channel->bdAddr);
        if (status == BT_STATUS_SUCCESS)
        {
            status=RF_AcceptChannel(Channel->rfChannel);
            DBG_PRINT_FLOW(("[HFG][FLOW] Accept RFCOMM channel : %d", status));
            if (status == BT_STATUS_PENDING) 
            {
            		Channel->linkFlags |= HFG_LINK_CONNECTING;
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_IN2);
            }
            else
            {
				kal_trace(TRACE_GROUP_5, BT_HFG_WARN_ACCEPT_CHANNEL_FAIL, status);			
                /* free RF channel and close hfg channel */
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
            }            
        }
        else
        {
            Report(("hfg_accept_connect: Open channel failed. Try to reject the RF channel."));
            status = RF_RejectChannel(Channel->rfChannel);
            if(status == BT_STATUS_PENDING)
            {
            		Channel->linkFlags |= HFG_LINK_DISCONNECTING;
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_IN2);
            }
            else
            {
				kal_trace(TRACE_GROUP_5, BT_HFG_WARN_REJECT_CHANNEL_FAIL, status);
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
            }
        }
    }
    else
    {
        DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
        status = BT_STATUS_FAILED;
    }

    DBG_PRINT_API(("[HFG][API] - hfg_accept_connect : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_reject_connect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Reject incoming connection request.
 *
 * Return:    (See header file)
 */
BtStatus hfg_reject_connect(HfgChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    DBG_PRINT_API(("[HFG][API] + hfg_reject_connect(Channel=0x%X)", (U32)Channel));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    
    kal_trace(TRACE_GROUP_5, BT_HFG_API_REJECT_CONNECT, Channel);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif

    if (!IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
        return BT_STATUS_NOT_FOUND;
    }

    if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_IN1)
    {
        status = RF_RejectChannel(Channel->rfChannel);
        if(status == BT_STATUS_PENDING)
        {
	     Channel->linkFlags |= HFG_LINK_DISCONNECTING;
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_IN2);
        }
        else
        {
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN_REJECT_CHANNEL_FAIL, status);
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
        }
    }
    else
    {
        DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
        status = BT_STATUS_FAILED;
    }
    	
    DBG_PRINT_API(("[HFG][API] - hfg_reject_connect : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_set_indicator_value()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets the current value for an indicator.
 *
 * Return:    (See header file)
 */
BtStatus hfg_set_indicator_value(HfgChannel *Channel, HfgIndicator Ind, 
                               U8 Value, HfgResponse *Response)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL	bHeadset;
    
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    
    bHeadset = (Channel->type == AG_TYPE_HS) ? 1 : 0;

    DBG_PRINT_API(("[HFG][API] + hfg_set_indicator_value(Channel=0x%X, Ind=%d, Value=%d,Response=0x%X)", 
                                (U32)Channel,
                                Ind,
                                Value,
                                (U32)Response));

    kal_trace(TRACE_GROUP_5, BT_HFG_API_SET_INDICATOR, Channel, Ind, Value, Response, bHeadset ? 1 : 0);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Response != 0);

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif

    OS_LockStack();

    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
		if(bHeadset)
        {
            status = BT_STATUS_NOT_SUPPORTED;
        }
        else if (HfgGetState(Channel) < HFG_STATE_OPEN)
        {
            status = BT_STATUS_NO_CONNECTION;
        }
        else
        {
            status = BT_STATUS_SUCCESS;
            if( Ind < NUM_OF_HFG_IND )
            {
                if( Value < g_indicatorRange[Ind][0] || Value > g_indicatorRange[Ind][1] 
				|| Channel->IndIndex[Ind] == 0 )
                {
					kal_trace(TRACE_GROUP_5, BT_HFG_WARN_INVALID_INDICATOR_VAL, Ind, g_indicatorRange[Ind][0], g_indicatorRange[Ind][1], Value);
                    status = BT_STATUS_INVALID_PARM;
                }
                else if ( Channel->indEnabled ) 
                {
                    /* Send the indicator */
                    Response->type = AT_INDICATOR_EVENT;
                    Response->p.hf.event.indicator = Channel->IndIndex[Ind];
                    //Response->p.hf.event.indicator = Ind;
                    Response->p.hf.event.value = Value;

                    status = AtSendResults(Channel, Response, FALSE);
                    DBG_PRINT_AT(("[HFG][AT] Auto OK response : No"));
                }
				else
				{
					kal_trace(TRACE_GROUP_5, BT_HFG_WARN_INDICATOR_REPORT_DISABLED);
            }
        }
			else
			{
				kal_trace(TRACE_GROUP_5, BT_HFG_WARN_INVALID_INDICATOR, Ind);
			}
	    	}
	}
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
    }

    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_set_indicator_value : return status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            hfg_get_indicator_value()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets the current value for an indicator.
 *
 * Return:    (See header file)
 */
#if 0
BtStatus hfg_get_indicator_value(HfgChannel *Channel, HfgIndicator Ind, U8 *Value)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_get_indicator_value(Channel=0x%X, Ind=%d, *Value=0x%X)", 
                                (U32)Channel,
                                Ind,
                                (U32)Value));
    Report(("+ hfg_get_indicator_value: Ind=%d.", Ind));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    OS_LockStack();

    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if(Channel->type == AG_TYPE_HS)
        {
            status = BT_STATUS_NOT_SUPPORTED;
        }
        else if (HfgGetState(Channel) >= HFG_STATE_OPEN)
        {
            status = BT_STATUS_NO_CONNECTION;
        }
        else 
        {
            status = BT_STATUS_SUCCESS;

            /* Set the indicator */
            switch (Ind) {
            case HFG_IND_SERVICE:
                *Value = Channel->service;
                break;
            case HFG_IND_CALL:
                *Value = Channel->call;
                break;
            case HFG_IND_CALL_SETUP:
                *Value = Channel->callSetup;
                break;
            case HFG_IND_CALL_HELD:
                *Value = Channel->held;
                break;
            case HFG_IND_BATTERY:
                *Value = Channel->battery;
                break;
            case HFG_IND_SIGNAL:
                *Value = Channel->signal;
                break;
            default:
                status = BT_STATUS_INVALID_PARM;
                break;
            }
        }
    }

    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_get_indicator_value : return status=%d, return Value=%d", status, *Value));
    return status;
}
#endif

/*---------------------------------------------------------------------------
 *            hfg_send_hf_results()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Generic routine for sending results.
 *
 * Return:    (See header file)
 */
BtStatus hfg_send_hf_results(HfgChannel *Channel, AtCommand Command, 
                           U32 Parms, U16 ParmLen, HfgResponse *Response,
                           BOOL done)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U16 cmdGrp = ((Command>>8)&0xF);

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

    DBG_PRINT_API(("[HFG][API] + hfg_send_hf_results(Channel=0x%X, Command=0x%X, Params=0x%X, ParmLen=%d, Response=0x%X, done=%d)", 
                                (U32)Channel,
                                Command,
                                Parms,
                                ParmLen,
                                (U32)Response,
                                done?1:0
                                ));
    kal_trace( TRACE_GROUP_5, BT_HFG_API_SEND_RESULT, Channel, Command, Parms, ParmLen, Response, done ? 1 : 0);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    if(Channel->bSuspended)
    {
        /* Shall never be here */
        bt_prompt_trace(MOD_BT, "Channel is suspended");
        return BT_STATUS_RESTRICTED;
    }
#endif

    OS_LockStack();

    if (IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        /* AG has to negotiation with HF in open state */
		if (HfgGetState(Channel) >= HFG_STATE_OPEN) 
		{
            Response->type = Command;
            /* Set up the repsonse parm */
			switch (Command) 
			{
            case AT_ERROR:
            case AT_MICROPHONE_GAIN:
            case AT_SPEAKER_GAIN:
                Response->p.parm8 = (U8)Parms;
                break;
            case AT_IN_BAND_RING_TONE:
            case AT_VOICE_RECOGNITION:
                Response->p.parmB = (BOOL)Parms;
                break;
            case AT_VOICE_TAG:
                Response->p.hf.voiceTag.number = (const char *)Parms;
                break;
            case AT_RAW:
                Response->p.data = (U8 *)Parms;
                Report( ("[HFG][hfg_send_hf_results] Received L4 result==%s", (S8*)(Response->p.data)) );
                break;
            default:
                if (Parms) 
                {
                    switch(cmdGrp)
                    {
                    case 0: /* Common cmd */
        			    if(ParmLen == 0)
		        	    {
		        	    	// copy value
        			    	OS_MemCopy((U8*)&Response->p, (const U8 *)&Parms, 4);
		        	    }
			            else
    		    	    {
	    	    	    	// copy structure
                        if ( ParmLen <= sizeof(Response->p) )    //klocwork warning
                        	OS_MemCopy((U8*)&Response->p, (const U8 *)Parms, ParmLen);
		        	    }			    
			            break;
                    case 1: /* Headset cmd */
                        if ( ParmLen <= sizeof(Response->p.hs) )    //klocwork warning
                        OS_MemCopy((U8*)&Response->p.hs, (const U8 *)Parms, ParmLen);
                        break;
                    case 2: /* Handsfreet cmd */
                        if ( ParmLen <= sizeof(Response->p.hf) )    //klocwork warning
                        OS_MemCopy((U8*)&Response->p.hf, (const U8 *)Parms, ParmLen);
                        break;
                    case 3: /* PB cmd */
                        if ( ParmLen <= sizeof(Response->p.pb) )    //klocwork warning
                        OS_MemCopy((U8*)&Response->p.pb, (const U8 *)Parms, ParmLen);
                        break;
                    case 4: /* SMS cmd */
        			    if(ParmLen == 0)
		        	    {
		        	    	// copy value
			            	OS_MemCopy((U8*)&Response->p.sms, (const U8 *)&Parms, 4);
        			    }
		        	    else
		    	        {
		    	        	// copy structure
                        if ( ParmLen <= sizeof(Response->p.sms) )    //klocwork warning
                            OS_MemCopy((U8*)&Response->p.sms, (const U8 *)Parms, ParmLen);
    		    	    }
                        break;
                    case 5: /* DUN cmd */
                        status = BT_STATUS_NOT_SUPPORTED;
                        break;
                    }
                }
				break;
            }

            switch (Command) 
            {
            case AT_LIST_CURRENT_CALLS:
                if (Response->p.hf.currentCalls.state == HFG_CALL_STATUS_NONE) 
                {
                    if (done) 
                    {
                        Response->type = AT_OK;
                        done = FALSE;
                    }
                    else 
                    {
                        status = BT_STATUS_SUCCESS;
                        goto exit;
                    }
                }
                break;
            case AT_CALL_ID:
                if (!(Channel->callId)) 
                {
                    status = BT_STATUS_NOT_SUPPORTED;
                }
                break;
	     case AT_CALL_WAIT_NOTIFY:
		  if (!(Channel->callWaiting)) 
                {
                    status = BT_STATUS_NOT_SUPPORTED;
                }		  	
	 	  break;
            case AT_ERROR:
                if (Channel->extendedErrors) 
                {
                    Response->type = AT_EXTENDED_ERROR;
                }
                break;
            }

            if(status == BT_STATUS_SUCCESS)
            {
                /* Send the response */
                status = AtSendResults(Channel, Response, done);
                if (status == BT_STATUS_PENDING) 
                {
                    if(done)
                    {
                        DBG_PRINT_AT(("[HFG][AT] Auto OK response : Yes"));
                        DBG_PRINT_AT(("[HFG][AT] Set auto OK flag"));
                        #if 1
                        Response->flag |= AT_RESULT_FLAG_AUTO_OK;
                        #else
                        Channel->flags |= CHANNEL_FLAG_SEND_OK;
                        #endif
                    }
                    else
                    {
                        DBG_PRINT_AT(("[HFG][AT] Auto OK response : No"));
                    }
                    
                    switch (Response->type) 
                    {
                    case AT_IN_BAND_RING_TONE:
                        Channel->ibRing = Response->p.hf.inBandRing.enabled;
                        break;
                    case AT_VOICE_RECOGNITION:
                        Channel->voiceRec = Response->p.hf.vrec.enabled;
                        break;
                    }
                }
                else
                {
                }
            }
        }
        else 
        {
        	Report(("hfg_send_hf_results: No connection."));
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN_NO_CONNECTION, Channel);
            status = BT_STATUS_NO_CONNECTION;
        }
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
	}

exit:
    OS_UnlockStack();
    DBG_PRINT_API(("[HFG][API] - hfg_send_hf_results : return status=%d", status));
    return status;
}


/*---------------------------------------------------------------------------
 *             hfg_is_nrec_enabled()
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Returns TRUE if Noise Reduction and Echo Cancelling is enabled in 
 *             the audio gateway.
 *
 * Return:     (See header file);
 */
#if 0
BOOL hfg_is_nrec_enabled(HfgChannel *Channel)
{
    BOOL enabled = FALSE;

    DBG_PRINT_API(("[HFG][API] + hfg_is_nrec_enabled(Channel=0x%X)", (U32)Channel));

    OS_LockStack();
    if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if (HfgGetState(Channel) >= HFG_STATE_SLC) 
        {
            enabled = !Channel->nrecDisable;
        }
    }
    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_is_nrec_enabled : return %s", enabled?"TRUE":"FALSE"));
    return enabled;
}
#endif
/*---------------------------------------------------------------------------
 *             hfg_is_inbandring_Enabled()
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Returns TRUE if In-band Ringing is is enabled in the audio 
 *             gateway.
 *
 * Return:     (See header file);
 */
#if 0
BOOL hfg_is_inbandring_Enabled(HfgChannel *Channel)
{
    BOOL enabled = FALSE;

    DBG_PRINT_API(("[HFG][API] + hfg_is_inbandring_Enabled(Channel=0x%X)", (U32)Channel));

    OS_LockStack();
    if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) {
        if (HfgGetState(Channel) >= HFG_STATE_SLC) {
            enabled = Channel->ibRing;
        }
    }
    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_is_inbandring_Enabled : return %s", enabled?"TRUE":"FALSE"));
    return enabled;
}
#endif
/*---------------------------------------------------------------------------
 *             hfg_is_callidnotify_enabled()
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Returns TRUE if Caller ID notification is enabled in the audio 
 *             gateway.
 *
 * Return:     (See header file);
 */
#if 0
BOOL hfg_is_callidnotify_enabled(HfgChannel *Channel)
{
    BOOL enabled = FALSE;

    DBG_PRINT_API(("[HFG][API] + hfg_is_callidnotify_enabled(Channel=0x%X)", (U32)Channel));

    OS_LockStack();
    if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) {
        if (HfgGetState(Channel) >= HFG_STATE_SLC) {
            enabled = Channel->callId;
        }
    }
    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_is_callidnotify_enabled : return %s", enabled?"TRUE":"FALSE"));
    return enabled;
}
#endif

/*---------------------------------------------------------------------------
 *             hfg_is_vr_active()
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Returns TRUE if Voice Recognition is active in the audio gateway.  
 *
 * Return:     (See header file);
 */
#if 0
BOOL hfg_is_vr_active(HfgChannel *Channel)
{
    BOOL active = FALSE;

    DBG_PRINT_API(("[HFG][API] + hfg_is_vr_active(Channel=0x%X)", (U32)Channel));

    OS_LockStack();
    if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) {
        if (HfgGetState(Channel) >= HFG_STATE_SLC) {
            active = Channel->voiceRec;
        }
    }
    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_is_vr_active : return %s", active?"TRUE":"FALSE"));
    return active;
}
#endif
/*---------------------------------------------------------------------------
 *             hfg_is_callwaiting_active()
 *---------------------------------------------------------------------------
 *
 * Synopsis:   Returns TRUE if Call Waiting is active in the audio gateway.  
 *
 * Return:     (See header file);
 */
#if 0
BOOL hfg_is_callwaiting_active(HfgChannel *Channel)
{
    BOOL active = FALSE;

    DBG_PRINT_API(("[HFG][API] + hfg_is_callwaiting_active(Channel=0x%X)", (U32)Channel));

    OS_LockStack();

    if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) 
    {
        if (HfgGetState(Channel) >= HFG_STATE_SLC) 
        {
            active = Channel->callWaiting;
        }
    }

    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_is_callwaiting_active : return %s", active?"TRUE":"FALSE"));
    return active;
}
#endif
#if HFG_SNIFF_TIMER >= 0
/*---------------------------------------------------------------------------
 *             hfg_enable_sniff_mode
 *---------------------------------------------------------------------------
 *
 * Synopsis:    Enables/Disables placing link into sniff mode on the specified 
 *              channel
 */
BtStatus hfg_enable_sniff_mode(HfgChannel *Channel, BOOL Enable) 
{
    BtSniffInfo sniffInfo;
    BtStatus    status = BT_STATUS_NOT_FOUND;

    DBG_PRINT_API(("[HFG][API] + hfg_enable_sniff_mode(Channel=0x%X, Enable=%d)", (U32)Channel, Enable?1:0));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Channel != 0);

	kal_trace( TRACE_GROUP_5, BT_HFG_API_ENABLE_SNIFF_MODE, Channel, Enable ? 1 : 0);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    OS_LockStack();

	if ((Channel) && IsNodeOnList(&HFG(channelList), &Channel->node)) 
	{
		if (Enable) 
		{
            if (!(Channel->flags & CHANNEL_FLAG_SNIFFING) && 
                	(HfgGetState(Channel) >= HFG_STATE_OPEN)) 
                	{
                /* Start the sniff timer */
                sniffInfo.minInterval = HFG_SNIFF_MIN_INTERVAL;
                sniffInfo.maxInterval = HFG_SNIFF_MAX_INTERVAL;
                sniffInfo.attempt = HFG_SNIFF_ATTEMPT;
                sniffInfo.timeout = HFG_SNIFF_TIMEOUT;
				CMGR_SetSniffTimer(&Channel->cmgrHandler, &sniffInfo, HFG_SNIFF_TIMER);
            }
        	}
		else 
		{
            if ((Channel->flags & CHANNEL_FLAG_SNIFFING) && 
				(HfgGetState(Channel) >= HFG_STATE_OPEN)) 
			{
                CMGR_ClearSniffTimer(&Channel->cmgrHandler);
            }
        }

        /* Set the internal state */
		if (Enable) 
		{
            Channel->flags |= CHANNEL_FLAG_SNIFFING;
		}
		else 
		{
            Channel->flags &= ~CHANNEL_FLAG_SNIFFING;
        }

        status = BT_STATUS_SUCCESS;
    }
	else
	{
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_CHANNEL_NOT_REGISTERED, Channel);
	}

    OS_UnlockStack();

    DBG_PRINT_API(("[HFG][API] - hfg_enable_sniff_mode : return status=%d", status));
    return status;
}

#if 0
BtStatus hfg_tx(HfgChannel *Channel, U8 *data, U16 length)
{
    Channel->bytesToSend = length;
    Channel->atTxPacket.dataLen = length;
    Channel->atTxPacket.data = data;
    return AtSendRfPacket(Channel, &Channel->atTxPacket);
}
#endif
#endif
