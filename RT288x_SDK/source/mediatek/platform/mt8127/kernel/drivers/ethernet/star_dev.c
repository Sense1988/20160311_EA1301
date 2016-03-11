/**
 * @brief Star Ethernet Device dependent functions
 * @author mtk02196
 *
 */

#include "star.h"
//#include "star_verify.h"

#ifndef __KERNEL_GPIO_PINMUX__
#define __KERNEL_GPIO_PINMUX__
#endif
#if IS_MT8580
#include <mach/mt85xx_gpio_pinmux.h>
#endif
extern int bsp_pinset(unsigned pinmux_sel,unsigned function);
extern int gpio_configure(unsigned gpio,int dir,int value);

/* =========================================================
   Common functions
   ========================================================= */
int StarDevInit(StarDev *dev, u32 base)
{
	memset(dev, 0, sizeof(StarDev));

	dev->base = base;

	return 0;
}

void StarResetEthernet(StarDev *dev)
{
	u32 base = dev->base;

	/* Power down NIC  to prevent PHY crash */
	{
#define MAX_WAIT_TIME		5000000
		unsigned int waitLoop;
	
		StarSetBit(STAR_MAC_CFG(base), STAR_MAC_CFG_NICPD);
		star_mb();
        /* Wait for MDC/MDIO Done bit is asserted */
		for (waitLoop = 0; waitLoop < MAX_WAIT_TIME; waitLoop ++)
        {
            if (StarGetReg(STAR_DUMMY(base)) & STAR_DUMMY_MDCMDIODONE)
                break;
		}
		if (waitLoop >= MAX_WAIT_TIME)
		{
			STAR_MSG(STAR_ERR, "Star Ethernet Reset Procedure - NIC Power Down doesn't ready.\n\r");
		}
        /* Wait for the NIC_PD_READY to complete */
		star_mb();
		for (waitLoop = 0; waitLoop < MAX_WAIT_TIME; waitLoop ++)
        {
            if (StarGetReg(STAR_MAC_CFG(base)) & STAR_MAC_CFG_NICPDRDY)
                break;
        }
		if (waitLoop >= MAX_WAIT_TIME)
		{
			STAR_MSG(STAR_ERR, "Star Ethernet Reset Procedure - NIC Power Down doesn't ready.\n\r");
		}
#undef MAX_WAIT_TIME
	}

}


int StarPhyMode(StarDev *dev)
{
#ifdef INTERNAL_PHY
    return INT_PHY_MODE;
#else
    return EXT_MII_MODE;
#endif
}


/* =========================================================
   MDC MDIO functions
   ========================================================= */
u16 StarMdcMdioRead(StarDev *dev, u32 phyAddr, u32 phyReg)
{
	u32 base = dev->base;
	u32 phyCtl;
	u16 data;
	u32 delay=0x400000;

	StarSetReg(STAR_PHY_CTRL0(base), STAR_PHY_CTRL0_RWOK); /* Clear previous read/write OK status (write 1 clear) */
	phyCtl = (  ((phyAddr & STAR_PHY_CTRL0_PA_MASK) << STAR_PHY_CTRL0_PA_OFFSET) |
	            ((phyReg & STAR_PHY_CTRL0_PREG_MASK) << STAR_PHY_CTRL0_PREG_OFFSET) |
	            STAR_PHY_CTRL0_RDCMD /* Read Command */
	          );
	star_mb();
	StarSetReg(STAR_PHY_CTRL0(base), phyCtl);
	star_mb();
	do {} while ((!StarIsSetBit(STAR_PHY_CTRL0(base), STAR_PHY_CTRL0_RWOK)) && (delay--)); /* Wait for read/write ok bit is asserted */

	star_mb();
	data = (u16)StarGetBitMask(STAR_PHY_CTRL0(base), STAR_PHY_CTRL0_RWDATA_MASK, STAR_PHY_CTRL0_RWDATA_OFFSET);

	return data;
}

void StarMdcMdioWrite(StarDev *dev, u32 phyAddr, u32 phyReg, u16 value)
{
	u32 base = dev->base;
	u32 phyCtl;
	u32 delay=0x400000;

	StarSetReg(STAR_PHY_CTRL0(base), STAR_PHY_CTRL0_RWOK); /* Clear previous read/write OK status (write 1 clear) */
	phyCtl = ((value & STAR_PHY_CTRL0_RWDATA_MASK) << STAR_PHY_CTRL0_RWDATA_OFFSET) |
	         ((phyAddr & STAR_PHY_CTRL0_PA_MASK) << STAR_PHY_CTRL0_PA_OFFSET) |
	         ((phyReg & STAR_PHY_CTRL0_PREG_MASK) << STAR_PHY_CTRL0_PREG_OFFSET) |
	         STAR_PHY_CTRL0_WTCMD; /* Write Command */
	star_mb();         
	StarSetReg(STAR_PHY_CTRL0(base), phyCtl);
	star_mb();
	do {} while ((!StarIsSetBit(STAR_PHY_CTRL0(base), STAR_PHY_CTRL0_RWOK)) && (delay--) ); /* Wait for read/write ok bit is asserted */

}


/* =========================================================
   DMA related functions
   ========================================================= */
static void DescTxInit(TxDesc *txDesc, u32 isEOR)
{
	txDesc->buffer = 0;
	txDesc->ctrlLen = TX_COWN | (isEOR ? TX_EOR : 0);
	txDesc->vtag = 0;
	txDesc->reserve = 0;
}

static void DescRxInit(RxDesc *rxDesc, u32 isEOR)
{
	rxDesc->buffer = 0;
	rxDesc->ctrlLen = RX_COWN | (isEOR ? RX_EOR : 0);
	rxDesc->vtag = 0;
	rxDesc->reserve = 0;
}

static void DescTxTake(TxDesc *txDesc)     /* Take ownership */ 
{
	if (DescTxDma(txDesc))
	{
		txDesc->ctrlLen |= TX_COWN;       /* Set CPU own */
	}
}

static void DescRxTake(RxDesc *rxDesc)     /* Take ownership */ 
{
	if (DescRxDma(rxDesc))
	{
		rxDesc->ctrlLen |= RX_COWN;       /* Set CPU own */
	}
}

