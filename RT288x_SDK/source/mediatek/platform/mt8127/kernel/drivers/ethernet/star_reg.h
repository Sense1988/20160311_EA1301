#ifndef _STAR_REG_H_
#define _STAR_REG_H_



#define IS_MT8580 0 
#define IS_MT8127 1



#if IS_MT8580
#include <mach/mt85xx.h>
#define USE_EXTEND_PHY 0
#define USE_INTERNAL_PHY 1
#endif

#if IS_MT8127
#include <mach/mt_clkmgr.h>
#define USE_EXTEND_PHY 1
#define USE_INTERNAL_PHY 0
#endif

/*
#define ETH_BASE                            (IO_VIRT + 0x33000)
#define PINMUX_BASE                         (IO_VIRT + 0x24000)
#define PDWNC_BASE                          (IO_VIRT + 0x24000)
#define ETHERNET_PDWNC_BASE                 (IO_VIRT + 0x24C00)
*/
#if IS_MT8580
/* We are using ioremap addresses, so the base should be IO_PHYS */
#define ETH_BASE                            (IO_PHYS + 0x33000)
#define PINMUX_BASE                         (IO_PHYS + 0x24000)
#define PDWNC_BASE                          (IO_PHYS + 0x24000)
#define ETHERNET_PDWNC_BASE                 (IO_PHYS + 0x24C00)
#define ETH_CHKSUM_BASE                     (IO_PHYS + 0x3d000)
#define IPLL1_BASE                          (IO_PHYS + 0x5c700)
#define IPLL2_BASE                          (IO_PHYS + 0x90300)
#define BSP_BASE                            (IO_PHYS + 0x0)
#elif IS_MT8127
#define ETH_BASE                            (0xF1280000)
#define PINMUX_BASE							(0xF0005600)
#define PDWNC_BASE                          (0xF0003000)
#define BSP_BASE                            (0xF0000000)

#endif
/* Note: ETH_IRQ for MT8550 and MT8555 is different */
#define ETH_IRQ_MT8550                      (70)
#define ETH_IRQ_MT8555                      (89)
#if IS_MT8580
#define ETH_IRQ                             VECTOR_ENET
#elif IS_MT8127
#define ETH_IRQ                             109 //VECTOR_ENET  //need confirm the  right value
#endif

#define ETH_DEFAULT_PHY_ADDR                (0)
/* Star Giga Ethernet Controller registers */
/* ====================================================================================== */
#define STAR_PHY_CTRL0(base)				(base + 0x0000)		/* PHY control register0 */
#define STAR_PHY_CTRL1(base)				(base + 0x0004)		/* PHY control register1 */
#define STAR_MAC_CFG(base)					(base + 0x0008)		/* MAC Configuration register */
#define STAR_FC_CFG(base)					(base + 0x000c)		/* Flow Control Configuration register */
#define STAR_ARL_CFG(base)					(base + 0x0010)		/* ARL Configuration register */
#define STAR_My_MAC_H(base)					(base + 0x0014)		/* My MAC High Byte */
#define STAR_My_MAC_L(base)					(base + 0x0018)		/* My MAC Low Byte */
#define STAR_HASH_CTRL(base)				(base + 0x001c)		/* Hash Table Control register */
#define STAR_VLAN_CTRL(base)				(base + 0x0020)		/* My VLAN ID Control register */
#define STAR_VLAN_ID_0_1(base)				(base + 0x0024)		/* My VLAN ID 0 - 1 register */
#define STAR_VLAN_ID_2_3(base)				(base + 0x0028)		/* My VLAN ID 2 - 3 register */
#define STAR_DUMMY(base)					(base + 0x002C)		/* Dummy Register */
#define STAR_DMA_CFG(base)					(base + 0x0030)		/* DMA Configuration register */
#define STAR_TX_DMA_CTRL(base)				(base + 0x0034)		/* TX DMA Control register */
#define STAR_RX_DMA_CTRL(base)				(base + 0x0038)		/* RX DMA Control register */
#define STAR_TX_DPTR(base)					(base + 0x003c)		/* TX Descriptor Pointer */
#define STAR_RX_DPTR(base)					(base + 0x0040)		/* RX Descriptor Pointer */
#define STAR_TX_BASE_ADDR(base)				(base + 0x0044)		/* TX Descriptor Base Address */
#define STAR_RX_BASE_ADDR(base)				(base + 0x0048)		/* RX Descriptor Base Address */
#define STAR_INT_STA(base)					(base + 0x0050)		/* Interrupt Status register */
#define STAR_INT_MASK(base)					(base + 0x0054)		/* Interrupt Mask register */
#define STAR_TEST0(base)					(base + 0x0058)		/* Test0(Clock Skew Setting) register */
#define STAR_TEST1(base)					(base + 0x005c)		/* Test1(Queue status) register */
#define STAR_EXTEND_CFG(base)		        (base + 0x0060)		/* Extended Configuration(Send Pause Off frame thre) register */
#define STAR_ETHPHY(base)					(base + 0x006C)		/* EthPhy Register */
#define STAR_AHB_BURSTTYPE(base)			(base + 0x0074)		/* AHB Burst Type Register */


