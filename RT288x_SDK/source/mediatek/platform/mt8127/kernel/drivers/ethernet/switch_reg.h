/**
 *  @file    switch_reg.h
 *  @brief   header file to define hardware registers
 *
 *  @author  HaoRen Kao (mtk02196)
 */
#ifndef _SWITCH_REG_H_
#define _SWITCH_REG_H_


/* Cavium Switch Register */
/* ====================================================================================== */
#define SWITCH_PHY_CTRL(base)             (base + 0x000)  /* PHY Control Register */
#define SWITCH_CFG1(base)                 (base + 0x004)  /* Switch Configuration Register 1 */
#define SWITCH_MAC0_CFG(base)	          (base + 0x008)  /* MAC Port 0 Configuration Register */
#define SWITCH_MAC1_CFG(base)             (base + 0x00c)  /* MAC Port 1 Configuration Register */
#define SWITCH_MAC2_CFG(base)             (base + 0x010)  /* MAC Port 2 (MII Port) Configuration Register */
#define SWITCH_PRI_CTRL(base)             (base + 0x014)  /* Priority Control Register */
#define SWITCH_UDP_PRI(base)              (base + 0x018)  /* UDP Priority Register */
#define SWITCH_TOS_PRI_GP0(base)          (base + 0x01c)  /* IP TOS 0~7 Priority Group 0 Register */
#define SWITCH_TOS_PRI_GP1(base)          (base + 0x020)  /* IP TOS 8~15 Priority Group 1 Register */
#define SWITCH_TOS_PRI_GP2(base)          (base + 0x024)  /* IP TOS 16~23 Priority Group 2 Register */
#define SWITCH_TOS_PRI_GP3(base)          (base + 0x028)  /* IP TOS 24~31 Priority Group 3 Register */
#define SWITCH_TOS_PRI_GP4(base)          (base + 0x02c)  /* IP TOS 32~39 Priority Group 4 Register */
#define SWITCH_TOS_PRI_GP5(base)          (base + 0x030)  /* IP TOS 40~47 Priority Group 5 Register */
#define SWITCH_TOS_PRI_GP6(base)          (base + 0x034)  /* IP TOS 48~55 Priority Group 6 Register */
#define SWITCH_TOS_PRI_GP7(base)          (base + 0x038)  /* IP TOS 56~63 Priority Group 7 Register */
#define SWITCH_TOS_PRI(base,group)        (SWITCH_TOS_PRI_GP0(base) + ((group)*4))  /* IP TOS Priority Group 0~7 Register */
#define SWITCH_SCH_CTRL(base)             (base + 0x03c)  /* Scheduling Control Register */
#define SWITCH_RATE_CTRL1(base)           (base + 0x040)  /* Rate Limit Control Register 1 */
#define SWITCH_FC_GLOBAL(base)            (base + 0x044)  /* Flow Control Global Threshold Register */
#define SWITCH_FC_PORT(base)              (base + 0x048)  /* Flow Control Port Threshold Register */
#define SWITCH_SMART_FC(base)             (base + 0x04c)  /* Smart Flow Control Register */
#define SWITCH_ARL_CTRL0(base)            (base + 0x050)  /* ARL Table Access Control 0 Register */
#define SWITCH_ARL_CTRL1(base)            (base + 0x054)  /* ARL Table Access Control 1 Register */
#define SWITCH_ARL_CTRL2(base)            (base + 0x058)  /* ARL Table Access Control 2 Register */
#define SWITCH_PORT_VID(base)             (base + 0x05c)  /* Port VID Register */
#define SWITCH_VLAN_GID01(base)           (base + 0x060)  /* VLAN Group ID 0~1 Register */
#define SWITCH_VLAN_GID23(base)           (base + 0x064)  /* VLAN Group ID 2~3 Register */
#define SWITCH_VLAN_GID45(base)           (base + 0x068)  /* VLAN Group ID 4~5 Register */
#define SWITCH_VLAN_GID67(base)           (base + 0x06c)  /* VLAN Group ID 6~7 Register */
#define SWITCH_VLAN_GID(base,gid)		  (SWITCH_VLAN_GID01(base) + ((gid)/2)*4)
#define SWITCH_VLAN_PM(base)              (base + 0x070)  /* VLAN Port Map Register */
#define SWITCH_VLAN_TAG_PM(base)          (base + 0x074)  /* VLAN Tag Port Map Register */
#define SWITCH_SESSION_ID01(base)         (base + 0x078)  /* Session ID 0~1 Register */
#define SWITCH_SESSION_ID23(base)         (base + 0x07c)  /* Session ID 2~3 Register */
#define SWITCH_SESSION_ID45(base)         (base + 0x080)  /* Session ID 4~5 Register */
#define SWITCH_SESSION_ID67(base)         (base + 0x084)  /* Session ID 6~7 Register */
#define SWITCH_INT_STA0(base)             (base + 0x088)  /* Interrupt Status Register 0 */
#define SWITCH_INT_MASK0(base)            (base + 0x08c)  /* Interrupt Mask Register 0 */
#define SWITCH_ATPOLL_PHY(base)           (base + 0x090)  /* Auto-Polling PHY Address Register */
#define SWITCH_TEST0(base)                (base + 0x094)  /* Test 0 Register (BIST Result & Clock Skew Setting) */
#define SWITCH_TEST1(base)                (base + 0x098)  /* Test 1 Register (Queue Status) */
#define SWITCH_IST_CTRL(base)             (base + 0x09c)  /* Inter Switch Tag Control Register */
#define SWITCH_CFG2(base)                 (base + 0x0a0)  /* Switch Configuration Register 2 */
#define SWITCH_RATE_CTRL2(base)           (base + 0x0a4)  /* Rate Limit Control Register 2 */
#define SWITCH_INT_STA1(base)             (base + 0x0a8)  /* Interrupt Status Register 1 */
#define SWITCH_INT_MASK1(base)            (base + 0x0ac)  /* Interrupt Mask Register 1 */
#define SWITCH_TEST2(base)                (base + 0x0b0)  /* Test 2 Register (Clock Skew Setting) */
#define SWITCH_TEST3(base)                (base + 0x0b4)  /* Test 3 Register (Queue Status) */


