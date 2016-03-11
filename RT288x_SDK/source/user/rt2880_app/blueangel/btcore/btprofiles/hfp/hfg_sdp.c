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
 *     $Workfile:hfg_sdp.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the SDP code for the Hands-free SDK.
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
 * Jun 2 2009 mtk80029
 * [MAUI_01837169] [BT] remove kal_prompt_trace
 * add trace
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

//#include "kal_release.h"
//#include "stack_common.h"
//#include "stack_msgs.h"
//#include "app_ltlcom.h"
//#include "stacklib.h"
//#include "event_shed.h"
//#include "stack_timer.h"
#include "stdio.h"
//#include "bluetooth_trc.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"
#include "sdpdb.h"
#include "sdp.h"
#include "sdap.h"

#define __BT_FILE__	BT_FILE_HFG_SDP

#if SDP_PARSING_FUNCS == XA_DISABLED
#error The Hands-free Gateway SDK requires the SDP parsing functions.
#endif /* SDP_PARSING_FUNCS == XA_DISABLED */

static U16 hfg_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST, 
    AID_BT_PROFILE_DESC_LIST, 
    AID_SUPPORTED_FEATURES,
    AID_SERVICE_CLASS_ID_LIST,
};

static U16 hsag_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST, 
    AID_BT_PROFILE_DESC_LIST,
    //AID_REMOTE_AUDIO_VOL_CONTROL
    AID_SERVICE_CLASS_ID_LIST,
};

static void hfgQueryCallback(	SdpQueryToken *sqt, 
                                       	U8 result, 
                                       	U8 attribute_index,
                                       	U8 *attribute_value );
/* Internal function prototypes */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            HfgRegisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers the SDP services.
 *
 * Return:    See SDP_AddRecord().
 */
BtStatus HfgRegisterSdpServices(BOOL bHeadset)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 type = bHeadset ? AG_TYPE_HS : AG_TYPE_HF;

    DBG_PRINT_FLOW(("[HFG][FLOW] HfgRegisterSdpServices : bHeadset=%d", bHeadset?1:0));
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_REGISTER_SDP, bHeadset ? 1 : 0);

    if(HFG(hfgSdpRecord)[type].num == 0)
    {
		U16 serv = bHeadset ? SC_HEADSET_AUDIO_GATEWAY : SC_HANDSFREE_AUDIO_GATEWAY;
        HFG(hfgSdpRecord)[type].num = SDPDB_GetLocalServiceAttribute(serv,
                                                                    &(HFG(hfgSdpRecord)[type].attribs),
                                                                    &(HFG(hfgService)[type].serviceId));
        if(HFG(hfgSdpRecord)[type].num > 0)
        {
            status = SDP_AddRecord(&(HFG(hfgSdpRecord)[type]));
            if(status != BT_STATUS_SUCCESS)
			{
                DBG_PRINT_ERROR(("[HFG][ERR] SDP_AddRecord fail : %d - (%d,%d)", status, __BT_FILE__, __LINE__));
				kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
			}
        }
        else
        {
            DBG_PRINT_ERROR(("[HFG][ERR] HfgRegisterSdpServices fail - (%d,%d)", __BT_FILE__, __LINE__));
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, HFG(hfgSdpRecord)[type].num);
            Assert(HFG(hfgSdpRecord)[type].num > 0);
            status = BT_STATUS_FAILED;
        }            
    }
    else
    {
        /* Already registered */
        DBG_PRINT_WARN(("[HFG][WARN] HfgRegisterSdpServices : already registered. (%d,%d)", __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN, __BT_FILE__, __LINE__);
    }
    
    Report(("- HfgRegisterSdpServices: status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgDeregisterSdpServices()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters the SDP services.
 *
 * Return:    See SDP_RemoveRecord().
 */
BtStatus HfgDeregisterSdpServices( BOOL bHeadset )
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 type = bHeadset ? AG_TYPE_HS : AG_TYPE_HF;
    
    DBG_PRINT_FLOW(("[HFG][FLOW] HfgDeregisterSdpServices : bHeadset=%d", bHeadset?1:0));
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_DEREGISTER_SDP, bHeadset ? 1 : 0);
    /* Remove the Hands-free entry */
    if(HFG(hfgSdpRecord)[type].num > 0)
    {
        status = SDP_RemoveRecord(&(HFG(hfgSdpRecord)[type]));
        if(status != BT_STATUS_SUCCESS)
		{
            DBG_PRINT_ERROR(("[HFG][ERR] SDP_RemoveRecord fail : %d - (%d,%d)", status, __BT_FILE__, __LINE__));        
			kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_1, __BT_FILE__, __LINE__, status);
		}
    }
    /* As a mark that the SDP record is not registered  */
    /* num and serviceId are all get through HfgRegisterSdpService, so we clear these to 0 */
    HFG(hfgSdpRecord)[type].num = 0;
    HFG(hfgService)[type].serviceId = 0;
    Report(("- HfgDeregisterSdpServices: status=%d", status));
    return status;
}

