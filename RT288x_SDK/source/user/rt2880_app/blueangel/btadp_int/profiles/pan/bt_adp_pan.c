/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_adp_pan.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is internal adaptation layer for pan profie 
 *
 * Author:
 * -------
 * Ting Zheng
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
#ifdef __BT_PAN_PROFILE__
#include <assert.h>

#include "bt_os_api.h"
#include "bnep.h"
#include "pan.h"

#include "utils.h"

#include "bluetooth_pan_message.h"
#include "bluetooth_pan_struct.h"

#include "bt_adp_pan.h"
#include "bt_adp_pan_ip.h"

#define IPPROTO_IPV6    6

typedef struct _pan_context_struct
{
    BT_BOOL	bConnInitiator;
    BT_BOOL	bConnTerminator;
    BT_BOOL bActive;    // true: if connection is active
    BD_ADDR	bd_addr;
    BT_BOOL	in_use;	
    PanUser	*pan;
    //U8			net_dev_idx;
} pan_context_struct;

pan_context_struct pan_cntx[PAN_MAX_DEV_NUM];
ListEntry panPacketList;		// packet inserted to l2cap pending list
ListEntry panPacketPendingList;	// packet queued temporialy to be inserted to l2cap list later

typedef struct _pan_packet_node
{
    ListEntry node;
    PanUser *pan;
    U8 pkt_type;	
    U8 *pkt;
    U16 len;	
} pan_packet_node;


static void pan_send_msg(
        msg_type msg,
        module_type dstMod,
        local_para_struct *local_para);
static void pan_send_packet(PanUser *pan, U8 pkt_type, U8 *buffer, U16 len);
static pan_context_struct *pan_find_free_context(void);
static pan_context_struct *pan_find_free_context_by_addr(BD_ADDR *bt_addr_p);
static pan_context_struct *pan_find_context_by_addr(BD_ADDR *bt_addr_p);
static BT_BOOL pan_is_connected(void);
static void pan_clear_connection(pan_context_struct *cntx);
static pan_packet_node *pan_new_packet_node(U8 *pkt_buff, U16 pkt_len, U16 offset);
static void pan_free_packet_node(pan_packet_node *pkt_node);


void pan_adp_init(void)
{
    U8 i;

    bt_prompt_trace(MOD_BT, "pan adp init...");
    btmtk_os_memset((U8 *)&pan_cntx, 0, sizeof(pan_cntx));

    /* Initialize Transmission Packet List */
    InitializeListHead(&panPacketList);
    InitializeListHead(&panPacketPendingList); 
}

void pan_handle_setup_connection(PanUser *pan)
{
    //PanSetupConnPkt *setupPkt;
    pan_packet_node *pkt_node;

    //setupPkt = PAN_MEM_MALLOC(sizeof(PanSetupConnPkt));

    pkt_node = pan_new_packet_node(NULL, sizeof(PanSetupConnPkt), 0);
    InsertTailList(&panPacketList, &(pkt_node->node));
 
    PAN_SetupConnection(pan, (PanSetupConnPkt *)pkt_node->pkt);
}

