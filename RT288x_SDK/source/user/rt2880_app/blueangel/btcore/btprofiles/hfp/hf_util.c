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

 * hf_util.c

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file contains utility functions for HFP HF role.

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

#include "sys/hfalloc.h"

#include "conmgr.h"

#include "btalloc.h"

#include "hf.h"



#define __BT_FILE__	BT_FILE_HF_UTIL



/*---------------------------------------------------------------------------

 *            HfAppCallback()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Sets up the callback parameters and calls the application.

 *

 * Return:    void

 */

void HfAppCallback(HfChannel_struct *pChannel, HfEvent event, BtStatus status, U32 data) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfCallbackParms_struct hfParms;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    OS_MemSet((U8 *)&hfParms, 0, (U32)sizeof(HfCallbackParms_struct));

    hfParms.event   = event;

    hfParms.errCode = pChannel->cmgrHandler.errCode;    

    hfParms.status  = status;

    hfParms.p.ptr   = (void *)data;

    

	kal_trace( TRACE_GROUP_5, BT_HF_INFO_CALLBACK_TO_ADP, pChannel, event, status, data);

    pChannel->callback(pChannel, &hfParms);

}



/*---------------------------------------------------------------------------

 *            HfAllocRfChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Allocate an RF channel (make it "in use").

 *

 * Return:    A pointer to the RF channel, NULL if not found.

 */

RfChannel *HfAllocRfChannel(void)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    RfChannel *pRfChannel = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_ALLOC_RF_CHANNEL);

	

    if (!IsListEmpty(&HF(freeRfChannelList))) 

    {

        pRfChannel = (RfChannel *)RemoveHeadList(&HF(freeRfChannelList)); 

    }



    if (pRfChannel) 

    {

        pRfChannel->callback     = HfRfCallback;

        pRfChannel->maxFrameSize = RF_MAX_FRAME_SIZE;

        pRfChannel->priority     = RF_DEFAULT_PRIORITY;

    }

    else

    {

		kal_trace(TRACE_GROUP_5, BT_HF_ERROR, __BT_FILE__, __LINE__);

    }

    return pRfChannel;

}



/*---------------------------------------------------------------------------

 *            HfFreeRfChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Frees an RF channel (makes it "available").

 *

 * Return:    void

 */

void HfFreeRfChannel(RfChannel *pRfChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    kal_trace(TRACE_GROUP_5, BT_HF_INFO_FREE_RF_CHANNEL, pRfChannel);



    if (!IsNodeOnList(&HF(freeRfChannelList), &pRfChannel->node)) 

    {

        pRfChannel->userContext = NULL;

        InsertTailList(&HF(freeRfChannelList), &pRfChannel->node);

    }

    else

    {

		kal_trace(TRACE_GROUP_5, BT_HF_ERROR, __BT_FILE__, __LINE__);

	}

}



/*---------------------------------------------------------------------------

 *            HfRegisterRfServerChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Register a server channel to RFCOMM.

 *

 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully registered.

 */

BtStatus HfRegisterRfServerChannel(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_FAILED;

    BOOL bReg       = FALSE;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/  

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_REGISTER_SERVER_CHANNEL, pChannel);

    bReg = (BOOL)(pChannel->pRfChannel->userContext != NULL);

    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_REGISTERED, bReg);

    if (bReg == FALSE)

    {

        status = RF_RegisterServerChannel(pChannel->pRfChannel, &HF(hfService)[pChannel->type], HF_RF_CREDIT);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_REGISTERING, status);

        if (status == BT_STATUS_SUCCESS)

        {      

            pChannel->pRfChannel->userContext = (void*)1;

        }

    }

    else

    {

		/* The RfChannel is already registered */

		kal_trace(TRACE_GROUP_5, BT_HF_WARN, __BT_FILE__, __LINE__);

    }

    return status;

}