/* =============================================================
		Detail definition of Cavium Switch registers
   ============================================================= */
/* SWITCH_PHY_CTRL */
/* =================================================================== */
#define SWITCH_PHY_CTRL_RWDATA_MASK         (0xffff)           /* Mask of Read/Write Data */
#define SWITCH_PHY_CTRL_RWDATA_OFFSET       (16)               /* Offset of Read/Write Data */
#define SWITCH_PHY_CTRL_RWOK                (1 << 15)          /* R/W command has completed (write 1 clear) */
#define SWITCH_PHY_CTRL_RDCMD               (1 << 14)          /* Read command (self clear) */
#define SWITCH_PHY_CTRL_WTCMD               (1 << 13)          /* Write command (self clear) */
#define SWITCH_PHY_CTRL_PHYREG_MASK         (0x1f)             /* Mask of PHY Register Address */
#define SWITCH_PHY_CTRL_PHYREG_OFFSET		(8)                /* Offset of PHY Register Address */
#define SWITCH_PHY_CTRL_PHYADDR_MASK		(0x1f)             /* Mask of PHY Address */
#define SWITCH_PHY_CTRL_PHYADDR_OFFSET		(0)                /* Offset of PHY Address */

/* SWITCH_CFG1 */
/* =================================================================== */
#define SWITCH_CFG1_P2_AS_CPU               (1 << 31)          /* 0: Treat as normal Port; 1: Treat as CPU Port */
#define SWITCH_CFG1_NIC_MODE                (1 << 30)          /* Network Ports are treated as NICs */
#define SWITCH_CFG1_SKIP_L2_LKUP_P1         (1 << 29)          /* Re-direct to CPU from port 1 */
#define SWITCH_CFG1_SKIP_L2_LKUP_P0         (1 << 28)          /* Re-direct to CPU from port 0 */
#define SWITCH_CFG1_NWAYIF_EN               (1 << 27)          /* Enable the PHY management interface */
#define SWITCH_CFG1_RMCTOCPU				(1 << 26)          /* Reserved multicast frame to CPU only */
#define SWITCH_CFG1_SAFILTER_MODE	        (1 << 25)          /* SA filter mode */
#define SWITCH_CFG1_FRWALL_MODE             (1 << 24)          /* Firewall Mode */
#define SWITCH_CFG1_HNAT_EN                 (1 << 23)          /* HNAT Accelerator Enable */
#define SWITCH_CFG1_IVL                     (1 << 22)          /* Independent VLAN Learning Enable */
#define SWITCH_CFG1_CRCSTRIP                (1 << 21)          /* CRC Stripping */
#define SWITCH_CFG1_COLMODE_MASK            (0x3)              /* Collision Mode */
#define SWITCH_CFG1_COLMODE_OFFSET          (19)
#define SWITCH_CFG1_COLMODE_ND              (0)                /* Never Drop */
#define SWITCH_CFG1_COLMODE_LIMIT_1         (1)                /* Collision limit : 1 */
#define SWITCH_CFG1_COLMODE_LIMIT_2         (2)                /* Collision limit : 2 */
#define SWITCH_CFG1_COLMODE_LIMIT_16        (3)                /* Collision limit : 16 (standard, default value) */
#define SWITCH_CFG1_RMC_FILTER              (1 << 18)          /* Reserved MC Filtering */
#define SWITCH_CFG1_BP_MODE_MASK            (0x3)              /* Back Pressure Mode */
#define SWITCH_CFG1_BP_MODE_OFFSET          (16)
#define SWITCH_CFG1_BP_MODE_DIS             (0)                /* Disable back pressure */
#define SWITCH_CFG1_BP_MODE_SMARTBP         (1)                /* Smart back pressure */
#define SWITCH_CFG1_BP_MODE_NORMAL          (2)                /* Jam all incoming packets until back pressure condition release */
#define SWITCH_CFG1_BP_MODE_FCH             (3)                /* Force carrier HIGH to do back pressure */
#define SWITCH_CFG1_JAMNUM_MASK             (0xf)              /* Back pressure consecutive Jam Number */
#define SWITCH_CFG1_JAMNUM_OFFSET           (12)
#define SWITCH_CFG1_COLCNT_CLR_MODE			(1 << 11)          /* 0: colcnt only reset when transmit a packet; 1: colcnt reset when tx a pkt or rx a pkt */
#define SWITCH_CFG1_BKOFF_MODE_MASK         (0x7)              /* Collision Back Off Timer Setting */
#define SWITCH_CFG1_BKOFF_MODE_OFFSET       (8)
#define SWITCH_CFG1_HASH_ALG_MASK           (0x3)              /* MAC Address Hashing Algorithm */
#define SWITCH_CFG1_HASH_ALG_OFFSET         (6)
#define SWITCH_CFG1_HASH_ALG_DIRECT         (0)
#define SWITCH_CFG1_HASH_ALG_XOR48          (1)
#define SWITCH_CFG1_HASH_ALG_XOR32          (2)
#define SWITCH_CFG1_MAXLEN_MASK             (0x3)              /* Maximum Packet Length */
#define SWITCH_CFG1_MAXLEN_OFFSET           (4)
#define SWITCH_CFG1_MAXLEN_1518             (0)
#define SWITCH_CFG1_MAXLEN_1522             (1)
#define SWITCH_CFG1_MAXLEN_1536             (2)
#define SWITCH_CFG1_MAXLEN_9K               (3)
#define SWITCH_CFG1_AGETIME_MASK            (0xf)              /* Aging Time Setting: 2^(N-1)*300sec when N = 0001~0111, Fast aging when N = 1xxx */
#define SWITCH_CFG1_AGETIME_OFFSET          (0)

