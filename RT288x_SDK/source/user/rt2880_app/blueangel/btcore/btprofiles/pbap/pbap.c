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

/***************************************************************************
*
* File:
*     $Workfile:pbap.c$ for iAnywhere Blue SDK, PBAP Version 1.1.2
*     $Revision: #2 $
*
* Description:
*     This file contains the functions that comprise the Phonebook Access
*     Profile implementation.
*
* $Project:XTNDAccess Blue SDK$
*
* Copyright 2005-2006 Extended Systems, Inc.
* Portions copyright 2006 iAnywhere Solutions, Inc.
* All rights reserved. All unpublished rights reserved.
*
* Unpublished Confidential Information of iAnywhere Solutions, Inc.  
* Do Not Disclose.
*
* No part of this work may be used or reproduced in any form or by any 
* means, or stored in a database or retrieval system, without prior written 
* permission of iAnywhere Solutions, Inc.
* 
* Use of this work is governed by a license granted by iAnywhere Solutions, 
* Inc.  This work contains confidential and proprietary information of 
* iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
* trademark and other intellectual property rights.
*
****************************************************************************
*============================================================================
* HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log$
*
* 
*
* 
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
/* Keep this header file outside of the compile option to avoid compile warning */

#ifdef __BT_PBAP_PROFILE__

#include "osapi.h"
#if BT_STACK == XA_ENABLED
#include "bttypes.h"
#endif /* BT_STACK == XA_ENABLED */
#include "pbap.h"
#include "pbapobs.h"


extern U8 ObexSdpDB_Registration(U32 service_type);
extern U8 ObexSdpDB_DeRegistration(U32 service_type);
extern void PBAPDeviceSdpDB_SupportedRepositories(U8 support_value);
/* Object Store Function call tables */
static const ObStoreFuncTable PhonebookStoreFuncTable = 
{
    PBAPOBS_Read,
    PBAPOBS_Write,
    PBAPOBS_GetObjectLen,
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
   //NULL,PBAPOBS_ReadFlex,
   //PBAPOBS_WriteFlex,
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
};

static const U8 PbapUuid[] = 
{
    0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8,
    0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 
};

#if PBAP_NUM_SERVERS > 0
static void SetOperation(const U8 *Type);
#endif /* PBAP_NUM_SERVERS > 0 */

static void PBAP_SetDefaultHeader(void);

#if XA_CONTEXT_PTR == XA_ENABLED
static PbapContext   temp;
PbapContext         *Pbap = &temp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
PbapContext          Pbap;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

#define PBAP_SERVER_INIT
/*****************************************************************************
* FUNCTION
*  PBAP_Init
* DESCRIPTION
*  Initialize the PBAP component.  This must be the first PBAP function called 
*  by the application layer, or if multiple PBAP applications exist, this function
*  should be called at system startup (see XA_LOAD_LIST in config.h).  GOEP and 
*  OBEX must also be initialized separately.
* PARAMETERS
*  void
* RETURNS
*  BT_BOOL
*****************************************************************************/
BT_BOOL PBAP_Init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL status = TRUE;  
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)Pbap, 0, sizeof(PbapContext)); 
#else
    OS_MemSet((U8 *)&Pbap, 0, sizeof(PbapContext)); 
#endif
	
    if (PBAPOBS_Init() == FALSE) 
    {	
	 Report(("PBAP Profile Initial FAILED\n"));			
        status = FALSE;
        goto Done;
    }
	
    PBAP(initialized) = TRUE;
	
Done:
    OS_UnlockObex();
    return status;
}


#if PBAP_NUM_SERVERS > 0 
/*****************************************************************************
* FUNCTION
*  PBAP_RegisterServer
* DESCRIPTION
*  PBAP server register function (now not multiplexor GOEP server request)
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_RegisterServer(PbapServerSession *Server, PbapServerCallback Callback)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if BT_SECURITY == XA_ENABLED
    BtSecurityLevel savedSecLevel;
#endif
    U8   saveSupportRepos;
    U8 i, j = PBAP_NUM_SERVERS;
    ObStatus status = OB_STATUS_BUSY;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server || !Callback) 
    {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE) 
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif
	ASSERT(Server && Callback);
#if BT_SECURITY == XA_ENABLED
    savedSecLevel = Server->secLevel;
#endif
    saveSupportRepos = Server->suppored_repositories;
    OS_MemSet((U8 *)Server, 0, sizeof(PbapServerSession));
	
#if BT_SECURITY == XA_ENABLED
    Server->secLevel = savedSecLevel;
#endif
     Server->suppored_repositories = saveSupportRepos;

    if (PBAP(serverCallback) == 0 || PBAP(serverCallback) == Callback)
    {
        PBAP(serverCallback) = Callback;
        
        for (i = PBAP_NUM_SERVERS; i > 0; i--) 
        {
            if (PBAP(server)[i-1] == Server) 
            {
                status = BT_STATUS_IN_USE;
                goto Error;
            } 
            else if (PBAP(server)[i-1] == 0) 
            {
                j = i - 1;
            }
        }
    }
    else 
    {
        status = BT_STATUS_INVALID_PARM;
        goto Error;
    }
	
    Server->obexConn.target = (U8*)PbapUuid;
    Server->obexConn.targetLen = 16;
#if OBEX_SERVER_CONS_SIZE > 0
    Server->sApp.numTargets = 1;
    Server->sApp.target[0] = &Server->obexConn;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    Server->sApp.type = GOEP_PROFILE_PBAP;
    Server->sApp.bt_service = GOEP_PROFILE_PBAP;
    Server->sApp.callback = PbapGoepServerCallback;
    Server->sApp.connFlags = GOEP_NEW_CONN;
    Server->sApp.connId = 0;
 
	
    if (j < PBAP_NUM_SERVERS)
    {
        PBAP(server)[j] = Server;
        status = GOEP_RegisterServer(&Server->sApp, &PhonebookStoreFuncTable);
        if (status != OB_STATUS_SUCCESS)
        {
            goto Error;
        }
		PBAPDeviceSdpDB_SupportedRepositories(Server->suppored_repositories);
		ObexSdpDB_Registration(0x112F);
		Report(("PBAP Registered On GOEP\n"));
    }
    else 
    {
        status = BT_STATUS_NO_RESOURCES;
        goto Error;
    }
    
#if BT_SECURITY == XA_ENABLED
    /* Add Phonebook Access Bluetooth security record */
    if (Server->secLevel != BSL_NO_SECURITY) {
        /* Security record to enforce authentication/authorization/encryption */ 
        status = GOEP_RegisterServerSecurityRecord(&Server->sApp, Server->secLevel);
        ASSERT(status == XA_STATUS_SUCCESS);
    }
#endif /* BT_SECURITY == XA_ENABLED */
    
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    if (status != OB_STATUS_SUCCESS) {
        GOEP_DeregisterServer(&Server->sApp);
		ObexSdpDB_DeRegistration(0x112f);
        goto Error;
    }
