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

/***************************************************************************
 *
 * File:
 *     $Workfile:uart.c$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision: #1 $
 *
 * Description:
 *      HCI Transport - UART physical driver.
 *      This file provides the interface between the UART HCI Transport 
 *      Driver that is platform independent and the controller or driver
 *      for the controller.
 *
 *      This particular driver uses Microsoft's VCOMM driver for accessing
 *      the Bluetooth controller.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined (BTMTK_ON_LINUX)
#include "assert.h"
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>


#include "bttypes.h"
#include "bt_types.h"
#include "bt_adp_fs.h"
#include "bt_debug.h"
#include "bt_feature.h"
#ifndef __BT_USE_KERNEL_DRIVER__
#include "cust_bt.h"
#endif

extern int set_bluetooth_power(int on);
extern int set_speed(int fd, struct termios *ti, int speed);

#if defined(__ANDROID_EMULATOR__)
#define CUST_BT_SERIAL_PORT "/dev/ttyS2"
#endif

typedef void (*UartCallback) (unsigned char event);

extern unsigned char BTCoreVerifySysInitState(void);

typedef struct {
    UartCallback cb_func;
}bt_adp_uart;

bt_adp_uart BTAdpUartCtx;

int openComm(void);

int commPort = -1;

#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
/* HCI dev events */
#define HCI_DEV_REG	1
#define HCI_DEV_UNREG	2
#define HCI_DEV_UP	3
#define HCI_DEV_DOWN	4
#define HCI_DEV_SUSPEND	5
#define HCI_DEV_RESUME	6

#define HCI_COMMAND_PKT		0x01
#define HCI_ACLDATA_PKT		0x02
#define HCI_SCODATA_PKT		0x03
#define HCI_EVENT_PKT		0x04
#define HCI_VENDOR_PKT		0xff
#define HCI_MAX_ACL_SIZE	1024
#define HCI_MAX_SCO_SIZE	255
#define HCI_MAX_EVENT_SIZE	260
#define HCI_MAX_FRAME_SIZE	(HCI_MAX_ACL_SIZE + 4)

#define SOL_HCI		0
#define HCI_FILTER	2
#define HCI_DEV_NONE	0xffff

#define HCI_MAX_DEV	1
#define BTPROTO_HCI	1

#define HCI_FLT_TYPE_BITS	31
#define HCI_FLT_EVENT_BITS	63

#define HCIDEVUP	_IOW('H', 201, int)
#define HCIGETDEVLIST	_IOR('H', 210, int)

#define EVT_INQUIRY_COMPLETE		0x01
#define EVT_INQUIRY_RESULT		0x02
#define EVT_CONN_COMPLETE		0x03
#define EVT_CONN_REQUEST		0x04
#define EVT_DISCONN_COMPLETE		0x05
#define EVT_CMD_STATUS 			0x0F
#define EVT_AUTH_COMPLETE		0x06
#define EVT_REMOTE_NAME_REQ_COMPLETE	0x07
#define EVT_READ_REMOTE_FEATURES_COMPLETE	0x0B
#define EVT_READ_REMOTE_VERSION_COMPLETE	0x0C
#define EVT_CMD_COMPLETE 		0x0E
#define EVT_RETURN_LINK_KEYS		0x15
#define EVT_PIN_CODE_REQ		0x16
#define EVT_LINK_KEY_REQ		0x17
#define EVT_LINK_KEY_NOTIFY		0x18
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
#define EVT_EXTENDED_INQUIRY_RESULT	0x2F
#define EVT_IO_CAPABILITY_REQUEST	0x31
#define EVT_IO_CAPABILITY_RESPONSE	0x32
#define EVT_USER_CONFIRM_REQUEST	0x33
#define EVT_USER_PASSKEY_REQUEST	0x34
#define EVT_REMOTE_OOB_DATA_REQUEST	0x35
#define EVT_SIMPLE_PAIRING_COMPLETE	0x36
#define EVT_USER_PASSKEY_NOTIFY		0x3B
#define EVT_KEYPRESS_NOTIFY		0x3C
#define EVT_REMOTE_HOST_FEATURES_NOTIFY	0x3D
#define EVT_LE_META_EVENT	0x3E/* HCI device flags */

