/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_adp_pan_ip.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is platform based implementation. It is responsible for network packet transmission. 
 *
 * Author:
 * -------
 * Ting Zheng
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
#ifdef __BT_PAN_PROFILE__
#include <errno.h>
#include <linux/if.h>
#include <linux/in.h>

#include "bttypes.h"
#include "btnet.h"
#include "bt_adp_pan_ip.h"


/*
 * get_if_hwaddr - get the hardware address for the specified
 * network interface device.
 *
 * [in] ifr_name
 * [out] hw_addr
 */
static S32 pan_ip_get_if_hwaddr(char *hw_name, U8 *hw_addr)
{
    struct ifreq ifr;
    int ret;
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
        return -1;
    memset(&ifr, 0, sizeof(ifr));
    strlcpy(ifr.ifr_name, hw_name, sizeof(ifr.ifr_name));
    ret = ioctl(sock_fd, SIOCGIFHWADDR, &ifr);
    close(sock_fd);
    
    if (ret >= 0)
        memcpy(hw_addr, ifr.ifr_hwaddr.sa_data, 6);
    return ret;
}

/*
 * pan_ip_open_network_dev 
 * 
 * [in] none
 * [out] hw_addr
 */
S32 pan_ip_open_network_dev(U8 *hw_addr)
{
    int unit;
    char name[10];

    unit =  openBtNet();
    if (unit < 0)
    {
        return unit;
    }
    
    sprintf(name, "btn%d", unit);
    pan_ip_get_if_hwaddr(name, hw_addr);
    return unit;
}

void pan_ip_close_network_dev()
{
    closeBtNet();
}

void pan_ip_send_packet(U8 *data, U16 len)
{
    S32 btnfd = getBtNetHandle();
    S16 numWrite;
    
    if (btnfd >= 0)
    {
        numWrite = write(btnfd, data, len);
        bt_prompt_trace(MOD_BT, "pan ip write packet len: %d", numWrite);
        if (numWrite < 0) 
        {
            // error message
            bt_prompt_trace(MOD_BT, "write error: %s (%d)", strerror(errno), errno);
        }   
    }
}

S16 pan_ip_receive_packet(U8 *buffer)
{
    S32 btnfd = getBtNetHandle();
    S16 numRead;

    numRead = read(btnfd, buffer, PAN_MRU);
    bt_prompt_trace(MOD_BT, "pan ip read packet len: %d", numRead);
    bt_prompt_trace(MOD_BT, "pan ip read packet data: %x, %x, %x", buffer[0], buffer[1], buffer[2]);


    if (numRead < 0)
    {
        // error message
        bt_prompt_trace(MOD_BT, "read error: %s (%d)", strerror(errno), errno);
    }
    return numRead;
}
#endif