#define TX_RESUME			  ((u32)0x01 << 2)
#define TX_STOP 			  ((u32)0x01 << 1)
#define TX_START			  ((u32)0x01 << 0)


#define EEE_CTRL(base)          	(base + 0x78)
  		#define LPI_MODE_EB 			((u32)0x01 << 0)
	
#define EEE_SLEEP_TIMER(base)  		(base + 0x7c)
#define EEE_WAKEUP_TIMER(base)  	(base + 0x80)
#define ETHPHY_CONFIG1(base)  		(base + 0x84)
#define RW_INTERNAL_PHY_CTRL(base)  (base + 0x84)
  		#define INTERNAL_PHY_ADDRESS_MASK    ((u32)0x3c0)
    	#define INTERNAL_PHY_ADDRESS_POS    (6)
	
#define ETHPHY_CONFIG2(base)  		(base + 0x88)	
#define RW_ARB_MON2(base) 			(base + 0x8C)		
#define ETHPHY_CLOCK_CONTROL(base)  (base + 0x90)	
#define ETHSYS_CONFIG(base)  		(base + 0x94)
  		#define INT_PHY_SEL 			((u32)0x01 << 3)         //0: select external PHY; 1:INternal PHY
		#define SWC_MII_MODE			((u32)0x01 << 2)			//1: is  switch MII mode
		#define EXT_MDC_MODE			((u32)0x01 << 1)			// 1: external mode, MDC/MDIO is from external
		#define MII_PAD_OE				((u32)0x01 << 0)			//0: MII PAD outpue disable; 1:enable
	
#define MAC_MODE_CONFIG(base)  		(base + 0x98)
  		#define BIG_ENDIAN				((u32)0x01 << 0)
	
#define SW_RESET_CONTROL(base)  	(base + 0x9c)
  		#define PHY_RSTN				((u32)0x01 << 4)
  		#define MRST					((u32)0x01 << 3)
  		#define NRST					((u32)0x01 << 2)
  		#define HRST					((u32)0x01 << 1)
  		#define DMA_RESET				((u32)0x01 << 0)
	
#define MAC_CLOCK_CONFIG(base)  	(base + 0xac)
  		#define TXCLK_OUT_INV			((u32)0x01 << 19)
  		#define RXCLK_OUT_INV			((u32)0x01 << 18)
  		#define TXCLK_IN_INV			((u32)0x01 << 17)
  		#define RXCLK_IN_INV			((u32)0x01 << 16)
  		#define MDC_INV					((u32)0x01 << 12)
  		#define MDC_NEG_LAT				((u32)0x01 << 8)
  		#define MDC_DIV					((u32)0xFF << 0)
  		#define MDC_CLK_DIV_10			((u32)0x0A << 0)




	/* MIB Counter register */
