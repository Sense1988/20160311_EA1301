#ifndef __TIME_ADAPTER_H
#define __TIME_ADAPTER_H

#include "conmgr.h"
#include "gatt.h"

// Time packet type definitions
#define TIME_PKT_READ			0x00
#define TIME_PKT_WRITE			0x01
#define TIME_PKT_NOTIFY		0x02

// Valid values for Time Update Control Point
#define TIME_UPDATE_CTRL_REQUEST				0x01
#define TIME_UPDATE_CTRL_CANCEL				0x02

// Valid values for Time Update Status, Current State
#define TIME_UPDATE_STATE_IDLE				0
#define TIME_UPDATE_STATE_UPDATE_PENDING	1

// Valid values for Time Update Status, Result
#define TIME_UPDATE_RESULT_SUCCESSFUL		0
#define TIME_UPDATE_RESULT_CANCELED			1
#define TIME_UPDATE_RESULT_NO_CONNECTION	2
#define TIME_UPDATE_RESULT_ERROR				3
#define TIME_UPDATE_RESULT_TIMEOUT			4
#define TIME_UPDATE_RESULT_NOT_ATTEMPETED	5

typedef struct {
	ListEntry node;
	BtPacket packet;
	U8 check_byte;
	U8 seq_num;
	U32	value_id;
	U8 type;
	U16 value;
	void *chnl;
	
	BtGattOp gatt_op;
	U16 start_handle;
	U16 end_handle;
	U8 success_flag;
	union {
		BtGattDiscoveryPrimaryServiceByUUID dps_uuid;
		BtGattDiscoveryCharacteristicByUUID dch_uuid;
		BtGattDiscoveryAllCharacteristicDescriptors dds;
		BtGattReadCharacteristicValue rch;
		BtGattWriteWithoutResponse wch_wo_rsp;
		BtGattValueNotifications vno;
		BtGattReadCharacteristicDescriptors rds;
		BtGattWriteDescriptors wds;
	} gatt_params;
} TimePacket;

// Shared information, will be prepared when enabling server
typedef struct {
	// Current Time :: CT Time
	struct {
		U16 year;
		U8 month;
		U8 day;
		U8 hours;
		U8 minutes;
		U8 seconds;
		U8 wday;
		U8 frac;
		U8 adj_reason;
	} ct_time;

	// Current Time :: Local Time Information
	struct {
		U8 time_zone;
		U8 curr_dst_offset;
	} local_time_info;

	// Current Time :: Reference Time Information
	struct {
		U8 time_source;
		U8 time_accuracy;
		U8 days_since_update;
		U8 hours_since_update;
	} ref_time_info;

	// Next DST :: Time with DST
	struct {
		U16 year;
		U8 month;
		U8 day;
		U8 hours;
		U8 minutes;
		U8 seconds;
		U8 next_dst_offset;
	} time_with_dst;
} TimeServerSharedInfo;

// For every connection
typedef struct {
	BtRemoteDevice *link;
	U8 in_use;
	U16 cttime_notify_config;
	U8 update_ctrl_point;
	struct {
		U8 cur_state;
		U8 result;
	} update_status;
} TimeServerConfigInfo;

TimePacket* timec_att_get_packet(void *cntx);
void timec_att_return_packet();

#define TIMEC_ATT_Get_Packet				timec_att_get_packet
#define TIMEC_ATT_Return_Packet			timec_att_return_packet

TimePacket* times_att_get_packet(void *cntx);
void times_att_return_packet();

#define TIMES_ATT_Get_Packet				times_att_get_packet
#define TIMES_ATT_Return_Packet			times_att_return_packet

// Synchronous ATT adaptation functions
BtStatus time_att_init();
BtStatus time_att_register(void *channel, void *callback, void *info);
BtStatus time_att_register_record(void *channel);
BtStatus time_att_deregister(void *channel);
BtStatus time_att_deregister_record(void *channel);
BtStatus time_att_write_local_value(void *cntx, U8 value_id);

#define TIME_ATT_Init					time_att_init
#define TIME_ATT_Register				time_att_register
#define TIME_ATT_Register_Record		time_att_register_record
#define TIME_ATT_Deregister				time_att_deregister
#define TIME_ATT_Deregister_Record		time_att_deregister_record
#define TIME_ATT_Write_Local_Value		time_att_write_local_value

// Asynchronous ATT adaptation functions
BtStatus time_att_accept(void *channel);
BtStatus time_att_reject(void *channel);
BtStatus time_att_server_disconnect(void *channel);
BtStatus time_att_notify_value(void* packet, U8 seq_num, U32 value_id);

BtStatus time_att_create_link(void *channel, unsigned char *addr);
BtStatus time_att_destroy_link(void *channel);
BtStatus time_att_query_service(void *channel);
BtStatus time_att_connect_service(void *channel);
BtStatus time_att_disconnect_service(void *channel);

BtStatus time_att_read_remote_value(void *packet, U8 seq_num, U32 value_id);
BtStatus time_att_write_remote_value(void *packet, U8 seq_num, U32 value_id, U16 value);

#define TIME_ATT_Accept					time_att_accept
#define TIME_ATT_Reject					time_att_reject
#define TIME_ATT_Server_Disconnect		time_att_server_disconnect
#define TIME_ATT_Notify_Value			time_att_notify_value

#define TIME_ATT_Create_Link			time_att_create_link
#define TIME_ATT_Destroy_Link			time_att_destroy_link
#define TIME_ATT_Query_Service			time_att_query_service
#define TIME_ATT_Connect_Service		time_att_connect_service
#define TIME_ATT_Disconnect_Service		time_att_disconnect_service

#define TIME_ATT_Read_Remote_Value	time_att_read_remote_value
#define TIME_ATT_Write_Remote_Value	time_att_write_remote_value

#endif
