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

#ifdef ANDROID
#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/MemoryHeapBase.h>
#include <binder/IServiceManager.h>

#include "bt_adp_ashm_impl.h"

namespace android {

enum { GET_BUFFER = IBinder::FIRST_CALL_TRANSACTION };

/**
 *  Client side - Proxy
 */
class BpShBuffer: public BpInterface<IShBuffer> {
public:
	BpShBuffer(const sp<IBinder>& impl) : BpInterface<IShBuffer>(impl)
	{
	}

	sp<IMemoryHeap> getBuffer()
	{
		Parcel data, reply;
		sp<IMemoryHeap> memHeap = NULL;
		data.writeInterfaceToken(IShBuffer::getInterfaceDescriptor());
		remote()->transact(GET_BUFFER, data, &reply);
		memHeap = interface_cast<IMemoryHeap> (reply.readStrongBinder());
		return memHeap;
	}
};

IMPLEMENT_META_INTERFACE(ShBuffer, "vendor.mtk.bt.adp.ashm");

/**
 * Server side implementtation
 */
status_t BnShBuffer::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	switch (code)
	{
		case GET_BUFFER:
		{
			CHECK_INTERFACE(IShBuffer, data, reply);
			sp<IMemoryHeap> Data = getBuffer();
			if (Data != NULL)
			{
				reply->writeStrongBinder(Data->asBinder());
			}
			return NO_ERROR;
			break;
		}
		default:
			return BBinder::onTransact(code, data, reply, flags);
	}
}

/**
 * Service implementation
 */
sp<IMemoryHeap> BtAdpAshmService::getBuffer()
{
	return mMemHeap;
}

status_t BtAdpAshmService::instantiate(const char* name, int size)
{
	status_t status;
	sp<IBinder> service;
//	char serviceName[50];
//	strcpy(serviceName, BT_ADP_ASHM_SERVICE_NAME_PREFIX);
//	strncat(serviceName, name, 49-strlen(name));
	service = defaultServiceManager()->checkService( String16(name) );
	if ( service == NULL ){

		status = defaultServiceManager()->addService(String16(name), new BtAdpAshmService(name, size));
		return NO_ERROR;
	}
	else {
		// already exist
		return 1;
	}
}

BtAdpAshmService::BtAdpAshmService(const char* name, int size)
{
	// MemoryHeapBase will use ashmem
	mMemHeap = new MemoryHeapBase(size, 0, name);
}

BtAdpAshmService::~BtAdpAshmService()
{
	mMemHeap = 0;
}

}; // namespace android

#endif
