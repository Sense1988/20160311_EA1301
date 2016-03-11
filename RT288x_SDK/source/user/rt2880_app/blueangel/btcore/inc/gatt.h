#ifndef __GATT_H
#define __GATT_H

#include "bt_feature.h"

#define BT_GATT_OP_START                    0x00
#define BT_GATT_OP_END                      0x01
#define BT_GATT_OP_ERROR                    0x02
#define BT_GATT_OP_LINK_DISCONNECTED        0x03

typedef struct _BtGattEvent
{
    U8 eType;     
    BtRemoteDevice *link;
    void *parms;
    void *pGattOp;
}BtGattEvent;

typedef struct _BtGattDiscoveryAllPrimaryService
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllPrimaryService;

typedef struct _BtGattDiscoveryPrimaryServiceByUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U8 uuid16[2];
	U8 uuid128[16];
}BtGattDiscoveryPrimaryServiceByUUID;

typedef struct _BtGattFindIncludeServices
{
	U16 startHandle;
	U16 endHandle;
	U16 tempData1;
	U16 tempData2;
	U16 tempData3;
}BtGattFindIncludeServices;


typedef struct _BtGattDiscoveryAllCharacteristicOfService
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllCharacteristicOfService;

typedef struct _BtGattDiscoveryCharacteristicByUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U8 uuid16[2];
	U8 uuid128[16];
}BtGattDiscoveryCharacteristicByUUID;

typedef struct _BtGattDiscoveryAllCharacteristicDescriptors
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllCharacteristicDescriptors;

typedef struct _BtGattReadCharacteristicValue
{
	U16 handle;
}BtGattReadCharacteristicValue;

typedef struct _BtGattReadCharacteristicUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U16 uuid16;
	U8 uuid128[16];
}BtGattReadCharacteristicUUID;

typedef struct _BtGattReadLongCharacteristicValue
{
	U16 handle;
	U16 offset;
}BtGattReadLongCharacteristicValue;

typedef struct _BtGattReadMultipleCharacteristicValue
{	
	/* Max support for 7 handle */
	U8 handleSetNo;
	U16 handle[7];
}BtGattReadMultipleCharacteristicValue;

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
}BtGattSignedWriteWithoutResponse;

typedef struct _BtGattWriteCharValueRequest
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteCharValueRequest;

typedef struct _BtGattWriteLongCharacteristicValues
{	
	U16 handle;
	U16 offset;
	U16 size;
	U8 *data;
}BtGattWriteLongCharacteristicValues;


typedef struct _BtGattReliableWrite
{	
	U16 handle;
	U16 offset;
	U16 excuteFlag;    
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

typedef struct
{
    union
    {
		BtGattDiscoveryAllPrimaryService *allPrimaryService;
		BtGattDiscoveryPrimaryServiceByUUID *primaryServiceByUUID;
		BtGattFindIncludeServices *findIncludeServices;
		BtGattDiscoveryAllCharacteristicOfService *allCharOfServices;
		BtGattDiscoveryCharacteristicByUUID *charByUUID;
		BtGattDiscoveryAllCharacteristicDescriptors *allCharDescriptors;
		BtGattReadCharacteristicValue *readCharValue;
		BtGattReadCharacteristicUUID *readCharUUID;
		BtGattReadLongCharacteristicValue *readLongCharValue;
		BtGattReadMultipleCharacteristicValue *readMultipleCharValue;
		BtGattWriteWithoutResponse *writeWithoutResponse;
		BtGattSignedWriteWithoutResponse *signedWriteWithoutResponse;
		BtGattWriteCharValueRequest *writeRequest;
		BtGattWriteLongCharacteristicValues *writeLongCharValues;
		BtGattReliableWrite *reliableWrite;
		BtGattValueNotifications *valueNotifications;
		BtGattValueIndications *valueIndications;
		BtGattReadCharacteristicDescriptors *readCharDescriptors;
		BtGattReadLongCharacteristicDescriptors *readLongCharDescriptors;
		BtGattWriteDescriptors *writeDescriptors;
		BtGattWriteLongDescriptors *writeLongDescriptors;
	}p;
}btGattOperationData;


typedef void (*GattOpHandler) (U8 event, BtRemoteDevice *link);
typedef void (*GattAPCallBack) (void *, BtGattEvent *);

typedef struct _BtGattOp
{
	BtOperation op; /* att operation */
	btGattOperationData parms;
	GattAPCallBack apCb;
} BtGattOp;


typedef struct _BtGattHandler
{
    ListEntry node;         	/* For internal stack use only. */
    GattAPCallBack callback;    /* Pointer to callback function */
} BtGattHandler;


BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadUsingCharacteristicUUID(BtRemoteDevice *link, BtGattOp *op);

#endif /* __ATT_H */ 