#define STAR_MIB_RXOKPKT(base)				(base + 0x0100)		/* RX OK Packet Counter register */
#define STAR_MIB_RXOKBYTE(base)				(base + 0x0104)		/* RX OK Byte Counter register */
#define STAR_MIB_RXRUNT(base)				(base + 0x0108)		/* RX Runt Packet Counter register */
#define STAR_MIB_RXOVERSIZE(base)			(base + 0x010c)		/* RX Over Size Packet Counter register */
#define STAR_MIB_RXNOBUFDROP(base)			(base + 0x0110)		/* RX No Buffer Drop Packet Counter register */
#define STAR_MIB_RXCRCERR(base)				(base + 0x0114)		/* RX CRC Error Packet Counter register */
#define STAR_MIB_RXARLDROP(base)			(base + 0x0118)		/* RX ARL Drop Packet Counter register */
#define STAR_MIB_RXVLANDROP(base)			(base + 0x011c)		/* RX My VLAN ID Mismatch Drop Packet Counter register */
#define STAR_MIB_RXCKSERR(base)				(base + 0x0120)		/* RX Checksum Error Packet Counter register */
#define STAR_MIB_RXPAUSE(base)				(base + 0x0124)		/* RX Pause Frame Packet Counter register */
#define STAR_MIB_TXOKPKT(base)				(base + 0x0128)		/* TX OK Packet Counter register */
#define STAR_MIB_TXOKBYTE(base)				(base + 0x012c)		/* TX OK Byte Counter register */
#define STAR_MIB_TXPAUSECOL(base)			(base + 0x0130)		/* TX Pause Frame Counter register (Full->pause count, half->collision count) */

#if IS_MT8580
/*  PDWNC register base
    Note: pdwnc_base starts from PDWNC_BASE + 0x100 */
#define PDWNC_BASE_CFG_OFFSET               (0x100)
/* Note: uP configuration register at 0x24188 */
#define PDWNC_REG_UP_CONF_OFFSET(base)      (base + 0x88)
#define UP_CONF_ETEN_EN                     (1U << 26)

/* Ethernet PDWNC register */
#define ETHERNET_PDWNC_REG_OFFSET           (ETHERNET_PDWNC_BASE + 0x000)

/* Pinmux register for Ethernet */
#define PINMUX_REG_PAD_PINMUX2(base)        (base + 0x00f8)     /* Pinmux for Ethernet */
#define PINMUX_REG_PAD_PINMUX2_ETH_OFFSET   (5)
#define PINMUX_REG_PAD_PINMUX2_ETH_MASK     (0x007fffe0)//(0x3ffff)
#define PINMUX_REG_PAD_PINMUX2_ETH_VALUE    (0x007ffe60)//(bit7=0 RXER=0, bit8=0 TXER=0)
#endif


/* =============================================================
		Detail definition of Star Giga Ethernet Controller registers
   ============================================================= */
/* STAR_PHY_CTRL0 (0x33000)*/
/* =================================================================== */
#define STAR_PHY_CTRL0_RWDATA_MASK			(0xffff)			/* Mask of Read/Write Data */
#define STAR_PHY_CTRL0_RWDATA_OFFSET		(16)				/* Offset of Read/Write Data */
#define STAR_PHY_CTRL0_RWOK					(1 << 15)			/* R/W command has completed (write 1 clear) */
#define STAR_PHY_CTRL0_RDCMD				(1 << 14)			/* Read command (self clear) */
#define STAR_PHY_CTRL0_WTCMD				(1 << 13)			/* Write command (self clear) */
#define STAR_PHY_CTRL0_PREG_MASK			(0x1f)				/* Mask of PHY Register Address */
#define STAR_PHY_CTRL0_PREG_OFFSET			(8)					/* Offset of PHY Register Address */
#define STAR_PHY_CTRL0_PA_MASK				(0x1f)				/* Mask of PHY Address */
#define STAR_PHY_CTRL0_PA_OFFSET			(0)					/* Offset of PHY Address */

