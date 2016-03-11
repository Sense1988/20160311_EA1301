#include "bt_common.h"
#include "btalloc.h"
#include "hci_meta.h"
#include "bt_feature.h"

#ifdef __BT_3_0_HS__
#include "meamp.h"
#include "pal_hci_struct.h"

void HciReportAMPDataBlocksEvent(U16 handler,
                                U16 num_of_packets,
                                U16 num_of_blocks)
{
	U8 entry;
    U8 controllerId;
    HciCallbackParms cbParms;
#ifdef __BT_30_HS_INFINITE_SENT__
#else
    entry = HciGetAMPTableEntry(handler);
    Report(("A HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));

    if (entry != INVALID_HCI_HANDLE)
    {
        HCC(ampCon)[entry].packetsSent -= num_of_blocks;
        HCC(wifiController).numOfDataBlockbuffer += num_of_blocks;
        HCC(wifiController).ampTxPacketsSent -= num_of_packets;
        Report(("B HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));
        Report(("HCC(wifiController).ampTxPacketsSent:%d",HCC(wifiController).ampTxPacketsSent));        
        if (HCC(metaMode))
        {
            if (HCC(radioEntry))
            {
                cbParms.ptr.txNumber = (U8)num_of_packets;
                Hci_MetaDataCallback(HCI_META_PKT_SENT_OUT, &cbParms);
            }
        }
        HCI_RequestToSend(handler);
    }
#endif    
}

void HciSetAMPControllerTxBuffer(U8 controllerId, BT_AMP_HCI_EVENT_READ_BLOCK_SIZE *tx_buffer)
{
    if(controllerId == HCC(wifiController).controllerId)
    {
        HCC(wifiController).maxACLTxDataLength = tx_buffer->max_ack_data_packet_length;
        HCC(wifiController).maxDataBlockLength = tx_buffer->data_block_length;
        HCC(wifiController).numOfDataBlockbuffer = tx_buffer->total_num_data_blocks;
        Report(("C HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));
 
    }
}

U8 HciGetAMPControllerTxAvailable(U8 controllerId)
{
    U8 maxTxPacketNum = 0;
#ifdef __BT_30_HS_INFINITE_SENT__
    return 10;
#else
    if(controllerId == HCC(wifiController).controllerId)
    {
        Report(("F HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));
        Report(("F HCC(wifiController).maxDataBlockLength:%d",HCC(wifiController).maxDataBlockLength));
        Report(("F HCC(wifiController).maxACLTxDataLength:%d",HCC(wifiController).maxACLTxDataLength));

        maxTxPacketNum = (HCC(wifiController).numOfDataBlockbuffer * HCC(wifiController).maxDataBlockLength)/
            HCC(wifiController).maxACLTxDataLength;
    }
    Report(("Tx available packet buffer :%d",maxTxPacketNum));
    return maxTxPacketNum;
#endif

}

void HciAMPControllerTxReduce(U8 controllerId, U16 tx_size, U16 hciHandle)
{
    U8 dataCount;
    U8 entry;
    entry = HciGetAMPTableEntry(hciHandle);
    if(entry == INVALID_HCI_HANDLE)
    {
        Assert(0);
    }
    HCC(ampCon)[entry].packetsSent++;
#ifndef __BT_30_HS_INFINITE_SENT__

    if(controllerId == HCC(wifiController).controllerId)
    {
        HCC(wifiController).ampTxPacketsSent++;
        HCC(wifiController).numOfDataBlockbuffer -= 1; /* For MAUI 5921 only 1*/
        Report(("E HCC(wifiController).numOfDataBlockbuffer:%d",HCC(wifiController).numOfDataBlockbuffer));
    }
#endif    
}


U8 HciSentAMPCommand(
                        msg_type msg,
                        local_para_struct *local_para,
                        U8 controllerId)

{
    U8 result=0;
    
    if (HCC(state) != HCI_STATE_READY)  
    {
        return 0x00;
    }
    if(msg != MSG_ID_PAL_BT_DATA_COMMAND)
    {
        Report(("HciSentAMPCommand %d", HCC(maxCommands))); //110127:QDAY
        if (HCC(maxCommands) == 0)
        {
            Assert(0);
        }
        
        HCC(maxCommands)--;
        if(local_para != NULL)
            BT_SendMessage(msg, MOD_PAL, local_para, local_para->msg_len);
        else
            BT_SendMessage(msg, MOD_PAL, NULL, 0);        
        result = 0;
    }
    else
    {
        if(local_para != NULL)
            result = BT_SendMessage(msg, MOD_PAL, local_para, local_para->msg_len);
        else
            result = BT_SendMessage(msg, MOD_PAL, NULL, 0);
    }
    return result;

}


void HciSentAMPTestCommand(
                        msg_type msg,
                        local_para_struct *local_para,
                        U8 controllerId)

{
    if(local_para != NULL)
        BT_SendMessage(msg, MOD_PAL, local_para, local_para->msg_len);
    else
        BT_SendMessage(msg, MOD_PAL, NULL, 0);

}

void HciProcessAMPRxEvent(U8 *parms, U16 length, U8 controllerId)
{
    U16 event;
    switch (parms[0])
    {
        case HCE_COMMAND_COMPLETE:
            HCC(maxCommands) = parms[2];
            Report(("HciProcessAMPRxEvent COMPLETE:%d",HCC(maxCommands)));
            break;
        case HCE_COMMAND_STATUS:
            HCC(maxCommands) = parms[3];
            Report(("HciProcessAMPRxEvent STATUSE:%d",HCC(maxCommands)));
            break;
    }
    Me_HandleAmpRxHCIEventMessage(parms, length, controllerId);
}


void hciProcessAMPRxData(pal_bt_data_event_struct *ptr)
{
    HciHandle hciConnHandle;


    BTSysAdpHostSleep(0);
    hciConnHandle = ptr->u2Handle;
    hciConnHandle = hciConnHandle | (0x3000);
    HciProcessAMPReceivedData(hciConnHandle, ptr->u2Data_total_len, &ptr->aucData_p);
}

#endif

void check_pal_struct(void){
#if 0
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_CREATE_PHYSICAL_LINK_COMMAND %d %d", 
    sizeof(bt_a2mp_create_physical_link_cmd_req_struct), 
    sizeof( pal_bt_create_physical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_ACCEPT_PHYSICAL_LINK_COMMAND %d %d ",  
    sizeof(bt_a2mp_accept_physical_link_cmd_req_struct), 
    sizeof( pal_bt_accept_physical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMMAND %d %d ",  
    sizeof(bt_a2mp_physical_link_disconnect_cmd_req_struct), 
    sizeof( pal_bt_disconnect_physical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_COMMAND %d %d ",  
    sizeof(bt_a2mp_create_logical_cmd_req_struct), 
    sizeof( pal_bt_create_logical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_COMMAND %d %d ",  
    sizeof(bt_a2mp_accept_logical_cmd_req_struct),
    sizeof( pal_bt_accept_logical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMMAND %d %d ",  
    sizeof(bt_a2mp_logical_link_disconnect_cmd_req_struct), 
    sizeof( pal_bt_disconnect_logical_link_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_FLOW_SPEC_MODIFY_COMMAND %d %d ",  
    sizeof(bt_a2mp_flow_spec_modify_cmd_req_struct), 
    sizeof( pal_bt_flow_spec_modify_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMMAND %d %d ",  
    sizeof(bt_a2mp_read_local_amp_assoc_cmd_req_struct), 
    sizeof( pal_bt_read_local_amp_assoc_command_struct));
OS_Report( "[PAL][Init] MSG_ID_PAL_BT_WRITE_REMOTE_AMP_ASSOC_COMMAND %d %d ",  
    sizeof(bt_a2mp_write_remote_amp_assoc_cmd_req_struct), 
    sizeof( pal_bt_write_remote_amp_assoc_command_struct));
#endif
}