#else /* OBEX_DEINIT_FUNCS == XA_ENABLED */
    ASSERT(status == OB_STATUS_SUCCESS);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
	
Error:
    OS_UnlockObex();
    return status;
}


#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  PBAP_DeregisterServer
* DESCRIPTION
*  deregister PBAP server from GOEP
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_DeregisterServer(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, numActiveServers;
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server)
    {
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE)
    {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    numActiveServers = 0;
    for (i = 0; i < PBAP_NUM_SERVERS; i++)
    {
        if (PBAP(server)[i] == Server) 
        {
            PBAP(server)[i] = 0;
            status = BT_STATUS_SUCCESS;
        }
        if (PBAP(server)[i] != 0)
        {
            numActiveServers++;
        }
    }
	
    if (numActiveServers == 0) 
    {
        PBAP(serverCallback) = 0;
    }
	
    status = GOEP_DeregisterServer(&Server->sApp);
    ObexSdpDB_DeRegistration(0x112f);
	/* kal_trace(TRACE_GROUP_5, BT_PBAP_DEREGISTERED);*/ 
	Report(("PBAP Deregistered On GOEP\n"));
	
    if (status == OB_STATUS_SUCCESS)
    {
#if BT_SECURITY == XA_ENABLED
        if (Server->secLevel != BSL_NO_SECURITY)
        {
            GOEP_UnregisterServerSecurityRecord(&Server->sApp);
			ASSERT(status == XA_STATUS_SUCCESS);
        }
#endif /* BT_SECURITY == XA_ENABLED */
    }
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */


#define PBAP_SERVER_PROCESS_FUNCTION
/*****************************************************************************
* FUNCTION
*  PBAP_ServerAbort
* DESCRIPTION
*  when the server find any error , then call this function to send error info to
*  client.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAbort(PbapServerSession *Server, PbapRespCode Resp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex(); 
    Report(("PBAP Server Abort\n"));
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerAbort(&Server->sApp, Resp);
	
Error:
    OS_UnlockObex();
    return status;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerContinue
* DESCRIPTION
* Synopsis: This function is called by the server in response to a received
*           PBAP_EVENT_CONTINUE event. It must be called once for every
*           CONTINUE event received. It may or may not be called in the 
*           context of the callback and can be deferred for flow control 
*           purposes.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerContinue(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerContinue(&Server->sApp);
	
Error:
    OS_UnlockObex();
    return status;
}


#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED 
/*****************************************************************************
* FUNCTION
*  PBAP_ServerTpDisconnect
* DESCRIPTION
* Synopsis: This function is used by the server to issue a transport 
*           disconnection.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerTpDisconnect(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server)
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    status = GOEP_ServerTpDisconnect(&Server->sApp);
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */


#define PBAP_SERVER_AUTH_FUNCTION
#if OBEX_AUTHENTICATION == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthenticate
* DESCRIPTION
* Synopsis: This function is called by the server to initiate OBEX 
*           authentication on the upcoming Phonebook Access OBEX connection.
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAuthenticate(PbapServerSession *Server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    TimeT       time;
    U16         passwordLen;
    ObStatus    status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    if (Server->flags & PBAP_FLAGS_AUTHENTICATED) {
        /* We are already authenticated */
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
	
    Server->flags |= PBAP_FLAGS_AUTHENTICATION_REQ;
    OS_MemSet((U8 *)&Server->chal, 0, sizeof(Server->chal));
	
    passwordLen = OS_StrLen((const char *)Server->password);
    time = OS_GetSystemTime();
    Server->chalStr[0] = (U8)(time);
    Server->chalStr[1] = (U8)(time >> 8);
    Server->chalStr[2] = (U8)(time >> 16);
    Server->chalStr[3] = (U8)(time >> 24);
    Server->chalStr[4] = ':';
    OS_MemCopy(Server->chalStr+5, Server->password, passwordLen);
	
    Server->chal.challenge = Server->chalStr;
    Server->chal.challengeLen = 5 + passwordLen;
    Server->chal.options = 0x01;  /* Must send the UserId */
    Server->chal.realm = Server->realm;
    Server->chal.realmLen = (U8)OS_StrLen((const char *)Server->realm);
	
    Server->flags |= PBAP_FLAGS_CHAL_REQ;
	
    status = OB_STATUS_SUCCESS;
	
Error:
    OS_UnlockObex();
    return status;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthCheck
* DESCRIPTION
* Synopsis: This function is called by the server to set the proper username, 
*           password, and challenge realm information for the upcoming 
*           Phonebook Access OBEX connection. 
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerSetAuthInfo(PbapServerSession *Server, PbapAuthInfo *Info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus    status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server || !Info)
    {
        goto Error;
    }
	
    if (Info->passwordLen >= PBAP_MAX_PASSWORD_LEN ||
        Info->userIdLen >= PBAP_MAX_USERID_LEN ||
        Info->realmLen >= PBAP_MAX_REALM_LEN)
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    /* Clear the password, userId, and realm to make sure null-termination
	* always exists, as we always ensure that one byte is unused.
	*/
    OS_MemSet(Server->password, 0, PBAP_MAX_PASSWORD_LEN);
    OS_MemSet(Server->userId, 0, PBAP_MAX_USERID_LEN);
    OS_MemSet(Server->realm, 0, PBAP_MAX_REALM_LEN);
	
    /* Set the authentication information */
    OS_MemCopy(Server->password, Info->password, Info->passwordLen);
    OS_MemCopy(Server->userId, Info->userId, Info->userIdLen);
    OS_MemCopy(Server->realm, Info->realm, Info->realmLen);
	
    status = OB_STATUS_SUCCESS;
	
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */



#define PBAP_SERVER_HANDLE_GOEP_EVENT_CALLBACK 
/*****************************************************************************
* FUNCTION
*  PbapGoepServerCallback
* DESCRIPTION
*  This function processes GOEP Server protocol events.
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void PbapGoepServerCallback(GoepServerEvent *Event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                                                         */
    /*----------------------------------------------------------------*/
    U8 *p, i, j;
    PbapServerSession *server;
    ObexRespCode rcode;
    PbapOp savedOper;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                                                               */
    /*----------------------------------------------------------------*/
    kal_trace(TRACE_GROUP_5, BT_PBAP_SERVER_GOEP_CALLBACK, Event->event, Event->oper);
	
    server = FindPbapServer(Event->handler);
    
    if (server == 0) 
    {
	Report(("PBAP Server Not Found\n"));
        return;
    }
	
    savedOper = PBAP(serverParms).oper;
    PBAP(serverParms).oper = savedOper;
    PBAP(serverParms).server = server;
	
    if ((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_DELETE))
    {
        /* This is an illegal Phonebook Access command. Abort it and hide it from the app. */
        if (Event->event == GOEP_EVENT_START)
        {
            GOEP_ServerAbort(Event->handler, OBRC_BAD_REQUEST);            
            GOEP_ServerContinue(Event->handler);
        }
		
        if (Event->event == GOEP_EVENT_CONTINUE)
        {
            GOEP_ServerContinue(Event->handler);
        }
        return;
    }
	
    switch (Event->event)
    {
	case GOEP_EVENT_AUTHORIZE_IND:
		PBAP(serverParms).event = PBAP_EVENT_AUTHRIZE_IND;
		PBAP(serverCallback)(&PBAP(serverParms));
		break;
		
	case GOEP_EVENT_START:
		OS_MemSet((U8 *)&PBAP(serverParms), 0, sizeof(PbapServerCallbackParms));
		switch (Event->oper)
		{
		case GOEP_OPER_CONNECT:
			PBAP(serverParms).oper = PBAPOP_CONNECT;
			break;
			
		case GOEP_OPER_DISCONNECT:
			PBAP(serverParms).oper = PBAPOP_DISCONNECT;
			break;
			
		case GOEP_OPER_PULL:
			/* for fix no header client */
			PBAP_SetDefaultHeader();                   
			server->object = PBAPOBS_New(&server->sApp);
		 	ASSERT(server->object);
			OS_MemSet((U8 *)&server->appParms, 0, PBAP_MAX_APP_PARMS_LEN);
			PBAP(serverParms).oper = PBAPOP_PULL;
			break;
			
		case GOEP_OPER_SETFOLDER:
			PBAP(serverParms).oper = PBAPOP_SET_PHONEBOOK;
			break;
		}
		
		PBAP(serverParms).event = PBAP_EVENT_START;
		PBAP(serverCallback)(&PBAP(serverParms));
		break;
		
        case GOEP_EVENT_HEADER_RX:
			
			if(Event->header.type == OBEXH_APP_PARAMS
				&& Event->header.len >0 
				&& PBAP(serverParms).oper == PBAPOP_PULL)
			{
				if (PBAP(serverParms).oper == PBAPOP_PULL)
				{
					SetOperation(Event->info.pushpull.type);
				}
				PBAP(serverParms).u.paramsRx.type = (U8 *)Event->info.pushpull.type;
				
                               if( (server->appParms[0] + Event->header.len + 1) >= PBAP_MAX_APP_PARMS_LEN){
                                   bt_prompt_trace(MOD_BT, "[PBAP] out of boundary of server->appParms");
                                   break;
                               }
				p = server->appParms + server->appParms[0] + 1;
				OS_MemCopy(p, Event->header.buffer, Event->header.len);
				server->appParms[0] += Event->header.len;
				
				if (server->appParms[0] == Event->header.totalLen)
				{
					p = server->appParms + 1;
					while (p - (server->appParms + 1) < Event->header.totalLen)
					{
						switch (p[0])
						{
						case PBAP_TAG_FILTER:
							j = 2;
							for (i = PBAP_FILTER_SIZE; i > 0; i--)
							{
								PBAP(serverParms).u.paramsRx.filter.byte[i-1] = p[j++];
							}
							break;
							
						case PBAP_TAG_FORMAT:
							PBAP(serverParms).u.paramsRx.format = p[2];
							break;
							
						case PBAP_TAG_ORDER:
							PBAP(serverParms).u.paramsRx.order = p[2];
							break;
							
						case PBAP_TAG_SEARCH_VALUE:
							PBAP(serverParms).u.paramsRx.searchValue = p + 2;
							PBAP(serverParms).u.paramsRx.searchValueLen = p[1];
							break;
							
						case PBAP_TAG_SEARCH_ATTRIB:
							PBAP(serverParms).u.paramsRx.searchAttrib = p[2];
							break;
							
						case PBAP_TAG_MAX_LIST_COUNT:
							PBAP(serverParms).u.paramsRx.maxListCount = BEtoHost16(p + 2);
							break;
							
						case PBAP_TAG_LIST_OFFSET:
							PBAP(serverParms).u.paramsRx.listStartOffset = BEtoHost16(p + 2);
							break;
						}
						
						p += 1 + 1 + p[1];
					}
				}				
				
				if (Event->info.pushpull.nameLen != 0)
				{
					PBAP(serverParms).u.paramsRx.name = (U8 *)Event->info.pushpull.name;
				}			
			}   
			break;
			
		case GOEP_EVENT_PROVIDE_OBJECT:
				if (PBAP(serverParms).oper == PBAPOP_PULL)
				{
					SetOperation(Event->info.pushpull.type);
				}   
				
				PBAP(serverParms).u.paramsRx.type = (U8 *)Event->info.pushpull.type;
				
				if ((Event->info.pushpull.nameLen != 0)
					&&(PBAP(serverParms).oper == PBAPOP_PULL_PHONEBOOK
					|| PBAP(serverParms).oper == PBAPOP_PULL_VCARD_LISTING
					|| PBAP(serverParms).oper == PBAPOP_PULL_VCARD_ENTRY))
				{
					PBAP(serverParms).u.paramsRx.name = (U8 *)Event->info.pushpull.name;
				}
				break;
				
		case GOEP_EVENT_CONTINUE:
				switch(Event->oper) 
				{
					case GOEP_OPER_DISCONNECT:
					case GOEP_OPER_ABORT:
						if (server->object)
						{
							PBAPOBS_Close(&server->object);
						}
						GOEP_ServerContinue(&server->sApp);
						return;

					 case GOEP_OPER_CONNECT:                    
					 	PBAP(serverParms).event = PBAP_EVENT_TP_CONNECTED;
						PBAP(serverParms).oper = PBAPOP_CONNECT;
						break;
						
					case GOEP_OPER_SETFOLDER:
						PBAP(serverParms).u.info.setPb.name = (const U8 *)Event->info.setfolder.name;
						PBAP(serverParms).u.info.setPb.flags = Event->info.setfolder.flags;
						PBAP(serverParms).u.info.setPb.reset = Event->info.setfolder.reset;
						PBAP(serverParms).oper = PBAPOP_SET_PHONEBOOK;
						break;
						
					case GOEP_OPER_PULL:
						{

						}
						break;
					default:
						break;
				}				
				PBAP(serverParms).event = PBAP_EVENT_CONTINUE;
				PBAP(serverCallback)(&PBAP(serverParms));
				break;
				
			case GOEP_EVENT_PRECOMPLETE:
				PBAP(serverParms).event = PBAP_EVENT_PRECOMPLETE;
				PBAP(serverCallback)(&PBAP(serverParms));
				break;
					
			case GOEP_EVENT_COMPLETE:
					switch (Event->oper)
					{
					case GOEP_OPER_DISCONNECT:
						PBAP(serverParms).oper = PBAPOP_DISCONNECT;
						server->flags = 0; /* Reset flags */
						break;
						
					case GOEP_OPER_PULL:
						if (server->object)
						{
							rcode = PBAPOBS_Close(&server->object);
							if (rcode != OBRC_SUCCESS)
							{
								GOEP_ServerAbort(&server->sApp, rcode);
							}
						} 
						break;
						
					default:
						break;
					}
					
#if OBEX_AUTHENTICATION == XA_ENABLED
					if ((server->flags & PBAP_FLAGS_AUTHENTICATION_REQ) && 
						!(server->flags & PBAP_FLAGS_RESP_RCVD))
					{
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_RCVD_AUTH_RESP;
						PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
						PBAP(serverCallback)(&PBAP(serverParms));
					}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					
					kal_trace(TRACE_GROUP_5, BT_PBAP_SERVER_OPER_COMPLETE, Event->oper);					
					PBAP(serverParms).event = PBAP_EVENT_COMPLETE;
					PBAP(serverCallback)(&PBAP(serverParms));					
					
#if OBEX_AUTHENTICATION == XA_ENABLED
					/* Clear authentication response flag if one was received */
					server->flags &= ~PBAP_FLAGS_RESP_RCVD;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					break;
					
				case GOEP_EVENT_TP_CONNECTED:
					break;
						
				case GOEP_EVENT_TP_DISCONNECTED:
					if (server->object)
					{
						PBAPOBS_Close(&server->object);
					}						
					server->flags = 0;
					PBAP(serverParms).event = PBAP_EVENT_TP_DISCONNECTED;
					PBAP(serverParms).oper = PBAPOP_NONE;
					PBAP(serverCallback)(&PBAP(serverParms));
					break;
						
				case GOEP_EVENT_ABORTED:
					if (server->object)
					{
						PBAPOBS_Close(&server->object);
					}
						
#if OBEX_AUTHENTICATION == XA_ENABLED
					if ((server->flags & PBAP_FLAGS_AUTHENTICATION_REQ) && 
							!(server->flags & PBAP_FLAGS_RESP_RCVD))
					{
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_RCVD_AUTH_RESP;
						PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
						PBAP(serverCallback)(&PBAP(serverParms));
					}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					PBAP(serverParms).event = PBAP_EVENT_ABORTED;
					PBAP(serverCallback)(&PBAP(serverParms));
					PBAP(serverParms).oper = PBAPOP_NONE;
						
#if OBEX_AUTHENTICATION == XA_ENABLED
					/* Clear authentication response flag if one was received */
					server->flags &= ~PBAP_FLAGS_RESP_RCVD;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
					break;						
		  				
#if OBEX_AUTHENTICATION == XA_ENABLED
				case GOEP_EVENT_AUTH_CHALLENGE:
					/* Authentication challenge has been received.
					* Prepare authentication response to go out.*/
					OS_MemSet((U8 *)&server->resp, 0, sizeof(server->resp));
					OS_MemSet((U8 *)&server->challengeInfo, 0, sizeof(server->challengeInfo));
					server->flags |= PBAP_FLAGS_CHAL_RCVD;
					server->challengeInfo.options = Event ->challenge.options;
					server->challengeInfo.realmLen =  Event ->challenge.realmLen;
					OS_MemCopy(server->challengeInfo.realm, Event ->challenge.realm, server->challengeInfo.realmLen);
					OS_MemCopy(server->challengeInfo.nonce, Event ->challenge.nonce, AUTH_NONCE_LEN);
					break;
					
				case GOEP_EVENT_AUTH_RESPONSE:
					/* Authentication response has been received */
					if (GOEP_ServerVerifyAuthResponse(Event->handler, server->password, 
							(U8)OS_StrLen((const char *)(server->password))))
					{
						server->flags |= PBAP_FLAGS_AUTHENTICATED;
						PBAP(serverParms).u.auth.result = TRUE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_NO_FAILURE;
					}
					else 
					{
						GOEP_ServerAbort(Event->handler, OBRC_UNAUTHORIZED);
						PBAP(serverParms).u.auth.result = FALSE;
						PBAP(serverParms).u.auth.reason = PBAP_AUTH_INVALID_CREDENTIALS;
					}
					PBAP(serverParms).event = PBAP_EVENT_AUTH_RESULT;
					PBAP(serverCallback)(&PBAP(serverParms));
					server->flags |= PBAP_FLAGS_RESP_RCVD;
					break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				default:
					break;
			} 
}


#define PBAP_SERVER_UTIL_FUNCTION
/*****************************************************************************
* FUNCTION
*  PBAP_SetDefaultHeader
* DESCRIPTION
*  if the get command have no header , it will use default header to get data
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
static void PBAP_SetDefaultHeader(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemSet(PBAP(serverParms).u.paramsRx.filter.byte, 0, PBAP_FILTER_SIZE);
    PBAP(serverParms).u.paramsRx.format = VCARD_FORMAT_21;
    PBAP(serverParms).u.paramsRx.order = PBAP_SORT_ORDER_INDEXED;
    PBAP(serverParms).u.paramsRx.searchValue = NULL;
    PBAP(serverParms).u.paramsRx.searchValueLen = 0;
    PBAP(serverParms).u.paramsRx.searchAttrib = PBAP_SEARCH_ATTRIB_NAME;
    PBAP(serverParms).u.paramsRx.listStartOffset = 0;
    PBAP(serverParms).u.paramsRx.maxListCount = PBAP_INVALID_COUNT;
}


/*****************************************************************************
* FUNCTION
*  PBAP_ServerAuthCheck
* DESCRIPTION
*  check the server wether has received the auth challeng request
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_ServerAuthCheck(PbapServerSession *Server, U8 cancel)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Server) 
    {
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Server);
    if (cancel == 1)
    {
        GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        goto Error;
    }
    
#if OBEX_AUTHENTICATION == XA_ENABLED
    if (Server->flags & PBAP_FLAGS_CHAL_REQ)
    {
        Server->flags &= ~PBAP_FLAGS_CHAL_REQ;
        /* Issue an OBEX Authentication challenge header */
        status = GOEP_ServerAuthenticate(&Server->sApp, 0, &Server->chal);
        if (status != OB_STATUS_SUCCESS)
        {
            GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        }
    }
    else if (Server->flags & PBAP_FLAGS_CHAL_RCVD)
    {
        Server->flags &= ~PBAP_FLAGS_CHAL_RCVD;
        /* Issue an OBEX Authentication response header */
        status = GOEP_ServerAuthenticate(&Server->sApp, &Server->resp, 0);
        if (status != OB_STATUS_SUCCESS)
        {
            GOEP_ServerAbort(&Server->sApp, OBRC_UNAUTHORIZED);
        }
    }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
	
Error:
    OS_UnlockObex();
    return status;
}

