#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "att_i.h"
#include "att.h"
#include "attdbmain.h"

#if ATT_PROTOCOL == XA_ENABLED
extern U16 ATTDb_CheckHandleSize(U16 recordHandle);


/****************************************************************************
 *
 * Internal Function prototypes
 *
 *
 */
void BT_ATT_HandleRxReady(BtRemoteDevice *link, U8 *data, U16 length)
{
    U8 opcode;
    BtAttConn *conn;
    U8 *payload;
    U8 payload_len;
    
    conn = BT_ATT_GetConCtx(link);
    opcode = data[0];
    payload = data+1;
    payload_len = length -1;
    
    OS_Report("BT_ATT_HandleRxReady(): opcode=%d", opcode);
    switch (opcode)
    {
        case BT_ATT_OPCODE_ERROR_RESPONSE:
            BT_ATT_HandleErrorResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST:
            BT_ATT_HandleExchangeMTURequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE:
            BT_ATT_HandleExchangeMTUResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_FIND_INFO_REQUEST:
            BT_ATT_HandleFindInfoRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_FIND_INFO_RESPONSE:
            BT_ATT_HandleFindInfoResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST:
            BT_ATT_HandleFindByTypeValueRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE:
            BT_ATT_HandleFindByTypeValueResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BY_TYPE_REQUEST:
            BT_ATT_HandleReadByTypeRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE:
            BT_ATT_HandleReadByTypeResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_REQUEST:
            BT_ATT_HandleReadRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_RESPONSE:
            BT_ATT_HandleReadResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BLOB_REQUEST:
            BT_ATT_HandleReadBlobRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BLOB_RESPONSE:
            BT_ATT_HandleReadBlobResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_MULTIPLE_REQUEST:
            BT_ATT_HandleReadMultipleRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE:
            BT_ATT_HandleReadMultipleResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST:
            BT_ATT_HandleReadByGroupTypeRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE:
            BT_ATT_HandleReadByGroupTypeResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_WRITE_REQUEST:
            BT_ATT_HandleWriteRequest(conn, payload, payload_len);
            break;  
        case BT_ATT_OPCODE_WRITE_RESPONSE:
            BT_ATT_HandleWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_WRITE_COMMAND:
            BT_ATT_HandleWriteCommand(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_PREPARE_WRITE_REQUEST:
            BT_ATT_HandlePrepareWriteRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE:
            BT_ATT_HandlePrepareWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST:
            BT_ATT_HandleExecuteWriteRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_EXECUTE_WRITE_RESPONSE:
            BT_ATT_HandleExecuteWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION:
            BT_ATT_HandleHandleValueNotification(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_INDICATION:
            BT_ATT_HandleHandleValueIndication(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION:
            BT_ATT_HandleHandleValueConfirmation(conn, payload, payload_len);
            break;  
        case BT_ATT_OPCODE_SIGNED_WRITE_COMMAND:
            payload = data;
            payload_len = length;
            BT_ATT_HandleHandleSignedWriteCommand(conn, payload, payload_len);
            break;  

    }
    
}

void BT_ATT_ReportEvent(U8 event_code, BtAttConn *conn, void *parm)
{
    OS_Report("BT_ATT_ReportEvent(): event_code=%d", event_code);
    ATTC(att_event).eType = event_code;
    ATTC(att_event).link = conn->link;
    ATTC(att_event).parms = (void *)parm;
    if(ATTC(callback) !=0)
        ATTC(callback)(&ATTC(att_event));
}


void BT_ATT_HandleErrorResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTErrorResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.opcode = data[0];
    ptr.attributeHandle = LEtoHost16(data+1);
    ptr.errorCode = data[3];
    OS_Report("BT_ATT_HandleErrorResponse: opcode=0x%x, errCode=0x%x",ptr.opcode, ptr.errorCode);
    BT_ATT_ReportEvent(BT_ATT_EVENT_ERROR_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleExchangeMTURequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExchangeMTURequestStruct ptr;
    BtATTExchangeMTUResponseStruct rsp;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.clientRxMtu = LEtoHost16(data);
    if(ptr.clientRxMtu < conn->local_desired_mtu)
    {
        conn->mtu = ptr.clientRxMtu;
    }
    else
    {
        conn->mtu = conn->local_desired_mtu;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXCHANGE_MTU_REQUEST, conn, &ptr);
    rsp.serverRxMtu = conn->mtu;
    BT_ATT_SendExchangeMTUResponse(&rsp,conn->link);
}

void BT_ATT_HandleExchangeMTUResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExchangeMTUResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.serverRxMtu = LEtoHost16(data);
    if(ptr.serverRxMtu < conn->mtu)
        conn->mtu = ptr.serverRxMtu;
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXCHANGE_MTU_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleFindInfoRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindInfoRequestStruct ptr;
    BtATTFindInfoResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);    
    if((ptr.startingHandle > ptr.endingHandle) ||
       (ptr.startingHandle == 0x00))
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_INFO_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_INFO_REQUEST, conn, &ptr);
    size =  ATTDb_HandleGetInfoReq(&ATTC(tempBuffer), conn->mtu - 2, &rsp.format, &ptr);
    if(size >0)
    {
        rsp.infoData = &ATTC(tempBuffer);
        rsp.infoDataLen = size;
        BT_ATT_SendFindInfoResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_INFO_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }

}