/*---------------------------------------------------------------------------
 *            HfgStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate the SDP service query.
 *
 * Return:    See SDP_Query().
 */
BtStatus HfgStartServiceQuery(HfgChannel *Channel, SdpQueryMode mode)
{
    BtStatus status;
    BOOL bHeadset;
    sdap_service_search_multi_attribute_struct search_pattern;
    	
    Report(("+ HfgStartServiceQuery: mode=%d", mode));

    bHeadset = (Channel->type == AG_TYPE_HS);

	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_START_SERVICE_QUERY, Channel, mode, bHeadset ? 1 : 0);
	
    /* Reset the query flag */
    Channel->queryFlags = 0x00;
    search_pattern.rm = Channel->cmgrHandler.remDev;
    search_pattern.sqt = &Channel->sdpQueryToken;
    search_pattern.uuid = bHeadset ? SC_HEADSET : SC_HANDSFREE;
    search_pattern.callback = hfgQueryCallback;
    if(bHeadset)
    {
        search_pattern.attribute_num = sizeof(hsag_attribute_id_list)/sizeof(hsag_attribute_id_list[0]);
        search_pattern.attribute_id = hsag_attribute_id_list;
    }
    else
    {
        search_pattern.attribute_num = sizeof(hfg_attribute_id_list)/sizeof(hfg_attribute_id_list[0]);
        search_pattern.attribute_id = hfg_attribute_id_list;        
    }
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);    

    DBG_PRINT_FLOW(("[HFG][FLOW] Start SDP query : %d", status));

    if(status == BT_STATUS_PENDING)
    {
        Channel->queryFlags |= SDP_QUERY_FLAG_ONGOING;
    }
    else
    {
        DBG_PRINT_ERROR(("[HFG][ERR] SDAP_ServiceSearchMultipleAttribute : %d - (%d,%d)", status, __BT_FILE__, __LINE__));
		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
    }

    Report(("- HfgStartServiceQuery: status=%d", status));
    return status;
} 

