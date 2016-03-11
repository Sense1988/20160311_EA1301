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
 * timeadapter.c
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
#include "time.h"

#include "bttypes.h"
#include "bt_os_api.h"
#include "att.h"
#include "attdb.h"
#include "gattcb.h"
#include "sdap.h"
#include "utils.h"  // for listentry

#include "bluetooth_time_struct.h"
#include "timeadapter.h"
#include "timec.h"
#include "times.h"

#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)

/********************************************************************************************
 * Internal variables
 ********************************************************************************************/
#ifdef __BT_TIMES_PROFILE__
static TimeServerSharedInfo _time_server_info;

static ListEntry _times_packet_list;
// static TimePacket _times_packets[TIME_MAX_CLIENT_NUM];
static TimePacket _times_packets[1];

static ListEntry _time_server_tp_list;
static U8 _time_server_tp_list_init = FALSE;
static TimeSTransport *_time_server_used_tps[TIME_MAX_CLIENT_NUM];
#endif

#ifdef __BT_TIMEC_PROFILE__
static ListEntry _timec_packet_list;
static TimePacket _timec_packets[TIME_MAX_SERVER_NUM];

static ListEntry _time_client_tp_list;
static U8 _time_client_tp_list_init = FALSE;
static TimeCTransport *_time_client_tps[TIME_MAX_SERVER_NUM];

static TimeCTransport *_cur_timec_chnl;
#endif

static BtGattHandler _time_gatt_handler;

/********************************************************************************************
 * External functions
 ********************************************************************************************/
// For adding or removing SDP record in BR/EDR
extern U8 TimeSdpDB_AddRecord(U8 idx, U8 cap);
extern U8 TimeSdpDB_RemoveRecord(U8 idx, U8 cap);

// For adding or removing GATT service record
extern U8 TimeAttDB_AddRecord();
extern U8 TimeAttDB_RemoveRecord();

// GATT functions (gatt_util.c)
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryAllCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_Notifications(BtRemoteDevice *link, BtGattOp *op);

/********************************************************************************************
 * Internal functions
 ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
static BtStatus _time_gatt_discovery_primary_service_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end);
static BtStatus _time_gatt_discovery_characteristic_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end);
static BtStatus _time_gatt_read_characteristic_value(
		TimePacket *pkt, U16 char_handle);
static BtStatus _time_gatt_write_without_response(
		TimePacket *pkt, U16 char_handle);
static BtStatus _time_gatt_discovery_characteristic_descriptors(
		TimePacket *pkt, U16 start, U16 end);
static BtStatus _time_gatt_read_characteristic_descriptor(
		TimePacket *pkt, U16 desc_handle);
static BtStatus _time_gatt_write_characteristic_descriptor(
		TimePacket *pkt, U16 desc_handle);

static BtStatus _time_access_service_handle_cache(
		TimeCTransport *pChnl, U8 value_id, U8 write, U16 *start_handle, U16 *end_handle) {
	U8 start_idx = 0, end_idx = 0;

	switch (value_id) {
		case GATT_TIME_CTTIME:
		case GATT_TIME_CTTIME_NOTIFY:
		case GATT_TIME_LOCAL_TIME_INFO:
		case GATT_TIME_REF_TIME_INFO:
			start_idx = CURRENT_TIME_SERVICE_START;
			end_idx = CURRENT_TIME_SERVICE_END;
			break;

		case GATT_TIME_DST_INFO:
			start_idx = NEXT_DST_SERVICE_START;
			end_idx = NEXT_DST_SERVICE_END;
			break;

		case GATT_TIME_SERVER_UPDATE_CTRL:
		case GATT_TIME_SERVER_UPDATE_STATE:
			start_idx = REF_TIME_UPDATE_SERVICE_START;
			end_idx = REF_TIME_UPDATE_SERVICE_END;
			break;
		default:
			return BT_STATUS_FAILED;
	}

	if (write == 1) {
		pChnl->cached_handles[start_idx] = *start_handle;
		pChnl->cached_handles[end_idx] = *end_handle;
	} else {
		*start_handle = pChnl->cached_handles[start_idx];
		*end_handle = pChnl->cached_handles[end_idx];
	}

	return BT_STATUS_SUCCESS;
}

static BtStatus _time_access_value_handle_cache(
		TimeCTransport *pChnl, U8 value_id, U8 write, U16 *value_handle) {
	U8 idx = 0;

	switch (value_id) {
		case GATT_TIME_CTTIME:
			idx = CT_TIME;
			break;
		case GATT_TIME_CTTIME_NOTIFY:
			idx = CT_TIME_CHAR_CONFIG;
			break;
		case GATT_TIME_LOCAL_TIME_INFO:
			idx = LOCAL_TIME_INFO;
			break;
		case GATT_TIME_REF_TIME_INFO:
			idx = REF_TIME_INFO;
			break;
		case GATT_TIME_DST_INFO:
			idx = TIME_WITH_DST;
			break;
		case GATT_TIME_SERVER_UPDATE_CTRL:
			idx = TIME_UPDATE_CTRL_POINT;
			break;
		case GATT_TIME_SERVER_UPDATE_STATE:
			idx = TIME_UPDATE_STATE;
			break;
		default:
			return BT_STATUS_FAILED;
	}

	if (write == 1) {
		pChnl->cached_handles[idx] = *value_handle;
	} else {
		*value_handle = pChnl->cached_handles[idx];
	}

	return BT_STATUS_SUCCESS;
}
#endif

#ifdef __BT_TIMES_PROFILE__
static TimeSTransport *_get_available_server_tp(BtRemoteDevice *link) {
	U8 i;
	TimeSTransport *server_tp;

	if (_time_server_tp_list_init != TRUE) {
		return NULL;
	}

	// If the link (BD_ADDR) has connected, return NULL.
	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		server_tp = _time_server_used_tps[i];
		if (server_tp == NULL) {
			break;
		} else {
			if (btmtk_os_memcmp(server_tp->bdaddr.addr, 6, link->bdAddr.addr, 6)) {
				OS_Report("[Time] %x:%x:%x:%x:%x:%x has already connected.",
						server_tp->bdaddr.addr[0], server_tp->bdaddr.addr[1], server_tp->bdaddr.addr[2],
						server_tp->bdaddr.addr[3], server_tp->bdaddr.addr[4], server_tp->bdaddr.addr[5]);
				return NULL;
			}
		}
	}

	if (i < TIME_MAX_CLIENT_NUM && IsListEmpty(&_time_server_tp_list)) {
		// Found one avaialbe server transport
		server_tp = (TimeSTransport *) RemoveHeadList(&_time_server_tp_list);
		server_tp->cttime_notify_config = 0x0000;
		server_tp->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
		server_tp->update_status.result = TIME_UPDATE_RESULT_NO_CONNECTION;
		_time_server_used_tps[i] = server_tp;

	} else {
		// No available server transport
		server_tp = NULL;
	}

	return server_tp;
}

static BOOL _return_server_tp(TimeSTransport *server_tp) {
	U8 i;

	if (_time_server_tp_list_init != TRUE || server_tp == NULL) {
		return FALSE;
	}

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		if (_time_server_used_tps[i] == server_tp) {
			_time_server_used_tps[i] = NULL;
			InsertTailList(&_time_server_tp_list, &server_tp->node);
			return TRUE;
		}
	}
	OS_Report("[Time] Error: %s() server_tp(0x%x) is not in used_list.", __FUNCTION__, server_tp);
	return FALSE;
}

static TimeSTransport *_find_server_tp_by_remdev(BtRemoteDevice *link) {
	U8 i;
	TimeSTransport *server_tp;

	if (link == NULL) {
		return NULL;
	}

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		server_tp = _time_server_used_tps[i];
		if ((server_tp != NULL) && btmtk_os_memcmp(server_tp->bdaddr.addr, 6, link->bdAddr.addr, 6)) {
			return server_tp;
		}
	}

	OS_Report("[Time] WARN: No server tp found for addr: %x:%x:%x:%x:%x:%x .",
			link->bdAddr.addr[0], link->bdAddr.addr[1], link->bdAddr.addr[2],
			link->bdAddr.addr[3], link->bdAddr.addr[4], link->bdAddr.addr[5]);

	return NULL;
}

#endif


/********************************************************************************************
 * Packet related functions
 ********************************************************************************************/
