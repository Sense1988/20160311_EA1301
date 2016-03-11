/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
//#include <cutils/log.h>
//#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BTSNIFF"


#define bt_tst_err(...) fprintf(stderr, __VA_ARGS__ )
#define bt_tst_dbg(...) fprintf(stderr, __VA_ARGS__ )

#define MAX_PATH 64
#define SNIFF_FOLDER "/tmp/"
#define MAX_LOG_FILE_SIZE 104857600 //100MB)


static unsigned int g_enableLog = 0;
static int g_hSnifferLogFile = -1;
static unsigned int virtual_sniffer_log_file_size=0;
#ifdef FOR_UBUNTU_TEST
int Virtual_Sniffer_RunTime_Config = 1;
#else
int Virtual_Sniffer_RunTime_Config = 0;
#endif

int VS_init();
void VS_deinit();
static int dbgWrite(const unsigned char *buf, int count);

static int dbgFileCreate(void)
{
    char           fileName[MAX_PATH]={0};
    char           fileName_tmp[MAX_PATH]={0};
    time_t  t = time(NULL);
    struct tm      *pTm = localtime(&t);
    
    if(Virtual_Sniffer_RunTime_Config == 0)
    {
        return -1;
    }
    
#ifdef FOR_UBUNTU_TEST
    snprintf(fileName, sizeof(fileName), SNIFF_FOLDER "blueangel_hci_sniffer_log_%d", (int)pTm->tm_year+1900);
#else
    if(0 == access("/mnt/sda1", F_OK))
    {
        snprintf(fileName, sizeof(fileName), "/mnt/sda1/blueangel_hci_sniffer_log_%d", (int)pTm->tm_year+1900);
    }
    else if(0 == access("/mnt/sdb1", F_OK))
    {
        snprintf(fileName, sizeof(fileName), "/mnt/sdb1/blueangel_hci_sniffer_log_%d", (int)pTm->tm_year+1900);
    }
    else if(0 == access("/mnt/sda", F_OK))
    {
        snprintf(fileName, sizeof(fileName), "/mnt/sda/blueangel_hci_sniffer_log_%d", (int)pTm->tm_year+1900);
    }
    else if(0 == access("/mnt/sdb", F_OK))
    {
        snprintf(fileName, sizeof(fileName), "/mnt/sdb/blueangel_hci_sniffer_log_%d", (int)pTm->tm_year+1900);
    }
    else
    {
        bt_tst_dbg("USB drive path not exist,!\n");
    	return -1;
    }
#endif

    snprintf(fileName_tmp, sizeof(fileName_tmp), "%s%02d", fileName, (int)pTm->tm_mon+1);
    memcpy(fileName, fileName_tmp, sizeof(fileName));
    memset(fileName_tmp, 0, sizeof(fileName_tmp));
    
    snprintf(fileName_tmp, sizeof(fileName_tmp), "%s%02d", fileName, (int)pTm->tm_mday);
    memcpy(fileName, fileName_tmp, sizeof(fileName));
    memset(fileName_tmp, 0, sizeof(fileName_tmp));
    
    snprintf(fileName_tmp, sizeof(fileName_tmp), "%s%02d", fileName, (int)pTm->tm_hour);
    memcpy(fileName, fileName_tmp, sizeof(fileName));
    memset(fileName_tmp, 0, sizeof(fileName_tmp));
    
    snprintf(fileName_tmp, sizeof(fileName_tmp), "%s%02d", fileName, (int)pTm->tm_min);
    memcpy(fileName, fileName_tmp, sizeof(fileName));
    memset(fileName_tmp, 0, sizeof(fileName_tmp));
    
    snprintf(fileName_tmp, sizeof(fileName_tmp), "%s%02d.cfa", fileName, (int)pTm->tm_sec);
    memcpy(fileName, fileName_tmp, sizeof(fileName));

    bt_tst_dbg("\033[1;32mVirtual Sniffer Log : open file(%s) \033[m\n", fileName);
    
    g_hSnifferLogFile = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    if(g_hSnifferLogFile < 0)
    {
    	bt_tst_dbg("Open file failed : errno=%d, %s\n", errno, strerror(errno));
    	return -1;
    }
    else
    {
        int     bytesWritten;
        unsigned char   fileId[] = { 0x62, 0x74, 0x73, 0x6e, 0x6f, 0x6f, 0x70, 0x00};
        unsigned char   fileVer[] = { 0x00, 0x00, 0x00, 0x01};
        unsigned char   fileType[] = { 0x00, 0x00, 0x03, 0xEA};
        
        /* magic word */
        bytesWritten = dbgWrite(fileId, sizeof(fileId));
        if(bytesWritten != sizeof(fileId))
        {
            bt_tst_dbg("write fileId failed\n");
            return -1;
        }

        /* version 1 */
        bytesWritten = dbgWrite(fileVer, sizeof(fileVer));
        if(bytesWritten != sizeof(fileVer))
        {
            bt_tst_dbg("write fileVer failed\n");
            return -1;
        }

        /* 1001 H4 */
        bytesWritten = dbgWrite(fileType, sizeof(fileType));
        if(bytesWritten != sizeof(fileType))
        {
            bt_tst_dbg("write fileType failed\n");
            return -1;
        }
    }
    return 1;
}