#define EVT_ENCRYPT_CHANGE  0x08
#define EVT_HARDWARE_ERROR  0x10
#define EVT_FLUSH_OCCURRED  0x11
#define EVT_ROLE_CHANGE     0x12
#define EVT_NUM_COMP_PKTS   0x13
#define EVT_MODE_CHANGE     0x14
#define EVT_SYNC_CONN_COMPLETE  0x2C
#define EVT_SYNC_CONN_CHANGED   0x2D
#define EVT_LINK_SUPERVISION_TIMEOUT_CHANGED  0x38

#define HCISETRAW       _IOW('H', 220, int)

enum {
	HCI_UP,
	HCI_INIT,
	HCI_RUNNING,

	HCI_PSCAN,
	HCI_ISCAN,
	HCI_AUTH,
	HCI_ENCRYPT,
	HCI_INQUIRY,

	HCI_RAW,

	HCI_SETUP,
	HCI_AUTO_OFF,
	HCI_MGMT,
	HCI_PAIRABLE,
	HCI_SERVICE_CACHE,
	HCI_LINK_KEYS,
	HCI_DEBUG_KEYS,

	HCI_RESET,
};

U8 sock_rx_buf[HCI_MAX_FRAME_SIZE];
U16 sock_rx_buf_start = 0, sock_rx_buf_end = 0;

#define DBG(fmt, arg...)  bt_prompt_trace(MOD_BT, "DEBUG: %s: " fmt "\n" , __FUNCTION__ , ## arg)

static struct dev_info {
	int id;
	int sk;

	int already_up;

} devs;

struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};

struct hci_dev_req {
	uint16_t dev_id;
	uint32_t dev_opt;
};

struct hci_dev_list_req {
	uint16_t dev_num;
	struct hci_dev_req dev_req[0];	/* hci_dev_req structures */
};

#define LINE_WIDTH          16
void DisplayHex(const char *Source, const U8 *Buffer, U16 Len)
{
    const U8 *p = Buffer;
    char *o, output[(LINE_WIDTH * 4) + 20]; /* 12 bytes padding */
    int po;

    po = 0;

    bt_prompt_trace(MOD_BT, " %s(%d) \n", Source, Len);
    while (p < Buffer + Len)
    {
        o = output;

        /* Append proper line advance spaces */
//        o += sprintf(o, " %s  ", Source);

        /* Dumps the packet in basic ASCII dump format */
        po = 0;
        while (po < LINE_WIDTH)
        {
            if (p + po < Buffer + Len)
            {
                o += snprintf(o, 3, " %02x", p[po]);
            }
            else
            {
                o += snprintf(o, 4, "   ");
            }
            if (++po == LINE_WIDTH / 2)
            {
                o += snprintf(o, 3, "  ");
            }
        }

//        bt_prompt_trace(MOD_BT, "%s\n", output);
        p += po;
    }
}

static inline void hci_set_bit(int nr, void *addr)
{
	*((uint32_t *) addr + (nr >> 5)) |= (1 << (nr & 31));
}

static inline void hci_filter_clear(struct hci_filter *f)
{
	memset(f, 0, sizeof(*f));
}

static inline void hci_filter_set_ptype(int t, struct hci_filter *f)
{
	hci_set_bit((t == HCI_VENDOR_PKT) ? 0 : (t & HCI_FLT_TYPE_BITS), &f->type_mask);
}

static inline void hci_filter_set_event(int e, struct hci_filter *f)
{
	hci_set_bit((e & HCI_FLT_EVENT_BITS), &f->event_mask);
}

static struct dev_info *init_dev_info(int index, int sk, int already_up)
{
	struct dev_info *dev = &devs;

	memset(dev, 0, sizeof(*dev));

	dev->id = index;
	dev->sk = sk;
    commPort = sk;
	bt_prompt_trace(MOD_BT, "init_dev_info: commPort = %d", commPort);
	dev->already_up = already_up;
	return dev;
}