/*****************************************************************************
* FUNCTION
*  SetOperation
* DESCRIPTION
* 
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
static void SetOperation(const U8 *Type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16     len;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    len = OS_StrLen((const char *)Type);
	
    if (OS_MemCmp(Type, len, (const U8 *)PULL_PHONEBOOK_TYPE, OS_StrLen((const char *)PULL_PHONEBOOK_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_PHONEBOOK;
    } 
    else if (OS_MemCmp(Type, len, (const U8 *)VCARD_LISTING_OBJECT_TYPE, OS_StrLen((const char *)VCARD_LISTING_OBJECT_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_VCARD_LISTING;
    } 
    else if (OS_MemCmp(Type, len, (const U8  *)VCARD_OBJECT_TYPE, OS_StrLen((const char *)VCARD_OBJECT_TYPE)))
    {
        PBAP(serverParms).oper = PBAPOP_PULL_VCARD_ENTRY;
    }
    
}


/*****************************************************************************
* FUNCTION
*  FindPbapServer
* DESCRIPTION
*  if the pbap server support multiple client, then this function is to find the
*  right server to handle the client request. return PBAP server session pointer.
* PARAMETERS
*  
* RETURNS
*  PbapServerSession
*****************************************************************************/
PbapServerSession *FindPbapServer(GoepServerApp *app)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < PBAP_NUM_SERVERS; i++)
    {
        if (PBAP(server)[i] && &PBAP(server)[i]->sApp == app)
        {
            return PBAP(server)[i];
        }
    }	
    return 0;
}