/* STAR_PHY_CTRL1 (0x33004) */
/* =================================================================== */
#define STAR_PHY_CTRL1_APDIS				(1 << 31)			/* Disable PHY auto polling (1:disable) */
#define STAR_PHY_CTRL1_APEN					(0 << 31)			/* Enable PHY auto polling (0:enable) */
#define STAR_PHY_CTRL1_PHYADDR_MASK			(0x1f)				/* Mask of PHY Address used for auto-polling */
#define STAR_PHY_CTRL1_PHYADDR_OFFSET		(24)				/* Offset of PHY Address used for auto-polling */
#define STAR_PHY_CTRL1_RGMII				(1 << 17)			/* RGMII_PHY used */
#define STAR_PHY_CTRL1_REVMII				(1 << 16)			/* Reversed MII Mode Enable, 0:normal 1:reversed MII(phy side) */
#define STAR_PHY_CTRL1_TXCLK_CKEN			(1 << 14)			/* TX clock period checking Enable */
#define STAR_PHY_CTRL1_FORCETXFC			(1 << 13)			/* Force TX Flow Control when MI disable */
#define STAR_PHY_CTRL1_FORCERXFC			(1 << 12)			/* Force RX Flow Control when MI disable */
#define STAR_PHY_CTRL1_FORCEFULL			(1 << 11)			/* Force Full Duplex when MI disable */
#define STAR_PHY_CTRL1_FORCESPD_MASK		(0x3)				/* Mask of Force Speed when MI disable */
#define STAR_PHY_CTRL1_FORCESPD_OFFSET		(9)					/* Offset of Force Speed when MI disable */
#define STAR_PHY_CTRL1_FORCESPD_10M			(0 << STAR_PHY_CTRL1_FORCESPD_OFFSET)
#define STAR_PHY_CTRL1_FORCESPD_100M		(1 << STAR_PHY_CTRL1_FORCESPD_OFFSET)
#define STAR_PHY_CTRL1_FORCESPD_1G			(2 << STAR_PHY_CTRL1_FORCESPD_OFFSET)
#define STAR_PHY_CTRL1_FORCESPD_RESV		(3 << STAR_PHY_CTRL1_FORCESPD_OFFSET)
#define STAR_PHY_CTRL1_ANEN					(1 << 8)			/* Auto-Negotiation Enable */
#define STAR_PHY_CTRL1_MIDIS				(1 << 7)			/* MI auto-polling disable, 0:active 1:disable */
	/* PHY status */
#define STAR_PHY_CTRL1_STA_TXFC				(1 << 6)			/* TX Flow Control status (only for 1000Mbps) */
#define STAR_PHY_CTRL1_STA_RXFC				(1 << 5)			/* RX Flow Control status */
#define STAR_PHY_CTRL1_STA_FULL				(1 << 4)			/* Duplex status, 1:full 0:half */
#define STAR_PHY_CTRL1_STA_SPD_MASK			(0x3)				/* Mask of Speed status */
#define STAR_PHY_CTRL1_STA_DPX_MASK			(0x1)				/* Mask of Duplex status */
#define STAR_PHY_CTRL1_STA_SPD_DPX_MASK	    (0x7)				/* Mask of Speed and Duplex status */
#define STAR_PHY_CTRL1_STA_SPD_OFFSET		(2)					/* Offset of Speed status */
#define STAR_PHY_CTRL1_STA_SPD_10M			(0 << STAR_PHY_CTRL1_STA_SPD_OFFSET)
#define STAR_PHY_CTRL1_STA_SPD_100M			(1 << STAR_PHY_CTRL1_STA_SPD_OFFSET)
#define STAR_PHY_CTRL1_STA_SPD_1G			(2 << STAR_PHY_CTRL1_STA_SPD_OFFSET)
#define STAR_PHY_CTRL1_STA_SPD_RESV			(3 << STAR_PHY_CTRL1_STA_SPD_OFFSET)
#define STAR_PHY_CTRL1_STA_TXCLK			(1 << 1)			/* TX clock status, 0:normal 1:no TXC or clk period too long */
#define STAR_PHY_CTRL1_STA_LINK				(1 << 0)			/* PHY Link status */
#define STAR_PHY_CTRL1_STA_10M_HALF			(0x0)			/* STAR_PHY_CTRL1[4:2]=0x0 */
#define STAR_PHY_CTRL1_STA_100M_HALF		(0x1)			/* STAR_PHY_CTRL1[4:2]=0x1 */
#define STAR_PHY_CTRL1_STA_10M_FULL			(0x4)			/* STAR_PHY_CTRL1[4:2]=0x4 */
#define STAR_PHY_CTRL1_STA_100M_FULL		(0x5)			/* STAR_PHY_CTRL1[4:2]=0x5 */