static void _init_time_packet_list() {
#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)
	int i;
#endif
	OS_Report("[Time] %s", __FUNCTION__);

#ifdef __BT_TIMEC_PROFILE__
	InitializeListHead(&_timec_packet_list);
	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		_timec_packets[i].check_byte = TIME_CHECK_BYTE;
		InsertTailList(&_timec_packet_list, &(_timec_packets[i].node));
	}
#endif

#ifdef __BT_TIMES_PROFILE__
	InitializeListHead(&_times_packet_list);
	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		_times_packets[i].check_byte = TIME_CHECK_BYTE;
		InsertTailList(&_times_packet_list, &(_times_packets[i].node));
	}
#endif
}

#ifdef __BT_TIMEC_PROFILE__
TimePacket* timec_att_get_packet(void *pChnl) {
	TimePacket *pkt = NULL;

	if (!IsListEmpty(&_timec_packet_list)) {
		pkt = (TimePacket *) RemoveHeadList(&_timec_packet_list);
		pkt->chnl = pChnl;
		btmtk_os_memset((U8 *) &pkt->gatt_params, 0, sizeof(pkt->gatt_params));
	}

	if (pkt == NULL) {
		OS_Report("[Time] No available client packet.");
	}

	return pkt;
}

void timec_att_return_packet(TimePacket *pkt) {
	InsertTailList(&_timec_packet_list, &pkt->node);
}
#endif

#ifdef __BT_TIMES_PROFILE__
TimePacket* times_att_get_packet(void *pChnl) {
	TimePacket *pkt = NULL;

	if (!IsListEmpty(&_times_packet_list)) {
		pkt = (TimePacket *) RemoveHeadList(&_times_packet_list);
		pkt->chnl = pChnl;
		btmtk_os_memset((U8 *) &pkt->gatt_params, 0, sizeof(pkt->gatt_params));
	}

	if (pkt == NULL) {
		OS_Report("[Time] No available server packet.");
	}

	return pkt;
}

void times_att_return_packet(TimePacket *pkt) {
	InsertTailList(&_times_packet_list, &pkt->node);
}
#endif


/********************************************************************************************
 * Interface Functions for Time Client
 ********************************************************************************************/
 #ifdef __BT_TIMEC_PROFILE__
void timec_cmgr_callback(CmgrHandler *handler, CmgrEvent event, BtStatus status) {
	TimeCTransport *pChnl;
	TimeCContext *pCntx;

	pChnl = ContainingRecord(handler, TimeCTransport, handler);
	pCntx = ContainingRecord(pChnl, TimeCContext, chnl);

	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	OS_Report("[Time] timec_cmgr_callback, event: %d, status: %d", event, status);

	switch (event) {
		case CMEVENT_DATA_LINK_CON_CNF:
			if (pCntx->state == TIME_CLIENT_LINK_CONNECT) {
				timec_notify_link_result(pCntx, status);

			} else if (pCntx->state == TIME_CLIENT_DISCONNECTING) {
				if (status == BT_STATUS_SUCCESS) {
					time_att_destroy_link(pChnl);
				} else {
					timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
				}
			}
			break;

		case CMEVENT_DATA_LINK_DIS:
			time_att_destroy_link(pChnl);
			if (pCntx->state != TIME_CLIENT_IDLE) {
				timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
			}
			break;

		default:
			break;
	}
}

void timec_sdp_query_callback(SdpQueryToken *sqt, U8 result, U8 serv_chnl) {
	BtStatus status;
	TimeCTransport *pChnl;
	TimeCContext *pCntx;

	if (sqt != NULL) {
		OS_Report("[Time] timec_sdp_query_callback, result: %d, serv_chnl: %d", result, serv_chnl);
	} else {
		OS_Report("[Time] timec_sdp_query_callback, ERROR: sqt is NULL");
		return;
	}

	pChnl = ContainingRecord(sqt, TimeCTransport, sdp_query_token);
	pCntx = ContainingRecord(pChnl, TimeCContext, chnl);

	timec_notify_query_result(pCntx, result);

	/* To be delete
	switch (result) {
		case BT_STATUS_SUCCESS:
			timec_notify_query_result(pCntx, BT_STATUS_SUCCESS);
			break;

		default:
			timec_notify_query_result(pCntx, BT_STATUS_FAILED);
			GattDisconnect(pChnl->handler.remDev);
			status = CMGR_RemoveDataLink(&pChnl->handler);
	}
	*/
}