int StarDmaInit(StarDev *dev, u32 desc_virAd, dma_addr_t desc_dmaAd)
{
	int i;
	u32 base = dev->base;

	dev->txRingSize = TX_DESC_NUM;
	dev->rxRingSize = RX_DESC_NUM;

	dev->txdesc = (TxDesc *)desc_virAd;
	dev->rxdesc = (RxDesc *)dev->txdesc + dev->txRingSize;

	for (i=0; i<dev->txRingSize; i++) { DescTxInit(dev->txdesc + i, i==dev->txRingSize-1); }
	for (i=0; i<dev->rxRingSize; i++) { DescRxInit(dev->rxdesc + i, i==dev->rxRingSize-1); }

	dev->txHead = 0;
	dev->txTail = 0;
	dev->rxHead = 0;
	dev->rxTail = 0;
	dev->txNum = 0;
	dev->rxNum = 0;

	/* TODO: Reset Procedure */
	/* OOXX.... */

 
        
	/* Set Tx/Rx descriptor address */
	StarSetReg(STAR_TX_BASE_ADDR(base), (u32)desc_dmaAd);
	StarSetReg(STAR_TX_DPTR(base), (u32)desc_dmaAd);
	StarSetReg(STAR_RX_BASE_ADDR(base), (u32)desc_dmaAd + sizeof(TxDesc) * dev->txRingSize);
	StarSetReg(STAR_RX_DPTR(base), (u32)desc_dmaAd + sizeof(TxDesc) * dev->txRingSize);
	/* Enable Rx 2 byte offset (FIXME) */
	//StarDmaRx2BtOffsetEnable(dev);
	/* Init DMA_CFG, Note: RX_OFFSET_2B_DIS is set to 0 */

	//StarSetReg(STAR_DMA_CFG(base),
	//			STAR_DMA_CFG_RX2BOFSTDIS 
	//		    );
     
	StarIntrDisable(dev);

	return 0;
}
#if 0
extern volatile PMAC_TX_DESC_CTRL_T txCtrl;
extern int  _TxDesCtrlBitOnfgVTG;
extern int   _TxDesCtrlfgVTG;

extern int  _TxDesCtrlBitOnfgTCO;
extern int   _TxDesCtrlfgTCO;

extern int  _TxDesCtrlBitOnfgUCO;
extern int   _TxDesCtrlfgUCO;

extern int  _TxDesCtrlBitOnfgICO;
extern int   _TxDesCtrlfgICO;

extern int  _TxDesCtrlBitOnfgINSV;
extern int   _TxDesCtrlfgINSV;


extern int  _TxDesCtrlBitOnfgTxIntOn;
extern int   _TxDesCtrlfgTxIntOn;


extern int  _TxDesCtrlBitOnfgLastSeg;
extern int   _TxDesCtrlfgLastSeg;
 
extern int  _TxDesCtrlBitOnfgFirstSeg;
extern int   _TxDesCtrlfgFirstSeg;
 
extern int  _TxDesCtrlBitOnfgEndOfRing;
extern int   _TxDesCtrlfgEndOfRing;
#endif
int StarDmaTxSet(StarDev *dev, u32 buffer, u32 length, u32 extBuf)
{
	int descIdx = dev->txHead;
	TxDesc *txDesc = dev->txdesc + descIdx;

	/* Error checking */
	if (dev->txNum == dev->txRingSize) goto err;
	if (!DescTxEmpty(txDesc)) goto err;          /* descriptor is not empty - cannot set */

	txDesc->buffer = buffer;
	txDesc->ctrlLen |= ((((length < 60)?60:length) & TX_LEN_MASK) << TX_LEN_OFFSET) | TX_FS | TX_LS | TX_INT/*Tx Interrupt Enable*/;
	txDesc->reserve = extBuf;
	wmb();
	txDesc->ctrlLen &= ~TX_COWN;     /* Set HW own */

	dev->txNum++;
	dev->txHead = DescTxLast(txDesc) ? 0 : descIdx + 1;
#if 0	
	txCtrl =(PMAC_TX_DESC_CTRL_T)(&txDesc->ctrlLen);
	if(_TxDesCtrlBitOnfgVTG) 
		txCtrl->Bits.fgVTG = _TxDesCtrlfgVTG;

	if(_TxDesCtrlBitOnfgTCO) 
		txCtrl->Bits.fgTCO = _TxDesCtrlfgTCO;

	if(_TxDesCtrlBitOnfgUCO) 
		txCtrl->Bits.fgUCO = _TxDesCtrlfgUCO;

	if(_TxDesCtrlBitOnfgICO) 
		txCtrl->Bits.fgICO = _TxDesCtrlfgICO;
	
	if(_TxDesCtrlBitOnfgINSV) 
		txCtrl->Bits.fgINSV = _TxDesCtrlfgINSV;


	if(_TxDesCtrlBitOnfgTxIntOn) 
		txCtrl->Bits.fgTxIntOn = _TxDesCtrlfgTxIntOn;


	 if(_TxDesCtrlBitOnfgLastSeg) 
		 txCtrl->Bits.fgLastSeg = _TxDesCtrlfgLastSeg;
	 
	 if(_TxDesCtrlBitOnfgFirstSeg) 
		 txCtrl->Bits.fgFirstSeg = _TxDesCtrlfgFirstSeg;
	 
	 if(_TxDesCtrlBitOnfgEndOfRing) 
		 txCtrl->Bits.fgEndOfRing = _TxDesCtrlfgEndOfRing;

	if (_TxDesCtrlBitOnfgVTG || _TxDesCtrlBitOnfgTCO ||
		_TxDesCtrlBitOnfgUCO || _TxDesCtrlBitOnfgICO ||
		_TxDesCtrlBitOnfgINSV || _TxDesCtrlBitOnfgTxIntOn ||
		_TxDesCtrlBitOnfgLastSeg || _TxDesCtrlBitOnfgFirstSeg ||
		_TxDesCtrlBitOnfgEndOfRing)
	{
		STAR_MSG(STAR_DBG,"Tx Desc: 0x%08X\n", txCtrl->u4Val);
		//HalFlushInvalidateDCache(); //Before Read Tx Descriptor //instead of map/unmap
	}
#endif	
	return descIdx;
err:
	return -1;
}

int StarDmaTxGet(StarDev *dev, u32 *buffer, u32 *ctrlLen, u32 *extBuf)
{
	int descIdx = dev->txTail;
	TxDesc *txDesc = dev->txdesc + descIdx;

	/* Error checking */
	if (dev->txNum == 0) goto err;             /* No buffer can be got */
	if (DescTxDma(txDesc)) goto err;          /* descriptor is owned by DMA - cannot get */
	if (DescTxEmpty(txDesc)) goto err;        /* descriptor is empty - cannot get */

	if (buffer != 0) *buffer = txDesc->buffer;
	if (ctrlLen != 0) *ctrlLen = txDesc->ctrlLen;
	if (extBuf != 0) *extBuf = txDesc->reserve;
	rmb();

	DescTxInit(txDesc, DescTxLast(txDesc));
	dev->txNum--;
	dev->txTail = DescTxLast(txDesc) ? 0 : descIdx + 1;

	return descIdx;
err:
	return -1;
}