static void start_hci_dev(int index)
{
	struct dev_info *dev = &devs;
	struct hci_filter flt;

	bt_prompt_trace(MOD_BT, "Listening for HCI events on hci%d", index);

	/* Set filter */
	hci_filter_clear(&flt);
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	hci_filter_set_ptype(HCI_ACLDATA_PKT, &flt);
	hci_filter_set_ptype(HCI_SCODATA_PKT, &flt);
	hci_filter_set_event(EVT_CMD_STATUS, &flt);
	hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
	hci_filter_set_event(EVT_PIN_CODE_REQ, &flt);
	hci_filter_set_event(EVT_LINK_KEY_REQ, &flt);
	hci_filter_set_event(EVT_LINK_KEY_NOTIFY, &flt);
	hci_filter_set_event(EVT_RETURN_LINK_KEYS, &flt);
	hci_filter_set_event(EVT_IO_CAPABILITY_REQUEST, &flt);
	hci_filter_set_event(EVT_IO_CAPABILITY_RESPONSE, &flt);
	hci_filter_set_event(EVT_USER_CONFIRM_REQUEST, &flt);
	hci_filter_set_event(EVT_USER_PASSKEY_REQUEST, &flt);
	hci_filter_set_event(EVT_REMOTE_OOB_DATA_REQUEST, &flt);
	hci_filter_set_event(EVT_USER_PASSKEY_NOTIFY, &flt);
	hci_filter_set_event(EVT_KEYPRESS_NOTIFY, &flt);
	hci_filter_set_event(EVT_SIMPLE_PAIRING_COMPLETE, &flt);
	hci_filter_set_event(EVT_AUTH_COMPLETE, &flt);
	hci_filter_set_event(EVT_REMOTE_NAME_REQ_COMPLETE, &flt);
	hci_filter_set_event(EVT_READ_REMOTE_VERSION_COMPLETE, &flt);
	hci_filter_set_event(EVT_READ_REMOTE_FEATURES_COMPLETE, &flt);
	hci_filter_set_event(EVT_REMOTE_HOST_FEATURES_NOTIFY, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_EXTENDED_INQUIRY_RESULT, &flt);
	hci_filter_set_event(EVT_CONN_REQUEST, &flt);
	hci_filter_set_event(EVT_CONN_COMPLETE, &flt);
	hci_filter_set_event(EVT_DISCONN_COMPLETE, &flt);
	hci_filter_set_event(EVT_ENCRYPT_CHANGE, &flt);
	hci_filter_set_event(EVT_HARDWARE_ERROR, &flt);
	hci_filter_set_event(EVT_FLUSH_OCCURRED, &flt);
	hci_filter_set_event(EVT_ROLE_CHANGE, &flt);
	hci_filter_set_event(EVT_NUM_COMP_PKTS, &flt);
	hci_filter_set_event(EVT_MODE_CHANGE, &flt);
	hci_filter_set_event(EVT_SYNC_CONN_COMPLETE, &flt);
	hci_filter_set_event(EVT_SYNC_CONN_CHANGED, &flt);
	hci_filter_set_event(EVT_LINK_SUPERVISION_TIMEOUT_CHANGED, &flt);
	hci_filter_set_event(EVT_LE_META_EVENT, &flt);
	if (setsockopt(dev->sk, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
		bt_prompt_trace(MOD_BT, "Can't set filter on hci%d: %s (%d)",
						index, strerror(errno), errno);
		return;
	}

	if (fcntl(dev->sk, F_SETFL, fcntl(dev->sk, F_GETFL, 0) | O_NONBLOCK) < 0) {
		bt_prompt_trace(MOD_BT, "Can't set non blocking mode: %s (%d)\n",
						strerror(errno), errno);
		return;
	}

}

int hci_close_dev(int dd)
{
	DBG("hci_close_dev %d", dd);
	return close(dd);
}

static void stop_hci_dev(int index)
{
	struct dev_info *dev = &devs;

	if (dev->sk < 0)
		return;

	bt_prompt_trace(MOD_BT, "Stopping hci%d event socket %d", index, dev->sk);

	hci_close_dev(dev->sk);
	init_dev_info(index, -1, dev->already_up);
}


/* Open HCI device.
 * Returns device descriptor (dd). */
int hci_open_dev(int dev_id)
{
	struct sockaddr_hci a;
	int dd, err;

    DBG("");
	/* Create HCI socket */
	dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (dd < 0)
		return dd;

    DBG("sock=%x",dd);
	/* Bind socket to the HCI device */
	memset(&a, 0, sizeof(a));
	a.hci_family = AF_BLUETOOTH;
	a.hci_dev = dev_id;
	if (bind(dd, (struct sockaddr *) &a, sizeof(a)) < 0)
		goto failed;

    if (ioctl(dd, HCISETRAW, 1) < 0) {
        DBG("ioctl error : HCISETRAW\n");
    }

	return dd;

failed:
	err = errno;
	close(dd);
	errno = err;

	return -1;
}

static struct dev_info *init_device(int index, int already_up)
{
	struct dev_info *dev;
	struct hci_dev_req dr;
	int dd;

	DBG("init_device hci%d", index);

	dd = hci_open_dev(index);
	if (dd < 0) {
		bt_prompt_trace(MOD_BT, "Unable to open hci%d: %s (%d)", index,
						strerror(errno), errno);
		return NULL;
	}


	dev = init_dev_info(index, dd, already_up);
	start_hci_dev(index);

	/* Avoid forking if nothing else has to be done */
	if (already_up)
		return dev;

	/* Do initialization in the separate process */
	memset(&dr, 0, sizeof(dr));
	dr.dev_id = index;

	/* Start HCI device */
	if (ioctl(dd, HCIDEVUP, index) < 0 && errno != EALREADY) {
		bt_prompt_trace(MOD_BT, "Can't init device hci%d: %s (%d)",
					index, strerror(errno), errno);
		goto fail;
	}
    return dev;

fail:
	hci_close_dev(dd);
    return NULL;
}


static void device_event(int event, int index)
{
	DBG("event %d, index %d", event, index);

	switch (event) {
	case HCI_DEV_REG:
		bt_prompt_trace(MOD_BT, "HCI dev %d registered", index);
		init_device(index, 0);
		break;

	case HCI_DEV_UNREG:
		bt_prompt_trace(MOD_BT, "HCI dev %d unregistered", index);
		stop_hci_dev(index);
		break;

	case HCI_DEV_UP:
		bt_prompt_trace(MOD_BT, "HCI dev %d up", index);
		break;

	case HCI_DEV_DOWN:
		bt_prompt_trace(MOD_BT, "HCI dev %d down", index);
		break;
	}
}

static inline int hci_test_bit(int nr, void *addr)
{
	return *((uint32_t *) addr + (nr >> 5)) & (1 << (nr & 31));
}

static int init_known_adapters(int ctl)
{
	struct hci_dev_list_req *dl;
	struct hci_dev_req *dr;
	int i, err;
	size_t req_size;

	DBG("");

	req_size = HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t);

	dl = malloc(req_size);
	if (!dl) {
		bt_prompt_trace(MOD_BT, "Can't allocate devlist buffer");
		return 0;
	}

	dr = dl->dev_req;

    while (1)
    {
        dl->dev_num = HCI_MAX_DEV;
    	if (ioctl(ctl, HCIGETDEVLIST, dl) < 0) {
    		err = -errno;
    		bt_prompt_trace(MOD_BT, "Can't get device list: %s (%d)", strerror(-err), -err);
    		free(dl);
    		return 0;
    	}

        if (dl->dev_num == 0)
        {
    		bt_prompt_trace(MOD_BT, "No BT adapter. Wait....");
            usleep(300000);
        }
        else
        {
            break;
        }
    }

    bt_prompt_trace(MOD_BT, "dl->dev_num = %d", dl->dev_num);
	for (i = 0; i < dl->dev_num; i++, dr++) {
		struct dev_info *dev;
		int already_up;

		already_up = hci_test_bit(HCI_UP, &dr->dev_opt);

		dev = init_device(dr->dev_id, already_up);
		if (dev == NULL)
			continue;

		if (!dev->already_up)
			continue;

		device_event(HCI_DEV_UP, dr->dev_id);
	}

	free(dl);

	return 0;
}

