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

 * hf.c

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file contains API functions for HFP HF role.

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

#include "btalloc.h"

#include "hf.h"



#define __BT_FILE__	BT_FILE_HF



/*---------------------------------------------------------------------------

 *            HF_Init()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Initialize Hands-Free SDK. Initialize state and other variables.

 *

 * Return:    (See hf.h)

 */

BOOL HF_Init(void)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    U16 i    = 0;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

	kal_trace(TRACE_GROUP_5, BT_HF_API_INIT);

	

    /* Initialize context memory */

    HfAlloc();



    /* Initialize state machine */

    HfInitStateMachine();



    /* Initialize lists */

    InitializeListHead(&HF(channelList));

    InitializeListHead(&HF(freeRfChannelList));

    for (i = 0; i < HF_SERVICE_TOTAL; i++) 

    {

        HF(rfChannels)[i].userContext = 0;

#ifndef __BT_2_1_SIMPLE_PAIRING__        

        RF_ChannelInit(&(HF(rfChannels)[i]), BT_SEC_HIGH);

#else

        RF_ChannelInit(&(HF(rfChannels)[i]), BT_SEC_MED);

#endif

        InsertTailList(&HF(freeRfChannelList), &HF(rfChannels)[i].node);

    }

	

	kal_trace(TRACE_GROUP_5, BT_HF_API_INIT_RET);    

    return TRUE;

}



/*---------------------------------------------------------------------------

 *            HF_Register()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Initializes all necessary data structures, registers with

 *            RFCOMM and SDP. 

 *

 * Return:    (See hf.h)

 */

BtStatus HF_Register(HfChannel_struct *pChannel, HfCallback callback, BOOL bHeadset)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/ 

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);

    if ( pChannel == NULL )
        return BT_STATUS_FAILED;

	kal_trace(TRACE_GROUP_5, BT_HF_API_REGISTER, pChannel, callback, bHeadset ? 1 : 0);

    

    OS_LockStack();

    if (IsNodeOnList(&HF(channelList), &pChannel->node)) 

    { 

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_ALREADY_REGISTERED, pChannel);

        status = BT_STATUS_IN_USE;           

        goto exit;

    }

    

    OS_MemSet((U8*)pChannel, 0, sizeof(HfChannel_struct));

    pChannel->type = bHeadset ? HF_SERVICE_TYPE_HS : HF_SERVICE_TYPE_HF;



    /* Register SDP record. if HF(hfSdpRecord).num is 0 that 

        means HF SDP is not registered yet */

    if (HF(hfSdpRecord)[pChannel->type].num == 0) 

    {

        HF(hfService)[pChannel->type].serviceId = 0;

        status = HfSdpRegisterService(bHeadset);

        if (status != BT_STATUS_SUCCESS) 

        {          

			kal_trace(TRACE_GROUP_5, BT_HF_ERROR_1, __BT_FILE__, __LINE__, status);

            goto exit;

        }

        

#if BT_SECURITY == XA_ENABLED

        HF(hfSecRecord)[pChannel->type].id      =  SEC_RFCOMM_ID;

        HF(hfSecRecord)[pChannel->type].channel = HF(hfService)[pChannel->type].serviceId;

        HF(hfSecRecord)[pChannel->type].level   = HF_SECURITY_SETTINGS;    

        status = SEC_Register(&(HF(hfSecRecord)[pChannel->type]));

        if (status != BT_STATUS_SUCCESS) 

        {

			kal_trace(TRACE_GROUP_5, BT_HF_ERROR_1, __BT_FILE__, __LINE__, status);

            HfSdpDeregisterService(bHeadset);

            goto exit;

        }

#endif			

    }



    /* Each HfChannel_struct allocates a RfChannel and register to RFCOMM  */

    if (status == BT_STATUS_SUCCESS)

    {

        /* Register a channel to receive Handsfree connections */

        pChannel->pRfChannel = HfAllocRfChannel();

        if (pChannel->pRfChannel) 

        {

            status = HfRegisterRfServerChannel(pChannel);

        } 

        else 

        {

            status = BT_STATUS_NO_RESOURCES;            

        }

        

        if (status != BT_STATUS_SUCCESS)

        {

			kal_trace(TRACE_GROUP_5, BT_HF_ERROR_1, __BT_FILE__, __LINE__, status);

            if (pChannel->pRfChannel)

            {

                HfFreeRfChannel(pChannel->pRfChannel);

                pChannel->pRfChannel = NULL;

            }

            if (HF(regCount[pChannel->type]) == 0)

            {

#if BT_SECURITY == XA_ENABLED                

                SEC_Unregister(&(HF(hfSecRecord)[pChannel->type]));

#endif	

                HfSdpDeregisterService(bHeadset);

            }

            goto exit;

        }

    }



    if (status == BT_STATUS_SUCCESS) 

    {

        pChannel->callback = callback;

        pChannel->bTxInProgress = FALSE;

        HfResetTxBuffer(pChannel);

        CMGR_RegisterHandler(&pChannel->cmgrHandler, HfCmgrCallback);

        HF(regCount[pChannel->type]++);

        InsertTailList(&HF(channelList), &pChannel->node);            

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_REGISTER_SUCCESS, pChannel);

    }

 