int StarDmaRxSet(StarDev *dev, u32 buffer, u32 length, u32 extBuf)
{
	int descIdx = dev->rxHead;
	RxDesc *rxDesc = dev->rxdesc + descIdx;

	/* Error checking */
	if (dev->rxNum == dev->rxRingSize) goto err;
	if (!DescRxEmpty(rxDesc)) goto err;		/* descriptor is not empty - cannot set */

	rxDesc->buffer = buffer;
	rxDesc->ctrlLen |= ((length & RX_LEN_MASK) << RX_LEN_OFFSET);
	rxDesc->reserve = extBuf;
	wmb();
	rxDesc->ctrlLen &= ~RX_COWN;       /* Set HW own */

	dev->rxNum++;
	dev->rxHead = DescRxLast(rxDesc) ? 0 : descIdx + 1;

	return descIdx;
err:
	return -1;
}

int StarDmaRxGet(StarDev *dev, u32 *buffer, u32 *ctrlLen, u32 *extBuf)
{
	int descIdx = dev->rxTail;
	RxDesc *rxDesc = dev->rxdesc + descIdx;

	/* Error checking */
	if (dev->rxNum == 0) goto err;             /* No buffer can be got */
	if (DescRxDma(rxDesc)) goto err;          /* descriptor is owned by DMA - cannot get */
	if (DescRxEmpty(rxDesc)) goto err;        /* descriptor is empty - cannot get */

	if (buffer != 0) *buffer = rxDesc->buffer;
	if (ctrlLen != 0) *ctrlLen = rxDesc->ctrlLen;
	if (extBuf != 0) *extBuf = rxDesc->reserve;
	rmb();

	DescRxInit(rxDesc, DescRxLast(rxDesc));
	dev->rxNum--;
	dev->rxTail = DescRxLast(rxDesc) ? 0 : descIdx + 1;

	return descIdx;
err:
	return -1;
}

void StarDmaTxStop(StarDev *dev)
{
	int i;	
	StarDmaTxDisable(dev);
	star_mb();
	for (i=0; i<dev->txRingSize; i++) DescTxTake(dev->txdesc + i);
}

void StarDmaRxStop(StarDev *dev)
{
	int i;	
	StarDmaRxDisable(dev);
	star_mb();
	for (i=0; i<dev->rxRingSize; i++) DescRxTake(dev->rxdesc + i);
}


/* =========================================================
   MAC related functions
   ========================================================= */
int StarMacInit(StarDev *dev, u8 macAddr[6])
{
	u32 base = dev->base;

#if 1
//#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
//#else
//        /* MII mode */
//        StarSetReg(STAR_EXT_CFG(dev->ethPdwncBase), PDWNC_MAC_EXT_INIT); //0x73FF0000 Little endian
        /* Reset DMA
           Note: Resetting DMA is important when the tx/rx descriptors 
                 are not set by first time. If the descriptor address 
                 registers have ever been set before, you will need this
                 reset. 
                 Reset MRST, NRST, HRST by writing '0' then '1' */
//        StarSetReg(STAR_EXT_CFG(dev->base), MAC_EXT_INIT & (~(HRST|MRST|NRST)));
//        StarSetReg(STAR_EXT_CFG(dev->base), MAC_EXT_INIT); //0x7FFF0000 Little endian
//        #endif
#else
        /* RMII mode */
        StarSetReg(STAR_EXT_CFG(dev->ethPdwncBase), PDWNC_MAC_EXT_INIT_RMII);
        StarSetReg(STAR_EXT_CFG(dev->base), MAC_EXT_INIT_RMII);
#endif

	/* Set Mac Address */
	StarSetReg(STAR_My_MAC_H(base), macAddr[0]<<8 | macAddr[1]<<0);
	StarSetReg(STAR_My_MAC_L(base), macAddr[2]<<24 | macAddr[3]<<16 | macAddr[4]<<8 | macAddr[5]<<0);


	/* Set Mac Configuration */
#ifdef CHECKSUM_OFFLOAD
	StarSetReg( STAR_MAC_CFG(base),
				//STAR_MAC_CFG_TXCKSEN |
				STAR_MAC_CFG_RXCKSEN |
	            STAR_MAC_CFG_CRCSTRIP |
	            //STAR_MAC_CFG_TXAUTOPAD | //add for veirfy code
	            //STAR_MAC_CFG_ACPTCKSERR| //add for veirfy code
	            STAR_MAC_CFG_MAXLEN_1522 |
	            (0x1f & STAR_MAC_CFG_IPG_MASK) << STAR_MAC_CFG_IPG_OFFSET /* 12 byte IPG */
	          );
#else
	StarSetReg( STAR_MAC_CFG(base),
	            STAR_MAC_CFG_CRCSTRIP |
	            STAR_MAC_CFG_MAXLEN_1522 |
	            (0x1f & STAR_MAC_CFG_IPG_MASK) << STAR_MAC_CFG_IPG_OFFSET /* 12 byte IPG */
	          );
#endif
    /* Init Flow Control register */
    StarSetReg(STAR_FC_CFG(base),
               STAR_FC_CFG_SEND_PAUSE_TH_DEF |
               STAR_FC_CFG_UCPAUSEDIS |
               STAR_FC_CFG_BPEN
              );
    /* Init SEND_PAUSE_RLS */
    StarSetReg(STAR_EXTEND_CFG(base),
               STAR_EXTEND_CFG_SEND_PAUSE_RLS_DEF);
               
    

	/* Init MIB counter (reset to 0) */
	StarMibInit(dev);
	/* Enable Hash Table BIST */
	StarSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_HASHEN);
	/* Reset Hash Table (All reset to 0) */
	StarResetHashTable(dev);
	printk("star:ARL:0x%x \n",StarGetReg(STAR_ARL_CFG(base)));
	StarClearBit(STAR_ARL_CFG(base), STAR_ARL_CFG_MISCMODE);
	StarClearBit(STAR_ARL_CFG(base), STAR_ARL_CFG_HASHALG_CRCDA);
	printk("star:ARL:0x%x \n",StarGetReg(STAR_ARL_CFG(base)));

	printk("star:enable MY VID filter, filter VID=0\n");
    /*Strip VLAN tag in RX packet*/
	StarSetBit(STAR_MAC_CFG(base), STAR_MAC_CFG_VLANSTRIP);
    
    /*Enable all MY VID filter, use MY VID0~VID3 default value to filter VID=0*/
    StarSetReg(STAR_VLAN_CTRL(base),
               STAR_MY_VID_EN_ALL);
               
	return 0;
}

