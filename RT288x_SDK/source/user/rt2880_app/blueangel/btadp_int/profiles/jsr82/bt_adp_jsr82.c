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


#include "bt_common.h"
#include "jsr82_session.h"
#include "bt_adp_jsr82.h"
#include "bt_adp_jsr82_event.h"
#if (BT_JSR82_ENABLED == 1)
#include "bluetooth_sdp_struct.h"
#include "appsdpdb.h"
#endif

#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)

void jsr82_adp_init(void)
{
	JSR82_Register(btmtk_adp_jsr82_int_event_callback);
#if (BT_JSR82_ENABLED == 1)
	BTAppSdpDbInit();
#endif
	BTJSR82_SessionApInit();
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_jsr82_handle_message
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_jsr82_handle_message(ilm_struct *ilm_ptr)
{

    switch (ilm_ptr->msg_id)
    {
#if 0	// These cases are not used currently.
        case MSG_ID_BT_JSR_DEACTIVE_REQ:
            BTJSR82_DeInitAll();
            JSR82_sendMsg(MSG_ID_BT_JSR_DEACTIVE_CNF, MOD_MMI, (local_para_struct*) NULL, NULL);
            break;
        case MSG_ID_BT_JSR82_RESTROE_ACL_REQ:
            bt_jsr82_HandleSessionApDisconnectAllAcl();
            break;
        case MSG_ID_BT_JSR_PANIC:
        {
            /* Add this handler for JVM to send this message to trigger panic mechanism to test its error handling: 2007-1210 */
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TRIGGER_PANIC_MECHANISM_BY_JVN_FOR_TESTING_ERROR_HANDLE);
            HCI_TransportReset();
            bma_panic();
        }
            break;
#endif
        case MSG_ID_BT_JSR82_ENABLE_SERVICE_REQ:
            bt_jsr82_HandleSessionApEnableService((bt_jsr82_enable_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TURNON_SERVICE_REQ:
            bt_jsr82_HandleSessionApTurnOnService((bt_jsr82_turnon_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TURNOFF_SERVICE_REQ:
            bt_jsr82_HandleSessionApTurnOffService((bt_jsr82_turnoff_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_CONNECT_REQ:
            bt_jsr82_HandleSessionApConnectReq((bt_jsr82_connect_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_CONNECT_RSP:
            bt_jsr82_HandleSessionApConnectRsp((bt_jsr82_connect_rsp_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_DISCONNECT_REQ:
            bt_jsr82_HandleSessionApDisConnectReq((bt_jsr82_disconnect_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TX_DATA_REQ:
            bt_jsr82_HandleSessionApTxReq((bt_jsr82_tx_data_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_DISABLE_SERVICE_REQ:
            bt_jsr82_HandleSessionApDisableService((bt_jsr82_disable_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_RX_DATA_RSP:
            bt_jsr82_HandleSessionApRxCfn((bt_jsr82_rx_data_rsp_struct*) ilm_ptr->local_para_ptr);
            break;
            /* Add by mtk01411:2007-0916 */
        case MSG_ID_BT_JSR82_SPP_GET_DATA_REQ:
            bt_jsr82_HandleSessionApGetSppDataReq((bt_jsr82_spp_get_data_req_struct*) ilm_ptr->local_para_ptr);
            break;
            /* Add by mtk01411: 2007-1119 */
        case MSG_ID_BT_JSR82_SET_ACL_SECURITY_REQ:
            bt_jsr82_HandleSetACLSecurityReq((bt_jsr82_set_acl_security_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_PUT_BYTES_REQ:
        	jsr82_session_PutBytes((bt_jsr82_put_bytes_req_struct*) ilm_ptr->local_para_ptr);
        	break;
        case MSG_ID_BT_JSR82_GET_BYTES_REQ:
        	jsr82_session_GetBytes((bt_jsr82_get_bytes_req_struct*) ilm_ptr->local_para_ptr);
        	break;
        case MSG_ID_BT_JSR82_DATA_AVAILABLE_IND_RSP:
        	jsr82_session_DataAvailableIndRsp((bt_jsr82_data_available_ind_rsp_struct*) ilm_ptr->local_para_ptr);
        	break;
        case MSG_ID_BT_JSR82_ASSIGN_BUFFER_REQ:
        	jsr82_session_AssignDeliverBufferReq((bt_jsr82_assign_buffer_req_struct*) ilm_ptr->local_para_ptr);
        	break;
    }
}
#endif