int mtk_hciops_setup(void)
{
	int sock, err;

	DBG("");

	/* Create and bind HCI socket */
	sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (sock < 0) {
		err = -errno;
		bt_prompt_trace(MOD_BT, "Can't open HCI socket: %s (%d)", strerror(-err),
								-err);
		return err;
	}
    DBG("sock=%x",sock);

	/* Set filter */
    init_known_adapters(sock);
    hci_close_dev(sock);

	return 0;
}

void mtk_hciops_teardown(void)
{
    stop_hci_dev(0);
}

U16 hci_sock_read(int fd, U8 *buffer, U16 length)
{
    int readByte = 0;
    int ret;
    if (fd < 0 || buffer == NULL || length == 0)
    {
        return 0;
    }

    if (sock_rx_buf_start == sock_rx_buf_end)
    {
        Assert(sock_rx_buf_start == 0);
        ret = read(fd, sock_rx_buf, HCI_MAX_FRAME_SIZE);
        if (ret < 0) {
            if (errno != EAGAIN)
            {
                bt_prompt_trace(MOD_BT, "[UART][ERR] UART_Read : %d", ret);
            }
            goto error;
        }
        else
        {
            sock_rx_buf_end = ret;
            //DisplayHex("SOCK read", sock_rx_buf, ret);
        }
    }

    Assert(sock_rx_buf_end > sock_rx_buf_start);
    readByte = min(length, sock_rx_buf_end - sock_rx_buf_start);
    OS_MemCopy(buffer, &sock_rx_buf[sock_rx_buf_start], readByte);
    sock_rx_buf_start += readByte;
    if (sock_rx_buf_start == sock_rx_buf_end)
    {
        sock_rx_buf_start = 0;
        sock_rx_buf_end = 0;
    }

error:
    return readByte;
}
#endif

