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

 * hf_sm.c

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file contains the connection state machine for HFP HF role.

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

#include "rfcomm_adp.h"

#include "conmgr.h"



#define __BT_FILE__	BT_FILE_HF_SM



static void HfStateClosed(HfChannel_struct *pChannel, RfCallbackParms *pParms);

static void HfStateConnPending(HfChannel_struct *pChannel, RfCallbackParms *pParms);

static void HfStateDiscPending(HfChannel_struct *pChannel, RfCallbackParms *pParms);

static void HfStateOpen(HfChannel_struct *pChannel, RfCallbackParms *pParms);



/*---------------------------------------------------------------------------

 *            HfInitStateMachine()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Initializes the HF state machine.

 *

 * Return:    void

 *

 */

void HfInitStateMachine(void)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    HF(hfState)[HF_STATE_CLOSED]       = HfStateClosed;

    HF(hfState)[HF_STATE_CONN_PENDING] = HfStateConnPending;    

    HF(hfState)[HF_STATE_DISC_PENDING] = HfStateDiscPending;  

    HF(hfState)[HF_STATE_OPEN]         = HfStateOpen;    

}



/*---------------------------------------------------------------------------

 *            HfCmgrCallback()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Called by device manager with link state events.

 *

 * Return:    (See header file)

 *

 */

void HfCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status            = BT_STATUS_SUCCESS;    

    HfChannel_struct *pChannel = ContainingRecord(Handler, HfChannel_struct, cmgrHandler);



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

	kal_trace(TRACE_GROUP_5, BT_HF_CBK_CMGR_CALLBACK, pChannel, Event, Status);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



    switch (Event) 

    {

    case CMEVENT_DATA_LINK_CON_CNF:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_CMGR_DATA_LINK_CON_CNF, Status);      

        if (Status == BT_STATUS_SUCCESS) 

        {

            HfSetLinkFlags(pChannel, HF_LINK_FLAG_ACL);

            if (HfSdpStartQuery(pChannel, BSQM_FIRST) == BT_STATUS_PENDING)

            {

                return;

            }          

        }

        else

        {

            HfUnsetLinkFlags(pChannel, HF_LINK_FLAG_ACL);

        }

        HfCloseChannel(pChannel);

        break;

    case CMEVENT_DATA_LINK_DIS:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_CMGR_DATA_LINK_DIS, Status);        

        HfCloseChannel(pChannel);

        break;

    case CMEVENT_AUDIO_LINK_CON:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_CMGR_AUDIO_LINK_CON, Status);          

        HfUnsetLinkFlags(pChannel, HF_LINK_FLAG_SCO_CONNECTING);           

        HfAppCallback(pChannel, HF_EVENT_AUDIO_CONNECTED, Status, (U32)&pChannel->bdAddr.addr);    

        break;

    case CMEVENT_AUDIO_LINK_DIS:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_CMGR_AUDIO_LINK_DIS, Status);           

        HfUnsetLinkFlags(pChannel, HF_LINK_FLAG_SCO_DISCONNECTING);            

        HfAppCallback(pChannel, HF_EVENT_AUDIO_DISCONNECTED, Status, (U32)&pChannel->bdAddr.addr);

        if (HfGetState(pChannel) == HF_STATE_DISC_PENDING)

        {

            status = CMGR_DeregisterScoHandler(&pChannel->cmgrHandler);

            kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_DEREGISTERING, status);                

            status = RF_CloseChannel(pChannel->pRfChannel);

            kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DISCONNECTING, status);   

        }

        break;

    default:

        break;

    }

}



/*---------------------------------------------------------------------------

 *            HfRfCallback()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  RFCOMM callback for the HF state machine.

 *

 * Return:    void

 */

void HfRfCallback(RfChannel *pRfChannel, RfCallbackParms *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status            = BT_STATUS_SUCCESS;

    HfChannel_struct *pChannel = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    pChannel = HfFindRegisteredChannel(pRfChannel);

    Assert(pChannel != NULL);

	kal_trace(TRACE_GROUP_5, BT_HF_CBK_RF_CALLBACK, pChannel, pParms->event, pParms->status);

    HF(hfState)[pChannel->state](pChannel, pParms);

}



/*---------------------------------------------------------------------------

 *            HfStateClosed()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Handle the closed state for the HF state machine.

 *

 * Return:    void

 */

