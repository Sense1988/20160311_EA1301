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
 * hf.h
 *
 * Project:
 * --------
 * BT Project
 *
 * Description:
 * ------------
 * This file contains API functions for HFP HF role.
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
#ifndef __HF_H__
#define __HF_H__

#include "hf_struct.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
typedef enum 
{
    BT_FILE_HF,			/* 0 */
    BT_FILE_HF_SDP,		/* 1 */        
    BT_FILE_HF_SM,		/* 2 */
    BT_FILE_HF_UTIL	    /* 3 */
}BtFile_enum;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HF_Init()
 *
 *    Initialize the Hands-Free SDK. This function should only be called
 *    once, normally at sytem initialization time. The calling of this 
 *    function can be specified in overide.h using the XA_LOAD_LIST macro
 *    (i.e. #define XA_LOAD_LIST XA_MODULE(HF)).
 *
 * Returns:
 *    TRUE - Initialization was successful
 *
 *    FALSE - Initialization failed.
 */
BOOL HF_Init(void);

/*---------------------------------------------------------------------------
 * HF_Register()
 *
 *    Registers and initializes a channel for use in creating and receiving
 *    service level connections. Registers the Hands-Free profile Hands-free
 *    with SDP. The application callback function is also bound to the channel.
 *
 * Parameters:
 *    pChannel - Contains a pointer to the channel structure that will be
 *        initialized and registered.
 *
 *    callback - The application callback function that will receive events.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *    BT_STATUS_IN_USE - The operation failed because the channel has already
 *        been initialized. 
 *
 *    BT_STATUS_FAILED - The operation failed because either the RFCOMM
 *        channel or the SDP record could not be registered.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_Register(HfChannel_struct *pChannel, HfCallback callback, BOOL bHeadset);

/*---------------------------------------------------------------------------
 * HF_Deregister()
 *
 *    Deregisters the channel. The channel becomes unbound from RFCOMM and
 *    SDP, and can no longer be used for creating service level connections.
 *
 * Parameters:
 *    pChannel - Contains a pointer to the channel structure that will be
 *        deregistered.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_BUSY - The operation failed because a service level connection 
 *        is still open to the audio gateway.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_Deregister(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_CreateServiceLink()
 *
 *    Creates a service level connection with the audio gateway.
 *    This includes performing SDP Queries to find the appropriate service
 *    and opening an RFCOMM channel. The success of the operation is indicated 
 *    by the HF_EVENT_SERVICE_CONNECTED event. If the connection fails, the
 *    application is notified by the HF_EVENT_SERVICE_DISCONNECTED event. 
 * 
 *    If an ACL link does not exist to the audio gateway, one will be 
 *    created first. If desired, however, the ACL link can be established 
 *    prior to calling this function.
 *
 * Parameters:
 *
 *    pChannel - Pointer to a registered channel structure.
 *
 *    pAddr - The Bluetooth address of the remote device.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the connection has been created (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_BUSY - The operation failed because a connection is already
 *        open to the remote device, or a new connection is being created.
 *
 *    BT_STATUS_FAILED - The channel has not been registered.
 *
 *    BT_STATUS_CONNECTION_FAILED - The connection failed.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_CreateServiceLink(HfChannel_struct *pChannel, BD_ADDR *pAddr);

/*---------------------------------------------------------------------------
 * HF_DisconnectServiceLink()
 *
 *    Releases the service level connection with the audio gateway. This will 
 *    close the RFCOMM channel and will also close the SCO and ACL links if no 
 *    other services are active, and no other link handlers are in use 
 *    (ME_CreateLink). When the operation is complete the application will be 
 *    notified by the HF_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the service level connection is down (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *        does not exist to the audio gateway.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DisconnectServiceLink(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_CreateAudioLink()
 *
 *    Creates an audio(SCO) link to the audio gateway. The success of the 
 *    operation is indicated by the HF_EVENT_AUDIO_CONNECTED event. If the 
 *    connection fails, the application is notified by the 
 *    HF_EVENT_AUDIO_DISCONNECTED event.  
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the audio link has been established (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_SUCCESS - The audio(SCO) link already exists.
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_NO_CONNECTION - The operation failed because a service level 
 *        connection does not exist to the audio gateway.
 *
 *    BT_STATUS_FAILED - An audio connection already exists.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_CreateAudioLink(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_DisconnectAudioLink()
 *
 *    Releases the audio connection with the audio gateway. When the 
 *    operation is complete, the application will be notified by the 
 *    HF_EVENT_AUDIO_DISCONNECTED event.
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the audio connection is down (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *        does not exist to the audio gateway.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DisconnectAudioLink(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_AcceptConnect()
 *
 *    Accept connection request from remote devices. If operation is successfully completed,
 *    HF sends HF_EVENT_SERVICE_CONNECTED event to ADP. If the connection can not be 
 *    created, HF sends HF_EVENT_SERVICE_DISCONNECTED event to ADP.
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the audio connection is up or down (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_AcceptConnect(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_RejectConnect()
 *
 *    Reject connection request from remote devices. If operation is successfully completed,
 *    HF sends HF_EVENT_SERVICE_DISCONNECTED event to ADP. 
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 * Returns:
 *    BT_STATUS_PENDING - The operation has started, the application will be 
 *        notified when the audio connection is down (via the callback 
 *        function registered by HF_Register).
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_RejectConnect(HfChannel_struct *pChannel);

/*---------------------------------------------------------------------------
 * HF_SendData()
 *
 *    Send AT command to the audio gateway.
 *
 * Parameters:
 *    pChannel - Pointer to a registered channel structure.
 *
 *    pData - The string of AT command.
 *
 *    len - The length of AT command string.
 *  
 * Returns:
 *    BT_STATUS_SUCCESS - The data has been sent to the queue.
 *
 *    BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *    BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *        initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_SendData(HfChannel_struct *pChannel, U8 *pData, U16 len); 

#endif /* __HF_H__ */
