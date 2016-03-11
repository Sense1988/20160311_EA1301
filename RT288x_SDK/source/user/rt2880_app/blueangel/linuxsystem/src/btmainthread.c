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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 * BtMainThread.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include <stdio.h>
#if !defined(BTMTK_ON_LINUX)
#include <windows.h> /* For the CreateThread prototype */
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/poll.h>
#include <linux/msg.h>
#include <signal.h>
#include <linux/futex.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"
#include "bluetooth_gap_struct.h"
#include "bt_feature.h"

#include "tst_ringbuffer.h"

#if defined(__ENABLE_BTNET_DEV__)
#include "btnet.h"
#endif

#if defined(__ENABLE_SLEEP_MODE__)
#include "eint.h"
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif 

#define __BT_PAL__ 1

#define USE_SELECT

extern void BTUartTriggerReadUart(void);

extern int g_serverSocket;
extern int g_a2dpstreamSocket;
extern int g_clientSocket;

extern int startTst();
extern int tst_flush(int milisec, int force);

void timerSignalHandler(int data)
{
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_TIMER_EXPIRY;
    ilm.src_mod_id = MOD_BT;
    ilm.dest_mod_id = MOD_BT;
    ilm.sap_id= INVALID_SAP;
    send (g_clientSocket, &ilm, (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE), MSG_DONTWAIT);
}

/*
void ExceptionHandler(int signum)
{
    tst_flush(3000, 1);
    signal(signum, SIG_DFL);
    kill(getpid, signum);
}
*/

extern void dumpTstStatus(void);
void userSigProcess(int data)
{
//    int i;
    dumpTstStatus();
}
int BTMTK_Main( void ); 

#if defined(__SPP_THROUGHPUT_TEST__)
int g_bSppServer = KAL_FALSE;
#endif

int main (int argc, char **argv)
{
    bt_prompt_trace(MOD_BT, "__FUNCTION__ = %s\n", __FUNCTION__);
    bt_prompt_trace(MOD_BT, "[BT] main\n");

#if defined(__SPP_THROUGHPUT_TEST__)
    if(argc >= 2)
   	{
   		OS_Report(("argv[1] = %s\n", argv[1]));
   		if(atoi(argv[1]) > 0)
   			g_bSppServer = KAL_TRUE;
   	}
   	OS_Report("g_bSppServer is %d\n", g_bSppServer?1:0));
#endif
    bt_messagehdl_init();
    signal(SIGALRM, timerSignalHandler);
    signal(SIGPIPE, SIG_IGN);
    //signal(SIGUSR1, userSigProcess);
    //signal(SIGSEGV, ExceptionHandler);
    //signal(SIGBUS, ExceptionHandler);
    BTMTK_Main();
    return 0;
}

extern int commPort;
/* Start Tst thread */