/*************************************************************************
* FUNCTION                                                            
*	hfgQueryCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*   Modified by MTK Dlight mtk01239
*
* PARAMETERS
*	sqt  : SdpQueryToken contains information such as remote device information. uuid...
*	result  : The service search result.
*      attribute_index: the current request attribute list index
*      attribute_value: points to the search result
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/

static void hfgQueryCallback(	SdpQueryToken *sqt, 
                                       	U8 result, 
                                        U8 attribute_index,
                                       	U8 *attribute_value )
{
    BtStatus status;
    HfgChannel     *Channel = ContainingRecord( sqt, HfgChannel, sdpQueryToken );
    BOOL bHeadset = (Channel->type==AG_TYPE_HS);
    U8 completeFlags = bHeadset ? SDP_QUERY_ALL_HS_FLAGS : SDP_QUERY_ALL_HF_FLAGS;
    U16 attrib;

    attrib = bHeadset ? hsag_attribute_id_list[attribute_index] : hfg_attribute_id_list[attribute_index];

    Report( ("[HFG][hfgQueryCallback] result==%d, attribute_index==x%X, attribute==0x%X", result, attribute_index, attrib) );
	kal_trace(TRACE_GROUP_5, BT_HFG_CBK_SDP_QUERY, sqt, result, attribute_index, attribute_value);
	kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
    
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            switch (attrib)
            {
		  case AID_SERVICE_CLASS_ID_LIST:
		  	/* This shall be the last returned sdp data */
			Channel->queryFlags |= SDP_QUERY_FLAG_SERVICE_CLASS_ID_LIST;
		  	break;
                case AID_PROTOCOL_DESC_LIST:
                    Channel->rfServerChannel = attribute_value[0];                    
                    Channel->queryFlags |= SDP_QUERY_FLAG_PROTOCOL;
                    DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_PROTOCOL_DESC_LIST, server channel=%d", Channel->rfServerChannel));
                    break;
                case AID_BT_PROFILE_DESC_LIST:
                    Channel->version = SDP_GetU16(attribute_value);
                    Channel->queryFlags |= SDP_QUERY_FLAG_PROFILE;
                    DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_BT_PROFILE_DESC_LIST, version=0x%X", Channel->version));
                    break;
                case AID_SUPPORTED_FEATURES:
                    /* SDP result parsing completed */
                    Channel->queryFlags |= SDP_QUERY_FLAG_FEATURES;
                    Channel->hfFeatures = SDP_GetU16(attribute_value);
                    if (!(Channel->flags & CHANNEL_FLAG_OUTGOING))
                    {
                        Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
                        Channel->cmgrHandler.errCode = BEC_NO_ERROR;
                        HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, BT_STATUS_SUCCESS, 
                                       (U32)(U32)Channel->hfFeatures);
                    }                    
                    Channel->hfFeatures = SDP_GetU16(attribute_value);
                    DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : AID_SUPPORTED_FEATURES, HF features=0x%X", Channel->hfFeatures));
                    break;
                case AID_REMOTE_AUDIO_VOL_CONTROL:
                    Channel->bRemoteVolControl = attribute_value[0];
                    Channel->queryFlags |= SDP_QUERY_FLAG_VOL_CONTROL;
                    break;
		default:
			kal_trace(TRACE_GROUP_5, BT_HFG_WARN_UNRECOGNIZED_SDP_ATTR, attrib);
			break;
            }
            break;
        case BT_STATUS_CONNECTION_FAILED:
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_CONNECTION_FAILED"));
            break;
        case BT_STATUS_FAILED:
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_FAILED"));
            break;
        case BT_STATUS_NOSERVICES:
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : BT_STATUS_NOSERVICES"));
            break; 
        default:
            DBG_PRINT_EVENT(("[HFG][EVENT] SDP callback : unknown result : %d", result));
            break;
    	}

    if( result != BT_STATUS_SUCCESS || (Channel->queryFlags & completeFlags) == completeFlags )
    {
        /* SDP query is over then clear ongoing bit  */
        Channel->queryFlags &= ~SDP_QUERY_FLAG_ONGOING;
        Report( ("[HFG][hfgQueryCallback] SDP query complete") );
		kal_trace(TRACE_GROUP_5, BT_HFG_INFO_SDP_QUERY_COMPLETE, Channel->rfServerChannel);
    }

    if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_OUT2)
    {
        if(result == BT_STATUS_SUCCESS)
        {
            if( (Channel->queryFlags & completeFlags) == completeFlags )
            {
                /* All attributes are returned */
                Assert(Channel->rfChannel->userContext == 0);
                Assert(Channel->rfServerChannel != 0);
                status = RF_OpenClientChannel(Channel->cmgrHandler.bdc->link,
                                                  		    Channel->rfServerChannel,
                                                  		    Channel->rfChannel, 1);
                DBG_PRINT_FLOW(("[HFG][FLOW] Open RFCOMM channel : %d", status));
                if (status == BT_STATUS_PENDING) 
                {
			Channel->linkFlags |= HFG_LINK_CONNECTING;
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_OUT3);
                } 
                else 
                {
					kal_trace(TRACE_GROUP_5, BT_HFG_WARN_OPEN_CHANNEL_FAIL, status);
                    Channel->cmgrHandler.errCode = BEC_LOCAL_TERMINATED;
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
                }
            }
        }
        else
        {
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED1);
        }
    }
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN2)
    {
        HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN1);
    }
    else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC1)
    {
        HfgDisconnecting(Channel);
    }
    else
    {
        DBG_PRINT_ERROR( ("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", HfgGetSubstate(Channel), __BT_FILE__, __LINE__) );
		kal_trace(TRACE_GROUP_5, BT_HFG_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, Channel, HfgGetSubstate(Channel));
        Assert(0);
    }	
    Report(("- hfgQueryCallback."));
}