/* STAR_MAC_CFG (0x33008)*/
/* =================================================================== */
#define STAR_MAC_CFG_NICPD					(1 << 31)			/* NIC Power Down */
#define STAR_MAC_CFG_WOLEN					(1 << 30)			/* Wake on LAN Enable */
#define STAR_MAC_CFG_NICPDRDY				(1 << 29)			/* NIC Power Down Ready */
#define STAR_MAC_CFG_TXCKSEN				(1 << 26)			/* TX IP/TCP/UDP Checksum offload Enable */
#define STAR_MAC_CFG_RXCKSEN				(1 << 25)			/* RX IP/TCP/UDP Checksum offload Enable */
#define STAR_MAC_CFG_ACPTCKSERR				(1 << 24)			/* Accept Checksum Error Packets */
#define STAR_MAC_CFG_ISTEN					(1 << 23)			/* Inter Switch Tag Enable */
#define STAR_MAC_CFG_VLANSTRIP				(1 << 22)			/* VLAN Tag Stripping */
#define STAR_MAC_CFG_ACPTCRCERR				(1 << 21)			/* Accept CRC Error Packets */
#define STAR_MAC_CFG_CRCSTRIP				(1 << 20)			/* CRC Stripping */
#define STAR_MAC_CFG_TXAUTOPAD				(1 << 19)			/* TX Auto Pad */
#define STAR_MAC_CFG_ACPTLONGPKT			(1 << 18)			/* Accept Oversized Packets */
#define STAR_MAC_CFG_MAXLEN_MASK			(0x3)				/* Mask of Maximum Packet Length */
#define STAR_MAC_CFG_MAXLEN_OFFSET			(16)				/* Offset of Maximum Packet Length */
#define STAR_MAC_CFG_MAXLEN_1518			(0 << STAR_MAC_CFG_MAXLEN_OFFSET)
#define STAR_MAC_CFG_MAXLEN_1522			(1 << STAR_MAC_CFG_MAXLEN_OFFSET)
#define STAR_MAC_CFG_MAXLEN_1536			(2 << STAR_MAC_CFG_MAXLEN_OFFSET)
#define STAR_MAC_CFG_MAXLEN_RESV			(3 << STAR_MAC_CFG_MAXLEN_OFFSET)
#define STAR_MAC_CFG_IPG_MASK				(0x1f)				/* Mask of Inter Packet Gap */
#define STAR_MAC_CFG_IPG_OFFSET				(10)				/* Offset of Inter Packet Gap */
#define STAR_MAC_CFG_NSKP16COL				(1 << 9)			/* Dont's skip 16 consecutive collisions packet */
#define STAR_MAC_CFG_FASTBACKOFF			(1 << 8)			/* Collision Fast Back-off */
#define STAR_MAC_CFG_TXVLAN_ATPARSE			(1 << 0)			/* TX VLAN Auto Parse. 1: Hardware decide VLAN packet */

/* STAR_FC_CFG (0x3300c)*/
/* =================================================================== */
#define STAR_FC_CFG_SENDPAUSETH_MASK		(0xfff)				/* Mask of Send Pause Threshold */
#define STAR_FC_CFG_SENDPAUSETH_OFFSET		(16)				/* Offset of Send Pause Threshold */
#define STAR_FC_CFG_COLCNT_CLR_MODE         (1 << 9)            /* Collisin Count Clear Mode */
#define STAR_FC_CFG_UCPAUSEDIS				(1 << 8)			/* Disable unicast Pause */
#define STAR_FC_CFG_BPEN					(1 << 7)			/* Half Duplex Back Pressure Enable */
#define STAR_FC_CFG_CRS_BP_MODE             (1 << 6)            /* Half Duplex Back Pressure force carrier */
#define STAR_FC_CFG_MAXBPCOLEN				(1 << 5)			/* Pass-one-every-N backpressure collision policy Enable */
#define STAR_FC_CFG_MAXBPCOLCNT_MASK		(0x1f)				/* Mask of Max backpressure collision count */
#define STAR_FC_CFG_MAXBPCOLCNT_OFFSET		(0)					/* Offset of Max backpressure collision count */

    /* default value for SEND_PAUSE_TH */
#define STAR_FC_CFG_SEND_PAUSE_TH_DEF       ((STAR_FC_CFG_SEND_PAUSE_TH_2K & \
                                              STAR_FC_CFG_SENDPAUSETH_MASK) \
                                              << STAR_FC_CFG_SENDPAUSETH_OFFSET)
#define STAR_FC_CFG_SEND_PAUSE_TH_2K        (0x800)


