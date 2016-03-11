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
 * File:
 *     $Workfile:avrcpsdp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #3 $
 *
 * Description: This file contains code for the AVRCP profile. 
 *
 * Created:     May 19, 2004
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/

#include "bttypes.h"
#include "btalloc.h"
#include "sys/avrcpi.h"
#include "sdpdb.h"
#include "sdp.h"
#include "sdap.h"

extern void App_Report(char *format,...);
/****************************************************************************
 *
 * SDP objects used to query AVRCP services.
 *
 ****************************************************************************/

static void avrcpQueryCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
void AVRCP_Connect_Timeout(EvmTimer *Timer);

static U16 avrcp_sdp_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST,
	AID_SUPPORTED_FEATURES,
    AID_ADDITIONAL_PROT_DESC_LISTS   /// optional
};
static U8 avrcp_sdp_attribute_num = 0x03;

void avrcpQueryTGCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value);
void avrcpQueryCTCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value);

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            AvrcpRegisterSdp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Registers an SDP entry.
 *
 * Return:    BT_STATUS_FAILED
 *            (Also see SDP_AddRecord)
 */
BtStatus AvrcpRegisterSdp(AvrcpChannel *Chnl)
{
    BtStatus    status;
    SdpRecord  *sdpRecord;
    U8 local_service_id = 0; /* for temportary used. */
    
    /* Configure the attributes */
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_REGISTER , Chnl->role);

    switch (Chnl->role) {
    case AVRCP_TG:
        if (AVRCP(ctChannelCount)++ > 0) 
            return BT_STATUS_SUCCESS;

        AVRCP(ctSdpRecord).num = SDPDB_GetLocalServiceAttribute(
                                                                SC_AV_REMOTE_CONTROL_TARGET,
                                                                &AVRCP(ctSdpRecord).attribs,
                                                                &local_service_id
                                                                );  
        
        sdpRecord = &AVRCP(ctSdpRecord);
		status = SDP_AddRecord(sdpRecord);
		OS_Report("[AVRCP] register TG status:%d", status);
        break;
    case AVRCP_CT:
        if (AVRCP(tgChannelCount)++ > 0) 
            return BT_STATUS_SUCCESS;

        /* Copy the SDP attributes to RAM */
        AVRCP(tgSdpRecord).num = SDPDB_GetLocalServiceAttribute(
                                                                SC_AV_REMOTE_CONTROL,
                                                                &AVRCP(tgSdpRecord).attribs,
                                                                &local_service_id
                                                                );  
        sdpRecord = &AVRCP(tgSdpRecord);
#if 0
		sdpRecord->classOfDevice = 0;
#endif
	    status = SDP_AddRecord(sdpRecord);
		OS_Report("[AVRCP] register CT status:%d", status);

        break;

    default:
        status = BT_STATUS_FAILED;
        goto fail;
    }


fail:
    return status;
}


/*---------------------------------------------------------------------------
 *            AvrcpDeregisterSdp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregisters an SDP entry.
 *
 * Return:    BT_STATUS_FAILED
 *            (Also see SDP_RemoveRecord)
 */
BtStatus AvrcpDeregisterSdp(AvrcpChannel *Chnl)
{
    BtStatus status = BT_STATUS_SUCCESS;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_DEREGISTER, Chnl->role);

    if (Chnl->role == AVRCP_CT) {
        if (--AVRCP(ctChannelCount) == 0) {
            status = SDP_RemoveRecord(&AVRCP(ctSdpRecord));
        }
    } else if (Chnl->role == AVRCP_TG) {
        if (--AVRCP(tgChannelCount) == 0) {
            status = SDP_RemoveRecord(&AVRCP(tgSdpRecord));
        }
    } else {
        status = BT_STATUS_FAILED;
    }

    return status;
}