#endif /* PBAP_NUM_SERVERS > 0 */

#if PBAP_NUM_CLIENTS > 0
static ObStatus pbapc_obex_connect(GoepClientApp *Client, ObexAuthResponse* authrsp); 
/*****************************************************************************
* FUNCTION
*  PBAP_ClientRegisterSDPDB
* DESCRIPTION
*  Register PCE SDP DB record.
* PARAMETERS
*  
* RETURNS
*  U8
*****************************************************************************/
U8 PBAP_ClientRegisterSDPDB()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 status;	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_LockObex();
	
	status = ObexSdpDB_Registration(SC_PBAP_CLIENT);

	OS_UnlockObex();
	return status;
}

/*****************************************************************************
* FUNCTION
*  PBAP_ClientDeregisterSDPDB
* DESCRIPTION
*  Remove PCE SDP DB record.
* PARAMETERS
*  
* RETURNS
*  U8
*****************************************************************************/
U8 PBAP_ClientDeregisterSDPDB()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 status;	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_LockObex();
	
	status = ObexSdpDB_DeRegistration(SC_PBAP_CLIENT);

	OS_UnlockObex();
	return status;
}

/*****************************************************************************
* FUNCTION
*  FindPbapClient
* DESCRIPTION
*  if the pbap client support multiple servers, then this function is to find the
*  right client to handle the server request. return PBAP client session pointer.
* PARAMETERS
*  
* RETURNS
*  PbapClientSession
*****************************************************************************/
PbapClientSession *FindPbapClient(GoepClientApp *app)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < PBAP_NUM_CLIENTS; i++)
    {
        if (PBAP(client)[i] && &PBAP(client)[i]->cApp == app)
        {
            return PBAP(client)[i];
        }
    }	
    return 0;
}