static void StarMibReset(StarDev *dev)
{
	u32 base = dev->base;

	/* MIB counter is read clear */
    
	StarGetReg(STAR_MIB_RXOKPKT(base));
	StarGetReg(STAR_MIB_RXOKBYTE(base));
	StarGetReg(STAR_MIB_RXRUNT(base));
	StarGetReg(STAR_MIB_RXOVERSIZE(base));
	StarGetReg(STAR_MIB_RXNOBUFDROP(base));
	StarGetReg(STAR_MIB_RXCRCERR(base));
	StarGetReg(STAR_MIB_RXARLDROP(base));
	StarGetReg(STAR_MIB_RXVLANDROP(base));
	StarGetReg(STAR_MIB_RXCKSERR(base));
	StarGetReg(STAR_MIB_RXPAUSE(base));
	StarGetReg(STAR_MIB_TXOKPKT(base));
	StarGetReg(STAR_MIB_TXOKBYTE(base));
	StarGetReg(STAR_MIB_TXPAUSECOL(base));
	
    StarSetReg(STAR_MIB_RXOKPKT(base), 0);
    StarSetReg(STAR_MIB_RXOKBYTE(base), 0);
    StarSetReg(STAR_MIB_RXRUNT(base), 0);
    StarSetReg(STAR_MIB_RXOVERSIZE(base), 0);
    StarSetReg(STAR_MIB_RXNOBUFDROP(base), 0);
    StarSetReg(STAR_MIB_RXCRCERR(base), 0);
    StarSetReg(STAR_MIB_RXARLDROP(base), 0);
    StarSetReg(STAR_MIB_RXVLANDROP(base), 0);
    StarSetReg(STAR_MIB_RXCKSERR(base), 0);
    StarSetReg(STAR_MIB_RXPAUSE(base), 0);
    StarSetReg(STAR_MIB_TXOKPKT(base), 0);
    StarSetReg(STAR_MIB_TXOKBYTE(base), 0);
    StarSetReg(STAR_MIB_TXPAUSECOL(base), 0);
	
}

int StarMibInit(StarDev *dev)
{
	StarMibReset(dev);

	return 0;
}

int StarPhyCtrlInit(StarDev *dev, u32 enable, u32 phyAddr)
{
	u32 base = dev->base;
	u32 data;

	data = STAR_PHY_CTRL1_FORCETXFC | 
	       STAR_PHY_CTRL1_FORCERXFC | 
	       STAR_PHY_CTRL1_FORCEFULL | 
	       STAR_PHY_CTRL1_FORCESPD_100M |
	       STAR_PHY_CTRL1_ANEN;

	if (enable) /* Enable PHY auto-polling */
	{
		StarSetReg( STAR_PHY_CTRL1(base),
		            data | STAR_PHY_CTRL1_APEN | ((phyAddr & STAR_PHY_CTRL1_PHYADDR_MASK) << STAR_PHY_CTRL1_PHYADDR_OFFSET)
		          );
	} else /* Disable PHY auto-polling */
	{
		StarSetReg( STAR_PHY_CTRL1(base),
		            data | STAR_PHY_CTRL1_APDIS
		          );
	}

	return 0;
}

void StarSetHashBit(StarDev *dev, u32 addr, u32 value)
{
	u32 base = dev->base;
	u32 data;

	do {} while (!StarIsSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_HTBISTDONE)); /* Wait for BIST Done */
	do {} while (!StarIsSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_HTBISTOK));   /* Wait for BIST OK */
	do {} while (StarIsSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_START));       /* Wait for Start Command clear */
	star_mb();
	data = (STAR_HASH_CTRL_HASHEN |
			STAR_HASH_CTRL_ACCESSWT | STAR_HASH_CTRL_START |
			(value ? STAR_HASH_CTRL_HBITDATA : 0) |
			((addr & STAR_HASH_CTRL_HBITADDR_MASK) << STAR_HASH_CTRL_HBITADDR_OFFSET) );
	StarSetReg(STAR_HASH_CTRL(base), data);
	//StarSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_START);
	star_mb();
	do {} while (StarIsSetBit(STAR_HASH_CTRL(base), STAR_HASH_CTRL_START));       /* Wait for Start Command clear */
}

int StarDetectPhyId(StarDev *dev)
{
	#define PHY_REG_IDENTFIR2	(3)      /* Reg3: PHY Identifier 2 */
	#define PHYID2_INTVITESSE	(0x0430) /* Internal PHY */
	#define PHYID2_RTL8201		(0x8201) /* RTL8201 */
	#define PHYID2_RTL8211		(0xC912) /* RTL8211 */
    #define PHYID2_IP101A       (0x0C50) /* IC+IP101A */
    #define PHYID2_SMSC7100     (0xC0B1) /*SMSC LAN7100 */
	#define PHYID2_SMSC8700     (0xC0C4) /*SMSC LAN8700 */
	#define PHYID2_SMSC8710A    (0xC0F1) /*SMSC LAN8710A */
	
	int phyId;
	u16 phyIdentifier2;

	for (phyId = 0; phyId < 32; phyId ++)
	{
		phyIdentifier2 = StarMdcMdioRead(dev, phyId, PHY_REG_IDENTFIR2);
		STAR_MSG(STAR_DBG,"%s(%d)PHY_ID=%d,vendor=0x%x\n",__FUNCTION__,__LINE__,phyId,phyIdentifier2);

		if (phyIdentifier2 == PHYID2_INTVITESSE)
		{
			STAR_MSG(STAR_DBG,"Star Ethernet: Internal Vitesse PHY\n\r");
			break;
		} else if (phyIdentifier2 == PHYID2_RTL8201)
		{
			STAR_MSG(STAR_DBG,"Star Ethernet: RTL8201 PHY\n\r");
			break;
		} else if (phyIdentifier2 == PHYID2_RTL8211)
		{
			STAR_MSG(STAR_DBG,"Star Ethernet: RTL8211 PHY\n\r");
			break;
		}
        else if ((phyIdentifier2&0xfff0) == PHYID2_IP101A)
        {
            STAR_MSG(STAR_DBG,"Star Ethernet: IC+IP101A PHY\n\r");
            break;
        }
        else if (phyIdentifier2 == PHYID2_SMSC7100)
        {
            STAR_MSG(STAR_DBG,"Star Ethernet: SMSC7100 PHY\n\r");
			
            break;
        }
        else if (phyIdentifier2 == PHYID2_SMSC8700)
        {
            STAR_MSG(STAR_DBG,"Star Ethernet: SMSC8700 PHY\n\r");
            break;
        }
		else if (phyIdentifier2 == PHYID2_SMSC8710A)
        {
            STAR_MSG(STAR_DBG,"Star Ethernet: SMSC8710A PHY\n\r");
            StarMdcMdioWrite(dev, phyId, 0,0x1000);
			StarMdcMdioWrite(dev, phyId, 4,0x0181);
			STAR_MSG(STAR_DBG,"Star Ethernet: SMSC8710A PHY:phy0=0x%x ,phy4=0x%x,enable AN funciton,if not open ,Mac don't update Link Info \n\r",StarMdcMdioRead(dev, phyId, 0),StarMdcMdioRead(dev, phyId, 4));
			
            break;
        }

	}

    /* If can't find phy id, try reading from PHY ID 2,
       and check the return value. If success, should return
       a value other than 0xffff.
     */
    if (phyId >= 32)
    {
        for (phyId = 0; phyId < 32; phyId ++)
        {
            phyIdentifier2 = StarMdcMdioRead(dev, phyId, PHY_REG_IDENTFIR2);
			
            if (phyIdentifier2 != 0xffff)
                break;
        }
    }
	return phyId;

}