static int dbgFileClose ()
{
    int status = 0;

    if (g_hSnifferLogFile >= 0) 
    {
        close(g_hSnifferLogFile);
        g_hSnifferLogFile = -1;
    }
    return status;
}

static int dbgWrite(const unsigned char *buf, int count)
{
    int nWritten = 0;
    int bytesToWrite;

    // Iverson add : need be removed
    // disable virtual sniffer log
    return 0;
    // Iverson add end.

    if(g_hSnifferLogFile < 0)
    {
        bt_tst_err("g_hSnifferLogFile is not opened\n");
        return -1;
    }

    bytesToWrite = count;    
    while(bytesToWrite > 0)
    {
        if ( Virtual_Sniffer_RunTime_Config )
        {
            nWritten = write(g_hSnifferLogFile, buf, bytesToWrite);
            virtual_sniffer_log_file_size += nWritten;
        }
        else
        {
            return count;
        }

        if ( nWritten <= 0 )
        {
            bt_tst_err("dbgWrite failed : nWritten=%d, err=%s, errno=%d\n", nWritten, strerror(errno), errno);
            return -1;
        }
            
        bytesToWrite -= nWritten;
        buf += nWritten;
    }
    return (count - bytesToWrite);

}

typedef unsigned long long uint64_t;

static uint64_t gettimestamp() 
{
    struct timeval tv;
    unsigned long long result = 0/*EPOCH_DIFF*/;
    gettimeofday(&tv,NULL);
    result += (tv.tv_sec-946684800ll);
    result *= 1000000LL;
    result += tv.tv_usec;
    return result;
}

static int dbgLogRaw (unsigned int fgSend, unsigned char *debugRaw, int logLen, int totallen)
{
    int bytesWritten = -1;
    unsigned char   ucTemp[24];
    uint64_t ulResultTime;
    uint64_t timeCurrent;

    if (g_hSnifferLogFile < 0) 
    {
        return -1;    
    }

    //Original Length
    ucTemp[0] = (unsigned char)(totallen >> 24); 
    ucTemp[1] = (unsigned char)(totallen >> 16);
    ucTemp[2] = (unsigned char)(totallen >> 8);
    ucTemp[3] = (unsigned char)(totallen);
    
    //Included Length
    ucTemp[4] = (unsigned char)(totallen >> 24); 
    ucTemp[5] = (unsigned char)(totallen >> 16);
    ucTemp[6] = (unsigned char)(totallen >> 8);
    ucTemp[7] = (unsigned char)(totallen);

    //Packet Flags
    ucTemp[8] = 0;                        
    ucTemp[9] = 0;
    ucTemp[10] = 0;
    if(fgSend)
    {
        ucTemp[11] = 0;
    }
    else
    {
        ucTemp[11] = 1;
    }

    //Cumulative Drops
    ucTemp[12] = 0;                        
    ucTemp[13] = 0;
    ucTemp[14] = 0;
    ucTemp[15] = 0;

    //current time
    timeCurrent = gettimestamp();
    ulResultTime = 0x00E03AB44A676000ll + timeCurrent;

    //Timestamp Microseconds high part
    ucTemp[16] = (unsigned char)((ulResultTime >> 56) & 0xFF);
    ucTemp[17] = (unsigned char)((ulResultTime >> 48) & 0xFF);
    ucTemp[18] = (unsigned char)((ulResultTime >> 40) & 0xFF);
    ucTemp[19] = (unsigned char)((ulResultTime >> 32) & 0xFF);
    
    //Timestamp Microseconds low part
    ucTemp[20] = (unsigned char)((ulResultTime >> 24) & 0xFF);
    ucTemp[21] = (unsigned char)((ulResultTime >> 16) & 0xFF);
    ucTemp[22] = (unsigned char)((ulResultTime >> 8) & 0xFF);
    ucTemp[23] = (unsigned char)((ulResultTime) & 0xFF);

    bytesWritten = dbgWrite(ucTemp, sizeof(ucTemp));
    if(bytesWritten < 0)
    {
        return -1;
    }

    /* Write message log to log file */
    bytesWritten = dbgWrite(debugRaw, logLen);
    if(bytesWritten < 0)
    {
        return -1;
    }

    return 0;
}

int VS_init(void)
{
    if(dbgFileCreate() < 0)
    {
        bt_tst_err("dbgFileCreate failed\n");
        VS_deinit();
        return -1;
    }
    else
    {
        g_enableLog = 1;
        virtual_sniffer_log_file_size=0;
    }
    return 0;
}

void VS_deinit(void)
{
    bt_tst_err("VS_deinit()\n");
    dbgFileClose();
    g_enableLog = 0;
}

int writePacket(unsigned char *pBuff, unsigned int len)
{
    int ret = -1;
    if(g_enableLog)
    {
        ret = dbgLogRaw(1, pBuff, len, len);

        if ( ret == -1 || virtual_sniffer_log_file_size > MAX_LOG_FILE_SIZE )
        {
            VS_deinit();
            VS_init();        
        }
    }
    return ret;
}

int readPacket(unsigned char type, unsigned char *pBuff, unsigned int len)
{
    int ret = -1;
    /* including UART hdr */
    if(g_enableLog)
    {
        ret = dbgLogRaw(0, &type, 1, len+1);
        if(ret >= 0)
        {
            ret = dbgWrite(pBuff,len);
        }
    }
    return ret;
}

