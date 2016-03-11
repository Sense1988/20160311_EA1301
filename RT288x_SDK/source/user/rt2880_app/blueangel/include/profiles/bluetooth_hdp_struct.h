#ifndef _BLUETOOTH_HDP_STRUCT_H_
#define _BLUETOOTH_HDP_STRUCT_H_

#define BT_HDP_INSTANCE_MAX_DESC_LEN 255
#define BT_HDP_INSTANCE_INVALID_ID 0xFF

#ifndef BOOL
typedef unsigned char BOOL;
#endif

typedef enum
{
	BT_HDP_SUCCESS,
	BT_HDP_FAIL,
	BT_HDP_FAIL_BUSY,
	BT_HDP_FAIL_NOT_ALLOW,
	BT_HDP_FAIL_TIMEOUT,
	BT_HDP_FAIL_REMOTE_REJECT,
	BT_HDP_FAIL_NO_RESOURCE,
} bt_hdp_status;


typedef enum
{
	BT_HDP_ROLE_SOURCE = 0x00,
	BT_HDP_ROLE_SINK 	= 0x01,
	BT_HDP_ROLE_INVALID = 0xFF,
} bt_hdp_role;

typedef enum 
{
	BT_HDP_CHANNEL_TYPE_NO_PREFERENCE	= 0x00,
	BT_HDP_CHANNEL_TYPE_RELIABLE		= 0x01,
	BT_HDP_CHANNEL_TYPE_STREAMING		= 0x02,	
} bt_hdp_channel_type;


typedef struct 
{
	LOCAL_PARA_HDR
	U8							role;
	U16 						dataType;
	U8  				    channelType;
	char 	description[BT_HDP_INSTANCE_MAX_DESC_LEN];	
} bt_hdp_register_instance_req_struct;


typedef struct 
{
	LOCAL_PARA_HDR
	U8	result;
	U8   mdepId;
} bt_hdp_register_instance_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	U8   mdepId;	
} bt_hdp_deregister_instance_req_struct;


typedef struct 
{
	LOCAL_PARA_HDR
	U8	result;
	U8   mdepId;
} bt_hdp_deregister_instance_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U8 				mdepId;
	U8				config;
	U32				index;   //internal indicator
} bt_hdp_connect_req_struct;


typedef struct 
{
	LOCAL_PARA_HDR
	U8 				result;
	bt_addr_struct 	bdaddr;
	U8 				mdepId;
	U16				mdlId;
	BOOL			mainChannel;
	U32				index; 
} bt_hdp_connect_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U16				mdlId;
	U32				index; 
} bt_hdp_disconnect_req_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	U8 				result;
	bt_addr_struct 	bdaddr;
	U16 			mdlId;
	U32				index;
} bt_hdp_disconnect_cnf_struct;

/*Force to disconnect L2CAP connection*/
typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U16				mdlId;
} bt_hdp_remove_connection_req_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	U8 				result;
	bt_addr_struct 	bdaddr;
} bt_hdp_remove_connection_cnf_struct;


typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U8 				mdepId;
	U16				mdlId;
	BOOL			mainChannel;
} bt_hdp_channel_opened_ind_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U16 			mdlId;
} bt_hdp_channel_closed_ind_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
} bt_hdp_get_main_channel_req_struct;


typedef struct 
{
	LOCAL_PARA_HDR
	U8 result;
	bt_addr_struct 	bdaddr;
	U16 mdlId;
} bt_hdp_get_main_channel_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U16				mdlId;
} bt_hdp_get_instance_req_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	U8 result;
	U8 mdepId;
} bt_hdp_get_instance_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U16 mdlId;
} bt_hdp_get_l2cap_channel_req_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	U8 result;
	bt_addr_struct 	bdaddr;
	U16 mdlId;
	U16 l2capId;
} bt_hdp_get_l2cap_channel_cnf_struct;

typedef struct 
{
	LOCAL_PARA_HDR
	bt_addr_struct 	bdaddr;
	U8				role;
} bt_hdp_echo_req_struct;
typedef struct 
{
	LOCAL_PARA_HDR
	U8 result;
	bt_addr_struct 	bdaddr;
	U8				role;
} bt_hdp_echo_cnf_struct;

typedef bt_hdp_remove_connection_req_struct bt_hdp_reopen_connection_req_struct;


#endif
