#ifndef __GATTCB_H
#define __GATTCB_H

#include "bt_feature.h"

#define BT_GATT_OPERATOR_EXCHANGE_MTU                       (0x01)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE      (0x02)
#define BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID  (0x03)
#define BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE               (0x04)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE      (0x05)
#define BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID             (0x06)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR      (0x07)
#define BT_GATT_OPERATOR_READ_CHAR_VALUE                    (0x08)
#define BT_GATT_OPERATOR_READ_USING_CHAR_UUID               (0x09)
#define BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE               (0x0a)
#define BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE           (0x0b)
#define BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE             (0x0c)
#define BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE      (0x0e)
#define BT_GATT_OPERATOR_WRITE_CHAR_VALUE                   (0x0f)
#define BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE              (0x10)
#define BT_GATT_OPERATOR_RELIABLEWRITES                     (0x11)
#define BT_GATT_OPERATOR_NOTIFICATIONS                      (0x12)
#define BT_GATT_OPERATOR_INDICATIONS                        (0x13)
#define BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS              (0x14)
#define BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS         (0x15)
#define BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS             (0x16)
#define BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS        (0x17)


#define BT_GATT_EVENT_RESULT_CONTINUE_DATA		0x00
#define BT_GATT_EVENT_RESULT_DONE_COMPLETED		0x01
#define BT_GATT_EVENT_RESULT_ERROR				0x02
#define BT_GATT_EVENT_RESULT_DISCONNECT			0x03


#define BT_GATT_APP_EVENT_CONNECTED 			0x00
#define BT_GATT_APP_EVENT_DISCONNECTED 			0x01
#define BT_GATT_APP_EVENT_VALUE_NOTIFICATION    0x02
#define BT_GATT_APP_EVENT_VALUE_INDICATION      0x03
#define BT_GATT_APP_EVENT_WRITE_REQUEST         0x04
#define BT_GATT_APP_EVENT_WRITE_COMMAND         0x05
#define BT_GATT_APP_EVENT_PREPARE_WRITE_REQUEST 0x06
#define BT_GATT_APP_EVENT_EXECUTE_WRITE_REQUEST 0x07
#define BT_GATT_APP_EVENT_SERVER                0x08



typedef struct _BtGattDiscoveryAllPrimaryServiceResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U16 uuid2;
	U8 *uuid16;
}BtGattDiscoveryAllPrimaryServiceResultEvent;

typedef struct _BtGattDiscoveryPrimaryServiceByUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryPrimaryServiceByUUIDResultEvent;


typedef struct _BtGattFindIncludeServicesResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U16 includeServiceHandle;
	U16 endGroupHandle;
	U8 type;
	U16 serviceUUID;
	U8 *serviceUUID16;
}BtGattFindIncludeServicesResultEvent;


typedef struct _BtGattDiscoveryAllCharacteristicOfServiceResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 charProperty;
	U16 charValueAttributeHandle;
	U8 type;
	U16 charUUID;
	U8 *charUUID126;
}BtGattDiscoveryAllCharacteristicOfServiceResultEvent;

typedef struct _BtGattDiscoveryCharacteristicByUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 charProperty;
	U16 charValueAttributeHandle;
	U8 type;
	U16 charUUID;
	U8 *charUUID126;
}BtGattDiscoveryCharacteristicByUUIDResultEvent;

typedef struct _BtGattDiscoveryAllCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 type;
	U16 uuid16;
	U8 *uuid128;
}BtGattDiscoveryAllCharacteristicDescriptorsResultEvent;

typedef struct _BtGattReadCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length; /* should be 1,2..     */
	U8 *data;   /* data[0] is the data */
}BtGattReadCharacteristicValueResultEvent;

typedef struct _BtGattReadCharacteristicUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 handle;
	U8 *data;
	U16 length;
}BtGattReadCharacteristicUUIDResultEvent;

typedef struct _BtGattReadLongCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
}BtGattReadLongCharacteristicValueResultEvent;

typedef struct _BtGattReadMultipleCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
}BtGattReadMultipleCharacteristicValueResultEvent;

typedef struct _BtGattWriteCharValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattWriteCharValueResultEvent;


typedef struct _BtGattWriteLongCharValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattWriteLongCharValueResultEvent;

typedef struct _BtGattReliableWriteResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattReliableWriteResultEvent;

typedef struct _BtGattReadCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
}BtGattReadCharacteristicDescriptorsResultEvent;

typedef struct _BtGattReadLongCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
}BtGattReadLongCharacteristicDescriptorsResultEvent;

typedef struct _BtGattWriteCharDescriptorResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattWriteCharDescriptorResultEvent;

typedef struct _BtGattWriteDescriptorResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattWriteLongCharDescriptorResultEvent;

typedef struct _BtGattWriteWithoutResponseResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattWriteWithoutResponseResultEvent;

typedef struct _BtGattSignedWriteWithoutResponseResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattSignedWriteWithoutResponseResultEvent;

typedef struct _BtGattValueNotificationResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattValueNotificationResultEvent;

typedef struct _BtGattValueIndicationsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
}BtGattValueIndicationsResultEvent;


#define BtGattValueIndicationEvent BtATTValueIndicationStruct
#define BtGattValueNotificationEvent BtATTValueNotificationStruct
#define BtGattWriteCommandEvent BtATTWriteCommandStruct
#define BtGattWriteRequestEvent BtATTWriteRequestStruct
#define BtGattPrepareWriteRequestEvent BtATTPrepareWriteRequestStruct
#define BtGattExecuteWriteRequestEvent BtATTExecuteWriteRequestStruct




#if 0
 


typedef struct _BtGattWriteWithoutResponse
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteWithoutResponse;

typedef struct _BtGattSignedWriteWithoutResponse
{	
	U16 handle;
	U16 size;
	U8 *data;
	U8 *signedData;
}BtGattSignedWriteWithoutResponse;

typedef struct _BtGattWriteCharValueRequest
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteCharValueRequest;


typedef struct _BtGattReliableWrite
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattReliableWrite;

typedef struct _BtGattValueNotifications
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattValueNotifications;

typedef struct _BtGattValueIndications
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattValueIndications;

typedef struct _BtGattReadCharacteristicDescriptors
{
	U16 handle;
}BtGattReadCharacteristicDescriptors;

typedef struct BtGattReadLongCharacteristicDescriptors
{
	U16 handle;
	U16 offset;
}BtGattReadLongCharacteristicDescriptors;

typedef struct _BtGattWriteDescriptors
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteDescriptors;

typedef struct _BtGattWriteLongDescriptors
{	
	U16 handle;
	U16 offset;
	U16 size;
	U8 *data;
}BtGattWriteLongDescriptors;

#endif

// Use this to get the final callback
typedef void (*BTGATTUTILCALLBACK)(U8 status, void *data);

#define  GattservResult BtGattDiscoveryPrimaryServiceByUUIDResultEvent

// GattServiceCntx -> use to limit a range
typedef struct
{
    U8  flag;
    U32 userCallback;
    //BtGattDiscoveryPrimaryServiceByUUIDResultEvent
	U16 startHandle;
	U16 endHandle;
    U8  result; /* result of search the service record */
    GattservResult servHandle;
    U32 serviceUUID; /* primary service UUID */
    U32 feature; /* GATT feature */
} GattServiceCntx;

// GattCharCntx -> use to read/write a characteristic
typedef struct
{
    U8   flag;
    U16  handle; /* index */    
} GattCharCntx;

typedef struct
{
    GattServiceCntx servcntx;
    GattCharCntx charcntx;
}GattCharPacket;

#endif /* __ATT_H */ 

