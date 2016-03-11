BtStatus L2CAP_CreateChannelReq(
 				const L2capPsm *Protocol,
				U16 Psm,
				BtRemoteDevice *Device,
				L2capAMPConnSettings *con_setting,
				U16 *LocalCid);

BtStatus L2CAP_CreateChannelRsp(U16 ChannelId, U16 ResponseCode, U16 ResponseStatus, L2capConnSettings *Settings);

BtStatus L2CAP_MoveChannelReq(
            BtRemoteDevice *Device,
            U16 ChannelId,
            U8 localControllerId,
            U8 remoteControllerId);

BtStatus L2CAP_MoveChannelRsp(
			U16 ChannelId,
			U16 result,
			U8 controllerId);

BtStatus L2CAP_MoveChannelCnfReq(
		BtRemoteDevice *Device,
		U16 ChannelId,
		U16 result);


BtStatus L2CAP_MoveChannelCnfRsp(
			BtRemoteDevice *Device,
			U16 ChannelId);