#if (USE_INTERNAL_PHY)


////default SlewRate == 3 /////ok for 8560
void vStarSetSlewRate(StarDev *dev , u32 uRate)
{
  StarPrivate *starPrv = dev->starPrv;
  if(uRate>0x06)
  {
	STAR_MSG(STAR_ERR, "setting fail ! value must be 0 ~ 6 \n");
	return;
  }

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  switch(uRate)
 {
   case 0:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F01);
   break;
   case 1:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F03);
   break;
   case 2:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F07);
   break;
   case 3:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F0f);
   break;
   case 4:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F1f);
   break;
   case 5:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F3f);
   break;
   case 6:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F7f);
   break;

   default:
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x15, 0x0F0f);
   break;

  }
  star_mb(); 
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);
  
}

 void vStarGetSlewRate(StarDev *dev , u32 * uRate)
{
  StarPrivate *starPrv = dev->starPrv;
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  switch(StarMdcMdioRead(dev, starPrv->phy_addr, 0x15)&0x7F)
 {
   case 0x01:
    *uRate = 0;
   break;
   case 0x03:
    *uRate = 1;
   break;
   case 0x07:
    *uRate = 2;
   break;
   case 0x0f:
   *uRate = 3;
   break;
   case 0x1f:
   *uRate = 4;
   break;
   case 0x3f:
   *uRate = 5;
   break;
   case 0x7f:
   *uRate = 6;
   break;

   default:
    
   break;

  }
  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);
 
}


///ok for 8560 default Bias = 0x01
 void vStarSetOutputBias(StarDev *dev , u32 uOBiasLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  if(uOBiasLevel>0x03)
  {
	STAR_MSG(STAR_ERR, "setting fail ! value must be 0 ~ 3 \n");
	return;
  }

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();
  uOBiasLevel &= 0x03;	


  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1d);  ////REG_TXG_OBIAS_100  //default value = 1
  val = ( val & (~(0x03<<4)))|((uOBiasLevel & 0x03)<< 4);//bit4~ bit5
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1d, val);

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x19);  ////REG_TXG_OBIAS_STB_100  //default value = 2
  val = ( val & (~(0x03<<4)))|((uOBiasLevel & 0x03)<< 4);//bit4~ bit5
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x19, val);
   star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page 
}

 void vStarGetOutputBias(StarDev *dev , u32 * uOBiasLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1d);  ////REG_TXG_OBIAS_100  //default value = 1
  val = ( val>>4)  & 0x03 ;

  *uOBiasLevel = val ;
  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}


////for 8555 Bias=1 ;for 8560 Bias=1  8560 OK
void vStarSetInputBias(StarDev *dev , u32 uInBiasLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  if(uInBiasLevel>0x03)
  {
	STAR_MSG(STAR_ERR, "setting fail ! value must be 0 ~ 3 \n");
	return;
  }

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();
  uInBiasLevel = 0x03 - (uInBiasLevel& 0x03);	

 

 val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
 val = ( val& 0xfffc)|((uInBiasLevel & 0x03)<< 0);//bit 0~ bit1	//REG_TXG_BIASCTRL	default = 0
 StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1d, val);

val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x19);
val = ( val& 0xfffc)|((uInBiasLevel & 0x03)<< 0);//bit 0~ bit1 //REG_TXG_BIASCTRL  default = 3
StarMdcMdioWrite(dev, starPrv->phy_addr, 0x19, val);
star_mb();
StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}

void vStarGetInputBias(StarDev *dev , u32 * uInBiasLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

 val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
 val =  val & 0x03;//bit 0~ bit1	//REG_TXG_BIASCTRL	default = 0
  *uInBiasLevel = 0x03 - val;
  star_mb();

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}


void vSetDACAmp(StarDev *dev , u32 uLevel) /////10M
{
	u32 val,tmp,counter=0;
	StarPrivate *starPrv = dev->starPrv;
	if(uLevel>0x0F)
	{
	  STAR_MSG(STAR_ERR, "setting fail ! value must be 0~15 \n");
	  return;
	}

	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
	star_mb();
	val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
	val = ( val & (~(0x0F<<12)))|((uLevel & 0x0F)<< 12);
	do{
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1E, val);  
	star_mb();
			msleep(1);
			counter++;
			tmp = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
		  }while(((tmp>>12)&0x0f)!=uLevel);
	STAR_MSG(STAR_ERR, "test page 0x1E=0x%x,count=%d\n",tmp,counter);
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page

}
void vGetDACAmp(StarDev *dev , u32 * uLevel)
{
	u32 val;
	StarPrivate *starPrv = dev->starPrv;
	
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
	star_mb();
	val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
	*uLevel = ( val>>12)& 0x0f ;
    star_mb();
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page

}

void vSet100Amp(StarDev *dev , u32 uLevel) /////100M DAC 
{
	u32 val;
	StarPrivate *starPrv = dev->starPrv;
	if(uLevel>0x0F)
	{
	  STAR_MSG(STAR_ERR, "setting fail ! value must be 0~15 \n");
	  return;
	}

	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
	star_mb();
	val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1A);
	val = ( val & (~0x0F))|(uLevel & 0x0F);
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1A, val);  
	star_mb();
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page

}
void vGet100Amp(StarDev *dev , u32 * uLevel)
{
	u32 val;
	StarPrivate *starPrv = dev->starPrv;
	
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
	star_mb();
	val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1A);
	*uLevel =  val & 0x0f ;
    star_mb();
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page

}