static void _time_gatt_notify_query_result(TimePacket *pkt, U8 *data, U8 len, U8 result) {
	TimeCContext *pCntx;

	if (pkt == NULL) {
		OS_Report("[Time] Error: %s with NULL packet", __FUNCTION__);
		return;
	}

	if (pkt->chnl == NULL) {
		OS_Report("[Time] Error: %s with NULL channel", __FUNCTION__);
		return;
	}

	pCntx = ContainingRecord(pkt->chnl, TimeCContext, chnl);

	switch (pkt->value_id) {
		case GATT_TIME_CTTIME:
			if (result == BT_STATUS_SUCCESS && len > 9) {
				pCntx->date_time[0] = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
				pCntx->date_time[1] = (U16) data[2];
				pCntx->date_time[2] = (U16) data[3];
				pCntx->date_time[3] = (U16) data[4];
				pCntx->date_time[4] = (U16) data[5];
				pCntx->date_time[5] = (U16) data[6];
				pCntx->day_of_week = data[7];
				pCntx->frac = data[8];
				pCntx->adj_reason = data[9];
				timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
			}
			timec_att_return_packet(pkt);
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			switch (pkt->type) {
				case TIME_PKT_READ:
					if (result == BT_STATUS_SUCCESS && len > 1) {
						pCntx->cttime_notify_config = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
						timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
					} else {
						timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
					}
					timec_att_return_packet(pkt);
					break;

				case TIME_PKT_WRITE:
					timec_notify_write_remote_value_result(pCntx, result, pkt->value_id);
					timec_att_return_packet(pkt);
					break;
			}
			break;

		case GATT_TIME_LOCAL_TIME_INFO:
			if (result == BT_STATUS_SUCCESS && len > 1) {
				pCntx->time_zone = data[0];
				pCntx->dst = data[1];
				timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
			}
			timec_att_return_packet(pkt);
			break;

		case GATT_TIME_REF_TIME_INFO:
			if (result == BT_STATUS_SUCCESS && len > 3) {
				pCntx->time_source = data[0];
				pCntx->accuracy = data[1];
				pCntx->days_since_update = data[2];
				pCntx->hours_since_update = data[3];
				timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
			}
			timec_att_return_packet(pkt);
			break;

		case GATT_TIME_DST_INFO:
			if (result == BT_STATUS_SUCCESS && len > 7) {
				pCntx->date_time[0] = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
				pCntx->date_time[1] = (U16) data[2];
				pCntx->date_time[2] = (U16) data[3];
				pCntx->date_time[3] = (U16) data[4];
				pCntx->date_time[4] = (U16) data[5];
				pCntx->date_time[5] = (U16) data[6];
				pCntx->dst = data[7];
				timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
			}
			timec_att_return_packet(pkt);
			break;

		case GATT_TIME_SERVER_UPDATE_CTRL:
			timec_notify_write_remote_value_result(pCntx, result, pkt->value_id);
			break;

		case GATT_TIME_SERVER_UPDATE_STATE:
			if (result == BT_STATUS_SUCCESS && len > 1) {
				timec_notify_read_remote_value_result(pCntx, result, pkt->value_id);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt->value_id);
			}
			timec_att_return_packet(pkt);
			break;
	}
}

