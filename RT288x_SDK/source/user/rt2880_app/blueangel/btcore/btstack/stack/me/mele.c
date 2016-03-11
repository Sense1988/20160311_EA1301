/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
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
 * mele.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy HCI usage
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_4_0_BLE__

#include "bt_common.h"
#include "btconfig_le.h"
#include "btalloc.h"
#include "utils.h"
#include "btutils.h"
#include "hci.h"
#include "lei.h"
#include "me_adp.h"

#define CONTROLLER_NOT_RETURN_ADDR_WHEN_FAILED

void LE_Init(void)
{
    btCtxLe.advCur = 0xFF;
    SM_Init();
}

static void LeHandleTaskEnd(void)
{
    int i;

    btCtxLe.hciCmd[btCtxLe.curCmdIdx].cmd = 0;
    btCtxLe.curCmdIdx++;
    btCtxLe.curCmdIdx %= LE_HCI_CMD_LIST_SIZE;
    OS_Report("curCmdIdx = %d", btCtxLe.curCmdIdx);
    /* Refresh command list to next */
    for (i = 0; i < LE_HCI_CMD_LIST_SIZE - 1; i++)
    {
        OS_Report("%d: 0x%x", i, btCtxLe.hciCmd[i].cmd);
    }
}

static void LeHandleErrorTaskEnd(void)
{
    Assert(0); 
}

static LeHciCmd *LeAllocHciToken(void)
{
    int i;

    for (i = 0; i < LE_HCI_CMD_LIST_SIZE; i++)
    {
        int idx = (btCtxLe.curCmdIdx + i) % LE_HCI_CMD_LIST_SIZE;
        if (btCtxLe.hciCmd[idx].cmd == 0)
        {
            return &btCtxLe.hciCmd[idx];
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BT_MELE_LEALLOCHCITOKEN, btCtxLe.hciCmd[idx].cmd);
        }
    }
    Assert(0); 
    return NULL;
}

LeHciCmd *LeFindHciToken(BtOperation *op)
{
    int i;

    for (i = 0; i < LE_HCI_CMD_LIST_SIZE; i++)
    {
        if (op == &btCtxLe.hciCmd[i].op)
        {
            return &btCtxLe.hciCmd[i];
        }
    }
    Assert(0); 
    return NULL;
}

U8 leUtilComposeAdvertiseData(U8 *buf, U32 bufLen, LeAdFlags flags, const U8 *name, U32 nameLen)
{
    U8 offset = 0;
    
    Assert(bufLen == LE_ADV_DATA_SIZE);
    OS_MemSet(buf, 0x0, LE_ADV_DATA_SIZE);

    bt_trace(TRACE_FUNC, BT_MELE_LEUTILCOMPOSEADVERTISEDATA, flags, nameLen);
    OS_Report("name=%s", name);
    
    /* Flags */
    if (flags != 0)
    {
        buf[offset++] = 2;
        buf[offset++] = BT_EIR_FLAGS;
        buf[offset++] = flags;
    }

    /* Local name */
    offset += meUtilWriteEirName(buf + offset, LE_ADV_DATA_SIZE - offset, name, nameLen);
    Assert(offset <= LE_ADV_DATA_SIZE);

#if BT_LE_ADV_DATA_SERVICE == TRUE
    /* Service */
    offset += meUtilWriteEirBLEServiceList(buf + offset, LE_ADV_DATA_SIZE - offset);
    Assert(offset <= LE_ADV_DATA_SIZE);
#endif
    return offset;
}

U8 leUtilComposeScanRspData(U8 *buf, U32 bufLen, const U8 *name, U32 nameLen)
{
    U32 offset = 0;
    
    Assert(bufLen == LE_ADV_DATA_SIZE);
    OS_MemSet(buf, 0x0, LE_ADV_DATA_SIZE);
    OS_Report("leUtilComposeScanRspData: name=%s", name);
    
    /* Local name */
    offset += meUtilWriteEirName(buf + offset, LE_ADV_DATA_SIZE - offset, name, nameLen);
    Assert(offset <= LE_ADV_DATA_SIZE);

#if BT_LE_ADV_DATA_SERVICE == TRUE
    /* Service */
    offset += meUtilWriteEirBLEServiceList(buf + offset, LE_ADV_DATA_SIZE - offset);
    Assert(offset <= LE_ADV_DATA_SIZE);
#endif
    return offset;
}

BtStatus leUtilUpdateAdvertiseData(void)
{
    U8 pdu[LE_ADV_DATA_SIZE];
    U8 len;
    #ifdef __BT_LE_STANDALONE__
    LeAdFlags flags = BEF_BR_EDR_NOT_SUPPORTED;
    #else
    LeAdFlags flags = 0;
    #endif
    
    if (btCtxMe.accModeExpValue == BAM_GENERAL_ACCESSIBLE || btCtxMe.accModeExpValue == BAM_DISCOVERABLE_ONLY)
    {
        flags |= BEF_LE_GENERAL_DISCOVERABLE_MODE;
    }
    else if (btCtxMe.accModeExpValue == BAM_LIMITED_ACCESSIBLE || btCtxMe.accModeExpValue == BAM_LIMITED_DISCOVERABLE)
    {
        flags |= BEF_LE_LIMITED_DISCOVERABLE_MODE;
    }
    len = leUtilComposeAdvertiseData(pdu, 
                                LE_ADV_DATA_SIZE,
                                flags,
                                btCtxMe.localName, 
                                btCtxMe.localNameLen);
    if (OS_MemCmp(btCtxLe.advData, btCtxLe.advDataLen, pdu, len) == FALSE)
    {
        OS_MemCopy(btCtxLe.advData, pdu, len);
        btCtxLe.advDataLen = len;
        LeHciSetAdvertiseData(len, pdu);
        len = leUtilComposeScanRspData(pdu, 
                                  LE_ADV_DATA_SIZE,
                                  btCtxMe.localName, 
                                  btCtxMe.localNameLen);
        LeHciSetScanRspData(len, pdu);
        return BT_STATUS_PENDING;
    }
    return BT_STATUS_SUCCESS;
}