void pan_handle_setup_conn_req(PanUser *pan, PanServiceType service, U16 status)
{
    pan_context_struct *cntx = pan_find_free_context_by_addr((BD_ADDR *)pan->destMac.addr);
    bt_pan_connection_authorize_ind_struct *msg_p;
    PanControlRespPkt *setupRespPkt;
    bt_pan_service_enum srcService;
    pan_packet_node *pkt_node;

    if (status != BNEP_SR_OPERATION_SUCCESS)
    {
        goto exit;		
    }

    if (cntx == NULL)
    {
        cntx = pan_find_free_context();
    }
    else
    {
        // it is reconnect retrying?
    }

    if (cntx != NULL)
    {
        switch (service)
        {
        case PANSVC_NAP:
            srcService = PAN_SERVICE_NAP;
            break;
        case PANSVC_GN:
            srcService = PAN_SERVICE_GN;
            break;
        case PANSVC_PANU:
            srcService = PAN_SERVICE_PANU;
            break;
        default:
            return;
        }

        msg_p = (bt_pan_connection_authorize_ind_struct*)construct_local_para(sizeof(bt_pan_connection_authorize_ind_struct), TD_UL);
        btmtk_os_memcpy((U8 *)&msg_p->bt_addr, pan->destMac.addr, sizeof(msg_p->bt_addr)); 
        msg_p->service = srcService;
        pan_send_msg(MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_IND, MOD_EXTERNAL,
    		(local_para_struct*)msg_p);
    }
    else
    {
        status = BNEP_SR_OPERATION_FAILED;
        goto exit;
    }

    btmtk_os_memcpy((U8 *)&cntx->bd_addr, (U8 *)&pan->destMac, sizeof(cntx->bd_addr));
    cntx->pan = pan;
    cntx->in_use = TRUE;
    
    return;		

exit:
    pkt_node = pan_new_packet_node(NULL, sizeof(PanControlRespPkt), 0);
    InsertTailList(&panPacketList, &(pkt_node->node));
    setupRespPkt = (PanControlRespPkt *)pkt_node->pkt;
    setupRespPkt->message[0] = (status&0xff00)>>8;
    setupRespPkt->message[1] = status&0xff;
    PAN_ControlResponse(pan, BNEP_SETUP_CONNECTION_RESPONSE_MSG, setupRespPkt, sizeof(PanControlRespPkt));
}

void pan_handle_control_ind(PanUser *pan, BnepControlType type, U8 *data, U16 status)
{
    pan_packet_node *pkt_node;
    pan_context_struct *cntx = pan_find_context_by_addr((BD_ADDR *)pan->destMac.addr);
    PanControlRespPkt *respPkt;
    U8 control_type;    
    U16 respPkt_len = sizeof(PanControlRespPkt);

    assert(cntx != NULL);

    bt_prompt_trace(MOD_BT, "pan handle control ind type: %d, status: %x, active: %d", type, status, cntx->bActive);

    switch (type)
    {
        case BNEP_FILTER_NET_TYPE_SET_MSG:
            {
                pkt_node = pan_new_packet_node(NULL, sizeof(PanControlRespPkt), 0);
                respPkt = (PanControlRespPkt *)pkt_node->pkt;
                respPkt->message[0] = (status & 0xff00)>>8;
                respPkt->message[1] = status & 0xff;
                control_type = BNEP_FILTER_NET_TYPE_RESPONSE_MSG;
            }
            break;
            
        case BNEP_FILTER_MULTI_ADDR_SET_MSG:
            {
                pkt_node = pan_new_packet_node(NULL, sizeof(PanControlRespPkt), 0);
                respPkt = (PanControlRespPkt *)pkt_node->pkt;
                respPkt->message[0] = (status & 0xff00)>>8;
                respPkt->message[1] = status & 0xff;
                control_type = BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG;
            }
            break;
          
        case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
        case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
            return;

        default:
            {
                pkt_node = pan_new_packet_node(NULL, sizeof(PanControlRespPkt), 0);
                respPkt = (PanControlRespPkt *)pkt_node->pkt;
                respPkt->message[0] = type;
                control_type = BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD;
                respPkt_len = respPkt_len - 1;				
            }
            break;			
    }

    /* if connection is not active yet, do not send control response until connection setup response is sent */
    if (cntx->bActive)
    {
        PAN_ControlResponse(pan, control_type, respPkt, respPkt_len);
        InsertTailList(&panPacketList, &(pkt_node->node));
    }
    else
    {
        pkt_node->len = respPkt_len;
        pkt_node->pan = cntx->pan;
        pkt_node->pkt_type = control_type;
        InsertTailList(&panPacketPendingList, &(pkt_node->node));
    }
    
}