/* SWITCH_MAC_CFG for MAC Port 0~2 */
/* =================================================================== */
#define SWITCH_MAC_CFG_BCS_BC               (1 << 31)          /* Include broadcast packets into Broadcast Storm Rate Control */
#define SWITCH_MAC_CFG_BCS_MC               (1 << 30)          /* Include multicast packets into Broadcast Storm Rate Control */
#define SWITCH_MAC_CFG_BCS_UN               (1 << 29)          /* Include unknown packets into Broadcast Storm Rate Control */
#define SWITCH_MAC_CFG_UCPAUSE_DIS          (1 << 28)          /* Disable the treating of unicast pause frames as 802.3x pause frames */
#define SWITCH_MAC_CFG_FWD_BC_CPU_DIS       (1 << 27)          /* Forwarding broadcast packets to CPU Disable */
#define SWITCH_MAC_CFG_FWD_MC_CPU_DIS       (1 << 26)          /* Forwarding multicast packets to CPU Disable */
#define SWITCH_MAC_CFG_FWD_UN_CPU_DIS       (1 << 25)          /* Forwarding unknown packets to CPU Disable */
#define SWITCH_MAC_CFG_INGRESS_CHK_EN       (1 << 24)          /* Ingress check enable */
#define SWITCH_MAC_CFG_SA_SECURE            (1 << 23)          /* SA secured mode. 0: Forward packets without caring if SA match; 1: Only forward packets with SA/Port match */
#define SWITCH_MAC_CFG_AGE_EN               (1 << 22)          /* Aging Enable. 0: MAC address doesn't be aged out */
#define SWITCH_MAC_CFG_BLK_MODE             (1 << 21)          /* 0: Forward all packets to CPU in blocking mode; 1: Only forward BPDU to CPU in blocking state */
#define SWITCH_MAC_CFG_BLK_STATE            (1 << 20)          /* Blocking state */
#define SWITCH_MAC_CFG_SA_LEARN_DIS         (1 << 19)          /* SA Learning Disable */
#define SWITCH_MAC_CFG_PORT_DIS             (1 << 18)          /* Port Disable */
#define SWITCH_MAC_CFG_BP_EN                (1 << 17)          /* Back pressure enable */
#define SWITCH_MAC_CFG_RGMII_PHY            (1 << 15)          /* RGMII_PHY used */
#define SWITCH_MAC_CFG_REVERSE_MII          (1 << 14)          /* Reversed MII Mode enable */
#define SWITCH_MAC_CFG_TXC_CKEN             (1 << 13)          /* TX Clock period checking enable */
#define SWITCH_MAC_CFG_FORCETXFC            (1 << 12)          /* Force TX flow control when AN disable (only for 1000Mbps) */
#define SWITCH_MAC_CFG_FORCERXFC            (1 << 11)          /* Force RX flow control when AN disable */
#define SWITCH_MAC_CFG_FORCEFULL            (1 << 10)          /* Force Full Duplex when AN disable */
#define SWITCH_MAC_CFG_FORCESPD_MASK        (0x3)              /* Mask of Force Speed when AN disable */
#define SWITCH_MAC_CFG_FORCESPD_OFFSET      (8)                /* Offset of Force Speed when MI disable */
#define SWITCH_MAC_CFG_FORCESPD_10M         (0)
#define SWITCH_MAC_CFG_FORCESPD_100M        (1)
#define SWITCH_MAC_CFG_FORCESPD_1G          (2)
#define SWITCH_MAC_CFG_FORCESPD_RESV        (3)
#define SWITCH_MAC_CFG_ANEN                 (1 << 7)           /* Auto-Negotiation Enable */
	/* Auto-polling PHY status */
#define SWITCH_MAC_CFG_STA_TXFC             (1 << 6)           /* TX Flow Control status (only for 1000Mbps) */
#define SWITCH_MAC_CFG_STA_RXFC             (1 << 5)           /* RX Flow Control status */
#define SWITCH_MAC_CFG_STA_FULL             (1 << 4)           /* Duplex status, 1:full 0:half */
#define SWITCH_MAC_CFG_STA_SPD_MASK         (0x3)              /* Mask of Speed status */
#define SWITCH_MAC_CFG_STA_SPD_OFFSET       (2)                /* Offset of Speed status */
#define SWITCH_MAC_CFG_STA_SPD_10M          (0)
#define SWITCH_MAC_CFG_STA_SPD_100M         (1)
#define SWITCH_MAC_CFG_STA_SPD_1G           (2)
#define SWITCH_MAC_CFG_STA_SPD_RESV         (3)
#define SWITCH_MAC_CFG_STA_TXCLK            (1 << 1)           /* TX clock status, 0:normal 1:no TXC or clk period too long */
#define SWITCH_MAC_CFG_STA_LINK             (1 << 0)           /* PHY Link status */

