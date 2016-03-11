
/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2005
 *
 *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_time.c
 *
 * Project:
 * --------
 * BT Project
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time Profile
 *
 * Author:
 * -------
 * Jacob Lee
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "bttypes.h"
#include "bt_common.h"

#include "bluetooth_struct.h"
#include "bluetooth_time_struct.h"
#include "times.h"
#include "timec.h"
#include "timeadapter.h"

#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)

// For now, there's noly one server application supported.
#ifdef __BT_TIMES_PROFILE__
TimeSApp g_timesapp[TIME_MAX_CLIENT_NUM];
#endif

#ifdef __BT_TIMEC_PROFILE__
TimeCApp g_timecapp[TIME_MAX_SERVER_NUM];
#endif

/* Send ILM message */
void time_send_msg(msg_type msg_id, local_para_struct *local_para,
		peer_buff_struct *peer_buff) {
	module_type dst_id = MOD_MMI;

	if (local_para != NULL) {
		BT_SendMessage(msg_id, dst_id, local_para, local_para->msg_len);
	} else {
		BT_SendMessage(msg_id, dst_id, NULL, 0);
	}
}

/********************************************************************************************
 * Functions forwarding Time Client events to MMI via ILMs
 ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
void timec_send_connect_cnf(U8 index, U8 rspcode) {
	bt_timec_connect_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_timec_connect_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMEC_CONNECT_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_disconnect_ind(U8 index, U8 rspcode) {
	bt_timec_disconnect_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_timec_disconnect_ind_struct), TD_CTRL);
	ind->index = index;
	ind->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMEC_DISCONNECT_IND, (local_para_struct *) ind, NULL);
}

void timec_send_get_cttime_cnf(U8 index, U8 rspcode) {
	bt_timec_get_cttime_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_cttime_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->year = pCntx->date_time[0];
		cnf->month = (U8) pCntx->date_time[1];
		cnf->day = (U8) pCntx->date_time[2];
		cnf->hours = (U8) pCntx->date_time[3];
		cnf->minutes = (U8) pCntx->date_time[4];
		cnf->seconds = (U8) pCntx->date_time[5];
		cnf->day_of_week = pCntx->day_of_week;
		cnf->frac256 = pCntx->frac;
		cnf->adjust_reason = pCntx->adj_reason;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_CTTIME_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_get_cttime_notify_cnf(U8 index, U8 rspcode) {
	bt_timec_get_cttime_notify_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_cttime_notify_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->notify_config = pCntx->cttime_notify_config;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_CTTIME_NOTIFY_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_set_cttime_notify_cnf(U8 index, U8 rspcode) {
	bt_timec_set_cttime_notify_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_set_cttime_notify_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_CTTIME_NOTIFY_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_get_local_time_info_cnf(U8 index, U8 rspcode) {
	bt_timec_get_local_time_info_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_local_time_info_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->time_zone = pCntx->time_zone;
		cnf->dst = pCntx->dst;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_LOCAL_TIME_INFO_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_get_ref_time_info_cnf(U8 index, U8 rspcode) {
	bt_timec_get_ref_time_info_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_ref_time_info_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->time_source = pCntx->time_source;
		cnf->accuracy = pCntx->accuracy;
		cnf->days_since_update = pCntx->days_since_update;
		cnf->hours_since_update = pCntx->hours_since_update;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_REF_TIME_INFO_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_get_dst_cnf(U8 index, U8 rspcode) {
	bt_timec_get_dst_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_dst_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->year = pCntx->date_time[0];
		cnf->month = (U8) pCntx->date_time[1];
		cnf->day = (U8) pCntx->date_time[2];
		cnf->hours = (U8) pCntx->date_time[3];
		cnf->minutes = (U8) pCntx->date_time[4];
		cnf->seconds = (U8) pCntx->date_time[5];
		cnf->dst = pCntx->dst;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_DST_CNF, (local_para_struct *) cnf, NULL);
}

void timec_sned_request_server_update_cnf(U8 index, U8 rspcode) {
	bt_timec_request_server_update_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_request_server_update_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMEC_REQUEST_SERVER_UPDATE_CNF, (local_para_struct *) cnf, NULL);
}

void timec_sned_cancel_server_update_cnf(U8 index, U8 rspcode) {
	bt_timec_cancel_server_update_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_cancel_server_update_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMEC_CANCEL_SERVER_UPDATE_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_get_server_update_status_cnf(U8 index, U8 rspcode) {
	bt_timec_get_server_update_status_cnf_struct *cnf;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	cnf = construct_local_para(sizeof(bt_timec_get_server_update_status_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	if (rspcode == BT_STATUS_SUCCESS) {
		cnf->cur_state = pCntx->update_state;
		cnf->result = pCntx->update_result;
	}

	time_send_msg((U16) MSG_ID_BT_TIMEC_GET_SERVER_UPDATE_STATUS_CNF, (local_para_struct *) cnf, NULL);
}

void timec_send_update_cttime_ind(U8 index) {
	bt_timec_update_cttime_ind_struct *ind;
	TimeCContext *pCntx = (TimeCContext *) g_timecapp[index].timecContext;

	ind = construct_local_para(sizeof(bt_timec_update_cttime_ind_struct), TD_CTRL);
	ind->index = index;
	ind->year = pCntx->date_time[0];
	ind->month = (U8) pCntx->date_time[1];
	ind->day = (U8) pCntx->date_time[2];
	ind->hours = (U8) pCntx->date_time[3];
	ind->minutes = (U8) pCntx->date_time[4];
	ind->seconds = (U8) pCntx->date_time[5];
	ind->day_of_week = pCntx->day_of_week;
	ind->frac256 = pCntx->frac;
	ind->adjust_reason = pCntx->adj_reason;

	time_send_msg((U16) MSG_ID_BT_TIMEC_UPDATE_CTTIME_IND, (local_para_struct *) ind, NULL);
}
#endif

/********************************************************************************************
 * Functions forwarding Time Server events to MMI via ILMs
 ********************************************************************************************/
 #ifdef __BT_TIMES_PROFILE__
