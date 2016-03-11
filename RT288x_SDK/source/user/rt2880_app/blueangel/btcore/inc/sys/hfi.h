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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfi.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #2 $
 *
 * Description: This file contains internal definitions for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2001-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __HFI_H__
#define __HFI_H__

#include "hf.h"

/* State machine function prototype */                     
typedef void (*HfState)(HfChannel_struct *pChannel, RfCallbackParms *pParms);

/* RFCOMM Credit */  
#define HF_RF_CREDIT                        0x01

/* Link Flags */
#define HF_LINK_FLAG_ACL                    0x01 /* Indicate if ACL is created and associated. */
#define HF_LINK_FLAG_SCO_CONNECTING         0x02 /* Indicate if SCO creation is ongoing.       */
#define HF_LINK_FLAG_SCO_DISCONNECTING      0x04 /* Indicate if SCO removal is ongoing.        */
#define HF_LINK_FLAG_CONN_INCOMING          0x08 /* Indicate if connect request is received.   */

#define HF_SDP_QUERY_FLAG_SVCCLASSID_LIST   0x01
#define HF_SDP_QUERY_FLAG_PROTOCOL          0x02
#define HF_SDP_QUERY_FLAG_PROFILE           0x04
#define HF_SDP_QUERY_FLAG_NETWORK           0x08
#define HF_SDP_QUERY_FLAG_FEATURES          0x10
#define HF_SDP_QUERY_FLAG_ONGOING           0x80   

/* DESC : Combination of all attributes queried by HS. Used to check if all 
 *            attributes are returned.
 */
#define HF_SDP_QUERY_ALL_HS_FLAGS		   (HF_SDP_QUERY_FLAG_SVCCLASSID_LIST |\
                                            HF_SDP_QUERY_FLAG_PROTOCOL)

/* DESC : Combination of all attributes queried by HF. Used to check if all 
 *            attributes are returned.
 */
#define HF_SDP_QUERY_ALL_HF_FLAGS		   (HF_SDP_QUERY_FLAG_SVCCLASSID_LIST |\
                                            HF_SDP_QUERY_FLAG_PROTOCOL |\
                                            HF_SDP_QUERY_FLAG_NETWORK |\
                                            HF_SDP_QUERY_FLAG_FEATURES)

#define HfGetState(pChannel)		       (pChannel->state)
#define HfGetLinkFlags(pChannel)	       (pChannel->linkFlags)
#define HfGetQueryFlags(pChannel)	       (pChannel->queryFlags)

#define HfSetState(pChannel, newState)     (pChannel->state       = newState)
#define HfSetLinkFlags(pChannel, flags)    (pChannel->linkFlags  |= flags)
#define HfSetQueryFlags(pChannel, flags)   (pChannel->queryFlags |= flags)

#define HfUnsetLinkFlags(pChannel, flags)  (pChannel->linkFlags  &= ~flags)
#define HfUnsetQueryFlags(pChannel, flags) (pChannel->queryFlags &= ~flags)

#define HfClearLinkFlags(pChannel)         (pChannel->linkFlags   = 0)
#define HfClearQueryFlags(pChannel)        (pChannel->queryFlags  = 0)

/* General utility functions */
void HfAppCallback(HfChannel_struct *pChannel, HfEvent event, BtStatus status, U32 data);
RfChannel *HfAllocRfChannel(void);
void HfFreeRfChannel(RfChannel *pRfChannel);
BtStatus HfRegisterRfServerChannel(HfChannel_struct *pChannel);
BtStatus HfDeregisterRfServerChannel(HfChannel_struct *pChannel);
BtStatus HfOpenCmgrChannel(HfChannel_struct *pChannel, BD_ADDR *pAddr);
BtStatus HfCloseCmgrChannel(HfChannel_struct *pChannel);
HfChannel_struct *HfFindRegisteredChannel(RfChannel *pRfChannel);
#if defined(__HF_BLOCK_SIMULTANEOUS_CONNECTION__)
void HfSuspendOtherContexts(HfChannel_struct *pChannel);
void HfResumeOtherContexts(HfChannel_struct *pChannel);
#endif
void HfChangeState(HfChannel_struct *pChannel, U8 newState);
void HfCloseChannel(HfChannel_struct *pChannel);
U16 HfRemainTxData(HfChannel_struct *pChannel);
U16 HfEmptyTxSize(HfChannel_struct *pChannel);
void HfResetTxBuffer(HfChannel_struct *pChannel);
BtStatus HfSendTxBuffer(HfChannel_struct *pChannel);
U16 HfWriteTxBuffer(HfChannel_struct *pChannel, U8 *pBuf, U16 len);

/* SDP utility functions */
BtStatus HfSdpRegisterService(BOOL bHeadset);
BtStatus HfSdpDeregisterService(BOOL bHeadset);
BtStatus HfSdpStartQuery(HfChannel_struct *pChannel, SdpQueryMode mode);

/* State Machine functions */
void HfInitStateMachine(void);
void HfCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
void HfRfCallback(RfChannel *pRfChannel, RfCallbackParms *pParms);

#endif /* __HFI_H__ */