#ifdef __BT_4_0_PRIVACY__
void leUtilGenRandomStaticAddrress(BD_ADDR *out)
{
    U32 value32 = (U32)OS_Rand32();
    U16 value16 = OS_Rand();

    if ((value32 == 0xFFFFFFFF && ((value16 & 0x3FFF) == 0x3FFF)) ||
        (value32 == 0 && ((value16 & 0x3FFF) == 0)))
    {
        leUtilGenRandomStaticAddrress(out);
        return;
    }
    StoreLE32(out->addr, value32);
    StoreLE16(&out->addr[4], value16);
    out->addr[5] |= 0xC0;
}

void leUtilGenRandomPrivateNonResolvableAddress(BD_ADDR *out)
{
    U32 value32 = (U32)OS_Rand32();
    U16 value16 = OS_Rand();

    StoreLE32(out->addr, value32);
    StoreLE16(&out->addr[4], value16);
    out->addr[5] &= 0x3F;
    if ((value32 == 0xFFFFFFFF && ((value16 & 0x3FFF) == 0x3FFF)) ||
        (value32 == 0 && ((value16 & 0x3FFF) == 0)) ||
        AreBdAddrsEqual(out,&MEC(bdaddr)))
    {
        leUtilGenRandomPrivateNonResolvableAddress(out);
        return;
    }
}

#ifdef __BT_LE_STANDALONE__
void leUtilGenRandomPrivateResolvableAddressPrand(U8 *out)
{
    U32 value32 = (U32)OS_Rand32() & 0x3FFFFF;
    if (value32 == 0x3FFFFF || value32 == 0)
    {
        leUtilGenRandomPrivateResolvableAddressPrand(out);
        return;
    }
    out[2] = (U8) (value32 >> 16);
    out[1] = (U8) (value32 >> 8);
    out[0] = (U8) value32;
    out[2] |= 0x40;
    out[2] &= 0x7F;
}

void leUtilGenRandomPrivateResolvableAddressCB(const BtEvent *event)
{
    BtEvent cbEvent;
        
    cbEvent.errCode = BEC_NO_ERROR;
    cbEvent.p.bdAddr.addr[0] = event->p.smEncryptResponse.EncryptData[0];
    cbEvent.p.bdAddr.addr[1] = event->p.smEncryptResponse.EncryptData[1];
    cbEvent.p.bdAddr.addr[2] = event->p.smEncryptResponse.EncryptData[2];
    cbEvent.p.bdAddr.addr[3] = btCtxLe.op.genRandomAddr.prand[0];
    cbEvent.p.bdAddr.addr[4] = btCtxLe.op.genRandomAddr.prand[1];
    cbEvent.p.bdAddr.addr[5] = btCtxLe.op.genRandomAddr.prand[2];
    btCtxLe.op.genRandomAddr.cb(&cbEvent);
}

void leUtilGenRandomPrivateResolvableAddress(BtCallBack cb)
{
    U8 irk[SM_IRK_VALUE_LENGTH];
    U8 r1[SM_IRK_VALUE_LENGTH] = {0};

    btCtxLe.op.genRandomAddr.cb = cb;
    leUtilGenRandomPrivateResolvableAddressPrand(btCtxLe.op.genRandomAddr.prand);
    /* ah(k, r) = e(k, r') mod 2^24 */
    OS_MemCopy(r1 + 13, btCtxLe.op.genRandomAddr.prand, 3);
    DDB_ReadLeKey(NULL, NULL, irk);
    LeHciAES128Encrypt(leUtilGenRandomPrivateResolvableAddressCB, irk, r1);
}
#else
void leUtilResolveRandomPrivateResolvableAddressCB(const BtEvent *event)
{
    BtEvent cbEvent;
    BtDeviceRecord rec;

    /* Check hash */
    OS_Report("leUtilResolveRandomPrivateResolvableAddressCB: index=%d", btCtxLe.op.resolveRandomAddr.recIndex);
    if (OS_MemCmp(btCtxLe.op.resolveRandomAddr.resAddr, 3, event->p.smEncryptResponse.EncryptData, 3))
    {
        cbEvent.errCode = BEC_NO_ERROR;
        if (DDB_EnumDeviceRecords(btCtxLe.op.resolveRandomAddr.recIndex, &rec) != BT_STATUS_SUCCESS)
        {
            Assert(0);
        }
        cbEvent.p.bdAddr = rec.bdAddr;
    }
    else
    {
        U8 r1[SM_IRK_VALUE_LENGTH] = {0};
    
        /* ah(k, r) = e(k, r') mod 2^24 */
        OS_MemCopy(r1 + 13, &btCtxLe.op.resolveRandomAddr.resAddr[3], 3);
        while (DDB_EnumDeviceRecords(btCtxLe.op.resolveRandomAddr.recIndex, &rec) == BT_STATUS_SUCCESS)
        {
            if (rec.smKeyType & SM_KEY_IRK)
            {
                LeHciAES128Encrypt(leUtilResolveRandomPrivateResolvableAddressCB, rec.irk, r1);
                return;
            }
            btCtxLe.op.resolveRandomAddr.recIndex++;
        }
        cbEvent.errCode = BEC_NOT_FOUND;
    }
    btCtxLe.op.genRandomAddr.cb(&cbEvent);
}