/* SWITCH_PRI_CTRL */
/* =================================================================== */
#define SWITCH_PRI_CTRL_UDP_PRI_MASK        (0x7)              /* Priority of UDP packet received from all ports */
#define SWITCH_PRI_CTRL_UDP_PRI_OFFSET      (21)
#define SWITCH_PRI_CTRL_PORT2_PRI_MASK      (0x7)              /* Priority of all packets received from port 2 */
#define SWITCH_PRI_CTRL_PORT2_PRI_OFFSET    (18)
#define SWITCH_PRI_CTRL_PORT1_PRI_MASK      (0x7)              /* Priority of all packets received from Port 1 */
#define SWITCH_PRI_CTRL_PORT1_PRI_OFFSET    (15)
#define SWITCH_PRI_CTRL_PORT0_PRI_MASK      (0x7)              /* Priority of all packets received from Port 0 */
#define SWITCH_PRI_CTRL_PORT0_PRI_OFFSET    (12)

#define SWITCH_PRI_CTRL_TOS_PORT2_EN        (1 << 11)          /* IP TOS Priority Check Enable - Port 2 */
#define SWITCH_PRI_CTRL_TOS_PORT1_EN        (1 << 10)          /* IP TOS Priority Check Enable - Port 1 */
#define SWITCH_PRI_CTRL_TOS_PORT0_EN        (1 << 9)           /* IP TOS Priority Check Enable - Port 0 */

#define SWITCH_PRI_CTRL_UDP_PORT2_EN        (1 << 8)           /* Per port UDP packet Priority Check Enable - Port 2 */
#define SWITCH_PRI_CTRL_UDP_PORT1_EN        (1 << 7)           /* Per port UDP packet Priority Check Enable - Port 1 */
#define SWITCH_PRI_CTRL_UDP_PORT0_EN        (1 << 6)           /* Per port UDP packet Priority Check Enable - Port 0 */

#define SWITCH_PRI_CTRL_VLAN_PORT2_EN       (1 << 5)           /* Per port VLAN Priority Check Enable - Port 2 */
#define SWITCH_PRI_CTRL_VLAN_PORT1_EN       (1 << 4)           /* Per port VLAN Priority Check Enable - Port 1 */
#define SWITCH_PRI_CTRL_VLAN_PORT0_EN       (1 << 3)           /* Per port VLAN Priority Check Enable - Port 0 */

#define SWITCH_PRI_CTRL_REGEN_USER_PRI      (1 << 2)           /* Regenerate User Priority in TX Priority Tag */

#define SWITCH_PRI_CTRL_TRAFFIC_CLASS_MASK  (0x3)              /* Number of Traffic class */
#define SWITCH_PRI_CTRL_TRAFFIC_CLASS_OFFSET (0)
#define SWITCH_PRI_CTRL_TRAFFIC_CLASS_1     (0)                /* 1 traffic class */
#define SWITCH_PRI_CTRL_TRAFFIC_CLASS_2     (1)                /* 2 traffic class */
#define SWITCH_PRI_CTRL_TRAFFIC_CLASS_4     (2)                /* 4 traffic class */

/* SWITCH_UDP_PRI */
/* =================================================================== */
#define SWITCH_UDP_PRI_DEFINED_PORT_MASK    (0xffff)          /* UDP defined Port */
#define SWITCH_UDP_PRI_DEFINED_PORT_OFFSET  (0)

/* SWITCH_TOS_PRI Group0 ~ Group7 */
/* =================================================================== */
#define SWITCH_TOS_PRI_MASK                 (0x7)             /* Mask of IP TOS Priority */
#define SWITCH_TOS_PRI_OFFSET(index)        ((index)*3)       /* Offset of IP TOS Priority, index is 0~7 */

/* SWITCH_SCH_CTRL */
/* =================================================================== */
#define SWITCH_SCH_CTRL_Q3WGT_MASK          (0x7)             /* Queue weight (for Weighted Round-Robin) */
#define SWITCH_SCH_CTRL_Q3WGT_OFFSET        (16)
#define SWITCH_SCH_CTRL_Q3WGT_1             (0)               /* Weight = 1 */
#define SWITCH_SCH_CTRL_Q3WGT_2             (1)               /* Weight = 2 */
#define SWITCH_SCH_CTRL_Q3WGT_4             (2)               /* Weight = 4 */
#define SWITCH_SCH_CTRL_Q3WGT_8             (3)               /* Weight = 8 */
#define SWITCH_SCH_CTRL_Q3WGT_16            (4)               /* Weight = 16 */

#define SWITCH_SCH_CTRL_Q2WGT_MASK          (0x7)             /* Queue weight (for Weighted Round-Robin) */
#define SWITCH_SCH_CTRL_Q2WGT_OFFSET        (12)
#define SWITCH_SCH_CTRL_Q2WGT_1             (0)               /* Weight = 1 */
#define SWITCH_SCH_CTRL_Q2WGT_2             (1)               /* Weight = 2 */
#define SWITCH_SCH_CTRL_Q2WGT_4             (2)               /* Weight = 4 */
#define SWITCH_SCH_CTRL_Q2WGT_8             (3)               /* Weight = 8 */
#define SWITCH_SCH_CTRL_Q2WGT_16            (4)               /* Weight = 16 */

#define SWITCH_SCH_CTRL_Q1WGT_MASK          (0x7)             /* Queue weight (for Weighted Round-Robin) */
#define SWITCH_SCH_CTRL_Q1WGT_OFFSET        (8)
#define SWITCH_SCH_CTRL_Q1WGT_1             (0)               /* Weight = 1 */
#define SWITCH_SCH_CTRL_Q1WGT_2             (1)               /* Weight = 2 */
#define SWITCH_SCH_CTRL_Q1WGT_4             (2)               /* Weight = 4 */
#define SWITCH_SCH_CTRL_Q1WGT_8             (3)               /* Weight = 8 */
#define SWITCH_SCH_CTRL_Q1WGT_16            (4)               /* Weight = 16 */