///ok ,for 8560 uLevel 2 , for 8555 uLevel 0
 void vStarSet10MAmp(StarDev *dev , u32 uLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  if(uLevel>3)
  {
    STAR_MSG(STAR_ERR, "setting fail ! value must be 0~3 \n");
	return;
  }

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30); 	
  star_mb();
  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x18);
  val = ( val& (~(0x03<<0)))|((uLevel & 0x03)<< 0);//bit 0, bit1 default =2

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x18, val);
  star_mb();

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}

 void vStarGet10MAmp(StarDev *dev , u32* uLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30); 	
  star_mb();
  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x18);
  val = ( val);//bit 0, bit1 default =2

  *uLevel=val & 0x03 ;
  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}

#if IS_MT8580
static void vStarSetExtend10MAmp(StarDev *dev , u32 uLevel)
{
 
}



//ok for 8560 uLevel = 1 ,for 8555 uLevel =1
static void vStarSetOutputExtraBias(StarDev *dev , u32 uLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
  val = ( val & (~(0x01<<12)))|((uLevel & 0x01)<< 12);//bit 12	 //default = 0 
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1D, val);
  
  star_mb();
  
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}
#endif


//ok. for 8560 uEnable= 0 , for 8555 uEnable =0
 void vStarSet50PercentBW(StarDev *dev , u32 uEnable)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  
  if(uEnable>1)
  {
    STAR_MSG(STAR_ERR, "setting fail ! value must be 0~1 \n");
	return;
  }

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
   val = ( val & (~(0x01<<14)))|((uEnable & 0x01)<< 14);//bit 14 default 0
   StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1D, val);

  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}

 void vStarGet50PercentBW(StarDev *dev , u32* uEnable)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);
  star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
   val = ( val >> 14)&0x01;//bit 14 default 0
 
  *uEnable= val ;
  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}
 


//ok, for 8560 uLevel=1 ,for 8555 uLevel=3
 void vStarSetFeedBackCap(StarDev *dev , u32 uLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;

  if(uLevel>3)
  {
    STAR_MSG(STAR_ERR, "setting fail ! value must be 0~3 \n");
	return;
  }
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);//test page
  	star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
  val = ( val & (~(0x03<<8)))|((uLevel & 0x03)<< 8);//bit 8 bit 9  default 0
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1D, val);

  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}

 void vStarGetFeedBackCap(StarDev *dev , u32* uLevel)
{
  u32 val;
  StarPrivate *starPrv = dev->starPrv;
  

  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x2a30);//test page
  star_mb();

  val = StarMdcMdioRead(dev, starPrv->phy_addr, 0x1D);
  val = ( val >> 8);//bit 8 bit 9  default 0

  *uLevel= val & 0x03 ;
  star_mb();
  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x0);//Normal Page
  
}



#endif //#if (USE_INTERNAL_PHY)	

 void  StarGainEdgeSet(StarDev *dev)
{
    
	 StarPrivate *starPrv = dev->starPrv;
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x2a30);
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x08, 0x0210); /* disable gating of RCLK125 according to YT's suggest *///bit0 to clear for Harmonic in 8550.8560
    	{
    		volatile u32 data1,data2;
			data1=StarMdcMdioRead(dev, starPrv->phy_addr, 0x03);
			data2=StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x03, data1&(~(1<<6))); /* PLL Control register[6]=TCLK125M_EDGE_SEL=0 , POSITIVE EDGE */
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1e, data2&(~(1<<2))); /* ANARG Register[2]     =RG_TCK250_EDG_SEL=0, NEGATIVE EDGE */
		    data1=StarMdcMdioRead(dev, starPrv->phy_addr, 0x03);
			data2=StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);	
			STAR_MSG(STAR_ERR,"StarGainEdgeSet->Set edge\n");
    	}

		{
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x2a30);
			star_mb();
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x09, ((1<<15)|(1<<14)|StarMdcMdioRead(dev, starPrv->phy_addr, 0x09)));
			star_mb();
			{
			  u32  TempLow,TempHi;
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x52b5);  
				star_mb();
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x10, 0xafa4); 
				star_mb();
				TempLow = StarMdcMdioRead(dev, starPrv->phy_addr, 0x11);  
				TempHi = StarMdcMdioRead(dev, starPrv->phy_addr, 0x12);
				TempHi = (TempHi & (~(0x7f<<1)))|(0x2D<<1);
				star_mb();
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x11, TempLow);
				star_mb();
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x12, TempHi);
				star_mb();
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x10, 0x8fa4); 
				star_mb();
				StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x00);
			 }
			STAR_MSG(STAR_ERR,"StarGainEdgeSet->Set Gain=45\n");
		}

    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);
}
  
int StarIntPhyInit(StarDev *dev)
{
#if (USE_INTERNAL_PHY)	
    volatile u32 data;
    StarPrivate *starPrv = dev->starPrv;
	volatile u32 data1,data2;

  		  ////for 8560 IC test ///internal PHY
	StarSetReg(dev->base+0x94, 0x08);
	star_mb();
	StarSetReg(dev->base+0x9C, 0x1F);
	star_mb();

 


    data= (StarGetReg(RW_INTERNAL_PHY_CTRL(dev->base))& (~INTERNAL_PHY_ADDRESS_MASK));
    StarSetReg(RW_INTERNAL_PHY_CTRL(dev->base),data|(starPrv->phy_addr <<INTERNAL_PHY_ADDRESS_POS));
   //  StarSetBit(RW_INTERNAL_PHY_CTRL(dev->base), INTERNAL_PHY_ENABLE_MDC_MDIO);

    /* Init internal PHY settings */
    
    StarSetBit(STAR_PHY_CTRL1(dev->base), STAR_PHY_CTRL1_APDIS);//disable Auto Polling when set PHY register
    star_mb();
	

	{
	  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x2a30);
	   star_mb();
	  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x0E, 0x01); 
	   star_mb();
	  StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x00);
	  msleep(2);
	  printk("PLL VCO auto reset for 8580A \n");
	}

    /* Token ring */    

	star_mb();

    /* test page */
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x2a30);
	star_mb();
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x08, 0x0210); /* disable gating of RCLK125 according to YT's suggest *///bit0 to clear for Harmonic in 8550.8560


    star_mb();

		//clock edge choose
	data1=StarMdcMdioRead(dev, starPrv->phy_addr, 0x03);
	data2=StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
	
	//STAR_MSG(STAR_ERR, "Test Page PLL Control register(0x03h)=0x%0x,ANARG Register=0x%0x \n",data1,data2);
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x03, data1&(~(1<<6))); /* PLL Control register[6]=TCLK125M_EDGE_SEL=0 , POSITIVE EDGE */
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1e, data2&(~(1<<2))); /* ANARG Register[2]     =RG_TCK250_EDG_SEL=0, NEGATIVE EDGE */
    data1=StarMdcMdioRead(dev, starPrv->phy_addr, 0x03);
	data2=StarMdcMdioRead(dev, starPrv->phy_addr, 0x1E);
	
	//STAR_MSG(STAR_ERR, "confirm Test Page PLL Control register(0x03h)=0x%0x(is 0x8?),ANARG Register=0x%0x(is 0?) \n",data1,data2);		

	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x0000);


 