void BT_ATT_HandleFindInfoResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindInfoResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.format = data[0];
    ptr.infoDataLen = length -1;
    ptr.infoData = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_INFO_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleFindByTypeValueRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindByTypeValueRequestStruct ptr;
    BtATTFindByTypeValueResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);
    ptr.attributeType = LEtoHost16(data+4);
    ptr.attributeValueLen = length -6 ;
    ptr.attributeValue = data+6;    

    OS_Report("BT_ATT_HandleFindByTypeValueRequest(): 0x%x~0x%x, 0x%x, %d, 0x%x, 0x%x", 
        ptr.startingHandle, ptr.endingHandle, ptr.attributeType, ptr.attributeValueLen, ptr.attributeValue[0], ptr.attributeValue[1]);

    if((ptr.startingHandle > ptr.endingHandle) ||
       (ptr.startingHandle == 0x00))
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }
    
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_BY_TYPE_VALUE_REQUEST, conn, &ptr);
    size =  ATTDb_HandleGetFindByTypeValueReq(&ATTC(tempBuffer), conn->mtu-1, &ptr, conn->link);
    if(size >0)
    {
        rsp.handleInfoList = &ATTC(tempBuffer);
        rsp.handleInfoListLen = size;
        BT_ATT_SendFindByTypeValueResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleFindByTypeValueResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindByTypeValueResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.handleInfoListLen = length;
    ptr.handleInfoList = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_BY_TYPE_VALUE_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleReadByTypeRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByTypeRequestStruct ptr;
    BtATTReadByTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code = 0x00;
    U16 error_handle;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);
    OS_Report("BT_ATT_HandleReadByTypeRequest(): 0x%x~0x%x", ptr.startingHandle, ptr.endingHandle);

    if((length - 4) == 2)
    {
        ptr.type = 2;
        ptr.attributeType2 = LEtoHost16(data+4);
    }
    else
    {
        ptr.type = 16;
        ptr.attributeType = data+4;
    }
    if(ptr.startingHandle > ptr.endingHandle)
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }    
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_TYPE_REQUEST, conn, &ptr);
    size =  ATTDb_HandleReadByTypeReq(&ATTC(tempBuffer), conn->mtu-2, &ptr, &rsp.length, conn->link, &error_code, &error_handle);
    if(size >0)
    {
        rsp.handleDataList = &ATTC(tempBuffer);
        rsp.handleDataListLen = size;
        BT_ATT_SendReadByTypeResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event failed with error code:%d", error_code));
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }        

}

void BT_ATT_HandleReadByTypeResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByTypeResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.length = data[0];
    ptr.handleDataListLen = length -1;
    ptr.handleDataList = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_TYPE_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleReadRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadRequestStruct ptr;
    BtATTReadResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.handle = LEtoHost16(data);
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_REQUEST, conn, &ptr);
    size =  ATTDb_HandleReadReq(&ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, conn->link);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeValue = &ATTC(tempBuffer);
        rsp.attributeValueLen= size;
        BT_ATT_SendReadResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }            
}

void BT_ATT_HandleReadResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.attributeValueLen = length;
    ptr.attributeValue = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleReadBlobRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadBlobRequestStruct ptr;
    BtATTReadBlobResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.valueOffset = LEtoHost16(data+2);   
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BLOB_REQUEST, conn, &ptr);
    size =  ATTDb_HandleReadBlobReq(&ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, conn->link);
    if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.partAttributeValue = &ATTC(tempBuffer);
        rsp.partAttributeValueLen = size;
        BT_ATT_SendReadBlobResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BLOB_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }            

}