exit:

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_REGISTER_RET, status);        

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_Deregister()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Deregisters with RFCOMM and SDP. 

 *

 * Return:    (See hf.h)

 */

BtStatus HF_Deregister(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus   status = BT_STATUS_SUCCESS;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/   

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    if ( pChannel == NULL )
        return BT_STATUS_FAILED;

	kal_trace(TRACE_GROUP_5, BT_HF_API_DEREGISTER, pChannel);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));

    

    OS_LockStack();		

    if (!IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        status = BT_STATUS_NOT_FOUND;   

        goto exit;        

    }



    if (HfGetState(pChannel) != HF_STATE_CLOSED)

    {

        kal_trace(TRACE_GROUP_5, BT_HF_INFO_DEFER_DEREGISTERATION, pChannel, HfGetState(pChannel));

        if (HfGetState(pChannel) == HF_STATE_OPEN)

        {

            HfChangeState(pChannel, HF_STATE_DISC_PENDING);

            if (CMGR_IsAudioUp(&pChannel->cmgrHandler))

            {

                status = CMGR_RemoveAudioLink(&pChannel->cmgrHandler);

                kal_trace(TRACE_GROUP_5, BT_HF_FLOW_AUDIO_DISCONNECTING, status);

                if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))

                {

                    status = BT_STATUS_PENDING;

                    HfSetLinkFlags(pChannel, HF_LINK_FLAG_SCO_DISCONNECTING);            

                }      

                else if (status == BT_STATUS_SUCCESS)

                {

                    status = CMGR_DeregisterScoHandler(&pChannel->cmgrHandler);

                	kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_DEREGISTERING, status);                

                    status = RF_CloseChannel(pChannel->pRfChannel);

                    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DISCONNECTING, status);                    

                }            

            }      

            else

            {

                status = CMGR_DeregisterScoHandler(&pChannel->cmgrHandler);

            	kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_DEREGISTERING, status);                

                status = RF_CloseChannel(pChannel->pRfChannel);

                kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DISCONNECTING, status);                

            }            

        }

        status = BT_STATUS_CONNECTION_EXIST;

        goto exit;             

    }



    Assert(HF(regCount[pChannel->type]));

    HfDeregisterRfServerChannel(pChannel);

	if (pChannel->pRfChannel)

	{

		HfFreeRfChannel(pChannel->pRfChannel);

		pChannel->pRfChannel = NULL;

	}

    CMGR_DeregisterHandler(&pChannel->cmgrHandler);

    RemoveEntryList(&pChannel->node);

    

    if (!(--HF(regCount[pChannel->type]))) 

    {

#if BT_SECURITY == XA_ENABLED

        SEC_Unregister(&(HF(hfSecRecord)[pChannel->type]));

#endif            

        HfSdpDeregisterService((BOOL)(pChannel->type == HF_SERVICE_TYPE_HS));

    }  



exit:    

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_DEREGISTER_RET, status);      

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_CreateServiceLink()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Opens a service level connection. 

 *

 * Return:    (See hf.h)

 */

