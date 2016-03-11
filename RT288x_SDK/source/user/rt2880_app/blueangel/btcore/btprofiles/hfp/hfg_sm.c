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
 *     $Workfile:hfg_sm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the connection state machine for the 
 *     hands-free component of the Hands-free SDK.
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
*****************************************************************************/
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
 * Jun 16 2009 mtk80029
 * [MAUI_01869480] [BT HFP] revise code related to audio connection request
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/
#include "stdio.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"
#include "rfcomm_adp.h"
#include "conmgr.h"

#define __BT_FILE__	BT_FILE_HFG_SM

#ifdef __BT_HFG_PROFILE__
#if defined( BTMTK_ON_WISESDK ) || defined( BTMTK_ON_WIN32 ) || defined(BTMTK_ON_LINUX)
U32 bt_hfg_features =( 
					   #if (defined(VRSI_ENABLE) || defined(VR_ENABLE)) 
                       HFG_FEATURE_VOICE_RECOGNITION    | 
                       #endif
			   HFG_FEATURE_VOICE_TAG |
			   HFG_FEATURE_ECHO_NOISE |
                       #ifdef __BT_INBAND_RING__
                       HFG_FEATURE_RING_TONE            | 
                       #endif
                       #ifdef __HF_V15__
                       HFG_FEATURE_ENHANCED_CALL_STATUS | 
                       HFG_FEATURE_ENHANCED_CALL_CTRL   | 
                       HFG_FEATURE_EXTENDED_ERRORS      |
                       #endif
                       HFG_FEATURE_REJECT               | 
					   HFG_FEATURE_THREE_WAY_CALLS);

U32 bt_hfg_get_supported_feature(void)
{
	return bt_hfg_features;

}

