#ifndef __GATT_I_H
#define __GATT_I_H

#include "gatt.h"

#define MAX_GATT_CON_NO  NUM_BT_DEVICES

#define GATT_CONN_STATE_IDLE            0x00
#define GATT_CONN_STATE_CONNECTED       0x01

typedef struct _BtGattConn
{
    U8 state;
    BtRemoteDevice *link;
    BtAttEvent *attEvent;
    BtGattEvent gattEvent;
    ListEntry gattRequestOpList;       /* GATT Operation queue */
    ListEntry gattNotificationOpList;       /* GATT Operation queue */
    ListEntry gattIndicationOpList;       /* GATT Operation queue */
    BtOperation *gattRequestCurOp;     /* GATT Current operation */
    BtOperation *gattNotificationCurOp;     /* GATT Current operation */
    BtOperation *gattIndicationCurOp;     /* GATT Current operation */
    GattOpHandler gattRequestHandler;
    GattOpHandler gattNotificationHandler;
    GattOpHandler gattIndicationHandler;
    U16 att_mtu;
    BtGattOp  mtuOp;
} BtGattConn;


typedef struct _BtGattContext
{
    BtGattConn conn[MAX_GATT_CON_NO];
	ListEntry regList;
} BtGattContext;

BtGattConn *BT_Gatt_GetConCtx(BtRemoteDevice *link);




#endif /* __ATT_I_H */

