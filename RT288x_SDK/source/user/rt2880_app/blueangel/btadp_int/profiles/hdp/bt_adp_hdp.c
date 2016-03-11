
#ifdef __BT_HDP_PROFILE__


#include "bt_adp_hdp.h"
#include "hdp.h"
#include "bt_mmi.h"
#include <sys/socket.h>
#include <errno.h>
#include "cutils/sockets.h"
#include "bluetooth_hdp_struct.h"
#include "bt_message.h"

hdp_adp_context_struct hdp_adp_context;
hdp_adp_channel_struct hdp_channel[HDP_ADP_MAX_CHANNEL_NUM];
static U16 currentMark = 0;
extern g_serverSocket;

#define DEBUG FALSE
/*****************************************************************************
 * FUNCTION
 *  hdp_send_msg
 * DESCRIPTION
 *  send message to external task.
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void hdp_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

void hdp_send_msg_ind(U16 watermark, void *data, U32 size)
{
	struct sockaddr_un instanceAddr;
	socklen_t   instanceAddrLen;
	char destAddr[HDP_ADP_INSTANCE_SOCKET_ADDR_LEN];
	U32 ret;

	OS_Report("[HDP]hdp_adp_send_data: watermark->%d, size->%d", watermark, size);

	OS_MemSet(destAddr, 0x0, HDP_ADP_INSTANCE_SOCKET_ADDR_LEN);
	sprintf(destAddr, "%s%d", BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX, watermark);
	socket_make_sockaddr_un(destAddr, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&instanceAddr, &instanceAddrLen);
	ret = sendto(g_serverSocket, data, size, 0,(const struct sockaddr*)&instanceAddr, instanceAddrLen);
	if (ret < 0)
	{
		OS_Report("fail to send data: %s", destAddr);
	}       
}

//TRUE: the data is from HDP instance
//FALSE: the data is not from HDP intance

BOOL hdp_adp_check_socket_address(const char *source)
{
	if (NULL != source && 
		!OS_StrnCmp(source, BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX, OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX)))
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}

}

//TRUE: the data is from HDP instance
//FALSE: the data is not from HDP intance
BOOL hdp_adp_get_data_channel(const char *source, U16 *watermark)
{
 	if (NULL != source &&
		!OS_StrnCmp(source, BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX, OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX)))
	{
		*watermark = atoi(source + OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX));
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

void hdp_print_channel()
	{
		U8 index = 0;

		if (!DEBUG)
		{
			return;
		}
		OS_Report("hdp_print_channel");
		hdp_adp_channel_struct * channel = NULL;
		for (index = 0; index < HDP_ADP_MAX_CHANNEL_NUM; index++)
		{
			channel = &hdp_channel[index];
			OS_Report("index :%d", index);
			OS_Report("state :%d", channel->state);
			OS_Report("BDARR: %x %x %x", channel->bdaddr.lap, channel->bdaddr.uap, channel->bdaddr.nap);
			OS_Report("l2cap id :%d", channel->l2capChnId );
			OS_Report("mdepId :%d", channel->mdepId);
			OS_Report("mdlId :%d", channel->mdlId);
			
		}
	}

BOOL hdp_deQueue_data(U8 *source, U8 *data, U32 size)
{
	struct sockaddr_un addr;
	socklen_t   addrLen;
	U16 watermark;
	BOOL ret = FALSE;
	
	OS_Report("hdp_deQueue_data: source is %s, data size is %d", source, size);
	if(hdp_adp_get_data_channel(source, &watermark))
    {
    	hdp_adp_send_data(watermark , data, size);
		ret = TRUE;
    }
	else
	{
		OS_Report("[HDP][ADP]fail to get l2cap channel id");
	}
    return ret;
}


hdp_adp_instance_struct *hdp_util_find_available_instance() 
{
	U8 index = 0;
	hdp_adp_instance_struct * instance = NULL;
	for (index = 0; index < HDP_ADP_MAX_INSTANCE_NUM; index++)
	{
		instance = &(hdp_adp_context.instance[index]);
		if (FALSE == instance->single)
		{
			return instance;
		}
	}
	return NULL;
}

BOOL hdp_util_cmp_device_addr(bt_addr_struct *bt_addr1, bt_addr_struct *bt_addr2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL result;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    

    if( (bt_addr1->lap & 0x00FFFFFF) == (bt_addr2->lap & 0x00FFFFFF) &&
        bt_addr1->uap == bt_addr2->uap && bt_addr1->nap == bt_addr2->nap)
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}

hdp_adp_instance_struct *hdp_util_find_instance(U8 mdepId) 
{
	U8 index = 0;
	hdp_adp_instance_struct * instance = NULL;
	for (index = 0; index < HDP_ADP_MAX_INSTANCE_NUM; index++)
	{
		instance = &(hdp_adp_context.instance[index]);
		if (TRUE == instance->single &&
			mdepId == instance->mdepId)
		{
			return instance;
		}
	}
	return NULL;
}
BOOL hdp_util_instance_exist(U8 mdepId) 
{
	U8 index = 0;
	hdp_adp_instance_struct * instance = NULL;
	for (index = 0; index < HDP_ADP_MAX_INSTANCE_NUM; index++)
	{
		instance = &hdp_adp_context.instance[index];
		if (TRUE == instance->single && 
			mdepId == instance->mdepId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

hdp_adp_channel_struct *hdp_util_find_available_channel() 
{
	U8 index = 0;
	hdp_adp_channel_struct * channel = NULL;
	for (index = 0; index < HDP_ADP_MAX_CHANNEL_NUM; index++)
	{
		channel = &hdp_channel[index];
		if (FALSE == channel->inUse)
		{
			return channel;
		}
	}
	return NULL;
}
hdp_adp_channel_struct *hdp_util_find_channel_by_mdl_id(bt_addr_struct *addr, U16 mdlId) 
{
	hdp_adp_channel_struct *header = &hdp_adp_context.channel;
	hdp_adp_channel_struct *next = header;

	do {
		next = GetNextNode(&next->node);
		if (hdp_util_cmp_device_addr(&next->bdaddr, addr) && 
			next->mdlId == mdlId)
		{
			return next;
		}
	} while (next != header);
	return NULL;
}

hdp_adp_channel_struct *hdp_util_find_channel_by_state(bt_addr_struct *addr, U8 state) 
{
	hdp_adp_channel_struct *header = &hdp_adp_context.channel;
	hdp_adp_channel_struct *next = header;

	do {
		next = GetNextNode(&next->node);
		if (next->state != HDP_ADP_CHANNEL_IDLE &&
			hdp_util_cmp_device_addr(&next->bdaddr, addr) && 
			next->state == state)
		{
			return next;
		}
	} while (next != header);
	return NULL;
}

hdp_adp_channel_struct *hdp_util_find_channel_by_l2cap_Id(U16 l2capId) 
{
	hdp_adp_channel_struct *header = &hdp_adp_context.channel;
	hdp_adp_channel_struct *next = header;

	do {
		next = GetNextNode(&next->node);
		if (next->state != HDP_ADP_CHANNEL_IDLE &&
			next->l2capChnId == l2capId)
		{
			return next;
		}
	} while (next != header);
	return NULL;
}

hdp_adp_channel_struct *hdp_util_find_channel_by_water_mark(U16 watermark) 
{
	hdp_adp_channel_struct *header = &hdp_adp_context.channel;
	hdp_adp_channel_struct *next = header;

	do {
		next = GetNextNode(&next->node);
		if (next->state != HDP_ADP_CHANNEL_IDLE &&
			next->waterMark == watermark)
		{
			return next;
		}
	} while (next != header);
	return NULL;
}




hdp_adp_channel_struct *hdp_util_find_main_channel(bt_addr_struct *addr) 
{
	hdp_adp_channel_struct *header = &hdp_adp_context.channel;
	hdp_adp_channel_struct *next = header;

	do {
		next = GetNextNode(&next->node);
		if (hdp_util_cmp_device_addr(&next->bdaddr, addr) && 
			next->mainChn == TRUE)
		{
			return next;
		}
	} while (next != header);
	return NULL;
}


void hdp_util_add_channel(hdp_adp_channel_struct * channel) 
{
	if (!IsNodeOnList(&hdp_adp_context.channel, channel)) 
	{
	InsertTailList(&hdp_adp_context.channel, channel);
	channel->inUse = TRUE;
        }
}
void hdp_util_remove_channel(hdp_adp_channel_struct * channel) 
{
	if (IsNodeOnList(&hdp_adp_context.channel, channel))
	{
		RemoveEntryList(channel);
	}
	channel->inUse = FALSE;
}

U16 hdp_util_get_channel_water_mark()
{
	U8 index = 0;
	BOOL inUse;
	do {
		currentMark++;
		inUse = FALSE;

		/*Notes: some inactive channels that may be reconnected have to considered*/
		for (index = 0; index < HDP_ADP_MAX_CHANNEL_NUM; index ++)
		{
			if(hdp_channel[index].waterMark == currentMark)	
			{
				inUse = TRUE;
				break;
			}
		}
	} while (inUse);
	return currentMark;
}