#define SWITCH_SCH_CTRL_Q0WGT_MASK          (0x7)             /* Queue weight (for Weighted Round-Robin) */
#define SWITCH_SCH_CTRL_Q0WGT_OFFSET        (4)
#define SWITCH_SCH_CTRL_Q0WGT_1             (0)               /* Weight = 1 */
#define SWITCH_SCH_CTRL_Q0WGT_2             (1)               /* Weight = 2 */
#define SWITCH_SCH_CTRL_Q0WGT_4             (2)               /* Weight = 4 */
#define SWITCH_SCH_CTRL_Q0WGT_8             (3)               /* Weight = 8 */
#define SWITCH_SCH_CTRL_Q0WGT_16            (4)               /* Weight = 16 */

#define SWITCH_SCH_CTRL_SCH_MODE_MASK       (0x3)             /* Scheduling Mode */
#define SWITCH_SCH_CTRL_SCH_MODE_OFFSET     (0)
#define SWITCH_SCH_CTRL_SCH_MODE_WRR        (0)               /* Weighted Round-Robin */
#define SWITCH_SCH_CTRL_SCH_MODE_STRICT     (1)               /* Strict priority */
#define SWITCH_SCH_CTRL_SCH_MODE_MIX        (2)               /* MIX mode */

/* SWITCH_RATE_CTRL1 */
/* =================================================================== */
#define SWITCH_RATE_CTRL1_BCS_RATE_MASK     (0xf)             /* Broadcast Storm Rate = 64 kbps * 2^N, N = 0~10 */
#define SWITCH_RATE_CTRL1_BCS_RATE_OFFSET   (24)
#define SWITCH_RATE_CTRL1_P1TXBW_MASK       (0x7f)            /* Port 1 TX Bandwidth = N * base_rate */
#define SWITCH_RATE_CTRL1_P1TXBW_OFFSET     (16)
#define SWITCH_RATE_CTRL1_P0TXBW_MASK       (0x7f)            /* Port 0 TX Bandwidth = N * base_rate */
#define SWITCH_RATE_CTRL1_P0TXBW_OFFSET     (8)
#define SWITCH_RATE_CTRL1_P1_BASERATE_MASK  (0x3)             /* Port 1 Base Rate */
#define SWITCH_RATE_CTRL1_P1_BASERATE_OFFSET (4)
#define SWITCH_RATE_CTRL1_P1_BASERATE_64K   (0)
#define SWITCH_RATE_CTRL1_P1_BASERATE_1M    (1)
#define SWITCH_RATE_CTRL1_P1_BASERATE_10M   (2)
#define SWITCH_RATE_CTRL1_P0_BASERATE_MASK  (0x3)             /* Port 0 Base Rate */
#define SWITCH_RATE_CTRL1_P0_BASERATE_OFFSET (4)
#define SWITCH_RATE_CTRL1_P0_BASERATE_64K   (0)
#define SWITCH_RATE_CTRL1_P0_BASERATE_1M    (1)
#define SWITCH_RATE_CTRL1_P0_BASERATE_10M   (2)
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_MASK  (0x3)            /* Egress bucket size for Egress Rate Limit */
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_OFFSET (0)
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_1P5K (0)
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_3K   (1)
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_9K   (2)
#define SWITCH_RATE_CTRL1_EGRESS_BKTSZ_12K  (3)

/* SWITCH_FC_GLOBAL */
/* =================================================================== */
#define SWITCH_FC_GLOBAL_MCFC_RLS_THRE_MASK     (0xf)      /* MC Flow Control Release Threshold.  unit: packet */
#define SWITCH_FC_GLOBAL_MCFC_RLS_THRE_OFFSET   (20)
#define SWITCH_FC_GLOBAL_MCFC_SET_THRE_MASK     (0xf)      /* MC Flow Control Assert Threshold.  unit: packet */
#define SWITCH_FC_GLOBAL_MCFC_SET_THRE_OFFSET   (16)
#define SWITCH_FC_GLOBAL_FC_RLS_THRE_MASK       (0x7f)     /* Flow Control Release Threshold.  unit: page */
#define SWITCH_FC_GLOBAL_FC_RLS_THRE_OFFSET     (8)
#define SWITCH_FC_GLOBAL_FC_SET_THRE_MASK       (0x7f)     /* Flow Control Assert Threshold.  unit: page */
#define SWITCH_FC_GLOBAL_FC_SET_THRE_OFFSET     (0)

/* SWITCH_FC_PORT */
/* =================================================================== */
#define SWITCH_FC_PORT_PRI3_THRE_MASK       (0x7f)         /* Buffer Threshold for Priority 3 packets.  unit: packet */
#define SWITCH_FC_PORT_PRI3_THRE_OFFSET     (24)
#define SWITCH_FC_PORT_PRI2_THRE_MASK       (0x7f)         /* Buffer Threshold for Priority 2 packets.  unit: packet */
#define SWITCH_FC_PORT_PRI2_THRE_OFFSET     (16)
#define SWITCH_FC_PORT_PRI1_THRE_MASK       (0x7f)         /* Buffer Threshold for Priority 1 packets.  unit: packet */
#define SWITCH_FC_PORT_PRI1_THRE_OFFSET     (8)
#define SWITCH_FC_PORT_PRI0_THRE_MASK       (0x7f)         /* Buffer Threshold for Priority 0 packets.  unit: packet */
#define SWITCH_FC_PORT_PRI0_THRE_OFFSET     (0)

/* SWITCH_SMART_FC */
/* =================================================================== */
#define SWITCH_SMART_FC_SPD2_MASK           (0x3)          /* Port 2 Speed used for smart flow control */
#define SWITCH_SMART_FC_SPD2_OFFSET         (6)
#define SWITCH_SMART_FC_SPD1_MASK           (0x3)          /* Port 1 Speed used for smart flow control */
#define SWITCH_SMART_FC_SPD1_OFFSET         (4)
#define SWITCH_SMART_FC_SPD0_MASK           (0x3)          /* Port 0 Speed used for smart flow control */
#define SWITCH_SMART_FC_SPD0_OFFSET         (2)
#define SWITCH_SMART_FC_ORDER2_EN           (1 << 1)       /* Smart flow control order 1 enable */
#define SWITCH_SMART_FC_ORDER1_EN           (1 << 0)       /* Smart flow control order 0 enable */