static void _time_gatt_discovery_primary_service_by_uuid_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattDiscoveryAllPrimaryServiceResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattDiscoveryAllPrimaryServiceResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			// There should be only one instance of any service of Time profile
			OS_Report("[Time] Primary Service found. type: %d, uuid: 0x%04x", ptr->type, ptr->uuid2);
			OS_Report("[Time] Handle reange: 0x%04x to 0x%04x", ptr->startHandle, ptr->endHandle);
			pkt->start_handle = ptr->startHandle;
			pkt->end_handle = ptr->endHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Discovery Primary Service by UUID with result DONE_COMPLETED. success: %d",
					pkt->success_flag);
			if (pkt->success_flag == 1) {
				 _time_access_service_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle, &pkt->end_handle);
				switch (pkt->value_id) {
					case GATT_TIME_CTTIME:
						uuid = BT_UUID_CT_TIME;
						break;
					case GATT_TIME_LOCAL_TIME_INFO:
						uuid = BT_UUID_LOCAL_TIME_INFO;
						break;
					case GATT_TIME_REF_TIME_INFO:
						uuid = BT_UUID_REF_TIME_INFO;
						break;
					case GATT_TIME_DST_INFO:
						uuid = BT_UUID_TIME_WITH_DST;
						break;
					case GATT_TIME_SERVER_UPDATE_CTRL:
						uuid = BT_UUID_TIME_UPDATE_CONTROL_POINT;
						break;
					case GATT_TIME_SERVER_UPDATE_STATE:
						uuid = BT_UUID_TIME_UPDATE_STATE;
						break;
					default:
						break;
				}

				if (pkt->value_id == GATT_TIME_CTTIME_NOTIFY) {
					status = _time_gatt_discovery_characteristic_descriptors(
							pkt, pkt->start_handle, pkt->end_handle);
					OS_Report("[Time] Discovery Characteristic Descriptors, status: %d", status);

				} else {
					status = _time_gatt_discovery_characteristic_by_uuid(
							pkt, uuid, pkt->start_handle, pkt->end_handle);
					OS_Report("[Time] Discovery Characteristic, status: %d", status);
				}

				if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS)
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_discovery_characteristic_by_uuid_cb(BtGattEvent *event) {
	BtStatus status = BT_STATUS_FAILED;
	BtGattDiscoveryCharacteristicByUUIDResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;

	ptr = (BtGattDiscoveryCharacteristicByUUIDResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			OS_Report("[Time] Characteristic found. type: %d, uuid: 0x%04x", ptr->type, ptr->charUUID);
			OS_Report("[Time] Char handle: 0x%04x, property: 0x%02x ", ptr->attributeHandle, ptr->charProperty);
			OS_Report("[Time] Char value handle: 0x%04x", ptr->charValueAttributeHandle);
			pkt->start_handle = ptr->charValueAttributeHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Discovery Characteristic by UUID with result DONE_COMPLETED. success: %d",
					pkt->success_flag);
			if (pkt->success_flag == 1) {
				_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
				switch (pkt->value_id) {
					case GATT_TIME_CTTIME:
					case GATT_TIME_LOCAL_TIME_INFO:
					case GATT_TIME_REF_TIME_INFO:
					case GATT_TIME_DST_INFO:
					case GATT_TIME_SERVER_UPDATE_STATE:
						status = _time_gatt_read_characteristic_value(pkt, pkt->start_handle);
						break;

					case GATT_TIME_SERVER_UPDATE_CTRL:
						status = _time_gatt_write_without_response(pkt, pkt->start_handle);
						break;

					default:
						break;
				}
				
				if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS)
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_read_characteristic_value_cb(BtGattEvent *event) {
	// To be modified.
	BtStatus status;
	BtGattReadCharacteristicValueResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattReadCharacteristicValueResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Read Characteristic Value with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, ptr->data, ptr->length, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_write_without_response_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattWriteWithoutResponseResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattWriteWithoutResponseResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Write withou Response with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

static void _time_gatt_discovery_characteristic_descriptors_cb(BtGattEvent *event) {
	BtStatus status = BT_STATUS_FAILED;
	BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			OS_Report("[Time] Characteristic Descriptor found. type: %d, uuid: 0x%04x", ptr->type, ptr->uuid16);
			OS_Report("[Time] Char desc value handle: 0x%04x", ptr->attributeHandle);
			pkt->start_handle = ptr->attributeHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Read Characteristic Descriptor with result DONE_COMPLETED");
			if (pkt->success_flag != 1) {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
				return;
			}

			_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
			if (pkt->value_id == GATT_TIME_CTTIME_NOTIFY) {
				switch (pkt->type) {
					case TIME_PKT_READ:
						status = _time_gatt_read_characteristic_descriptor(pkt, pkt->start_handle);
						break;
					case TIME_PKT_WRITE:
						status = _time_gatt_write_characteristic_descriptor(pkt, pkt->start_handle);
						break;
				}

				if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
				}
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_read_characteristic_descriptor_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattReadCharacteristicDescriptorsResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattReadCharacteristicDescriptorsResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Read Characteristic Descriptor with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, ptr->data, ptr->length, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

static void _time_gatt_write_characteristic_descriptor_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattWriteCharDescriptorResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattWriteCharDescriptorResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Write Characteristic Descriptor with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

void time_gatt_ap_callback(void *user_data, BtGattEvent *event) {
	OS_Report("[Time] time_gatt_ap_callback(), event: %d", event->eType);
	event->pGattOp = user_data;

	switch (event->eType) {
		case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
			_time_gatt_discovery_primary_service_by_uuid_cb(event);
			break;
		case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
			_time_gatt_discovery_characteristic_by_uuid_cb(event);
			break;
		case BT_GATT_OPERATOR_READ_CHAR_VALUE:
			_time_gatt_read_characteristic_value_cb(event);
			break;
		case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
			_time_gatt_write_without_response_cb(event);
			break;
		case BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS:
			_time_gatt_read_characteristic_descriptor_cb(event);
			break;
		case BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS:
			_time_gatt_write_characteristic_descriptor_cb(event);
			break;
	}
}

static BtStatus _time_gatt_discovery_primary_service_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dps_uuid.startHandle = start;
	pkt->gatt_params.dps_uuid.endHandle = end;
	pkt->gatt_params.dps_uuid.type = 0x02;
	pkt->gatt_params.dps_uuid.uuid16[0] = uuid & 0xff;
	pkt->gatt_params.dps_uuid.uuid16[1] = uuid >> 8;
	gatt_op->parms.p.primaryServiceByUUID = &pkt->gatt_params.dps_uuid;

	return BT_GATT_DiscoveryPrimaryServiceByUUID(pChnl->handler.bdc->link, gatt_op);
}

static BtStatus _time_gatt_discovery_characteristic_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dch_uuid.startHandle = start;
	pkt->gatt_params.dch_uuid.endHandle = end;
	pkt->gatt_params.dch_uuid.type = 0x02;
	pkt->gatt_params.dch_uuid.uuid16[0] = uuid & 0xff;
	pkt->gatt_params.dch_uuid.uuid16[1] = uuid >> 8;
	gatt_op->parms.p.charByUUID = &pkt->gatt_params.dch_uuid;

	return BT_GATT_DiscoveryCharacteristicByUUID(pChnl->handler.bdc->link, gatt_op);
}

static BtStatus _time_gatt_read_characteristic_value(TimePacket *pkt, U16 char_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.rch.handle = char_handle;
	gatt_op->parms.p.readCharValue = &pkt->gatt_params.rch;

	return BT_GATT_ReadCharacteristicValue(pChnl->handler.bdc->link, gatt_op);
}

BtStatus _time_gatt_write_without_response(TimePacket *pkt, U16 char_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;
	U8 value = (U8) pkt->value;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.wch_wo_rsp.handle = char_handle;
	pkt->gatt_params.wch_wo_rsp.size = 1;
	pkt->gatt_params.wch_wo_rsp.data = &value;
	gatt_op->parms.p.writeWithoutResponse = &pkt->gatt_params.wch_wo_rsp;

	return BT_GATT_WriteWithoutResponse(pChnl->handler.bdc->link, gatt_op);
}

static BtStatus _time_gatt_discovery_characteristic_descriptors(
		TimePacket *pkt, U16 start, U16 end) {

	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dds.startHandle = start;
	pkt->gatt_params.dds.endHandle = end;
	gatt_op->parms.p.allCharDescriptors= &pkt->gatt_params.dds;

	return BT_GATT_DiscoveryAllCharacteristicDescriptors(pChnl->handler.bdc->link, gatt_op);
}

static BtStatus _time_gatt_read_characteristic_descriptor(TimePacket *pkt, U16 desc_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.rds.handle = desc_handle;
	gatt_op->parms.p.readCharDescriptors = &pkt->gatt_params.rds;

	return BT_GATT_ReadCharacteristicDescriptors(pChnl->handler.bdc->link, gatt_op);
}

static BtStatus _time_gatt_write_characteristic_descriptor(TimePacket *pkt, U16 desc_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.wds.handle = desc_handle;
	pkt->gatt_params.wds.size = 2;
	pkt->gatt_params.wds.data = (U8 *) &pkt->value;
	gatt_op->parms.p.writeDescriptors = &pkt->gatt_params.wds;

	return BT_GATT_WriteCharacteristicDescriptors(pChnl->handler.bdc->link, gatt_op);
}
#endif

/********************************************************************************************
 * Interface Functions for Time Server
 ********************************************************************************************/
 #ifdef __BT_TIMES_PROFILE__
void times_cmgr_callback(CmgrHandler *handler, CmgrEvent event, BtStatus status) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = ContainingRecord(handler, TimeSTransport, handler);
	pCntx = ContainingRecord(pChnl, TimeSContext, chnl);

	OS_Report("[Time] times_cmgr_callback, event: %d, status: %d", event, status);

	switch (event) {
		case CMEVENT_DATA_LINK_DIS:
			if (pChnl != NULL && btmtk_os_memcmp(pChnl->bdaddr.addr, 6, handler->remDev->bdAddr.addr, 6)) {
				CMGR_RemoveDataLink(handler);
				_return_server_tp(pChnl);
				times_notify_disconnected(pCntx);
			}
			break;

		default:
			break;
	}
}

