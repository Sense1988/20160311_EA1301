/*****************************************************************************
 *
 * Filename:
 * ---------
 * hfsdpdb.c
 *
 * Project:
 * --------
 * BT Project
 *
 * Description:
 * ------------
 * This file contains functions and global variable to the Hands-Free HF Service Records. 
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #2 $
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

/****************************************************************************
 * Hands-Free HF SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList 
 */
const U8 g_hfClassId[] = 
{
    SDP_ATTRIB_HEADER_8BIT(6),       /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(SC_HANDSFREE),    /* Hands-Free UUID in Big Endian */
    SDP_UUID_16BIT(SC_GENERIC_AUDIO) /* Generic Audio UUID in Big Endian */
};

/*---------------------------------------------------------------------------
 * SDP Protocol Descriptor List object registered by Hands-Free HF.
 * 
 * Value of the protocol descriptor list for the Hands-Free HF 
 * profile.  This structure is a ROMable representation of the RAM structure.
 * This structure is copied into a RAM structure used to register the 
 * service. A RAM structure must be used to be able to dynamically set the 
 * RFCOMM channel in the RFCOMM protocol descriptor.
 */
const U8 g_hfProtoDescList[] = 
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
    SDP_ATTRIB_HEADER_8BIT(5),      /* Data element sequence for RFCOMM, 5 bytes */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* Uuid16 RFCOMM */
    SDP_UINT_8BIT(HF_HF_CHNL_NUM_START)
};

/*
 * BluetoothProfileDescriptorList
 */
const U8 g_hfProfileDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(8),
    SDP_ATTRIB_HEADER_8BIT(6),    /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_UUID_16BIT(SC_HANDSFREE), /* Uuid16 Hands-Free */
    SDP_UINT_16BIT(0x0105)        /* Uint16 version number */
};

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).
 */
const U8 g_hfLangBaseIdList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */
    SDP_UINT_16BIT(0x656E),     /* English Language */
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */
};

/*
 * * OPTIONAL *  ServiceName
 */
const U8 g_hfServiceName[] = 
{
    SDP_TEXT_8BIT(16),  /* Null terminated text string */
    'H', 'a', 'n', 'd', 's', '-', 'F', 'r', 'e', 'e', ' ', 'u',
    'n', 'i', 't','\0'
};

/*
 * * OPTIONAL *  Supported Features
 */
U8 g_hfSupportedFeatures[3];

/*---------------------------------------------------------------------------
 * Hands-Free Public Browse Group.
 */
const U8 g_hfBrowseGroup[] = 
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
 * Hands-Free attributes.
 * 
 * This is a ROM template for the RAM structure used to register the
 * Hands-Free SDP record.
 */
SdpAttribute g_hfSdpAttributes[7];

/****************************************************************************
 *
 * Function Definitions 
 *
 ****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  HfphfSdpDB_GetAttribute
 * DESCRIPTION
 *  This function is used to get the Handsfree service record attributes pointer,
 *  attribute number and the service channel in the service record
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_HANDSFREE.
 *  attribute           [IN]        
 *  srv_chn             [IN]        the RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 HfphfSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{
    U8 attribute_num = 0;

   	g_hfSupportedFeatures[0]   = (U8)DETD_UINT + DESD_2BYTES;
	g_hfSupportedFeatures[1]   = 0;
	g_hfSupportedFeatures[2]   = 0x15;

	g_hfSdpAttributes[0].id    = AID_SERVICE_CLASS_ID_LIST;
	g_hfSdpAttributes[0].len   = sizeof(g_hfClassId);
	g_hfSdpAttributes[0].value = (const U8*)(g_hfClassId);	
	g_hfSdpAttributes[0].flags = 0x0000;		

	g_hfSdpAttributes[1].id    = AID_PROTOCOL_DESC_LIST;
	g_hfSdpAttributes[1].len   = sizeof(g_hfProtoDescList);
	g_hfSdpAttributes[1].value = (const U8*)(g_hfProtoDescList);	
	g_hfSdpAttributes[1].flags = 0x0000;		

	g_hfSdpAttributes[2].id    = AID_BROWSE_GROUP_LIST;
	g_hfSdpAttributes[2].len   = sizeof(g_hfBrowseGroup);
	g_hfSdpAttributes[2].value = (const U8*)(g_hfBrowseGroup);	
	g_hfSdpAttributes[2].flags = 0x0000;		

	g_hfSdpAttributes[3].id    = AID_LANG_BASE_ID_LIST;
	g_hfSdpAttributes[3].len   = sizeof(g_hfLangBaseIdList);
	g_hfSdpAttributes[3].value = (const U8*)(g_hfLangBaseIdList);	
	g_hfSdpAttributes[3].flags = 0x0000;		

	g_hfSdpAttributes[4].id    = AID_BT_PROFILE_DESC_LIST;
	g_hfSdpAttributes[4].len   = sizeof(g_hfProfileDescList);
	g_hfSdpAttributes[4].value = (const U8*)(g_hfProfileDescList);	
	g_hfSdpAttributes[4].flags = 0x0000;		

	g_hfSdpAttributes[5].id    = (AID_SERVICE_NAME + 0x0100);
	g_hfSdpAttributes[5].len   = sizeof(g_hfServiceName);
	g_hfSdpAttributes[5].value = (const U8*)(g_hfServiceName);	
	g_hfSdpAttributes[5].flags = 0x0000;		

	g_hfSdpAttributes[6].id    = AID_SUPPORTED_FEATURES;
	g_hfSdpAttributes[6].len   = sizeof(g_hfSupportedFeatures);
	g_hfSdpAttributes[6].value = (const U8*)(g_hfSupportedFeatures);	
	g_hfSdpAttributes[6].flags = 0x0000;

    switch (service_type)
    {
    case SC_HANDSFREE:
        *attribute    = (SdpAttribute *)&g_hfSdpAttributes;
        attribute_num = sizeof(g_hfSdpAttributes) / sizeof(*(g_hfSdpAttributes));
        *srv_chn      = (U8)g_hfProtoDescList[sizeof(g_hfProtoDescList) - 1];
        break;
    }
    return attribute_num;
}

