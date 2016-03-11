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
 * prxattdb.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Proximity ATT record database
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
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"

#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)

extern U8 ProximityImmAlertLevel(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 BtPrxLocalTxPowerDatabase(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 ProximityLinkLossAlert(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);

const U8 attPrxAlertRecord[] = 
{
    /* ALERT  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_ALERT),

    ATT_HANDLE16(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_WRITE_WO_RESPONSE, ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED), 
};

const U8 attPrxTxPowerRecord[] = 
{
    /* TX Power  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_TXPOWER),

    ATT_HANDLE16(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, BT_UUID_PROXIMITY_TXPOWER_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, BT_UUID_PROXIMITY_TXPOWER_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED), 
};

const U8 attPrxLinkLossRecord[] = 
{
    /* Link Loss  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_LINKLOSS),

    ATT_HANDLE16(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED), 
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  PrxAttDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 PrxAttDB_Registration(void)
{
    BtStatus status;

    status = ATTDB_AddRecord(attPrxAlertRecord, sizeof(attPrxAlertRecord));
    ATTDB_AddRecordCB(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, ProximityImmAlertLevel);
    ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, 
                         ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT_END, 
                         BT_UUID_GATT_TYPE_PRIMARY_SERVICE);

    status = ATTDB_AddRecord(attPrxTxPowerRecord, sizeof(attPrxTxPowerRecord));
    ATTDB_AddRecordCB(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, BtPrxLocalTxPowerDatabase);
    ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, 
                         ATT_HANDLE_PRIMARY_SERVICE_TX_POWER_END, 
                         BT_UUID_GATT_TYPE_PRIMARY_SERVICE);

    status = ATTDB_AddRecord(attPrxLinkLossRecord, sizeof(attPrxLinkLossRecord));
    ATTDB_AddRecordCB(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, ProximityLinkLossAlert);
    ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, 
                         ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END, 
                         BT_UUID_GATT_TYPE_PRIMARY_SERVICE);

    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  PrxAttDB_Deregistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 PrxAttDB_Deregistration(void)
{
    ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END);
    ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, 
                            ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT_END);
    ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, 
                            ATT_HANDLE_PRIMARY_SERVICE_TX_POWER_END);
    ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, 
                            ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END);
    return (U8)BT_STATUS_SUCCESS;
}
#endif