/*****************************************************************************
* FUNCTION
*  PbapGoepServerCallback
* DESCRIPTION
*  This function processes GOEP Server protocol events.
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void PbapGoepClientCallback(GoepClientEvent *Event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	PbapClientSession	*client;
	GoepClientApp		*goepClient;
	PbapClientCallbackParms parms;
	U8		*p;
	U16		len;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	goepClient = Event->handler;
	
	client = FindPbapClient(goepClient);

	BT_PBAP_LOG5(BT_PBAP_CLIENT_GOEP_CALLBACK,Event->event,Event->oper,client->connState,Event->reason,Event->failed_rsp_code);

	if (client == 0)
	{
		BT_PBAP_LOG(BT_PBAP_CLIENT_NOT_FOUND);
		return;
	}

	if ((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_DELETE))
	{
		/* This is an illegal Phonebook access command. Hide it from the app */
		if (Event->event == GOEP_EVENT_CONTINUE)
		{
			GOEP_ClientContinue(Event->handler);
		}
		return;
	}

	parms.client = client;

	switch (Event->event)
	{
	case GOEP_EVENT_TP_CONNECTED:
		{
			if (client->connState == PBAP_STATE_DISCONNECTING)
			{
				GOEP_TpDisconnect(goepClient);
				break;
			}
			
			pbapc_obex_connect(goepClient, NULL);
			client->connState = PBAP_STATE_OBEX_CONNECTING;
		}
		break;
	case GOEP_EVENT_TP_DISCONNECTED:
		{
			parms.event = PBAP_EVENT_TP_DISCONNECTED;
			client->connState = PBAP_STATE_IDLE;
			PBAP(clientCallback)(&parms);
			if (client->object)
			{
				PBAPOBS_Close(&client->object);
				client->object = NULL;
			}
			PBAP_DeregisterClient(client);
		}
		break;
	case GOEP_EVENT_CONTINUE:
		{
			switch (Event->oper)
			{
			case GOEP_OPER_PULL:
				GOEP_ClientContinue(&client->cApp);
				break;
			default:
				break;
			}
		}
		break;
	case GOEP_EVENT_HEADER_RX:
		{
			BT_PBAP_LOG1(BT_PBAP_CLIENT_CB_HEADER_RX, Event->header.type);
			if ((Event->header.type == OBEXH_APP_PARAMS) && (Event->header.len > 0))
			{
				p = Event->header.buffer;
				len = Event->header.len;

				while (len > 0)
				{
					U16 parm_len = 0;
					switch (p[0])
					{
					case PBAP_TAG_PHONEBOOK_SIZE:
						{
							parms.u.paramsRx.phonebookSize = BEtoHost16(p + 2);
							parm_len = 4;
						}
						break;
					case PBAP_TAG_MISSED_CALLS:
						{
							parms.u.paramsRx.newMissedCall = *(p + 2);
							parm_len = 3;
						}
						break;
					default:
						BT_PBAP_LOG1(BT_PBAP_CLIENT_CB_RX_UNKNOW_APP_PARAMS, p[0]);
						break;
					}
					len -= parm_len;
					p += parm_len;
				}

				parms.event = PBAP_EVENT_PARAMS_RX;
				PBAP(clientCallback)(&parms);
			}	
		}
		break;
	case GOEP_EVENT_COMPLETE:
		{
			if (client->connState == PBAP_STATE_DISCONNECTING)
			{
				if (Event->oper == GOEP_OPER_DISCONNECT)
				{
					GOEP_TpDisconnect(goepClient);
				}
				else
				{
					GOEP_Disconnect(goepClient);
				}
				break;
			}
			parms.event = PBAP_EVENT_COMPLETE;
			switch (Event->oper)
			{
			case GOEP_OPER_CONNECT:
				{
					parms.oper = PBAPOP_CONNECT;
					PBAP(clientCallback)(&parms);
					client->connState = PBAP_STATE_CONNECTED;
				}
				break;
			case GOEP_OPER_SETFOLDER:
				{
					parms.oper = PBAPOP_SET_PHONEBOOK;
					PBAP(clientCallback)(&parms);
					client->connState = PBAP_STATE_CONNECTED;
				}
				break;
			case GOEP_OPER_PULL:
				{
					parms.oper = PBAPOP_PULL;
					PBAP(clientCallback)(&parms);
					if (client->object)
					{
						PBAPOBS_Close(&client->object);
						client->object = NULL;
					}
					client->connState = PBAP_STATE_CONNECTED;
				}
				break;
			case GOEP_OPER_DISCONNECT:
				{
					GOEP_TpDisconnect(goepClient);
					client->connState = PBAP_STATE_DISCONNECTING;
				}
				break;
			case GOEP_OPER_ABORT:
				{
					parms.oper = PBAPOP_ABORT;
					PBAP(clientCallback)(&parms);
					client->connState = PBAP_STATE_CONNECTED;
				}
			default:
				break;
			}

			if (client->object)
			{
				PBAPOBS_Close(&client->object);
				client->object = NULL;
			}
		}
		break;
	case GOEP_EVENT_ABORTED:
		{
			if (client->connState == PBAP_STATE_DISCONNECTING)
			{
				GOEP_TpDisconnect(&client->cApp);
				break;
			}
			else
#if OBEX_AUTHENTICATION == XA_ENABLED
			if ((Event->oper == GOEP_OPER_CONNECT) && (Event->reason == OBRC_UNAUTHORIZED) &&
				(client->flags & PBAP_FLAGS_CHAL_RCVD))
			{
				parms.event = PBAP_EVENT_AUTHCHAL_IND;
				PBAP(clientCallback)(&parms);
				break;
			}
			else
#endif
			{
				client->connState = PBAP_STATE_CONNECTED;
				switch (Event->oper)
				{
				case GOEP_OPER_SETFOLDER:
					{
						parms.oper = PBAPOP_SET_PHONEBOOK;
						parms.event = PBAP_EVENT_ABORTED;
						PBAP(clientCallback)(&parms);
						if (client->object)
						{
							PBAPOBS_Close(&client->object);
							client->object = NULL;
						}						
					}
					break;
				case GOEP_OPER_PULL:
					{
						parms.oper = PBAPOP_PULL;
						parms.event = PBAP_EVENT_ABORTED;
						PBAP(clientCallback)(&parms);
						if (client->object)
						{
							PBAPOBS_Close(&client->object);
							client->object = NULL;
						}
					}
					break;
				default:
					BT_PBAP_LOG1(BT_PBAP_CLIENT_CB_NOT_HANDLE, Event->event);
					break;
				}
			}

			if (client->object)
			{
				PBAPOBS_Close(&client->object);
				client->object = NULL;
			}
		}
		break;
#if OBEX_AUTHENTICATION == XA_ENABLED
	case GOEP_EVENT_AUTH_CHALLENGE:
		{
			client->flags |= PBAP_FLAGS_CHAL_RCVD;
			OS_MemSet((U8*)&client->resp, 0, sizeof(client->resp));
			OS_MemSet((U8*)&client->challengeInfo, 0, sizeof(client->challengeInfo));
			client->challengeInfo.options = Event->challenge.options;
			client->challengeInfo.realmLen = Event->challenge.realmLen;
			OS_MemCopy(client->challengeInfo.realm, Event->challenge.realm, client->challengeInfo.realmLen);
			OS_MemCopy(client->challengeInfo.nonce, Event->challenge.nonce, AUTH_NONCE_LEN);
		}
		break;
#endif		
	default:
		BT_PBAP_LOG1(BT_PBAP_CLIENT_CB_NOT_HANDLE, Event->event);
		break;
	}
}

