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

 * bluetooth_hf_struct.h

 *

 * Project:

 * --------

 * BT Project

 *

 * Description:

 * ------------

 * This file is used to define struct of local parameter for hf adp sap

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

#ifndef __BLUETOOTH_HF_STRUCT_H__

#define __BLUETOOTH_HF_STRUCT_H__



#include "bttypes.h"

#include "bt_message.h"

#if defined(BTMTK_ON_LINUX) && !defined(GEN_FOR_PC)

#include <sys/socket.h>

#include <sys/un.h>

#endif 



/***************************************************************************** 

* Definations

*****************************************************************************/

#define HF_CONNECT_GUARD_TIMER         20000    /* 20 sec */

#define HF_SCO_CONNECT_GUARD_TIMER     5000000  /* 5 sec */



/* AT settings */

#define HF_MAX_AT_STRING_LEN           (RF_MAX_FRAME_SIZE)



/***************************************************************************** 

* Structure

*****************************************************************************/

typedef enum

{

    BT_HF_CONNECT_ACCEPT_CNF_ACCEPTED = 0,

    BT_HF_CONNECT_ACCEPT_CNF_REJECTED,

    BT_HF_CONNECT_ACCEPT_CNF_TOTAL

} bt_hf_connect_accept_confirm_enum;



typedef enum

{

	BT_HF_RESULT_OK = 0,

	BT_HF_RESULT_FAIL,

	BT_HF_RESULT_CHIP_REASON,

	BT_HF_RESULT_TIMEOUT,

	BT_HF_RESULT_MED_ERROR /* Error cause by MED state error, not BT */

} bt_hf_result_enum;



typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_header_struct;



typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

    U16             result;

} bt_hf_general_cnf_struct;



/* MSG_ID_BT_HF_ACTIVATE_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext; /* not used, just for sync with other structures */

    void            *pReqContext;    

    kal_bool        bHeadset;

} bt_hf_activate_req_struct;



/* MSG_ID_BT_HF_ACTIVATE_CNF */

typedef bt_hf_general_cnf_struct bt_hf_activate_cnf_struct;



/* MSG_ID_BT_HF_DEACTIVATE_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_deactivate_req_struct;



/* MSG_ID_BT_HF_DEACTIVATE_CNF */

typedef bt_hf_general_cnf_struct bt_hf_deactivate_cnf_struct;



/* MSG_ID_BT_HF_CONNECT_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

    BD_ADDR         bt_addr;

} bt_hf_connect_req_struct;



/* MSG_ID_BT_HF_CONNECT_CNF */

typedef bt_hf_general_cnf_struct bt_hf_connect_cnf_struct;



/* MSG_ID_BT_HF_DISCONNECT_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_disconnect_req_struct;



/* MSG_ID_BT_HF_DISCONNECT_CNF */

typedef bt_hf_general_cnf_struct bt_hf_disconnect_cnf_struct;



/* MSG_ID_BT_HF_SCO_CONNECT_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_sco_connect_req_struct;



/* MSG_ID_BT_HF_SCO_CONNECT_CNF */

typedef bt_hf_general_cnf_struct bt_hf_sco_connect_cnf_struct;



/* MSG_ID_BT_HF_SCO_DISCONNECT_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_sco_disconnect_req_struct;



/* MSG_ID_BT_HF_SCO_DISCONNECT_CNF */

typedef bt_hf_general_cnf_struct bt_hf_sco_disconnect_cnf_struct;



/* MSG_ID_BT_HF_ACCEPT_CHANNEL_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_accept_channel_req_struct;



/* MSG_ID_BT_HF_ACCEPT_CHANNEL_CNF */

typedef bt_hf_general_cnf_struct bt_hf_accept_channel_cnf_struct;



/* MSG_ID_BT_HF_REJECT_CHANNEL_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

} bt_hf_reject_channel_req_struct;



/* MSG_ID_BT_HF_REJECT_CHANNEL_CNF */

typedef bt_hf_general_cnf_struct bt_hf_reject_channel_cnf_struct;



/* MSG_ID_BT_HF_SEND_DATA_REQ */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pReqContext;

    U16              size;     

    char             data[HF_MAX_AT_STRING_LEN];      

} bt_hf_send_data_req_struct;



/* MSG_ID_BT_HF_SEND_DATA_CNF */

typedef bt_hf_general_cnf_struct bt_hf_send_data_cnf_struct;



/* MSG_ID_BT_HF_CONNECT_REQ_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;   

    BD_ADDR          bt_addr;

} bt_hf_connect_req_ind_struct;



/* MSG_ID_BT_HF_CONNECTED_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;    

    BD_ADDR          bt_addr;

} bt_hf_connected_ind_struct;



/* MSG_ID_BT_HF_DISCONNECTED_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;   

    BD_ADDR          bt_addr;

} bt_hf_disconnected_ind_struct;



/* MSG_ID_BT_HF_SCO_CONNECTED_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;   

    U16		        status;

} bt_hf_sco_connected_ind_struct;



/* MSG_ID_BT_HF_SCO_DISCONNECTED_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;   

} bt_hf_sco_disconnected_ind_struct;



/* MSG_ID_BT_HF_DATA_IND */

typedef struct

{

    LOCAL_PARA_HDR

    void            *pContext;

    void            *pUserContext;

    U16              size;     

    char             data[HF_MAX_AT_STRING_LEN];      

} bt_hf_data_ind_struct;



#endif /* __BLUETOOTH_HF_STRUCT_H__ */

