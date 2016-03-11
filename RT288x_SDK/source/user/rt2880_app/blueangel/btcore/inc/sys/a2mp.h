#ifndef __A2MP_H
#define __A2MP_H

#include "btconfig.h"
#include "a2mp.h"
#include "l2cap.h"
#include "eventmgr.h"

#if A2MP_PROTOCOL == XA_ENABLED

#define A2MP_CODE_COMMAND_REJECT	0x01
#define A2MP_CODE_DISCOVER_REQ		0x02
#define A2MP_CODE_DISCOVER_RSP		0x03
#define A2MP_CODE_CHANGE_NOTIFY		0x04
#define A2MP_CODE_CHANGE_RSP		0x05
#define A2MP_CODE_GET_INFO_REQ		0x06
#define A2MP_CODE_GET_INFO_RSP		0x07
#define A2MP_CODE_GET_AMP_ASSOC_REQ		0x08
#define A2MP_CODE_GET_AMP_ASSOC_RSP		0x09
#define A2MP_CODE_CREATE_PHY_LINK_REQ	0x0A
#define A2MP_CODE_CREATE_PHY_LINK_RSP	0x0B
#define A2MP_CODE_DISC_PHY_LINK_REQ		0x0C
#define A2MP_CODE_DISC_PHY_LINK_RSP		0x0D
#define A2MP_CODE_INVALID               0xFF


#define A2MP_STATE_IDLE				0x00
#define A2MP_STATE_ALLOCATED		0x01

#define A2MP_CHANNEL_MAX_TX_PACKET	0x03

typedef struct _A2MP_CONN
{
    U8 state;
    U16 psm;
    L2capChannelId l2ChannelId; /* L2CAP channel ID              */
    BtRemoteDevice *remDev;
    U8 waitPhysicalLinkCompleted;
} A2MP_CONN;

typedef struct _BtA2MPPacket
{
    ListEntry node; /* Used internally by the stack. */
    BtPacket btPacket;
    U8 data_buff[A2MP_MAX_TX_BUFFER];
}BtA2MPPacket;

typedef struct _A2MP_MAIN_CONN
{
    U8 state;
    U8 localIdent;
    U8 remoteIdent;
    U8 last_opcode;
    U8 last_rxCmdopcode;    
    U8 localControllerId;
    U8 ampNeedToCreatePhysicalLink;
    U8 stopCreatePhysicalLink;
    U8 ampPhysicalLinkCreating;
    U8 discoverDone;
    U8 waitAmpPhysicalLinkResponse;
    U8 physicalLinkHandler;
    U16 tx_mtu;
    U16 localCid;
    BtRemoteDevice *remDev;    
    EvmTimer timer;
    A2MP_CONN a2mp_channel[NUM_A2MP_SERVERS];
    ListEntry rxPacketsBufferList;
    ListEntry receivedPacketsList;
    BtRxPacket RxPackets[A2MP_NUM_RETRANSMISSION_WINDOW_SIZE];
    U8 l2capRawData[A2MP_NUM_RETRANSMISSION_WINDOW_SIZE][A2MP_MAX_FRAME_SIZE+20];
    U8 txMaxCountPerIFrame[128];
    U32 txSrejList[A2MP_L2CAP_MAX_SREJ_SIZE];
    BtA2MPPacket a2mp_txPacket[A2MP_CHANNEL_MAX_TX_PACKET];
    ListEntry freeTtxPacketsList;
    U8 local_amp_assoc[BT_MAX_AMP_ASSOC_SIZE];
    U16 local_amp_assoc_size;
    U8 remote_amp_assoc[BT_MAX_AMP_ASSOC_SIZE];
    U16 remote_amp_assoc_size;
    
} A2MP_MAIN_CONN;



typedef struct _BtA2mpContext
{
    L2capPsm psm;               /* L2CAP PSM                     */
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
	BtSecurityRecord secRecord;
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
	A2MP_MAIN_CONN a2mp_main_channel[NUM_BT_DEVICES];
	BtHandler a2mpMeHandler;
	U8 nextIdent;
    U8 controllerListSize;
    U8 *controllerList;
} BtA2mpContext;

#define A2MP_DISCOVERY_RESULT_ERROR 1
#define A2MP_GETINFO_RESULT_ERROR 2
#define A2MP_CREATE_CHANNEL_RSP_ERROR 3
#define A2MP_GETAMPASSOC_RESULT_ERROR 4
#define A2MP_PHYSICAL_LINK_CREATE_FAILED_ERROR 5

#endif /* A2MP_PROTOCOL == XA_ENABLED */
#endif /* __A2MP_H */ 