//work around for SLT fail issue for 8560/8580

	/////patch for SLT fail IC (Rx ADC not good)
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x2a30);
	star_mb();
	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x09, ((1<<15)|(1<<14)|StarMdcMdioRead(dev, starPrv->phy_addr, 0x09)));
    star_mb();
    {
      u32  TempLow,TempHi;
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x52b5);  
		star_mb();
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x10, 0xafa4);	
		star_mb();
		TempLow = StarMdcMdioRead(dev, starPrv->phy_addr, 0x11);  
		TempHi = StarMdcMdioRead(dev, starPrv->phy_addr, 0x12);
        TempHi = (TempHi & (~(0x7f<<1)))|(0x2D<<1);
		star_mb();
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x11, TempLow);
		star_mb();
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x12, TempHi);
		star_mb();
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x10, 0x8fa4); 
		star_mb();
		StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x00);
     }



    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x00);
	star_mb();
#if IS_MT8580

	vSetDACAmp(dev, INTERNAL_PHY_DACAMP);
	vSet100Amp(dev, INTERNAL_PHY_100AMP);

	
	
    vStarSetOutputBias(dev, INTERNAL_PHY_OUTPUT_BIAS_LEVEL);
    vStarSetInputBias(dev, INTERNAL_PHY_INPUT_BIAS_LEVEL);
    vStarSet10MAmp(dev, INTERNAL_PHY_10M_AMP_LEVEL);
	vStarSetOutputExtraBias(dev, INTERNAL_PHY_OUT_EXTRA_BAIS_CONTROL);
	
    vStarSetExtend10MAmp(dev, INTERNAL_PHY_EXTEND_10M_AMP_LEVEL); //only for after MT8555 ECO IC
    
    vStarSet50PercentBW(dev, INTERNAL_PHY_50_PERCENT_BW_ENABLE);
    vStarSetFeedBackCap(dev, INTERNAL_PHY_FEED_BACK_CAP);

	vStarSetSlewRate(dev, INTERNAL_PHY_SLEW_RATE_LEVEL);//set Slew rate
#endif

    star_mb();
    /* extend page */
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x01);  
	star_mb();
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1b, 0x1a0); /* enable auto-crossover as AN is disabled *///8555,8560

	StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);
	star_mb();

   

    /* main page */
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);
	star_mb();
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x17, 0x0020); /* ACTI_EN for power save when link down */

    

    star_mb();
    //End Internal PHY Setting
    StarClearBit(STAR_PHY_CTRL1(dev->base), STAR_PHY_CTRL1_APDIS);//Enable Auto Polling after  set PHY register
#endif//#if (!USE_INTERNAL_PHY)

    return 0;
}

void StarPhyReset(StarDev *dev)
{
    u16 val = 0;
    StarPrivate *starPrv = dev->starPrv;

    val = StarMdcMdioRead(dev, starPrv->phy_addr, 0);
    val |= 0x8000;
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0, val);
}


int StarHwInit(StarDev *dev)
{    
    /* Init pdwnc to enable Ethernet engine accessible by ARM only: 
        0: Accessed by ARM
        1: Accessed by T8032
    */
    /* SWITCH_ETHERNET_TO_ARM(); */
#if IS_MT8580
    StarSetReg(PDWNC_REG_UP_CONF_OFFSET(dev->pdwncBase),
                StarGetReg(PDWNC_REG_UP_CONF_OFFSET(dev->pdwncBase))&(~UP_CONF_ETEN_EN));
    star_mb();
	if((StarGetReg(dev->pinmuxBase+0x38) & 0x03) != 0x03 || (StarGetReg(dev->pinmuxBase+0x4C) & (1<<11)))
	{
	   	STAR_MSG(STAR_ERR, "Ethernet Clock was disabled,Now enable again!\n");
	   	StarSetReg(dev->pinmuxBase+0x38,StarGetReg(dev->pinmuxBase+0x38) | 0x03);
		StarSetReg(dev->pinmuxBase+0x4C,StarGetReg(dev->pinmuxBase+0x4C) & ~(1<<11));
		msleep(5);
	} 
#endif

    /* Init pinmux for Ethernet */
#if USE_EXTEND_PHY
    
    //pinmux setting           
   
	StarSetReg(dev->pinmuxBase+0x30,(StarGetReg(dev->pinmuxBase+0x30)&0xffff0FFF) | 0x6000);   //EXT_COL
	StarSetReg(dev->pinmuxBase+0x40,(StarGetReg(dev->pinmuxBase+0x40)&0xffff0000) | 0x4936);   // EXT_TXD2 , EXT_TXD1 , EXT_TXD0 ,EXT_MDIO,EXT_MDC
	StarSetReg(dev->pinmuxBase+0x50,(StarGetReg(dev->pinmuxBase+0x50)&0xffff0000) | 0x4924);   //EXT_RXDV, EXT_RXC, EXT_RXER, EXT_TXC, EXT_TXD3
	StarSetReg(dev->pinmuxBase+0x60,(StarGetReg(dev->pinmuxBase+0x60)&0xffffF000) | 0x0124);  //EXT_RXD2,EXT_RXD1,EXT_RXD0
	StarSetReg(dev->pinmuxBase+0x80,(StarGetReg(dev->pinmuxBase+0x80)&0xffff00FF) | 0x6c00);  //EXT_TXEN,EXT_RXD3
	

	STAR_MSG(STAR_ERR, "pinmux setting:0x%x=0x%x\n",dev->pinmuxBase+0x30,StarGetReg(dev->pinmuxBase+0x30));
	STAR_MSG(STAR_ERR, "pinmux setting:0x%x=0x%x\n",dev->pinmuxBase+0x40,StarGetReg(dev->pinmuxBase+0x40));
	STAR_MSG(STAR_ERR, "pinmux setting:0x%x=0x%x\n",dev->pinmuxBase+0x50,StarGetReg(dev->pinmuxBase+0x50));
	STAR_MSG(STAR_ERR, "pinmux setting:0x%x=0x%x\n",dev->pinmuxBase+0x60,StarGetReg(dev->pinmuxBase+0x60));
	STAR_MSG(STAR_ERR, "pinmux setting:0x%x=0x%x\n",dev->pinmuxBase+0x80,StarGetReg(dev->pinmuxBase+0x80));
	
	STAR_MSG(STAR_ERR, "external MII setting:0x%x[3:0]=0x%x\n",(dev->bspBase+0x3420),StarGetReg(dev->bspBase+0x3420));  //0xf0003420[3:0]: 1,RMII;
	STAR_MSG(STAR_ERR, "mac clock setting:   0x%x[11,9]=0x%x\n",(dev->bspBase+0x1208),StarGetReg(dev->bspBase+0x1208));  //0xf0001208[9],0xf0001208[11],
	STAR_MSG(STAR_ERR, "mac clock setting:   0x%x[18:16]=0x%x\n",(dev->bspBase+0xa0),StarGetReg(dev->bspBase+0xa0));  //0xf00000a0[18:16] 
	STAR_MSG(STAR_ERR, "ETH power down set:  0x%x[28]=0x%x\n",(dev->bspBase+0x3008),StarGetReg(dev->bspBase+0x3008));  
	STAR_MSG(STAR_ERR, "ETH power down clear:0x%x[28]=0x%x\n",(dev->bspBase+0x3010),StarGetReg(dev->bspBase+0x3010));  
	STAR_MSG(STAR_ERR, "ETH power down status:0x%x[28]=0x%x\n",(dev->bspBase+0x3018),StarGetReg(dev->bspBase+0x3018));  

 


	
	
    StarSetReg(ETHSYS_CONFIG(dev->base),SWC_MII_MODE|EXT_MDC_MODE|MII_PAD_OE);  
 
   STAR_MSG(STAR_ERR, "external MII setting:0x%x=0x%x\n",ETHSYS_CONFIG(dev->base),StarGetReg(ETHSYS_CONFIG(dev->base)));

   // add code to control 8710A nRST  signal.  

#endif


	star_mb();
    if (StarPhyMode(dev) == INT_PHY_MODE)
    {
        StarIntPhyInit(dev);
    }

	//star_led_init(dev);

    return 0;
}