void time_gatt_notify_value_callback(void *user_data, BtGattEvent *event) {
	BtGattValueNotificationResultEvent *ptr;
	TimePacket *pkt;
	TimeSContext *pCntx;

	OS_Report("[Time] time_gatt_notify_value_callback(), event: 0x%02x (should be 0x12)", event->eType);
	if (event->eType != BT_GATT_OPERATOR_NOTIFICATIONS) {
		return;
	}

	ptr = (BtGattValueNotificationResultEvent *) event->parms;
	pkt = ContainingRecord(user_data, TimePacket, gatt_op);
	pCntx = ContainingRecord(pkt->chnl, TimeSContext, chnl);

	OS_Report("[Time] Notify Value with result: %d", ptr->flag);

	if (pCntx != NULL) {
		times_notify_notify_value_result(pCntx);
	} else {
		OS_Report("[Time] ERROR: %s() with NULL context", __FUNCTION__);
	}
}

static BtStatus _time_gatt_notify_value(TimePacket *pkt, U16 value_handle, U16 size, U8 *data) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeSTransport *pChnl = (TimeSTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_notify_value_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.vno.handle = value_handle;
	pkt->gatt_params.vno.size = size;
	pkt->gatt_params.vno.data = data;
	gatt_op->parms.p.valueNotifications = &pkt->gatt_params.vno;

	return BT_GATT_Notifications(pChnl->handler.bdc->link, gatt_op);
}

BtStatus time_gatt_notify_incoming(BtRemoteDevice *link) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = _get_available_server_tp(link);
	if (pChnl != NULL) {
		CMGR_CreateDataLink(&pChnl->handler, &link->bdAddr);
		btmtk_os_memcpy(pChnl->bdaddr.addr, link->bdAddr.addr, sizeof(link->bdAddr));

		pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
		times_notify_incoming(pCntx, link->bdAddr.addr);

		return BT_STATUS_SUCCESS;
	}
	return BT_STATUS_FAILED;
}

U8 time_att_db_cttime_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	U8 status = BT_STATUS_FAILED;
	time_t rawtime;
	struct tm *timeinfo;

	switch (type) {
		case ATT_OP_READ:
			time(&rawtime);
			timeinfo = localtime(&rawtime);

			_time_server_info.ct_time.year = timeinfo->tm_year + 1900;
			_time_server_info.ct_time.month = (U8) timeinfo->tm_mon + 1;
			_time_server_info.ct_time.day = (U8) timeinfo->tm_mday;
			_time_server_info.ct_time.hours = (U8) timeinfo->tm_hour;
			_time_server_info.ct_time.minutes = (U8) timeinfo->tm_min;
			_time_server_info.ct_time.seconds = (U8) timeinfo->tm_sec;
			_time_server_info.ct_time.wday = (U8) timeinfo->tm_wday + 1;
			_time_server_info.ct_time.frac = 0;
			_time_server_info.ct_time.adj_reason = 0;

			*raw = (U8 *) &_time_server_info.ct_time;
			*len = sizeof(_time_server_info.ct_time);
			status = BT_STATUS_SUCCESS;
			break;

		default:
			break;
	}
	
	return status;
}

U8 time_att_db_cttime_config_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	pChnl = _find_server_tp_by_remdev(link);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ) {
		*raw = (U8 *) &pChnl->cttime_notify_config;
		*len = sizeof(pChnl->cttime_notify_config);
		return BT_STATUS_SUCCESS;

	} else if (type == ATT_OP_WRITE) {
		if (*len != 2) {
			return BT_STATUS_FAILED;
		} else {
			pChnl->cttime_notify_config = (0x00ff & *raw[0]) | (0xff00 & (*raw[1] << 8));
			OS_Report("[Time] Current Time notify config updated: 0x%04x", pChnl->cttime_notify_config);
			return BT_STATUS_SUCCESS;
		}
	}
	return BT_STATUS_FAILED;
}