BtStatus HF_CreateServiceLink(HfChannel_struct *pChannel, BD_ADDR *pAddr) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    if ( pChannel == NULL || pAddr )
        return BT_STATUS_NOT_FOUND;

    kal_trace(TRACE_GROUP_5, BT_HF_API_CREATE_SERVICE_LINK, 

            				 pChannel,

            				 pAddr->addr[0], 

            				 pAddr->addr[1], 

            				 pAddr->addr[2], 

            				 pAddr->addr[3], 

            				 pAddr->addr[4], 

            				 pAddr->addr[5]);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif    



    OS_LockStack();

    if (!IsNodeOnList(&HF(channelList), &pChannel->node))    

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        goto exit;         

    }



    if (HfGetState(pChannel) != HF_STATE_CLOSED)

    {     

		kal_trace(TRACE_GROUP_5, BT_HF_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, pChannel, HfGetState(pChannel));

        status = BT_STATUS_IN_USE;   

        goto exit;            

    }



    status = HfDeregisterRfServerChannel(pChannel);

    if (status != BT_STATUS_SUCCESS)

    {

        goto exit;

    }

    

    status = HfOpenCmgrChannel(pChannel, pAddr);

    if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_1, __BT_FILE__, __LINE__, status);

		HfRegisterRfServerChannel(pChannel);

        goto exit;

    }



    if (status == BT_STATUS_PENDING)

    {

	    HfChangeState(pChannel, HF_STATE_CONN_PENDING);

        goto exit;        

    }

    

    status = HfSdpStartQuery(pChannel, BSQM_FIRST);

    if (status == BT_STATUS_PENDING)

    {

        HfChangeState(pChannel, HF_STATE_CONN_PENDING);

    }

    else

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_1, __BT_FILE__, __LINE__, status);

        HfCloseCmgrChannel(pChannel);

        HfRegisterRfServerChannel(pChannel);

    }



exit:

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_CREATE_SERVICE_LINK_RET, status);      

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_DisconnectServiceLink()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Close a service level connection. 

 *

 * Return:    (See hf.h)

 */

BtStatus HF_DisconnectServiceLink(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    if ( pChannel == NULL )
        return BT_STATUS_NOT_FOUND;

    kal_trace(TRACE_GROUP_5, BT_HF_API_DISCONNECT_SERVICE_LINK, pChannel, HfGetState(pChannel));

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif  



    OS_LockStack();

    if (IsNodeOnList(&HF(channelList), &pChannel->node))

    {

        switch (HfGetState(pChannel))

        {

        case HF_STATE_CLOSED:

        case HF_STATE_CONN_PENDING:            

            status = BT_STATUS_NO_CONNECTION;

            break;        

        case HF_STATE_DISC_PENDING:

            status = BT_STATUS_IN_PROGRESS;

            break;            

        case HF_STATE_OPEN:

            HfChangeState(pChannel, HF_STATE_DISC_PENDING);

            if (CMGR_IsAudioUp(&pChannel->cmgrHandler))

            {

                status = CMGR_RemoveAudioLink(&pChannel->cmgrHandler);

                kal_trace(TRACE_GROUP_5, BT_HF_FLOW_AUDIO_DISCONNECTING, status);

                if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))

                {

                    status = BT_STATUS_PENDING;

                    HfSetLinkFlags(pChannel, HF_LINK_FLAG_SCO_DISCONNECTING);            

                }      

                else if (status == BT_STATUS_SUCCESS)

                {

                    status = CMGR_DeregisterScoHandler(&pChannel->cmgrHandler);

                	kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_DEREGISTERING, status);                

                    status = RF_CloseChannel(pChannel->pRfChannel);

                    kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DISCONNECTING, status);                    

                }            

            }      

            else

            {

                status = CMGR_DeregisterScoHandler(&pChannel->cmgrHandler);

            	kal_trace(TRACE_GROUP_5, BT_HF_FLOW_SCO_DEREGISTERING, status);                

                status = RF_CloseChannel(pChannel->pRfChannel);

                kal_trace(TRACE_GROUP_5, BT_HF_FLOW_RF_DISCONNECTING, status);                

            }            

            break;

        default:           

			kal_trace(TRACE_GROUP_5, BT_HF_ERROR_UNEXPECTED_STATE, __BT_FILE__, __LINE__, pChannel, HfGetState(pChannel));

            status = BT_STATUS_FAILED; 

            break;

        }	

    }

	else

	{

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

	}

    

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_DISCONNECT_SERVICE_LINK_RET, status);     

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_CreateAudioLink()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Creates an audio (SCO) link to the audio gateway.

 *

 * Return:    (See hf.h)

 */

