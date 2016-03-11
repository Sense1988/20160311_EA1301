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
#include <binder/MemoryHeapBase.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>


#include "bt_adp_ashm.h"
#include "bt_adp_ashm_impl.h"

using namespace android;

/**
 * use for server-side to start ashm service and then client sides can get shared memory via btmtk_ashm_get_buffer
 */
void btmtk_ashm_start_service(const char* name, int size)
{
	status_t status;
	status = BtAdpAshmService::instantiate(name, size);
	if( status == NO_ERROR ){
		ProcessState::self()->startThreadPool();
		LOGI("[bt_ashm] ashm service[%s] started.", name);
	}
	else {
		LOGI("[bt_ashm] ashm service[%s] already exists.", name);
	}
}

/**
 * use for client-side to get shared memory
 */
sp<IMemoryHeap> btmtk_ashm_get_memory(const char* name)
{
	sp<IShBuffer> buffer = NULL;

	// Get the buffer service
	LOGD("[bt_ashm] get default ServiceManager and Binder");
	sp<IServiceManager> sm = defaultServiceManager();
	sp<IBinder> binder = sm->getService(String16(name));
	if (binder != 0)
	{
		LOGD("[bt_ashm] get buffer");
		buffer = IShBuffer::asInterface(binder);
	}
	if (buffer == NULL)
	{
		LOGE("[bt_ashm] ashm service[%s] is not ready.", name);
		return NULL; //(void*)-1;
	}
	else
	{
		return buffer->getBuffer();
	}
}

/**
 * usage example
 */
/*
int main(int argc, char** argv)
{
	char* opp = "com.mtk.bt.profile.opp";
	char* ftp = "com.mtk.bt.profile.ftp";
	unsigned char *buffer;

	if( argc == 1 ){

		// server mode
		LOGI("[bt_ashm] start service x 2");
		btmtk_ashm_start_service(opp, 2048);
		btmtk_ashm_start_service(ftp, 2048);
		IPCThreadState::self()->joinThreadPool();
	}
	else
	{
		// client mode
		LOGI("[bt_ashm] client access");

		// change opp
		buffer = (unsigned char *)btmtk_ashm_get_buffer(opp);
		if (buffer != (unsigned char*)-1)
		{
			LOGI("[bt_ashm] client access[%s] - old: ptr=%p val=0x%x\n", opp, buffer, *buffer);
			*buffer = (*buffer)+1;
			LOGI("[bt_ashm] client access[%s] - new: ptr=%p val=0x%x\n", opp, buffer, *buffer);
		}
		else
		{
			LOGE("[bt_ashm] error: shared memory not available\n");
		}

		// change ftp
		buffer = (unsigned char *)btmtk_ashm_get_buffer(ftp);
		if (buffer != (unsigned char*)-1)
		{
			LOGI("[bt_ashm] client access[%s] - old: ptr=%p val=0x%x\n", ftp, buffer, *buffer);
			*buffer = (*buffer)+2;
			LOGI("[bt_ashm] client access[%s] - new: ptr=%p val=0x%x\n", ftp, buffer, *buffer);
		}
		else
		{
			LOGE("[bt_ashm] error: shared memory not available\n");
		}
	}

	return 0;
}
*/
#endif