/*************************************************
 *  Name : BTUartLinuxInit
 *  Parameter : 
 *    func : If there is Rx data, callback to stack to handle
 *  Return value : 
 *    BT_STATUS_SUCCESS : Success
 *    BT_STATUS_FAILED : Failed
 *************************************************/
BtStatus BTUartLinuxInit(UartCallback func)
{
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
	if (mtk_hciops_setup() < 0) 
    {
		bt_prompt_trace(MOD_BT, "adapter_ops_setup failed");
        return BT_STATUS_FAILED;
    }
    else
    {
        BTAdpUartCtx.cb_func = func;
        return BT_STATUS_SUCCESS;
    }
#else
    // Checking if the uart is already opened
    // if com port is opened, let opencomm to close then open again
    /*
    if (commPort >= 0)
    {
        bt_android_log("[UART][ERR] the uart already opened : %d", commPort);
        bt_prompt_trace(MOD_BT, "[UART][ERR] the uart already opened : %d", commPort);
        return BT_STATUS_SUCCESS;
    }
    */
   // Create this write operation's OVERLAPPED structure's hEvent.
    BTAdpUartCtx.cb_func = func;
    //Power on BT before open uart
    set_bluetooth_power(0);
    /* Power on controller */
    if( set_bluetooth_power(1) < 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] failed to set bt power");
        return BT_STATUS_FAILED;
    }

    if ( openComm() != 0 ){
        set_bluetooth_power(0);
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
#endif
}

unsigned char BTUartLinuxShutdown(void)
{
    bt_prompt_trace(MOD_BT, "[UART] Close UART port : %d", commPort);
    if(commPort >= 0)
    {
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
        mtk_hciops_teardown();
        commPort = -1;
        BTAdpUartCtx.cb_func = NULL;
#else
        bt_android_log("[UART] Close UART port : %d", commPort);
        if( close(commPort) < 0 ) {
            bt_prompt_trace(MOD_BT, "[UART] Close UART port failed : %d, %s", errno, strerror(errno));
            bt_android_log("[UART][ERR] Close UART port failed : %d, %s", errno, strerror(errno));
        }
        commPort = -1;
#endif
    }
    return 0x00;
}

void BTUartTriggerReadUart(void)
{
    if(BTCoreVerifySysInitState() == 1)
        BTAdpUartCtx.cb_func(0x01);
}

#ifndef __BT_USE_KERNEL_DRIVER__
static void wholeChipReset(int err)
{
  bt_prompt_trace(MOD_BT, "[RESET] wholeChipReset : errno=%d", errno);
  Assert(0);
  if(err == 88){
		waitWholeChipResetComplete();
  }
	close(commPort);
	commPort = -1;
}
#endif

/* static char packetbuf[1024]; */
unsigned short BTUartLinuxRead(unsigned char *buffer, unsigned short length)
{
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
    U16 readByte = 0;
    if(commPort >= 0)
    {
        readByte = hci_sock_read(commPort, buffer, length);
    }
    return readByte;
#else
    unsigned short bytesToCopy = 0;
    int nRead = 0;

    bytesToCopy = length;
    bt_prompt_trace_lv3(MOD_BT, "[BT] Read uart buffer=0x%x, length=%d, commport=%d", buffer, length, commPort);

    if(commPort < 0)
    {
        bt_prompt_trace(MOD_BT, "Read com after closed");
        return 0;
    }

    while(bytesToCopy && (nRead = read(commPort, buffer, bytesToCopy)) > 0)
    {
        bt_prompt_trace_lv3(MOD_BT, "[BT] %d bytes is read from uart", nRead);
        bytesToCopy -= nRead;
        buffer += nRead;
    }
    if(nRead < 0){
        //bt_prompt_trace(MOD_BT, "[RESET] BTUartLinuxRead return %d, errno=%d", nRead, errno);
        if(errno == 88 || errno == 99){
        	wholeChipReset(errno);
        	return 0;
        }
    }
    bt_prompt_trace_lv3(MOD_BT, "[BT] total %d bytes is read", length - bytesToCopy);
	
    return (length - bytesToCopy);
#endif
} 