void times_send_register_cnf(U8 index, U8 rspcode) {
	bt_times_register_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_register_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_REGISTER_CNF, (local_para_struct *) cnf, NULL);
}

void times_send_deregister_cnf(U8 index, U8 rspcode) {
	bt_times_deregister_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_deregister_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_DEREGISTER_CNF, (local_para_struct *) cnf, NULL);
}

void times_send_authorize_ind(U8 index, U8 *addr) {
	bt_times_authorize_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_authorize_ind_struct), TD_CTRL);
	ind->index = index;
	memset(ind->addr, 0, 6);
	if (addr != NULL) {
		memcpy(ind->addr, addr, 6);
	}

	time_send_msg((U16) MSG_ID_BT_TIMES_AUTHORIZE_IND, (local_para_struct *) ind, NULL);
}

void times_send_connect_ind(U8 index, U8 *addr) {
	bt_times_connect_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_connect_ind_struct), TD_CTRL);
	ind->index = index;
	memset(ind->addr, 0, 6);
	if (addr != NULL) {
		memcpy(ind->addr, addr, 6);
	}

	time_send_msg((U16) MSG_ID_BT_TIMES_CONNECT_IND, (local_para_struct *) ind, NULL);
}

void times_send_disconnect_ind(U8 index, U8 rspcode) {
	bt_times_disconnect_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_connect_ind_struct), TD_CTRL);
	ind->index = index;
	ind->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_DISCONNECT_IND, (local_para_struct *) ind, NULL);
}

void times_send_get_cttime_ind(U8 index) {
	bt_times_get_cttime_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_get_cttime_ind_struct), TD_CTRL);
	ind->index = index;

	time_send_msg((U16) MSG_ID_BT_TIMES_GET_CTTIME_IND, (local_para_struct *) ind, NULL);
}

void times_send_request_update_ind(U8 index) {
	bt_times_request_server_update_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_request_server_update_ind_struct), TD_CTRL);
	ind->index = index;

	time_send_msg((U16) MSG_ID_BT_TIMES_REQUEST_SERVER_UPDATE_IND, (local_para_struct *) ind, NULL);
}

void times_send_cancel_update_ind(U8 index) {
	bt_times_cancel_server_update_ind_struct *ind;

	ind = construct_local_para(sizeof(bt_times_cancel_server_update_ind_struct), TD_CTRL);
	ind->index = index;

	time_send_msg((U16) MSG_ID_BT_TIMES_CANCEL_SERVER_UPDATE_IND, (local_para_struct *) ind, NULL);
}

