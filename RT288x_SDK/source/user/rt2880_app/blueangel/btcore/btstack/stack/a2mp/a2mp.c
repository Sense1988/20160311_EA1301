#include "bt_common.h"
#include "btalloc.h"

#if A2MP_PROTOCOL == XA_ENABLED
extern BtStatus BTA2MP_PhysicalLinkCreate(BtRemoteDevice *link);
extern A2MP_CONN *A2MP_AllocatePSMChannel(BtRemoteDevice *link, U16 psm);
extern A2MP_CONN *A2MP_AllocatePSMChannelWaiting(BtRemoteDevice *link, U16 psm);
extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
extern A2MP_CONN *A2MP_FindPSMChannelByCid(U16 cid);


BtStatus A2MPHandleCreateServerChannelReq(
                                        BtRemoteDevice *RemDev,
										U16 l2ChannelId,
										U16	ampPsm,
										U8 remoteControlId)
{
	A2MP_CONN *Channel;
	A2MP_MAIN_CONN *Main_Channel;
	L2capAMPConnSettings settings;
	BtStatus status;
    U16 ResponseCode;

	Channel = A2MP_AllocatePSMChannel(RemDev, ampPsm);
	/* RFCOMM move to AMP */

	if(Channel == 0)
	{
	    Report(("A2MPHandleCreateServerChannelReq channel ==0"));
        Channel = A2MP_AllocatePSMChannelWaiting(RemDev, ampPsm);
        if(Channel == 0)
        {
            Report(("A2MPHandleCreateServerChannelReq channel ==0"));
            /* Conflict create channel request, reject one. */
            status = L2CAP_CreateChannelRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0x00);
            return BT_STATUS_FAILED;
        }
	}
    Main_Channel = A2MP_FindMainChannel(RemDev);
    if(Main_Channel ==0)
    {
	    Report(("A2MPHandleCreateServerChannelReq Main_Channel ==0"));
        status = L2CAP_CreateChannelRsp(l2ChannelId, L2CONN_REJECT_CONTROLLER_ID_NOT_SUPPORTED, 0x00);
        A2MP_FreePSMChannel(Channel);
		return BT_STATUS_FAILED;
	}
    Channel->remDev = RemDev;
	Channel->l2ChannelId = l2ChannelId;

	settings.remoteAmpControllerId = remoteControlId;
    Report(("A2MPHandleCreateServerChannelReq remoteControlId:%d",remoteControlId));
    Report(("A2MPHandleCreateServerChannelReq Main_Channel->localControllerId:%d",Main_Channel->localControllerId));

    if(remoteControlId != Main_Channel->localControllerId)
    {

        ResponseCode = L2CONN_REJECT_CONTROLLER_ID_NOT_SUPPORTED;
        status = L2CAP_CreateChannelRsp(l2ChannelId, ResponseCode, 0x00);
        A2MP_FreePSMChannel(Channel);
        return BT_STATUS_FAILED;
    }
    Report(("BTA2MP_PhysicalLinkCreate accept"));

    ResponseCode = L2CONN_ACCEPTED;
    status = L2CAP_CreateChannelRsp(l2ChannelId, ResponseCode, 0x00);
    return BT_STATUS_SUCCESS;

}


BtStatus A2MPHandleCreateServerChannelFailed(BtRemoteDevice *link, U16 l2ChannelId)
{
    U8 i=0;
    A2MP_MAIN_CONN *a2mp_main_channel;
    a2mp_main_channel = A2MP_FindMainChannel(link);
    for(i=0;i<NUM_A2MP_SERVERS;i++)
    {
        if((a2mp_main_channel->a2mp_channel[i].state == A2MP_STATE_ALLOCATED) &&
            (a2mp_main_channel->a2mp_channel[i].l2ChannelId == l2ChannelId))
            A2MP_FreePSMChannel(&(a2mp_main_channel->a2mp_channel[i]));
    }

    return BT_STATUS_SUCCESS;
}


#endif /* A2MP_PROTOCOL == XA_ENABLED */

