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
#ifdef __BT_4_0_BLE__
#include "bt_common.h"
#include "btconfig.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "btutils.h"
#include "btconfig_le.h"
#include "btbm.h"
#include "btbm_int.h"
#include "me_adp.h"
#include "le_adp.h"
#include "att.h"
#include "attdef.h"
#include "gattcb.h"

#define BTBM_GATT_OP_NUM 6

typedef enum
{
    BOP_GATT_NOP,
    BOP_GATT_WRITE_PRIVACY,
    BOP_GATT_WRITE_RECONNECT_ADDR,
    BOP_GATT_SERVICE_SEARCH_ALL
    /* reserved: BOP_REM_NAME_REQ = 0x11,*/
} gattOpType;

extern U8 GapAttDB_Registration(void);

typedef struct
{
    MeCommandToken *token;

    /* Below are used by GATT module */
    BtGattOp gattOp; /* gattOp.parms.p pointers to the field below */
    union 
    {
        BtGattDiscoveryAllPrimaryService discService;
        BtGattDiscoveryPrimaryServiceByUUID discServiceUuid;
        BtGattReadCharacteristicUUID readCharUuid;
        BtGattReadCharacteristicValue readCharValue;
        BtGattWriteCharValueRequest writeCharValue;
        BtGattSignedWriteWithoutResponse signedWriteCharValue;
    } gattParm;
} btbmGattOp;

extern BMCONTENT_T BMController;

btbmGattOp btbmGattOps[BTBM_GATT_OP_NUM];
MeCommandToken btbmGattTokens[BTBM_GATT_OP_NUM];
BtHandler btbmGattHandler;

btbmGattOp *btbmGattGetFreeOp(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (btbmGattOps[i].token == NULL)
        {
            OS_Report("btbmGattOps[%d]",i );
            return &btbmGattOps[i];
        }
    }

    Assert(0);
    return NULL;
}

MeCommandToken *btbmGattGetFreeToken(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (btbmGattTokens[i].op.opType == BOP_NOP)
        {
            OS_Report("btbmGattTokens[%d]",i );
            return &btbmGattTokens[i];
        }
    }

    Assert(0);
    return NULL;
}

void btbmGattFreeOp(btbmGattOp *op)
{
    if (op->token)
    {
        OS_MemSet((U8 *)op->token, 0x0, sizeof(MeCommandToken));
    }
    OS_MemSet((U8 *)op, 0x0, sizeof(btbmGattOp));
}

btbmGattOp *btbmGattGetOp(BtGattOp *gattOp)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (gattOp == &btbmGattOps[i].gattOp)
        {
            return &btbmGattOps[i];
        }
    }

    return NULL;
}

btbmGattOp *btbmGattGetOpByToken(MeCommandToken *tok)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (tok == btbmGattOps[i].token)
        {
            return &btbmGattOps[i];
        }
    }

    return NULL;
}

static void btbmGattRemoveLink(BtRemoteDevice *link)
{
    Assert(link);

    if (IsNodeOnList(&link->handlers, &btbmGattHandler.node))
    {
        ME_LE_DiscLink(&btbmGattHandler, &link->bdAddr, BEC_USER_TERMINATED);
    }
}