/* SWITCH_ARL_CTRL0 */
/* =================================================================== */
#define SWITCH_ARL_CTRL0_WT_CMD             (1 << 3)       /* ARL table write command */
#define SWITCH_ARL_CTRL0_LKUP_CMD           (1 << 2)       /* ARL tablle loop up command */
#define SWITCH_ARL_CTRL0_SRCH_AGAIN_CMD      (1 << 1)       /* ARL table: Search for the next valid address command */
#define SWITCH_ARL_CTRL0_SRCH_START_CMD     (1 << 0)       /* ARL table: Search for the valid entries from the Start of address table */

/* SWITCH_ARL_CTRL1 */
/* =================================================================== */
#define SWITCH_ARL_CTRL1_MAC_39_32_MASK     (0xff)         /* MAC address [39:32] of the address entry */
#define SWITCH_ARL_CTRL1_MAC_39_32_OFFSET   (24)
#define SWITCH_ARL_CTRL1_MAC_47_40_MASK     (0xff)         /* MAC address [47:40] of the address entry */
#define SWITCH_ARL_CTRL1_MAC_47_40_OFFSET   (16)
#define SWITCH_ARL_CTRL1_PORTMAP_MASK       (0x7)          /* Port map of the address entry. bit from high to low->port2 to port0 */
#define SWITCH_ARL_CTRL1_PORTMAP_OFFSET     (11)           /* bit 13: port 2, bit 12: port 1, bit 11: port 0 */
#define SWITCH_ARL_CTRL1_AGE_FLD_MASK       (0x7)          /* Age field of the address entry */
#define SWITCH_ARL_CTRL1_AGE_FLD_OFFSET     (8)
#define SWITCH_ARL_CTRL1_VLAN_GID_MASK      (0x7)          /* VLAN group ID of the address entry */
#define SWITCH_ARL_CTRL1_VLAN_GID_OFFSET    (5)
#define SWITCH_ARL_CTRL1_VLANMAC            (1 << 4)       /* VLAN_MAC bit of the address entry */
#define SWITCH_ARL_CTRL1_FILTER             (1 << 3)       /* Filter bit of the address entry */
#define SWITCH_ARL_CTRL1_LKUP_SRCH_MATCH    (1 << 2)       /* Match loop up or Search a valid entry */
#define SWITCH_ARL_CTRL1_TABLE_END          (1 << 1)       /* Search to the end of the address table */
#define SWITCH_ARL_CTRL1_CMD_CPLT           (1 << 0)       /* The ARL table access command has completed */

/* SWITCH_ARL_CTRL2 */
/* =================================================================== */
#define SWITCH_ARL_CTRL2_MAC_7_0_MASK       (0xff)         /* MAC address [7:0] of the address entry */
#define SWITCH_ARL_CTRL2_MAC_7_0_OFFSET     (24)
#define SWITCH_ARL_CTRL2_MAC_15_8_MASK      (0xff)         /* MAC address [15:8] of the address entry */
#define SWITCH_ARL_CTRL2_MAC_15_8_OFFSET    (16)
#define SWITCH_ARL_CTRL2_MAC_23_16_MASK     (0xff)         /* MAC address [23:16] of the address entry */
#define SWITCH_ARL_CTRL2_MAC_23_16_OFFSET   (8)
#define SWITCH_ARL_CTRL2_MAC_31_24_MASK     (0xff)         /* MAC address [31:24] of the address entry */
#define SWITCH_ARL_CTRL2_MAC_31_24_OFFSET   (0)

/* SWITCH_PORT_VID */
/* =================================================================== */
#define SWITCH_PORT_VID_MASK                (0x7)          /* Mask of Port VID */
#define SWITCH_PORT_VID_P2_OFFSET           (8)
#define SWITCH_PORT_VID_P1_OFFSET           (4)
#define SWITCH_PORT_VID_P0_OFFSET           (0)
#define SWITCH_PORT_VID_OFFSET(port)		((port) * 4)

/* SWITCH_VLAN_GID GID0~GID7 */
/* =================================================================== */
#define SWITCH_VLAN_GID_MASK                (0xfff)        /* Mask of VLAN Group ID */
#define SWITCH_VLAN_GID01_1_OFFSET          (12)           /* Physical VLAN ID of GID 1 */
#define SWITCH_VLAN_GID01_0_OFFSET          (0)            /* Physical VLAN ID of GID 0 */
#define SWITCH_VLAN_GID23_3_OFFSET          (12)           /* Physical VLAN ID of GID 3 */
#define SWITCH_VLAN_GID23_2_OFFSET          (0)            /* Physical VLAN ID of GID 2 */
#define SWITCH_VLAN_GID45_5_OFFSET          (12)           /* Physical VLAN ID of GID 5 */
#define SWITCH_VLAN_GID45_4_OFFSET          (0)            /* Physical VLAN ID of GID 4 */
#define SWITCH_VLAN_GID67_7_OFFSET          (12)           /* Physical VLAN ID of GID 7 */
#define SWITCH_VLAN_GID67_6_OFFSET          (0)            /* Physical VLAN ID of GID 6 */
#define SWITCH_VLAN_GID_OFFSET(gid)			(((gid) & 0x1)?12:0)

