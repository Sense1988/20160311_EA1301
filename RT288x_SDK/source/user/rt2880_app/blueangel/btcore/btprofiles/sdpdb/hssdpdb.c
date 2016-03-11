/*****************************************************************************
 *
 * Filename:
 * ---------
 * hssdpdb.c
 *
 * Project:
 * --------
 * BT Project
 *
 * Description:
 * ------------
 * This file contains functions and global variable to the HeadSet HS Service Records. 
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "sdp.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object.
 * 
 * Value of the protocol descriptor list for the Headset profile HS role.
 * This structure is a ROMable representation of the RAM structure.
 * During HF_Register, this structure is copied into the RAM structure
 * used to register the server. A RAM structure must be used to be able
 * to dynamically set the RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 g_hsProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(12), /* Data element sequence, 12 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data element sequence for L2CAP, 3 bytes */
    SDP_UUID_16BIT(PROT_L2CAP), /* Uuid16 L2CAP */

    /*
     * Next protocol descriptor in the list is RFCOMM. It contains two
     * elements which are the UUID and the channel. Ultimately this
     * channel will need to filled in with value returned by RFCOMM.
     */
    SDP_ATTRIB_HEADER_8BIT(5),   /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM), /* Uuid16 RFCOMM */
    SDP_UINT_8BIT(HS_HS_CHNL_NUM_START)
};

/*
 * BluetoothProfileDescriptorList
 */
const U8 g_hsProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_HEADSET), /* Uuid16 Headset */
    SDP_UINT_16BIT(0x0102)      /* Uint16 version number */
};

/*---------------------------------------------------------------------------
 *
 * SDP objects registered by Headset Audio Gateway.
 * They are the Service Class ID and the Protocol Descriptor List
 */
const U8 g_hsClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),         /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_HEADSET),        /* Headset UUID in Big Endian */
    SDP_UUID_16BIT(SC_GENERIC_AUDIO),  /* Generic Audio UUID in Big Endian */
    SDP_UUID_16BIT(SC_HEADSET_HEADSET) /* Headset-HS UUID in Big Endian */    
};

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).
 */
const U8 g_hsLangBaseIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* English Language */
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */
};

/*
 * * OPTIONAL * Audio Gateway service name (digianswer requires it).
 */
const U8 g_hsServiceName[] = 
{
    SDP_TEXT_8BIT(8),  /* Null terminated text string, 7 bytes */
    'H', 'e', 'a', 'd', 's', 'e', 't', '\0'
};

/*---------------------------------------------------------------------------
 * Audio Gateway Public Browse Group.
 */
const U8 g_hsBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

/****************************************************************************
 *
 * Definitions Of Exported Globals 
 *
 ****************************************************************************/

/*
 * Headset's attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * Headset's SDP record.
 */
SdpAttribute g_hsSdpAttributes[] = 
{
    /* Headset class ID List attribute */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, g_hsClassId),
    /* Headset protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, g_hsProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, g_hsBrowseGroup),
    /* Language base id (Optional: Used with service name) */
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, g_hsLangBaseIdList),
    /* Headset profile descriptor list attribute */
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, g_hsProfileDescList),
    /* Headset service name (Optional) */
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), g_hsServiceName),
};

/****************************************************************************
 *
 * Function Definitions 
 *
 ****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  HsphsSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the Headset service record attributes pointer,
 *  attribute number and the service channel in the service record
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_HEADSET.
 *  attribute           [IN]        
 *  srv_chn             [OUT]       The RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 HsphsSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{
    U8 attribute_num = 0;

    switch (service_type)
    {
    case SC_HEADSET:
        *attribute    = (SdpAttribute *)&g_hsSdpAttributes;
        attribute_num = sizeof(g_hsSdpAttributes) / sizeof(*(g_hsSdpAttributes));
        *srv_chn      = (U8)g_hsProtoDescList[sizeof(g_hsProtoDescList) - 1];
        break;
    }
    return attribute_num;
}