U8 time_att_db_local_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	pChnl = _find_server_tp_by_remdev(link);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ) {
		*raw = (U8 *) &_time_server_info.local_time_info;
		*len = sizeof(_time_server_info.local_time_info);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_ref_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	pChnl = _find_server_tp_by_remdev(link);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ) {
		*raw = (U8 *) &_time_server_info.ref_time_info;
		*len = sizeof(_time_server_info.ref_time_info);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_time_with_dst_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	pChnl = _find_server_tp_by_remdev(link);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ) {
		*raw = (U8 *) &_time_server_info.time_with_dst;
		*len = sizeof(_time_server_info.time_with_dst);
		return BT_STATUS_FAILED;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_update_ctrl_point_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = _find_server_tp_by_remdev(link);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
	if (type == ATT_OP_WRITE) {
		if ((pChnl->update_status.cur_state == TIME_UPDATE_STATE_IDLE) &&
				(*raw[0] == TIME_UPDATE_CTRL_REQUEST)) {
			pChnl->update_status.cur_state = TIME_UPDATE_STATE_UPDATE_PENDING;
			times_notify_request_server_update(pCntx);
			return BT_STATUS_SUCCESS;

		} else if ((pChnl->update_status.cur_state == TIME_UPDATE_STATE_UPDATE_PENDING) &&
				(*raw[0] == TIME_UPDATE_CTRL_CANCEL)) {
			pChnl->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
			pChnl->update_status.result = TIME_UPDATE_RESULT_CANCELED;
			times_notify_cancel_server_update(pCntx);
			return BT_STATUS_SUCCESS;
		}
	}
	return BT_STATUS_FAILED;
}

U8 time_att_db_update_state_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl = _find_server_tp_by_remdev(link);

	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ) {
		*raw = (U8 *) &pChnl->update_status;
		*len = sizeof(pChnl->update_status);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}
#endif

/********************************************************************************************
 * Common Interface Functions, shared by Time client and server
 ********************************************************************************************/
void time_gatt_connection_callback(void *handler, BtGattEvent *event) {
#ifdef __BT_TIMEC_PROFILE__
	TimeCContext *pCntx;
#endif

	switch (event->eType) {
		case BT_GATT_APP_EVENT_CONNECTED:
			OS_Report("[Time] BT_GATT_APP_EVENT_CONNECTED");

#ifdef __BT_TIMEC_PROFILE__
			if (event->link && _cur_timec_chnl) {
				pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
				timec_notify_connect_result(pCntx, BT_STATUS_SUCCESS);
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x", event->link);
			}
#endif

			break;

		case BT_GATT_APP_EVENT_DISCONNECTED:
			OS_Report("[Time] BT_GATT_APP_EVENT_DISCONNECTED");

#ifdef __BT_TIMEC_PROFILE__
			if (event->link && _cur_timec_chnl) {
				pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
				timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
				time_att_destroy_link(_cur_timec_chnl);
				_cur_timec_chnl = NULL;
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x", event->link);
			}
#endif

			break;

		case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
			OS_Report("[Time] BT_GATT_APP_EVENT_VALUE_NOTIFICATION");

#ifdef __BT_TIMEC_PROFILE__
			if (event->link && _cur_timec_chnl) {
				BtGattValueNotificationEvent *ptr = (BtGattValueNotificationEvent *) event->parms;
				if (ptr->attributeValue && ptr->attributeValueLen > 9) {
					pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
					pCntx->date_time[0] = (0x00ff & ptr->attributeValue[0]) | (0xff00 & (ptr->attributeValue[1] << 8));
					pCntx->date_time[1] = (U16) ptr->attributeValue[2];
					pCntx->date_time[2] = (U16) ptr->attributeValue[3];
					pCntx->date_time[3] = (U16) ptr->attributeValue[4];
					pCntx->date_time[4] = (U16) ptr->attributeValue[5];
					pCntx->date_time[5] = (U16) ptr->attributeValue[6];
					pCntx->day_of_week = ptr->attributeValue[7];
					pCntx->frac = ptr->attributeValue[8];
					pCntx->adj_reason = ptr->attributeValue[9];
					timec_notify_server_notification(pCntx, GATT_TIME_CTTIME);
				}
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x", event->link);
			}
#endif

			break;

		case BT_GATT_APP_EVENT_VALUE_INDICATION:
			OS_Report("[Time] BT_GATT_APP_EVENT_VALUE_INDICATION");
			break;

		case BT_GATT_APP_EVENT_SERVER:
			OS_Report("[Time] BT_GATT_APP_EVENT_SERVER");

#ifdef __BT_TIMES_PROFILE__
			time_gatt_notify_incoming(event->link);
#endif

			break;

		default:
			OS_Report("[Time] Invalid event: %d", event->eType);
			break;
	}
}


/********************************************************************************************
 * Exported functions
 ********************************************************************************************/
BtStatus time_att_init() {
	_init_time_packet_list();
	BT_GattRegisterConnectedEvent(&_time_gatt_handler, time_gatt_connection_callback);
	return BT_STATUS_SUCCESS;
}

#ifdef __BT_TIMES_PROFILE__
BtStatus time_att_register(void *channel, void *callback, void *info) {
	TimeSTransport *chnl = (TimeSTransport *) channel;
	L2capPsm *psm = &chnl->psm;
	BtStatus status;
	U8 i;

	OS_Report("[Time] %s, channel 0x%x", __FUNCTION__, channel);
	TIMEASSERT(chnl->check_byte == TIME_CHECK_BYTE);

	status = CMGR_RegisterHandler(&chnl->handler, times_cmgr_callback);
	OS_Report("[Time] CMGR_RegisterHandler() returns %d", status);

	if (_time_server_tp_list_init != TRUE) {
		_time_server_tp_list_init = TRUE;
		InitializeListHead(&_time_server_tp_list);
		for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
			_time_server_used_tps[i] = NULL;
		}
	}
	InsertTailList(&_time_server_tp_list, &(chnl->node));

	return BT_STATUS_SUCCESS;
}

BtStatus time_att_deregister(void *channel) {
	TimeSTransport *chnl = (TimeSTransport *) channel;
	TIMEASSERT(chnl->check_byte == TIME_CHECK_BYTE);

	if (_time_server_tp_list_init == TRUE) {
		RemoveEntryList(&(chnl->node));
	}

	return BT_STATUS_SUCCESS;
}

BtStatus time_att_register_record(void *channel) {
	// For BR/EDR mode, register 3 service records.
	TimeSdpDB_AddRecord(0, 0xff);
	TimeAttDB_AddRecord();
	return BT_STATUS_SUCCESS;
}