/*---------------------------------------------------------------------------

 *            HfDeregisterRfServerChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Deregister a server channel to RFCOMM.

 *

 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully deregistered.

 */

BtStatus HfDeregisterRfServerChannel(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_FAILED;

    BOOL bReg       = FALSE;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_DEREGISTER_SERVER_CHANNEL, pChannel);

    bReg = (BOOL)(pChannel->pRfChannel->userContext != NULL);    

    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_REGISTERED, bReg);    

	if (bReg == TRUE)

    {

        status = RF_DeregisterServerChannel(pChannel->pRfChannel, &HF(hfService)[pChannel->type]);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DEREGISTERING, status);        

        if (status == BT_STATUS_SUCCESS)

        {

            pChannel->pRfChannel->userContext = NULL;

        }	

    }	

	else

	{

		kal_trace(TRACE_GROUP_5, BT_HF_WARN, __BT_FILE__, __LINE__);

	}  

    return status;

}



/*---------------------------------------------------------------------------

 *            HfOpenCmgrChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Open ACL association and set set HF_LINK_FLAG_ACL to linkflag 

 *

 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully associated.

 */

BtStatus HfOpenCmgrChannel(HfChannel_struct *pChannel, BD_ADDR *pAddr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    BOOL bACL       = FALSE;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/   

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_OPEN_ACL_CHANNEL, 

                    		 pChannel,

                    		 pAddr->addr[0],

                    		 pAddr->addr[1],

                    		 pAddr->addr[2],

                    		 pAddr->addr[3],

                    		 pAddr->addr[4],

                    		 pAddr->addr[5]);

    bACL = (BOOL)((HfGetLinkFlags(pChannel) & HF_LINK_FLAG_ACL) == HF_LINK_FLAG_ACL);

    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_ACL_ASSOCIATED, bACL);

    if (bACL == FALSE)

    {

        status = CMGR_CreateDataLink(&pChannel->cmgrHandler, pAddr);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_ACL_ASSOCIATING, status);        

        if (status == BT_STATUS_SUCCESS)

        {

            HfSetLinkFlags(pChannel, HF_LINK_FLAG_ACL);

        }

    }

	else

	{

		kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, pChannel);

    }

    return status;

}



/*---------------------------------------------------------------------------

 *            HfCloseCmgrChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Close ACL association and set link flag to 0

 *

 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.

 */

BtStatus HfCloseCmgrChannel(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

    BOOL bACL       = FALSE;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CLOSE_ACL_CHANNEL, pChannel);



    bACL = (BOOL)((HfGetLinkFlags(pChannel) & HF_LINK_FLAG_ACL) == HF_LINK_FLAG_ACL);

    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_ACL_ASSOCIATED, bACL);    

	if (bACL == TRUE) 

    {

        status = CMGR_RemoveDataLink(&pChannel->cmgrHandler);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_ACL_DISASSOCIATING, status);            

        CMGR_RemoveCmgrWithDevice(&pChannel->cmgrHandler);

        HfClearLinkFlags(pChannel);

        if (status != BT_STATUS_SUCCESS)

        {

            Assert(0);

        }

    }

	else

	{

		kal_trace(TRACE_GROUP_5, BT_HF_WARN, __BT_FILE__, __LINE__);

	}   

    return status;

}



/*---------------------------------------------------------------------------

 *            HfFindRegisteredChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Find registered hf channel according to rf channel.

 *

 * Return:    The pointer to the registered hf channel

 */

HfChannel_struct *HfFindRegisteredChannel(RfChannel *pRfChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfChannel_struct *pChannel = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    pChannel = (HfChannel_struct *)GetHeadList(&HF(channelList));

    while (&pChannel->node != &HF(channelList)) 

	{

        if (pChannel->pRfChannel == pRfChannel) 

        {

            break;

        }



        /* Look at the next node */

        pChannel = (HfChannel_struct *)GetNextNode(&pChannel->node);

    }



    if (&pChannel->node == &HF(channelList)) 

	{

        /* Not found */

        pChannel = NULL;

    }

    return pChannel;

}



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

