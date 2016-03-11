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
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
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
 * Bt_a2dp_message.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Tina Shen
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
#ifndef __BT_A2DP_MESSAGE_H__
#define __BT_A2DP_MESSAGE_H__

#ifdef SOURCE_INSIGHT_TRACE
typedef enum 
{
#endif
   /*A2DP*/
#ifdef BTMTK_ON_WISE    
    MSG_ID_BT_A2DP_ACTIVATE_REQ,
    MSG_ID_BT_A2DP_GROUP_START = MSG_ID_BT_A2DP_ACTIVATE_REQ,
#else
    MSG_ID_BT_A2DP_ACTIVATE_REQ = MSG_ID_BT_A2DP_GROUP_START,
#endif /* BTMTK_ON_WISE */                       
   MSG_ID_BT_A2DP_ACTIVATE_CNF,
   MSG_ID_BT_A2DP_DEACTIVATE_REQ,
   MSG_ID_BT_A2DP_DEACTIVATE_CNF,

#ifdef __PRJ_MT7650_SONY_BDP__
   MSG_ID_BT_A2DP_AAC_ENABLE_REQ,
   MSG_ID_BT_A2DP_AAC_ENABLE_CNF,
   MSG_ID_BT_SECOND_STREAM_IND,
   MSG_ID_BT_AVDTP_CONNECTED_IND,
   MSG_ID_BT_AVDTP_DISCONNECTED_IND,
   MSG_ID_BT_A2DP_SET_LOCAL_ROLE_REQ,
#endif
  
   MSG_ID_BT_A2DP_APPI_BT_COMMAND,
   MSG_ID_BT_A2DP_APPI_BT_CONNECT_ACCEPT_RSP,
   MSG_ID_BT_A2DP_APPI_BT_SDK_PAUSE_REQ,
   MSG_ID_BT_A2DP_APPI_BT_SDK_RESUME_REQ,   
   
   MSG_ID_BT_MEDIA_A2DP_CODEC_OPEN_CNF,
   MSG_ID_BT_MEDIA_A2DP_CODEC_CLOSE_CNF,   
   // for MED

   MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ,
   MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF,
   MSG_ID_BT_A2DP_STREAM_RECONFIG_IND,
   MSG_ID_BT_A2DP_STREAM_RECONFIG_RES,
   MSG_ID_BT_A2DP_STREAM_OPEN_REQ,  //320
   MSG_ID_BT_A2DP_STREAM_OPEN_CNF, 
   MSG_ID_BT_A2DP_STREAM_OPEN_IND,
   MSG_ID_BT_A2DP_STREAM_OPEN_RES,
   MSG_ID_BT_A2DP_STREAM_START_REQ,
   MSG_ID_BT_A2DP_STREAM_START_CNF,
   MSG_ID_BT_A2DP_STREAM_START_IND,
   MSG_ID_BT_A2DP_STREAM_START_RES,  
   MSG_ID_BT_A2DP_STREAM_PAUSE_REQ,
   MSG_ID_BT_A2DP_STREAM_PAUSE_CNF,
   MSG_ID_BT_A2DP_STREAM_PAUSE_IND,
   MSG_ID_BT_A2DP_STREAM_PAUSE_RES,
   MSG_ID_BT_A2DP_STREAM_QOS_IND,
   MSG_ID_BT_A2DP_STREAM_CLOSE_REQ, //333
   MSG_ID_BT_A2DP_STREAM_CLOSE_CNF,
   MSG_ID_BT_A2DP_STREAM_CLOSE_IND,
   MSG_ID_BT_A2DP_STREAM_CLOSE_RES,
   MSG_ID_BT_A2DP_STREAM_ABORT_REQ,
   MSG_ID_BT_A2DP_STREAM_ABORT_CNF,
   MSG_ID_BT_A2DP_STREAM_ABORT_IND,
   MSG_ID_BT_A2DP_STREAM_ABORT_RES,
   MSG_ID_BT_A2DP_STREAM_CONFIG_CNF,
   MSG_ID_BT_A2DP_STREAM_CHANNEL_OPEN_REQ,
   MSG_ID_BT_A2DP_STREAM_DATA_IND,
   MSG_ID_BT_A2DP_GROUP_END = MSG_ID_BT_A2DP_STREAM_DATA_IND,
   MSG_ID_BT_A2DP_STREAM_DATA_OUT = MSG_ID_BT_A2DP_DATA_GROUP_START,
   MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ,
   MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_GET,
   MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_CNF,
   MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_CHANGE,
   MSG_ID_BT_A2DP_STREAM_DATA_START_CNF,
   MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF,
   MSG_ID_BT_A2DP_STREAM_DATA_DISC_IND,
   MSG_ID_BT_A2DP_STREAM_DATA_DISCONNECTING_IND,
   MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED,
   MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED,
   MSG_ID_BT_A2DP_DATA_GROUP_END = MSG_ID_BT_A2DP_STREAM_DATA_DISCONNECTING_IND,
   MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_START_REQ = MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_GROUP_START,//FOR FMOverBt Feature
   MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_STOP_REQ,
   MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_SUSPEND_REQ,
   MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_RESUME_REQ,
   MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_GROUP_END = MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_RESUME_REQ,
#ifdef SOURCE_INSIGHT_TRACE
}A2DP_MSG_TYPE;
#endif

#endif