BtStatus AvrcpCreateAclLink(BD_ADDR *Addr, CmgrHandler *Handler, CmgrCallback Callback)
{
    AvrcpChannel   *chnl = ContainingRecord(Handler, AvrcpChannel, cmgrHandler);
    BtStatus status;
       
    status = CMGR_CreateDataLink(Handler, Addr);
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_CREATEACL, status);

    if (status == BT_STATUS_SUCCESS) 
    {
#ifdef __BT_AVRCP_V14__
        status = AvrcpStartServiceQuery(chnl, BSQM_FIRST);
#else
		status = AvrcpStartMultiServiceQuery(chnl, BSQM_FIRST, chnl->role);
#endif
    }
    return status;

}

/*---------------------------------------------------------------------------
 *            Avrcp14StartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Starts the AVRCP14 SDP query.
 * 
 *
 * Return:    
 *
 */
BtStatus AvrcpStartMultiServiceQuery(AvrcpChannel *Chnl, SdpQueryMode mode, U16 local_role)
{
    BtStatus status;
    //U16      remote_role = AVRCP_CT;
    U16      remote_role = AVRCP_TG;//modified by mtk40390
    sdap_service_search_multi_attribute_struct search_pattern;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_START_QUERY, local_role);

	OS_MemSet( (U8 *)&search_pattern, 0, sizeof(sdap_service_search_multi_attribute_struct));
    if (mode == BSQM_FIRST) 
    {
        /* Fix up SDP attribute value to point to desired role */
        if (local_role == AVRCP_CT) 
        {
            remote_role = AVRCP_TG;
            search_pattern.callback = avrcpQueryTGCallback;
            search_pattern.attribute_num = 0x03;
            search_pattern.attribute_id  = avrcp_sdp_attribute_id_list;            
            
            /// clean up the search recrods
            OS_MemSet( (U8 *)&(Chnl->remoteTGSdp), 0, sizeof(Chnl->remoteTGSdp) );
        } 
        else if (local_role == AVRCP_TG) 
        {
            remote_role = AVRCP_CT; 
            search_pattern.callback = avrcpQueryCTCallback;
            search_pattern.attribute_num = 0x03;
            search_pattern.attribute_id  = avrcp_sdp_attribute_id_list;
            
            /// clean up the search recrods
            OS_MemSet( (U8 *)&(Chnl->remoteCTSdp), 0, sizeof(Chnl->remoteCTSdp));
        } 
        else 
        {
            return BT_STATUS_FAILED;
        }
    }

    /* Reset the query flag */
    search_pattern.rm = (Chnl->cmgrHandler.bdc->link);
    search_pattern.sqt = &Chnl->sdpQueryToken;
    search_pattern.uuid = remote_role;
    
    status =  SDAP_ServiceSearchMultipleAttribute(search_pattern);
    
    return status;
}


/*---------------------------------------------------------------------------
 *            AvrcpStartServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Starts the SDP query.
 *            
 *
 * Return:    
 *
 */
BtStatus AvrcpStartServiceQuery(AvrcpChannel *Chnl, SdpQueryMode mode)
{
    BtStatus status;
    U16      role = 0;
    sdap_service_search_struct search_pattern;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_START_QUERY, Chnl->role);

    if (mode == BSQM_FIRST) 
    {
        /* Fix up SDP attribute value to point to desired role */
        if (Chnl->role == AVRCP_CT) 
        {
            role = AVRCP_TG;
        } 
        else if (Chnl->role == AVRCP_TG) 
        {
            role = AVRCP_CT;
        } 
        else 
        {
            return BT_STATUS_FAILED;
        }
        Chnl->sdpQueryToken.uuid = role;
        Chnl->sdpQueryToken.mode = BSPM_BEGINNING;
    }

    search_pattern.rm = (Chnl->cmgrHandler.bdc->link);
    search_pattern.sqt = &Chnl->sdpQueryToken;
    search_pattern.uuid = role;    
    search_pattern.callback = avrcpQueryCallback;    

    status = SDAP_ServiceSearch(search_pattern);

    return status;
}