void leUtilResolveRandomPrivateResolvableAddress(BD_ADDR *resAddr, BtCallBack cb)
{
    U8 irk[SM_IRK_VALUE_LENGTH];
    U8 r1[SM_IRK_VALUE_LENGTH] = {0};
    BtDeviceRecord rec;
    BtEvent cbEvent;

    Assert(resAddr != NULL && cb != NULL);
    OS_Report("leUtilResolveRandomPrivateResolvableAddress");
    BTBMDumpBdAddr(resAddr->addr);
    /* ah(k, r) = e(k, r') mod 2^24 */
    OS_MemCopy(r1 + 13, &resAddr[3], 3);
    OS_MemCopy(btCtxLe.op.resolveRandomAddr.resAddr, resAddr, BD_ADDR_SIZE);
    btCtxLe.op.resolveRandomAddr.cb = cb;
    btCtxLe.op.resolveRandomAddr.recIndex = 0;
    while (DDB_EnumDeviceRecords(btCtxLe.op.resolveRandomAddr.recIndex, &rec) == BT_STATUS_SUCCESS)
    {
        if (rec.smKeyType & SM_KEY_IRK)
        {
            LeHciAES128Encrypt(leUtilResolveRandomPrivateResolvableAddressCB, rec.irk, r1);
            return;
        }
        btCtxLe.op.resolveRandomAddr.recIndex++;
    }
    cbEvent.errCode = BEC_NOT_FOUND;
    cb(&cbEvent);
}
#endif
#endif

void LeHciSetEventMask(U8 *mask)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETEVENTMASK, mask[0], mask[1], mask[2], mask[3], mask[4], mask[5], mask[6], mask[7]);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_EVENT_MASK;
    OS_MemCopy(hciCmd->parms, mask, 8);
    hciCmd->parmLen = 8;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadBufferSize(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADBUFFERSIZE);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_BUFFER_SIZE;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadLocalFeatures(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADLOCALFEATURES);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_LOCAL_FEATURES;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciSetRandomAddress(BD_ADDR bdAddr)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETRANDOMADDRESS, bdAddr.addr[0], bdAddr.addr[1], bdAddr.addr[2], bdAddr.addr[3], bdAddr.addr[4], bdAddr.addr[5]);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_RANDOM_ADDRESS;
    OS_MemCopy(hciCmd->parms, bdAddr.addr, BD_ADDR_SIZE);
    hciCmd->parmLen = BD_ADDR_SIZE;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

/*****************************************************************************
 * FUNCTION
 *  LeHciSetAdvertiseParm
 * DESCRIPTION
 *  Get available packet from pool
 * PARAMETERS
 *  intervalMin       [IN] 
 *  intervalMax       [IN] 
 *  type              [IN] 
 *  ownAddrType       [IN] 
 *  directAddrType    [IN] only valid when type = 0x1
 *  bdAddr            [IN] only valid when type = 0x1
 *  channelMap        [IN] 
 *  filter            [IN] 
 * RETURNS
 *  packet
 *****************************************************************************/
void LeHciSetAdvertiseParm(U16 intervalMin, 
                           U16 intervalMax, 
                           HciLeAdvertiseType type, 
                           LeAddrType ownAddrType, 
                           LeAddrType directAddrType, 
                           BD_ADDR bdAddr,
                           LeAdvChannelType channelMap,
                           HciLeAdvertiseFilter filter)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETADVERTISEPARM, 
             intervalMin, intervalMax, type, ownAddrType, directAddrType, channelMap, filter);
    BTBMDumpBdAddr(bdAddr.addr);
    intervalMin = (((intervalMin < 0x0020) ? 0x0020 : intervalMin) > 0x4000) ? 0x4000 : intervalMin;
    intervalMax = (((intervalMax < intervalMin) ? intervalMin : intervalMax) > 0x4000) ? 0x4000 : intervalMax;
    Assert(type >= 0 && type < 0x04);
    Assert(ownAddrType >= 0 && ownAddrType < 0x02);
    Assert(directAddrType >= 0 && directAddrType < 0x02);
    Assert(channelMap > 0 && channelMap < 0x08);
    Assert(filter >= 0 && filter < 0x04);

    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_ADVERTISE_PARAM;
    StoreLE16(hciCmd->parms, intervalMin);
    StoreLE16(hciCmd->parms + 2, intervalMax);
    hciCmd->parms[4] = type;
    hciCmd->parms[5] = ownAddrType;
    hciCmd->parms[6] = directAddrType;
    OS_MemCopy(hciCmd->parms + 7, bdAddr.addr, BD_ADDR_SIZE);
    hciCmd->parms[13] = channelMap;
    hciCmd->parms[14] = filter;
    hciCmd->parmLen = 15;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadAdvertiseTxPower(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADADVERTISETXPOWER);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_ADVERTISE_TX_POWER;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciSetAdvertiseData(U8 dataLen, U8 *data)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETADVERTISEDATA, dataLen);
    bt_parm_trace(dataLen, data);
    
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_ADVERTISE_DATA;
    hciCmd->parms[0] = dataLen;


    OS_MemCopy(hciCmd->parms + 1, data, dataLen);
    hciCmd->parmLen = 1 + dataLen;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciSetScanRspData(U8 dataLen, U8 *data)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETSCANRSPDATA, dataLen);
    bt_parm_trace(dataLen, data);

    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_SCAN_RESPONSE_DATA;
    hciCmd->parms[0] = dataLen;
    OS_MemCopy(hciCmd->parms + 1, data, dataLen);
    hciCmd->parmLen = 1 + dataLen;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

