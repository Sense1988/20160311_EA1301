#ifndef __BT_HDP_ADP_H__
#define __BT_HDP_ADP_H__

#include "hdp.h"
#include "bluetooth_struct.h"

#define HDP_ADP_MAX_INSTANCE_NUM 	10
#define HDP_ADP_MAX_CHANNEL_NUM 	20
#define HDP_ADP_INSTANCE_SOCKET_ADDR_LEN 255
#define HDP_ADP_MAX_DATA_LEN		1024

typedef enum
{
	HDP_ADP_CHANNEL_IDLE,			//the channel is idle
	HDP_ADP_CHANNEL_CONNECTING,			//the channel is in connecting or disconnecting
	HDP_ADP_CHANNEL_DISCONNECTING,			//the channel is in connecting or disconnecting
	HDP_ADP_CHANNEL_CONNECTED,			//the channel is connected
} hdp_adp_channel_state;

typedef struct
{	
	ListEntry  				node;
	BOOL 					inUse;
	hdp_adp_channel_state 	state;
	bt_addr_struct			bdaddr;
	U8						mdepId; // the instance ID
	U16 				mdlId;
	L2capChannelId 			l2capChnId;
	BOOL 					mainChn;
	U32					index;

	/*Notes: the non-conflict water mark is very important to identify l2cap channel instead of real L2CAP channel ID,*/
	/* 		due to the fact that the L2CAP channel ID may be reused.*/
	/*		If upper layer transit data using socket based on L2CAP ID, it will fail to create socket with same socket address again */
	/*		when L2CAP channel is reused in short duration */
	U16			        waterMark;  
} hdp_adp_channel_struct;

typedef struct
{
	BOOL 	single;  //TRUE: some instance has registered in the record
	U8 		mdepId;
	
} hdp_adp_instance_struct;

typedef struct
{
	BOOL initialized;
	hdp_adp_instance_struct instance[HDP_ADP_MAX_INSTANCE_NUM];
	ListEntry  				channel;
} hdp_adp_context_struct;

#endif