void pan_handle_connected(PanUser *pan, PanServiceType service)
{
    pan_context_struct *cntx = pan_find_context_by_addr((BD_ADDR *)pan->destMac.addr);
    bt_pan_service_enum srcService;
    BD_ADDR macAddr;
    BtStatus status;
    S32 unit;

    assert(cntx != NULL);

    // open network device
    if ((unit = pan_ip_open_network_dev(macAddr.addr)) < 0)
    {
        status = PAN_Close(cntx->pan);
        if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
        {
            cntx->bConnTerminator = TRUE;
        }
        return; 	
    }

    PAN_SetLocalIpAddr(PAN_NET_DEV_IP_ADDR);
    PAN_SetLocalMacAddr(macAddr.addr);

    switch (service)
    {
    case PANSVC_NAP:
        srcService = PAN_SERVICE_NAP;
        break;
    case PANSVC_GN:
        srcService = PAN_SERVICE_GN;
        break;
    case PANSVC_PANU:
        srcService = PAN_SERVICE_PANU;
        break;
    default:
        return;
    }

    cntx->bActive = TRUE;
    if (cntx->bConnInitiator)
    {
        bt_pan_connect_cnf_struct *msg_p;
        msg_p = (bt_pan_connect_cnf_struct*) construct_local_para(sizeof(bt_pan_connect_cnf_struct), TD_UL);
        btmtk_os_memcpy((U8 *)&msg_p->bt_addr, pan->destMac.addr, sizeof(msg_p->bt_addr)); 
        msg_p->result = TRUE;
        msg_p->service = srcService;
        msg_p->unit = unit;
        pan_send_msg(MSG_ID_BT_PAN_CONNECT_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
    }
    else
    {
        bt_pan_connect_ind_struct *msg_p;
        msg_p = (bt_pan_connect_ind_struct*) construct_local_para(sizeof(bt_pan_connect_ind_struct), TD_UL);
        btmtk_os_memcpy((U8 *)&msg_p->bt_addr, pan->destMac.addr, sizeof(msg_p->bt_addr)); 
        msg_p->service = srcService;
        msg_p->unit = unit;
        pan_send_msg(MSG_ID_BT_PAN_CONNECT_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
    }
	
}

void pan_handle_disconnected(PanUser *pan, U16 status)
{
    pan_context_struct *cntx = pan_find_context_by_addr((BD_ADDR *)&pan->destMac);
    L2capDiscReason reason = status;

    bt_prompt_trace(MOD_BT, "pan disconnected, btaddr: %x, %x, %x", pan->destMac.addr[0], pan->destMac.addr[1], pan->destMac.addr[2]);
    bt_prompt_trace(MOD_BT, "pan disconnected cntx: %x", cntx);

    if (cntx == NULL)
        return;

    if (cntx->bConnTerminator)
    {
        bt_pan_disconnect_cnf_struct *msg_p;
        msg_p = (bt_pan_disconnect_cnf_struct*) construct_local_para(sizeof(bt_pan_disconnect_cnf_struct), TD_UL);
        btmtk_os_memcpy((U8 *)&msg_p->bt_addr, pan->destMac.addr, sizeof(msg_p->bt_addr)); 
        msg_p->result = TRUE;
        pan_send_msg(MSG_ID_BT_PAN_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
    else
    {
        bt_pan_disconnect_ind_struct *msg_p;
        msg_p = (bt_pan_disconnect_ind_struct*) construct_local_para(sizeof(bt_pan_disconnect_ind_struct), TD_UL);
        btmtk_os_memcpy((U8 *)&msg_p->bt_addr, pan->destMac.addr, sizeof(msg_p->bt_addr)); 
        pan_send_msg(MSG_ID_BT_PAN_DISCONNECT_IND, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
	
	pan_clear_connection(cntx);
}

void pan_handle_data_ind(PanUser *pan, PanPacketType type, U8 *data, U16 len)
{
    switch (type)
    {
        case PANPKT_ARP_REQUEST:
            /* It is a arp request, construct and send reply */
#if 0
            {
                PanPacket * panPkt;
                panPkt = PAN_MEM_MALLOC(sizeof(PanPacket) + len);
                btmtk_os_memcpy((U8 *)panPkt+sizeof(PanPacket), data, len);				
                PAN_SendPkt(pan, panPkt, PANPKT_ARP_REPLY, len);
            }
#endif
            break;			

        case PANPKT_ARP_REPLY:
            /* It is a arp reply, notify upper layer if necessary */
            break;			

        case PANPKT_IP:
            /* It is a IP packet, send to external network */
            pan_ip_send_packet(data, len);			
            break;			

        default:
            break;			
    }
}

void pan_handle_data_cmplt(PanUser *pan, U8 *data, U16 status)
{
    pan_packet_node *panPkt;

    bt_prompt_trace(MOD_BT, "pan handle data complete: %x, %d", data, status);

    panPkt = (pan_packet_node *)GetHeadList(&panPacketList);
    while ((ListEntry *)panPkt != &panPacketList) 
    {
        if (data == (U8 *)panPkt->pkt) 
        {
            /* Found the matching packet node */
            RemoveEntryList(&(panPkt->node));
            //PAN_MEM_FREEIF(panPkt->pkt);
            //PAN_MEM_FREEIF(panPkt);
            pan_free_packet_node(panPkt);
            break;
        }
            
        /* Try the next packet node */
        panPkt = (pan_packet_node *)GetNextNode(&panPkt->node);
    }

    if (status == BT_STATUS_SUCCESS)
    {
        /* Try to send packet in pending list */
        if (!IsListEmpty(&panPacketPendingList))
        {
            pan_send_msg(MSG_ID_BT_PAN_SEND_PENDING_PACKET_REQ, MOD_BT, NULL);
        }
    }
    else
    {
        /* It may be caused by connection timeout, remove all pending packets */
        while (!IsListEmpty(&panPacketPendingList)) 
        {
            panPkt = (pan_packet_node *)RemoveHeadList(&panPacketPendingList);
            pan_free_packet_node(panPkt);
        }
    }
}

void pan_adp_callback(PanCallbackParms *parms)
{
    switch (parms->event)
    {
        case PANEVENT_SETUP_CONNECTION:
            /* Local device initiates connection */
            pan_handle_setup_connection(parms->pan);			
            break;			
        case PANEVENT_SETUP_CONN_REQ:
            /* Remote device initiates connection, notify user to do authorization */
            pan_handle_setup_conn_req(parms->pan, parms->service, parms->status);			
            break;			
        case PANEVENT_CONTROL_IND:
            pan_handle_control_ind(parms->pan, parms->pkt[0], &parms->pkt[1], parms->status);			
            break;			
        case PANEVENT_OPEN:
            pan_handle_connected(parms->pan, parms->service);		
            break;			
        case PANEVENT_CLOSED:
            pan_handle_disconnected(parms->pan, parms->status);			
            break;			
        //case PANEVENT_SDP_FAILED:
            //break;			
        case PANEVENT_DATA_IND:
            pan_handle_data_ind(parms->pan, parms->type, parms->pkt, parms->rxPktLen);			
            break;			
        case PANEVENT_DATA_HANDLED:
            pan_handle_data_cmplt(parms->pan, parms->pkt, parms->status);			
            break;			
        default:
            break;			
    }
}

void btmtk_adp_pan_handle_activate_req(ilm_struct *ilm_ptr)
{
    bt_pan_activate_cnf_struct *msg_p;
    PanService service;

    msg_p = (bt_pan_activate_cnf_struct*) construct_local_para(sizeof(bt_pan_activate_cnf_struct), TD_UL);

    service.callback = pan_adp_callback;
    service.type = PANSVC_NAP;

    if (PAN_Register(&service) == BT_STATUS_SUCCESS)
    {
        service.type = PANSVC_GN;
        if (PAN_Register(&service) == BT_STATUS_SUCCESS)
        {
            msg_p->result = TRUE;
        }
        else
        {
            msg_p->result = FALSE;
        }
    }
    else
    {
        msg_p->result = FALSE;
    }
    pan_send_msg(MSG_ID_BT_PAN_ACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_pan_handle_deactivate_req(ilm_struct *ilm_ptr)
{
    bt_pan_deactivate_cnf_struct *msg_p;
    PanService service;

    msg_p = (bt_pan_deactivate_cnf_struct*) construct_local_para(sizeof(bt_pan_deactivate_cnf_struct), TD_UL);

    service.type = PANSVC_NAP;
    if (PAN_Deregister(&service) == BT_STATUS_SUCCESS)
    {
        service.type = PANSVC_GN;
        if (PAN_Deregister(&service) == BT_STATUS_SUCCESS)
        {
            msg_p->result = TRUE;
        }
        else
        {
            msg_p->result = FALSE;
        }
    }
    else
    {
        msg_p->result = FALSE;
    }
    pan_send_msg(MSG_ID_BT_PAN_DEACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_pan_handle_connect_req(ilm_struct *ilm_ptr)
{
    bt_pan_connect_req_struct *req_p = (bt_pan_connect_req_struct*) ilm_ptr->local_para_ptr;
    pan_context_struct *cntx = pan_find_free_context_by_addr(&req_p->bt_addr);    
    BtStatus status = BT_STATUS_FAILED;
    PanServiceType srcService;
    PanServiceType dstService;

    if (cntx != NULL)
    {
        // it is in reconnecting?
    }
    else
    {
        if ((cntx = pan_find_free_context()) == NULL)
        {
            goto fail;
        }
    }

    bt_prompt_trace(MOD_BT, "pan connect request, service: %d", req_p->service);
    switch (req_p->service)
    {
        case PAN_SERVICE_NAP:
            srcService = PANSVC_NAP;
            dstService = PANSVC_PANU;
            break;
        case PAN_SERVICE_GN:
            srcService = PANSVC_GN;
            dstService = PANSVC_PANU;
            break;
        case PAN_SERVICE_PANU:
            srcService = 	PANSVC_PANU;
            if (req_p->dstservice == PAN_SERVICE_NAP)
            {
                dstService = PANSVC_NAP;
            }
            else if (req_p->dstservice == PAN_SERVICE_GN)
            {
                dstService = PANSVC_GN;
            }
            else
            {
                goto fail;
            }
            break;			
        default:
            goto fail;
    }
    status = PAN_Open(&cntx->pan, srcService, dstService, &req_p->bt_addr);
    if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))
    {
        goto fail;
    }

    cntx->bConnInitiator = TRUE;
    cntx->bd_addr = req_p->bt_addr;
    cntx->in_use = TRUE;
    return;

fail:
    /* send connect cnf failed result to upper layer */
    {
        bt_pan_connect_cnf_struct *msg_p;
        msg_p = (bt_pan_connect_cnf_struct*) construct_local_para(sizeof(bt_pan_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        pan_send_msg(MSG_ID_BT_PAN_CONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }	
}

void btmtk_adp_pan_handle_disconnect_req(ilm_struct *ilm_ptr)
{
    bt_pan_disconnect_req_struct *req_p = (bt_pan_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    pan_context_struct *cntx = pan_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    if (cntx == NULL)
    {
        goto fail;
    }

    status = PAN_Close(cntx->pan);
    if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
    {
        cntx->bConnTerminator = TRUE;
        return;		
    }

fail:
    /* send disconnect cnf failed result to upper layer */
    {
        bt_pan_disconnect_cnf_struct *msg_p;
        msg_p = (bt_pan_disconnect_cnf_struct*) construct_local_para(sizeof(bt_pan_disconnect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        pan_send_msg(MSG_ID_BT_PAN_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }

}

void btmtk_adp_pan_handle_connection_authorize_rsp(ilm_struct *ilm_ptr)
{
    bt_pan_connection_authorize_rsp_struct *rsp_p = (bt_pan_connection_authorize_rsp_struct*) ilm_ptr->local_para_ptr;
    pan_context_struct *cntx = pan_find_context_by_addr(&rsp_p->bt_addr);    
    PanControlRespPkt *setupRespPkt;
    pan_packet_node *pkt_node;
    U16 status;

    assert(cntx != NULL);

    if (rsp_p->accept)
    {
        status = BNEP_SR_OPERATION_SUCCESS;
    }
    else
    {
        status = BNEP_SR_OPERATION_FAILED;
    }
    pkt_node = pan_new_packet_node(NULL, sizeof(PanControlRespPkt), 0);
    InsertTailList(&panPacketList, &(pkt_node->node));
    setupRespPkt = (PanControlRespPkt *)pkt_node->pkt;
    setupRespPkt->message[0] = (status&0xff00)>>8;
    setupRespPkt->message[1] = status&0xff;
    PAN_ControlResponse(cntx->pan, BNEP_SETUP_CONNECTION_RESPONSE_MSG, setupRespPkt, sizeof(PanControlRespPkt));

}

void btmtk_adp_pan_handle_set_nettype_filter_req(ilm_struct *ilm_ptr)
{
    bt_pan_set_nettype_filter_req *req_p = (bt_pan_set_nettype_filter_req*) ilm_ptr->local_para_ptr;
    pan_context_struct *cntx = pan_find_context_by_addr(&req_p->bt_addr);    
    PanFilterPkt *filterPkt;
    pan_packet_node *pkt_node;

    assert(cntx != NULL);

    pkt_node = pan_new_packet_node(req_p->list, sizeof(PanFilterPkt) + req_p->listLen, sizeof(PanFilterPkt));
    InsertTailList(&panPacketList, &(pkt_node->node));

    filterPkt = (PanFilterPkt *) pkt_node->pkt;
    filterPkt->listLen[0] = (req_p->listLen & 0xff00) >> 8;
    filterPkt->listLen[1] = req_p->listLen & 0xff;

    //btmtk_os_memcpy(filterPkt->list, req_p->list, req_p->listLen);
    PAN_FilterProtocol(cntx->pan, filterPkt);
}

void btmtk_adp_pan_handle_set_multiaddr_filter_req(ilm_struct *ilm_ptr)
{
    bt_pan_set_multiaddr_filter_req *req_p = (bt_pan_set_multiaddr_filter_req*) ilm_ptr->local_para_ptr;
    pan_context_struct *cntx = pan_find_context_by_addr(&req_p->bt_addr);    
    PanFilterPkt *filterPkt;
    pan_packet_node *pkt_node;

    assert(cntx != NULL);

    pkt_node = pan_new_packet_node(req_p->list, sizeof(PanFilterPkt) + req_p->listLen, sizeof(PanFilterPkt));
    InsertTailList(&panPacketList, &(pkt_node->node));

    filterPkt = (PanFilterPkt *) pkt_node->pkt;
    filterPkt->listLen[0] = (req_p->listLen & 0xff00) >> 8;
    filterPkt->listLen[1] = req_p->listLen & 0xff;

    //btmtk_os_memcpy(filterPkt->list, req_p->list, req_p->listLen);
    PAN_FilterMulticast(cntx->pan, filterPkt);
}

void btmtk_adp_pan_handle_send_packet_req(ilm_struct *ilm_ptr)
{
    U8 buffer[PAN_MRU];
    S16 len;
    U8 *pkt;
    pan_context_struct *cntx;

    len = pan_ip_receive_packet(buffer);
    if (len < 0)
        return;
    
    pkt = buffer + ETHER_HDR_SIZE;
    //len -= ETHER_HDR_SIZE;
    // filter IPv6 packet
    if ((pkt[0]&0xf0)>>4 == IPPROTO_IPV6)
    {
        bt_prompt_trace(MOD_BT, "pan receive ipv6 packet, ignore...");
        return;
    }

    bt_prompt_trace(MOD_BT, "pan dstmac: %x, %x, %x, %x, %x, %x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    bt_prompt_trace(MOD_BT, "pan srcmac: %x, %x, %x, %x, %x, %x", buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);
    
    cntx = pan_find_context_by_addr((BD_ADDR *)buffer);
    if (cntx != NULL)
    {
        pan_send_packet(cntx->pan, PANPKT_ETHER_PKT, buffer, len);
    }
    else 
    {
        U8 i = 0;

        /* it might be broadcast or multicast packet, or it 
             shall be forwarded via NAP to other device. 
             So send packet to all connected device
        */ 	
        for (i = 0; i < PAN_MAX_DEV_NUM; i++)
        {
            if (pan_cntx[i].in_use)
            {
                cntx = &pan_cntx[i];
                pan_send_packet(cntx->pan, PANPKT_ETHER_PKT, buffer, len);
            }
        }
    }
}

void btmtk_adp_pan_handle_send_pending_packet_req(ilm_struct *ilm_ptr)
{
    pan_packet_node *panPkt;
    BtStatus status;

    if (IsListEmpty(&panPacketPendingList))
    {
        return;
    }

    panPkt = (pan_packet_node *)GetHeadList(&panPacketPendingList);

    if (panPkt->pkt_type < PANPKT_UNKNOWN)
    {
        
        status = PAN_ControlResponse(panPkt->pan, (BnepControlType)panPkt->pkt_type, 
			(PanControlRespPkt *)panPkt->pkt, panPkt->len);
    }
    else
    {
        status = PAN_SendPkt(panPkt->pan, (PanPacket *)panPkt->pkt, 
			(PanPacketType)panPkt->pkt_type, panPkt->len);
    }
    
    
    bt_prompt_trace(MOD_BT, "pan send pending packet: %x, status: %d", panPkt->pkt, status);
    
    /* Remove packet node from pending list */
    RemoveEntryList(&(panPkt->node));
    
    if (status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING)
    {
        InsertTailList(&panPacketList, &(panPkt->node));
    }
    else
    {
        pan_free_packet_node(panPkt);
    }

}


#ifdef __PAN_PTS_TEST__
void btmtk_adp_pan_handle_pts_test_send_packet_req(ilm_struct *ilm_ptr)
{
    bt_pan_pts_test_send_packet_req_struct *req;
    U8 *buffer;
    S16 len;
    pan_context_struct *cntx;

    req = (bt_pan_pts_test_send_packet_req_struct *) ilm_ptr->local_para_ptr;

    len = req->len;
    buffer = (U8 *)req + sizeof(bt_pan_pts_test_send_packet_req_struct);
    
    bt_prompt_trace(MOD_BT, "pan dstmac: %x, %x, %x, %x, %x, %x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    bt_prompt_trace(MOD_BT, "pan srcmac: %x, %x, %x, %x, %x, %x", buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);

    bt_prompt_trace(MOD_BT, "pan pts send packet type: %x", req->packet_type);

    cntx = pan_find_context_by_addr((BD_ADDR *)buffer);
    if (cntx != NULL)
    {
        pan_send_packet(cntx->pan, req->packet_type, buffer, len);
    }
    else 
    {
        U8 i = 0;

        /* it might be broadcast or multicast packet, or it 
             shall be forwarded via NAP to other device. 
             So send packet to all connected device
        */ 	
        for (i = 0; i < PAN_MAX_DEV_NUM; i++)
        {
            if (pan_cntx[i].in_use)
            {
                cntx = &pan_cntx[i];
                pan_send_packet(cntx->pan, req->packet_type, buffer, len);
            }
        }
    }
}
#endif


void btmtk_adp_pan_handle_message(void *msg)
{
    ilm_struct *ilm_ptr = (ilm_struct *)msg;
    bt_prompt_trace(MOD_BT, "pan int-adp handle message");

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_PAN_ACTIVATE_REQ:
            btmtk_adp_pan_handle_activate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_DEACTIVATE_REQ:
            btmtk_adp_pan_handle_deactivate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_CONNECT_REQ:
            btmtk_adp_pan_handle_connect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_DISCONNECT_REQ:
            btmtk_adp_pan_handle_disconnect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_RSP:
            btmtk_adp_pan_handle_connection_authorize_rsp(ilm_ptr);			
            break;
        //case MSG_ID_BT_PAN_PACKET_AVAILABLE_IND_RSP:
            //btmtk_adp_pan_handle_packet_available_ind_rsp(ilm_ptr);			
            //break;
        case MSG_ID_BT_PAN_SET_NETTYPE_FILETER_REQ:
            btmtk_adp_pan_handle_set_nettype_filter_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_SET_MULTIADDR_FILETER_REQ:
            btmtk_adp_pan_handle_set_multiaddr_filter_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_SEND_PACKET_REQ:
            btmtk_adp_pan_handle_send_packet_req(ilm_ptr);			
            break;
        case MSG_ID_BT_PAN_SEND_PENDING_PACKET_REQ:
            btmtk_adp_pan_handle_send_pending_packet_req(ilm_ptr);			
            break;

#ifdef __PAN_PTS_TEST__
        case MSG_ID_BT_PAN_PTS_TEST_SEND_PACKET_REQ:
            bt_prompt_trace(MOD_BT, "pan int-adp handle message pts send packet....");
            btmtk_adp_pan_handle_pts_test_send_packet_req(ilm_ptr);		
            break;
#endif
        default:
            break;
    }
}

static void pan_send_msg(
        msg_type msg,
        module_type dstMod,
        local_para_struct *local_para)
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

static void pan_send_packet(PanUser *pan, U8 pkt_type, U8 *buffer, U16 len)
{
    pan_packet_node * panPkt;
    BtStatus    status;

    panPkt = pan_new_packet_node(buffer, sizeof(PanPacket) + len, sizeof(PanPacket));

    status = PAN_SendPkt(pan, (PanPacket *)panPkt->pkt, pkt_type, len);
    bt_prompt_trace(MOD_BT, "pan send packet: %x, status: %d", panPkt->pkt, status);
    if (status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING)
    {
        InsertTailList(&panPacketList, &(panPkt->node));
    }
    else if (status == BT_STATUS_NO_RESOURCES)
    {
        panPkt->len = len;
        panPkt->pan = pan;
        panPkt->pkt_type = pkt_type;
        InsertTailList(&panPacketPendingList, &(panPkt->node));
    }
    else
    {
        pan_free_packet_node(panPkt);
    }

}

static pan_context_struct *pan_find_free_context(void)
{
    pan_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (!pan_cntx[i].in_use)
        {
            cntx = &pan_cntx[i];
            break;
        }
    }
    return cntx;	
}

static pan_context_struct *pan_find_free_context_by_addr(BD_ADDR *bt_addr_p)
{
    pan_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (PAN_DEVICE_ADDR_EQUAL(&pan_cntx[i].bd_addr, bt_addr_p) && 
             !pan_cntx[i].in_use)
        {
            cntx = &pan_cntx[i];
            break;
        }
    }
    return cntx;	
}

static BT_BOOL pan_is_connected(void)
{
    U8 i;

    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (pan_cntx[i].bActive)
        {
            return TRUE;
        }
    }
    return FALSE;	
}

static pan_context_struct *pan_find_context_by_addr(BD_ADDR *bt_addr_p)
{
    pan_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (PAN_DEVICE_ADDR_EQUAL(&pan_cntx[i].bd_addr, bt_addr_p) && 
             pan_cntx[i].in_use)
        {
            cntx = &pan_cntx[i];
            break;
        }
    }
    return cntx;	
}

static void pan_clear_connection(pan_context_struct *cntx)
{
    pan_packet_node *panPkt;

    cntx->bActive = FALSE;
    if (!pan_is_connected())
    {
        // close network device if there is no active connection
        pan_ip_close_network_dev();
        while (!IsListEmpty(&panPacketList)) 
        {
            panPkt = (pan_packet_node *)RemoveHeadList(&panPacketList);
            //PAN_MEM_FREEIF(panPkt->pkt);
            //PAN_MEM_FREEIF(panPkt);
            pan_free_packet_node(panPkt);
        }
        while (!IsListEmpty(&panPacketPendingList)) 
        {
            panPkt = (pan_packet_node *)RemoveHeadList(&panPacketPendingList);
            //PAN_MEM_FREEIF(panPkt->pkt);
            //PAN_MEM_FREEIF(panPkt);
            pan_free_packet_node(panPkt);
        }
    }
   
    btmtk_os_memset((U8 *)cntx, 0, sizeof(pan_context_struct));
}

static pan_packet_node *pan_new_packet_node(U8 *pkt_buff, U16 pkt_len, U16 offset)
{
    pan_packet_node *pkt_node;

    pkt_node = (pan_packet_node *)PAN_MEM_MALLOC(sizeof(pan_packet_node));

    pkt_node->pkt = (U8 *)PAN_MEM_MALLOC(pkt_len);

    if (pkt_buff != NULL)
    {
        btmtk_os_memcpy(((U8 *)(pkt_node->pkt))+offset, pkt_buff, pkt_len - offset);
    }

    return pkt_node;
}

static void pan_free_packet_node(pan_packet_node *pkt_node)
{
    PAN_MEM_FREEIF(pkt_node->pkt);
    PAN_MEM_FREEIF(pkt_node);
}

#endif