/* STAR_ARL_CFG (0x33010) */
/* =================================================================== */
#define STAR_ARL_CFG_FILTER_PRI_TAG			(1 << 6)			/* Filter Priority-tagged packet */
#define STAR_ARL_CFG_FILTER_VLAN_UNTAG		(1 << 5)			/* Filter VLAN-untagged packet */
#define STAR_ARL_CFG_MISCMODE				(1 << 4)			/* Miscellaneous(promiscuous) mode */
#define STAR_ARL_CFG_MYMACONLY				(1 << 3)			/* 1:Only My_MAC/BC packets are received, 0:My_MAC/BC/Hash_Table_hit packets are received */
#define STAR_ARL_CFG_CPULEARNDIS			(1 << 2)			/* From CPU SA Learning Disable */
#define STAR_ARL_CFG_RESVMCFILTER			(1 << 1)			/* Reserved Multicast Address Filtering, 0:forward to CPU 1:drop */
#define STAR_ARL_CFG_HASHALG_CRCDA			(1 << 0)			/* MAC Address Hashing algorithm, 0:DA as hash 1:CRC of DA as hash */

/* STAR_HASH_CTRL (0x3301c) */
/* =================================================================== */
#define STAR_HASH_CTRL_HASHEN				(1 << 31)			/* Hash Table Enable */
#define STAR_HASH_CTRL_HTBISTDONE			(1 << 17)			/* Hash Table BIST(Build In Self Test) Done */
#define STAR_HASH_CTRL_HTBISTOK				(1 << 16)			/* Hash Table BIST(Build In Self Test) OK */
#define STAR_HASH_CTRL_START				(1 << 14)			/* Hash Access Command Start */
#define STAR_HASH_CTRL_ACCESSWT				(1 << 13)			/* Hash Access Write Command, 0:read 1:write */
#define STAR_HASH_CTRL_ACCESSRD				(0 << 13)			/* Hash Access Read Command, 0:read 1:write */
#define STAR_HASH_CTRL_HBITDATA				(1 << 12)			/* Hash Bit Data (Read or Write) */
#define STAR_HASH_CTRL_HBITADDR_MASK		(0x1ff)				/* Mask of Hash Bit Address */
#define STAR_HASH_CTRL_HBITADDR_OFFSET		(0)					/* Offset of Hash Bit Address */

/* STAR_MY_VID (0x33020) */
/* =================================================================== */
#define STAR_MY_VID_EN_ALL				    (0xf)			/* Enable MY VID0~VID3 filter */


/* STARETH_DUMMY (0x3302c) */
/* =================================================================== */
#define STAR_DUMMY_FPGA_MODE				(1 << 31)			/* FPGA mode or ASIC mode */
#define STAR_DUMMY_TXRXRDY					(1 << 1)			/* Asserted when tx/rx path is IDLE and rxclk available */
#define STAR_DUMMY_MDCMDIODONE				(1 << 0)			/* MDC/MDIO done */

/* STAR_DMA_CFG (0x0x33030) */
/* =================================================================== */
#define STAR_DMA_CFG_RX2BOFSTDIS			(1 << 16)			/* RX 2 Bytes offset disable */
#define STAR_DMA_CFG_TXPOLLPERIOD_MASK		(0x3)				/* Mask of TX DMA Auto-Poll Period */
#define STAR_DMA_CFG_TXPOLLPERIOD_OFFSET	(6)					/* Offset of TX DMA Auto-Poll Period */
#define STAR_DMA_CFG_TXPOLLPERIOD_1US		(0 << STAR_DMA_CFG_TXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_TXPOLLPERIOD_10US		(1 << STAR_DMA_CFG_TXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_TXPOLLPERIOD_100US		(2 << STAR_DMA_CFG_TXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_TXPOLLPERIOD_1000US	(3 << STAR_DMA_CFG_TXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_TXPOLLEN				(1 << 5)			/* TX DMA Auto-Poll C-Bit Enable */
#define STAR_DMA_CFG_TXSUSPEND				(1 << 4)			/* TX DMA Suspend */
#define STAR_DMA_CFG_RXPOLLPERIOD_MASK		(0x3)				/* Mask of RX DMA Auto-Poll Period */
#define STAR_DMA_CFG_RXPOLLPERIOD_OFFSET	(2)					/* Offset of RX DMA Auto-Poll Period */
#define STAR_DMA_CFG_RXPOLLPERIOD_1US		(0 << STAR_DMA_CFG_RXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_RXPOLLPERIOD_10US		(1 << STAR_DMA_CFG_RXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_RXPOLLPERIOD_100US		(2 << STAR_DMA_CFG_RXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_RXPOLLPERIOD_1000US	(3 << STAR_DMA_CFG_RXPOLLPERIOD_OFFSET)
#define STAR_DMA_CFG_RXPOLLEN				(1 << 1)			/* RX DMA Auto-Poll C-Bit Enable */
#define STAR_DMA_CFG_RXSUSPEND				(1 << 0)			/* RX DMA Suspend */
	
