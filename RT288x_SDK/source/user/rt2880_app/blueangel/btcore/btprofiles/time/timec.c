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
 * timec.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time profile
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
#ifdef __BT_TIMEC_PROFILE__
#include "bttypes.h"
#include "bt_os_api.h"
#include "bluetooth_time_struct.h"
#include "timec.h"
#include "timeadapter.h"
#include "utils.h"

TimeCContext g_timec[TIME_MAX_SERVER_NUM];

/*********************************************************************************************
 * Internal data and functions
 *********************************************************************************************/
static TimeCContext* _get_available_client_context() {
	U8 i = 0;
	TimeCContext* pCntx;

	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		pCntx = &g_timec[i];
		if (pCntx->in_use == FALSE) {
			pCntx->in_use = TRUE;
			return pCntx;
		}
	}

	OS_Report("[Time] No available client context.");
	return NULL;
}

static void _release_client_context(TimeCContext *pCntx) {
	U8 i = 0;

	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		if (pCntx == &g_timec[i]) {
			pCntx->in_use = FALSE;
			// Clear cached handles
			btmtk_os_memset((U8 *) pCntx->chnl.cached_handles, 0, sizeof(pCntx->chnl.cached_handles));
			return;
		}
	}

	OS_Report("[Time] Invalid client context pointer: 0x%x", pCntx);
	TIMEASSERT(FALSE);
}

// This function doesn't have any effect for now.
static BtStatus _query_channel(TimeCContext *pCntx) {
	BtStatus status;

	pCntx->state = TIME_CLIENT_QUERY;
	status = TIME_ATT_Query_Service(&pCntx->chnl);
	return status;
}

static BtStatus _open_channel(TimeCContext *pCntx) {
	BtStatus status;

	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}
	pCntx->state = TIME_CLIENT_CONNECTING;
	status = TIME_ATT_Connect_Service(&pCntx->chnl);
	return status;
}

/*********************************************************************************************
 * Functions called from internal adaptation layer
 *********************************************************************************************/
BT_BOOL TIMEC_ResetContext() {
	U8 index = 0;
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	// Reset the context
	for (index = 0; index < TIME_MAX_SERVER_NUM; index++) {
		pCntx = &g_timec[index];
		memset((void *) pCntx, 0, sizeof(pCntx));
	}
	return TRUE;
}

BtStatus TIMEC_Init(TimeCApp *app) {
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (app == NULL) {
		return BT_STATUS_FAILED;
	}

	pCntx = _get_available_client_context();
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	app->timecContext = pCntx;
	pCntx->chnl.check_byte = TIME_CHECK_BYTE;
	return BT_STATUS_SUCCESS;
}

BtStatus TIMEC_Deinit(TimeCApp *app) {
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (app == NULL) {
		return BT_STATUS_FAILED;
	}

	pCntx = app->timecContext;
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	TIMEASSERT(pCntx->chnl.check_byte == TIME_CHECK_BYTE);
	_release_client_context(pCntx);
	app->timecContext = NULL;
	return BT_STATUS_SUCCESS;
}

BtStatus TIMEC_Connect(TimeCApp *app, U8 addr[6], BTMTK_TIMEC_CALLBACK callback) {
	BtStatus status;
	TimeCContext *pCntx = app->timecContext;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	TIMEASSERT(pCntx->chnl.check_byte == TIME_CHECK_BYTE);

	pCntx->state = TIME_CLIENT_LINK_CONNECT;
	pCntx->callback = callback;
	pCntx->app = app;

	// Creat ACL link
	status = TIME_ATT_Create_Link(&pCntx->chnl, addr);
	if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
		OS_Report("[Time] Client failed to crate link.");
		return BT_STATUS_FAILED;
	}

	// Query channel
	if (status == BT_STATUS_SUCCESS) {
		status = _query_channel(app->timecContext);
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			OS_Report("[Time] Client failed to query channel.");
			return BT_STATUS_FAILED;
		}
	}

	// Connect
	if (status == BT_STATUS_SUCCESS) {
		status = _open_channel(app->timecContext);
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			OS_Report("[Time] Client failed to open channel.");
			TIME_ATT_Destroy_Link(&pCntx->chnl);
			return BT_STATUS_FAILED;
		}
	}

	return status;
}

BtStatus TIMEC_Disconnect(TimeCApp *app) {
	BtStatus status = BT_STATUS_SUCCESS;
	TimeCContext *pCntx = app->timecContext;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	if (pCntx->state != TIME_CLIENT_IDLE) {
		pCntx->state = TIME_CLIENT_DISCONNECTING;
		status = TIME_ATT_Disconnect_Service(&pCntx->chnl);
	}

	return status;
}

BtStatus TIMEC_Read_Remote_Value(TimeCApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCContext *pCntx = app->timecContext;
	TimePacket *pkt;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL || pCntx->state != TIME_CLIENT_CONNECTED) {
		return status;
	}

	pkt = TIMEC_ATT_Get_Packet(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		status = TIME_ATT_Read_Remote_Value(pkt, 1, value_id);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			TIMEC_ATT_Return_Packet(pkt);
		}
	}

	return status;
}