/* SWITCH_VLAN_PM */
/* =================================================================== */
#define SWITCH_VLAN_PM_MASK                 (0x7)          /* Mask of Port map (3 port) */
#define SWITCH_VLAN_PM_GID7_OFFSET          (21)
#define SWITCH_VLAN_PM_GID6_OFFSET          (18)
#define SWITCH_VLAN_PM_GID5_OFFSET          (15)
#define SWITCH_VLAN_PM_GID4_OFFSET          (12)
#define SWITCH_VLAN_PM_GID3_OFFSET          (9)
#define SWITCH_VLAN_PM_GID2_OFFSET          (6)
#define SWITCH_VLAN_PM_GID1_OFFSET          (3)
#define SWITCH_VLAN_PM_GID0_OFFSET          (0)
#define SWITCH_VLAN_PM_OFFSET(gid)			((gid) * 3)

/* SWITCH_VLAN_TAG_PM */
/* =================================================================== */
#define SWITCH_VLAN_TAG_PM_MASK             (0x7)          /* Mask of Port map (3 port) */
#define SWITCH_VLAN_TAG_PM_GID7_OFFSET      (21)
#define SWITCH_VLAN_TAG_PM_GID6_OFFSET      (18)
#define SWITCH_VLAN_TAG_PM_GID5_OFFSET      (15)
#define SWITCH_VLAN_TAG_PM_GID4_OFFSET      (12)
#define SWITCH_VLAN_TAG_PM_GID3_OFFSET      (9)
#define SWITCH_VLAN_TAG_PM_GID2_OFFSET      (6)
#define SWITCH_VLAN_TAG_PM_GID1_OFFSET      (3)
#define SWITCH_VLAN_TAG_PM_GID0_OFFSET      (0)
#define SWITCH_VLAN_TAG_PM_OFFSET(gid)		((gid) * 3)

/* SWITCH_SESSION_ID ID0~ID7 */
/* =================================================================== */
#define SWITCH_SESSION_ID_MASK              (0xffff)       /* Mask of Session ID */
#define SWITCH_SESSION_ID01_1_OFFSET        (16)
#define SWITCH_SESSION_ID01_0_OFFSET        (0)
#define SWITCH_SESSION_ID23_3_OFFSET        (16)
#define SWITCH_SESSION_ID23_2_OFFSET        (0)
#define SWITCH_SESSION_ID45_5_OFFSET        (16)
#define SWITCH_SESSION_ID45_4_OFFSET        (0)
#define SWITCH_SESSION_ID67_7_OFFSET        (16)
#define SWITCH_SESSION_ID67_6_OFFSET        (0)

/* SWITCH_INT_STA0 (interrupt status) */
/* =================================================================== */
#define SWITCH_INT_STA0_P1_INGRESS_DROP     (1 << 31)      /* Drop by Port 1 ingress check */
#define SWITCH_INT_STA0_P1_LOCAL_DROP       (1 << 30)      /* Drop by Port 1 local traffic */
#define SWITCH_INT_STA0_P1_RMC_PAUSE_DROP   (1 << 29)      /* Drop by Reserved multicast packet or pause frame */
#define SWITCH_INT_STA0_P1_NO_DEST_DROP     (1 << 28)      /* Assert when a packet is received but no where to forward */
#define SWITCH_INT_STA0_P1_JAM_DROP         (1 << 27)      /* Drop by back pressure */
#define SWITCH_INT_STA0_P1_RXERR_DROP       (1 << 26)      /* Drop by Rx Packet Error (CRC error/Runt frame/....) */
#define SWITCH_INT_STA0_P1_BCS_DROP         (1 << 25)      /* Drop by Broadcast Storm */
#define SWITCH_INT_STA0_P1_NOLINK_DROP      (1 << 24)      /* Drop by No free links */

#define SWITCH_INT_STA0_P0_INGRESS_DROP     (1 << 23)      /* Drop by Port 0 ingress check */
#define SWITCH_INT_STA0_P0_LOCAL_DROP       (1 << 22)      /* Drop by Port 0 local traffic */
#define SWITCH_INT_STA0_P0_RMC_PAUSE_DROP   (1 << 21)      /* Drop by Reserved multicast packet or pause frame */
#define SWITCH_INT_STA0_P0_NO_DEST_DROP     (1 << 20)      /* Assert when a packet is received but no where to forward */
#define SWITCH_INT_STA0_P0_JAM_DROP         (1 << 19)      /* Drop by back pressure */
#define SWITCH_INT_STA0_P0_RXERR_DROP       (1 << 18)      /* Drop by Rx Packet Error (CRC error/Runt frame/....) */
#define SWITCH_INT_STA0_P0_BCS_DROP         (1 << 17)      /* Drop by Broadcast Storm */
#define SWITCH_INT_STA0_P0_NOLINK_DROP      (1 << 16)      /* Drop by No free links */

#define SWITCH_INT_STA0_P2_UNKNM_VLAN       (1 << 12)      /* Port 2 receive packet with unknown VLAN */
#define SWITCH_INT_STA0_P1_UNKNM_VLAN       (1 << 11)      /* Port 1 receive packet with unknown VLAN */
#define SWITCH_INT_STA0_P0_UNKNM_VLAN       (1 << 10)      /* Port 0 receive packet with unknown VLAN */

#define SWITCH_INT_STA0_P2_INTRUDER         (1 << 9)       /* Port 2 receive intruder packets */
#define SWITCH_INT_STA0_P1_INTRUDER         (1 << 8)       /* Port 1 receive intruder packets */
#define SWITCH_INT_STA0_P0_INTRUDER         (1 << 7)       /* Port 0 receive intruder packets */