/*****************************************************************************
* FUNCTION
*  PBAP_RegisterClient
* DESCRIPTION
*  PBAP client register function 
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_RegisterClient(PbapClientSession *Client, PbapClientCallback Callback)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j = PBAP_NUM_CLIENTS;
    ObStatus status = OB_STATUS_BUSY;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client || !Callback) 
    {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE) 
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
#endif
	ASSERT(Client && Callback);

    if (PBAP(clientCallback) == 0 || PBAP(clientCallback) == Callback)
    {
        PBAP(clientCallback) = Callback;
        
        for (i = PBAP_NUM_CLIENTS; i > 0; i--) 
        {
            if (PBAP(client)[i-1] == Client) 
            {
                status = BT_STATUS_IN_USE;
                goto Error;
            } 
            else if (PBAP(client)[i-1] == 0) 
            {
                j = i - 1;
            }
        }
    }
    else 
    {
        status = BT_STATUS_INVALID_PARM;
        goto Error;
    }

	OS_MemSet((U8 *)Client, 0, sizeof(PbapClientSession));
	
    if (j < PBAP_NUM_CLIENTS)
	{
		Client->cApp.type = GOEP_PROFILE_PBAP;
		Client->cApp.callback = PbapGoepClientCallback;
		Client->cApp.connFlags = GOEP_NEW_CONN;
		Client->cApp.connId = 0;

		PBAP(client)[j] = Client;
		status = GOEP_RegisterClient(&Client->cApp, &PhonebookStoreFuncTable);
		BT_PBAP_LOG1(BT_PBAP_REG_CLIENT_RESULT, status);
		if (status != OB_STATUS_SUCCESS)
		{
			goto Error;
		}
	}
	else 
	{
		status = BT_STATUS_NO_RESOURCES;
		goto Error;
	}
	
Error:
    OS_UnlockObex();
    return status;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  PBAP_DeregisterClient
* DESCRIPTION
*  deregister PBAP client from GOEP
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAP_DeregisterClient(PbapClientSession *Client)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, numActiveClients;
    ObStatus status = OB_STATUS_INVALID_PARM;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!Client)
    {
        goto Error;
    }
	
    if (PBAP(initialized) != TRUE)
    {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Client);
    numActiveClients = 0;
    for (i = 0; i < PBAP_NUM_CLIENTS; i++)
    {
        if (PBAP(client)[i] == Client) 
        {
            PBAP(client)[i] = 0;
            status = BT_STATUS_SUCCESS;
        }
        if (PBAP(client)[i] != 0)
        {
            numActiveClients++;
        }
    }
	
    if (numActiveClients == 0) 
    {
        PBAP(clientCallback) = 0;
    }
	
    status = GOEP_DeregisterClient(&Client->cApp);
	BT_PBAP_LOG1(BT_PBAP_DEREG_CLIENT_RESULT, status);
Error:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/*****************************************************************************
* FUNCTION
*  PBAPC_Connect
* DESCRIPTION
*  Connect with PBAP Server
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Connect(PbapClientSession *Client, BD_ADDR *addr)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status = OB_STATUS_FAILED;
	GoepClientApp *ClientApp = NULL;
	ObexTpAddr btAddr;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_LockObex();

	ClientApp = &(Client->cApp);
	btAddr.type = OBEX_TP_BLUETOOTH;
	OS_MemCopy((U8*)&(btAddr.proto.bt.addr), (U8*)addr, sizeof(BD_ADDR));
	btAddr.proto.bt.uuid = SC_PBAP_SERVER;

	status = GOEP_TpConnect(ClientApp, &btAddr);

	if (status == OB_STATUS_PENDING)
	{
		Client->connState = PBAP_STATE_TP_CONNECTING;
	}
	else if (status == OB_STATUS_SUCCESS)
	{
		status = pbapc_obex_connect(ClientApp, NULL);

		if (status == OB_STATUS_PENDING)
		{
			Client->connState = PBAP_STATE_OBEX_CONNECTING;
		}
	}
	
	OS_UnlockObex();

	return status;
}

/*****************************************************************************
* FUNCTION
*  pbapc_obex_connect
* DESCRIPTION
*  Create OBEX connection
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus pbapc_obex_connect(GoepClientApp *Client, ObexAuthResponse* authrsp)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status = OB_STATUS_FAILED;
	
	GoepConnectReq *connectReq = &Client->connect_req;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_LockObex();

	OS_MemSet((U8*)connectReq, 0, sizeof(GoepConnectReq));

	connectReq->target = (U8*)PbapUuid;
	connectReq->targetLen = 16;
	connectReq->mru = PBAP_MRU_SIZE;

	if (authrsp)
	{
		connectReq->response = authrsp;
	}

	status = GOEP_Connect(Client, connectReq);

	if (status != OB_STATUS_PENDING)
	{
		GOEP_TpDisconnect(Client);
	}

	BT_PBAP_LOG1(BT_PBAP_GOEP_CONNECT_RETURN, status);
	
	OS_UnlockObex();

	return status;
}

/*****************************************************************************
* FUNCTION
*  PBAPC_Set_Path
* DESCRIPTION
*  Pbap client set folder path function
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Set_Path(PbapClientSession *client, PbapClientOperData *pbapData)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status = OB_STATUS_FAILED;
	GoepFolderReq req;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	OS_LockObex();

	OS_MemSet((U8*)&req, 0x0, sizeof(GoepFolderReq));

	switch (pbapData->pathType)
	{
	case PBAP_ROOT_FOLDER:
		req.reset = TRUE;
		req.flags = OSPF_DONT_CREATE;
		break;
	case PBAP_BACK_FOLDER:
		req.reset = FALSE;
		req.flags = (OSPF_DONT_CREATE|OSPF_BACKUP);
		req.name = (GoepUniType*)pbapData->name;
		break;
	case PBAP_FORWARD_FOLDER:
		req.reset = FALSE;
		req.flags = OSPF_DONT_CREATE;
		req.name = (GoepUniType*)pbapData->name;
		break;
	default:
		ASSERT(0);
		break;
	}

	status = GOEP_SetFolder(&(client->cApp), &req);

	if (status == OB_STATUS_PENDING)
	{
		client->connState = PBAP_STATE_SETTING_PATH;
	}

	BT_PBAP_LOG1(BT_PBAP_GOEP_SETPATH_RETURN, status);
	
	OS_UnlockObex();

	return status;	
}

/*****************************************************************************
* FUNCTION
*  PBAPC_Pull_Entry
* DESCRIPTION
*  Pbap client function used to get vCard entry
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Pull_Entry(PbapClientSession *client, PbapClientOperData *pbapData)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status				= OB_STATUS_FAILED;
	GoepObjectReq pullReq;
	PbapObStoreHandle object;
	U16 i = 0;
	U16 j = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_PBAP_LOG(BT_PBAP_ENTRY_PULL_ENTRY);
	
	OS_LockObex();

	client->object = PBAPOBS_New(&client->cApp);
	object = client->object;
	ASSERT(object);

	OS_MemSet((U8*)&pullReq, 0, sizeof(GoepObjectReq));
	pullReq.name = (GoepUniType *)pbapData->name;
	pullReq.type = (U8*)VCARD_OBJECT_TYPE;
	pullReq.object = object;

	OS_MemSet((U8*)client->appParms, 0, PBAP_MAX_APP_PARMS_LEN);

	client->appParms[i++] = PBAP_TAG_FILTER;
	client->appParms[i++] = PBAP_FILTER_SIZE;
	
	for (j = PBAP_FILTER_SIZE; j > 0; j--)
	{
		client->appParms[i++] = pbapData->filter.byte[j-1];
	}

	client->appParms[i++] = PBAP_TAG_FORMAT;
	client->appParms[i++] = 1;
	client->appParms[i++] = pbapData->format;

	if (GOEP_ClientQueueHeader(&(client->cApp), OBEXH_APP_PARAMS, client->appParms, i) == TRUE)
	{
		status = GOEP_Pull(&(client->cApp), &pullReq, FALSE);
		BT_PBAP_LOG1(BT_PBAP_GOEP_PULL_RETURN, status);
	}
	else
	{
		BT_PBAP_LOG1(BT_PBAP_QUEUE_HEADER_RETURN, FALSE);
	}

	if (status == BT_STATUS_PENDING)
	{
		client->connState = PBAP_STATE_PULLING;
	}
	
	OS_UnlockObex();

	return status;

}

/*****************************************************************************
* FUNCTION
*  PBAPC_Pull_vCard_Listing
* DESCRIPTION
*  PBAP client function used to get vCard listing
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Pull_vCard_Listing(PbapClientSession *client, PbapClientOperData *pbapData)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status				= OB_STATUS_FAILED;
	GoepObjectReq pullReq;
	PbapObStoreHandle object;
	U16 i = 0;
	U16 j = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_PBAP_LOG(BT_PBAP_ENTRY_PULL_LISTING);
	
	OS_LockObex();

	client->object = PBAPOBS_New(&client->cApp);
	object = client->object;
	ASSERT(object);

	OS_MemSet((U8*)&pullReq, 0, sizeof(GoepObjectReq));
	pullReq.name = (GoepUniType *)pbapData->name;
	pullReq.type = (U8*)VCARD_LISTING_OBJECT_TYPE;
	pullReq.object = object;

	OS_MemSet((U8*)client->appParms, 0, PBAP_MAX_APP_PARMS_LEN);

	client->appParms[i++] = PBAP_TAG_ORDER;
	client->appParms[i++] = 1;
	client->appParms[i++] = pbapData->order;

	if ( pbapData->searchValueLen > 0 )
	{
    	/*	 searchValueLen sholudn't larger than (PBAP_MAX_APP_PARMS_LEN - (Order header:3 + SearchAttribute header:3
    	*	+ MaxListCount header:4 + ListStartOffset header:4 + SearchValue header(tag + len):2 ))
    	*	If APP wants to set this parameter, PBAP_MAX_APP_PARMS_LEN should be checked if the size is enough.
    	*/
    	ASSERT(pbapData->searchValueLen <= (PBAP_MAX_APP_PARMS_LEN - (3 + 3 + 4 + 4 + 2)));
    	/* Could ignore the SearchValue header directly when searchValueLen is 0 ? */
    	client->appParms[i++] = PBAP_TAG_SEARCH_VALUE;
    	client->appParms[i++] = pbapData->searchValueLen;

		OS_MemCopy(&client->appParms[i], (U8*)(pbapData->searchValue), pbapData->searchValueLen);
		i += pbapData->searchValueLen;
	}
	
	client->appParms[i++] = PBAP_TAG_SEARCH_ATTRIB;
	client->appParms[i++] = 1;
	client->appParms[i++] = pbapData->searchAttrib;

	client->appParms[i++] = PBAP_TAG_MAX_LIST_COUNT;
	client->appParms[i++] = 2;
	StoreBE16(&client->appParms[i], pbapData->maxListCount);
	i += 2;

	client->appParms[i++] = PBAP_TAG_LIST_OFFSET;
	client->appParms[i++] = 2;
	StoreBE16(&client->appParms[i], pbapData->listStartOffset);
	i += 2;

	if (GOEP_ClientQueueHeader(&(client->cApp), OBEXH_APP_PARAMS, client->appParms, i) == TRUE)
	{
		status = GOEP_Pull(&(client->cApp), &pullReq, FALSE);
		BT_PBAP_LOG1(BT_PBAP_GOEP_PULL_RETURN, status);
	}
	else
	{
		BT_PBAP_LOG1(BT_PBAP_QUEUE_HEADER_RETURN, FALSE);
	}

	if (status == BT_STATUS_PENDING)
	{
		client->connState = PBAP_STATE_PULLING;
	}
	
	OS_UnlockObex();

	return status;
}