BOOL bt_hfg_check_supported_feature(U32 feature_flag)
{

	if ( bt_hfg_get_supported_feature() & feature_flag )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif /* defined( BTMTK_ON_WISESDK ) || defined( BTMTK_ON_WIN32 ) */
kal_bool hfga_is_profile_support_esco(void)
{
	return TRUE;
}
#endif

/*---------------------------------------------------------------------------
 *            HfgPacketHldr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle packet sending
 *
 * Return:    void
 */
#if defined(USE_EXT_AT_PARSER)
void HfgPacketHldr(HfgChannel *Channel, U8 Event, AtCommands *Parms)
{
    BtStatus    status;
    AtResults  *results;
//    BOOL        bAutoOK = FALSE;
    (void)results;

    Report(("+ HfgPacketHldr: Event=%d.", Event));

    /* Preinitialize the OK response */
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    Channel->atResults.type = AT_OK;
    results = &Channel->atResults;

    switch (Event) 
    {
    case EVENT_AT_DATA:
        Report(("HfgPacketHldr: EVENT_AT_DATA case, Parms->type=%d.", Parms->type));
        switch (Parms->type) 
        {
        case AT_CHECK_ALIVE:
		break;
        case AT_SUPPORTED_FEATURES:
            /* Report the hands-free device features */
            Channel->hfFeatures = Parms->p.hf.features.bitmap;
            Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
            Channel->cmgrHandler.errCode = BEC_NO_ERROR;
            break;

        case (AT_INDICATORS | AT_READ):
            if (!(Channel->flags & CHANNEL_FLAG_FEATURES_SENT)) 
            {
                DBG_PRINT_FLOW(("[HFG][FLOW] AT+BRSF is not received"));
                /* FLOW : AT+BRSF is not received */
                if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN1)
                {
                    if (!(Channel->flags & CHANNEL_FLAG_OUTGOING)) 
                    {
                        status = HfgStartServiceQuery(Channel, BSQM_FIRST);
                        if (status == BT_STATUS_PENDING) 
                        {
                            HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN2);
                        }
                        else
                        {
                            /* Can't start the query */
                            HfgDisconnecting(Channel);
                        }
                    } 
                }
                else
                {
                    Report(("HfgPacketHldr: Unexpected case in (AT_INDICATORS | AT_READ)."));
                }
            }
            break;

        case AT_EVENT_REPORTING:
            if (Parms->p.hf.report.mode == 3) {
                Channel->indEnabled = Parms->p.hf.report.ind;
            }
            #ifdef __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if ( !(Channel->hfFeatures & HFG_HANDSFREE_FEATURE_CALL_WAITING) &&
                    (Channel->flags & CHANNEL_FLAG_NEGOTIATE)) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            else
            #endif
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            break;

        case (AT_CALL_HOLD | AT_TEST):
            #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            #endif
            break;

        default:
            break;
        }
        break;

    case EVENT_RF_PACKET_SENT:
        Report(("HfgPacketHldr: EVENT_RF_PACKET_SENT case."));
        /* Results sent internally */
        #if 1
        HfgCheckNegotiateDone(Channel);
        #else
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
        #endif
        return;
    default:
        return;
    }
    // Pass AT CMD to Ext adp
    {
        HfgAtData atData;
        atData.dataLen = Channel->atRxLen;
        atData.rawData = Channel->atRxBuffer;
        HfgAppCallback(Channel, HFG_EVENT_AT_COMMAND_DATA, 
                                    BT_STATUS_SUCCESS, (U32)&atData);
    }
}
#else
void HfgPacketHldr(HfgChannel *Channel, U8 Event, AtCommands *Parms)
{
    BtStatus    status;
    AtResults  *results;
    BOOL        bAutoOK = FALSE;

    Report(("+ HfgPacketHldr: Event=%d.", Event));

    /* Preinitialize the OK response */
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    Channel->atResults.type = AT_OK;
    results = &Channel->atResults;

    switch (Event) 
    {
    case EVENT_AT_DATA:
        Report(("HfgPacketHldr: EVENT_AT_DATA case, Parms->type=%d.", Parms->type));
        switch (Parms->type) 
        {
        case AT_CHECK_ALIVE:
		break;
        //brad - service level connection establishment start
        case AT_SUPPORTED_FEATURES:
            /* Respond with the supported features */
            /* Channel->flags |= CHANNEL_FLAG_SEND_OK;*/
            bAutoOK = TRUE;
            Channel->atResults.type = AT_SUPPORTED_FEATURES;
		#ifdef __BT_HFG_PROFILE__
            Channel->atResults.p.hf.features.bitmap = bt_hfg_get_supported_feature();  //HFG_SDK_FEATURES;
		#else
            Channel->atResults.p.hf.features.bitmap = 
				#ifdef __BT_INBAND_RING__
				HFG_FEATURE_RING_TONE            | 
				#endif
				#ifdef __HF_V15__
				HFG_FEATURE_ENHANCED_CALL_STATUS | 
				HFG_FEATURE_ENHANCED_CALL_CTRL   | 
				HFG_FEATURE_EXTENDED_ERRORS      |
				#endif
				HFG_FEATURE_REJECT               | 
				HFG_FEATURE_THREE_WAY_CALLS;
		#endif
            /* Report the hands-free device features */
            Channel->hfFeatures = Parms->p.hf.features.bitmap;
            Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
            Channel->cmgrHandler.errCode = BEC_NO_ERROR;
            HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, 
                           BT_STATUS_SUCCESS, 
                           (U32)Channel->hfFeatures);
            break;

        case (AT_INDICATORS | AT_TEST):
            HfgSetupIndicatorTestRsp(Channel);
            bAutoOK = TRUE;
            break;

        case (AT_INDICATORS | AT_READ):
            if (!(Channel->flags & CHANNEL_FLAG_FEATURES_SENT)) 
            {
                DBG_PRINT_FLOW(("[HFG][FLOW] AT+BRSF is not received"));
                /* FLOW : AT+BRSF is not received */
                if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN1)
                {
                    if (!(Channel->flags & CHANNEL_FLAG_OUTGOING)) 
                    {
                        status = HfgStartServiceQuery(Channel, BSQM_FIRST);
                        if (status == BT_STATUS_PENDING) 
                        {
                        		HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN2);
                        }
                        else
                        {
                            /* Can't start the query */
                            HfgDisconnecting(Channel);
                        }
                } 
                else 
                {
                    Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
                    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
                    HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, 
                                   BT_STATUS_SUCCESS, 
                                   (U32)Channel->hfFeatures);
                }
            }
            else
            {
                Report(("HfgPacketHldr: Unexpected case in (AT_INDICATORS | AT_READ)."));
            }

            if (!(Channel->flags & CHANNEL_FLAG_OUTGOING)) 
            {
                /* Supported features are not known, and SDP query is necessary */
                if (HfgStartServiceQuery(Channel, BSQM_FIRST) != BT_STATUS_PENDING) 
                {
                    /* Can't start the query */
                    /*(void)RF_CloseChannel(Channel->rfChannel);*/
                    HfgDisconnecting(Channel);
                }
            }
            else
            {
                Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
                Channel->cmgrHandler.errCode = BEC_NO_ERROR;
                HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, 
                                       BT_STATUS_SUCCESS, 
                                       (U32)Channel->hfFeatures);
            }
        }

        #ifdef HFG_SELF_HANDLE_READ_INDICATOR
        HfgSetupIndicatorReadRsp(Channel);
        break;
        #else
        HfgAppCallback(Channel, HFG_EVENT_READ_INDICATORS/*HFG_EVENT_AT_COMMAND_DATA*/, 
        BT_STATUS_SUCCESS, (U32)0);
        return;
        #endif

        case AT_EVENT_REPORTING:

            if (Parms->p.hf.report.mode == 3) {
                Channel->indEnabled = Parms->p.hf.report.ind;
            }

            #ifdef __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if ( !(Channel->hfFeatures & HFG_HANDSFREE_FEATURE_CALL_WAITING) &&
                    (Channel->flags & CHANNEL_FLAG_NEGOTIATE)) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            else
            #endif
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            break;

        case (AT_CALL_HOLD | AT_TEST):
            HfgSetupCallHoldReadRsp(Channel);
            if ( Channel->atResults.type == (AT_CALL_HOLD | AT_TEST))
                bAutoOK = TRUE;
            #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            #endif
            break;
            //brad - service level connection establishment end

        case AT_ANSWER:
            HfgAppCallback(Channel, HFG_EVENT_ANSWER_CALL, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_DIAL_NUMBER:
            HfgAppCallback(Channel, HFG_EVENT_DIAL_NUMBER, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dial.number);
            return;

        case AT_DIAL_MEMORY:
            HfgAppCallback(Channel, HFG_EVENT_MEMORY_DIAL, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dial.number);
            return;

        case AT_REDIAL:
            HfgAppCallback(Channel, HFG_EVENT_REDIAL, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_CALL_HOLD:
            HfgAppCallback(Channel, HFG_EVENT_CALL_HOLD, 
                           BT_STATUS_SUCCESS, (U32)&Parms->p.hf.hold);
            return;
            
        case AT_RESPONSE_AND_HOLD | AT_READ:
            HfgAppCallback(Channel, HFG_EVENT_QUERY_RESPONSE_HOLD, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_RESPONSE_AND_HOLD:
            HfgAppCallback(Channel, HFG_EVENT_RESPONSE_HOLD, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.btrh.setting);
            return;

        case AT_HANG_UP:
            HfgAppCallback(Channel, HFG_EVENT_HANGUP, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_GENERATE_DTMF_TONE:
            HfgAppCallback(Channel, HFG_EVENT_GENERATE_DTMF, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dtmf.tone);
            return;

        case AT_VOICE_TAG:
		#if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_VOICE_TAG))
            {
                HfgAppCallback(Channel, HFG_EVENT_GET_LAST_VOICE_TAG, 
                                        BT_STATUS_SUCCESS, (U32)0);
			return;
            }
            else
		#endif
            {
                results->type = AT_ERROR;
                results->p.error.type = 0;
            }
            break;


        case AT_VOICE_RECOGNITION:
		#if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_VOICE_RECOGNITION))
            {
                Channel->voiceRec = Parms->p.hf.vrec.enabled;
                HfgAppCallback(Channel, HFG_EVENT_ENABLE_VOICE_RECOGNITION, 
                               BT_STATUS_SUCCESS, (U32)Parms->p.hf.vrec.enabled);
			return;
            }
            else
		#endif
            {
                results->type = AT_ERROR;
                results->p.error.type = 0;
            }                
            break;

        case AT_ECHO_C_AND_NOISE_R:
		#if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_ECHO_NOISE))
            {
                Channel->nrecDisable = TRUE;
                HfgAppCallback(Channel, HFG_EVENT_DISABLE_NREC, 
                               BT_STATUS_SUCCESS, (U32)0);
			return;
            }
            else
		#endif
            {
                results->type = AT_ERROR;
                results->p.error.type = 0;
            }
            break;
            /* L4 will not take care NREC commands, so shall reply OK by HFP */
            //return;

        case AT_LIST_CURRENT_CALLS:
            HfgAppCallback(Channel, HFG_EVENT_LIST_CURRENT_CALLS, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_CALL_ID:
            Channel->callId = Parms->p.hf.callId.enabled;
            HfgAppCallback(Channel, HFG_EVENT_ENABLE_CALLER_ID, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.callId.enabled);
            return;

        case AT_CALL_WAIT_NOTIFY:
            Channel->callWaiting = Parms->p.hf.wait.notify;
            HfgAppCallback(Channel, HFG_EVENT_ENABLE_CALL_WAITING, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.wait.notify);
            return;
            
        case AT_SUBSCRIBER_NUM:
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUBSCRIBER_NUMBER, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_NETWORK_OPERATOR:
            if ((Parms->p.hf.networkOper.mode == 3) &&
                (Parms->p.hf.networkOper.format == 0)) 
            {
            } 
            else
            {
                if (Channel->extendedErrors) 
                {
                    Channel->atResults.type = AT_EXTENDED_ERROR;
                    Channel->atResults.p.error.type = ATCME_OP_NOT_SUPPORTED;
                }
                else
                {
                    Channel->atResults.type = AT_ERROR;
                }
            }
            break;
            
        case (AT_NETWORK_OPERATOR | AT_READ):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_NETWORK_OPERATOR, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_SET_ERROR_MODE:
		#if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_EXTENDED_ERRORS))
            {
                if (Parms->p.error.mode == 1) 
                {
                    Channel->extendedErrors = TRUE;
                    HfgAppCallback(Channel, HFG_EVENT_ENABLE_EXTENDED_ERRORS, 
                                   BT_STATUS_SUCCESS, (U32)0);
                    return;
                    
                }
                else
                {
                    if (Channel->extendedErrors) 
                    {
                        Channel->atResults.type = AT_EXTENDED_ERROR;
                        Channel->atResults.p.error.type = ATCME_OP_NOT_SUPPORTED;
                    }
                    else
                    {
                        Channel->atResults.type = AT_ERROR;
                    }
                }
            }
            else
		#endif
            {
                results->type = AT_ERROR;
                results->p.error.type = 0;
            }
            break;

        case AT_MICROPHONE_GAIN:
            HfgAppCallback(Channel, HFG_EVENT_REPORT_MIC_VOLUME, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.mic.gain);
            break;
            /* L4 will not take care VGS and VGM commands, so shall reply OK by HFP */
			//return;

        case AT_SPEAKER_GAIN:
            HfgAppCallback(Channel, HFG_EVENT_REPORT_SPK_VOLUME, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.speaker.gain);
            break;

        case AT_KEYPAD_CONTROL:
            HfgAppCallback(Channel, HFG_EVENT_KEYPAD_CONTROL, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.keypad.button);
            break;
            
	case AT_SELECT_CHARACTER_SET:
	{
		#if 1
		HfgAppCallback(Channel, HFG_EVENT_SELECT_CHARSET, BT_STATUS_SUCCESS, (U32)Parms->p.charSet.type);
		return;
		#else
		HfgCHarsetType charsetType;
		status = HfgDecodeCharset(Channel, Parms->p.charSet.type, &charsetType);
	      if ( status == BT_STATUS_SUCCESS )
     	      {
	     	      Channel->charset = charsetType;
            		HfgAppCallback(Channel, HFG_EVENT_CHARSET_SELECTED, 
                           		BT_STATUS_SUCCESS, (U32)charsetType);
	     	}
		 else
	 	{
			Channel->atResults.type = AT_ERROR;
 			results->p.error.type = 0;
	 	}
		 break;
		#endif
	}
	case (AT_SELECT_CHARACTER_SET|AT_TEST):
		#if 1
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_CHARSET, BT_STATUS_SUCCESS, 0);
		return;
		#else
		if ( BT_STATUS_SUCCESS == HfgSetupCharsetTestRsp(Channel) )
		{
			Channel->atResults.type = AT_SELECT_CHARACTER_SET|AT_TEST;
			bAutoOK = TRUE;
		}
		else
		{
			Channel->atResults.type = AT_ERROR;
 			results->p.error.type = 0;
		}
		break;
		#endif
	case (AT_SELECT_CHARACTER_SET|AT_READ):
		#if 1
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_CHARSET, BT_STATUS_SUCCESS, 0);
		return;
		#else
		results->p.charset = HfgGetCharsetDesc(Channel, Channel->charset);
		if(results->p.charset)
		{
			Channel->atResults.type = AT_SELECT_CHARACTER_SET|AT_READ;
			bAutoOK = TRUE;
		}
		else
		{
			Channel->atResults.type = AT_ERROR;
 			results->p.error.type = 0;
		}
		break;
		#endif

	case AT_MANUFACTURE_ID:
		#if 1
		HfgAppCallback(Channel, HFG_EVENT_QUERY_MANUFACTURE_ID, BT_STATUS_SUCCESS, 0);
		return;
		#else
		results->p.manufactureID = HfgGetManufactureID(Channel);
		Channel->atResults.type = AT_MANUFACTURE_ID;
		bAutoOK = TRUE;
		break;
		#endif
		
	case AT_MODEL_ID:
		#if 1
		HfgAppCallback(Channel, HFG_EVENT_QUERY_MODEL_ID, BT_STATUS_SUCCESS, 0);
		return;
		#else		
		results->p.modelID = HfgGetModelID(Channel);
		Channel->atResults.type = AT_MODEL_ID;
		bAutoOK = TRUE;
		break;
		#endif
            /* L4 will not take care VGS and VGM commands, so shall reply OK by HFP */
			//return;
        /***********************************************************
        *   Phonebook indication
        ***********************************************************/
        case (AT_SELECT_PHONEBOOK_STORAGE|AT_TEST):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_PHONEBOOK, 
                                    BT_STATUS_SUCCESS, 0);
            return;
            
        case AT_SELECT_PHONEBOOK_STORAGE:
            HfgAppCallback(Channel, HFG_EVENT_SELECT_PHONEBOOK, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.pb.storage.select);
            return;
            
        case (AT_SELECT_PHONEBOOK_STORAGE|AT_READ):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_PHONEBOOK, 
                           BT_STATUS_SUCCESS, 0);
            return;

        case (AT_READ_PHONEBOOK_ENTRY|AT_TEST):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_READ_PBENTRY_INFO, 
               BT_STATUS_SUCCESS, 0);
            return;

        case AT_READ_PHONEBOOK_ENTRY:
            HfgAppCallback(Channel, HFG_EVENT_READ_PBENTRY, 
               BT_STATUS_SUCCESS, (U32)&Parms->p.pb.read);
            return;

        case (AT_FIND_PHONEBOOK_ENTRY|AT_TEST):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_FIND_PBENTRY_INFO, 
               BT_STATUS_SUCCESS, 0);
            return;

        case AT_FIND_PHONEBOOK_ENTRY:
            HfgAppCallback(Channel, HFG_EVENT_FIND_PBENTRY, 
               BT_STATUS_SUCCESS, (U32)&Parms->p.pb.find);
            return;

        case (AT_WRITE_PHONEBOOK_ENTRY|AT_TEST):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_WRITE_PBENTRY_INFO, 
               BT_STATUS_SUCCESS, 0);
            return;

        case AT_WRITE_PHONEBOOK_ENTRY:
            HfgAppCallback(Channel, HFG_EVENT_WRITE_PBENTRY, 
               BT_STATUS_SUCCESS, (U32)&Parms->p.pb.write);
            return;
			            
        /***********************************************************
        *   SMS indication
        ***********************************************************/
	case (AT_SELECT_SMS_SERVICE|AT_TEST):	/* AT+CSMS=? */
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_SMS_SERVICE, 
               BT_STATUS_SUCCESS, 0);
            return;
	case (AT_SELECT_SMS_SERVICE|AT_READ):	/* AT+CSMS? */
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_SMS_SERVICE, 
               BT_STATUS_SUCCESS, 0);
		return;
	case AT_SELECT_SMS_SERVICE:				/* AT+CSMS=<service> */
            HfgAppCallback(Channel, HFG_EVENT_SELECT_SMS_SERVICE, 
               BT_STATUS_SUCCESS, (U32)Parms->p.sms.service);
		return;
	case (AT_PREFERRED_SMS_STORAGE|AT_TEST):	/* AT+CPMS=? */
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_PREF_MSG_STORAGE, 
               BT_STATUS_SUCCESS, 0);
		return;
	case (AT_PREFERRED_SMS_STORAGE|AT_READ):	/* AT+CPMS? */
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE,
               BT_STATUS_SUCCESS, 0);
		return;
	case AT_PREFERRED_SMS_STORAGE:				/* AT+CPMS=<mem1>[,<mem2>[,<mem3>]] */
            HfgAppCallback(Channel, HFG_EVENT_SELECT_PREF_MSG_STORAGE,
               BT_STATUS_SUCCESS, (U32)&Parms->p.sms.preferred);
		return;
	case (AT_SMS_MESSAGE_FORMAT|AT_TEST):		/* AT+CMGF=? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_MSG_FORMAT, 
		   BT_STATUS_SUCCESS, 0);
		return;
	case (AT_SMS_MESSAGE_FORMAT|AT_READ):		/* AT+CMGF? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_MSG_FORMAT,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_SMS_MESSAGE_FORMAT:			/* AT+CMGF=<mode> */
		HfgAppCallback(Channel, HFG_EVENT_SELECT_MSG_FORMAT,
		   BT_STATUS_SUCCESS, (U32)Parms->p.sms.format);
		return;
	case (AT_SMS_SERVICE_CENTER|AT_READ):	/* AT+CSCA? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SERVICE_CENTRE,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_SMS_SERVICE_CENTER:				/* AT+CSCA=<sca>[,<tosca>] */
		HfgAppCallback(Channel, HFG_EVENT_SET_SERVICE_CENTRE,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.SCA);
		return;
	case (AT_SET_TEXT_MODE_PARMS|AT_READ):	/* AT+CSMP? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_TEXT_MODE_PARAMS,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_SET_TEXT_MODE_PARMS:			/* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
		HfgAppCallback(Channel, HFG_EVENT_SET_TEXT_MODE_PARAMS,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.textModeParam);
		return;
	case (AT_SMS_SHOW_TEXT_MODE|AT_TEST):	/* AT+CSDH=? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_SHOW_PARAMS,
		   BT_STATUS_SUCCESS, 0);
		return;
	case (AT_SMS_SHOW_TEXT_MODE|AT_READ):	/* AT+CSDH? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_SHOW_PARAMS,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_SMS_SHOW_TEXT_MODE:			/* AT+CSDH=[<show>] */
		HfgAppCallback(Channel, HFG_EVENT_SET_SHOW_PARAMS,
		   BT_STATUS_SUCCESS, (U32)Parms->p.sms.show);
		return;
	case (AT_NEW_MESSAGE_INDICATION|AT_TEST):	/* AT+CNMI=? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_NEW_MSG_INDICATION,
		   BT_STATUS_SUCCESS, 0);
		return;
	case (AT_NEW_MESSAGE_INDICATION|AT_READ):	/* AT+CNMI? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_NEW_MSG_INDICATION,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_NEW_MESSAGE_INDICATION:			/* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
		HfgAppCallback(Channel, HFG_EVENT_SET_NEW_MSG_INDICATION,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.newMsgIndSetting);
		return;
	case (AT_LIST_MESSAGES|AT_TEST):			/* AT+CMGL=? */
		HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_LIST_STATUS,
		   BT_STATUS_SUCCESS, 0);
		return;
	case AT_LIST_MESSAGES:					/* AT+CMGL[=<stat>] */
		HfgAppCallback(Channel, HFG_EVENT_LIST_MSG,
		   BT_STATUS_SUCCESS, (U32)Parms->p.sms.status);
		return;
	case AT_READ_MESSAGE:					/* AT+CMGR=<index> */
		HfgAppCallback(Channel, HFG_EVENT_READ_MSG,
		   BT_STATUS_SUCCESS, (U32)Parms->p.sms.readMsgIndex);		
		return;
	case AT_SEND_MESSAGE:					/* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
											/* PDU MODE : AT+CMGS=<length><CR>PDU is given<ctrl-Z/ESC> */
		HfgAppCallback(Channel, HFG_EVENT_SEND_MSG,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.sendMsg);
		return;
	case AT_SEND_STORED_MESSAGE:			/* AT+CMSS=<index>[,<da>[,<toda>]] */
		HfgAppCallback(Channel, HFG_EVENT_SEND_STORED_MSG,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.storedMsg);
		return;
	case AT_STORE_MESSAGE:					/* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC> */
											/* PDU MODE : AT+CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> */
		HfgAppCallback(Channel, HFG_EVENT_WRITE_MSG,
		   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.writeMsg);
		return;
	case AT_DELETE_MESSAGE:
		HfgAppCallback(Channel, HFG_EVENT_DELETE_MSG,
		   BT_STATUS_SUCCESS, (U32)Parms->p.sms.delMsgIndex);
		return;
        default:
	  #if 0
        {
            HfgAtData at_data;
            at_data.dataLen = Channel->atRxLen;
            at_data.rawData = (U8*)&Channel->atRxBuffer[0];
            HfgAppCallback(Channel, HFG_EVENT_AT_COMMAND_DATA, 
                       BT_STATUS_SUCCESS, (U32)&at_data);
            
            return;
       	}            
	  #else
            if (Channel->extendedErrors) {
                Channel->atResults.type = AT_EXTENDED_ERROR;
                Channel->atResults.p.error.type = ATCME_OP_NOT_SUPPORTED;
            } else {
                Channel->atResults.type = AT_ERROR;
            }
            break;
            #endif    
        }
        break;

    case EVENT_RF_PACKET_SENT:
        Report(("HfgPacketHldr: EVENT_RF_PACKET_SENT case."));
        /* Results sent internally */
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
        return;

    default:
    	Report(("HfgPacketHldr: Unsupported case."));
        Assert(0);
        break;
    }

	results = HfgAllocResponse();
	if(!results)
	{
		status = BT_STATUS_NO_RESOURCES;
		//goto exit;
		HfgDisconnecting(Channel);
	}
	else
	{
		OS_MemCopy((U8*)results, (const U8*)&Channel->atResults, sizeof(HfgResponse));
    status = AtSendResults(Channel, results, bAutoOK);
	}

    if (status != BT_STATUS_PENDING) 
    {
		if(results)
    		HfgFreeResponse(results);
        /* Fatal error sending response, disconnect */
       	/*(void)RF_CloseChannel(Channel->rfChannel);
	       HfgDisconnecting(Channel);*/
    }
    else
    {
        results->flag |= AT_RESULT_FLAG_SELF_HANDLED;
    }
    Report(("- HfgPacketHldr: status=%d.", status));
}
#endif /* USE_EXT_AT_PARSER */
/*---------------------------------------------------------------------------
 *            HfRfCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  RFCOMM callback for the HF state machine.
 *
 * Return:    void
 */
