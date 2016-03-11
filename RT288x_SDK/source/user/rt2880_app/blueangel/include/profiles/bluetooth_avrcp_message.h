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
 * Bt_avrcp_message.h
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
#ifndef __BT_AVRCP_MESSAGE_H__
#define __BT_AVRCP_MESSAGE_H__

#ifdef SOURCE_INSIGHT_TRACE
enum 
{
#endif	
   /*AVRCP*/   
#ifdef BTMTK_ON_WISE    
    MSG_ID_BT_AVRCP_ACTIVATE_REQ,
    MSG_ID_BT_AVRCP_GROUP_START = MSG_ID_BT_AVRCP_ACTIVATE_REQ,
#else
    MSG_ID_BT_AVRCP_ACTIVATE_REQ = MSG_ID_BT_AVRCP_GROUP_START,
#endif /* BTMTK_ON_WISE */                       
   MSG_ID_BT_AVRCP_ACTIVATE_CNF,
   MSG_ID_BT_AVRCP_DEACTIVATE_REQ,
   MSG_ID_BT_AVRCP_DEACTIVATE_CNF,
   MSG_ID_BT_AVRCP_CONNECT_REQ,
   MSG_ID_BT_AVRCP_CONNECT_CNF,
   MSG_ID_BT_AVRCP_CONNECT_IND,
   MSG_ID_BT_AVRCP_CONNECT_IND_RSP,
   MSG_ID_BT_AVRCP_BROWSE_CONNECT_REQ,
   MSG_ID_BT_AVRCP_BROWSE_CONNECT_CNF,
   MSG_ID_BT_AVRCP_BROWSE_CONNECT_IND,
   MSG_ID_BT_AVRCP_DISCONNECT_REQ,
   MSG_ID_BT_AVRCP_DISCONNECT_CNF, 
   MSG_ID_BT_AVRCP_DISCONNECT_IND,
   MSG_ID_BT_AVRCP_CONNECT_ACCEPT_RSP,
   MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_REQ,
   MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_IND,
   MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_CNF,
   MSG_ID_BT_AVRCP_CMD_FRAME_REQ,
   MSG_ID_BT_AVRCP_CMD_FRAME_CNF,
   MSG_ID_BT_AVRCP_CMD_FRAME_IND,
   MSG_ID_BT_AVRCP_CMD_FRAME_IND_RSP,
   MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND, /* TARGET ROLE */
   MSG_ID_BT_AVRCP_GET_CAPABILITIES_RSP,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_RSP,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_RSP,
   MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND,
   MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_RSP,
   MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND,
   MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_RSP,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_RSP,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_RSP,
   MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND,
   MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_RSP,
   MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND,
   MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_RSP,
   MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND,
   MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_RSP,
   MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND,
   MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_RSP,
   MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_IND,
   MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP,
   MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_IND,
   MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_RSP,   
   MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_IND,
   MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_RSP,
   MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_IND,
   MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_RSP,
   MSG_ID_BT_AVRCP_GET_PLAYERITEMS_IND,
   MSG_ID_BT_AVRCP_GET_PLAYERITEMS_RSP,   
   MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND,
   MSG_ID_BT_AVRCP_GET_FOLDERITEMS_RSP,
   MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_IND,
   MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_RSP,
   MSG_ID_BT_AVRCP_CHANGE_PATH_IND,
   MSG_ID_BT_AVRCP_CHANGE_PATH_RSP,
   MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_IND,
   MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_RSP,
   MSG_ID_BT_AVRCP_PLAY_ITEMS_IND,
   MSG_ID_BT_AVRCP_PLAY_ITEMS_RSP,
   MSG_ID_BT_AVRCP_SEARCH_IND,
   MSG_ID_BT_AVRCP_SEARCH_RSP,
   MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_IND,
   MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_RSP,
   MSG_ID_BT_AVRCP_GET_CAPABILITIES_REQ, /* CONTROLLER ROLE */
   MSG_ID_BT_AVRCP_GET_CAPABILITIES_CNF,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_REQ,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_CNF,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_REQ,
   MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_CNF,
   MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_REQ,
   MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_CNF,
   MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_REQ,
   MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_CNF,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_REQ,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_CNF,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_REQ,
   MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_CNF,
   MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_REQ,
   MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_CNF,
   MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_REQ,
   MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_CNF,
   MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_REQ,
   MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF,
   MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_REQ,
   MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_CNF,
   MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_REQ,
   MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_CNF,
   MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_REQ,
   MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_CNF,   
   MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_REQ,
   MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF,
   MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_REQ,
   MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_CNF,
   MSG_ID_BT_AVRCP_GET_PLAYERITEMS_REQ,
   MSG_ID_BT_AVRCP_GET_PLAYERITEMS_CNF,   
   MSG_ID_BT_AVRCP_GET_FOLDERITEMS_REQ,
   MSG_ID_BT_AVRCP_GET_FOLDERITEMS_CNF,
   MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_REQ,
   MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_CNF,
   MSG_ID_BT_AVRCP_CHANGE_PATH_REQ,
   MSG_ID_BT_AVRCP_CHANGE_PATH_CNF,
   MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_REQ,
   MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_CNF,
   MSG_ID_BT_AVRCP_PLAY_ITEMS_REQ,
   MSG_ID_BT_AVRCP_PLAY_ITEMS_CNF,
   MSG_ID_BT_AVRCP_SEARCH_REQ,
   MSG_ID_BT_AVRCP_SEARCH_CNF,
   MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_REQ,
   MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_CNF,
   MSG_ID_BT_AVRCP_GROUP_END = MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_CNF,
#ifdef SOURCE_INSIGHT_TRACE
};
#endif
#endif