BtStatus TIMEC_Write_Remote_Value(TimeCApp *app, U8 value_id, U16 value) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCContext *pCntx = app->timecContext;
	TimePacket *pkt;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL || pCntx->state != TIME_CLIENT_CONNECTED) {
		return status;
	}

	pkt = TIMEC_ATT_Get_Packet(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		status = TIME_ATT_Write_Remote_Value(pkt, 1, value_id, value);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			TIMEC_ATT_Return_Packet(pkt);
		}
	}

	return status;
}


/*********************************************************************************************
 * Notify functions. To internal and external adaptation layers.
 *********************************************************************************************/
/* Notify internal adaption layer for sending ILM to MMI */
void timec_notify(TimeCContext *pCntx, U8 event_id, void *data) {
	TimeCEvent event;
	if (pCntx->callback) {
		event.event_id = event_id;
		event.data = data;
		pCntx->callback(&event, pCntx->app);

	} else {
		OS_Report("[Time] Null client callback. Force disconnect.");

		if (pCntx->state != TIME_CLIENT_DISCONNECTING && pCntx->state != TIME_CLIENT_IDLE) {
			TIMEC_Disconnect(pCntx->app);
		}
	}
}

void timec_notify_link_result(TimeCContext *pCntx, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	OS_Report("[Time] %s(), rspcode: %d", __FUNCTION__, rspcode);
	if (pCntx->state != TIME_CLIENT_LINK_CONNECT) {
		OS_Report("[Time] %s(): Invalid state %d", __FUNCTION__, pCntx->state);
		return;
	}

	if (rspcode == BT_STATUS_SUCCESS) {
		status = _query_channel(pCntx);
		timec_notify_query_result(pCntx, status);

	} else if (rspcode == BT_STATUS_PENDING) {
		// Wait for the response

	} else {
		status = TIME_ATT_Destroy_Link(&pCntx->chnl);
		if (status == BT_STATUS_SUCCESS) {
			// Failed to create data link.
			timec_notify_connect_result(pCntx, BT_STATUS_NO_CONNECTION);
		}
	}
}

void timec_notify_query_result(TimeCContext *pCntx, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	if (pCntx->state != TIME_CLIENT_QUERY) {
		OS_Report("[Time] %s(): Invalid state %d", __FUNCTION__, pCntx->state);
		return;
	}

	if (rspcode == BT_STATUS_SUCCESS) {
		status = _open_channel(pCntx);
		timec_notify_connect_result(pCntx, status);

	} else if (rspcode == BT_STATUS_PENDING) {
		// Wait for the response

	} else {
		status = TIME_ATT_Destroy_Link(&pCntx->chnl);
		if (status == BT_STATUS_SUCCESS) {
			// Service not found.
			timec_notify_connect_result(pCntx, BT_STATUS_NOT_FOUND);
		}
	}
}

void timec_notify_connect_result(TimeCContext *pCntx, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	OS_Report("[Time] %s(), rspcode: %d", __FUNCTION__, rspcode);
	switch (pCntx->state) {
		case TIME_CLIENT_LINK_CONNECT:
		case TIME_CLIENT_QUERY:
		case TIME_CLIENT_CONNECTING:
			// Valid states
			break;

		default:
			OS_Report("[Time] %s(): Invalid state %d", __FUNCTION__, pCntx->state);
			return;
	}

	if (rspcode == BT_STATUS_SUCCESS) {
		pCntx->state = TIME_CLIENT_CONNECTED;
		timec_notify(pCntx, TIMEC_EVENT_CONNECTED, &rspcode);

	} else if (rspcode == BT_STATUS_PENDING) {
		// Wait for response

	} else {
		pCntx->state = TIME_CLIENT_IDLE;
		timec_notify(pCntx, TIMEC_EVENT_CONNECT_FAIL, &rspcode);
	}
}

void timec_notify_disconnect_result(TimeCContext *pCntx, U32 rspcode) {
	pCntx->state = TIME_CLIENT_IDLE;
	timec_notify(pCntx, TIMEC_EVENT_DISCONNECTED, &rspcode);
}

void timec_notify_read_remote_value_result(TimeCContext *pCntx, U32 rspcode, U8 value_id) {
	U8 event_type = 0;
	switch (value_id) {
		case GATT_TIME_CTTIME:
			event_type = TIMEC_EVENT_GET_CTTIME;
			break;
		case GATT_TIME_CTTIME_NOTIFY:
			event_type = TIMEC_EVENT_GET_CTTIME_NOTIFY;
			break;
	}
	timec_notify(pCntx, event_type, &rspcode);
}

void timec_notify_write_remote_value_result(TimeCContext *pCntx, U32 rspcode, U8 value_id) {
	U8 event_type = 0;
	switch (value_id) {
		case GATT_TIME_SERVER_UPDATE_CTRL:
			event_type = TIMEC_EVENT_REQUEST_UPDATE;
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			event_type = TIMEC_EVENT_SET_CTTIME_NOTIFY;
			break;
	}
	timec_notify(pCntx, event_type, &rspcode);
}

void timec_notify_server_notification(TimeCContext *pCntx, U8 value_id) {
	U8 event_type = 0;
	switch (value_id) {
		case GATT_TIME_CTTIME:
			event_type = TIMEC_EVENT_CTTIME_UPDATED;
	}
	timec_notify(pCntx, event_type, BT_STATUS_SUCCESS);
}

#endif
