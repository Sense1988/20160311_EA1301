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
 *     $Workfile:ddb4w.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains an implementation of the device
 *     database operating in the Windows environment. 
 *
 * Created:
 *     January 26, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
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
 ****************************************************************************/
/***************************************************************************** 
* Include
*****************************************************************************/
/* basic system service headers */
#include "bt_common.h"
#include "bttypes.h"
#include "ddb.h"
#include "btconfig.h"
#include "bt_adp_fs.h"
#include <stdio.h>
#ifdef __BT_4_0_BLE__
#include "le_adp.h"
#endif

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define DDB_MAX_ENTRIES  40
#define DDB_COD_MAX_ENTRIES  40
#define DDB_NOT_FOUND    0xffff

/****************************************************************************
 *
 * Data
 *
 ****************************************************************************/


/* The device database is kept in memory. */

typedef struct _bt_ddb_data {
    I16                  numDdbRecs;
    BtDeviceRecord       devDb[DDB_MAX_ENTRIES];
} bt_ddb_data;

typedef struct _bt_ddb_cod_data {
    I16                  numcodDdbRecs;
    BtCodRecord          codDb[DDB_COD_MAX_ENTRIES];
} bt_ddb_cod_data;

typedef struct _bt_ddb_le_data {
    U8 csrk[SM_CSRK_VALUE_LENGTH];
    U32 signCounter;  /* with CSRK */
#ifdef __BT_4_0_PRIVACY__
    U8 irk[SM_IRK_VALUE_LENGTH];
#endif
} bt_ddb_le_data;

static bt_ddb_data ddb_linkkey;
static bt_ddb_cod_data ddb_cod;
#ifdef __BT_4_0_BLE__
static bt_ddb_le_data ddb_lekey;
#endif

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

static I16 DdbFindRecord(const BD_ADDR *bdAddr);
static I16 DdbCodFindRecord(const BD_ADDR *bdAddr);

/*****************************************************************************
 * FUNCTION
 *  ddb_init_link_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_init_link_key(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((U8 *)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((U8 *)BT_DB_PATH);
    }
    
    file_handle = btmtk_fs_open((U8 *)BT_DB_FILE, BTMTK_FS_READ_ONLY);

    if (file_handle < BTMTK_FS_OK)   /* first time, create the file */
    {
        file_handle = btmtk_fs_open((U8 *)BT_DB_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, buffer, file_size);
        btmtk_fs_close(file_handle);
    }
    else
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
        btmtk_fs_read(file_handle, buffer, file_size);
        btmtk_fs_close(file_handle);
    }
}

