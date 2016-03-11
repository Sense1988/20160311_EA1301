/*******************************************************************************
 *
 * Filename:
 * ---------
 * btnet.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to communicate with bt network device
 *
 * Author:
 * -------
 * Ting Zheng
 ********************************************************************************/
#ifdef __ENABLE_BTNET_DEV__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <linux/bt_net_dev.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <linux/if.h>
#include <linux/in.h>
#include <sys/endian.h>

#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_log.h"

static int g_btnfd = -1;
static int g_btnunit;

#define BTNETWORK_DEVNAME			"/dev/btn"
#define BTNETWORK_DEV_IPADDR			0xc0a80501	// 192.168.5.1
#define BTNETWORK_DEV_NETMASK		0xffffff00	//255.255.255.0

#define SIN_ADDR(x)	(((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define SIN_FAMILY(x)	(((struct sockaddr_in *) (&(x)))->sin_family)

#ifdef CONFIG_BTNET_DEV
#undef CONFIG_BTNET_DEV
#endif

void closeBtNet(void);
static S32 configBtNetDev(int unit);


int openBtNet(void)
{
    int unit = -1;
    char ipcfg[60] = {0};

    if (g_btnfd >= 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]btn device is opened already");    
        return g_btnunit;
    }
    
    bt_prompt_trace(MOD_BT, "[BTN]initBtNet");    

    g_btnfd = open(BTNETWORK_DEVNAME, O_RDWR);
  
    if (g_btnfd < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet open btn device failed, %s(%d)", strerror(errno), errno);   
        return -1;
    }
   // mask by mtk94037 for build erro
   // ioctl(g_btnfd, BTNIOCNEWUNIT_ETH, &unit);	
    bt_prompt_trace(MOD_BT, "[BTN]initBtNet pan fd: %d, unit index: %d", g_btnfd, unit);    

#ifdef CONFIG_BTNET_DEV
    //config ip addr
    if (unit >= 0)
    {
        if (configBtNetDev(unit) < 0)
        {
            closeBtNet();
            return -1;			
        }
    }
#endif

    g_btnunit = unit;
	
    return g_btnunit;    
}

void closeBtNet(void)
{
    /* unnecessary to use ioctl to close network unit, since it shall be destroyed with the file accordingly */
    close(g_btnfd);
    g_btnfd = -1;
}

int getBtNetHandle(void)
{
    return g_btnfd;
}

static S32 configBtNetDev(int unit)
{
    struct ifreq ifr;
    int fd;	
    int ret = -1;	

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd <= 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet create socket failed,  %s(%d)",  fd, strerror(errno), errno);    
        return -1;
    }
    
    memset(&ifr, 0, sizeof(ifr));
    sprintf(ifr.ifr_name, "btn%d", unit);	


    SIN_FAMILY(ifr.ifr_addr) = AF_INET;
    SIN_ADDR(ifr.ifr_addr) = htonl(BTNETWORK_DEV_IPADDR);
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet set ip address failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }

    ifr.ifr_flags = IFF_UP;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet up interface failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }


    SIN_FAMILY(ifr.ifr_netmask) = AF_INET;
    SIN_ADDR(ifr.ifr_netmask) = htonl(BTNETWORK_DEV_NETMASK);
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        bt_prompt_trace(MOD_BT, "[BTN]initBtNet set netmask failed,  %s(%d)",  strerror(errno), errno);    
        goto exit;
    }

    ret = 0;

exit:
    close(fd);
    return ret;	
}

#endif