#if defined(__ANDROID_EMULATOR__) || defined(__MTK_STP_SUPPORTED__)|| defined(__BT_TRANSPORT_DRV__) || defined(__BT_USE_KERNEL_DRIVER__)
int set_bluetooth_power(int on) 
{
    // STP will do the power on
    return 0;
}
#else /* __ANDROID_EMULATOR__ */
static int rfkill_id = -1;
static char *rfkill_state_path = NULL;
static int init_rfkill() {
    char path[64];
    char buf[16];
    int fd;
    int sz;
    int id;
    bt_prompt_trace(MOD_BT, "init_rfkill");
    for (id = 0; ; id++) {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/type", id);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            bt_prompt_trace(MOD_BT, "[WARN] open(%s) failed: %s (%d)", path, strerror(errno), errno);
            return -1;
        }
        sz = read(fd, &buf, sizeof(buf));
        close(fd);
        if (sz >= 9 && memcmp(buf, "bluetooth", 9) == 0) {
            rfkill_id = id;
            break;
        }
    }
    asprintf(&rfkill_state_path, "/sys/class/rfkill/rfkill%d/state", rfkill_id);
    bt_prompt_trace(MOD_BT, "init_rfkill : path=%s, id=%d", rfkill_state_path, rfkill_id);
    return 0;
}
int set_bluetooth_power(int on) 
{
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');

    bt_prompt_trace(MOD_BT, "set_bluetooth_power(%d)", on);
    if (rfkill_id == -1) 
    {
        if (init_rfkill()) 
            goto out;
    }    
    fd = open(rfkill_state_path, O_WRONLY);
    if (fd < 0) 
    {
        bt_prompt_trace(MOD_BT, "[ERR] open(%s) for write failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        bt_android_log("[ERR] open(%s) for write failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        goto out;    
    }
    bt_android_log("write rfkill : %c", buffer);
    sz = write(fd, &buffer, 1);
    if (sz < 0) 
    {  
        bt_prompt_trace(MOD_BT, "[ERR] write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        bt_android_log("[ERR] write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        goto out;
    }
    ret = 0;
out:
    if (fd >= 0) 
        close(fd);
#if defined(__ENABLE_SLEEP_MODE__)
    if(on != 0)
    {
        if(ret == 0)
            initEint();
    }
    else
    {
        closeEint();
    }
#endif
    bt_android_log("set_bluetooth_power(%d)=%d", on, ret);
    return ret;
}
#endif

#if defined(__BT_HS_30__)	
extern int pal_main_begin();
extern int pal_main_middle(int ret, fd_set *prfds);
extern int pal_main_middle_all();
extern int pal_main_end(); 

#if defined(__BT_PAL__)
extern int getPalHandleNum();
extern int getPalHandle(int index);
int startPal(){
	return -1;
}
#else
// OOO:
static pid_t palpid = 0;
static pthread_t palthread;
static pthread_cond_t palcond;
kal_bool g_palStarted = KAL_FALSE;

// Use another therad
void* BTMTK_Pal(void *lpParam) 
{  
	pal_main_begin();
	pal_main_middle_all();	
	pal_main_end();		
	return 0;
}

int startPal(){
    int res = -1;

    bt_tst_log("[pal]  startPal");
    if( 0 != pthread_cond_init(&palcond, NULL))
    {
        res = -1;
        bt_tst_err("[pal] pthread_cond_init failed");
    }
    else
    {
        bt_tst_log("[pal] pthread_cond_init success");
        if( 0 != pthread_create( &palthread, NULL, BTMTK_Pal, NULL) )
        {
            res = -1;
            pthread_cond_destroy(&palcond);
            bt_tst_err("[pal] pthread_create failed");
        }
        else
        {
            g_palStarted = KAL_TRUE;
            bt_tst_log("[pal] pthread_create success");
        }
    }
    return res;

}


#endif
#endif


int BTMTK_Main( void ) 
{ 
    ilm_struct *ptr = 0;
	
    #if defined(__ENABLE_SLEEP_MODE__)
    int eintfd = -1;
    #endif
    #if defined(__ENABLE_BTNET_DEV__)
    int btnetfd = -1;
    #endif    
    
    bt_prompt_trace_lv3(MOD_BT, "BT main\n");

    /* +VSNIFF */
    initVirtualSniff();
    /* -VSNIFF */

    btmtk_adp_system_init();

    #ifdef __USE_CATCHER_LOG__
    startTst();
    #endif

#if defined(__BT_HS_30__)		
#if defined(__BT_PAL__)
	startPal();
	pal_main_begin();
#endif
#endif

    while(1)
    {
        int res = 0;
        fd_set readfs;
		int g_max_set_fd = 0;
        //struct timeval Timeout;

        //Timeout.tv_usec = 500;
        //Timeout.tv_sec  = 0;
        FD_ZERO(&readfs);
        if(commPort >= 0)
        {
            FD_SET(commPort, &readfs);
        }
        if(g_serverSocket >= 0)
        {
            FD_SET(g_serverSocket, &readfs);
			g_max_set_fd = max (g_max_set_fd, g_serverSocket);
        }
        else
        {
            bt_prompt_trace(MOD_BT, "[BT] g_serverSocket == 0. exit");
        }        
        if(g_a2dpstreamSocket >= 0)
        {
            FD_SET(g_a2dpstreamSocket, &readfs);
			g_max_set_fd = max (g_max_set_fd, g_a2dpstreamSocket);
        }
        /* Add sleep mode eint */
        #if defined(__ENABLE_SLEEP_MODE__)
        eintfd = getEintHandle();
        if(eintfd >= 0)
        {
            FD_SET(eintfd, &readfs);
			g_max_set_fd = max (g_max_set_fd, eintfd);			
        }
        #endif
        #if defined(__ENABLE_BTNET_DEV__)
        btnetfd = getBtNetHandle();
        if (btnetfd >= 0)
        {
            FD_SET(btnetfd, &readfs);
			g_max_set_fd = max (g_max_set_fd, btnetfd);			
        }
        #endif
	    #if defined(__BT_HS_30__) && defined(__BT_PAL__)
		{
            int i;
			int maxfd = 0;
            int btpalfd; /* pal temp file descriptor */
			PalHandle_init();
			maxfd = getPalHandleNum();
			/* Add the pal sockets */
			for( i = 0; i< maxfd; i++ ){
				btpalfd = getPalHandle(i);
	    	    if (btpalfd >= 0) 
	        	{
	            	FD_SET(btpalfd, &readfs);
					g_max_set_fd = max (g_max_set_fd, btpalfd);			
	    	    }else{
	    	    	// fail to get the pad fd !
	    	    	bt_prompt_trace_lv2(MOD_BT, "[BT][PAL] fail to get pal fds index:%d of total:%d", i, getPalHandleNum() );
	    	    } 
			}
		}
		#endif

        bt_prompt_trace_lv3(MOD_BT, "[BT] start select g_serverSocket=%d, commPort=%d g_a2dpstreamSocket=%d", g_serverSocket, commPort, g_a2dpstreamSocket);

#if 0		
        #if defined(__ENABLE_SLEEP_MODE__)
        #if defined(__ENABLE_BTNET_DEV__)
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket,max(eintfd, btnetfd))))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #else
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket,eintfd)))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #endif
        #else
        #if defined(__ENABLE_BTNET_DEV__)
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket, btnetfd)))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #else
        res = select(max(commPort, max(g_serverSocket,g_a2dpstreamSocket))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #endif
        #endif
#else
        res = select(max(commPort, g_max_set_fd)+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
#endif
		
        bt_prompt_trace_lv3(MOD_BT, "[BT] select return : %d", res);
        if(res < 0)
        {
            bt_prompt_trace(MOD_BT, "[BT] select failed : %s, errno=%d", strerror(errno), errno);
            continue;
        }

        if(res == 0)
        {
            bt_prompt_trace(MOD_BT, "[BT] wait uart timeout");
            btmtk_adp_system_main_loop();
        }
        else
        {
            #if defined(__ENABLE_SLEEP_MODE__)
            if(eintfd >= 0 && FD_ISSET(eintfd, &readfs))
            {
                ilm_struct ilm;
                ilm.msg_id = MSG_ID_BT_HOST_WAKE_UP_IND;
                bt_prompt_trace(MOD_BT, "[SM] BT Eint triggered");
                bt_main(&ilm);
            }
            #endif
            #if defined(__ENABLE_BTNET_DEV__)
            if (btnetfd >= 0 && FD_ISSET(btnetfd, &readfs))
            {
                ilm_struct ilm;
                ilm.msg_id = MSG_ID_BT_PAN_SEND_PACKET_REQ;
                bt_prompt_trace(MOD_BT, "[BT] PAN network device ready to read");
                bt_main(&ilm);
            }
            #endif
            if (commPort >= 0 && FD_ISSET(commPort, &readfs))
            {
                   bt_prompt_trace_lv3(MOD_BT, "[BT] com buffer filled");
                   BTUartTriggerReadUart();
            }
            if(g_serverSocket >= 0 && FD_ISSET(g_serverSocket, &readfs))
            {
                U16 *field;

//                bt_prompt_trace_lv3(MOD_BT, "[MSG] message received");
                ptr = (ilm_struct *)bt_deQueue_message(MOD_BT);
		if(NULL != ptr)
                {
                if(ptr->src_mod_id == MOD_PAL)
                {
                /// OOO: pal -> bt
                    field = (U16 *)ptr->ilm_data;
                    *field = 1;
                    field = (U16 *)&ptr->ilm_data[2];
                    if(*field > 100)
                        *field = 100;                
                    if(*field != 100)
                        bt_log_primitive(ptr);
                }
                else
                    bt_log_primitive(ptr);                    

                bt_main(ptr);
            }
            }
            if(g_a2dpstreamSocket >= 0 && FD_ISSET(g_a2dpstreamSocket, &readfs))
            {
//                int result = 0;
                ilm_struct ilm;
                bt_prompt_trace_lv3(MOD_BT, "[MSG] message received from g_a2dpstreamSocket");
                res = recvfrom(g_a2dpstreamSocket, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
                bt_log_primitive(&ilm);
                bt_main(&ilm);
            }
#if defined(__BT_HS_30__)
#if defined(__BT_PAL__)
			/// init pal check loop
			pal_main_middle(res, &readfs);
#endif
#endif
        }
    }
    bt_prompt_trace(MOD_BT, "[BT] BTMTK_Main exit!!");

#if defined(__BT_HS_30__)		
#if defined(__BT_PAL__)
	pal_main_end();
#endif
#endif

    return 0;
} 

