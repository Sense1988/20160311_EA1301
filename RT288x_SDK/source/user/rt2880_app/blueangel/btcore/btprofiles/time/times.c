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
 * times.c
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
#ifdef __BT_TIMES_PROFILE__
#include "bttypes.h"
#include "bluetooth_time_struct.h"
#include "times.h"
#include "timeadapter.h"

static TimeSContext _time_server_contexts[TIME_MAX_CLIENT_NUM];
static U8 _time_server_cntx_used_count = 0;

/*********************************************************************************************
 * Internal data and functions
 *********************************************************************************************/
static TimeSContext* _get_available_server_context() {
	U8 i = 0;
	TimeSContext* pCntx;

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		pCntx = &_time_server_contexts[i];
		if (pCntx->in_use == FALSE) {
			pCntx->in_use = TRUE;
			_time_server_cntx_used_count++;
			return pCntx;
		}
	}

	OS_Report("[Time] No available server context.");
	return NULL;
}

static void _release_server_context(TimeSContext *pCntx) {
	U8 i = 0;

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		if (pCntx == &_time_server_contexts[i]) {
			pCntx->in_use = FALSE;
			_time_server_cntx_used_count--;
			return;
		}
	}

	OS_Report("[Time] Invalid server context pointer: 0x%x", pCntx);
	TIMEASSERT(FALSE);
}

/*********************************************************************************************
 * Functions called from internal adaptation layer
 *********************************************************************************************/
/* @brief invoke by system before using time server */
BT_BOOL TIMES_ResetContext() {
	U8 index = 0;
	TimeSContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	// Reset the context
	for (index = 0; index < TIME_MAX_CLIENT_NUM; index++) {
		pCntx = &_time_server_contexts[index];
		memset((void *) pCntx, 0, sizeof(pCntx));
	}
	return TRUE;
}

BtStatus TIMES_Register(TimeSApp *app, TimeSTransport *chnl, BTMTK_TIMES_CALLBACK callback) {
	BtStatus status;
	TimeSContext* pCntx;

	if (callback == NULL) {
		OS_Report("[Time] %s callback is NULL", __FUNCTION__);
		return BT_STATUS_FAILED;
	}

	pCntx = _get_available_server_context();
	if (pCntx != NULL) {
		// TODO: Clean ATT database
		pCntx->state = TIME_SERVER_READY; 
		pCntx->chnl.check_byte = TIME_CHECK_BYTE;

		if (BT_STATUS_SUCCESS == TIME_ATT_Register(&(pCntx->chnl), NULL, &app->id)) {
			if (_time_server_cntx_used_count == 1) {
				// For now, under BR/EDR mode, we only need to regsiter SDP record once.
				TIME_ATT_Register_Record(&pCntx->chnl);
			}

			pCntx->callback = callback;
			pCntx->app = (void*) app;
			app->timesContext = (void*) pCntx;

			return BT_STATUS_SUCCESS;
		}
	}

	if (pCntx != NULL) {
		_release_server_context(pCntx);
	}

	OS_Report("[Time] Server registration failed.");
	return BT_STATUS_FAILED;
}

BtStatus TIMES_Deregister(TimeSApp *app) {
	BtStatus status;
	TimeSContext* pCntx;
	pCntx = (TimeSContext *) app->timesContext;

	if (pCntx != NULL) {
		if (pCntx->state == TIME_SERVER_READY) {
			if (_time_server_cntx_used_count == 1) {
				// For now, under BR/EDR mode, we unregister the SDP record when returning the
				// last time server context.
				TIME_ATT_Deregister_Record(&pCntx->chnl);
			}

			status = TIME_ATT_Deregister(&pCntx->chnl);
			if (BT_STATUS_SUCCESS == status) {
				_release_server_context(pCntx);
				app->timesContext = NULL;
				return BT_STATUS_SUCCESS;

			} else {
				OS_Report("[Time] TIME_ATT_Deregister() returns %d", status);
			}
		} else {
			OS_Report("[Time] Wrong state to deregister.");
		}
	}

	return BT_STATUS_FAILED;
}