BtStatus HF_CreateAudioLink(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/  

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    if ( pChannel == NULL )
        return BT_STATUS_NOT_FOUND;

    kal_trace(TRACE_GROUP_5, BT_HF_API_CREATE_AUDIO_LINK, 

        pChannel, HfGetState(pChannel), CMGR_IsAudioUp(&pChannel->cmgrHandler) ? 1 : 0);



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif      



    OS_LockStack();

    if (!IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        goto exit;           

	}



    if (HfGetState(pChannel) != HF_STATE_OPEN)

    {		

		kal_trace(TRACE_GROUP_5, BT_HF_WARN, __BT_FILE__, __LINE__);

        status = BT_STATUS_NO_CONNECTION;

        goto exit;          

    }    



    if (CMGR_IsAudioUp(&pChannel->cmgrHandler))

    {

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_AUDIO_EXIST);

        status = BT_STATUS_SUCCESS;

    }

    else if (HfGetLinkFlags(pChannel) & HF_LINK_FLAG_SCO_CONNECTING)

    {

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_AUDIO_CONENCTING);

        status = BT_STATUS_IN_PROGRESS;

    }

    else if (HfGetLinkFlags(pChannel) & HF_LINK_FLAG_SCO_DISCONNECTING)

    {

        kal_trace(TRACE_GROUP_5, BT_HF_INFO_AUDIO_DISCONENCTING);

        status = BT_STATUS_FAILED;

    }            

    else

    {

        status = CMGR_CreateAudioLink(&pChannel->cmgrHandler);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_AUDIO_CONNECTING, status);

        if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))

        {

            status = BT_STATUS_PENDING;

            HfSetLinkFlags(pChannel, HF_LINK_FLAG_SCO_CONNECTING);

        }

    }    



exit:

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_CREATE_AUDIO_LINK_RET, status);         

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_DisconnectAudioLink()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Releases the audio connection with the audio gateway.

 *

 * Return:    (See hf.h)

 */

BtStatus HF_DisconnectAudioLink(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    kal_trace(TRACE_GROUP_5, BT_HF_API_DISCONNECT_AUDIO_LINK, pChannel, HfGetState(pChannel), CMGR_IsAudioUp(&pChannel->cmgrHandler) ? 1 : 0);



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif  



    OS_LockStack();

    if (!IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        goto exit;          

	}



    if (HfGetState(pChannel) != HF_STATE_OPEN)

    {		

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_NO_CONNECTION);

        status = BT_STATUS_NO_CONNECTION;        

        goto exit;          

    }  



    if (!CMGR_IsAudioUp(&pChannel->cmgrHandler))

    {

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_NO_AUDIO_CONNECTION);        

        status = BT_STATUS_SUCCESS;

    }

    else if (HfGetLinkFlags(pChannel) & HF_LINK_FLAG_SCO_CONNECTING)

    {

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_AUDIO_CONENCTING);        

        status = BT_STATUS_FAILED;

    }

    else if (HfGetLinkFlags(pChannel) & HF_LINK_FLAG_SCO_DISCONNECTING)

    {

		kal_trace(TRACE_GROUP_5, BT_HF_INFO_AUDIO_DISCONENCTING);        

        status = BT_STATUS_IN_PROGRESS;

    }     

    else

    {

        status = CMGR_RemoveAudioLink(&pChannel->cmgrHandler);

        kal_trace(TRACE_GROUP_5, BT_HF_FLOW_AUDIO_DISCONNECTING, status);

        if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))

        {

            status = BT_STATUS_PENDING;

            HfSetLinkFlags(pChannel, HF_LINK_FLAG_SCO_DISCONNECTING);            

        }

    }    



exit:

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_DISCONNECT_AUDIO_LINK_RET, status);     

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_AcceptConnect()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Accept incoming connection request.

 *

 * Return:    (See hf.h)

 */

