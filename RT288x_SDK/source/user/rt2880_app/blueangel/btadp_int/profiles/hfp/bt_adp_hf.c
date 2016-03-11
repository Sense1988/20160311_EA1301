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

 * bt_adp_hf.c

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file is used to handle message and event for HFP HF role

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

#include "hf.h"

#include "bt_adp_hf.h"

#include "bluetooth_hf_message.h"

#include "bluetooth_hf_struct.h"



BOOL g_bHfInited = FALSE;

ListEntry g_channel_list;



static void hfa_app_callback(HfChannel_struct *pChannel, HfCallbackParms_struct *pParms);



/*****************************************************************************

* Internal functions

****************************************************************************/

/*****************************************************************************

 * FUNCTION

 *  hfa_send_msg

 * DESCRIPTION

 *  This function is to send common message

 * PARAMETERS

 *  msg         [IN]  message type

 *  dstMod      [IN]  dest module type

 *  sap         [IN]  sap type

 *  pLocalPara  [IN]  the pointer to local para

 *  pPeerBuff   [IN]  the pointer to peer buff

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_send_msg(

                msg_type           msg,

                module_type        dstMod,

                sap_type           sap,

                local_para_struct *pLocalPara,

                peer_buff_struct  *pPeerBuff)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if (pLocalPara != NULL)

    {

        BT_SendMessage(msg, dstMod, pLocalPara, pLocalPara->msg_len);

    }

    else

    {

        BT_SendMessage(msg, dstMod, NULL, 0);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_send_channel_msg

 * DESCRIPTION

 *  This function is to send common message according to channel context

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  msg          [IN]  message type 

 *  pPara        [IN]  the pointer to local para

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_send_channel_msg(

                HfAdpChannel_struct *pAdpChannel, 

                msg_type             msg,

                void                *pPara)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    hfa_send_msg(msg, pAdpChannel->registered_module, BT_APP_SAP, pPara, NULL);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_connect_timerout

 * DESCRIPTION

 *  This function is to handle HFP connection timeout

 * PARAMETERS

 *  pTimer     [IN]  the pointer of EvmTimer

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_connect_timerout(EvmTimer *pTimer)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfAdpChannel_struct *pAdpChannel = (HfAdpChannel_struct*)pTimer->context;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    kal_trace(TRACE_GROUP_5, BT_HF_ADP_CONNECT_TIMER_TRIGGERED);

	HF_DisconnectServiceLink(&pAdpChannel->channel);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_activate_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_ACTIVATE_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message     

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_activate_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                  = BT_STATUS_NO_RESOURCES;

    HfAdpChannel_struct *pAdpChannel = NULL;

    bt_hf_activate_req_struct *req_p = NULL;

    bt_hf_activate_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    req_p = (bt_hf_activate_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = (HfAdpChannel_struct*)get_ctrl_buffer(sizeof(HfAdpChannel_struct));

    if (pAdpChannel != NULL)

    {

        OS_MemSet((U8*)pAdpChannel, 0, sizeof(HfAdpChannel_struct));

		pAdpChannel->bHeadset = req_p->bHeadset;

        status = HF_Register(&pAdpChannel->channel, hfa_app_callback, req_p->bHeadset);

        if (status == BT_STATUS_SUCCESS)

        {

            pAdpChannel->pUserContext = req_p->pReqContext;

            pAdpChannel->registered_module = ilm_ptr->src_mod_id;

            InsertTailList(&g_channel_list, &pAdpChannel->node);

	        /* Initialize timer */

	        pAdpChannel->timer.context = (void*)pAdpChannel;

        }

        else

        {

            free_ctrl_buffer((void*)pAdpChannel);

            pAdpChannel = NULL;

        }

    }



    /* send active confirm message */

    cnf_p = (bt_hf_activate_cnf_struct*)construct_local_para(sizeof(bt_hf_activate_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;

    cnf_p->result      = status;    

    hfa_send_msg(MSG_ID_BT_HF_ACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_deactivate_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_DEACTIVATE_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message     

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_deactivate_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                    = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel   = NULL;    

    bt_hf_deactivate_req_struct *req_p = NULL;

    bt_hf_deactivate_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    req_p = (bt_hf_deactivate_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct deactivate confirm message */

    cnf_p = (bt_hf_deactivate_cnf_struct*)construct_local_para(sizeof(bt_hf_deactivate_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;

    

    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_Deregister(&pAdpChannel->channel);

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_DEACTIVATE_CNF, (local_para_struct*)cnf_p);

        if (status == BT_STATUS_CONNECTION_EXIST)

        {

            pAdpChannel->bDeregistered = TRUE;

        }

        else

        {

            RemoveEntryList(&pAdpChannel->node);  

            free_ctrl_buffer((void*)pAdpChannel);

            pAdpChannel = NULL;            

        }

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_DEACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_connect_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_CONNECT_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_connect_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                  = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel = NULL;    

    bt_hf_connect_req_struct *req_p  = NULL;

    bt_hf_connect_cnf_struct *cnf_p  = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    req_p = (bt_hf_connect_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct connect confirm message */

    cnf_p = (bt_hf_connect_cnf_struct*)construct_local_para(sizeof(bt_hf_connect_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;

    

    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_CreateServiceLink(&pAdpChannel->channel, (BD_ADDR*)&req_p->bt_addr);

        if (status == BT_STATUS_PENDING)

        {

            pAdpChannel->timer.func = hfa_handle_connect_timerout;

            EVM_StartTimer(&pAdpChannel->timer, HF_CONNECT_TIMEOUT);

            kal_trace(TRACE_GROUP_5, BT_HF_ADP_CONNECT_TIMER_STARTED);   

        }     

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_CONNECT_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_disconnect_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_DISCONNECT_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_disconnect_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                    = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel   = NULL;       

    bt_hf_disconnect_req_struct *req_p = NULL;

    bt_hf_disconnect_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    req_p = (bt_hf_disconnect_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct disconnect confirm message */

    cnf_p = (bt_hf_disconnect_cnf_struct*)construct_local_para(sizeof(bt_hf_disconnect_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;



    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_DisconnectServiceLink(&pAdpChannel->channel);

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_DISCONNECT_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_sco_connect_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_SCO_CONNECT_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_sco_connect_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                     = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel    = NULL;    

    bt_hf_sco_connect_req_struct *req_p = NULL;

    bt_hf_sco_connect_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    req_p = (bt_hf_sco_connect_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct sco connect confirm message */

    cnf_p = (bt_hf_sco_connect_cnf_struct*)construct_local_para(sizeof(bt_hf_sco_connect_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;



    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_CreateAudioLink(&pAdpChannel->channel);

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_SCO_CONNECT_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_SCO_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfg_adp_sco_disconnect_req_hdler

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_SCO_DISCONNECT_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message  

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_sco_disconnect_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                        = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel       = NULL;    

    bt_hf_sco_disconnect_req_struct *req_p = NULL;

    bt_hf_sco_disconnect_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    req_p = (bt_hf_sco_disconnect_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct return message */

    cnf_p = (bt_hf_sco_disconnect_cnf_struct*)construct_local_para(sizeof(bt_hf_sco_disconnect_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;



    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_DisconnectAudioLink(&pAdpChannel->channel);

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_SCO_DISCONNECT_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_SCO_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_accept_channel_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_ACCEPT_CHANNEL_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_accept_channel_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                        = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel       = NULL;    

    bt_hf_accept_channel_req_struct *req_p = NULL;

    bt_hf_accept_channel_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    req_p = (bt_hf_accept_channel_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct accept channel confirm message */

    cnf_p = (bt_hf_accept_channel_cnf_struct*)construct_local_para(sizeof(bt_hf_accept_channel_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;



    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_AcceptConnect(&pAdpChannel->channel);

        if (status == BT_STATUS_PENDING)

        {

            pAdpChannel->timer.func = hfa_handle_connect_timerout;

            EVM_StartTimer(&pAdpChannel->timer, HF_CONNECT_ACCEPT_TIMEOUT);

            kal_trace(TRACE_GROUP_5, BT_HF_ADP_CONNECT_TIMER_STARTED);   

        }

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_ACCEPT_CHANNEL_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_ACCEPT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_reject_channel_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_REJECT_CHANNEL_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_reject_channel_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                        = BT_STATUS_NOT_FOUND;

    HfAdpChannel_struct *pAdpChannel       = NULL;    

    bt_hf_reject_channel_req_struct *req_p = NULL;

    bt_hf_reject_channel_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    req_p = (bt_hf_reject_channel_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;



    /* construct reject connect confirm message */

    cnf_p = (bt_hf_reject_channel_cnf_struct*)construct_local_para(sizeof(bt_hf_reject_channel_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;



    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_RejectConnect(&pAdpChannel->channel);

        cnf_p->result = status;

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_REJECT_CHANNEL_CNF, (local_para_struct*)cnf_p);

    }

    else

    {

        cnf_p->result = status;

        hfa_send_msg(MSG_ID_BT_HF_REJECT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_send_data_req

 * DESCRIPTION

 *  This function is to handle MSG_ID_BT_HF_SEND_DATA_REQ

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message       

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_send_data_req(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    BtStatus status                   = BT_STATUS_NOT_FOUND;     

    HfAdpChannel_struct *pAdpChannel  = NULL;     

    bt_hf_send_data_req_struct *req_p = NULL;

    bt_hf_send_data_cnf_struct *cnf_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    req_p = (bt_hf_send_data_req_struct*)ilm_ptr->local_para_ptr;

    pAdpChannel = req_p->pContext;

    bt_prompt_trace(MOD_BT, "[HF][ADP] Send AT cmd : %s", req_p->data);

    if (IsNodeOnList(&g_channel_list, &pAdpChannel->node)) 

    {

        status = HF_SendData(&pAdpChannel->channel, (U8 *)req_p->data, req_p->size);

    }

    else

    {

        pAdpChannel = NULL;

    }



    /* construct send data confirm message */

    cnf_p = (bt_hf_send_data_cnf_struct*)construct_local_para(sizeof(bt_hf_send_data_cnf_struct), TD_UL);

    cnf_p->pContext    = pAdpChannel;

    cnf_p->pReqContext = req_p->pReqContext;

    cnf_p->result      = status;

    if (pAdpChannel != NULL)

    {

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_SEND_DATA_CNF, (local_para_struct*)cnf_p);        

    }

    else

    {

        hfa_send_msg(MSG_ID_BT_HF_SEND_DATA_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_p, NULL);

    }   

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_auth_req

 * DESCRIPTION

 *  This function is to handle HF_EVENT_AUTH_REQ

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters 

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_auth_req(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    bt_hf_connect_req_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    ind_p = (bt_hf_connect_req_ind_struct*)construct_local_para(sizeof(bt_hf_connect_req_ind_struct), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    OS_MemCopy((U8*)&ind_p->bt_addr, (const U8*)pParms->p.pAddr, sizeof(BD_ADDR));

    hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_CONNECT_REQ_IND, (local_para_struct*)ind_p);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_service_connected

 * DESCRIPTION

 *  This function is to handle HF_EVENT_SERVICE_CONNECTED

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters   

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_service_connected(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    bt_hf_connected_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    ind_p = (bt_hf_connected_ind_struct*)construct_local_para(sizeof(bt_hf_connected_ind_struct), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    OS_MemCopy((U8*)&ind_p->bt_addr, (const U8*)pParms->p.pAddr, sizeof(BD_ADDR));

    hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_CONNECTED_IND, (local_para_struct*)ind_p);   

    if (pAdpChannel->timer.func)

    {

        EVM_CancelTimer(&pAdpChannel->timer);

        pAdpChannel->timer.func = NULL;        

        kal_trace(TRACE_GROUP_5, BT_HF_ADP_CONNECT_TIMER_CANCELED);           

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_service_disconnected

 * DESCRIPTION

 *  This function is to handle HF_EVENT_SERVICE_DISCONNECTED

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters   

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_service_disconnected(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    bt_hf_disconnected_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    ind_p = (bt_hf_disconnected_ind_struct*)construct_local_para(sizeof(bt_hf_disconnected_ind_struct), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    if (pParms->p.pAddr)

    {

        OS_MemCopy((U8*)&ind_p->bt_addr, (const U8*)pParms->p.pAddr, sizeof(BD_ADDR));    

    }

    else

    {

        OS_MemSet((U8*)&ind_p->bt_addr, 0, sizeof(BD_ADDR));

    }



    if (pAdpChannel->timer.func)

    {

        EVM_CancelTimer(&pAdpChannel->timer);

        pAdpChannel->timer.func = NULL;        

        kal_trace(TRACE_GROUP_5, BT_HF_ADP_CONNECT_TIMER_CANCELED);           

    }      



    if (pAdpChannel->bDeregistered)

    {

        /* If the handle is deregistered, deregister the profile handle */

        kal_trace(TRACE_GROUP_5, BT_HF_ADP_DEFERED_DEREGISTERATION, pAdpChannel);

        HF_Deregister(&pAdpChannel->channel);

        RemoveEntryList(&pAdpChannel->node);  

        free_ctrl_buffer((void*)pAdpChannel);

        pAdpChannel = NULL;           

    }

    else

    {

        /* Send disconnected ind only if the handle is not deregistered yet */

        hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_DISCONNECTED_IND, (local_para_struct*)ind_p);

    }

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_audio_connected

 * DESCRIPTION

 *  This function is to handle HF_EVENT_AUDIO_CONNECTED

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters   

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_audio_connected(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    bt_hf_sco_connected_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    ind_p = (bt_hf_sco_connected_ind_struct*)construct_local_para(sizeof(bt_hf_sco_connected_ind_struct), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    ind_p->status       = pParms->status;

    hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_SCO_CONNECTED_IND, (local_para_struct*)ind_p);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_audio_disconnected

 * DESCRIPTION

 *  This function is to handle HF_EVENT_AUDIO_DISCONNECTED

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters     

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_audio_disconnected(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    bt_hf_sco_disconnected_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/    

    ind_p = (bt_hf_sco_disconnected_ind_struct*)construct_local_para(sizeof(bt_hf_sco_disconnected_ind_struct), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_SCO_DISCONNECTED_IND, (local_para_struct*)ind_p);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_handle_data_ind

 * DESCRIPTION

 *  This function is to handle HF_EVENT_DATA_IND

 * PARAMETERS

 *  pAdpChannel  [IN]  the pointer to HF adp channel    

 *  pParms       [IN]  the pointer to callback parameters    

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_handle_data_ind(HfAdpChannel_struct *pAdpChannel, HfCallbackParms_struct *pParms)

{    

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    U16 len = pParms->p.pAtResult->dataLen;

    bt_hf_data_ind_struct *ind_p = NULL;



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/  

    if (len > HF_MAX_AT_STRING_LEN)

    {

        len = HF_MAX_AT_STRING_LEN;

    }



	bt_prompt_trace(MOD_BT, "[HF][ADP] Receive AT result code,org size=%d,cursize = %d", pParms->p.pAtResult->dataLen,len); 

    ind_p = (bt_hf_data_ind_struct*)construct_local_para(

            (U16)(sizeof(bt_hf_data_ind_struct) - HF_MAX_AT_STRING_LEN + len + 1), TD_UL);

    ind_p->pContext     = (void*)pAdpChannel;

    ind_p->pUserContext = pAdpChannel->pUserContext;

    ind_p->size         = len;

    OS_MemCopy((U8*)ind_p->data, (const U8*)pParms->p.pAtResult->pData, len);

    ind_p->data[len] = '\0';

    bt_prompt_trace(MOD_BT, "[HF][ADP] Receive AT result code : %s ", ind_p->data); 

	

    hfa_send_channel_msg(pAdpChannel, MSG_ID_BT_HF_DATA_IND, (local_para_struct*)ind_p);

}



/*****************************************************************************

 * FUNCTION

 *  hfa_app_callback

 * DESCRIPTION

 *  This function is to handle HFP event from profile level

 * PARAMETERS

 *  pChannel     [IN]  the pointer to HF channel    

 *  pParms       [IN]  the pointer to callback parameters   

 * RETURNS

 *  void

 *****************************************************************************/

static void hfa_app_callback(HfChannel_struct *pChannel, HfCallbackParms_struct *pParms)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/

    HfAdpChannel_struct *pAdpChannel = ContainingRecord(pChannel, HfAdpChannel_struct, channel);



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if (!IsNodeOnList(&g_channel_list, &pAdpChannel->node))

    {

        kal_trace(TRACE_GROUP_5, BT_HF_ADP_WARN_CHANNEL_NOT_FOUND, pAdpChannel);

        return;

    }

    

    switch (pParms->event)

    {  

    case HF_EVENT_AUTH_REQ:

        hfa_handle_auth_req(pAdpChannel, pParms);

        break;

    case HF_EVENT_SERVICE_CONNECTED:

        hfa_handle_service_connected(pAdpChannel, pParms);

        break;        

    case HF_EVENT_SERVICE_DISCONNECTED:

        hfa_handle_service_disconnected(pAdpChannel, pParms);

        break;        

    case HF_EVENT_AUDIO_CONNECTED:

        hfa_handle_audio_connected(pAdpChannel, pParms);

        break;        

    case HF_EVENT_AUDIO_DISCONNECTED:

        hfa_handle_audio_disconnected(pAdpChannel, pParms);

        break;              

    case HF_EVENT_DATA_IND:

        hfa_handle_data_ind(pAdpChannel, pParms);

        break;        

    default:

        kal_trace(TRACE_GROUP_5, BT_HF_ADP_UNSUPPORTED_EVENT);

        break;

    }

}



/*****************************************************************************

* External functions 

****************************************************************************/

/*****************************************************************************

 * FUNCTION

 *  hf_adp_init

 * DESCRIPTION

 *  This function is to init adp layer of HFP HF role

 * PARAMETERS

 *  void

 * RETURNS

 *  void

 *****************************************************************************/

BOOL hf_adp_init(void)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    if (g_bHfInited == FALSE)

    {

        g_bHfInited = TRUE;

        InitializeListHead(&g_channel_list);

        return HF_Init();

    }

    else

    {

        HfAdpChannel_struct *pAdpChannel = NULL;



        /* Unregister all contexts */

        pAdpChannel = (HfAdpChannel_struct*)GetHeadList(&g_channel_list);

        while (&pAdpChannel->node != &g_channel_list)

        {

            HF_Deregister(&pAdpChannel->channel);

            pAdpChannel = (HfAdpChannel_struct*)GetHeadList(&pAdpChannel->node);

        }



        /* Register all contexts again */

        pAdpChannel = (HfAdpChannel_struct*)GetHeadList(&g_channel_list);

        while (&pAdpChannel->node != &g_channel_list)

        {

            HF_Register(&pAdpChannel->channel, hfa_app_callback, pAdpChannel->bHeadset);

            pAdpChannel = (HfAdpChannel_struct*)GetHeadList(&pAdpChannel->node);

        }

    }



    return TRUE;    

}



/*****************************************************************************

 * FUNCTION

 *  hf_adp_handler

 * DESCRIPTION

 *  This function is to handle HFP messages sent from other tasks

 * PARAMETERS

 *  ilm_ptr     [IN]  inter layer message     

 * RETURNS

 *  void

 *****************************************************************************/

void hf_adp_handler(ilm_struct *ilm_ptr)

{

    /*----------------------------------------------------------------*/

    /* Local Variables                                                */

    /*----------------------------------------------------------------*/



    /*----------------------------------------------------------------*/

    /* Code Body                                                      */

    /*----------------------------------------------------------------*/

    switch (ilm_ptr->msg_id)

    {

    case MSG_ID_BT_HF_ACTIVATE_REQ:

        hfa_handle_activate_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_DEACTIVATE_REQ:

        hfa_handle_deactivate_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_CONNECT_REQ:

        hfa_handle_connect_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_DISCONNECT_REQ:

        hfa_handle_disconnect_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_SCO_CONNECT_REQ:

        hfa_handle_sco_connect_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_SCO_DISCONNECT_REQ:

        hfa_handle_sco_disconnect_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_ACCEPT_CHANNEL_REQ:

        hfa_handle_accept_channel_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_REJECT_CHANNEL_REQ:

        hfa_handle_reject_channel_req(ilm_ptr);

        break;

    case MSG_ID_BT_HF_SEND_DATA_REQ:

        hfa_handle_send_data_req(ilm_ptr);

        break;    

    default:

        break;

    }

}

#endif /* __BT_HF_PROFILE__ */