/* STAR_INT_STA (0x33050) */
/* =================================================================== */
#define STAR_INT_STA_RX_PCODE			    (1 << 10)			/* RX 802.3az LPI Code Interrupt*/
#define STAR_INT_STA_TX_SKIP			    (1 << 9)			/* To NIC Tx Skip Interrupt (retry>15) */
#define STAR_INT_STA_TXC					(1 << 8)			/* To NIC DMA Transmit Complete Interrupt */
#define STAR_INT_STA_TXQE					(1 << 7)			/* To NIC DMA Queue Empty Interrupt */
#define STAR_INT_STA_RXC					(1 << 6)			/* From NIC DMA Receive Complete Interrupt */
#define STAR_INT_STA_RXQF					(1 << 5)			/* From NIC DMA Receive Queue Full Interrupt */
#define STAR_INT_STA_MAGICPKT				(1 << 4)			/* Magic packet received */
#define STAR_INT_STA_MIBCNTHALF				(1 << 3)			/* Assert when any one MIB counter reach 0x80000000(half) */
#define STAR_INT_STA_PORTCHANGE				(1 << 2)			/* Assert MAC Port Change Link State (link up <-> link down) */
#define STAR_INT_STA_RXFIFOFULL				(1 << 1)			/* Assert when RX Buffer full */


/* STAR_EXTEND_CFG (0x33060) */
/* =================================================================== */
#define STAR_EXTEND_CFG_SDPAUSEOFFTH_MASK	    (0xfff)		    /* Mask of Send Pause Off Frame Threshold */
#define STAR_EXTEND_CFG_SDPAUSEOFFTH_OFFSET	    (16)	        /* Offset of Send Pause Off Frame Threshold */

    /* default value for SEND_PAUSE_RLS */
#define STAR_EXTEND_CFG_SEND_PAUSE_RLS_DEF      ((STAR_EXTEND_CFG_SEND_PAUSE_RLS_1K& \
                                                STAR_EXTEND_CFG_SDPAUSEOFFTH_MASK) \
                                                << STAR_EXTEND_CFG_SDPAUSEOFFTH_OFFSET)
#define STAR_EXTEND_CFG_SEND_PAUSE_RLS_1K       (0x400)

/* STAR_TEST1 (0x3305c) */
/* =================================================================== */
#define STAR_TEST1_RST_HASH_BIST			(1 << 31)			/* Restart Hash Table Bist */

/* STAR_ETHPHY (0x3306c) */
/* =================================================================== */
#define STAR_ETHPHY_FRC_SMI_EN				(1 << 2)			/* Force SMI Enable */


/* STAR_AHB_BURSTTYPE (0x33074) */
/* =================================================================== */
  #define TX_BURST					((u32)0x07 << 4) 
  	  #define TX_BURST_4			((u32)0x03 << 4) 
  	  #define TX_BURST_8			((u32)0x05 << 4) 
  	  #define TX_BURST_16			((u32)0x07 << 4) 
  #define RX_BURST					((u32)0x01 << 0)  
  	  #define RX_BURST_4			((u32)0x03 << 0) 
  	  #define RX_BURST_8			((u32)0x05 << 0) 
  	  #define RX_BURST_16			((u32)0x07 << 0) 


/* Internal PHY registers */
#define EXT_PHY_CTRL_5_REG              (0x1b)
    #define BLD_PS_CORR_DIS             (0x1 << 15)