static void btbmGattCallback(const BtEvent *Event)
{
    btbmGattOp *op;
    BtDeviceContext *bdc;
    BtEvent cbEvent;
    int i;
    
    OS_Report("btbmGattCallback event %d", Event->eType);
    switch (Event->eType)
    {
    case BTEVENT_LINK_CONNECT_CNF:
        if (Event->errCode == BEC_NO_ERROR && (bdc = DS_FindDevice(&Event->p.remDev->bdAddr)) != 0)
        {
            bdc->link = Event->p.remDev;
        }

        for (i = 0; i < BTBM_GATT_OP_NUM; i++)
        {
            if (btbmGattOps[i].token &&
                AreBdAddrsEqual(&Event->p.remDev->bdAddr, &btbmGattOps[i].token->remDev->bdAddr))
            {
                op = &btbmGattOps[i];
                switch (op->token->op.opType)
                {
                case BOP_REM_NAME_REQ:
                    if (Event->errCode == BEC_NO_ERROR)
                    {
                        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
                    }
                    else
                    {
                        cbEvent.errCode = BEC_NO_CONNECTION;
                        op->token->p.name.io.out.len = 0;
                        op->token->p.name.io.out.name = NULL;
                        cbEvent.p.meToken = op->token;
                        op->token->callback(&cbEvent);
                        op->token = NULL;
                        btbmGattFreeOp(op);
                    }
                    break;
                case BOP_GATT_WRITE_PRIVACY:
                    if (Event->errCode == BEC_NO_ERROR)
                    {
                        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
                    }
                    else
                    {
                        cbEvent.errCode = BEC_NO_CONNECTION;
                        op->token->callback(&cbEvent);
                        btbmGattFreeOp(op);
                    }
                    break;
                case BOP_GATT_SERVICE_SEARCH_ALL:
                    if (Event->errCode == BEC_NO_ERROR)
                    {
                        BT_GATT_DiscoveryAllPrimaryService(op->token->remDev, &op->gattOp);
                    }
                    else
                    {
                        cbEvent.errCode = BEC_NO_CONNECTION;
                        op->token->callback(&cbEvent);
                        btbmGattFreeOp(op);
                    }
                    break;
                default:
                    break;
                }
            }
        }
        break;

    case BTEVENT_LINK_DISCONNECT:
        break;
    }
}


static void btbmGattGetRemoateDeviceNameCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattGetRemoateDeviceNameCallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else if(op->token == NULL)  /* already callback */
    {
        btbmGattFreeOp(op);
        return;
    }
    else
    {
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
            {
                /* GAP service handle range discovered, then read value */
                BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
                
                Report(("result->flag:%d, result->length:%d", result->flag, result->length));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                {
                    btbmGattRemoveLink(op->token->remDev);
                    cbEvent.eType = op->token->eType;  /* BTEVENT_NAME_RESULT */
                    cbEvent.errCode = BEC_NO_ERROR;
                    op->token->p.name.io.out.len = result->length;
                    op->token->p.name.io.out.name = result->data;
                    cbEvent.p.meToken = op->token;
                    op->token->callback(&cbEvent);
                    op->token = NULL;
                }
                else
                {
                    goto error;
                }
            }
            break;
    
            default:
                break;;
        }
        return;
    }

error:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;  /* BTEVENT_NAME_RESULT */
    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    op->token->p.name.io.out.len = 0;
    op->token->p.name.io.out.name = NULL;
    cbEvent.p.meToken = op->token;
    op->token->callback(&cbEvent);
    btbmGattFreeOp(op);
}

static void btbmGattServiceSearchAllCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattServiceSearchAllCallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else if(op->token == NULL)  /* already callback */
    {
        btbmGattFreeOp(op);
        return;
    }
    else
    {
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE:
            {
                /* GAP service handle range discovered, then read value */
                BtGattDiscoveryAllPrimaryServiceResultEvent *result = event->parms;
                
                Report(("result->flag:%d, handle 0x%x~0x%x, type=%d, uuid2=0x%x", 
                    result->flag, result->startHandle, result->endHandle, result->type, result->uuid2));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                {
                    if (result->type == BT_UUID16_SIZE)
                    {
                        if (result->uuid2 >= 0x1800 && result->uuid2 < 0x1820)
                        {
                            op->token->p.gattReq.u.service_list |= ((U32) 1 << (result->uuid2 - 0x1800));
                        }
                    }
                    else if (result->type == BT_UUID128_SIZE)
                    {
                    }
                    else
                    {
                        Assert(0);
                    }
                }
                else
                {
                    goto error;
                }
            }
            break;
    
            default:
                break;;
        }
        return;
    }

error:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;  /* BTEVENT_NAME_RESULT */
    cbEvent.errCode = BEC_NO_ERROR;
    cbEvent.p.meToken = op->token;
    op->token->callback(&cbEvent);
    btbmGattFreeOp(op);
}

static void btbmGattWriteRemotePrivacyCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattWriteRemotePrivacyCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
        {
            /* value and handle is read, then write */
            BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
            
            Report(("result->flag:%d, result->length:%d", result->flag, result->length));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    if (result->length == 1 && *result->data == op->token->p.gattReq.u.privacy)
                    {
                    }
                    else
                    {
                        btbmGattOp *op2 = btbmGattGetFreeOp();
                        BtGattWriteCharValueRequest *parm = &op2->gattParm.writeCharValue;
                        
                        InitializeListEntry(&op2->gattOp.op.node);
                        op2->token = op->token;
                        op->token = NULL;  /* op2 continue token operation */
                        op2->gattOp.apCb = btbmGattWriteRemotePrivacyCallBack;
                        parm->handle = result->handle;
                        parm->size = 0x1;
                        parm->data = &op2->token->p.gattReq.u.privacy;
                        op2->gattOp.parms.p.writeRequest = parm;
                        BT_GATT_WriteCharacteristicValue(op2->token->remDev, &op2->gattOp);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    if (op->token)  /* operation done */
                    {
                        cbEvent.errCode = BEC_NO_ERROR;
                        goto done;
                    }
                    else /* op2 operation continuing */
                    {
                         btbmGattFreeOp(op);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }
        }
        break;

        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;
//    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    if (op->token->callback)
    {
        op->token->callback(&cbEvent);
    }
    btbmGattFreeOp(op);
}

static void btbmGattWriteRemoteReconnectAddressCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattWriteRemoteReconnectAddressCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
        {
            /* value and handle is read, then write */
            BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
            
            Report(("result->flag:%d, result->length:%d", result->flag, result->length));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    if (result->length == 6 && OS_MemCmp(result->data, 6, op->token->p.gattReq.u.recon.addr, 6))
                    {
                    }
                    else
                    {
                        btbmGattOp *op2 = btbmGattGetFreeOp();
                        BtGattWriteCharValueRequest *parm = &op2->gattParm.writeCharValue;
                        
                        InitializeListEntry(&op2->gattOp.op.node);
                        op2->token = op->token;
                        op->token = NULL;  /* op2 continue token operation */
                        op2->gattOp.apCb = btbmGattWriteRemoteReconnectAddressCallBack;
                        parm->handle = result->handle;
                        parm->size = BD_ADDR_SIZE;
                        parm->data = op->token->p.gattReq.u.recon.addr;
                        op2->gattOp.parms.p.writeRequest = parm;
                        BT_GATT_WriteCharacteristicValue(op2->token->remDev, &op2->gattOp);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    if (op->token)  /* operation done */
                    {
                        cbEvent.errCode = BEC_NO_ERROR;
                        goto done;
                    }
                    else /* op2 operation continuing */
                    {
                         btbmGattFreeOp(op);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }
        }
        break;

        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;
//    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    if (op->token->callback)
    {
        op->token->callback(&cbEvent);
    }
    btbmGattFreeOp(op);
}