/*---------------------------------------------------------------------------

 *            HfSuspendOtherContexts()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Suspend other contexts

 *

 * Return:    void

 */

void HfSuspendOtherContexts(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfChannel_struct *pTmpChannel = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    pTmpChannel = (HfChannel_struct *)GetHeadList(&pChannel->node);

    while (pTmpChannel != pChannel)

    {

        if (&pTmpChannel->node != &HF(channelList))

        {

            kal_trace(TRACE_GROUP_5, BT_HF_INFO_SUSPEND_CONTEXT, pTmpChannel);

            Assert(HfGetState(pTmpChannel) == HF_STATE_CLOSED);            

            pTmpChannel->bSuspended = TRUE;

            HfDeregisterRfServerChannel(pTmpChannel);

        }

        pTmpChannel = (HfChannel_struct *)GetHeadList(&pTmpChannel->node);

    }

}



/*---------------------------------------------------------------------------

 *            HfResumeOtherContexts()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Resume other contexts

 *

 * Return:    void

 */

void HfResumeOtherContexts(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfChannel_struct *pTmpChannel = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/      

    pTmpChannel = (HfChannel_struct *)GetHeadList(&pChannel->node);

    while (pTmpChannel != pChannel)

    {

        if (&pTmpChannel->node != &HF(channelList))

        {

            kal_trace(TRACE_GROUP_5, BT_HF_INFO_RESUME_CONTEXT, pTmpChannel);   

            Assert(HfGetState(pTmpChannel) == HF_STATE_CLOSED);            

            Assert(pTmpChannel->bSuspended == TRUE);

            pTmpChannel->bSuspended = FALSE;

            Assert(BT_STATUS_SUCCESS == HfRegisterRfServerChannel(pTmpChannel));

        }

        pTmpChannel = (HfChannel_struct *)GetHeadList(&pTmpChannel->node);

    }

}

#endif



/*---------------------------------------------------------------------------

 *            HfChangeState()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Change state of the channel

 *

 * Return:    void

 */

void HfChangeState(HfChannel_struct *pChannel, U8 newState)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    U8 preState = HfGetState(pChannel);



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    if (preState == newState)

    {

        return;

    }



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (newState == HF_STATE_CLOSED)

    {

        HfResumeOtherContexts(pChannel);

    }

    else

    {

        if (preState == HF_STATE_CLOSED)

        {

            HfSuspendOtherContexts(pChannel);

        }

    }

#endif    



    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_CHANGE_STATE, preState, newState);

    HfSetState(pChannel, newState);

}



/*---------------------------------------------------------------------------

 *            HfCloseChannel()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Clean up the channel state and notify app

 *

 * Return:    void

 */

void HfCloseChannel(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if (HfGetState(pChannel) != HF_STATE_CLOSED)

    {     

        /* Clean up the state */

        pChannel->bTxInProgress = FALSE;        

        HfClearQueryFlags(pChannel);

        HfChangeState(pChannel, HF_STATE_CLOSED);

        HfResetTxBuffer(pChannel);        



        /* Release link handler */

        if (HfGetLinkFlags(pChannel) & HF_LINK_FLAG_ACL)

        {

            HfCloseCmgrChannel(pChannel);

        }



        /* Register RF channel */

        if (pChannel->pRfChannel->userContext == NULL)

        {

            HfRegisterRfServerChannel(pChannel);

        } 



        HfAppCallback(pChannel, HF_EVENT_SERVICE_DISCONNECTED, BT_STATUS_SUCCESS, (U32)&pChannel->bdAddr.addr);        

    }

}



/*---------------------------------------------------------------------------

 *            HfRemainTxData()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Calc the remain tx data size.

 *

 * Return:    Size of the remain tx data.

 */

