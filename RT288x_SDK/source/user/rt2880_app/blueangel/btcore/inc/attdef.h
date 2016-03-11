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
 * attdef.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Attribute protocol definition
 *
 * Author:
 * -------
 * Dlight Ting (mtk01239)
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
#ifndef __ATTDEF_H
#define __ATTDEF_H

#include "me_adp.h"

#define ATT_PERMISSIONS_READ_ONLY    (0x00)
#define ATT_PERMISSIONS_WRITE_ONLY   (0x01)
#define ATT_PERMISSIONS_READ_WRITE   (0x02)
#define ATT_PERMISSIONS_GATT_DEFINED (0x03)  /* defined in Charateristic Properties */


#define BT_UUID_GATT_SERVICE_GAP           0x1800
#define BT_UUID_GATT_SERVICE_GATT          0x1801
#define BT_UUID_GATT_SERVICE_ALERT         0x1802
#define BT_UUID_GATT_SERVICE_LINKLOSS      0x1803
#define BT_UUID_GATT_SERVICE_TXPOWER       0x1804
#define BT_UUID_GATT_SERVICE_TIME          0x1805
#define BT_UUID_GATT_SERVICE_TIMEUPDATE    0x1806
#define BT_UUID_GATT_SERVICE_DST           0x1807
#define BT_UUID_GATT_SERVICE_HEALTH        0x1809
#define BT_UUID_GATT_SERVICE_DEVICE        0x180A
#define BT_UUID_GATT_SERVICE_NETWORK       0x180B
#define BT_UUID_GATT_SERVICE_WATCHDOG      0x180C
#define BT_UUID_GATT_SERVICE_HEART         0x180D
#define BT_UUID_GATT_SERVICE_PHONEALERT    0x180E
#define BT_UUID_GATT_SERVICE_BATTERY       0x180F
#define BT_UUID_GATT_SERVICE_BLOODPRESSURE 0x1810


#define BT_UUID_GATT_TYPE_PRIMARY_SERVICE   0x2800
#define BT_UUID_GATT_TYPE_SECONDARY_SERVICE   0x2801
#define BT_UUID_GATT_TYPE_INCLUDE               0x2802
#define BT_UUID_GATT_TYPE_CHARACTERISTIC        0x2803

#define BT_UUID_GATT_CHAR_DESC_CHAR_EXTEND_PROPERTY          0x2900
#define BT_UUID_GATT_CHAR_DESC_CHAR_USER_DESC                0x2901
#define BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG            0x2902
#define BT_UUID_GATT_CHAR_DESC_SERVER_CHAR_CONFIG            0x2903
#define BT_UUID_GATT_CHAR_DESC_CHAR_FORMAT                  0x2904
#define BT_UUID_GATT_CHAR_DESC_CHAR_AGGREGATE_FORMAT        0x2905

#define BT_UUID_GATT_CHAR_TYPE_DEVICE_NAME                  0x2A00
#define BT_UUID_GATT_CHAR_TYPE_APPEARANCE                   0x2A01
#define BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PRIVACY_FLAG      0x2A02
#define BT_UUID_GATT_CHAR_TYPE_RECONNECTION_ADDRESS         0x2A03
#define BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PREFER_CONNECT_PARM        0x2A04
#define BT_UUID_GATT_CHAR_TYPE_SERIVCE_CHANGED              0x2A05


#define BT_UUID_PROXIMITY_ALERT_LEVEL                       0x2A06 /* todo */
#define BT_UUID_PROXIMITY_TXPOWER_LEVEL                     0x2A07 /* todo */


/* For characteristics of Time profile */
#define BT_UUID_CT_TIME								0x2A10 /* todo */
#define BT_UUID_LOCAL_TIME_INFO					0x2A11 /* todo */
#define BT_UUID_REF_TIME_INFO						0x2A12 /* todo */
#define BT_UUID_TIME_WITH_DST						0x2A13 /* todo */
#define BT_UUID_TIME_UPDATE_CONTROL_POINT		0x2A14 /* todo */
#define BT_UUID_TIME_UPDATE_STATE					0x2A15 /* todo */


#define BT_ATT_CONST_VARIABLE               0x00
#define BT_ATT_FIX_VARIABLE                 0x01
#define BT_ATT_NON_FIX_VARIABLE             0x02
#define BT_ATT_FIX_VARIABLE_IN_UPPER_AP     0x03

typedef enum
{
    ATT_OP_BROADCAST,
    ATT_OP_READ,
    ATT_OP_WRITE_WO_RESPONSE,
    ATT_OP_WRITE,
    ATT_OP_NOTIFY,
    ATT_OP_INDICATE,
    ATT_OP_SIGNED_WRITE
} bt_att_op;