/*****************************************************************************
 * FUNCTION
 *  ddb_write_link_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_write_link_key(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((U8 *)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((U8 *)BT_DB_PATH);
    }

    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
    file_handle = btmtk_fs_open((U8 *)BT_DB_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, buffer, file_size);
    btmtk_fs_close(file_handle);

    // Dump link key to BT_DB_LINK_KEY_FILE
    {
        int i;
        char str_tmp[64]={0};
        bt_ddb_data* ddb_linkkey_tmp = (bt_ddb_data*)buffer;
        file_handle = btmtk_fs_open((U8 *)BT_DB_LINK_KEY_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);

        for ( i = 0 ; i < ddb_linkkey_tmp->numDdbRecs ; i++ )
        {
            memset(str_tmp, 0, 64);
            snprintf(str_tmp, sizeof(str_tmp), "%02X:%02X:%02X:%02X:%02X:%02X ", 
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[5], 
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[4],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[3],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[2],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[1],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[0]);
            snprintf(&str_tmp[18], sizeof(str_tmp)-18, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", 
                                    ddb_linkkey_tmp->devDb[i].linkKey[0],
                                    ddb_linkkey_tmp->devDb[i].linkKey[1],
                                    ddb_linkkey_tmp->devDb[i].linkKey[2],
                                    ddb_linkkey_tmp->devDb[i].linkKey[3],
                                    ddb_linkkey_tmp->devDb[i].linkKey[4],
                                    ddb_linkkey_tmp->devDb[i].linkKey[5],
                                    ddb_linkkey_tmp->devDb[i].linkKey[6],
                                    ddb_linkkey_tmp->devDb[i].linkKey[7],
                                    ddb_linkkey_tmp->devDb[i].linkKey[8],
                                    ddb_linkkey_tmp->devDb[i].linkKey[9],
                                    ddb_linkkey_tmp->devDb[i].linkKey[10],
                                    ddb_linkkey_tmp->devDb[i].linkKey[11],
                                    ddb_linkkey_tmp->devDb[i].linkKey[12],
                                    ddb_linkkey_tmp->devDb[i].linkKey[13],
                                    ddb_linkkey_tmp->devDb[i].linkKey[14],
                                    ddb_linkkey_tmp->devDb[i].linkKey[15]);
            btmtk_fs_write(file_handle, (U8*)str_tmp, strlen(str_tmp));
        }
        btmtk_fs_close(file_handle);
    }

    // Dump device name to BT_DB_DEV_NAME_FILE
    {
        int i;
        char str_tmp[18];
        bt_ddb_data* ddb_linkkey_tmp = (bt_ddb_data*)buffer;
        file_handle = btmtk_fs_open((U8 *)BT_DB_DEV_NAME_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);

        for ( i = 0 ; i < ddb_linkkey_tmp->numDdbRecs ; i++ )
        {
            memset(str_tmp, 0, 18);
            snprintf(str_tmp, sizeof(str_tmp), "%02X:%02X:%02X:%02X:%02X:%02X ", 
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[5], 
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[4],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[3],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[2],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[1],
                                    ddb_linkkey_tmp->devDb[i].bdAddr.addr[0]);
            btmtk_fs_write(file_handle, (U8*)str_tmp, 18);
            btmtk_fs_write(file_handle, (U8*)ddb_linkkey_tmp->devDb[i].name, strlen(ddb_linkkey_tmp->devDb[i].name));
            btmtk_fs_write(file_handle, (U8*)"\n", 1);
        }
        btmtk_fs_close(file_handle);
    }
    
}


/*****************************************************************************
 * FUNCTION
 *  ddb_init_cod
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_init_cod(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((U8 *)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((U8 *)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((U8 *)BT_DB_COD_FILE, BTMTK_FS_READ_ONLY);

    if (file_handle < BTMTK_FS_OK)   /* first time, create the file */
    {
        file_handle = btmtk_fs_open((U8 *)BT_DB_COD_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, buffer, file_size);
        btmtk_fs_close(file_handle);
    }
    else
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
        btmtk_fs_read(file_handle, buffer, file_size);
        btmtk_fs_close(file_handle);
    }
}


/*****************************************************************************
 * FUNCTION
 *  ddb_write_cod
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_write_cod(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((U8 *)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((U8 *)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((U8 *)BT_DB_COD_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, buffer, file_size);
    btmtk_fs_close(file_handle);
}


#ifdef __BT_4_0_BLE__
/*****************************************************************************
 * FUNCTION
 *  ddb_init_le_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_init_le_key(bt_ddb_le_data *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((U8 *)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((U8 *)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((U8 *)BT_DB_LE_FILE, BTMTK_FS_READ_ONLY);

    if (file_handle < BTMTK_FS_OK)   /* first time, create the file */
    {
        SM_GenCSRK(buffer->csrk);
        buffer->signCounter = 0;
#ifdef __BT_4_0_PRIVACY__
        SM_GenIRK(buffer->irk);
#endif
        file_handle = btmtk_fs_open((U8 *)BT_DB_LE_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, buffer, sizeof(bt_ddb_le_data));
        btmtk_fs_close(file_handle);
    }
    else
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
        btmtk_fs_read(file_handle, buffer, sizeof(bt_ddb_le_data));
        btmtk_fs_close(file_handle);
    }
}


