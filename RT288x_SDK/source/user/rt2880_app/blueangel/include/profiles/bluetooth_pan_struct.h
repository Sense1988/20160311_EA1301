/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bluetooth_pan_struct.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to define pan structure for adaptation layer.
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
#ifndef __BLUETOOTH_PAN_STRUCT_H__
#define __BLUETOOTH_PAN_STRUCT_H__

#include "bttypes.h"

#define PAN_MAX_DEV_NUM		NUM_BT_DEVICES


#define PAN_MEM_MALLOC(size)		get_ctrl_buffer(size)
#define PAN_MEM_FREEIF(p)			do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		
#define PAN_MEM_FREE(p)			free_ctrl_buffer(p)	

#define PAN_DEVICE_ADDR_EQUAL(dev1, dev2)     ((memcmp(\
                                                                                                         (U8 *)dev1, (U8 *)dev2, \
                                    sizeof(BD_ADDR)) == 0) ? TRUE : FALSE)

typedef enum
{
    PAN_SERVICE_NAP = 0,
    PAN_SERVICE_GN,
    PAN_SERVICE_PANU
} bt_pan_service_enum;


typedef struct
{
    LOCAL_PARA_HDR
    bt_pan_service_enum service;		
    bt_pan_service_enum dstservice;		
    BD_ADDR bt_addr;
} bt_pan_connect_req_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
} bt_pan_disconnect_req_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BT_BOOL result;
} bt_pan_activate_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BT_BOOL result;
} bt_pan_deactivate_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
    bt_pan_service_enum service;		
} bt_pan_connection_authorize_ind_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
    BT_BOOL accept;		
} bt_pan_connection_authorize_rsp_struct;

typedef struct
{
    LOCAL_PARA_HDR
    bt_pan_service_enum service;		
    BD_ADDR bt_addr;
    U16 unit;
} bt_pan_connect_ind_struct;

typedef struct
{
    LOCAL_PARA_HDR
    bt_pan_service_enum service;		
    BD_ADDR bt_addr;
    BT_BOOL result;	
    U16 unit;
} bt_pan_connect_cnf_struct;


typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
} bt_pan_disconnect_ind_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
    BT_BOOL result;	
} bt_pan_disconnect_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
    U16 listLen;
    U8 list[0];
} bt_pan_set_nettype_filter_req;

typedef struct
{
    LOCAL_PARA_HDR
    BD_ADDR bt_addr;
    U16 listLen;
    U8 list[0];
} bt_pan_set_multiaddr_filter_req;

typedef struct
{
    LOCAL_PARA_HDR
    U16 len;	
    U8 packet_type;	
    U8 packet[0];
} bt_pan_pts_test_send_packet_req_struct;

#endif