typedef enum
{
    GATT_CHAR_PROP_BROADCAST = 0x01,
    GATT_CHAR_PROP_READ = 0x02,
    GATT_CHAR_PROP_WRITE_WO_RESPONSE = 0x04,
    GATT_CHAR_PROP_WRITE = 0x08,
    GATT_CHAR_PROP_NOTIFY = 0x10,
    GATT_CHAR_PROP_INDICATE = 0x20,
    GATT_CHAR_PROP_SIGNED_WRITE = 0x40,
    GATT_EXT_PROP = 0x80
} bt_gatt_char_properties;


#define BT_UUID16_SIZE  (0x02)
#define BT_UUID128_SIZE (0x10)

/* Handle(2B) 0x02(1B) CONST_VALUE(1B) VALUESIZE(2B) UUID(2) */
#define ATT_HANDLE16(HANDLE, UUID, CONST_VALUE, VALUE_SIZE)                                              \
            (U8)(((HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            BT_UUID16_SIZE,  /* SIZE Include Header */      \
            (U8)(CONST_VALUE), \
            (U8)(((VALUE_SIZE) & 0xff00) >> 8),        \
            (U8)((VALUE_SIZE) & 0x00ff),        \
            (U8)(((UUID) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((UUID) & 0x00ff)       /* Bits[7:0] of UUID */

/* Handle(2B) 0x10(1B) CONST_VALUE(1B) VALUESIZE(2B) UUID(2) */
#define ATT_HANDLE128(HANDLE, UUID, CONST_VALUE, VALUE_SIZE)                                              \
                (U8)(((HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
                (U8)((HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
                BT_UUID128_SIZE,        \
                (U8)(CONST_VALUE), \
                (U8)(((VALUE_SIZE) & 0xff00) >> 8),        \
                (U8)((VALUE_SIZE) & 0x00ff),            \
                UUID        /* 128-bit UUID */

#define ATT_GROUP(START_HANDLE, END_HANDLE, UUID) \
            (U8)(((START_HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((START_HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            (U8)(((END_HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((END_HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            UUID        /* 128-bit UUID */


/* PERS(1B) */
#define ATT_PERMINSIONS(PERS)                                              \
                                (U8 )(PERS)        

                                

#define ATT_SETUUID16(uuid)                                              \
            (U8)((uuid) & 0x00ff),             /* Bits[7:0] of UUID */   \
			(U8)(((uuid) & 0xff00) >> 8)	 /* Bits[15:8] of UUID */	  


/* PROP(1B) REC_HANDLE(2B) UUID(2B) */
#define ATT_SETCHARACTERISTIC(PROP, REC_HANDLE, UUID)                   \
                                (U8) (PROP),                            \
                                (U8)((REC_HANDLE) & 0xff),     \
                                (U8)(((REC_HANDLE) & 0xff00) >> 8),             \
                                (U8)( ((UUID) & 0xff) ),           \
                                (U8)(((UUID) & 0xff00) >> 8)



#define ATT_SETCHARACTERISTIC128(PROP, REC_HANDLE, UUID)                   \
                                                                (U8) (PROP),                            \
                                                                (U8)((REC_HANDLE) & 0xff),     \
                                                                (U8)(((REC_HANDLE) & 0xff00) >> 8),             \
                                                                UUID
                                                                
#define ATT_SETINCLUDE16(HDL_START, HDL_END, UUID)                        \
                (U8)((HDL_START) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((HDL_START) & 0xff00) >> 8),     /* Bits[15:8] of UUID */    \
                (U8)((HDL_END) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((HDL_END) & 0xff00) >> 8),     /* Bits[15:8] of UUID */     \
                (U8)((UUID) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((UUID) & 0xff00) >> 8)     /* Bits[15:8] of UUID */     
                
                                                                
#define ATT_SETINCLUDE128(HDL_START, HDL_END)                        \
                                (U8)((HDL_START) & 0x00ff),             /* Bits[7:0] of UUID */   \
                                (U8)(((HDL_START) & 0xff00) >> 8),     /* Bits[15:8] of UUID */  \
                                (U8)((HDL_END) & 0x00ff),             /* Bits[7:0] of UUID */   \
                                (U8)(((HDL_END) & 0xff00) >> 8)     /* Bits[15:8] of UUID */    
                                

#define ATT_SETEXTENDEDPROPERTIES(PROP)                        \
                    (U8)((PROP) & 0x00ff),             /* Bits[7:0] of UUID */   \
                    (U8)(((PROP) & 0xff00) >> 8)     /* Bits[15:8] of UUID */   


#define ATT_SETCHARFORMAT(FORMAT, EXP, UNIT, N_SP, DESCR)                        \
                        (U8) (FORMAT),\
                        (U8) (EXP), \
                        (U8)((UNIT) & 0x00ff),             /* Bits[7:0] of UUID */   \
                        (U8)(((UNIT) & 0xff00) >> 8),     /* Bits[15:8] of UUID */ \
                        (U8) (N_SP), \
                        (U8)((DESCR) & 0x00ff),             /* Bits[7:0] of UUID */   \
                        (U8)(((DESCR) & 0xff00) >> 8)   /* Bits[15:8] of UUID */   

typedef U8 (*AttdbCallback) (U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);

#endif  /* __ATTDEF_H */