void DDB_ReadLeKey(U8 *csrk, U32 *counter, U8 *irk)
{
    if (csrk)
    {
        OS_MemCopy(csrk, ddb_lekey.csrk, SM_CSRK_VALUE_LENGTH);
    }

    if (counter)
    {
        *counter = ddb_lekey.signCounter;
    }

#ifdef __BT_4_0_PRIVACY__
    if (irk)
    {
        OS_MemCopy(irk, ddb_lekey.irk, SM_IRK_VALUE_LENGTH);
    }
#endif
}

void DDB_WriteLeKey(U8 *csrk, U32 *counter, U8 *irk)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    if (csrk)
    {
        OS_MemCopy(ddb_lekey.csrk, csrk, SM_CSRK_VALUE_LENGTH);
    }

    if (counter)
    {
        ddb_lekey.signCounter = *counter;
    }

#ifdef __BT_4_0_PRIVACY__
    if (irk)
    {
        OS_MemCopy(ddb_lekey.irk, irk, SM_IRK_VALUE_LENGTH);
    }
#endif
    file_handle = btmtk_fs_open(BT_DB_LE_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, (U8 *)&ddb_lekey, sizeof(bt_ddb_le_data));
    btmtk_fs_close(file_handle);
}

#endif

/*---------------------------------------------------------------------------
 *            DDB_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Open
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Open(void)
{
    ddb_linkkey.numDdbRecs = 0;
    ddb_init_link_key((U32) sizeof(bt_ddb_data), (void*)&ddb_linkkey);
#ifdef __BT_4_0_BLE__
    ddb_init_le_key(&ddb_lekey);
#endif
    OS_Report("DDB_Open ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs);
    return BT_STATUS_FAILED;
}

/*---------------------------------------------------------------------------
 *            DDB_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Close(void)
{

    BtStatus status;

    status = BT_STATUS_FAILED;

    if (DDB_Flush() == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }

    ddb_linkkey.numDdbRecs = 0;
    return status;

}

/*---------------------------------------------------------------------------
 *            DDB_Flush()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Force the database to be written to disk. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Flush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Flush(void)
{
    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FLUSHED);
    Assert(ddb_linkkey.numDdbRecs <= DDB_MAX_ENTRIES);
    ddb_write_link_key((U32) sizeof(bt_ddb_data), (void*)&ddb_linkkey);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_FlushAllRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void DDB_FlushAllRecord(void)
{
    ddb_linkkey.numDdbRecs = 0;
    OS_MemSet((U8*) & ddb_linkkey.devDb, 0, sizeof(ddb_linkkey.devDb));
    DDB_Flush();
}

/*---------------------------------------------------------------------------
 *            DDB_AddRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Add a record to the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_AddRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_AddRecord(const BtDeviceRecord *record)
{

    I16 count;
    BtDeviceRecord temp_ddb[DDB_MAX_ENTRIES];

    count = DdbFindRecord(&(record->bdAddr));
    if (count == DDB_NOT_FOUND)
    {
        /* A record with this bdAddr does not exist so add it */
        if (ddb_linkkey.numDdbRecs == DDB_MAX_ENTRIES)
        {
            count = ddb_linkkey.numDdbRecs - 1;
            OS_MemCopy((U8*) (&temp_ddb[0]), (U8*) (&ddb_linkkey.devDb[1]), (U16) ((ddb_linkkey.numDdbRecs - 1) * sizeof(BtDeviceRecord)));
            OS_MemCopy((U8*) (&ddb_linkkey.devDb[0]), (U8*) (&temp_ddb[0]), (U16) ((ddb_linkkey.numDdbRecs - 1) * sizeof(BtDeviceRecord)));
        }
        else
        {
            count = ddb_linkkey.numDdbRecs;
            ddb_linkkey.numDdbRecs++;
        }
    }
    Report(("DDB_AddRecord ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs));
    OS_MemCopy((U8*) (&ddb_linkkey.devDb[count]), (U8*)record, sizeof(BtDeviceRecord));
    if (DDB_Flush() != BT_STATUS_SUCCESS)
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_SAVEING_ERROR, count);
    }

    return BT_STATUS_SUCCESS;

}