void BT_ATT_HandleReadBlobResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadBlobResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.partAttributeValueLen = length;
    ptr.partAttributeValue = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BLOB_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleReadMultipleRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadMultipleRequestStruct ptr;
    BtATTReadMultipleResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    U16 error_handle;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.setOfHandlesLen = length;
    ptr.setOfHandles = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_MULTIPLE_REQUEST, conn, &ptr);

    size =  ATTDb_HandleReadMultipleReq(&ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, &error_handle, conn->link);
    if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.setOfValues = &ATTC(tempBuffer);
        rsp.setOfValuesLen = size;
        BT_ATT_SendReadMultipleResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_MULTIPLE_REQUEST; 
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }

    
}

void BT_ATT_HandleReadMultipleResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadMultipleResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.setOfValuesLen = length;
    ptr.setOfValues = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_MULTIPLE_RESPONSE, conn, &ptr);
}


void BT_ATT_HandleReadByGroupTypeRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByGroupTypeRequestStruct ptr;
    BtATTReadByGroupTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    U16 error_handler;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);
    if((length -4) == 2)
    {
        ptr.type = 2;
        ptr.attributeType2 = LEtoHost16(data+4);
    }
    else
    {
        ptr.type = 16;
        ptr.attributeType = data+4;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_GROUP_TYPE_REQUEST, conn, &ptr);
    
    size =  ATTDb_HandleReadGroupReq(&ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, &error_handler, conn->link);
    if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.length = ATTC(tempBuffer)[0];
        rsp.attributeDataList = &ATTC(tempBuffer)[1];
        rsp.attributeDataListLen = size-1;
        BT_ATT_SendReadByGroupTypeResponse(&rsp, conn->link);
    }
    else
    {
        if(error_code == BT_ATT_ERROR_CODE_READ_NOT_PERMITTED)
            rsp_err.attributeHandle = error_handler;
        else
            rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleReadByGroupTypeResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByGroupTypeResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.length = data[0];
    ptr.attributeDataListLen = length -1;
    ptr.attributeDataList = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_GROUP_TYPE_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTWriteRequestStruct ptr;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValue = data+2;
    ptr.attributeValueLen = length -2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_REQUEST, conn, &ptr);
    error_code = ATTDb_HandleWriteRequest(&ATTC(tempBuffer),&ptr, conn->link, TRUE);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        BT_ATT_SendWriteResponse(conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    
}

void BT_ATT_HandleWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_RESPONSE, conn, NULL);
}

void BT_ATT_HandleWriteCommand(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTWriteCommandStruct ptr;

    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_COMMAND, conn, &ptr);
    ATTDb_HandleWriteRequest(&ATTC(tempBuffer),&ptr, conn->link, FALSE);
    /* Not required to send response */
}


void BT_ATT_HandlePrepareWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTPrepareWriteRequestStruct ptr;
    BtATTPrepareWriteResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code;
    U8 i=0;
    U16 size;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.valueOffset = LEtoHost16(data+2);
    ptr.partAttributeValueLen = length-4;
    ptr.partAttributeValue = data+4;
    BT_ATT_ReportEvent(BT_ATT_EVENT_PREPARE_WRITE_REQUEST, conn, &ptr);
    error_code = ATTDb_CheckHandlePermission(ptr.attributeHandle, ATT_OP_WRITE);

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        size = ATTDb_CheckHandleSize(ptr.attributeHandle);
        if(ptr.valueOffset > size)
        {
            error_code = BT_ATT_ERROR_CODE_INVALID_OFFSET;
        }
    }

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
        {
            if(conn->prepareWriteBuffer[i].available == 0)
            {
                conn->prepareWriteBuffer[i].available = 1;
                conn->prepareWriteBuffer[i].handle = ptr.attributeHandle;
                conn->prepareWriteBuffer[i].offset = ptr.valueOffset;
                conn->prepareWriteBuffer[i].length = ptr.partAttributeValueLen;
                if(conn->prepareWriteBuffer[i].length > MAX_ATT_TX_MTU-5)
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr.partAttributeValue, MAX_ATT_TX_MTU-5);
                    conn->prepareWriteBuffer[i].length = MAX_ATT_TX_MTU-5;
                }
                else
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr.partAttributeValue, conn->prepareWriteBuffer[i].length);
                }
                break;
            }
        }
        if(i == MAX_PREPARE_WRITE_NO)
            error_code = BT_ATT_ERROR_CODE_INSUFFICIENT_RESOURCES;
        /* No buffer available*/
    }
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeHandle = ptr.attributeHandle;
        rsp.partAttributeValue = ptr.partAttributeValue;
        rsp.partAttributeValueLen = ptr.partAttributeValueLen;
        rsp.valueOffset = ptr.valueOffset;
        BT_ATT_SendPrepareWriteResponse(&rsp, conn->link);        
    }
    else
    {
        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }

}