/*************************************************************************
* FUNCTION                                                            
*	avrcpQueryCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*   Modified by MTK Dlight mtk01239
*
* PARAMETERS
*	sqt  : SdpQueryToken contains information such as remote device information. uuid...
*	result  : The service search result.
*      serv_chn: It is not used in AVRCP profile
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/

static void avrcpQueryCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
    SdpQueryToken *token;
    BtRemoteDevice *remDev;
    AvrcpChannel *chnl = ContainingRecord(
                                                                    sqt, 
                                                                    AvrcpChannel, 
                                                                    sdpQueryToken
                                                                    );

    (void)token;
    token  = sqt;
    if(chnl->cmgrHandler.bdc ==0)
    {
        /* 
            The remote side connects our AVRCP after we send avrcp connect req.
            We will reject the remote side avrcp connect req but will get a disconnect ind.
            The Avrcp connect req from our side will think the connect requestion is failed and 
            remove the cmgr and rem at the same time.
        */
        if (sqt->rm !=0)
            sqt->rm->discFlag = 1;
        return;
    }

    remDev = chnl->cmgrHandler.bdc->link;
#if 1    
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_QUERY_CB, result);
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
//            status = AVCTP_Connect(&chnl->chnl, remDev);
            if(chnl->conntimer.func!=0)
                EVM_CancelTimer(&chnl->conntimer);
            chnl->conntimer.func = AVRCP_Connect_Timeout;
            chnl->conntimer.context = remDev;
            EVM_StartTimer(&chnl->conntimer, 20);
            /* AVRCP implementor shall handle the status report, ESI not handle it!*/                
            break;
        case BT_STATUS_CONNECTION_FAILED:
            break;
        case BT_STATUS_FAILED:
            AvrcpReportFailedConnect(chnl);
            /* Do we not indicate this back somehow? */
            break;
        case BT_STATUS_NOSERVICES:
            AvrcpReportFailedConnect(chnl);
            break;
    }
#endif

//    AVCTP_Connect(&chnl->chnl, remDev);

}