void StarLinkStatusChange(StarDev *dev)

	{
			/* This function shall be called only when PHY_AUTO_POLL is enabled */
			u32 val = 0;
			u32 speed = 0;
		
			val = StarGetReg(STAR_PHY_CTRL1(dev->base));
		
			if (dev->linkUp != ((val & STAR_PHY_CTRL1_STA_LINK)?1UL:0UL))
			{
				dev->linkUp = (val & STAR_PHY_CTRL1_STA_LINK)?1UL:0UL;
				STAR_MSG(STAR_WARN, "Link status: %s\n", dev->linkUp? "Up":"Down");
				if (dev->linkUp)
				{
					speed = ((val >> STAR_PHY_CTRL1_STA_SPD_OFFSET) & STAR_PHY_CTRL1_STA_SPD_MASK);
					STAR_MSG(STAR_WARN, "%s Duplex - %s Mbps mode\n",
						   (val & STAR_PHY_CTRL1_STA_FULL)?"Full":"Half",
						   !speed? "10":(speed==1?"100":(speed==2?"1000":"unknown")));
					STAR_MSG(STAR_WARN, "TX flow control:%s, RX flow control:%s\n",
							(val & STAR_PHY_CTRL1_STA_TXFC)?"On":"Off",
							(val & STAR_PHY_CTRL1_STA_RXFC)?"On":"Off");
					netif_carrier_on(((StarPrivate *)dev->starPrv)->dev);

					
				}
				else
				{
					netif_carrier_off(((StarPrivate *)dev->starPrv)->dev);

				}
			}
	
	}




void StarNICPdSet(StarDev *dev, u32 val)
{
    u32 data;
    int retry = 0;
#define MAX_NICPDRDY_RETRY  10000
    
    data = StarGetReg(STAR_MAC_CFG(dev->base));
    if (val)
    {
        data |= STAR_MAC_CFG_NICPD;
        StarSetReg(STAR_MAC_CFG(dev->base), data);
        /* wait until NIC_PD_READY and clear it */
        do
        {
            if ((data = StarGetReg(STAR_MAC_CFG(dev->base))) & STAR_MAC_CFG_NICPDRDY)
            {
                data |= STAR_MAC_CFG_NICPDRDY; /* clear NIC_PD_READY */
                StarSetReg(STAR_MAC_CFG(dev->base), data);    
                break;
            }
        } while(retry++ < MAX_NICPDRDY_RETRY);
        if (retry >= MAX_NICPDRDY_RETRY)
        {
            STAR_MSG(STAR_ERR, "Error NIC_PD_READY is not set in %d retries!\n", MAX_NICPDRDY_RETRY);
        }
    }
    else
    {
        data &= (~STAR_MAC_CFG_NICPD);
        StarSetReg(STAR_MAC_CFG(dev->base), data);
    }
}

/**********************************************************
 * device cfg related functions
 **********************************************************/
static const char filename[] = "/misc/network.ini";
//write mac address into nand flash
int i4WriteEthMacAddr(struct net_device *dev, struct sockaddr *addr)
{
  int fd;
  int wr_size;
  int ret = 0;
  mm_segment_t old_fs;
  
  if (netif_running(dev))
	return -EBUSY;
  if (!is_valid_ether_addr(addr->sa_data))
	return -EADDRNOTAVAIL;

  old_fs = get_fs();
  set_fs(KERNEL_DS);

  fd = sys_open(filename, O_WRONLY, 0);
  if (fd >= 0)
  {
    wr_size = (int)sys_write(fd, addr->sa_data, (size_t)ETH_ALEN);
    sys_close(fd);
    if(wr_size == ETH_ALEN)
    {
      printk(KERN_ERR "[MAC]<write MAC addr ok> length : %d !!\n", wr_size);
    }
    else
    {
      printk(KERN_ERR "[MAC]<write MAC addr fail> return %d !!\n", wr_size);    
      ret = -EBUSY;
    }
  }
  else
  {
    printk(KERN_ERR "[MAC]<open file fail>handle : %d !!\n", fd);
    ret = -EBUSY;
  }

  set_fs(old_fs);

  return ret;
}
