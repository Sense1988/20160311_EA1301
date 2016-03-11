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



/*****************************************************************************

 *

 * Filename:

 * ---------

 * hf_sdp.c

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file contains the SDP register, deregister and query for HFP HF role.

 *

 *============================================================================

 *             HISTORY

 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!

 *------------------------------------------------------------------------------

 * $Revision: #1 $

 * $Modtime: $

 * $Log: $

 *------------------------------------------------------------------------------

 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!

 *============================================================================

 ****************************************************************************/

#ifdef __BT_HF_PROFILE__   

#include "stdio.h"

#include "sys/hfalloc.h"

#include "btalloc.h"

#include "hf.h"

#include "sdpdb.h"

#include "sdp.h"

#include "sdap.h"



#define __BT_FILE__	BT_FILE_HF_SDP



#if SDP_PARSING_FUNCS == XA_DISABLED

#error The Hands-free SDK requires the SDP parsing functions.

#endif /* SDP_PARSING_FUNCS == XA_DISABLED */



static U16 hf_attribute_id_list[] = 

{

    AID_PROTOCOL_DESC_LIST, 

    //AID_BT_PROFILE_DESC_LIST, 

    AID_EXTERNAL_NETWORK,    

    AID_SUPPORTED_FEATURES,

    AID_SERVICE_CLASS_ID_LIST,    

};



static U16 hs_attribute_id_list[] = 

{

    AID_PROTOCOL_DESC_LIST, 

    //AID_BT_PROFILE_DESC_LIST,

    AID_SERVICE_CLASS_ID_LIST,    

};



/* Internal function prototypes */

static void hfSdpQueryCallback(SdpQueryToken *sqt, 

                               U8             result, 

                               U8             attribute_index,

                               U8            *attribute_value);



/****************************************************************************

 *

 * Functions

 *

 ****************************************************************************/



/*---------------------------------------------------------------------------

 *            HfSdpRegisterService()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Registers the SDP services.

 *

 * Return:    See SDP_AddRecord().

 */

BtStatus HfSdpRegisterService(BOOL bHeadset)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    U8 type         = bHeadset ? HF_SERVICE_TYPE_HS : HF_SERVICE_TYPE_HF;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_REGISTER_SDP, bHeadset ? 1 : 0);



    if (HF(hfSdpRecord)[type].num == 0)

    {

		U16 serv = bHeadset ? SC_HEADSET : SC_HANDSFREE;

        HF(hfSdpRecord)[type].num = SDPDB_GetLocalServiceAttribute(serv,

                                                                    &(HF(hfSdpRecord)[type].attribs),

                                                                    &(HF(hfService)[type].serviceId));

        if (HF(hfSdpRecord)[type].num > 0)

        {

            status = SDP_AddRecord(&(HF(hfSdpRecord)[type]));

            kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SDP_ADD_RECORD, status);            

        }

        else

        {

			kal_trace(TRACE_GROUP_5, BT_HF_ERROR_1, __BT_FILE__, __LINE__, HF(hfSdpRecord)[type].num);

            Assert(HF(hfSdpRecord)[type].num > 0);

            status = BT_STATUS_FAILED;

        }            

    }

    else

    {

        /* Already registered */

		kal_trace(TRACE_GROUP_5, BT_HF_WARN, __BT_FILE__, __LINE__);

    }

    

    return status;

}



/*---------------------------------------------------------------------------

 *            HfSdpDeregisterService()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Deregisters the SDP services.

 *

 * Return:    See SDP_RemoveRecord().

 */

BtStatus HfSdpDeregisterService(BOOL bHeadset)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    U8 type         = bHeadset ? HF_SERVICE_TYPE_HS : HF_SERVICE_TYPE_HF;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/     

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_DEREGISTER_SDP, bHeadset ? 1 : 0);

    

    /* Remove the Hands-free entry */

    if (HF(hfSdpRecord)[type].num > 0)

    {

        status = SDP_RemoveRecord(&(HF(hfSdpRecord)[type]));

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SDP_REMOVE_RECORD, status);          

    }

    

    /* As a mark that the SDP record is not registered  */

    /* num and serviceId are all get through HfRegisterSdpService, so we clear these to 0 */

    HF(hfSdpRecord)[type].num     = 0;

    HF(hfService)[type].serviceId = 0;

    return status;

}



/*---------------------------------------------------------------------------

 *            HfSdpStartQuery()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Initiate the SDP service query.

 *

 * Return:    See SDP_Query().

 */

