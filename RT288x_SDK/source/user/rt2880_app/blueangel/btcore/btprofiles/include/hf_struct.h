/* Copyright Statement:

 *

 * This software/firmware and related documentation ("MediaTek Software") are

 * protected under relevant copyright laws. The information contained herein

 * is confidential and proprietary to MediaTek Inc. and/or its licensors.

 * Without the prior written permission of MediaTek inc. and/or its licensors,

 * any reproduction, modification, use or disclosure of MediaTek Software,

 * and information contained herein, in whole or in part, shall be strictly prohibited.

 *

 * MediaTek Inc. (C) 2010. All rights reserved.

 *

 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES

 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")

 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON

 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,

 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF

 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.

 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE

 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR

 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH

 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES

 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES

 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK

 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR

 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND

 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,

 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,

 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO

 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.

 *

 * The following software/firmware and/or related documentation ("MediaTek Software")

 * have been modified by MediaTek Inc. All revisions are subject to any receiver's

 * applicable license agreements with MediaTek Inc.

 */



/*****************************************************************************

 *

 * Filename:

 * ---------

 * hf_struct.h

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file is used to define structure for HFP HF role

 *

 *============================================================================

 *             HISTORY

 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!

 *------------------------------------------------------------------------------

 * $Revision: #1 $

 * $Modtime: $

 * $Log: $

 *------------------------------------------------------------------------------

 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!

 *============================================================================

 ****************************************************************************/

#ifndef __HF_STRUCT_H__

#define __HF_STRUCT_H__



#include "bt_types.h"

#include "rfcomm_adp.h"

#include "conmgr.h"



#define HF_TX_BUFFER_POW                 10

#define HF_TX_BUFFER_SIZE               (1 << 10)

#define HF_TX_BUFFER_MODE               (HF_TX_BUFFER_SIZE - 1)



/*---------------------------------------------------------------------------

 * HF_SECURITY_SETTINGS constant

 *    Defines the security settings for a hands-free connection. The default

 *    is set to BSL_DEFAULT (see BtSecurityLevel).

 */

#ifndef HF_SECURITY_SETTINGS

#define HF_SECURITY_SETTINGS BSL_DEFAULT

#endif                               



/****************************************************************************

 *

 * Types

 *

 ****************************************************************************/

/*---------------------------------------------------------------------------

 * HfEvent type

 *

 *    All indications and confirmations are sent through a callback

 *    function. Depending on the event, different elements of

 *    HfCallbackInfo "parms" union will be valid as described below.

 */

typedef U8 HfEvent;



/** When receive RFEVENT_OPEN_IND, HFP request ADP by sending HF_EVENT_AUTH_REQ

 *  to confirm if it want to accept the connection request.

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.remDev" field 

 *  contains a pointer to the remote device context.

 */

#define HF_EVENT_AUTH_REQ				    0



/** A service level connection has been established. This can happen as the

 *  result of a call to HF_CreateServiceLink, or if the audio gateway device 

 *  establishes the service connection. When this event has been received, a 

 *  data connection is available for sending to the audio gateway device.

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.remDev" field 

 *  contains a pointer to the remote device context.

 */

#define HF_EVENT_SERVICE_CONNECTED          1



/** The service level connection has been released. This can happen as the

 *  result of a call to HF_DisconnectServiceLink, or if the audio gateway device

 *  releases the service connection. Communication with the audio gateway device 

 *  is no longer possible. In order to communicate with the audio gateway device,

 *  a new service level connection must be established.

 *

 *  This event can also occur when an attempt to create a service level 

 *  connection (HF_CreateServiceLink) fails.

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.remDev" field 

 *  contains a pointer to the remote device context. In addition, the

 *  "HfCallbackParms_struct.errCode" fields contains the reason for disconnect.

 */

#define HF_EVENT_SERVICE_DISCONNECTED       2



/** An audio connection has been established. This event occurs whenever the

 *  audio channel (SCO) comes up, whether it is initiated by the audio gateway

 *  or the hands-free unit.

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.remDev" field 

 *  contains a pointer to the remote device context.

 */

#define HF_EVENT_AUDIO_CONNECTED            3



/** An audio connection has been released. This event occurs whenever the

 *  audio channel (SCO) goes down, whether it is terminated by the audio gateway

 *  or the hands-free unit.

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.remDev" field 

 *  contains a pointer to the remote device context. In addition, the

 *  "HfCallbackParms_struct.errCode" fields contains the reason for disconnect.

 */

#define HF_EVENT_AUDIO_DISCONNECTED         4

 

/** An AT result code has been received from the audio gateway. 

 *

 *  When this callback is received, the "HfCallbackParms_struct.p.data" field 

 *  contains the AT result code.

 */

#define HF_EVENT_DATA_IND                   5



/****************************************************************************

 *

 * Data Structures

 *

 ****************************************************************************/

/* Service Type */

typedef enum

{

    HF_SERVICE_TYPE_HS = 0x00,

    HF_SERVICE_TYPE_HF,

    HF_SERVICE_TOTAL

} HfServiceType_enum;



/* Channel States */

typedef enum

{

    HF_STATE_CLOSED = 0x00,

    HF_STATE_CONN_PENDING,

    HF_STATE_DISC_PENDING,

    HF_STATE_OPEN,    

    HF_STATE_TOTAL

} HfState_enum;

 

/*---------------------------------------------------------------------------

 * HfAtResult_struct structure

 *

 * This structure is used to identify an HF AT result code.

 */

typedef struct

{

    U8          *pData;   /* Contains a pointer to at result code */

    U16          dataLen; /* Contains the length of at result code */

} HfAtResult_struct; 



typedef struct _HfChannel_struct HfChannel_struct;



/*---------------------------------------------------------------------------

 * HfCallbackParms_struct structure

 *

 * This structure is sent to the application's callback to notify it of

 * any state changes.

 */

typedef struct

{

    HfEvent      event; 

	U8			 errCode;    

	BtStatus     status;

    union 

    {

        void              *ptr;

        BD_ADDR           *pAddr;

        HfAtResult_struct *pAtResult;

    } p;

} HfCallbackParms_struct;



/*---------------------------------------------------------------------------

 * HfCallback type

 *

 * A function of this type is called to indicate Hands-Free events to

 * the application.

 */

typedef void (*HfCallback)(HfChannel_struct *pChannel, HfCallbackParms_struct *pParms);



/*---------------------------------------------------------------------------

 * HfChannel_struct structure

 *

 * The Hands-Free channel. This structure is used to identify a

 * connection to the audio gateway device.

 */

struct _HfChannel_struct 

{

    /* Registration variables */

    ListEntry              node;  

    HfServiceType_enum     type; 

    HfCallback             callback; 

    RfChannel             *pRfChannel;  

    

    /* Connection State Variables */

    HfState_enum           state;           /* Current connection state */

    U8                     linkFlags;       /* Levels of service connected */



    /* SDP variables for client */

    SdpQueryToken          sdpQueryToken;   /* Used to query the service */

    U8                     queryFlags;      /* Defines which SDP entries were parsed from AG */

    U8                     rfServerChannel; /* When connecting AG */



    /* Channel Resources */

    CmgrHandler            cmgrHandler;

	BD_ADDR		           bdAddr; 

    BOOL                   bTxInProgress;

#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)

    BOOL                   bSuspended;

#endif    

    BtPacket               txPacket;

    XaBufferDesc           txBuffer;

    U8                     txData[HF_TX_BUFFER_SIZE];    

};



#endif /* __HF_STRUCT_H__ */