BtStatus LeHciSetAdvertiseEnable(BOOL enable)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETADVERTISEENABLE, enable);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_ADVERTISE_ENABLE;
    hciCmd->parms[0] = enable;
    hciCmd->parmLen = 1;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
    return BT_STATUS_PENDING;
}

void LeHciSetScanParm(HciLeScanType type, U16 interval, U16 window, LeAddrType ownAddrType, HciLeScanFilter filter)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETSCANPARM, type, interval, window, ownAddrType, filter);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_SCAN_PARAM;
    hciCmd->parms[0] = type;
    StoreLE16(hciCmd->parms + 1, interval);
    StoreLE16(hciCmd->parms + 3, window);
    hciCmd->parms[5] = ownAddrType;
    hciCmd->parms[6] = filter;
    hciCmd->parmLen = 7;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

/* filterDup: chip can remember max 16 addresses */
BtStatus LeHciSetScanEnable(BOOL enable, BOOL filterDup)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETSCANENABLE, enable, filterDup);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_SCAN_ENABLE;
    hciCmd->parms[0] = enable;
    hciCmd->parms[1] = filterDup;
    hciCmd->parmLen = 2;
    hciCmd->cmdParm.enable = enable;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();

    return BT_STATUS_PENDING;
}

/* supervisionTO: used after connection established */
BtStatus LeHciCreateConnection(BtRemoteDevice *remDev,
                           U16 scanInterval, 
                           U16 scanWindow, 
                           HciLeInitiatorFilter filter, 
                           LeAddrType peerAddrType, 
                           BD_ADDR *peerAddr,
                           LeAddrType ownAddrType, 
                           U16 connIntervalMin,
                           U16 connIntervalMax,
                           U16 connLatency,
                           U16 supervisionTO,
                           U16 CEMin,
                           U16 CEMax)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCICREATECONNECTION, scanInterval, scanWindow, filter, peerAddrType, ownAddrType);
    BTBMDumpBdAddr(peerAddr->addr);
    bt_trace(TRACE_GROUP_1, BT_MELE_LEHCICREATECONNECTION2, 
             connIntervalMin, connIntervalMax, connLatency, supervisionTO, CEMin, CEMax);

    scanInterval = (((scanInterval < 0x0004) ? 0x0004 : scanInterval) > 0x4000) ? 0x4000 : scanInterval;
    scanWindow = (((scanWindow < 0x0004) ? 0x0004 : scanWindow) > scanInterval) ? scanInterval : scanWindow;
    Assert(filter >= 0 && filter < 0x02);
    Assert(peerAddrType >= 0 && peerAddrType < 0x02);
    Assert(ownAddrType >= 0 && ownAddrType < 0x02);
    connIntervalMin = (((connIntervalMin < 0x0006) ? 0x0006 : connIntervalMin) > 0x0C80) ? 0x0C80 : connIntervalMin;
    connIntervalMax = (((connIntervalMax < connIntervalMin) ? connIntervalMin : connIntervalMax) > 0x0C80) ? 0x0C80 : connIntervalMax;
    connLatency = (connLatency > 0x01F3) ? 0x01F3 : connLatency;
    supervisionTO = (((supervisionTO < 0x000A) ? 0x000A : supervisionTO) > 0x0C80) ? 0x0C80 : supervisionTO;

    hciCmd->handler.remDev = remDev;
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_CREATE_CONNECTION;
    StoreLE16(hciCmd->parms, scanInterval);
    StoreLE16(hciCmd->parms + 2, scanWindow);
    hciCmd->parms[4] = filter;
    hciCmd->parms[5] = peerAddrType;
    OS_MemCopy(hciCmd->parms + 6, peerAddr->addr, BD_ADDR_SIZE);
    hciCmd->parms[12] = ownAddrType;
    StoreLE16(hciCmd->parms + 13, connIntervalMin);
    StoreLE16(hciCmd->parms + 15, connIntervalMax);
    StoreLE16(hciCmd->parms + 17, connLatency);
    StoreLE16(hciCmd->parms + 19, supervisionTO);
    StoreLE16(hciCmd->parms + 21, CEMin);
    StoreLE16(hciCmd->parms + 23, CEMax);
    hciCmd->parmLen = 25;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();

    return BT_STATUS_PENDING;
}