BtStatus time_att_deregister_record(void *channel) {
	TimeSdpDB_RemoveRecord(0, 0xff);
	TimeAttDB_RemoveRecord();
	return BT_STATUS_SUCCESS;
}

BtStatus time_att_accept(void *channel) {
	TimeSTransport *pChnl = (TimeSTransport *) channel;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	// Return success directly.
	return BT_STATUS_SUCCESS;
}

BtStatus time_att_reject(void *channel) {
	OS_Report("[Time] %s()", __FUNCTION__);
	return time_att_server_disconnect(channel);
}

BtStatus time_att_write_local_value(void *cntx, U8 value_id) {
	TimeSContext *pCntx = (TimeSContext *) cntx;
	TimeSTransport *server_tp;
	U8 i = 0;

	if (pCntx != NULL) {
		switch (value_id) {
			case GATT_TIME_LOCAL_TIME_INFO:
				_time_server_info.local_time_info.time_zone = pCntx->time_zone;
				_time_server_info.local_time_info.curr_dst_offset = pCntx->dst;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_REF_TIME_INFO:
				_time_server_info.ref_time_info.time_source = pCntx->time_source;
				_time_server_info.ref_time_info.time_accuracy = pCntx->accuracy;
				_time_server_info.ref_time_info.days_since_update = pCntx->days_since_update;
				_time_server_info.ref_time_info.hours_since_update = pCntx->hours_since_update;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_DST_INFO:
				_time_server_info.time_with_dst.year = pCntx->date_time[0];
				_time_server_info.time_with_dst.month = (U8) pCntx->date_time[1];
				_time_server_info.time_with_dst.day = (U8) pCntx->date_time[2];
				_time_server_info.time_with_dst.hours = (U8) pCntx->date_time[3];
				_time_server_info.time_with_dst.minutes = (U8) pCntx->date_time[4];
				_time_server_info.time_with_dst.seconds = (U8) pCntx->date_time[5];
				_time_server_info.time_with_dst.next_dst_offset = (U8) pCntx->dst;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_SERVER_UPDATE_STATE:
				for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
					server_tp = _time_server_used_tps[i];
					if (server_tp != NULL &&
							(server_tp->update_status.cur_state == TIME_UPDATE_STATE_UPDATE_PENDING)) {
						server_tp->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
						server_tp->update_status.result = pCntx->update_result;
					}
				}
				return BT_STATUS_SUCCESS;

			default:
				OS_Report("[Time] Write local value failed, value_id: %d", value_id);
		}
	} 
	return BT_STATUS_FAILED;
}

BtStatus time_att_notify_value(void *packet, U8 seq_num, U32 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimePacket *pkt = (TimePacket *) packet;
	TimeSTransport *pChnl = pkt->chnl;
	TimeSContext *pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
	U8 *data = NULL;
	U16 value_handle = 0x0000, size = 0;
	time_t rawtime;
	struct tm *timeinfo;

	if (pCntx != NULL && (pChnl->cttime_notify_config & 0x0001)) {
		switch (value_id) {
			case GATT_TIME_CTTIME:
				// TODO: Check don't-notify conditions. Within 15 min, Time adjust < 1min, etc.
				time(&rawtime);
				timeinfo = localtime(&rawtime);

				_time_server_info.ct_time.year = timeinfo->tm_year + 1900;
				_time_server_info.ct_time.month = (U8) timeinfo->tm_mon + 1;
				_time_server_info.ct_time.day = (U8) timeinfo->tm_mday;
				_time_server_info.ct_time.hours = (U8) timeinfo->tm_hour;
				_time_server_info.ct_time.minutes = (U8) timeinfo->tm_min;
				_time_server_info.ct_time.seconds = (U8) timeinfo->tm_sec;
				_time_server_info.ct_time.wday = (U8) timeinfo->tm_wday + 1;
				_time_server_info.ct_time.frac = 0;
				_time_server_info.ct_time.adj_reason = pCntx->adj_reason;

				value_handle = ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE;
				data = (U8 *) &_time_server_info.ct_time;
				size = sizeof(_time_server_info.ct_time);
				break;
		}
		status = _time_gatt_notify_value(packet, value_handle, size, data);
	}
	return status;
}

BtStatus time_att_server_disconnect(void *channel) {
	BtStatus status;
	TimeSTransport *pChnl = (TimeSTransport *) channel;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);
	OS_Report("[Time] %s()", __FUNCTION__);

	status = GattDisconnect(pChnl->handler.remDev);
	OS_Report("[Time] GattDisconnect() returns %d", status);

	status = CMGR_RemoveDataLink(&pChnl->handler);
	OS_Report("[Time] CMGR_RemoveDataLink() returns %d", status);

	_return_server_tp(pChnl);

	return status;
}
#endif

#ifdef __BT_TIMEC_PROFILE__
BtStatus time_att_create_link(void *channel, unsigned char *addr) {
	BtStatus status;
	TimeCTransport *pChnl = (TimeCTransport *) channel;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	// memcpy(&chnl->bdaddr.addr, addr, 6);
	pChnl->bdaddr.addr[5] = addr[5];
	pChnl->bdaddr.addr[4] = addr[4];
	pChnl->bdaddr.addr[3] = addr[3];
	pChnl->bdaddr.addr[2] = addr[2];
	pChnl->bdaddr.addr[1] = addr[1];
	pChnl->bdaddr.addr[0] = addr[0];

	memset(&pChnl->handler, 0, sizeof(pChnl->handler));
	status = CMGR_RegisterHandler(&pChnl->handler, (CmgrCallback) timec_cmgr_callback);
	OS_Report("[Time] %s(), register CMGR handler, status: %d", __FUNCTION__, status);

	if (status == BT_STATUS_SUCCESS) {
		status = CMGR_CreateDataLink(&pChnl->handler, &pChnl->bdaddr);
		OS_Report("[Time] %s(), create data link to %x:%x:%x:%x:%x:%x, status: %d", __FUNCTION__,
				addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], status);

		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			CMGR_DeregisterHandler(&(pChnl->handler));
		}
	}

	return status;
}