/*************************************************************************
* FUNCTION                                                            
*	avrcpQueryTGCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*
* PARAMETERS
*	sqt  : SdpQueryToken contains information such as remote device information. uuid...
*	result  : The service search result.
*   serv_chn: It is not used in AVRCP profile
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/
void avrcpQueryTGCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value)
{
    SdpQueryToken *token;
    BtRemoteDevice *remDev;
    AvrcpChannel *chnl = ContainingRecord(
                                                                    sqt, 
                                                                    AvrcpChannel, 
                                                                    sdpQueryToken
                                                                    );
    (void)token;
    token  = sqt;

    if ( chnl == NULL )
        return;
    
    if(chnl->cmgrHandler.bdc ==0)
    {
        /* 
            The remote side connects our AVRCP after we send avrcp connect req.
            We will reject the remote side avrcp connect req but will get a disconnect ind.
            The Avrcp connect req from our side will think the connect requestion is failed and 
            remove the cmgr and rem at the same time.
        */
        if (sqt->rm !=0)
            sqt->rm->discFlag = 1;
        return;
    }

    if( chnl ){
    	/// has do the search 
    	chnl->remoteTGSdp.tgValid = TRUE;
    }
    remDev = chnl->cmgrHandler.bdc->link;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_TG_CALLBACK , result, attribute_index);
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
        	{
        		/// remote has a TG record
        		chnl->remoteTGSdp.tgAvailable= TRUE;
        		
	        	switch( avrcp_sdp_attribute_id_list[attribute_index] ){
	        		case AID_PROTOCOL_DESC_LIST:
	        			{
	        				/// update the result
	        				chnl->remoteTGSdp.tgVersion = BEtoHost16(attribute_value);
	        			}
	        			break;
					case AID_SUPPORTED_FEATURES:
						{
	        				/// update the result
	        				chnl->remoteTGSdp.tgFeatureFlag = BEtoHost16(attribute_value);
	        				
	        				/// try to connect
							if( 2 == avrcp_sdp_attribute_num ){
								if(chnl->conntimer.func!=0)
									EVM_CancelTimer(&chnl->conntimer);
								chnl->conntimer.func = AVRCP_Connect_Timeout;
								chnl->conntimer.context = remDev;
								EVM_StartTimer(&chnl->conntimer, 20);
							}
	        			}
	        			break;
					case AID_ADDITIONAL_PROT_DESC_LISTS:
						{
							chnl->remoteTGSdp.tgSupportBrowse = BEtoHost16(attribute_value);

							kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_TG_INFO , chnl->remoteTGSdp.tgVersion, chnl->remoteTGSdp.tgFeatureFlag, chnl->remoteTGSdp.tgSupportBrowse);

							if(chnl->conntimer.func!=0)
								EVM_CancelTimer(&chnl->conntimer);
							chnl->conntimer.func = AVRCP_Connect_Timeout;
							chnl->conntimer.context = remDev;
							EVM_StartTimer(&chnl->conntimer, 20);
						}
						break;
	        	}
	        }
            /* AVRCP implementor shall handle the status report, ESI not handle it!*/                
            break;
        case BT_STATUS_CONNECTION_FAILED:
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:
        	/// NO CT or TG record available => report fail
        	/// One of CT or TG is available => try to connect
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_TG_INFO , chnl->remoteTGSdp.tgAvailable, chnl->remoteTGSdp.tgVersion, chnl->remoteTGSdp.tgFeatureFlag, chnl->remoteTGSdp.tgSupportBrowse);

			if(attribute_index == 0){
				//fail to query remote's avrcp record, still connect it
				if(chnl->conntimer.func != 0)
					EVM_CancelTimer(&chnl->conntimer);
				chnl->conntimer.func = AVRCP_Connect_Timeout;
	            chnl->conntimer.context = remDev;
	            EVM_StartTimer(&chnl->conntimer, 20);
			}else if( FALSE == chnl->remoteCTSdp.ctAvailable && FALSE == chnl->remoteTGSdp.tgAvailable ){
				// search remote's CT role and try to connect it
                //AvrcpStartMultiServiceQuery(chnl, BSQM_FIRST, AVRCP_TG); // local is TG
                AvrcpStartMultiServiceQuery(chnl, BSQM_FIRST, AVRCP_CT); //local is CT
            }else{
            	/// After a TG connect
	            if(chnl->conntimer.func!=0)
	                EVM_CancelTimer(&chnl->conntimer);
	            chnl->conntimer.func = AVRCP_Connect_Timeout;
	            chnl->conntimer.context = remDev;
	            EVM_StartTimer(&chnl->conntimer, 20);
            }
            break;
    }
}