BtStatus HF_AcceptConnect(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    if ( pChannel == NULL )
        return BT_STATUS_NOT_FOUND;

    kal_trace(TRACE_GROUP_5, BT_HF_API_ACCEPT_CONNECT, pChannel);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif      



    OS_LockStack();

    if (!IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        goto exit;     

    }



    if (!(HfGetLinkFlags(pChannel) & HF_LINK_FLAG_CONN_INCOMING))

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_NO_INCOMING_CONNECTION, pChannel);    

        status = BT_STATUS_FAILED;

        goto exit; 

    }



    status = HfOpenCmgrChannel(pChannel, &pChannel->bdAddr);

    if (status == BT_STATUS_SUCCESS)

    {

        status = RF_AcceptChannel(pChannel->pRfChannel);   

        if (status != BT_STATUS_PENDING) 

        {

			kal_trace(TRACE_GROUP_5, BT_HF_WARN_ACCEPT_CHANNEL_FAIL, status);			

        }         

    }

    else

    {

        status = RF_RejectChannel(pChannel->pRfChannel);

        if (status != BT_STATUS_PENDING) 

        {

			kal_trace(TRACE_GROUP_5, BT_HF_WARN_REJECT_CHANNEL_FAIL, status);

        }        

    }



exit:

    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_ACCEPT_CONNECT_RET, status);     

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_RejectConnect()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Reject incoming connection request.

 *

 * Return:    (See hf.h)

 */

BtStatus HF_RejectConnect(HfChannel_struct *pChannel) 

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_NOT_FOUND;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    kal_trace(TRACE_GROUP_5, BT_HF_API_REJECT_CONNECT, pChannel);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif      



    OS_LockStack();

    if (!IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        goto exit;   

    }



    if (!(HfGetLinkFlags(pChannel) & HF_LINK_FLAG_CONN_INCOMING))

    {

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_NO_INCOMING_CONNECTION, pChannel);        

        status = BT_STATUS_FAILED;

        goto exit; 

    }    



    status = RF_RejectChannel(pChannel->pRfChannel);

    if (status != BT_STATUS_PENDING) 

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_REJECT_CHANNEL_FAIL, status);

    }      



exit:

    OS_UnlockStack();     

	kal_trace(TRACE_GROUP_5, BT_HF_API_REJECT_CONNECT_RET, status);        

    return status;

}



/*---------------------------------------------------------------------------

 *            HF_SendData()

 *---------------------------------------------------------------------------

 *

 * Synopsis:  Generic routine for sending at commands.

 *

 * Return:    (See hf.h)

 */

BtStatus HF_SendData(HfChannel_struct *pChannel, U8 *pData, U16 len)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status = BT_STATUS_SUCCESS;

	U16 writeLen    = 0;    



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/  

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pChannel != NULL);    

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pData != NULL);  

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, len != 0);      

    kal_trace(TRACE_GROUP_5, BT_HF_API_SEND_DATA, pChannel, pData, len);

	kal_trace(TRACE_GROUP_5, BT_HF_INFO_CHANNEL_STATE, 

        HfGetState(pChannel), HfGetLinkFlags(pChannel), HfGetQueryFlags(pChannel));



#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    if (pChannel->bSuspended)

    {

        /* Shall never be here */    

        kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_SUSPENDED, pChannel);

        return BT_STATUS_RESTRICTED;

    }

#endif      



    OS_LockStack();

    if (IsNodeOnList(&HF(channelList), &pChannel->node)) 

    {

        if (HfGetState(pChannel) == HF_STATE_OPEN)

        {

            writeLen = HfWriteTxBuffer(pChannel, pData, len);

            if (writeLen < len)

            {

    	        kal_trace(TRACE_GROUP_5, BT_HF_WARN_NO_RESOURCES, pChannel);                

                status = BT_STATUS_NO_RESOURCES;

            }

        }

        else

        {

    	    kal_trace(TRACE_GROUP_5, BT_HF_WARN_NO_CONNECTION, pChannel);    

            status = BT_STATUS_NO_CONNECTION;

        }      

    }    

    else

    {

		kal_trace(TRACE_GROUP_5, BT_HF_WARN_CHANNEL_NOT_REGISTERED, pChannel);

        status = BT_STATUS_NOT_FOUND;   

    }



    OS_UnlockStack();

	kal_trace(TRACE_GROUP_5, BT_HF_API_SEND_DATA_RET, status);     

    return status; 

}

#endif /* __BT_HF_PROFILE__ */