#define SWITCH_INT_STA0_PORT_STA_CHG        (1 << 6)       /* Assert when Any port change link state (link up <-> link down) */
#define SWITCH_INT_STA0_BUFFER_FULL         (1 << 5)       /* Assert when all pages of packet buffer are used */
#define SWITCH_INT_STA0_GLOBAL_Q_FULL       (1 << 4)       /* Assert when global threshold reached */
//#define SWITCH_INT_STA0_HNAT_Q_FULL         (1 << 3)       /* Assert when global threshold reached and HNAT queue threshold reached */
#define SWITCH_INT_STA0_P2_Q_FULL           (1 << 2)       /* Assert when global threshold reached and Port 2 queue threshold reached */
#define SWITCH_INT_STA0_P1_Q_FULL           (1 << 1)       /* Assert when global threshold reached and Port 1 queue threshold reached */
#define SWITCH_INT_STA0_P0_Q_FULL           (1 << 0)       /* Assert when global threshold reached and Port 0 queue threshold reached */

/* SWITCH_INT_MASK0 */
/* =================================================================== */
#define SWITCH_INT_MASK0_RXINT_MASK         (0xffff)       /* Interrupt Mask of Interrupt Status Register0 bit 31~16 */
#define SWITCH_INT_MASK0_RXINT_OFFSET       (16)
#define SWITCH_INT_MASK0_INT0_MASK          (0x1fff)       /* Interrupt Mask of Interrupt Status Register0 bit 12~0 */
#define SWITCH_INT_MASK0_INT0_OFFSET        (0)

/* SWITCH_ATPOLL_PHY */
/* =================================================================== */
#define SWITCH_ATPOLL_PHY_MASK              (0x1f)         /* Mask of Auto-polling PHY address */
#define SWITCH_ATPOLL_PHY_P1_OFFSET         (8)
#define SWITCH_ATPOLL_PHY_P0_OFFSET         (0)

/* SWITCH_TEST0 */
/* =================================================================== */
#define SWITCH_TEST0_BISS_START             (1 << 21)      /* Start BISS Sequence (Write 1 to start) */
#define SWITCH_TEST0_BISS_DIS               (1 << 18)      /* Build-In-Self-Skip Disable */
#define SWITCH_TEST0_BISS_BIST_DONE         (1 << 17)      /* Assert when all embedded memory BIST complete */
#define SWITCH_TEST0_MEM_BIST_FAIL			(1 << 16)      /* Memory BIST fail flag */
#define SWITCH_TEST0_AD_BIST_FAIL			(1 << 4)       /* Address table BIST fail */
#define SWITCH_TEST0_LD_BIST_FAIL			(1 << 3)       /* Relay-info-table BIST fail */
#define SWITCH_TEST0_MC_BIST_FAIL			(1 << 2)       /* MC-link-table BIST fail */
#define SWITCH_TEST0_LT_BIST_FAIL			(1 << 1)       /* Make-link-table BIST fail */
#define SWITCH_TEST0_DA_BIST_FAIL			(1 << 0)       /* Packet buffer BIST fail */

/* SWITCH_TEST1 */
/* =================================================================== */
#define SWITCH_TEST1_FREE_PAGE_CNT_MASK     (0xff)         /* Free pages count of packet buffer */
#define SWITCH_TEST1_FREE_PAGE_CNT_OFFSET   (0)

/* SWITCH_IST_CTRL */
/* =================================================================== */
#define SWITCH_IST_CTRL_IST_MASK            (0x7)          /* Treat all rx packets as VLAN tagged packets. It is for IST application */
#define SWITCH_IST_CTRL_IST_OFFSET          (0)

/* SWITCH_RATE_CTRL2 */
/* =================================================================== */
#define SWITCH_RATE_CTRL2_P2TXBW_MASK       (0x7f)         /* Port 2 TX Bandwidth = N * base_rate */
#define SWITCH_RATE_CTRL2_P2TXBW_OFFSET     (8)
#define SWITCH_RATE_CTRL2_P2_BASERATE_MASK  (0x3)          /* Port 2 Base Rate */
#define SWITCH_RATE_CTRL2_P2_BASERATE_OFFSET (0)
#define SWITCH_RATE_CTRL2_P2_BASERATE_64K   (0)
#define SWITCH_RATE_CTRL2_P2_BASERATE_1M    (1)
#define SWITCH_RATE_CTRL2_P2_BASERATE_10M   (2)

/* SWITCH_INT_STA1 */
/* =================================================================== */
#define SWITCH_INT_STA1_P2_INGRESS_DROP     (1 << 7)       /* Drop by Port 2 ingress check */
#define SWITCH_INT_STA1_P2_LOCAL_DROP       (1 << 6)       /* Drop by Port 2 local traffic */
#define SWITCH_INT_STA1_P2_RMC_PAUSE_DROP   (1 << 5)       /* Drop by Reserved multicast packet or pause frame */
#define SWITCH_INT_STA1_P2_NO_DEST_DROP     (1 << 4)       /* Assert when a packet is received but no where to forward */
#define SWITCH_INT_STA1_P2_JAM_DROP         (1 << 3)       /* Drop by back pressure */
#define SWITCH_INT_STA1_P2_RXERR_DROP       (1 << 2)       /* Drop by Rx Packet Error (CRC error/Runt frame/....) */
#define SWITCH_INT_STA1_P2_BCS_DROP         (1 << 1)       /* Drop by Broadcast Storm */
#define SWITCH_INT_STA1_P2_NOLINK_DROP      (1 << 0)       /* Drop by No free links */

/* SWITCH_INT_MASK1 */
/* =================================================================== */
#define SWITCH_INT_MASK1_RXINT_MASK         (0xff)         /* Interrupt Mask of Interrupt Status Register1 bit 7~0 */
#define SWITCH_INT_MASK1_RXINT_OFFSET       (0)


#endif /* _SWITCH_REG_H_ */