/*************************************************************************
* FUNCTION                                                            
*	avrcpQueryCTCallback
* DESCRIPTION                                                           
*   The function is called by SDAP to indicate the profiles the search result
*   Modified by MTK Dlight mtk01239
*
* PARAMETERS
*	sqt  : SdpQueryToken contains information such as remote device information. uuid...
*	result  : The service search result.
*      serv_chn: It is not used in AVRCP profile
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/
void avrcpQueryCTCallback(SdpQueryToken *sqt, U8 result, U8 attribute_index, U8 *attribute_value)
{
    SdpQueryToken *token;
    BtRemoteDevice *remDev;
    AvrcpChannel *chnl = ContainingRecord(
                                                                    sqt, 
                                                                    AvrcpChannel, 
                                                                    sdpQueryToken
                                                                    );

    (void)token;
    token  = sqt;

    if ( chnl == NULL )
        return;
    
    if(chnl->cmgrHandler.bdc ==0)
    {
        /* 
            The remote side connects our AVRCP after we send avrcp connect req.
            We will reject the remote side avrcp connect req but will get a disconnect ind.
            The Avrcp connect req from our side will think the connect requestion is failed and 
            remove the cmgr and rem at the same time.
        */
        if (sqt->rm !=0)
            sqt->rm->discFlag = 1;
        return;
    }

    remDev = chnl->cmgrHandler.bdc->link;
    
    if( chnl ){
    	/// has do the search 
    	chnl->remoteCTSdp.ctValid = TRUE;
    }
    
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_CT_CALLBACK , result, attribute_index);
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
        	{
        		/// remote has a TG record
        		chnl->remoteCTSdp.ctAvailable= TRUE;
        		
	        	switch( avrcp_sdp_attribute_id_list[attribute_index] ){
	        		case AID_PROTOCOL_DESC_LIST:
	        			{
	        				/// update the result
	        				chnl->remoteCTSdp.ctVersion = BEtoHost16(attribute_value);

	        			}
	        			break;
					case AID_SUPPORTED_FEATURES:
	        			{
	        				/// update the result
	        				chnl->remoteCTSdp.ctFeatureFlag = BEtoHost16(attribute_value);

							/// try to connect
							if( 2 == avrcp_sdp_attribute_num ){
								if(chnl->conntimer.func!=0)
									EVM_CancelTimer(&chnl->conntimer);
								chnl->conntimer.func = AVRCP_Connect_Timeout;
								chnl->conntimer.context = remDev;
								EVM_StartTimer(&chnl->conntimer, 20);
							}
	        			}
	        			break;

					case AID_ADDITIONAL_PROT_DESC_LISTS:
						{
							chnl->remoteCTSdp.ctSupportBrowse = BEtoHost16(attribute_value);
							kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_CT_INFO , chnl->remoteCTSdp.ctAvailable, chnl->remoteCTSdp.ctVersion, chnl->remoteCTSdp.ctFeatureFlag, chnl->remoteCTSdp.ctSupportBrowse);

							if(chnl->conntimer.func!=0)
								EVM_CancelTimer(&chnl->conntimer);
							chnl->conntimer.func = AVRCP_Connect_Timeout;
							chnl->conntimer.context = remDev;
							EVM_StartTimer(&chnl->conntimer, 20);
						}
						break;
	        	}
	        }             
            break;
        case BT_STATUS_CONNECTION_FAILED:
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:
        	{
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_CT_INFO , chnl->remoteCTSdp.ctAvailable, chnl->remoteCTSdp.ctVersion, chnl->remoteCTSdp.ctFeatureFlag, chnl->remoteCTSdp.ctSupportBrowse);

	        	/// NO TG record available => search CT
	        	/// One of CT or TG is available => try to conne	ct
	        	if( FALSE == chnl->remoteCTSdp.ctAvailable && FALSE == chnl->remoteTGSdp.tgAvailable ){
	            	AvrcpReportFailedConnect(chnl);
	            }else{
	            	/// After a TG connect
		            if(chnl->conntimer.func!=0)
		                EVM_CancelTimer(&chnl->conntimer);
		            chnl->conntimer.func = AVRCP_Connect_Timeout;
		            chnl->conntimer.context = remDev;
		            EVM_StartTimer(&chnl->conntimer, 20);
	            }
        	}
            break;
    }

}

/*************************************************************************
* FUNCTION                                                            
*	AVRCP_Connect_Timeout
* DESCRIPTION                                                           
*   The function is called by Timer
*   It is used to delay the connect action
*
* PARAMETERS
*   Timer
* RETURNS
*	None
* LOCAL AFFECTED
*   
*************************************************************************/
void AVRCP_Connect_Timeout(EvmTimer *Timer)
{
    AvrcpChannel *channel;
    BtStatus status = BT_STATUS_FAILED;

    channel = ContainingRecord(Timer, AvrcpChannel, conntimer);
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_CONNECT_TIMEOUT, channel->chnl.channelType);

    status = AVCTP_Connect(&channel->chnl, (BtRemoteDevice *)Timer->context);
    OS_Report("[AVRCP] status:%d", status);
    if( status != BT_STATUS_PENDING ){
        AvrcpReportFailedConnect(channel);
    }
    
}