void BT_ATT_HandlePrepareWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTPrepareWriteRequestStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.valueOffset = LEtoHost16(data+2);
    ptr.partAttributeValueLen = length-4;
    ptr.partAttributeValue = data+4;
    BT_ATT_ReportEvent(BT_ATT_EVENT_PREPARE_WRITE_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleExecuteWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExecuteWriteRequestStruct ptr;
    U8 i = 0;
    U8 error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    BtATTErrorResponseStruct rsp_err;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.flags = data[0];
    if(ptr.flags == 0x01)
    {
        for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
        {
            if(conn->prepareWriteBuffer[i].available == 1)
            {
                error_code = ATTDB_HandleExecuteWriteRequest(&conn->prepareWriteBuffer[i], conn->link);
                if(error_code != BT_ATT_ERROR_CODE_NO_ERROR)
                {
                    rsp_err.attributeHandle = conn->prepareWriteBuffer[i].handle;
                    break;
                    /* ignore error, flush all*/
                }
            }
        }
    }
    for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
    {
        conn->prepareWriteBuffer[i].available = 0;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXECUTE_WRITE_REQUEST, conn, &ptr);
    if(error_code != BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    else
    {
        BT_ATT_SendExecuteWriteResponse(conn->link);
    }
}

void BT_ATT_HandleExecuteWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXECUTE_WRITE_RESPONSE, conn, NULL);
}

void BT_ATT_HandleHandleValueNotification(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTValueNotificationStruct ptr;
    
    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_NOTIFICATION, conn, &ptr);
}

void BT_ATT_HandleHandleValueIndication(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTValueIndicationStruct ptr;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_INDICATION, conn, &ptr);
    BT_ATT_SendHandleValueConfirmation(conn->link);

}

void BT_ATT_HandleHandleValueConfirmation(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_CONFIRMATION, conn, NULL);
    
}

#ifdef __BT_4_0_BLE__
static void attSignWriteVerifyCB(const BtEvent *event)
{
    int j;
    U8 status;
    BtPacket *pkt;
    BtRemoteDevice *link = ME_FindRemoteDevice(&event->p.smSignData.bdAddr);
    BtAttConn *conn = BT_ATT_GetConCtx(link);

    switch (event->eType)
    {
        case BTEVENT_LE_SIGN_VERIFY_COMPLETE:    
        if (event->errCode == BEC_NO_ERROR)
        {
            BtATTSignedWriteCommandStruct ptr;
            ptr.attributeHandle = LEtoHost16(event->p.smSignData.data + 1);
            ptr.attributeValueLen = event->p.smSignData.dataLen - 3;
            ptr.attributeValue = event->p.smSignData.data + 3;
//            ptr.authenticationSignature = data + 2 + ptr.attributeValueLen;
            BT_ATT_ReportEvent(BT_ATT_EVENT_SIGNED_WRITE_COMMAND, conn, &ptr);
            ATTDb_HandleSignedWriteCommand(&ATTC(tempBuffer),&ptr, conn->link);
        }
        break;
    }
}

BtSecurityToken attSecurityToken;      /* Passed to ME for access requests. */
BtSecurityRecord attSecurityRecord;

static void attSignWriteSecCallback(const BtEvent *Event)
{
    SEC_Unregister(&attSecurityRecord);
}
#endif

void BT_ATT_HandleHandleSignedWriteCommand(BtAttConn *conn, U8 *data, U16 length)
{
#ifdef __BT_4_0_BLE__
    BtDeviceRecord record;
    if (SEC_FindDeviceRecord(&conn->link->bdAddr, &record) != BT_STATUS_SUCCESS ||
        (record.smKeyType & SM_KEY_CSRK) == 0)
    {
        SEC_DeleteDeviceRecord(&conn->link->bdAddr);
        attSecurityRecord.level_type = BT_SEC_MED;
        attSecurityRecord.id = attSignWriteSecCallback;
        attSecurityRecord.channel = 0x001F;
        SEC_RegisterRecord(&attSecurityRecord);
        attSecurityRecord.level = BSL_NO_SECURITY;  //overwrite
        
        attSecurityToken.remDev = conn->link;
        attSecurityToken.id = attSignWriteSecCallback;
        attSecurityToken.channel = 0x001F;
        SEC_AccessRequest(&attSecurityToken);
        return;
    }

    if (ME_LE_VerifySignedData(attSignWriteVerifyCB, &conn->link->bdAddr, data, length) == BT_STATUS_PENDING)
    {
        return;
    }

#endif
}

#endif