/*---------------------------------------------------------------------------
 *            DDB_FindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_FindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_FindRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record)
{

    I16 count;
    Report(("DDB_FindRecord ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs));

    count = DdbFindRecord(bdAddr);

    if (count != DDB_NOT_FOUND && count < DDB_MAX_ENTRIES)
    {
        *record = ddb_linkkey.devDb[count];
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

/*---------------------------------------------------------------------------
 *            DDB_DeleteRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete a record from the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_DeleteRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_DeleteRecord(const BD_ADDR *bdAddr)
{

    I16 count;
    BtDeviceRecord temp_ddb[DDB_MAX_ENTRIES];

    count = DdbFindRecord(bdAddr);
    if (count != DDB_NOT_FOUND)
    {
        /*
         * If it is the last record then all we need to do is reduce
         * * numDdbRecs by 1. Otherwise we need to shift the array
         */
        ddb_linkkey.numDdbRecs--;
        if (count < ddb_linkkey.numDdbRecs && (count+1)<DDB_MAX_ENTRIES)
        {
            /* We need to shift the array */
            OS_MemCopy(
                (U8*) (&temp_ddb[0]),
                (U8*) (&ddb_linkkey.devDb[count + 1]),
                (U16) ((ddb_linkkey.numDdbRecs - count) * sizeof(BtDeviceRecord)));

            OS_MemCopy(
                (U8*) (&ddb_linkkey.devDb[count]),
                (U8*) (&temp_ddb[0]),
                (U16) ((ddb_linkkey.numDdbRecs - count) * sizeof(BtDeviceRecord)));
            OS_MemSet((U8*) (&ddb_linkkey.devDb[ddb_linkkey.numDdbRecs]), 0, sizeof(BtDeviceRecord));
        }
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

/*---------------------------------------------------------------------------
 *            DDB_EnumDeviceRecords()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enumerate the records in the device database. 
 *
 * Return:    status of the operation.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_EnumDeviceRecords
 * DESCRIPTION
 *  
 * PARAMETERS
 *  i           [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_EnumDeviceRecords(I16 i, BtDeviceRecord *record)
{
    if (i >= ddb_linkkey.numDdbRecs)
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        *record = ddb_linkkey.devDb[i];
    }
    return BT_STATUS_SUCCESS;

}

/*---------------------------------------------------------------------------
 *            DdbFindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    Index of BtDeviceRecord if found otherwise DDB_NOT_FOUND.
 */


