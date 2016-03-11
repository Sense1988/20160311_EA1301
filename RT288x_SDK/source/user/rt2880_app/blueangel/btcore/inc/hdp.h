/*******************************************************************************
 *
 * Filename:
 * ---------
 *   hdp.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the  Health Device Profile (MAP).
 *
 * Author:
 * -------
 *   Adam Chen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifndef __HDP_H__
#define __HDP_H__

#ifndef __BT_HDP_PROFILE__


#include <mcap.h>
#include <conmgr.h>

#define HDP_PROFILE 1

#define HDP_SDP_DEFAULT_PSM 	0x0001
#define HDP_SDP_MCAP_VERSION 	0x0100
#define HDP_SDP_HDP_VERSION 	0x0100
#define HDP_SDP_IEEE_11073		0x01
#define HDP_SDP_MCAP_DEFAULT_FEATURES 	MCAP_RECONNECT_ACCEPT  //8 bit, default,  no reconnect, no CSP 

#define HDP_INSTANCE_MAX_NUM 10
#define HDP_INSTNACE_DESC_MAX_LEN 255
#define HDP_MDEP_ID_ECHO_TEST 0x00
#define HDP_MDEP_ID_MAX_VALUE 0x7F

#define HDP_PACKET_POOL_MAX_VALUE 4

#define HDP_INVALID_MDEP_ID 0xFF //0xFF is reserved by MCAP, so HDP considers it as invalid value
#define HDP_INVALID_L2CAP_MODE 0

#define HDP_ECHO_TEST_PACKET "HDP ECHO TEST PACKET"
#define HDP_ECHO_TEST_PACKET_LEN strlen(HDP_ECHO_TEST_PACKET)


/*************************************************************
*  Definition of current SDP query status (Value of HdpChannel.queryFlags)
**************************************************************/
/* DESC : Indicate if Protocol desciptor is returned through the callback.
 *
 * SET : related desciptor is returned.
 * UNSET : related desciptor is not returned.
 */
typedef enum
{
	HDP_SDP_QUERY_FLAG_PROTOCOL 			= 0x01,
	HDP_SDP_QUERY_FLAG_PROFILE 				= 0x02,
	HDP_SDP_QUERY_FLAG_ADDITIONAL_PROTOCAL 	= 0x04,
	HDP_SDP_QUERY_FLAG_HDP_FEATURES 		= 0x08,
	HDP_SDP_QUERY_FLAG_MCAP_PROC 			= 0x10,
	HDP_SDP_QUERY_OUTGOING					= 0x80,
} HDPSdpQueryFlag;

/*used for MCAP supported procedure in SDP record*/
typedef enum
{
	MCAP_RECONNECT_INITIATE	= 0x02,
	MCAP_RECONNECT_ACCEPT	= 0x04,
	MCAP_CSP				= 0x08,
	MCAP_SYNC_MASTER		= 0x10,
};
typedef U8 MCAPSupProc;

typedef U16 MdepId;

/*used for supported features in SDP record*/
typedef enum
{
	HDP_ROLE_SOURCE = 0x00,
	HDP_ROLE_SINK 	= 0x01,
	HDP_ROLE_RESERVED
};
typedef U8 HdpRole;

typedef enum 
{
	HDP_CHANNEL_NO_PREFERENCE	= 0x00,
	HDP_CHANNEL_RELIABLE		= 0x01,
	HDP_CHANNEL_STREAMING		= 0x02,	
};
typedef U8 HdpChannelType;

typedef enum 
{
	HDP_STATE_IDLE,
	HDP_STATE_CONNECTING,
	HDP_STATE_SERVICE_SEARCH,
	HDP_STATE_MCL_CONNECT,
	HDP_STATE_MDL_CONFIG,
	HDP_STATE_MDL_CONNECT,
	HDP_STATE_MDL_DISCONNECT,	
};
typedef U8 HdpState;

typedef enum 
{
	HDP_CHANNEL_STATE_IDLE,
	HDP_CHANNEL_STATE_CONNECTING,
	HDP_CHANNEL_STATE_CONNECTED,
	HDP_CHANNEL_STATE_DISCONNECTING,	
};
typedef U8 HdpChannelState;


typedef enum
{
	HDP_PKT_IDLE,
	HDP_PKT_IN_USE,
};
typedef U8 HdpPktState;

typedef enum
{
	HDP_DATA_PKT_READY,
	HDP_DATA_PKT_PROCESSING,
};
typedef U8 HdpDataPktState;

typedef enum
{
	HDP_PKT_MCAP_TO_HDP,
	HDP_PKT_HDP_TO_MCAP,
};
typedef U8 HdpPktDirection;


typedef enum 
{
	HDP_OPERATION_IDLE,
	HDP_OPERATION_CONNECT,	
	HDP_OPERATION_DISCONNECT,
};
typedef U8 HdpOperation;


typedef enum
{	
	HDP_EVENT_CONNECT_CNF,
	HDP_EVENT_DISCONNECT_CNF,
	HDP_EVENT_CHANNEL_OPENED,
	HDP_EVENT_CHANNEL_CLOSED,	
	HDP_EVENT_DATA_RCV_IND,	
	HDP_EVENT_CONNECTION_REMOVED,	
	HDP_EVENT_ECHO_CNF,	
	HDP_EVENT_RANGE,	
} _HdpEvent;