bt_hdp_status hdp_util_convert_status(BtStatus status)
{
    switch (status)
    {
    case BT_STATUS_SUCCESS:
        return BT_HDP_SUCCESS;
		
    case BT_STATUS_BUSY:
    case BT_STATUS_IN_USE:
    case BT_STATUS_IN_PROGRESS:
    case BT_STATUS_PENDING:
        return BT_HDP_FAIL_BUSY;
		
    case BT_STATUS_TIMEOUT:
        return BT_HDP_FAIL_TIMEOUT;
		
    case BT_STATUS_RESTRICTED:
    case BT_STATUS_BLOCK_LIST:
    case BT_STATUS_CHIP_REASON:
        return BT_HDP_FAIL_NOT_ALLOW;
		
    case BT_STATUS_REMOTE_REJECT:
        return BT_HDP_FAIL_REMOTE_REJECT;
		
    default:
        return BT_HDP_FAIL;
    }        
}


void hdp_adp_register_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_register_instance_req_struct *msg = (bt_hdp_register_instance_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_register_instance_cnf_struct *cnf = 
        (bt_hdp_register_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_register_instance_cnf_struct), TD_UL | TD_RESET);
	hdp_adp_instance_struct *instance = NULL;
	U8 id = HDP_INVALID_MDEP_ID;
	HdpConfig config;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	
	instance = hdp_util_find_available_instance();
	if (NULL == instance)
	{
		cnf->result = BT_HDP_FAIL_NO_RESOURCE;
		goto exit;
	}
	OS_MemSet(&config , 0x0, sizeof(HdpConfig));
	config.role = msg->role;
	config.channelType =  msg->channelType;
	config.datatype = msg->dataType;
	if (OS_StrLen(msg->description) > 0)
	{
		OS_StrnCpy(config.description, msg->description, HDP_INSTNACE_DESC_MAX_LEN);
	}
	id = hdp_register(&config);
	if (HDP_INVALID_MDEP_ID != id)
	{
		instance->single = TRUE;
		instance->mdepId = id;
		
		cnf->result = BT_HDP_SUCCESS;
		cnf->mdepId = id;		
	}
	else
	{
		cnf->result = BT_HDP_FAIL;
		OS_Report("fail to register instance");
	}