/*****************************************************************************
 * FUNCTION
 *  DdbFindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static I16 DdbFindRecord(const BD_ADDR *bdAddr)
{

    I16 count;

    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[0], bdAddr->addr[1], bdAddr->addr[2]);
    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[3], bdAddr->addr[4], bdAddr->addr[5]);

    for (count = 0; count < ddb_linkkey.numDdbRecs; count++)
    {
        kal_trace(TRACE_GROUP_1,
            BT_DDB4W_DB_STORED_RECORD,
            ddb_linkkey.devDb[count].bdAddr.addr[0],
            ddb_linkkey.devDb[count].bdAddr.addr[1],
            ddb_linkkey.devDb[count].bdAddr.addr[2]);
        kal_trace(TRACE_GROUP_1,
            BT_DDB4W_DB_STORED_RECORD,
            ddb_linkkey.devDb[count].bdAddr.addr[3],
            ddb_linkkey.devDb[count].bdAddr.addr[4],
            ddb_linkkey.devDb[count].bdAddr.addr[5]);
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_LINK_KEY, 
            ddb_linkkey.devDb[count].linkKey[0], 
            ddb_linkkey.devDb[count].linkKey[1], 
            ddb_linkkey.devDb[count].linkKey[2],
            ddb_linkkey.devDb[count].linkKey[3],
            ddb_linkkey.devDb[count].linkKey[4],
            ddb_linkkey.devDb[count].linkKey[5],
            ddb_linkkey.devDb[count].linkKey[6],
            ddb_linkkey.devDb[count].linkKey[7],
            ddb_linkkey.devDb[count].linkKey[8],
            ddb_linkkey.devDb[count].linkKey[9],
            ddb_linkkey.devDb[count].linkKey[10],
            ddb_linkkey.devDb[count].linkKey[11],
            ddb_linkkey.devDb[count].linkKey[12],
            ddb_linkkey.devDb[count].linkKey[13],
            ddb_linkkey.devDb[count].linkKey[14],
            ddb_linkkey.devDb[count].linkKey[15]);        
        if (OS_MemCmp(bdAddr->addr, 6, ddb_linkkey.devDb[count].bdAddr.addr, 6))
        {
            /* The record is found so return it */
            return count;
        }
    }
    return DDB_NOT_FOUND;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Open
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Open(void)
{
    ddb_cod.numcodDdbRecs = 0;
    ddb_init_cod((U32) sizeof(bt_ddb_cod_data), (void*)&ddb_cod);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Close(void)
{

    BtStatus status;

    status = BT_STATUS_FAILED;

    if (DDB_COD_Flush() == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }

    ddb_cod.numcodDdbRecs = 0;
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Flush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Flush(void)
{
    Assert(ddb_cod.numcodDdbRecs <= DDB_COD_MAX_ENTRIES);
    ddb_write_cod((U32) sizeof(bt_ddb_cod_data), (void*)&ddb_cod);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_AddRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_AddRecord(BD_ADDR bdAddr, U32 cod)
{

    I16 count;
    BtCodRecord temp_ddb[DDB_COD_MAX_ENTRIES];

    count = DdbCodFindRecord(&bdAddr);

    if (count == DDB_NOT_FOUND)
    {
        /* A record with this bdAddr does not exist so add it */
        if (ddb_cod.numcodDdbRecs == DDB_MAX_ENTRIES)
        {
            count = ddb_cod.numcodDdbRecs - 1;
            OS_MemCopy((U8*) (&temp_ddb[0]), (U8*) (&ddb_cod.codDb[1]), (U16) ((ddb_cod.numcodDdbRecs - 1) * sizeof(BtCodRecord)));
            OS_MemCopy((U8*) (&ddb_cod.codDb[0]), (U8*) (&temp_ddb[0]), (U16) ((ddb_cod.numcodDdbRecs - 1) * sizeof(BtCodRecord)));
        }
        else
        {
            count = ddb_cod.numcodDdbRecs;
            ddb_cod.numcodDdbRecs++;
        }
    }
    kal_trace(TRACE_GROUP_1,BT_DDB4W_COD_NUMBER, ddb_cod.numcodDdbRecs);
    ddb_cod.codDb[count].bdAddr = bdAddr;
    ddb_cod.codDb[count].cod = cod;
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_FindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_FindRecord(const BD_ADDR *bdAddr, BtCodRecord *record)
{

    I16 count;

    count = DdbCodFindRecord(bdAddr);

    if (count != DDB_NOT_FOUND && count<DDB_COD_MAX_ENTRIES)
    {
        *record = ddb_cod.codDb[count];
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}


/*****************************************************************************
 * FUNCTION
 *  DdbCodFindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static I16 DdbCodFindRecord(const BD_ADDR *bdAddr)
{

    I16 count;

    for (count = 0; count < ddb_cod.numcodDdbRecs; count++)
    {
        if (OS_MemCmp(bdAddr->addr, 6, ddb_cod.codDb[count].bdAddr.addr, 6))
        {
            /* The record is found so return it */
            kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[0], bdAddr->addr[1], bdAddr->addr[2]);
            kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[3], bdAddr->addr[4], bdAddr->addr[5]);
            return count;
        }
    }
    return DDB_NOT_FOUND;

}