typedef U8 HdpEvent; 
typedef struct
{
    HdpEvent	  event;
	BtStatus      result; 

	union {
		/*connected or disconnected*/
		/*connected: search service succeed or MCL connected*/
		/*disconnected: send delete mdl message in MCL connection, or mcl is deleted*/
		struct {
			MdepId		mdepId;
			BD_ADDR 	addr;
			U16 		mdlId;
		} connection;

		/*mdl opened or closed*/
		struct {
			MdepId		mdepId;
			BD_ADDR 	addr;
			U16 		mdlId;
			U16 		l2capId;			
			BOOL 		delFlag; //decide the channel should be deleted or not
		} channel;
		
		struct {
			U16		l2capChnId;
   			void*  	value;
			U32	   	size;
		} data;
    } content;
	
} HdpCallbackParm;

typedef void (*HdpCallback)(HdpCallbackParm *parms);

typedef struct
{
	ListEntry			node;
	BD_ADDR				bdAddr;
	HdpOperation		operation;
} HdpPendingTask;




/* packet in packet pool*/
/*Notes: */
/* 1. HdpDataPacket release point: when getting a hdp packet from cache list*/
/* 2. McapDataPacket release point: when mcap has handled the packet*/
typedef struct
{	
	ListEntry 		node;
	MdepId			instanceId;
	U16 			channelId; 
	McapDataPacket	data;
	
} HdpDataPacket;

//l2cap packet container
typedef struct
{	
	ListEntry 		node;
    HdpPktState 	state;
    BtPacket    	btPkt;
	
} HdpPkt; 

typedef struct
{
	U16 			datatype;
	HdpRole 		role;
	HdpChannelType 	channelType;
	char 			description[HDP_INSTNACE_DESC_MAX_LEN];
} HdpConfig;


/*used for SDP registration*/
typedef struct
{
	U16						mdepId;
	U16 					dataType;
	HdpRole					role;
	char					descriotion[HDP_INSTNACE_DESC_MAX_LEN];
} HdpFeature;


typedef struct
{
	BOOL 					registered;
	HdpFeature				feature;
	HdpChannelType			channelType;
} HdpInstance;


typedef struct
{
	U16						mdepId;
	L2capPsmValue			ctrChnPsm;
	L2capPsmValue			dataChnPsm;
//	ListEntry				feature;		// list of HdpFeature
	U16						mcapVersion;
	U16						hdpVersion;
	U8						mcapProc;
} HdpSdpRecord;

typedef struct
{
	ListEntry				node;
	HdpChannelState			state;
	HdpInstance *			instance;			//reference to HDP Instance
	HdpChannelType			type;
	BD_ADDR					bdAddr;

	BOOL 					launchEcho; 		//flag to indicate local device launch to echo test

	BOOL					reopening;
	
	CmgrHandler				cmgrHdlr;
	MdlL2Configuration		config;
	
	McapMdl*   				mcapmdl;
	U16						mtu;                 //the hdp mtu = L2CAP mtu - BT_PACKET_HEADER_LEN

	void *					data;
	U32						size;

	/*SDP parms*/
	SdpQueryToken			sdpQueryToken;
	U8						queryFlags; 	//see HDPSdpQueryFlag
	U16						rmUuid;
	HdpSdpRecord			rmSdpRecord;
} HdpChannel;

/**/
typedef struct
{
	ListEntry  				node;
	HdpChannelState			state;
	BD_ADDR    				bdAddr;	
	McapMcl*   				mcapMcl;
} HdpMcl;

typedef struct 
{
	BOOL			initialized;
	HdpInstance		instance[HDP_INSTANCE_MAX_NUM];
	
	ListEntry		mcl;      				/*store HdpMcl object, Not McapMcll*/ 
	ListEntry		channel;      			/*store HdpChannel object, Not McapMdll*/ 

	/*local psm assigned by MCAP*/
	U16  			mclPsm;	
	U16				mdlPsm;	

	HdpCallback		callback;
	
	ListEntry		dataPkt; //cache for user data, notes: FIFO(first in first out)
	ListEntry		pktPool; // used for L2CAP packet pool
	
} HdpContext;

BtStatus hdp_init();
BtStatus hdp_deinit();
MdepId hdp_register(HdpConfig *config);
BtStatus hdp_deregister(MdepId mdepid);
BtStatus hdp_open_channel(MdepId id, BD_ADDR *bdaddr, HdpChannelType type);
BtStatus hdp_reopen_channel(BD_ADDR *bdaddr, U16 mdlId);
BtStatus hdp_close_channel(BD_ADDR *bdaddr, U16 mdlId);
BtStatus hdp_remove_connection(BD_ADDR *bdaddr);
BtStatus hdp_echo(HdpRole role, BD_ADDR *bdaddr);
BtStatus hdp_send_data(U16 l2capId, void* data, U32 len);

#endif //__BT_HDP_PROFILE__

#endif