/*****************************************************************************
* FUNCTION
*  PBAPC_Pull_Entry
* DESCRIPTION
*  Pbap client function used to get an entire phone book
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Pull_PB_Folder(PbapClientSession *client, PbapClientOperData *pbapData)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ObStatus status				= OB_STATUS_FAILED;
	GoepObjectReq pullReq;
	PbapObStoreHandle object;
	U16 i = 0;
	U16 j = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_PBAP_LOG(BT_PBAP_ENTRY_PULL_PB_FOLDER);
	
	OS_LockObex();

	client->object = PBAPOBS_New(&client->cApp);
	object = client->object;
	ASSERT(object);

	OS_MemSet((U8*)&pullReq, 0, sizeof(GoepObjectReq));
	pullReq.name = (GoepUniType *)pbapData->name;
	pullReq.type = (U8*)PULL_PHONEBOOK_TYPE;
	pullReq.object = object;

	OS_MemSet((U8*)client->appParms, 0, PBAP_MAX_APP_PARMS_LEN);

	client->appParms[i++] = PBAP_TAG_FILTER;
	client->appParms[i++] = PBAP_FILTER_SIZE;
	
	for (j = PBAP_FILTER_SIZE; j > 0; j--)
	{
		client->appParms[i++] = pbapData->filter.byte[j-1];
	}

	client->appParms[i++] = PBAP_TAG_FORMAT;
	client->appParms[i++] = 1;
	client->appParms[i++] = pbapData->format;

	client->appParms[i++] = PBAP_TAG_MAX_LIST_COUNT;
	client->appParms[i++] = 2;
	StoreBE16(&client->appParms[i], pbapData->maxListCount);
	i += 2;

	client->appParms[i++] = PBAP_TAG_LIST_OFFSET;
	client->appParms[i++] = 2;
	StoreBE16(&client->appParms[i], pbapData->listStartOffset);
	i += 2;

	if (GOEP_ClientQueueHeader(&(client->cApp), OBEXH_APP_PARAMS, client->appParms, i) == TRUE)
	{
		status = GOEP_Pull(&(client->cApp), &pullReq, FALSE);
		BT_PBAP_LOG1(BT_PBAP_GOEP_PULL_RETURN, status);
	}
	else
	{
		BT_PBAP_LOG1(BT_PBAP_QUEUE_HEADER_RETURN, FALSE);
	}

	if (status == BT_STATUS_PENDING)
	{
		client->connState = PBAP_STATE_PULLING;
	}
	
	OS_UnlockObex();

	return status;
}

/*****************************************************************************
* FUNCTION
*  PBAPC_Abort
* DESCRIPTION
*  Abort current PBAP client operation
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Abort(PbapClientSession *client)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	ObStatus status				= OB_STATUS_FAILED;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	OS_LockObex();

	status = GOEP_ClientAbort(&client->cApp);
	
	BT_PBAP_LOG1(BT_PBAP_ABORT_STATUS, status);
	
	OS_UnlockObex();

	return status;
}

/*****************************************************************************
* FUNCTION
*  PBAPC_Disconnect
* DESCRIPTION
*  PBAP client disconnect function
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Disconnect(PbapClientSession *client)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	ObStatus status				= OB_STATUS_FAILED;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	OS_LockObex();

	BT_PBAP_LOG1(BT_PBAP_CLIENT_DISCONNECT_ENTRY, client->connState);

	switch (client->connState)
	{
	case PBAP_STATE_IDLE:
		{
			status = OB_STATUS_SUCCESS;
		}
		break;
	case PBAP_STATE_OBEX_CONNECTING:
	case PBAP_STATE_TP_CONNECTING:
		{
			status = GOEP_TpDisconnect(&client->cApp);
		}
		break;
	case PBAP_STATE_CONNECTED:
	case PBAP_STATE_SETTING_PATH:		
		{
			status = GOEP_Disconnect(&client->cApp);
			if (status != OB_STATUS_PENDING)
			{
				/* Can GOEP_TpDisconnect return OB_STATUS_SUCCESS ? */
				status = GOEP_TpDisconnect(&client->cApp);
			}
		}
		break;
	case PBAP_STATE_PULLING:	
		{
			status = GOEP_ClientAbort(&client->cApp);
		}
		break;
	case PBAP_STATE_DISCONNECTING:
		{
			status = OB_STATUS_PENDING;
		}
		break;
	default:
		{
			status = GOEP_TpDisconnect(&client->cApp);
		}
		break;
	}

	if (status == OB_STATUS_PENDING)
	{
		client->connState = PBAP_STATE_DISCONNECTING;
	}
	else if (status == OB_STATUS_SUCCESS)
	{
		client->connState = PBAP_STATE_IDLE;
	}

	BT_PBAP_LOG2(BT_PBAP_CLIENT_DISCONNECT_RESULT, status, client->connState);
	OS_UnlockObex();

	return status;

}