BtStatus TIMES_AcceptIncoming(TimeSApp *app, BtStatus rspcode) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx;
	pCntx = (TimeSContext *) app->timesContext;

	if (pCntx != NULL) {
		if (pCntx->state == TIME_SERVER_CONNECTING) {
			if (rspcode == BT_STATUS_SUCCESS) {
				status = TIME_ATT_Accept(&pCntx->chnl);
			} else {
				status = TIME_ATT_Reject(&pCntx->chnl);
			}
			OS_Report("[Time] TIMES_AcceptIncoming, rspcode: %d, status: %d", rspcode, status);

		} else {
			OS_Report("[Time] Wrong state to accept incoming.");
		}
	}

	return status; 
}

BtStatus TIMES_Disconnect(TimeSApp *app) {
	BtStatus status = BT_STATUS_SUCCESS;
	TimeSContext *pCntx;
	pCntx = (TimeSContext *) app->timesContext;

	if (pCntx != NULL) {
		switch (pCntx->state) {
			case TIME_SERVER_CONNECTING:
				OS_Report("[Time] Server disconnects in connecting phase.");
				TIMES_AcceptIncoming(app, BT_STATUS_FAILED);
				break;

			case TIME_SERVER_CONNECTED:
				status = TIME_ATT_Server_Disconnect(&pCntx->chnl);
				if (status == BT_STATUS_SUCCESS) {
					pCntx->state = TIME_SERVER_READY;
				} else {
					pCntx->state = TIME_SERVER_DISCONNECTING;
				}
				break;

			default:
				break;
		}
	}

	return status;
}

BtStatus TIMES_Set_Local_Info(TimeSApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx = (TimeSContext *) app->timesContext;

	if (pCntx != NULL) {
		status = TIME_ATT_Write_Local_Value(pCntx, value_id);
	}
	return status;
}

BtStatus TIMES_Notify_Value(TimeSApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx = (TimeSContext *) app->timesContext;
	TimePacket *pkt;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL || pCntx->state != TIME_SERVER_CONNECTED) {
		return status;
	}

	pkt = TIMES_ATT_Get_Packet(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		status = TIME_ATT_Notify_Value(pkt, 1, value_id);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			TIMES_ATT_Return_Packet(pkt);
		}
	}

	return status;
}

/*********************************************************************************************
 * Notify functions. To internal and external adaptation layers.
 *********************************************************************************************/
/* Notify internal adaption layer for sending ILM to MMI */
void times_notify(TimeSContext *pCntx, U8 event_id, void *data) {
	TimeSEvent event;
	if (pCntx->callback) {
		event.event_id = event_id;
		event.data = data;
		pCntx->callback(&event, pCntx->app);

	} else {
		OS_Report("[Time] Null server callback. Force disconnect.");

		if (pCntx->state != TIME_SERVER_DISCONNECTING && pCntx->state != TIME_SERVER_IDLE) {
			TIMES_Disconnect(pCntx->app);
		}
	}
}

void times_notify_incoming(TimeSContext *pCntx, U8 *addr) {
	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx->state == TIME_SERVER_READY) {
		pCntx->state = TIME_SERVER_CONNECTING;
		if (addr != NULL) {
			memcpy(pCntx->addr, addr, 6);
		} else {
			memset(pCntx->addr, 0, 6);
		}
		times_notify(pCntx, TIMES_EVENT_INCOMING, pCntx->addr);

	} else {
		// For now, reject directly.
		TIMES_AcceptIncoming(pCntx->app, BT_STATUS_FAILED);
	}
}

void times_notify_connected(TimeSContext *pCntx, U8 *addr) {
	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx->state == TIME_SERVER_CONNECTING) {
		pCntx->state = TIME_SERVER_CONNECTED;
		if (addr != NULL) {
			memcpy(pCntx->addr, addr, 6);
		} else {
			memset(pCntx->addr, 0, 6);
		}
		times_notify(pCntx, TIMES_EVENT_CONNECTED, pCntx->addr);

	} else {
		OS_Report("[Time] Notify connected in invalid state: %d", pCntx->state);
	}
}

void times_notify_disconnected(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	pCntx->state = TIME_SERVER_READY;
	times_notify(pCntx, TIMES_EVENT_DISCONNECTED, NULL);
}

void times_notify_notify_value_result(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	times_notify(pCntx, TIMES_EVENT_UPDATE_CTTIME, NULL);
}

void times_notify_request_server_update(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	times_notify(pCntx, TIMES_EVENT_REQUEST_UPDATE, NULL);
}

void times_notify_cancel_server_update(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	times_notify(pCntx, TIMES_EVENT_CANCEL_UPDATE, NULL);
}

#endif