/*static char packetbuf[1024];*/
unsigned short BTUartLinuxWrite(const unsigned char *buffer, unsigned short length)
{
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
    U16 bytesWritten = 0;
	struct iovec iv[3];
	int ivn;
    int ret;

	iv[0].iov_base = (unsigned char*)buffer;
	iv[0].iov_len  = length;
	ivn = 1;

    //bt_prompt_trace(MOD_BT, "UART_Write : commPort=%d, len=%d", commPort, length);
	if ((ret = writev(commPort, iv, ivn)) < 0) {  //commPort = dd Device descriptor returned by hci_open_dev
	    bt_prompt_trace(MOD_BT, "UART_Write error(%d): %s", errno, strerror(errno));
	}
    else
    {
        bytesWritten = (U16)ret;
    }
    return bytesWritten;
#else
    int nWritten = 0;
    int bytesToWrite;
    U16 i;
    unsigned char *ptr;


    bytesToWrite = length;

    /*
    ptr = packetbuf;
    for(i = 0;i < length;i++)
    {
        sprintf(ptr, "%02X ", buffer[i]);
        ptr+=3;
    }
    //bt_prompt_trace(MOD_BT, "[BT] Write %d bytes to uart", length);
    bt_prompt_trace(MOD_BT, "[BT] Write to uart %d : \"%s\"", commPort, packetbuf);
    */

    if(commPort < 0)
    {
        bt_prompt_trace(MOD_BT, "Write com after closed");
        return 0;
    }

	
    while(bytesToWrite > 0)
    {
        bt_prompt_trace_lv3(MOD_BT, "[BT] Start write to uart buffer=0x%x, bytesToWrite=%d", buffer, bytesToWrite);
        nWritten = write(commPort, buffer, bytesToWrite);
        if (nWritten > 0)
        {
            bt_prompt_trace_lv3(MOD_BT, "[BT][UART] %d bytes are written to uart", nWritten);
            bytesToWrite -= nWritten;
            buffer += nWritten;
        }
        else
        {
            bt_prompt_trace(MOD_BT, "[UART] Write uart failed : nWritten=%d, err=%s, errno=%d", nWritten, strerror(errno), errno);
            break;
        }
    }
    if(nWritten < 0){
        //bt_prompt_trace(MOD_BT, "[RESET] BTUartLinuxWrite return %d, errno=%d", nWritten, errno);
        if(errno == 88 || errno == 99){
        	wholeChipReset(errno);
        	return 0;
        }
    }
    bt_prompt_trace_lv3(MOD_BT, "[BT][UART] total wrote %d bytes to uart", length - bytesToWrite);
    return (length - bytesToWrite);
#endif
} 

void BTUartLinuxSetSpeed(unsigned short speed)
{
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
#else
    struct termios options;
    bt_prompt_trace(MOD_BT, "[BT][UART] set uart speed to speed %d", speed);
    return;
    /* Get default com port setting */
    if( -1 == tcgetattr(commPort, &options) )
    {
        return;
    }

    /* Set baud rate to 115200 */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    /* Set new com port setting */
    if( -1 == tcsetattr(commPort, TCSANOW, &options) )
    {
        return;
    }
    return;
#endif
}