/*****************************************************************************
* FUNCTION
*  PBAPC_Check_Authentication
* DESCRIPTION
*  PBAP client authentication response function
* PARAMETERS
*  
* RETURNS
*  ObStatus
*****************************************************************************/
ObStatus PBAPC_Check_Authentication(PbapClientSession *client, U8 cancel)
{
	ObStatus status = OB_STATUS_FAILED;
	
	BT_PBAP_LOG2(BT_PBAP_CLIENT_CHECK_AUTHEN, client->flags, client->connState);
	
	if (client->flags & PBAP_FLAGS_CHAL_RCVD)
	{
		client->flags &= ~PBAP_FLAGS_CHAL_RCVD;

		if (!cancel)
		{
			if (client->connState != PBAP_STATE_OBEX_CONNECTING)
			{
				return BT_STATUS_FAILED;
			}

			status = pbapc_obex_connect(&client->cApp, &client->resp);
		}
		else
		{
			if (client->connState == PBAP_STATE_DISCONNECTING)
			{
				return OB_STATUS_PENDING;
			}

			status = GOEP_TpDisconnect(&client->cApp);
		}
	}

	return status;
}

#endif /* #if PBAP_NUM_CLIENTS > 0  */

#endif /* __BT_PBAP_NEW_PROFILE__ */
