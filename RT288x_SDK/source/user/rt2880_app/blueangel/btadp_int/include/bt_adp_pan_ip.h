#ifndef __BT_ADP_PAN_IP_H__
#define __BT_ADP_PAN_IP_H__

/* A standard ethernet frame can carry a 1500 byte payload. 14 byte ethernet headers may included, four
     byte 802.1q VLAN tags, and 4 byte CRC's in their counts, for 1514, 1518, 1522.
     1518 is the most common number.
*/
#define PAN_MRU     1518    //Ethernet packet MRU
#define PAN_NET_DEV_IP_ADDR  0xC0A80501  // 192.168.5.1 static ip allocation

S32 pan_ip_open_network_dev(U8 *hw_addr);
void pan_ip_close_network_dev(void);
void pan_ip_send_packet(U8 *data, U16 len);
S16 pan_ip_receive_packet(U8 *buffer);

#endif