void LeHciCreateConnectionCancel(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCICREATECONNECTIONCANCEL);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_CREATE_CONNECTION_CANCEL;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadWhiteListSize(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADWHITELISTSIZE);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_WHITE_LIST_SIZE;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciClearWhiteList(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCICLEARWHITELIST);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_CLEAR_WHITE_LIST;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciAddToWhiteList(LeAddrType addrType, BD_ADDR bdAddr)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIADDTOWHITELIST, addrType, bdAddr.addr[0], bdAddr.addr[1], bdAddr.addr[2], bdAddr.addr[3], bdAddr.addr[4], bdAddr.addr[5]);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_ADD_TO_WHITE_LIST;
    hciCmd->parms[0] = addrType;
    OS_MemCopy(hciCmd->parms + 1, bdAddr.addr, BD_ADDR_SIZE);
    hciCmd->parmLen = 1 + BD_ADDR_SIZE;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciRemoveFromWhiteList(LeAddrType addrType, BD_ADDR bdAddr)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREMOVEFROMWHITELIST, addrType, bdAddr.addr[0], bdAddr.addr[1], bdAddr.addr[2], bdAddr.addr[3], bdAddr.addr[4], bdAddr.addr[5]);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_REMOVE_FROM_WHITE_LIST;
    hciCmd->parms[0] = addrType;
    OS_MemCopy(hciCmd->parms + 1, bdAddr.addr, BD_ADDR_SIZE);
    hciCmd->parmLen = 1 + BD_ADDR_SIZE;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciUpdateConnection(U16 handle, U16 intervalMin, U16 intervalMax, U16 latency, U16 sTimeout, U16 ceMin, U16 ceMax)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIUPDATECONNECTION, handle, intervalMin, intervalMax, latency, sTimeout, ceMin, ceMax);
    /* Paramter check */
    Assert(handle < 0x0F00);
    intervalMin = (((intervalMin < 0x0006) ? 0x0006 : intervalMin) > 0x0C80) ? 0x0C80 : intervalMin;
    intervalMax = (((intervalMax < intervalMin) ? intervalMin : intervalMax) > 0x0C80) ? 0x0C80 : intervalMax;
    latency = (latency > 0x01F3) ? 0x01F3 : latency;
    sTimeout = (((sTimeout < 0x000A) ? 0x000A : sTimeout) > 0x0C80) ? 0x0C80 : sTimeout;
    ceMax = (ceMax < ceMin) ? ceMin : ceMax;
    
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_CONNECT_UPDATE;
    StoreLE16(hciCmd->parms, handle);
    StoreLE16(hciCmd->parms + 2, intervalMin);
    StoreLE16(hciCmd->parms + 4, intervalMax);
    StoreLE16(hciCmd->parms + 6, latency);
    StoreLE16(hciCmd->parms + 8, sTimeout);
    StoreLE16(hciCmd->parms + 10, ceMin);
    StoreLE16(hciCmd->parms + 12, ceMax);
    hciCmd->parmLen = 14;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciSetChannelMap(U8 *map)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISETCHANNELMAP, map[0], map[1], map[2], map[3], map[4]);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_SET_HOST_CHANNEL_CLASS;
    map[4] &= 0x1F;  /* bit 38, 39, 40 reserved */
    OS_MemCopy(hciCmd->parms, map, 5);
    hciCmd->parmLen = 5;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadChannelMap(U16 handle)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADCHANNELMAP, handle);
    Assert(handle < 0x0F00);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_CHANNEL_MAP;
    StoreLE16(hciCmd->parms, handle);
    hciCmd->parmLen = 2;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadRemoteFeatures(U16 handle)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADREMOTEFEATURES, handle);
    Assert(handle < 0x0F00);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_REMOTE_FEATURES;
    StoreLE16(hciCmd->parms, handle);
    hciCmd->parmLen = 2;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

BtStatus LeHciAES128Encrypt(BtCallBack cb, U8 *key, U8 *data)
{

    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIAES128ENCRYPT, cb);
    bt_parm_trace(16, key);
    bt_parm_trace(16, data);
    hciCmd->handler.callback = cb;
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_ENCRYPT;
    OS_MemCopy(hciCmd->parms, key, 16);
    OS_MemCopy(hciCmd->parms + 16, data, 16);
    hciCmd->parmLen = 32;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
    return BT_STATUS_PENDING;
}

BtStatus LeHciRand(BtCallBack cb)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIRAND, cb);
    hciCmd->handler.callback = cb;
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_RAND;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
    return BT_STATUS_PENDING;
}

BtStatus LeHciStartEncryption(U16 hciHandle, U64 random, U16 ediv, U8 *ltk)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCISTARTENCRYPTION, hciHandle, random, ediv);
    bt_parm_trace(16, ltk);

    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_START_ENCRYPT;
    StoreLE16(hciCmd->parms, hciHandle);
    StoreLE64(hciCmd->parms + 2, random);
    StoreLE16(hciCmd->parms + 10, ediv);
    OS_MemCopy(hciCmd->parms + 12, ltk, 16);
    hciCmd->parmLen = 28;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();

    return BT_STATUS_PENDING;
}

void LeHciLTKRequestReply(U16 hciHandle, U8 *ltk)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCILTKREQUESTREPLY, hciHandle);
    bt_parm_trace(16, ltk);

    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_LONG_TERM_KEY_REQ_REPL;
    StoreLE16(hciCmd->parms, hciHandle);
    OS_MemCopy(hciCmd->parms + 2, ltk, 16);
    hciCmd->parmLen = 18;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciLTKRequestNegReply(U16 hciHandle)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCILTKREQUESTNEGREPLY, hciHandle);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL;
    StoreLE16(hciCmd->parms, hciHandle);
    hciCmd->parmLen = 2;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReadSupportedStates(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIREADSUPPORTEDSTATES);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_READ_SUPPORT_STATES;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciReceiverTest(U8 rxFreq)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCIRECEIVERTEST, rxFreq);
    Assert(rxFreq <= 0x27);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_RECEIVER_TEST;
    hciCmd->parms[0] = rxFreq;
    hciCmd->parmLen = 1;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciTransmitterTest(U8 txFreq, U8 len, U8 payload)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCITRANSMITTERTEST, txFreq, len, payload);
    Assert(txFreq <= 0x27 && len < 0x26 && payload < 0x8);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_TRANSMITTER_TEST;
    hciCmd->parms[0] = txFreq;
    hciCmd->parms[1] = len;
    hciCmd->parms[2] = payload;
    hciCmd->parmLen = 3;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}