exit:
	hdp_send_msg(MSG_ID_BT_HDP_REGISTER_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
	
}
void hdp_adp_deregister_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_deregister_instance_req_struct *msg = (bt_hdp_deregister_instance_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_deregister_instance_cnf_struct *cnf = 
        (bt_hdp_register_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_deregister_instance_cnf_struct), TD_UL | TD_RESET);
	hdp_adp_instance_struct *instance = hdp_util_find_instance(msg->mdepId);
	BtStatus ret;
	HdpConfig config;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	if (NULL == instance)
	{
		cnf->result = BT_HDP_FAIL;
		goto exit;
	}
	ret = hdp_deregister(msg->mdepId);
	if (BT_STATUS_SUCCESS == ret)
	{
		instance->single = FALSE;
		OS_Report("[HDP]deregister instance: %d", ret);
	}
	else
	{
		OS_Report("[HDP]fail to deregister instance: %d", ret);
	}
	cnf->result = hdp_util_convert_status(ret);
exit:
	cnf->mdepId = msg->mdepId;
	hdp_send_msg(MSG_ID_BT_HDP_DEREGISTER_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_connect(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_connect_req_struct *msg = (bt_hdp_connect_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_connect_cnf_struct *cnf = NULL;        
	BD_ADDR bdaddr;
	hdp_adp_channel_struct *channel = NULL;	
	BtStatus ret;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	if (FALSE == hdp_util_instance_exist(msg->mdepId))
	{
		ret = BT_STATUS_NOT_FOUND;
		OS_Report("[HDP][ADP]Err No instance is found");
		goto exit;
	}
	
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);

	//check channel state
/*	if (hdp_util_find_main_channel(&msg->bdaddr) != NULL)
	{
		//the main channel already exist
		ret = BT_STATUS_SUCCESS;
		goto exit;
	}*/
	channel = hdp_util_find_available_channel();	
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP]Err No available channel is found");
		ret = BT_STATUS_NO_RESOURCES;
		goto exit;
	}
	OS_MemCopy(&channel->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
	channel->mdepId = msg->mdepId;
	channel->state = HDP_ADP_CHANNEL_CONNECTING;
	channel->index = msg->index;
	channel->waterMark = hdp_util_get_channel_water_mark();
	hdp_util_add_channel(channel);
	//debug
	hdp_print_channel();
	
	ret = hdp_open_channel(msg->mdepId, &bdaddr, msg->config);
	if (BT_STATUS_PENDING == ret)
	{
		return;
	} 
	else if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
	else
	{
		hdp_util_remove_channel(channel);
		OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	//debug
	hdp_print_channel();
exit:
	cnf = (bt_hdp_connect_cnf_struct *) construct_local_para(sizeof(bt_hdp_connect_cnf_struct), TD_UL | TD_RESET);
	cnf->result = hdp_util_convert_status(ret);
	cnf->mdepId = msg->mdepId;
	cnf->index = msg->index;
	OS_MemCopy(&cnf->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}
void hdp_adp_disconnect(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_disconnect_req_struct *msg = (bt_hdp_disconnect_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_disconnect_cnf_struct *cnf = NULL;     
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	if (NULL == channel || channel->state == HDP_ADP_CHANNEL_IDLE)
	{
		ret = BT_STATUS_FAILED;
		goto exit;
	}
	if (channel->state == HDP_ADP_CHANNEL_DISCONNECTING)
	{
		ret = BT_STATUS_IN_PROGRESS;
		goto exit;
	}
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
	ret = hdp_close_channel(&bdaddr, msg->mdlId);
	if (BT_STATUS_PENDING == ret)
	{
		channel->state = HDP_ADP_CHANNEL_DISCONNECTING;
		return;		
	} 
	else if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_IDLE;
		hdp_util_remove_channel(channel);
		OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	else
	{
		OS_Report("[HDP][ADP]fail to disconnect with remote device");
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
exit:
	cnf = (bt_hdp_disconnect_cnf_struct *) construct_local_para(sizeof(bt_hdp_disconnect_cnf_struct), TD_UL | TD_RESET);
	cnf->result = hdp_util_convert_status(ret);
	cnf->mdlId = msg->mdlId;
	cnf->index = msg->index;
	OS_MemCopy(&cnf->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);	

}

void hdp_adp_remove_connection(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_remove_connection_req_struct *msg = (bt_hdp_remove_connection_req_struct *) ilm_ptr->local_para_ptr;
	BD_ADDR bdaddr;
	hdp_adp_channel_struct *channel = NULL;
  	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_remove_connection");
	if (msg->mdlId == 0)
	{
		channel = hdp_util_find_main_channel(&msg->bdaddr);
	}
	else
	{
		channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);
	}
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP] there is no main channel");
	}
	else if (HDP_ADP_CHANNEL_CONNECTED != channel->state)
	{
		OS_Report("[HDP][ADP] main channel is not connected, state is %d", channel->state);
	} else {
		OS_Report("[HDP][ADP] main channel l2cap id is %d", channel->l2capChnId);
		L2CAP_DisconnectReq(channel->l2capChnId);		
	}
		CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
		hdp_remove_connection(&bdaddr);
} 
void hdp_adp_reopen_connection(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_reopen_connection_req_struct *msg = (bt_hdp_reopen_connection_req_struct *) ilm_ptr->local_para_ptr;
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_reopen_connection");
	if (msg->mdlId == 0)
	{
		channel = hdp_util_find_main_channel(&msg->bdaddr);
	}
	else
	{
		channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);
	}
	
	if (NULL == channel)
	{		
		OS_Report("[HDP][ADP] there is no main channel");
	}	
	else if (HDP_ADP_CHANNEL_CONNECTED == channel->state)
	{
		OS_Report("[HDP][ADP] main channel has been connected");
	} else {
		CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
		ret = hdp_reopen_channel(&bdaddr, channel->mdlId);
		if (BT_STATUS_SUCCESS != ret && BT_STATUS_PENDING != ret)
		{
			hdp_util_remove_channel(channel);
			OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));	
		}
	}
}