U16 HfRemainTxData(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    XaBufferDesc *pBuf = &pChannel->txBuffer;

    U16 size           = 0;

    

    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    size  = (U16)(pBuf->writeOffset + HF_TX_BUFFER_SIZE);

    size -= pBuf->readOffset;

    size &= HF_TX_BUFFER_MODE;

    return size;

}



/*---------------------------------------------------------------------------

 *            HfEmptyTxSize()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Calc the empty tx data size.

 *

 * Return:    Size of the empty tx data.

 */

U16 HfEmptyTxSize(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    XaBufferDesc *pBuf = &pChannel->txBuffer;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    return (HF_TX_BUFFER_MODE - HfRemainTxData(pChannel));

}



/*---------------------------------------------------------------------------

 *            HfResetTxBuffer()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Reset the tx buffer.

 *

 * Return:    void.

 */

void HfResetTxBuffer(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    pChannel->txBuffer.readOffset  = 0;

    pChannel->txBuffer.writeOffset = 0;

    pChannel->txBuffer.buff        = pChannel->txData;

}



/*---------------------------------------------------------------------------

 *            HfSendTxBuffer()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Send tx buffer to RFCOMM.

 *

 * Return:    Status of RF_SendData.

 */

BtStatus HfSendTxBuffer(HfChannel_struct *pChannel)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status   = BT_STATUS_FAILED;

    BtPacket *pPacket = &pChannel->txPacket;

    U16	len           = HfRemainTxData(pChannel);



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if ((len == 0) || pChannel->bTxInProgress)

    {

        return status;

    }



    if (len > (HF_TX_BUFFER_SIZE - pChannel->txBuffer.readOffset))

    {

        len = (HF_TX_BUFFER_SIZE - pChannel->txBuffer.readOffset);

    }



    if (len > RF_FrameSize(pChannel->pRfChannel)) 

    {

        len = RF_FrameSize(pChannel->pRfChannel);

    }



    pPacket->dataLen = len;

    pPacket->data    = (pChannel->txBuffer.buff + pChannel->txBuffer.readOffset);



    status = RF_SendData(pChannel->pRfChannel, pPacket);

    if (status == BT_STATUS_PENDING)

    {

        pChannel->txBuffer.readOffset += len;

        pChannel->txBuffer.readOffset &= HF_TX_BUFFER_MODE;

        pChannel->bTxInProgress = TRUE;

    }

    else

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_SEND_RF_PACKET_FAILED);

    }

    return status;

}



/*---------------------------------------------------------------------------

 *            HfWriteTxBuffer()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Write data to tx buffer.

 *

 * Return:    Size of written data.

 */

U16 HfWriteTxBuffer(HfChannel_struct *pChannel, U8 *pBuf, U16 len)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    U16 remain   = HfEmptyTxSize(pChannel);

    U16 edge     = HF_TX_BUFFER_SIZE - pChannel->txBuffer.writeOffset;

    U8 *ptr      = pChannel->txBuffer.buff + pChannel->txBuffer.writeOffset;

    U16 writeLen = 0;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if (len > remain)

    {

        len = remain;

    }    

    

    writeLen = len;

    if (len > edge)

    {

        OS_MemCopy(ptr, pBuf, edge);

        pBuf += edge;

        ptr = pChannel->txBuffer.buff;

        pChannel->txBuffer.writeOffset = 0;

        len -= edge;

    }  

    

    if (len > 0)

    {

        OS_MemCopy(ptr, pBuf, len);

        pChannel->txBuffer.writeOffset += len;

        pChannel->txBuffer.writeOffset &= HF_TX_BUFFER_MODE;

    }



    if (!pChannel->bTxInProgress && (HfRemainTxData(pChannel) > 0))

    {

        kal_trace(TRACE_GROUP_5, BT_HF_AT_BYTES_TO_SEND, HfRemainTxData(pChannel));      

        HfSendTxBuffer(pChannel);

    }

    return writeLen;

}



#endif /* __BT_HF_PROFILE__ */