void times_send_update_cttime_cnf(U8 index, U8 rspcode) {
	bt_times_update_cttime_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_update_cttime_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_UPDATE_CTTIME_CNF, (local_para_struct *) cnf, NULL);

}

void times_send_set_local_time_info_cnf(U8 index, U8 rspcode) {
	bt_times_set_local_time_info_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_set_local_time_info_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_SET_LOCAL_TIME_INFO_CNF, (local_para_struct *) cnf, NULL);
}

void times_send_set_ref_time_info_cnf(U8 index, U8 rspcode) {
	bt_times_set_ref_time_info_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_set_ref_time_info_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_SET_REF_TIME_INFO_CNF, (local_para_struct *) cnf, NULL);
}

void times_send_set_dst_cnf(U8 index, U8 rspcode) {
	bt_times_set_dst_cnf_struct *cnf;

	cnf = construct_local_para(sizeof(bt_times_set_dst_cnf_struct), TD_CTRL);
	cnf->index = index;
	cnf->rspcode = rspcode;

	time_send_msg((U16) MSG_ID_BT_TIMES_SET_DST_CNF, (local_para_struct *) cnf, NULL);
}
#endif

/********************************************************************************************
 * Callback functions from lower level
 ********************************************************************************************/
 #ifdef __BT_TIMEC_PROFILE__
void timec_adp_callback(TimeCEvent *event, void *apdata) {
	TimeCApp *app = (TimeCApp *) apdata;
	U8 *data;
	data = event->data;

	OS_Report("[Time] %s(), event: %d", __FUNCTION__, event->event_id);
	switch (event->event_id) {
		case TIMEC_EVENT_CONNECTED:
			timec_send_connect_cnf(app->id, *data);
			app->state = TIME_CLIENT_CONNECTED;
			break;

		case TIMEC_EVENT_CONNECT_FAIL:
			TIMEC_Deinit(app);
			timec_send_connect_cnf(app->id, *data);
			app->id = 0;
			app->state = TIME_CLIENT_IDLE; 
			break;

		case TIMEC_EVENT_DISCONNECTED:
			TIMEC_Deinit(app);
			timec_send_disconnect_ind(app->id, *data);
			app->id = 0;
			app->state = TIME_CLIENT_IDLE; 
			break;

		case TIMEC_EVENT_GET_CTTIME:
			timec_send_get_cttime_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_GET_CTTIME_NOTIFY:
			timec_send_get_cttime_notify_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_SET_CTTIME_NOTIFY:
			timec_send_set_cttime_notify_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_GET_LOCALTIME_INFO:
			timec_send_get_local_time_info_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_GET_REFTIME_INFO:
			timec_send_get_ref_time_info_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_GET_DST:
			timec_send_get_dst_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_REQUEST_UPDATE:
			timec_sned_request_server_update_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_CANCEL_UPDATE:
			timec_sned_cancel_server_update_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_GET_UPDATE_STATE:
			timec_send_get_server_update_status_cnf(app->id, *data);
			break;

		case TIMEC_EVENT_CTTIME_UPDATED:
			timec_send_update_cttime_ind(app->id);
			break;

		default:
			OS_Report("[Time] Unkonwn client event: %d", event->event_id);
	}
}
#endif

#ifdef __BT_TIMES_PROFILE__
void times_adp_callback(TimeSEvent *event, void *apdata) {
	TimeSApp *app = (TimeSApp *) apdata;
	U8 *data;
	data = event->data;

	OS_Report("[Time] %s(), event: %d", __FUNCTION__, event->event_id);
	switch (event->event_id) {
		case TIMES_EVENT_INCOMING:
			// Incoming connection request.
			times_send_authorize_ind(app->id, data);
			break;

		case TIMES_EVENT_CONNECTED:
			times_send_connect_ind(app->id, data);
			break;

		case TIMES_EVENT_DISCONNECTED:
			times_send_disconnect_ind(app->id, BT_STATUS_SUCCESS);
			break;

		case TIMES_EVENT_REGISTER:
		case TIMES_EVENT_DEREGISTER:
			break;

		case TIMES_EVENT_GET_CTTIME:
			times_send_get_cttime_ind(app->id);
			break;

		case TIMES_EVENT_REQUEST_UPDATE:
			times_send_request_update_ind(app->id);
			break;

		case TIMES_EVENT_CANCEL_UPDATE:
			times_send_cancel_update_ind(app->id);
			break;

		default:
			OS_Report("[Time] Unknown server event: %d", event->event_id);
	}
}
#endif