enum MacInitReg
{
  MAC_CFG_INIT        	= (u32)(STAR_MAC_CFG_RXCKSEN | 
                                   STAR_MAC_CFG_ACPTCKSERR | 
                                   STAR_MAC_CFG_CRCSTRIP| 
                                   STAR_MAC_CFG_MAXLEN_1522 | 
                                   (STAR_MAC_CFG_IPG_MASK<<STAR_MAC_CFG_IPG_OFFSET)),   


		  MAC_CLK_INIT				= (u32)( MDC_CLK_DIV_10),
		  MAC_EXT_INIT				= (u32)( MRST | NRST | HRST | DMA_RESET),
		  MAC_EXT_INIT_RMII 		= (u32)(MRST | NRST | HRST ),
		  PDWNC_MAC_EXT_INIT		= (u32)(MRST | NRST | HRST | DMA_RESET),
		  PDWNC_MAC_EXT_INIT_RMII	= (u32)(MRST | NRST | HRST),
		  PDWNC_MAC_EXT_CFG 		= (u32)(MRST | NRST | HRST ),
		  PDWNC_MAC_EXT_CFG_RMII	= (u32)(MRST | NRST | HRST),
		  MAC_FILTER_INIT       	= (u32)(0),                               
		  MAC_FLOWCTRL_INIT  		= (u32)(STAR_FC_CFG_SEND_PAUSE_TH_DEF /* 2k */ | 
                                   				STAR_FC_CFG_UCPAUSEDIS | 
                                   				STAR_FC_CFG_BPEN ),                                      	

  		  PHY_CTRL_INIT				= (u32)(STAR_PHY_CTRL1_FORCETXFC | 
                                   				STAR_PHY_CTRL1_FORCERXFC | 
                                   				STAR_PHY_CTRL1_FORCEFULL | 
                                   				STAR_PHY_CTRL1_FORCESPD_100M | 
                                   				STAR_PHY_CTRL1_ANEN),

  		  DMA_BUSMODE_INIT      	= (u32)(TX_BURST_16 | RX_BURST_16),      // Bus Mode
  		  DMA_OPMODE_INIT      		= (u32)(STAR_DMA_CFG_RX2BOFSTDIS | 
												STAR_DMA_CFG_TXSUSPEND | 
                                   				STAR_DMA_CFG_RXSUSPEND),       // Operation Mode  

  		  DMA_RX_INT_MASK       	= (u32)(STAR_INT_STA_RXC | 
                                   				STAR_INT_STA_RXFIFOFULL | 
                                   				STAR_INT_STA_RXQF),
                                     
  		 DMA_TX_INT_MASK       		= (u32)(STAR_INT_STA_TXC 
),

  DMA_INT_ENABLE        = (u32)(STAR_INT_STA_TXC | 
                                   STAR_INT_STA_RXC | 
                                   STAR_INT_STA_RXQF | 
                                   STAR_INT_STA_RXFIFOFULL 
									),         

  DMA_INT_MASK          = (u32)(STAR_INT_STA_TXC | 
                                   STAR_INT_STA_RXC | 
                                   STAR_INT_STA_RXQF | 
                                   STAR_INT_STA_RXFIFOFULL 
								   ),

  DMA_INT_ENABLE_ALL   	= (u32)(STAR_INT_STA_TX_SKIP | 
                                   STAR_INT_STA_TXC | 
                                   STAR_INT_STA_TXQE | 
                                   STAR_INT_STA_RXC | 
                                   STAR_INT_STA_RXQF |
                                   STAR_INT_STA_MAGICPKT |
                                   STAR_INT_STA_MIBCNTHALF |
                                   STAR_INT_STA_PORTCHANGE |
                                   STAR_INT_STA_RXFIFOFULL |
	  							   STAR_INT_STA_RX_PCODE

                                   ),

  DMA_INT_CLEAR_ALL     = (u32)(STAR_INT_STA_TX_SKIP | 
                                   STAR_INT_STA_TXC | 
                                   STAR_INT_STA_TXQE | 
                                   STAR_INT_STA_RXC | 
                                   STAR_INT_STA_RXQF |
                                   STAR_INT_STA_MAGICPKT |
                                   STAR_INT_STA_MIBCNTHALF |
                                   STAR_INT_STA_PORTCHANGE |
                                   STAR_INT_STA_RXFIFOFULL |
                                   STAR_INT_STA_RX_PCODE
                                   ),
};


#endif /* _STAR_REG_H_ */