void LeHciTestEnd(void)
{
    LeHciCmd *hciCmd = LeAllocHciToken();

    bt_trace(TRACE_FUNC, BT_MELE_LEHCITESTEND);
    hciCmd->op.opType = BOP_LE;
    hciCmd->cmd = HCC_LE_TEST_END;
    hciCmd->parmLen = 0;

    MeAddOperation(hciCmd);
    /* Attempt to start the operation */
    MeStartOperation();
}


void LeHandleOp(BtOpEvent event)
{
    LeHciCmd *curHciCmd = LeFindHciToken(btCtxMe.curOp);
    
    bt_trace(TRACE_FUNC, BT_MELE_LEHANDLEOP, event, curHciCmd->cmd, curHciCmd->parmLen);
    bt_parm_trace(curHciCmd->parmLen, curHciCmd->parms);
    switch (event)
    {
        case BOE_START:
            if(!IsEntryAvailable(&(btCtxMe.hciGenCmd.node)))
            {
                Assert(0);
            }
            switch (curHciCmd->cmd)
            {
            case HCC_LE_CREATE_CONNECTION:
            case HCC_LE_CONNECT_UPDATE:
            case HCC_LE_READ_REMOTE_FEATURES:
            case HCC_LE_START_ENCRYPT:
                btCtxMe.taskHandler = MeHandleConnectionStatusOnly;
                btCtxMe.taskEvent = HCE_COMMAND_STATUS;
                break;
            default:
                btCtxMe.taskHandler = MeHandleCompleteTask;
                btCtxMe.taskEvent = HCE_COMMAND_COMPLETE;
                break;
            }
            OS_MemCopy(btCtxMe.hciGenCmd.parms, curHciCmd->parms, curHciCmd->parmLen);
            BT_XFile_EncryptionCommand(btCtxMe.hciGenCmd.parms, curHciCmd->parmLen);
            MeSendHciCommand(curHciCmd->cmd, curHciCmd->parmLen);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            bt_trace(TRACE_GROUP_1, BT_MELE_LEHANDLEOP2, btCtxMe.btEvent.errCode);
            switch (curHciCmd->cmd)
            {
            case HCC_LE_CREATE_CONNECTION:
                if (btCtxMe.btEvent.errCode != HC_STATUS_SUCCESS)
                {
                    btCtxMe.btEvent.eType = BTEVENT_LINK_CONNECT_CNF;
                    btCtxMe.btEvent.p.remDev = curHciCmd->handler.remDev;
                    MeCallLinkHandlers(curHciCmd->handler.remDev, &btCtxMe.btEvent);
                }
                break;
            case HCC_LE_ENCRYPT:
                Assert(curHciCmd->parmLen == 16);
                if (btCtxMe.btEvent.errCode == HC_STATUS_SUCCESS)
                {
                    OS_MemCopy(btCtxMe.btEvent.p.smEncryptResponse.EncryptData, curHciCmd->parms, 16);
                }
                
                if (curHciCmd->handler.callback)
                {
                    curHciCmd->handler.callback(&btCtxMe.btEvent);
                }
                break;
            case HCC_LE_RAND:
                Assert(curHciCmd->parmLen == 8);
                if (btCtxMe.btEvent.errCode == HC_STATUS_SUCCESS)
                {
                    OS_MemCopy(btCtxMe.btEvent.p.smRand64Response.randValue64, curHciCmd->parms, 8);
                }
                if (curHciCmd->handler.callback)
                {
                    curHciCmd->handler.callback(&btCtxMe.btEvent);
                }
                break;
            case HCC_LE_SET_SCAN_ENABLE:
                if (curHciCmd->cmdParm.enable == FALSE)
                {
                    btCtxMe.btEvent.errCode = BEC_NO_ERROR;
                    btCtxMe.btEvent.eType = BTEVENT_LE_INQUIRY_CANCELED;
                    MeReportResults(BEM_INQUIRY_CANCELED);
                }
                break;
            case HCC_LE_READ_REMOTE_FEATURES:
                break;
            case HCC_LE_START_ENCRYPT:
                if (btCtxMe.btEvent.errCode != HC_STATUS_SUCCESS)
                {
                    btCtxMe.btEvent.eType = BTEVENT_ENCRYPT_COMPLETE;
                    btCtxMe.btEvent.p.remDev = curHciCmd->handler.remDev;
                    MeCallLinkHandlers(curHciCmd->handler.remDev, &btCtxMe.btEvent);
                }
                break;
            case HCC_LE_LONG_TERM_KEY_REQ_REPL:
            case HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL:
            case HCC_LE_CREATE_CONNECTION_CANCEL:
            default:
                break;
            }
            /* The HCI op is sent to controller so clear the op type */
            LeHandleTaskEnd();
            break;

        default:
            LeHandleErrorTaskEnd();
            break;
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}

static void HandleLeAdvertiseReport(U8 len, U8 *parm)
{
    int reportNo = parm[0];
    U8 *ptr = parm + 1;
    
    bt_trace(TRACE_FUNC, BT_MELE_HANDLELEADVERTISEREPORT, len, reportNo);
    len -= 1;
    while (len > 0)
    {
        BtInquiryResult inqResult;
        U8 dataSize = ptr[8];

        OS_MemSet((U8 *)&inqResult, 0x0, sizeof(BtInquiryResult));
        bt_trace(TRACE_GROUP_1, BT_MELE_HANDLELEADVERTISEREPORT1, ptr[0], dataSize);
        switch(ptr[0])
        {
        case HCI_ADV_IND:
        case HCI_SCAN_RSP:
        case HCI_ADV_SCAN_IND:  /* cannot send CONNECT_REQ when receiving SCAN_IND */
        case HCI_ADV_NONCONN_IND:
        case HCI_ADV_DIRECT_IND:
            inqResult.advType = ptr[0];
            inqResult.addrType = ptr[1];
            OS_MemCopy(inqResult.bdAddr.addr, ptr + 2, 6);
            inqResult.devType = BT_DEV_TYPE_LE;  /* AD type flag may not appear -> BR/EDR not supported = 0 */
            inqResult.rssi = ptr[9 + dataSize];
            if (dataSize)
            {
                inqResult.supported_eir = TRUE;
                inqResult.eirResponse = ptr + 9;
                ptr[9 + dataSize] = 0;
            }

            bt_trace(TRACE_GROUP_1, BT_MELE_HANDLELEADVERTISEREPORT2, inqResult.addrType, inqResult.rssi);
            BTBMDumpBdAddr(inqResult.bdAddr.addr);
            bt_parm_trace(dataSize, inqResult.eirResponse);

            btCtxMe.btEvent.errCode = BEC_NO_ERROR;
            btCtxMe.btEvent.eType = BTEVENT_INQUIRY_RESULT;
            OS_MemCopy((U8 *)&btCtxMe.btEvent.p.inqResult, (U8 *)&inqResult, sizeof(BtInquiryResult));
            MeReportResults(BEM_INQUIRY_RESULT);
        default:
            len -= (10 + dataSize);
            ptr += (10 + dataSize);
            break;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  HandleLeConnectComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]
 *    byte[0]      Status
 *    byte[1:2]    Connection_Handle
 *    byte[3]      Role
 *    byte[4]      Peer_Address_Type
 *    byte[5:10]   Peer_Address
 *    byte[11:12]  Conn_Interval
 *    byte[13:14]  Conn_Latency
 *    byte[15:16]  Supervision_Timeout
 *    byte[17]     Master_Clock_Accuracy
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleLeConnectComplete(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    HcStatus status = parm[0];
    BtEventMask mask;

    bt_trace(TRACE_FUNC, BT_MELE_HANDLELECONNECTCOMPLETE, status, parm[1], parm[2], parm[3], parm[4]);
    BTBMDumpBdAddr(parm + 5);
    if ((remDev = ME_FindRemoteDeviceP(parm + 5)) == NULL 
#ifdef CONTROLLER_NOT_RETURN_ADDR_WHEN_FAILED
        && ME_CheckCreatingLink() == FALSE
#endif
        )  /* We are responder so dev context not exist yet */
    {
        if (status == HC_STATUS_SUCCESS)
        {
            BtDeviceContext *bdc, device;
            if ((bdc = DS_FindDevice((BD_ADDR *)(parm + 5))) == NULL)
            {
                OS_MemSet((U8*) &device, 0, (U32) sizeof(BtDeviceContext));
                OS_MemCopy(device.addr.addr, parm + 5, BD_ADDR_SIZE);
                (void)DS_AddDevice(&device, 0);
                bdc = DS_FindDevice((BD_ADDR *)(parm + 5));
            }
    
            if ((remDev = MeGetFreeRemoteDevice()) != NULL)
            {
                /* Store the proper values */
                OS_MemCopy(remDev->bdAddr.addr, parm + 5, 6);
                bdc->link = remDev;
            }
        }
        else
        {
            return;  /* ignore the event */
        }
    }
    else
    {
        if (status != HC_STATUS_SUCCESS)
        {
#ifdef CONTROLLER_NOT_RETURN_ADDR_WHEN_FAILED
            U8 i = 0;
            for (i = 0; i < NUM_BT_DEVICES; i++)
            {
                if (MEC(devTable)[i].state == BDS_OUT_CON)
                {
                    remDev = &MEC(devTable)[i];
                    break;
                }
            }
#endif
        }
        EVM_ResetTimer(&remDev->connectTimer);  /* Stop connect timer */
    }

    btCtxMe.btEvent.eType = ((remDev->state == BDS_OUT_CON) ? BTEVENT_LINK_CONNECT_CNF : BTEVENT_LINK_CONNECT_IND);
    mask = ((remDev->state == BDS_OUT_CON) ? BEM_LINK_CONNECT_CNF : BEM_LINK_CONNECT_IND);
    btCtxMe.btEvent.errCode = bt_util_convert_err4(status);
    btCtxMe.btEvent.p.remDev = remDev;

    if (status == HC_STATUS_SUCCESS)
    {
        BtDeviceRecord record;
        BtDeviceContext *bdc = DS_FindDevice((BD_ADDR *)(parm + 5));

        Assert(bdc);
        bdc->addrType = parm[4];
#ifdef __BT_LE_STANDALONE__
        bdc->devType = BT_DEV_TYPE_UNKNOWN;
#else
        bdc->devType = BT_DEV_TYPE_LE;
#endif
        OS_Report("bdc->devType=%d", bdc->devType);
        OS_MemSet(remDev->fixCidState, 0, sizeof(remDev->fixCidState));
        remDev->hciHandle = LEtoHost16(parm + 1);
        AssertEval(HciAddConnection(remDev->hciHandle, 0x01) != INVALID_HCI_HANDLE);
        remDev->role = ((parm[3] == BCR_MASTER) ? BCR_MASTER : BCR_SLAVE);
        remDev->okToSend = TRUE;  /* L2cap data is ok to send */
        remDev->supervisionTimeout = LEtoHost16(parm + 15);
        remDev->state = BDS_CONNECTED;
        remDev->leConnInterval = LEtoHost16(parm + 11) * 1.25;
        if (SEC_FindDeviceRecord(&(remDev->bdAddr), &record) == BT_STATUS_SUCCESS)
        {
            remDev->gapKeyType = record.gapKeyType;
        }
        else
        {
            remDev->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
        }
        OS_Report("leConnInterval %dms", remDev->leConnInterval);
        LeHciReadRemoteFeatures(remDev->hciHandle);
        bt_trace(TRACE_GROUP_1, BT_MELE_HANDLELECONNECTCOMPLETE2, remDev->hciHandle, remDev->role, remDev->supervisionTimeout);
        btCtxMe.activeCons += 1;
        OS_Report("btCtxMe.activeCons=%d", btCtxMe.activeCons);
        L2CAP_LinkConnectedByCid(remDev, 0x0004);           
        L2CAP_LinkConnectedByCid(remDev, 0x0006);        
        MeCallLinkHandlers(remDev, &btCtxMe.btEvent);  /* Callback remDev->handlers one by one */
        MeReportResults(mask);
    }
    else
    {
        CallAndRemoveHandlers(remDev);
        MeFreeRemoteDevice(remDev);
    }
}

/*****************************************************************************
 * FUNCTION
 *  HandleLeReadRemoteFeature
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]
 *    byte[0]      Status
 *    byte[1:2]    Connection_Handle
 *    byte[3:10]   LE_Features
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleLeReadRemoteFeature(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    HcStatus status = parm[0];
    U16 hciHandle = LEtoHost16(parm + 1);
    
    bt_trace(TRACE_FUNC, BT_MELE_HANDLELEREADREMOTEFEATURE, status, hciHandle);
    bt_parm_trace(8, parm + 3);
    if ((status == HC_STATUS_SUCCESS) && (remDev = MeMapHciToRemoteDevice(hciHandle)) != 0)
    {
        OS_MemCopy(remDev->remoteLEFeature, parm + 3, 8);
    }
}



/*****************************************************************************
 * FUNCTION
 *  HandleLeLTKRequest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]
 *    byte[0:1]    Connection_Handle
 *    byte[2:9]    64bit RAND
 *    byte[10:11]  16bit EDIV
 * RETURNS
 *  void
 *****************************************************************************/
static void HandleLeLTKRequest(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    BtDeviceRecord rec;
    U16 hciHandle = LEtoHost16(parm);
    
    bt_trace(TRACE_FUNC, BT_MELE_HANDLELELTKREQUEST, hciHandle);
    if ((remDev = MeMapHciToRemoteDevice(hciHandle)) != 0)
    {
        bt_trace(TRACE_GROUP_1, BT_MELE_HANDLELELTKREQUEST1, remDev->authState);
        BTBMDumpBdAddr(remDev->bdAddr.addr);
        if (SEC_FindDeviceRecord(&remDev->bdAddr, &rec) == BT_STATUS_SUCCESS)
        {
            if (remDev->authState == BAS_WAITING_KEY_REQ)  /* during pairing */
            {
                BtEvent event;
                event.eType = BTEVENT_LE_SM_RESULT;
                event.errCode = HC_STATUS_SUCCESS;
                event.p.remDev = remDev;
                MeCallLinkHandlers(remDev, &event);
                return;
            }
            else if (rec.distSmKeyType & SM_KEY_LTK)
            {
                /* Current role is slave */
                LeHciLTKRequestReply(hciHandle, rec.distLTK);
                return;
            }
        }
    }
    LeHciLTKRequestNegReply(hciHandle);
}

void LeHandleEvents(U8 len, U8 *parm)
{
    U8 subevent = parm[0];

    bt_trace(TRACE_FUNC, BT_MELE_LEHANDLEEVENTS, subevent);
    len -= 1;
    parm += 1;
    switch(subevent)
    {
    case HCE_LE_CONNECT_COMPLETE:
        HandleLeConnectComplete(len, parm);
        break;
    case HCE_LE_ADVERTISE_REPORT:
        HandleLeAdvertiseReport(len, parm);
        break;
    case HCE_LE_CONNECT_UPDATE_COMPLETE:
        break;
    case HCE_LE_READ_REMOTE_FEATURES_COMPLETE:
        HandleLeReadRemoteFeature(len, parm);
        break;
    case HCE_LE_LONG_TERM_KEY_REQUEST_EVENT:
        HandleLeLTKRequest(len, parm);
        break;
    default:
        Assert(0);
        break;
    }
}

void LeHandleConnectTimeout(EvmTimer *timer)
{
    BtRemoteDevice *remDev;
    U8 i = 0;

    bt_trace(TRACE_FUNC, BT_MELE_LEHANDLECONNECTTIMEOUT);
    remDev = (BtRemoteDevice*) timer->context;
    BTBMDumpBdAddr(remDev->bdAddr.addr);
    remDev->connectTimer.func = NULL;
//    btCtxMe.btEvent.eType = BTEVENT_LINK_CONNECT_CNF;
//    btCtxMe.btEvent.errCode = BEC_CONNECTION_TIMEOUT;
//    MeCallLinkHandlers(remDev, &btCtxMe.btEvent);
    LeHciCreateConnectionCancel();
}

#endif /* #ifdef __BT_4_0_BLE__ */

