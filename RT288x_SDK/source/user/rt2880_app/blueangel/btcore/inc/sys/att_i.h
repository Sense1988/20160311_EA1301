/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
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

/*******************************************************************************
 *
 * Filename:
 * ---------
 * att_i.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Attribute Protocol internal header
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef __ATT_I_H
#define __ATT_I_H

#include "me_adp.h"
#include "sec_adp.h"
#include "l2cap_adp.h"

#define MAX_ATT_CON_NO  NUM_BT_DEVICES
#define MAX_ATT_TX_PACKET_NO    7
/* One Request, One Response, Two Command,  Two Notification, One Confirmation */

#define MAX_PREPARE_WRITE_NO 3
#define MAX_ATT_RX_MTU_BREDR 48
#define MAX_ATT_RX_MTU_BLE   23

#define MAX_ATT_TX_MTU 60
/* For LE, ATT_MTU 23, For BR/EDR is 48, We use 60, not large or small */

#define MAX_ATT_TX_TIMEOUT 300000   
/* 30 seconds for ATT transaction timeout*/

#define ATT_CONN_DISCONNECTED       0
#define ATT_CONN_CONNECTED          1


#define ATT_TRANSACTION_NONE        0
#define ATT_TRANSACTION_ONGOING     1



typedef struct _BtAttEvent
{
    U8 eType;     
    BtRemoteDevice *link;
    void *parms;
}BtAttEvent;

typedef void (*GattCallBack) (BtAttEvent *);

typedef struct _BtAttPrepareWrite
{
    U8 available;
    U16 handle;
    U16 offset;
    U16 length;
    U8 writeBuffer[MAX_ATT_TX_MTU-5];
} BtAttPrepareWrite;


typedef struct _BtAttConn
{
    U8  state;
    BtRemoteDevice *link;
    BtPacket pkt[MAX_ATT_TX_PACKET_NO];
    U8 txBuffer[MAX_ATT_TX_PACKET_NO][MAX_ATT_TX_MTU];
    ListEntry freePacketList;
    U16 mtu;
    U16 l2cap_id;
    U16 local_desired_mtu;
    EvmTimer txWaitTimer;
    EvmTimer rxWaitTimer;    
    U8 tx_transaction_ongoing;
    U8 rx_transaction_ongoing;
    BtAttPrepareWrite prepareWriteBuffer[MAX_PREPARE_WRITE_NO];
    BtSecurityToken secToken;
    BtSecurityRecord secRecord;
} BtAttConn;


typedef struct _BtAttContext
{
    BtAttConn conn[MAX_ATT_CON_NO];
    BtAttEvent att_event;
    GattCallBack callback;
    U8 tempBuffer[MAX_ATT_TX_MTU];
    L2capPsm psm;
} BtAttContext;





#endif /* __ATT_I_H */