void HfgRfCallback(RfChannel *rfChannel, RfCallbackParms *Parms)
{
    HfgChannel  *Channel;
    U8          event = 0;
    U16         offset = 0;
    BtStatus status = BT_STATUS_SUCCESS;

    Report(("[HFG][HfgRfCallback] +rfChannel==x%X, RfEvent==0x%X", (U32)rfChannel, Parms->event) );

    Channel = HfgFindRegisteredChannel(rfChannel);
    Assert(Channel != 0);
    if ( Channel == NULL )
        return;

	kal_trace(TRACE_GROUP_5, BT_HFG_CBK_RF_CALLBACK, Channel, Parms->event, Parms->status);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
	
    switch (Parms->event) 
    {
    case RFEVENT_OPEN_IND:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_OPEN_IND."));
        if(Channel)
        {
            /* Channel->remDev = Parms->ptrs.remDev; */
	     OS_MemCopy((U8*)Channel->bdAddr.addr, (U8*)Parms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));
            if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CLOSED1)
            {
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_IN1);
            }
            else
            {
                Report(("HfgRfCallback: Only in C1, the server channel is registered."));
                /* Only in C1, the server channel is registered, so there shall no open ind received in other 
                substates other than C1. */
                if( BT_STATUS_PENDING == RF_RejectChannel(rfChannel) )
			Channel->linkFlags |= HFG_LINK_DISCONNECTING;
		
            }
        }
        else
        {
            Report(("HfgRfCallback: RFEVENT_OPEN_IND: HFP shall never get into this case."));
            /* HFP shall never get into this case */
            DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
            (void)RF_RejectChannel(rfChannel);
        }
        break;

    case RFEVENT_OPEN:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_OPEN."));
        /* Register a SCO handler */
        if (Channel) 
        {
            /* Channel->remDev = Parms->ptrs.remDev; */
	     OS_MemCopy((U8*)Channel->bdAddr.addr, (U8*)Parms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));
            CMGR_SetDeviceAsMaster(&(Channel->cmgrHandler));
            HfgEnableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags |= HFG_LINK_HANDSFREE;
	     Channel->linkFlags &= ~(HFG_LINK_CONNECTING|HFG_LINK_DISCONNECTING);
            Report(("HfgRfCallback: RFCOMM HF connection established."));

            switch(HfgGetSubstate(Channel))
            {
            case HFG_SUBSTATE_CONN_OUT3:
            case HFG_SUBSTATE_CONN_IN2:
                DBG_PRINT_FLOW(("[HFG][FLOW] Is headset profile : %s", (Channel->type == AG_TYPE_HS)?"Yes":"No"));
                if(Channel->type == AG_TYPE_HS)
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN1);
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC1);
                }
                else
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN1);
                }
                break;
            case HFG_SUBSTATE_DISC1:
                HfgDisconnecting(Channel);
                break;
            default:
                Report(("HfgRfCallback: RFEVENT_OPEN: HFG shall not get into this case."));
                /* HFG shall not get into this case. */
                DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
                Assert(0);
                /*RF_CloseChannel(rfChannel);*/
                break;
            }
        }
        else
        {
        	Assert(0);
            	/*RF_CloseChannel(rfChannel);*/
        }
        break;
    case RFEVENT_DATA_IND:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_DATA_IND."));
        if (Channel) 
        {
            Report(("HfgRfCallback: RFCOMM HF data received."));
            RF_AdvanceCredit(rfChannel, 1);
            event = EVENT_AT_DATA;
            while (offset < Parms->dataLen) 
            {
                DBG_PRINT_AT(("[HFG][AT] Remain data > 0. Remain len=%d", Parms->dataLen));
                if (HfgParseRfcommData(Channel, Parms, &offset, &Channel->atParms) == BT_STATUS_SUCCESS) 
                {
                    HfgPacketHldr(Channel, event, &Channel->atParms);
			Channel->atParms.type = 0;
                    Channel->atRxLen = 0;
                }
            }
        }
        break;

    case RFEVENT_PACKET_HANDLED:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_PACKET_HANDLED, Status=%d", Parms->status));
        if (Channel) 
        {
            AtResults *result;
            AtCommand cmd;
            status = Parms->status;
            if(status != BT_STATUS_SUCCESS)
            {
                /* Only when RFCOMM channel is closed, it does not return success */
                DBG_PRINT_AT(("[HFG][AT] RFEVENT_PACKET_HANDLED : Fail"));
            }
            else if(IsListEmpty(&Channel->rspQueue))
            {
                DBG_PRINT_AT(("[HFG][AT][ERR] rspQueue empty : Yes"));
                DBG_PRINT_AT(("[HFG][AT] Disconnecting"));
				kal_trace(TRACE_GROUP_5, BT_HFG_INFO_RESPONSE_QUEUE_EMPTY);
                HfgDisconnecting(Channel);
                status = BT_STATUS_FAILED;
            }
            else if(Channel->bytesToSend > 0)
            {
                DBG_PRINT_AT(("[HFG][AT] Bytestosend=%d", Channel->bytesToSend ));
				kal_trace( TRACE_GROUP_5, BT_HFG_INFO_AT_SEND_REMAIN_DATA, Channel->bytesToSend);
                /* Send more data */
                Parms->ptrs.packet->data += Parms->ptrs.packet->dataLen;
                Parms->ptrs.packet->dataLen = Channel->bytesToSend;
                status = AtSendRfPacket(Channel, Parms->ptrs.packet);
                if (status == BT_STATUS_PENDING) 
                {
                    DBG_PRINT_AT(("[HFG][AT] Pending"));
                }
            }
            else
            {
                result = (AtResults*)(Channel->rspQueue.Flink);
                if(result->flag & AT_RESULT_FLAG_AUTO_OK)
                {
                    DBG_PRINT_AT(("[HFG][AT] Auto OK response : Yes"));
                    cmd = result->type;
                    result->type = AT_OK;
                    status = sendResults(Channel, result);
                    result->type = cmd;
                    result->flag &= ~AT_RESULT_FLAG_AUTO_OK;
                }
                else
                {
                    DBG_PRINT_AT(("[HFG][AT] Auto OK response : No"));
                }
            }

            /* If status is pending, it means the result is not completed */
            /* Traverse the rspQueue to send next result. */
            while(status != BT_STATUS_PENDING && !IsListEmpty(&Channel->rspQueue))
            {
                event = EVENT_RF_PACKET_SENT;
                HfgPacketHldr(Channel, event, NULL/*&atParms*/);
                HfgReportPacketCompleted(Channel, status);
                if(IsListEmpty(&Channel->rspQueue))
                {
                    DBG_PRINT_AT(("[HFG][AT] rspQueue empty : Yes"));
                    break;
                }
                else
                {
                    DBG_PRINT_AT(("[HFG][AT] rspQueue empty : No"));
                    result = (AtResults*)(Channel->rspQueue.Flink);
                    status = sendResults(Channel, result);
                }
            }
        } 
        else 
        {
            Report(("HfgRfCallback: RFEVENT_PACKET_HANDLED: No HFG channel."));
            return;
        }
        break;

    case RFEVENT_CLOSED:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_CLOSED"));
        if (Channel) 
        {
            HfgDisableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags &= ~HFG_LINK_HANDSFREE;
	     Channel->linkFlags &= ~(HFG_LINK_CONNECTING|HFG_LINK_DISCONNECTING);
            switch(HfgGetSubstate(Channel))
            {
            case HFG_SUBSTATE_CONN_OUT3:
            case HFG_SUBSTATE_CONN_IN1:
            case HFG_SUBSTATE_CONN_IN2:
            case HFG_SUBSTATE_OPEN1:
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
                break;
            case HFG_SUBSTATE_OPEN2:
            case HFG_SUBSTATE_SLC1:
            case HFG_SUBSTATE_SLC2:
            case HFG_SUBSTATE_DISC1:
                HfgDisconnecting(Channel);
                break;
            default:
                DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
                Assert(0);
                break;
            }
        }
        break;

    default:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM ignore other events : %d", Parms->event));
        /* Ignore other events */
        break;
    }
    Report(("- HfgRfCallback."));
}