static BtStatus _timec_att_query_service(TimeCTransport *pChnl, U16 uuid, SdapCallBack cb) {
	BtStatus status = BT_STATUS_FAILED;
	sdap_service_search_struct search_pattern;

	if (pChnl->handler.remDev) {
		pChnl->sdp_query_token.uuid = uuid,
		pChnl->sdp_query_token.mode = BSPM_BEGINNING;

		search_pattern.rm = pChnl->handler.remDev;
		search_pattern.sqt = &pChnl->sdp_query_token;
		search_pattern.uuid = uuid;
		search_pattern.callback = cb;

		status = SDAP_ServiceSearch(search_pattern);
		OS_Report("[Time] SDAP_ServiceSearch() returns %d", status);
	}
	return status;
}

BtStatus time_att_query_service(void *channel) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCTransport *pChnl = (TimeCTransport *) channel;

	if (pChnl == NULL) {
		return status;
	}
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	return _timec_att_query_service(pChnl, BT_UUID_GATT_SERVICE_TIME, timec_sdp_query_callback);
}

BtStatus time_att_connect_service(void *channel) {
	BtStatus status;
	TimeCTransport *pChnl = (TimeCTransport *) channel;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	if (pChnl->handler.remDev) {
		_cur_timec_chnl = pChnl;
		// Wait for BT_GATT_APP_EVENT_CONNECTED
		status = GattClientConnect(pChnl->handler.remDev);
		OS_Report("[Time] GattClientConnect() returns %d", status);
	} else {
		status = BT_STATUS_INVALID_PARM;
		OS_Report("[Time] Error: pChnl->handler.remDev is NULL");
	}

	return status;
}

BtStatus time_att_destroy_link(void *channel) {
	BtStatus status;
	TimeCTransport *pChnl = (TimeCTransport *) channel;

	status = CMGR_RemoveDataLink(&pChnl->handler);
	status = CMGR_DeregisterHandler(&pChnl->handler);

	// Always return success
	return BT_STATUS_SUCCESS;
}

BtStatus time_att_disconnect_service(void *channel) {
	BtStatus status;
	TimeCTransport *pChnl = (TimeCTransport *) channel;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	status = GattDisconnect(pChnl->handler.remDev);
	if (status == BT_STATUS_SUCCESS) {
		time_att_destroy_link(pChnl);
	}

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}

BtStatus time_att_read_remote_value(void *packet, U8 seq_num, U32 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimePacket *pkt = (TimePacket *) packet;
	TimeCTransport *pChnl = pkt->chnl;
	U16 value_handle = 0x0000, start_handle = 0x0000, end_handle = 0x0000;
	U16 value_uuid = 0, service_uuid = 0;

	pkt->seq_num = seq_num;
	pkt->value_id = value_id;
	pkt->type = TIME_PKT_READ;
	pkt->value = 0;

	_time_access_value_handle_cache(pChnl, value_id, 0, &value_handle);
	_time_access_service_handle_cache(pChnl, value_id, 0, &start_handle, &end_handle);

	switch (value_id) {
		case GATT_TIME_CTTIME:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_CT_TIME;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_LOCAL_TIME_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_LOCAL_TIME_INFO;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_REF_TIME_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_REF_TIME_INFO;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_DST_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_TIME_WITH_DST;
				service_uuid = BT_UUID_GATT_SERVICE_DST;
			}
			break;

		case GATT_TIME_SERVER_UPDATE_STATE:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_TIME_UPDATE_STATE;
				service_uuid = BT_UUID_GATT_SERVICE_TIMEUPDATE;
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_descriptor(pkt, value_handle);
			} else {
				if (start_handle != 0x0000 && end_handle != 0x0000) {
					status = _time_gatt_discovery_characteristic_descriptors(pkt, start_handle, end_handle);
				} else {
					status = _time_gatt_discovery_primary_service_by_uuid(
							pkt, BT_UUID_GATT_SERVICE_TIME, 0x0001, 0xffff);
				}
			}
			return status;

		default:
			return BT_STATUS_FAILED;
	}

	if (start_handle != 0x0000 && end_handle != 0x0000) {
		status = _time_gatt_discovery_characteristic_by_uuid(pkt, value_uuid, start_handle, end_handle);
	} else {
		status = _time_gatt_discovery_primary_service_by_uuid(pkt, service_uuid, 0x0001, 0xffff);
	}

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}

// In Time Profile, the largest value size is 2-byte.
BtStatus time_att_write_remote_value(void *packet, U8 seq_num, U32 value_id, U16 value) {
	BtStatus status = BT_STATUS_FAILED;
	TimePacket *pkt = (TimePacket *) packet;
	TimeCTransport *pChnl = pkt->chnl;
	U16 value_handle = 0x0000, start_handle = 0x0000, end_handle = 0x0000;
	U16 value_uuid = 0, service_uuid = 0;

	pkt->seq_num = seq_num;
	pkt->value_id = value_id;
	pkt->type = TIME_PKT_WRITE;
	pkt->value = value;

	_time_access_value_handle_cache(pChnl, value_id, 0, &value_handle);
	_time_access_service_handle_cache(pChnl, value_id, 0, &start_handle, &end_handle);

	switch (value_id) {
		case GATT_TIME_SERVER_UPDATE_CTRL:
			if (value_handle != 0x0000) {
				status = _time_gatt_write_without_response(pkt, value_handle);
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			if (value_handle != 0x0000) {
				status = _time_gatt_write_characteristic_descriptor(pkt, value_handle);
			} else {
				if (start_handle != 0x0000 && end_handle != 0x0000) {
					status = _time_gatt_discovery_characteristic_descriptors(pkt, start_handle, end_handle);
				} else {
					status = _time_gatt_discovery_primary_service_by_uuid(
							pkt, BT_UUID_GATT_SERVICE_TIME, 0x0001, 0xffff);
				}
			}
			return status;

		default:
			return BT_STATUS_FAILED;
	}

	if (start_handle != 0x0000 && end_handle != 0x0000) {
		status = _time_gatt_discovery_characteristic_by_uuid(pkt, value_uuid, start_handle, end_handle);
	} else {
		status = _time_gatt_discovery_primary_service_by_uuid(pkt, service_uuid, 0x0001, 0xffff);
	}

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}
#endif

#endif /* #if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__) */