int readUartSetting()
{
    int file_handle;
    int port;
    file_handle = btmtk_fs_open((const U8*)"\\uart.ini", BTMTK_FS_READ_ONLY);

    if (file_handle < 0)   /* first time, create the file */
    {
        char *str = "[BT][UART]COM_PORT = COM4\n";
        file_handle = btmtk_fs_open((const U8*)"\\uart.ini", BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        btmtk_fs_write(file_handle, (U8*)str, strlen(str));
        btmtk_fs_close(file_handle);
    }
    else
    {
        char str[20] = {0};
        btmtk_fs_read(file_handle, (U8*)str, 20);
        sscanf(str, "COM_PORT = COM%d", &port);
        btmtk_fs_close(file_handle);
    }
    return port;
}

int openComm(void)
{
#ifdef __BT_USE_KERNEL_DRIVER__  //TODO: spin off to USB driver file
    return -1;
#else
#if 1
    struct termios ti;
    char *device = CUST_BT_SERIAL_PORT;

    bt_prompt_trace(MOD_BT, "[UART] openComm");
    bt_android_log("[UART] openComm");
    if (commPort >= 0)  //modified by autumn
    {
        bt_prompt_trace(MOD_BT, "[UART] openComm already opened");
        bt_android_log("[UART][ERR] commPort=%d already opened.", commPort);
        bt_android_log("[BT][UART] + close uart=%d", commPort);
        close(commPort);
        bt_android_log("[BT][UART] - close uart");
    }
    bt_android_log("[BT][UART] + open uart");
    commPort = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);    // Modified by SH
    bt_android_log("[BT][UART] - open uart");
    if (commPort < 0) {
        bt_prompt_trace(MOD_BT, "[UART]Can't open serial port: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't open serial port: %s (%d)\n", strerror(errno), errno);
        return -1;
    }

#ifndef __MTK_STP_SUPPORTED__
    //fcntl(commPort, F_SETFL, FASYNC); // Added by SH
    tcflush(commPort, TCIOFLUSH);

    if (tcgetattr(commPort, &ti) < 0) {
        bt_prompt_trace(MOD_BT, "[UART]Can't get port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't get port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Close commPort");
        close(commPort);
        commPort = -1;
        return -1;
    }

    cfmakeraw(&ti);

    ti.c_cflag |= CLOCAL;
    ti.c_cflag &= ~CRTSCTS;
    #if !defined(__ANDROID_EMULATOR__)
    /* Emulator does not support flow control */
    ti.c_iflag &= ~(IXON | IXOFF | IXANY | 0x80000000);
    #endif
	      
    if (tcsetattr(commPort, TCSANOW, &ti) < 0) {
        bt_prompt_trace(MOD_BT, "[UART] Can't change port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Can't change port settings: %s (%d)\n", strerror(errno), errno);
        bt_android_log("[UART][ERR] Close commPort");
        close(commPort);
        commPort = -1;
        return -1;
    }
        
    if( set_speed(commPort, &ti, 115200) < 0){
        bt_android_log("[UART][ERR] set_speed failed");
        close(commPort);
        commPort = -1;
        return -1;
    }

    tcflush(commPort, TCIOFLUSH);
#endif /* ifndef __MTK_STP_SUPPORTED__ */
    bt_prompt_trace(MOD_BT, "[UART] Opening UART successfully : %d", commPort);
    bt_android_log("[UART] Opening UART successfully : %d", commPort);
    return 0;
#else
    
    char portName[20];
    
    sprintf(portName, "/dev/ttyS%d", 2/*readUartSetting()*/);

    bt_prompt_trace(MOD_BT, "Opening UART port : %s", portName);
    
    /* Open UART port */
    if (-1 == (commPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) ) 
    {
        bt_prompt_trace(MOD_BT, "[Failed] Opening UART port ");
        return 0;
    } 
    else
    {
        struct termios options;

        //fcntl(commPort, F_SETOWN, getpid());
        fcntl(commPort, F_SETFL, FASYNC);

        /* Get default com port setting */
        if( -1 == tcgetattr(commPort, &options) )
        {
            bt_prompt_trace(MOD_BT, "[Failed] tcgetattr UART port failed");
            close(commPort);
            return 0;
        }

        /* Set baud rate to 115200 */
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);

        options.c_cflag |= (CLOCAL | CREAD);

        options.c_cflag &= ~PARENB;  /* No parity */
        options.c_cflag &= ~CSTOPB;  /* 1 stop bit */
        options.c_cflag &= ~CSIZE;   /* Mask the character size bits */
        options.c_cflag |= CS8;         /* Select 8 data bits */
        /* Disable RTS & CTS */
        options.c_cflag &= ~CRTSCTS;

        tcflush(commPort, TCIFLUSH);
        /* Set new com port setting */

        if( -1 == tcsetattr(commPort, TCSANOW, &options) )
        {
            bt_prompt_trace(MOD_BT, "[Failed] tcsetattr UART port failed");
            close(commPort);
            return 0;
        }

        return 1;
    }
#endif
#endif
} 
#endif  /* #if defined (BTMTK_ON_LINUX) */