BtStatus HfSdpStartQuery(HfChannel_struct *pChannel, SdpQueryMode mode)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    BOOL bHeadset   = (pChannel->type == HF_SERVICE_TYPE_HS);

    sdap_service_search_multi_attribute_struct search_pattern;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/     

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_START_SERVICE_QUERY, pChannel, mode, bHeadset ? 1 : 0);

	

    HfClearQueryFlags(pChannel);

    OS_MemSet((U8 *)&search_pattern, 0, (U32)sizeof(sdap_service_search_multi_attribute_struct));

    search_pattern.rm       = pChannel->cmgrHandler.remDev;

    search_pattern.sqt      = &pChannel->sdpQueryToken;

    search_pattern.uuid     = bHeadset ? SC_HEADSET_AUDIO_GATEWAY : SC_HANDSFREE_AUDIO_GATEWAY;

    search_pattern.callback = hfSdpQueryCallback;

    if (bHeadset)

    {

        search_pattern.attribute_num = sizeof(hs_attribute_id_list) / sizeof(hs_attribute_id_list[0]);

        search_pattern.attribute_id  = hs_attribute_id_list;

    }

    else

    {

        search_pattern.attribute_num = sizeof(hf_attribute_id_list) / sizeof(hf_attribute_id_list[0]);

        search_pattern.attribute_id  = hf_attribute_id_list;        

    }

    

    status = SDAP_ServiceSearchMultipleAttribute(search_pattern);    

    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SDP_SERVICE_SEARCH, status);

    if (status == BT_STATUS_PENDING)

    {

        HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_ONGOING);

    }

    return status;

} 



/*************************************************************************

* FUNCTION                                                            

*	hfSdpQueryCallback

* DESCRIPTION                                                           

*   The function is called by SDAP to indicate the profiles the search result

*

* PARAMETERS

*	sqt  : SdpQueryToken contains information such as remote device information. uuid...

*	result  : The service search result.

*      attribute_index: the current request attribute list index

*      attribute_value: points to the search result

* RETURNS

*	None

*************************************************************************/

static void hfSdpQueryCallback(SdpQueryToken *sqt, 

                               U8             result, 

                               U8             attribute_index,

                               U8            *attribute_value)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status            = BT_STATUS_SUCCESS;

    HfChannel_struct *pChannel = ContainingRecord(sqt, HfChannel_struct, sdpQueryToken);

    BOOL bHeadset              = (pChannel->type == HF_SERVICE_TYPE_HS);

    U8 completeFlags           = 0;

    U16 attrib                 = 0;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

	kal_trace(TRACE_GROUP_5, BT_HF_CBK_SDP_QUERY, sqt, result, attribute_index, attribute_value);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



    completeFlags = bHeadset ? HF_SDP_QUERY_ALL_HS_FLAGS : HF_SDP_QUERY_ALL_HF_FLAGS;

    attrib        = bHeadset ? hs_attribute_id_list[attribute_index] : hf_attribute_id_list[attribute_index];

    switch (result) 

    {

    case BT_STATUS_SUCCESS:

        switch (attrib)

        {

        case AID_SERVICE_CLASS_ID_LIST:

            /* This shall be the last returned sdp data */

            HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_SVCCLASSID_LIST);

            break;                

        case AID_PROTOCOL_DESC_LIST:

            pChannel->rfServerChannel = attribute_value[0]; 

            HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_PROTOCOL);    

            kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_PROTOCOL, pChannel->rfServerChannel);                 

            break;

        case AID_BT_PROFILE_DESC_LIST:

            HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_PROFILE);                          

            kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_PROFILE, SDP_GetU16(attribute_value));               

            break;

        case AID_EXTERNAL_NETWORK:

            HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_NETWORK);             

            kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_NETWORK, attribute_value[0]);               

            break;

        case AID_SUPPORTED_FEATURES:

            HfSetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_FEATURES);               

            kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_FEATURES, SDP_GetU16(attribute_value));               

            break;                    

        default:

            kal_trace(TRACE_GROUP_5, BT_HF_WARN_UNRECOGNIZED_SDP_ATTR, attrib);

            break;

        }

        break;

    case BT_STATUS_CONNECTION_FAILED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_CONNECTION_FAILED);           

        break;

    case BT_STATUS_FAILED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_FAILED);           

        break;

    case BT_STATUS_NOSERVICES:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_NOSERVICES);         

        break; 

    default:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_SDP_UNKNOWN_RESULT, result);            

        break;

    }



    if ((result != BT_STATUS_SUCCESS) || 

        ((HfGetQueryFlags(pChannel) & completeFlags) == completeFlags))

    {

        /* SDP query is over then clear ongoing bit */

        HfUnsetQueryFlags(pChannel, HF_SDP_QUERY_FLAG_ONGOING);

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_SDP_QUERY_COMPLETE, pChannel->rfServerChannel);

    }



    if (result == BT_STATUS_SUCCESS)

    {

        if ((HfGetQueryFlags(pChannel) & completeFlags) == completeFlags)

        {

            /* All attributes are returned */

            Assert(pChannel->pRfChannel->userContext == NULL);

            Assert(pChannel->rfServerChannel != 0);

            status = RF_OpenClientChannel(pChannel->cmgrHandler.bdc->link,

                                          pChannel->rfServerChannel,

                                          pChannel->pRfChannel, 

                                          HF_RF_CREDIT);

		    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_CONNECTING, status);            

            if (status != BT_STATUS_PENDING) 

            {

                pChannel->cmgrHandler.errCode = BEC_LOCAL_TERMINATED;

                HfCloseChannel(pChannel);

            }

        }

    }	

    else

    {

        HfCloseChannel(pChannel);

    }

}

#endif /* __BT_HF_PROFILE__ */

