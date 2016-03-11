/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bluetooth_pan_message.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to define message for communication between BT task and external task.
 *
 * Author:
 * -------
 * Ting Zheng
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
#ifndef __BT_PAN_MESSAGE_H__
#define __BT_PAN_MESSAGE_H__

    /*APP --> PAN*/
    MSG_ID_BT_PAN_ACTIVATE_REQ = MSG_ID_BT_PAN_GROUP_START,
    MSG_ID_BT_PAN_DEACTIVATE_REQ,
    MSG_ID_BT_PAN_CONNECT_REQ,
    MSG_ID_BT_PAN_DISCONNECT_REQ,
    MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_RSP,
    MSG_ID_BT_PAN_SET_NETTYPE_FILETER_REQ,
    MSG_ID_BT_PAN_SET_MULTIADDR_FILETER_REQ,
    MSG_ID_BT_PAN_SEND_PACKET_REQ,
    MSG_ID_BT_PAN_SEND_PENDING_PACKET_REQ,
    MSG_ID_BT_PAN_PTS_TEST_SEND_PACKET_REQ,

    MSG_ID_BT_PAN_CMD,	// used if cmd agent is necessary

    /*APP <-- PAN*/
    MSG_ID_BT_PAN_ACTIVATE_CNF,
    MSG_ID_BT_PAN_DEACTIVATE_CNF,
    MSG_ID_BT_PAN_CONNECT_CNF,
    MSG_ID_BT_PAN_DISCONNECT_CNF,
    MSG_ID_BT_PAN_SET_NETTYPE_FILETER_CNF,
    MSG_ID_BT_PAN_SET_MULTIADDR_FILETER_CNF,
    MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_IND,
    MSG_ID_BT_PAN_CONNECT_IND,
    MSG_ID_BT_PAN_DISCONNECT_IND,
    MSG_ID_BT_PAN_SET_NETTYPE_FILTER_IND,
    MSG_ID_BT_PAN_SET_MULTIADDR_FILTER_IND,
    MSG_ID_BT_PAN_GROUP_END = MSG_ID_BT_PAN_SET_MULTIADDR_FILTER_IND,

#endif