static void HfStateClosed(HfChannel_struct *pChannel, RfCallbackParms *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    switch (pParms->event)

    {

    case RFEVENT_OPEN_IND:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_OPEN_IND);            

        OS_MemCopy((U8*)pChannel->bdAddr.addr, (U8*)pParms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));

        HfChangeState(pChannel, HF_STATE_CONN_PENDING);

        HfSetLinkFlags(pChannel, HF_LINK_FLAG_CONN_INCOMING);

        HfAppCallback(pChannel, HF_EVENT_AUTH_REQ, BT_STATUS_SUCCESS, (U32)&pChannel->bdAddr.addr); 

        break;  

    default:

        break;

    }

}



/*---------------------------------------------------------------------------

 *            HfStateConnPending()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Handle the connect pending state for the HF state machine.

 *

 * Return:    void

 */

static void HfStateConnPending(HfChannel_struct *pChannel, RfCallbackParms *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/     

    switch (pParms->event)

    {

    case RFEVENT_OPEN_IND:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_OPEN_IND);           

        status = RF_RejectChannel(pChannel->pRfChannel);

        if (status != BT_STATUS_PENDING) 

        {

            kal_trace(TRACE_GROUP_5, BT_HF_WARN_REJECT_CHANNEL_FAIL, status);

        }  

        break;  

    case RFEVENT_OPEN:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_OPEN);          

        OS_MemCopy((U8*)pChannel->bdAddr.addr, (U8*)pParms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));

        HfChangeState(pChannel, HF_STATE_OPEN);

        HfUnsetLinkFlags(pChannel, HF_LINK_FLAG_CONN_INCOMING);         

        status = CMGR_RegisterScoHandler(&pChannel->cmgrHandler);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_REGISTERING, status);      

        HfAppCallback(pChannel, HF_EVENT_SERVICE_CONNECTED, BT_STATUS_SUCCESS, (U32)&pChannel->bdAddr.addr);  

        break;        

    case RFEVENT_CLOSED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_CLOSED);          

        HfCloseChannel(pChannel);

        break;          

    default:

        break;

    }  

}



/*---------------------------------------------------------------------------

 *            HfStateDiscPending()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Handle the disconnect pending state for the HF state machine.

 *

 * Return:    void

 */

static void HfStateDiscPending(HfChannel_struct *pChannel, RfCallbackParms *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    switch (pParms->event)

    {

    case RFEVENT_OPEN_IND:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_OPEN_IND);           

        status = RF_RejectChannel(pChannel->pRfChannel);

        if (status != BT_STATUS_PENDING) 

        {

            kal_trace(TRACE_GROUP_5, BT_HF_WARN_REJECT_CHANNEL_FAIL, status);

        }  

        break;  

    case RFEVENT_CLOSED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_CLOSED);          

        HfCloseChannel(pChannel);

        break;         

    default:

        break;

    } 

}



/*---------------------------------------------------------------------------

 *            HfStateOpen()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Handle the open state for the HF state machine.

 *

 * Return:    void

 */

static void HfStateOpen(HfChannel_struct *pChannel, RfCallbackParms *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    HfAtResult_struct atData;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/     

    switch (pParms->event)

    {

    case RFEVENT_OPEN_IND:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_OPEN_IND);           

        status = RF_RejectChannel(pChannel->pRfChannel);

        if (status != BT_STATUS_PENDING) 

        {

            kal_trace(TRACE_GROUP_5, BT_HF_WARN_REJECT_CHANNEL_FAIL, status);

        } 

        break;     

    case RFEVENT_PACKET_HANDLED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_PACKET_HANDLED);          

        pChannel->bTxInProgress = FALSE;            

        if (HfRemainTxData(pChannel) > 0)

        {

            kal_trace(TRACE_GROUP_5, BT_HF_AT_BYTES_TO_SEND, HfRemainTxData(pChannel));          

            HfSendTxBuffer(pChannel);

        }

        break;

    case RFEVENT_DATA_IND:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_DATA_IND);          

        RF_AdvanceCredit(pChannel->pRfChannel, HF_RF_CREDIT);                   

        OS_MemSet((U8 *)&atData, 0, (U32)sizeof(HfAtResult_struct));

        atData.pData   = pParms->ptrs.data;

        atData.dataLen = pParms->dataLen;

        HfAppCallback(pChannel, HF_EVENT_DATA_IND, BT_STATUS_SUCCESS, (U32)&atData);   

        break;  

    case RFEVENT_CLOSED:

        kal_trace(TRACE_GROUP_5, BT_HF_EVENT_RF_CLOSED);          

        HfCloseChannel(pChannel);

        break;        

    default:

        break;

    }

}

#endif /* __BT_HF_PROFILE__ */