/********************************************************************************************
 * Handler functions for client ILMs from MMI
 ********************************************************************************************/
 #ifdef __BT_TIMEC_PROFILE__
/* MSG_ID_BT_TIMEC_CONNECT_REQ handler function */
void timec_handle_connect_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_connect_req_struct *req;
	req = (bt_timec_connect_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_connect_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	if (g_timecapp[req->index].state != TIME_CLIENT_IDLE) {
		OS_Report("[Time] Already connected, index: %d", req->index);
		// Terminate previous connection
		TIMEC_Disconnect(&g_timecapp[req->index]);
		return;
	}

	g_timecapp[req->index].id = req->index;
	g_timecapp[req->index].state = TIME_CLIENT_LINK_CONNECT;

	status = TIMEC_Init(&g_timecapp[req->index]);
	if (BT_STATUS_SUCCESS != status) {
		OS_Report("[Time] TIMEC_Init() failed.");
		g_timecapp[req->index].state = TIME_CLIENT_IDLE;
		timec_send_connect_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Connect(&g_timecapp[req->index], req->addr, timec_adp_callback);
	if (BT_STATUS_SUCCESS == status) {
		timec_send_connect_cnf(req->index, BT_STATUS_SUCCESS);
	} else if (BT_STATUS_PENDING == status) {
		// Wait for the response
	} else {
		OS_Report("[Time] TIMEC_Connect() failed.");
		g_timecapp[req->index].state = TIME_CLIENT_IDLE;
		TIMEC_Deinit(&g_timecapp[req->index]);
		timec_send_connect_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_DISCONNECT_REQ handler function */
void timec_handle_disconnect_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_disconnect_req_struct *req;
	req = (bt_timec_disconnect_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_disconnect_ind(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	status = TIMEC_Disconnect(&g_timecapp[req->index]);
	switch (status) {
		case BT_STATUS_PENDING:
			// Wait for response
			break;

		case BT_STATUS_SUCCESS:
			TIMEC_Deinit(&g_timecapp[req->index]);
		default:
			timec_send_disconnect_ind(req->index, status);
			g_timecapp[req->index].state = TIME_CLIENT_IDLE;
	}

}

/* MSG_ID_BT_TIMEC_GET_CTTIME_REQ handler function */
void timec_handle_get_cttime_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_cttime_req_struct *req;
	req = (bt_timec_get_cttime_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_cttime_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_CTTIME);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_cttime_cnf(req->index, status);
	}
}

/* MSG_ID_BT_TIMEC_GET_CTTIME_NOTIFY_REQ handler function */
void timec_handle_get_cttime_notify_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_cttime_notify_req_struct *req;
	req = (bt_timec_get_cttime_notify_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_cttime_notify_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_CTTIME_NOTIFY);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_cttime_notify_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_SET_CTTIME_NOTIFY_REQ handler function */
void timec_handle_set_cttime_notify_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_set_cttime_notify_req_struct *req;
	req = (bt_timec_set_cttime_notify_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_set_cttime_notify_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Write_Remote_Value(
			&g_timecapp[req->index], GATT_TIME_CTTIME_NOTIFY, req->notify_config);
	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() write remote value returns: %d.", __FUNCTION__, status);
		timec_send_set_cttime_notify_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_GET_LOCAL_TIME_INFO_REQ handler function */
void timec_handle_get_local_time_info_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_local_time_info_req_struct *req;
	req = (bt_timec_get_local_time_info_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_local_time_info_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_LOCAL_TIME_INFO);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_local_time_info_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_GET_REF_TIME_INFO_REQ handler function */
void timec_handle_get_ref_time_info_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_ref_time_info_req_struct *req;
	req = (bt_timec_get_ref_time_info_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_ref_time_info_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_REF_TIME_INFO);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_ref_time_info_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_GET_DST_REQ handler function */
void timec_handle_get_dst_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_dst_req_struct *req;
	req = (bt_timec_get_dst_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_dst_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_DST_INFO);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_dst_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_REQUEST_SERVER_UPDATE_REQ handler function */
void timec_handle_request_server_update_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	U16 value = (U16) TIME_UPDATE_CTRL_REQUEST;
	bt_timec_request_server_update_req_struct *req;
	req = (bt_timec_request_server_update_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_sned_request_server_update_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Write_Remote_Value(
			&g_timecapp[req->index], GATT_TIME_SERVER_UPDATE_CTRL, value);
	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() write remote value returns: %d.", __FUNCTION__, status);
		timec_sned_request_server_update_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_CANCEL_SERVER_UPDATE_REQ handler function */
void timec_handle_cancel_server_update_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	U16 value = (U16) TIME_UPDATE_CTRL_CANCEL;
	bt_timec_cancel_server_update_req_struct *req;
	req = (bt_timec_cancel_server_update_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_sned_cancel_server_update_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Write_Remote_Value(
			&g_timecapp[req->index], GATT_TIME_SERVER_UPDATE_CTRL, value);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() write remote value returns: %d.", __FUNCTION__, status);
		timec_sned_cancel_server_update_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMEC_GET_SERVER_UPDATE_STATUS_REQ handler function */
void timec_handle_get_server_update_status_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_timec_get_server_update_status_req_struct *req;
	req = (bt_timec_get_server_update_status_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_SERVER_NUM) {
		timec_send_get_server_update_status_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	status = TIMEC_Read_Remote_Value(&g_timecapp[req->index], GATT_TIME_SERVER_UPDATE_STATE);

	if (status != BT_STATUS_PENDING) {
		OS_Report("[Time] %s() read remote value returns: %d.", __FUNCTION__, status);
		timec_send_get_server_update_status_cnf(req->index, BT_STATUS_FAILED);
	}
}
#endif

/********************************************************************************************
 * Handler functions for server ILMs from MMI
 ********************************************************************************************/
 #ifdef __BT_TIMES_PROFILE__
/* MSG_ID_BT_TIMES_REGISTER_REQ handler function */
void times_handle_register_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_times_register_req_struct *req;
	req = (bt_times_register_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_CLIENT_NUM) {
		times_send_register_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	if (g_timesapp[req->index].isReg == 1) {
		// Duplicated register request.
		OS_Report("[Time] Duplicated register request, index: %d", req->index);
		TIMES_Disconnect(&g_timesapp[req->index]);
		return;
	}

	g_timesapp[req->index].id = req->index;
	g_timesapp[req->index].isReg = 1;

	if (BT_STATUS_SUCCESS == TIMES_Register(&g_timesapp[req->index], NULL, times_adp_callback)) {
		times_send_register_cnf(req->index, BT_STATUS_SUCCESS);
	} else {
		g_timesapp[req->index].isReg = 0;
		times_send_register_cnf(req->index, BT_STATUS_FAILED);
	}
}

/* MSG_ID_BT_TIMES_DEREGISTER_REQ handler function */
void times_handle_deregister_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_times_deregister_req_struct *req;
	req = (bt_times_deregister_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_CLIENT_NUM) {
		times_send_deregister_cnf(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	if (g_timesapp[req->index].isReg != 1) {
		times_send_deregister_cnf(req->index, BT_STATUS_NOSERVICES);
		return;
	}

	status = TIMES_Deregister(&g_timesapp[req->index]);
	if (BT_STATUS_SUCCESS == status) {
		times_send_deregister_cnf(req->index, BT_STATUS_SUCCESS);
	} else {
		OS_Report("[Time] Failed to deregister server, status: %d", status);
		times_send_deregister_cnf(req->index, BT_STATUS_FAILED);
	}

	g_timesapp[req->index].isReg = 0;
}

/* MSG_ID_BT_TIMES_AUTHORIZE_RSP handler function */
void times_handle_authorize_rsp(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_times_authorize_rsp_struct *rsp;
	rsp = (bt_times_authorize_rsp_struct *) ilm_ptr->ilm_data;

	if (rsp->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for authorization response: %d", rsp->index);
		return;
	}

	status = TIMES_AcceptIncoming(&g_timesapp[rsp->index], rsp->rspcode);
	if (status == BT_STATUS_SUCCESS) {
		// A wired case... 
		// times_send_connect_ind(rsp->index, status);

	} else if (status == BT_STATUS_PENDING) {
		// Waiting for the connect event. 

	} else {
		OS_Report("[Time] Failed to accept the connection request, status: %d", status);
		TIMES_Disconnect(&g_timesapp[rsp->index]);
	}
}

/* MSG_ID_BT_TIMES_DISCONNECT_REQ handler function */
void times_handle_disconnect_req(ilm_struct *ilm_ptr) {
	BtStatus status;
	bt_times_disconnect_req_struct *req;
	req = (bt_times_disconnect_req_struct *) ilm_ptr->ilm_data;

	if (req->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for disconnect request: %d", req->index);
		times_send_disconnect_ind(req->index, BT_STATUS_NOT_FOUND);
		return;
	}

	if (g_timesapp[req->index].isReg != 1) {
		OS_Report("[Time] No service for disconnect request, index: %d", req->index);
		times_send_disconnect_ind(req->index, BT_STATUS_NOSERVICES);
		return;
	}

	status = TIMES_Disconnect(&g_timesapp[req->index]);
	switch (status) {
		case BT_STATUS_PENDING:
			// Wait for response
			return;

		default:
			times_send_disconnect_ind(req->index, BT_STATUS_SUCCESS);
	}
}

/* MSG_ID_BT_TIMES_UPDATE_CTTIME_REQ handler function */
void times_handle_update_cttime_req(ilm_struct *ilm_ptr) {
	BtStatus status = BT_STATUS_FAILED;
	bt_times_update_cttime_req_struct *req;
	TimeSContext *pCntx;

	req = (bt_times_update_cttime_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for updating cttime request: %d", req->index);
		times_send_update_cttime_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	pCntx = g_timesapp[req->index].timesContext;
	if (pCntx != NULL) {
		pCntx->adj_reason = req->adjust_reason;
		status = TIMES_Notify_Value(&g_timesapp[req->index], GATT_TIME_CTTIME);
	}

	if (status != BT_STATUS_PENDING) {
		OS_Report("%s() notify value returns: %d.", __FUNCTION__, status);
		times_send_update_cttime_cnf(req->index, status);
	}
}

/* MSG_ID_BT_TIMES_SET_LOCAL_TIME_INFO_REQ handler function */
void times_handle_set_local_time_info_req(ilm_struct *ilm_ptr) {
	BtStatus status= BT_STATUS_FAILED;
	bt_times_set_local_time_info_req_struct *req;
	TimeSContext *pCntx;

	req = (bt_times_set_local_time_info_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for setting local time info request: %d", req->index);
		times_send_set_local_time_info_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	pCntx = g_timesapp[req->index].timesContext;

	if (pCntx != NULL) {
		pCntx->time_zone = req->time_zone;
		pCntx->dst = req->dst;
		status = TIMES_Set_Local_Info(&g_timesapp[req->index], GATT_TIME_LOCAL_TIME_INFO);
	}

	times_send_set_local_time_info_cnf(req->index, status);
}

/* MSG_ID_BT_TIMES_SET_REF_TIME_INFO_REQ handler function */
void times_handle_set_ref_time_info_req(ilm_struct *ilm_ptr) {
	BtStatus status = BT_STATUS_FAILED;
	bt_times_set_ref_time_info_req_struct *req;
	TimeSContext *pCntx;

	req = (bt_times_set_ref_time_info_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for setting ref time info request: %d", req->index);
		times_send_set_ref_time_info_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	pCntx = g_timesapp[req->index].timesContext;

	if (pCntx != NULL) {
		pCntx->time_source = req->time_source;
		pCntx->accuracy = req->accuracy;
		status = TIMES_Set_Local_Info(&g_timesapp[req->index], GATT_TIME_REF_TIME_INFO);
	}

	times_send_set_ref_time_info_cnf(req->index, status);
}

/* MSG_ID_BT_TIMES_SET_DST_REQ handler function */
void times_handle_set_dst_req(ilm_struct *ilm_ptr) {
	BtStatus status= BT_STATUS_FAILED;
	bt_times_set_dst_req_struct *req;
	TimeSContext *pCntx;

	req = (bt_times_set_dst_req_struct *) ilm_ptr->ilm_data;
	if (req->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for setting dst request: %d", req->index);
		times_send_set_dst_cnf(req->index, BT_STATUS_FAILED);
		return;
	}

	pCntx = g_timesapp[req->index].timesContext;

	if (pCntx != NULL) {
		pCntx->date_time[0] = req->year;
		pCntx->date_time[1] = (U16) req->month;
		pCntx->date_time[2] = (U16) req->day;
		pCntx->date_time[3] = (U16) req->hours;
		pCntx->date_time[4] = (U16) req->minutes;
		pCntx->date_time[5] = (U16) req->seconds;
		pCntx->dst = req->dst;
		status = TIMES_Set_Local_Info(&g_timesapp[req->index], GATT_TIME_DST_INFO);
	}

	times_send_set_dst_cnf(req->index, status);
}

/* MSG_ID_BT_TIMES_GET_SERVER_UPDATE_STATUS_RSP handler function */
void times_handle_get_server_update_status_rsp(ilm_struct *ilm_ptr) {
	BtStatus status = BT_STATUS_FAILED;
	bt_times_get_server_update_status_rsp_struct *rsp;
	TimeSContext *pCntx;

	rsp = (bt_times_get_server_update_status_rsp_struct *) ilm_ptr->ilm_data;
	if (rsp->index >= TIME_MAX_CLIENT_NUM) {
		OS_Report("[Time] Invalid index for responding update status: %d", rsp->index);
		return;
	}

	pCntx = g_timesapp[rsp->index].timesContext;

	if (pCntx != NULL) {
		pCntx->update_result = rsp->result;
		status = TIMES_Set_Local_Info(&g_timesapp[rsp->index], GATT_TIME_SERVER_UPDATE_STATE);
	}
}
#endif

/********************************************************************************************
 * Entry function for message handlers
 ********************************************************************************************/
void btmtk_adp_time_handle_message(ilm_struct *ilm_ptr) {
	OS_Report("[Time] %s, msg_id: %d", __FUNCTION__, ilm_ptr->msg_id);

	switch (ilm_ptr->msg_id) {

#ifdef __BT_TIMEC_PROFILE__
		case MSG_ID_BT_TIMEC_CONNECT_REQ:
			timec_handle_connect_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_DISCONNECT_REQ:
			timec_handle_disconnect_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_CTTIME_REQ:
			timec_handle_get_cttime_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_CTTIME_NOTIFY_REQ:
			timec_handle_get_cttime_notify_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_SET_CTTIME_NOTIFY_REQ:
			timec_handle_set_cttime_notify_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_LOCAL_TIME_INFO_REQ:
			timec_handle_get_local_time_info_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_REF_TIME_INFO_REQ:
			timec_handle_get_ref_time_info_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_DST_REQ:
			timec_handle_get_dst_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_REQUEST_SERVER_UPDATE_REQ:
			timec_handle_request_server_update_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_CANCEL_SERVER_UPDATE_REQ:
			timec_handle_cancel_server_update_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMEC_GET_SERVER_UPDATE_STATUS_REQ:
			timec_handle_get_server_update_status_req(ilm_ptr);
			break;
#endif

#ifdef __BT_TIMES_PROFILE__
		case MSG_ID_BT_TIMES_REGISTER_REQ:
			times_handle_register_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_DEREGISTER_REQ:
			times_handle_deregister_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_AUTHORIZE_RSP:
			times_handle_authorize_rsp(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_DISCONNECT_REQ:
			times_handle_disconnect_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_REQUEST_SERVER_UPDATE_RSP:
		case MSG_ID_BT_TIMES_CANCEL_SERVER_UPDATE_RSP:
			break;
		case MSG_ID_BT_TIMES_UPDATE_CTTIME_REQ:
			// Used to notify client that the time has changed and needed to update.
			times_handle_update_cttime_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_SET_LOCAL_TIME_INFO_REQ:
			times_handle_set_local_time_info_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_SET_REF_TIME_INFO_REQ:
			times_handle_set_ref_time_info_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_SET_DST_REQ:
			times_handle_set_dst_req(ilm_ptr);
			break;
		case MSG_ID_BT_TIMES_GET_SERVER_UPDATE_STATUS_RSP:
			// Used to update the time-update state
			times_handle_get_server_update_status_rsp(ilm_ptr);
			break;
#endif

		default:
			OS_Report("[Time] Invalid msg_id: %d", ilm_ptr->msg_id);
	}
}

void time_adp_init() {
	U8 i = 0;
	OS_Report("[Time] time_adp_init");

	TIME_ATT_Init();

#ifdef __BT_TIMES_PROFILE__
	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		g_timesapp[i].id = 0;
	}
	TIMES_ResetContext();
#endif

#ifdef __BT_TIMEC_PROFILE__
	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		g_timecapp[i].id = 0;
		g_timecapp[i].state = 0;
	}
	TIMEC_ResetContext();
#endif

}

#endif /* #if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__) */