void hdp_adp_send_data(U16 watermark,void * data,U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/  
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_water_mark(watermark);
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_send_data: channel watermark is %d", watermark);

	hdp_print_channel();

	if (NULL == channel)
	{
		OS_Report("[HDP] fail to find channel");
		return;
	}	
	hdp_send_data(channel->l2capChnId, data, len);
}


void hdp_adp_get_main_channel(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_get_main_channel_req_struct *msg = (bt_hdp_get_main_channel_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_main_channel_cnf_struct *cnf = 
		(bt_hdp_get_main_channel_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_main_channel_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/	
	channel = hdp_util_find_main_channel(&msg->bdaddr);
	if (NULL != channel)
	{
		ret = BT_STATUS_SUCCESS;
		cnf->mdlId = channel->mdlId;
		OS_MemCopy(&cnf->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	hdp_send_msg(MSG_ID_BT_HDP_GET_MAIN_CHANNEL_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_get_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_get_instance_req_struct *msg = (bt_hdp_get_instance_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_instance_cnf_struct *cnf = 
		(bt_hdp_get_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_instance_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);

	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	if (NULL != channel)
	{
		ret = BT_STATUS_SUCCESS;
		cnf->mdepId = channel->mdepId;
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	hdp_send_msg(MSG_ID_BT_HDP_GET_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void hdp_adp_get_channel_id(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_get_l2cap_channel_req_struct *msg = (bt_hdp_get_l2cap_channel_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_l2cap_channel_cnf_struct *cnf = 
		(bt_hdp_get_l2cap_channel_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_l2cap_channel_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);	
	BD_ADDR bdaddr;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_get_channel_id: addr is 0x%x : 0x%x:0x%x, mdl id is %d", 
				msg->bdaddr.lap, msg->bdaddr.uap, msg->bdaddr.nap, msg->mdlId);
	if (NULL != channel)
	{
		if (channel->state == HDP_ADP_CHANNEL_CONNECTED)
		{		
			ret = BT_STATUS_SUCCESS;
			cnf->l2capId = channel->waterMark;
			cnf->mdlId = channel->mdlId;
		}
		else
		{
			//TODO: reconnect mdl if mdl state is disconnected
			//hdp_open_channel(MdepId id,BD_ADDR * bdaddr);
			CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
			ret = hdp_reopen_channel(&bdaddr, channel->mdlId);
		}
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	OS_MemCopy(&cnf->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_echo(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_echo_req_struct *msg = (bt_hdp_echo_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_echo_cnf_struct *cnf = NULL;
	BtStatus ret;	
	BD_ADDR bdaddr;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_echo: addr is 0x%x : 0x%x:0x%x, role is %d", 
				msg->bdaddr.lap, msg->bdaddr.uap, msg->bdaddr.nap, msg->role);
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
	ret = hdp_echo(msg->role, &bdaddr);
	if (BT_STATUS_PENDING != ret)
	{
		cnf = (bt_hdp_echo_cnf_struct *) construct_local_para(sizeof(bt_hdp_echo_cnf_struct), TD_UL | TD_RESET);
		cnf->result = hdp_util_convert_status(ret);
		cnf->role = msg->role;
		OS_MemCopy(&cnf->bdaddr, &msg->bdaddr, sizeof(bt_addr_struct));
		hdp_send_msg(MSG_ID_BT_HDP_ECHO_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
	}

}

void hdp_adp_connect_cnf(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_connect_cnf_struct *cnf = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_connect_cnf: device addr = %x:%x:%x:%x:%x:%x, mdepId is %d, mdlId is %d",
				parms->content.connection.addr.addr[0], parms->content.connection.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.connection.addr.addr[3], parms->content.connection.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.connection.mdepId, parms->content.connection.mdlId);	
	OS_MemSet(&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, &parms->content.connection.addr);

	//debug
	hdp_print_channel();
	OS_Report("[HDP][ADP] remode device: %x %x %x", btAddr.lap, btAddr.uap, btAddr.nap);
	
	channel = hdp_util_find_channel_by_state(&btAddr, HDP_ADP_CHANNEL_CONNECTING);
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP]no connecting channel is found");
		return;
	}
	
	cnf = (bt_hdp_connect_cnf_struct *) construct_local_para(sizeof(bt_hdp_connect_cnf_struct), TD_UL | TD_RESET);
	if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
		channel->mdlId = parms->content.connection.mdlId;
//		channel->l2capChnId = parms->content.connection.l2capId;

		if (NULL == hdp_util_find_main_channel(&btAddr))
		{
			channel->mainChn = TRUE;		
		}
	}

	cnf->result = hdp_util_convert_status(ret);
	cnf->index = channel->index;
	cnf->mdepId = channel->mdepId;
	cnf->mainChannel = FALSE;
	cnf->mdlId = channel->mdlId;
	OS_MemCopy(&cnf->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);	
	
	if (BT_STATUS_SUCCESS != ret)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
}


void hdp_adp_disconnect_cnf(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_disconnect_cnf_struct *cnf = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_disconnect_cnf: device addr = %x:%x:%x:%x:%x:%x, mdepId is %d, mdlId is %d",
				parms->content.connection.addr.addr[0], parms->content.connection.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.connection.addr.addr[3], parms->content.connection.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.connection.mdepId, parms->content.connection.mdlId);	
	
	OS_MemSet(&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, &parms->content.connection.addr);
	channel = hdp_util_find_channel_by_state(&btAddr, HDP_ADP_CHANNEL_DISCONNECTING);
	if (NULL == channel)
	{
		OS_Report("no connecting channel is found");
		return;
	}
	
	cnf = (bt_hdp_disconnect_cnf_struct *) construct_local_para(sizeof(bt_hdp_disconnect_cnf_struct), TD_UL | TD_RESET);

	cnf->result = hdp_util_convert_status(ret);
	cnf->index = channel->index;
	cnf->mdlId = channel->mdlId;
	OS_MemCopy(&cnf->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

	
	if (BT_STATUS_SUCCESS == ret)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	else
	{
		OS_Report("[HDP] fail to disconnect");
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
}

void hdp_adp_channel_opened_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_channel_opened_ind_struct *ind = NULL;
	hdp_adp_instance_struct *instance = NULL; 
	BOOL 					mainChannel = FALSE;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_channel_opened_ind: device addr = %x%x%x%x%x%x, mdepId is %d, mdlId is %d, l2cap id is %d",
				parms->content.channel.addr.addr[0], parms->content.channel.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.channel.addr.addr[3], parms->content.channel.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.channel.mdepId, parms->content.channel.mdlId, parms->content.channel.l2capId);

	//Notes: incorrect to find instance based on mdepID
	instance = hdp_util_find_instance(parms->content.channel.mdepId);
	if (NULL == instance)
	{
		OS_Report("no instance is found");
		return;
	}
	
	OS_MemSet(&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, &parms->content.channel.addr);

	channel = hdp_util_find_channel_by_mdl_id(&btAddr, parms->content.channel.mdlId);

	if (BT_STATUS_SUCCESS != ret)
	{
		if (NULL != channel)
		{
			channel->state = HDP_ADP_CHANNEL_IDLE;
			hdp_util_remove_channel(channel);
			OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
		}
		OS_Report("[HDP]Err to open hdp channel");
		return;
	}
	
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP] No related channel is found, so add a new channel to list");
		channel = hdp_util_find_available_channel();
		if (NULL == channel)
		{
			OS_Report("no channel resource is found");
			return;
		}
		OS_MemCopy(&channel->bdaddr, &btAddr, sizeof(bt_addr_struct));
		channel->mdlId = parms->content.channel.mdlId;
		channel->mdepId = parms->content.channel.mdepId;
			if (NULL == hdp_util_find_main_channel(&btAddr))
		{
			channel->mainChn = TRUE;
			OS_Report("[HDP][ADP]the mdl channel is the first connection for the device");
		}
		
		hdp_util_add_channel(channel);
	}	
	else if (instance->mdepId != channel->mdepId)
	{
		if (HDP_ADP_CHANNEL_IDLE != channel->state)
		{
		    //It is forbiden to access two instance for one hdp channel identified by MDL ID and BD_ADDR
		    OS_Report("[HDP] Err: the channel is being mapped to two intances.");
		    return;
	        }
		else
		{
		    //Notes: replace the mdep id with the new value if the orignal channel is in ilde state
		    channel->mdepId = parms->content.channel.mdepId;			
		}
	}

	//Notes: channel is connected every time, water mark has to be updated
	channel->waterMark = hdp_util_get_channel_water_mark();
	channel->state = HDP_ADP_CHANNEL_CONNECTED;
	channel->l2capChnId = parms->content.channel.l2capId;
	hdp_print_channel();
	
	ind = (bt_hdp_channel_opened_ind_struct *) construct_local_para(sizeof(bt_hdp_channel_opened_ind_struct), TD_UL | TD_RESET);
	ind->mdepId = channel->mdepId;
	ind->mainChannel = channel->mainChn;
	ind->mdlId = channel->mdlId;
	OS_MemCopy(&ind->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CHANNEL_OPENED_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);
}

void hdp_adp_channel_closed_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_channel_closed_ind_struct *ind = NULL;
	hdp_adp_instance_struct *instance = NULL; 
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_channel_closed_ind: device addr = %x%x%x%x%x%x, mdepId is %d, mdlId is %d, l2cap id is %d",
				parms->content.channel.addr.addr[0], parms->content.channel.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.channel.addr.addr[3], parms->content.channel.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.channel.mdepId, parms->content.channel.mdlId, parms->content.channel.l2capId);
	
	OS_MemSet(&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, &parms->content.channel.addr);
	channel = hdp_util_find_channel_by_mdl_id(&btAddr, parms->content.channel.mdlId);
	if (NULL == channel)
	{
		OS_Report("no channel is found");
		return;
	}
	if (parms->content.channel.delFlag)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet(channel, 0x0, sizeof(hdp_adp_channel_struct));
		OS_Report("[HDP] clear channel cache");
	}
	else
	{
		channel->state = HDP_ADP_CHANNEL_IDLE;
	}
	
	//NOTES: Android framework requires channel closed indication to remove channel.
	/*		So even if instance can not be found, the message should be sent*/
	//If no instance is found, it is not neccessary to notify upper layer 
/*	instance = hdp_util_find_instance(parms->content.channel.mdepId);
	if (NULL == instance)
	{
		OS_Report("[HDP][ADP]no instance is found");
		return;
	}
	*/
	
	ind = (bt_hdp_channel_closed_ind_struct *) construct_local_para(sizeof(bt_hdp_channel_closed_ind_struct), TD_UL | TD_RESET);
	ind->mdlId = parms->content.channel.mdlId;
	OS_MemCopy(&ind->bdaddr, &btAddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CHANNEL_CLOSED_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);
}
void hdp_adp_data_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	hdp_adp_channel_struct *channel = NULL;	
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	OS_Report("hdp_adp_data_ind: l2cap id->%d", parms->content.data.l2capChnId);
	channel = hdp_util_find_channel_by_l2cap_Id(parms->content.data.l2capChnId) ;
	if (NULL != channel &&
		NULL != parms->content.data.value &&
		0 < parms->content.data.size)
	{
		hdp_send_msg_ind(channel->waterMark, parms->content.data.value, parms->content.data.size);		
	}
	else
	{
		OS_Report("invalid data is received");
	}
}


void hdp_adp_connection_removed(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hdp_remove_connection_cnf_struct *cnf = 
    			(bt_hdp_remove_connection_cnf_struct *) construct_local_para(sizeof(bt_hdp_remove_connection_cnf_struct), TD_UL | TD_RESET);;     
	bt_addr_struct bdaddr;
	BtStatus ret;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	CONVERT_ARRAY2BDADDR(&bdaddr, &parms->content.connection.addr);

	ret = hdp_remove_connection(&bdaddr);
	cnf->result = hdp_util_convert_status(ret);	
	OS_MemCopy(&cnf->bdaddr, &bdaddr, sizeof(bt_addr_struct));		
	hdp_send_msg(MSG_ID_BT_HDP_REMOVE_CONNECTION_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);		 	
}

void hdp_adp_Callback(HdpCallbackParm *parms)
{	
	if (NULL == parms)
	{
		return;
	}
	OS_Report("[HDP][ADP]hdp_adp_Callback: event-> %d, status-> %d", parms->event, parms->result);
	switch(parms->event)
	{
		case HDP_EVENT_CONNECT_CNF:
			hdp_adp_connect_cnf(parms);
			break;
						
		case HDP_EVENT_DISCONNECT_CNF:
			hdp_adp_disconnect_cnf(parms);
			break;
						
		case HDP_EVENT_CHANNEL_OPENED:
			hdp_adp_channel_opened_ind(parms);
			break;
			
		case HDP_EVENT_CHANNEL_CLOSED:
			hdp_adp_channel_closed_ind(parms);
			break;
			
		case HDP_EVENT_DATA_RCV_IND:	
			hdp_adp_data_ind(parms);
			break;	

		case HDP_EVENT_CONNECTION_REMOVED:
			hdp_adp_connection_removed(parms);
			break;

		default:
			OS_Report("[HDP][ADP]invalid event: %d", parms->event);
	}
}



/********************************************************************************************
 * Entry function for message handlers
 ********************************************************************************************/
void hdp_adp_init()
{
	BtStatus ret;
	if (TRUE == hdp_adp_context.initialized)
	{
		return;
	}

	OS_MemSet(&hdp_adp_context, 0x0, sizeof(hdp_adp_context_struct));
	OS_MemSet(hdp_channel, 0x0, sizeof(hdp_adp_channel_struct)*HDP_ADP_MAX_CHANNEL_NUM);
	ret = hdp_init(hdp_adp_Callback);
	if (BT_STATUS_SUCCESS == ret)
	{
		hdp_adp_context.initialized = TRUE;
		InitializeListHead(&hdp_adp_context.channel);
	}
}
void hdp_adp_deinit()
{
	U8 index;
	hdp_deinit();
	hdp_adp_context.initialized = FALSE;

	while(!IsListEmpty(&hdp_adp_context.channel))
	{
		RemoveHeadList(&hdp_adp_context.channel);
	}
}



void btmtk_adp_hdp_handle_message(ilm_struct *ilm_ptr) {
	OS_Report("[Hdp] %s, msg_id: %d", __FUNCTION__, ilm_ptr->msg_id);

	switch (ilm_ptr->msg_id) {	
		case MSG_ID_BT_HDP_ACTIVATE_REQ :
			break;
			
		case MSG_ID_BT_HDP_DEACTIVATE_REQ:   
			break;
			     			 
		case MSG_ID_BT_HDP_REGISTER_INSTANCE_REQ: 
			hdp_adp_register_instance(ilm_ptr);
			break;
						
		case MSG_ID_BT_HDP_DEREGISTER_INSTANCE_REQ:
			hdp_adp_deregister_instance(ilm_ptr);
			break;
		
		case MSG_ID_BT_HDP_CONNECT_REQ:    
			hdp_adp_connect(ilm_ptr);
			break;
			        
		case MSG_ID_BT_HDP_DISCONNECT_REQ:    
			hdp_adp_disconnect(ilm_ptr); 
			break;

		case MSG_ID_BT_HDP_REMOVE_CONNECTION_REQ:
			hdp_adp_remove_connection(ilm_ptr);
			break;
			      			                                      
		case MSG_ID_BT_HDP_GET_MAIN_CHANNEL_REQ:     
			hdp_adp_get_main_channel(ilm_ptr);
			break;
			
		case MSG_ID_BT_HDP_GET_INSTANCE_REQ:       
			hdp_adp_get_instance(ilm_ptr);  
			break;
			 			  
		case MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_REQ:     
			hdp_adp_get_channel_id(ilm_ptr);
			break;
			
		case MSG_ID_BT_HDP_ECHO_REQ:
			hdp_adp_echo(ilm_ptr);
			break;

		case MSG_ID_BT_HDP_RECONNECT_REQ:
			hdp_adp_reopen_connection(ilm_ptr);
			break;
			
		default:
			break;
	}
}

#endif