/*---------------------------------------------------------------------------
 *            HfgServiceConnectionCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by device manager with link state events.
 *
 * Return:    (See header file)
 *
 */
void HfgServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 
{
    HfgChannel *Channel = ContainingRecord(Handler, HfgChannel, cmgrHandler);
    BtStatus return_code;

    Report(("+ HfgServiceConnectionCallback: Event=%d, Status=%d.", Event, Status));
	kal_trace(TRACE_GROUP_5, BT_HFG_CBK_CMGR_CALLBACK, Channel, Event, Status);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    switch (Event) 
    {
    case CMEVENT_DATA_LINK_CON_CNF:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_DATA_LINK_CON_CNF, Status=%d.", Status));
	// UPdate linkFlags
	if (Status == BT_STATUS_SUCCESS) 
		Channel->linkFlags |= HFG_LINK_ACL;
	else
		Channel->linkFlags &= ~HFG_LINK_ACL;
	
	Channel->linkFlags &= ~HFG_LINK_ACL_CONNECTING;
	
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_OUT1)
        {
            if (Status == BT_STATUS_SUCCESS) 
            {
                //Channel->linkFlags |= HFG_LINK_ACL;
                /* FLOW : Start SDP query */
                if ((return_code = HfgStartServiceQuery(Channel, BSQM_FIRST)) == BT_STATUS_PENDING)
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_OUT2);
                }
                else
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
                }
            }
            else
            {
                /* This event shall not be received in other substates other than C1 */
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
            }
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC1)
        {
            HfgDisconnecting(Channel);
        }
        else
        {
            DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
        }
        break;

        case CMEVENT_DATA_LINK_DIS:
            /* We shall never run into this case */
            DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_DATA_LINK_DIS, Status=%d.", Status));
            DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
            HfgCloseChannel(Channel);
            break;

        case CMEVENT_AUDIO_LINK_CON:
            DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_CON, Status=%d.", Status));
            /* Clear SCO connecting flag */
            Channel->linkFlags &= ~HFG_LINK_SCO_CONNECTING;
	     /*
            HfgAppCallback(Channel, HFG_EVENT_AUDIO_CONNECTED, Status, 
            (U32)&Channel->cmgrHandler.bdc->link->bdAddr);
            */
            if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC1)
            {
                /* Remote connect SCO link */
                /* It could happen in SLC1 but no action needed */
		  /* Only report event when remote request connection success */
		  if(Status == BT_STATUS_SUCCESS)
		  {
		         HfgAppCallback(Channel, HFG_EVENT_AUDIO_CONNECTED, Status,
		         				(U32)&Channel->bdAddr.addr/* Channel->cmgrHandler.bdc->link->bdAddr */);
		  }
            }
            else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC2)
            {
	         HfgAppCallback(Channel, HFG_EVENT_AUDIO_CONNECTED, Status,
	         				(U32)&Channel->bdAddr.addr/*Channel->cmgrHandler.bdc->link->bdAddr*/);
                HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC1);
            }
            else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC1)
            {
                HfgDisconnecting(Channel);
            }
            else
            {
                DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
            }
            break;

        case CMEVENT_AUDIO_LINK_DIS:
            DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_DIS, Status=%d.", Status));
            /* Clear SCO disconnecting flag */
            Channel->linkFlags &= ~HFG_LINK_SCO_DISCONNECTING;
            HfgAppCallback(Channel, HFG_EVENT_AUDIO_DISCONNECTED, Status, 
            (U32)&Channel->bdAddr.addr /*Channel->cmgrHandler.bdc->link->bdAddr*/);

            if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC1)
            {
                /* Remote disconnect SCO link */
                /* It could happen in SLC1 but no action needed */
            }
            else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC2)
            {
                HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC1);
            }
            else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC1)
            {
                HfgDisconnecting(Channel);
            }
            else
            {
                DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
            }
            break;

        #if BT_SCO_HCI_DATA == XA_ENABLED
        case CMEVENT_AUDIO_DATA:
            DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_DATA, Status=%d.", Status));
            HfgAppCallback(Channel, HFG_EVENT_AUDIO_DATA, Status, (U32)&Handler->audioData);
            break;

        case CMEVENT_AUDIO_DATA_SENT:
            DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_DATA_SENT, Status=%d.", Status));
            HfgAppCallback(Channel, HFG_EVENT_AUDIO_DATA_SENT, Status, (U32)&Handler->audioPacket);
            break;
        #endif
    }
    Report(("- HfgServiceConnectionCallback."));
}