static void btbmGattHandleRemNameReqTimeout(EvmTimer *timer)
{
    BtStatus status;
    MeCommandToken *nameToken = (MeCommandToken*) timer->context;
    BtEvent nameCbEvent;
    btbmGattOp *op = btbmGattGetOpByToken(nameToken);


    OS_Report("btbmGattHandleRemNameReqTimeout: tokenID=%d", btbmUtilGetNameTokenID(nameToken));
    timer->func = 0;

    OS_MemSet((U8*) & nameCbEvent, 0, sizeof(nameCbEvent));
    nameCbEvent.eType = nameToken->eType;
    nameCbEvent.errCode = BEC_HOST_TIMEOUT;
    nameCbEvent.p.meToken = nameToken;
    nameCbEvent.p.meToken->p.name.io.out.len = 0;
    nameToken->callback(&nameCbEvent);
    op->token = NULL;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_Init
 * DESCRIPTION
 *  Module init
 * PARAMETERS
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_Init(void)
{
    OS_MemSet((U8 *)btbmGattOps, 0x0, sizeof(btbmGattOp) * BTBM_GATT_OP_NUM);
    OS_MemSet((U8 *)btbmGattTokens, 0x0, sizeof(MeCommandToken) * BTBM_GATT_OP_NUM);
    GapAttDB_Registration();
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_GetRemoteDeviceName
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_GetRemoteDeviceName(MeCommandToken *token)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL isOnOpList = FALSE, isOnCmdList = FALSE;
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    if (token == NULL)
    {
        goto done;
    }

    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_GETREMOTEDEVICENAME, MEC(stackState), btbmUtilGetNameTokenID(token));
    if ((token->callback == 0) || (isOnOpList = IsNodeOnList(&MEC(opList), &(token->op.node))) == TRUE ||
        (isOnCmdList = IsNodeOnList(&MEC(cmdList), &(token->op.node))) == TRUE)
    {
        bt_trace(TRACE_ERROR, BT_MEIF_ME_LE_GETREMOTEDEVICENAME1, token->callback, isOnOpList, isOnCmdList);
        goto done;
    }

    /* Set up callback token */
    token->op.opType = BOP_REM_NAME_REQ;
    token->eType = BTEVENT_NAME_RESULT;
    token->tokenTimer.func = btbmGattHandleRemNameReqTimeout;
    token->tokenTimer.context = token;
    EVM_StartTimer(&token->tokenTimer, 10000);

    op = btbmGattGetFreeOp();
    parm = &op->gattParm.readCharUuid;
    InitializeListEntry(&op->gattOp.op.node);
    op->token = token;
    op->gattOp.apCb = btbmGattGetRemoateDeviceNameCallBack;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_DEVICE_NAME;
    op->gattOp.parms.p.readCharUUID = parm;

    if ((token->remDev = ME_FindRemoteDevice(&token->p.name.bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, &token->p.name.bdAddr, NULL, &token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
    }
    
    status = BT_STATUS_PENDING;

done:
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_ServiceSearchAll
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_ServiceSearchAll(BD_ADDR *bdAddr, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattDiscoveryAllPrimaryService *parm;

    OS_Report("BTBMGATT_ServiceSearchAll");
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_ServiceSearchAll: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_SERVICE_SEARCH_ALL;
    op->token->eType = SDEVENT_QUERY_RSP;
    op->token->p.gattReq.bdAddr = *bdAddr;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattServiceSearchAllCallBack;
    parm = &op->gattParm.discService;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    op->gattOp.parms.p.allPrimaryService = parm;

    if ((op->token->remDev = ME_FindRemoteDevice(bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, bdAddr, NULL, &op->token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_DiscoveryAllPrimaryService(op->token->remDev, &op->gattOp);
    }
    
    return BT_STATUS_PENDING;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_WriteRemotePrivacy
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  privacy       [IN]        
 *  callback      [IN]
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_WriteRemotePrivacy(BD_ADDR *bdAddr,BOOL privacy, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    OS_Report("BTBMGATT_WriteRemotePrivacy: privacy=%d", privacy);
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_WriteRemotePrivacy: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_WRITE_PRIVACY;
    op->token->eType = BTEVENT_GATT_WRITE_PRIVACY;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->p.gattReq.bdAddr = *bdAddr;
    op->token->p.gattReq.u.privacy = privacy;
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattWriteRemotePrivacyCallBack;
    parm = &op->gattParm.readCharUuid;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PRIVACY_FLAG;
    op->gattOp.parms.p.readCharUUID = parm;

    if ((op->token->remDev = ME_FindRemoteDevice(bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, bdAddr, NULL, &op->token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
    }
    
    return BT_STATUS_PENDING;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_WriteRemoteReconnectAddress
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  privacy       [IN]        
 *  callback      [IN]
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_WriteRemoteReconnectAddress(BD_ADDR *bdAddr, BD_ADDR *reconAddr, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    OS_Report("BTBMGATT_WriteRemoteReconnectAddress");
    BTBMDumpBdAddr(bdAddr->addr);
    BTBMDumpBdAddr(reconAddr->addr);
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_WriteRemoteReconnectAddress: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_WRITE_RECONNECT_ADDR;
    op->token->eType = BTEVENT_GATT_WRITE_RECONNECT_ADDR;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->p.gattReq.bdAddr = *bdAddr;
    op->token->p.gattReq.u.recon = *reconAddr;
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattWriteRemoteReconnectAddressCallBack;
    parm = &op->gattParm.readCharUuid;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_RECONNECTION_ADDRESS;
    op->gattOp.parms.p.readCharUUID = parm;

    if ((op->token->remDev = ME_FindRemoteDevice(bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, bdAddr, NULL, &op->token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
    }
    
    return BT_STATUS_PENDING;
}

BtStatus BTBMGATT_Cancel(BtRemoteDevice *link)
{
    btbmGattRemoveLink(link);
    return BT_STATUS_SUCCESS;
}

static void UPF_SignedWriteCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("UPF_SignedWriteCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattFreeOp(op);
}

void UPF_SignedWrite(btbm_bd_addr_t *addr)
{
    U16 handle = 0;
    U8 size = 0;
    U8 data[64];
    BD_ADDR bdAddr;
    BtRemoteDevice *remDev; 
    btbmGattOp *op;
    BtGattSignedWriteWithoutResponse *parm;

    OS_Report("UPF_SignedWrite");
    CONVERT_BDADDRSRC2ARRAY(bdAddr.addr, addr);
    BTBMDumpBdAddr(bdAddr.addr);

    remDev = ME_FindRemoteDevice(&bdAddr);
    if (remDev)
    {
        op = btbmGattGetFreeOp();
        parm = &op->gattParm.signedWriteCharValue;
        InitializeListEntry(&op->gattOp.op.node);
        op->token = btbmGattGetFreeToken();
        op->gattOp.apCb = UPF_SignedWriteCallBack;
        DDB_ReadUPF_ATT(&handle, &size, data);
        parm->handle = handle;
        parm->size = size;
        parm->data = data;
        op->token->remDev = remDev;
        op->gattOp.parms.p.signedWriteWithoutResponse = parm;
        BT_GATT_SignedWriteWithoutResponse(op->token->remDev, &op->gattOp);
    }
}


U8 BtbmAttHandleDeviceName(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    U8 status = BT_ATT_ERROR_CODE_NO_ERROR;
    bt_bm_write_local_name_req_struct req;

    switch (type)
    {
        case ATT_OP_READ:
            *len = BMController.bm_setting.len;
            *raw = (U8 *)BMController.bm_setting.local_name;
			OS_Report("[GAP] read name: %s", BMController.bm_setting.local_name);
            break;
        case ATT_OP_WRITE:
            OS_Report("[GAP] write name: %s",  *raw);
            req.name_len = *len;
            OS_MemSet(req.name, 0, BTBM_ADP_MAX_NAME_LEN);
            OS_MemCopy(req.name, *raw, *len);
            BTBMGapWriteLocalName(&req);
            break;
    }
    return status;
}

U8 BtbmAttHandleAppearance(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    U8 status = BT_ATT_ERROR_CODE_NO_ERROR;

    switch (type)
    {
        case ATT_OP_READ:
            *len = 2;
            *raw = BMController.bm_setting.appearance;
			OS_Report("[GAP] read appearance: 0x%x, 0x%x", BMController.bm_setting.appearance[0], BMController.bm_setting.appearance[1]);
            break;
        case ATT_OP_WRITE:
            break;
    }
    return status;
}

#ifdef __BT_4_0_PRIVACY__
U8 BtbmAttHandlePrivacy(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    U8 status = BT_ATT_ERROR_CODE_NO_ERROR;

    switch (type)
    {
        case ATT_OP_READ:
            break;
        case ATT_OP_WRITE:
            break;
    }
    return status;
}

U8 BtbmAttHandleReconnectAddress(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    U8 status = BT_ATT_ERROR_CODE_NO_ERROR;

    switch (type)
    {
        case ATT_OP_READ:
            break;
        case ATT_OP_WRITE:
            break;
    }
    return status;
}

#endif
#endif
