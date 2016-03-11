#ifndef __MEAMP_ADP_H
#define __MEAMP_ADP_H


/*---------------------------------------------------------------------------
 * BtRemoteDevice structure
 *
 *     Represents a remote device. This structure is used to manage the ACL
 *     and SCO links to a remote device.
 */
typedef struct _BtAMPDeviceInfo
{

    U16 supervisionTimeout;
    U32 flushTimeOut;

} BtAMPDeviceInfo;

#endif /* __MEAMP_ADP_H */
