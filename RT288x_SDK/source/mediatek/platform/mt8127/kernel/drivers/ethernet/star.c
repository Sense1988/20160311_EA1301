/**
 *	@file star.c
 *	@brief Star Ethernet Driver
 *	@author mtk02196
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/spi/spi.h>
#include <linux/workqueue.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
//#include <linux/mii.h>
#include <mach/irqs.h>

#include "star.h"
//#include "Ethconf.h"
#include <linux/proc_fs.h>
//static struct proc_dir_entry *star_proc_dir = NULL;
//static struct proc_dir_entry *star_eth_mac_entry = NULL;
//static struct proc_dir_entry *star_bt_mac_entry = NULL;
//static struct proc_dir_entry *star_slt_test_entry = NULL;
#ifdef FASTPATH
#include <asm/arch/fastpath/fastpath.h>
#endif
#ifdef SUPPORT_MOD_LOG                                                                                                                                                                                             
#include <asm/arch/log/mod_log.h>
#endif
#define  ETH_SUPPORT_WOL 1           //WOL : wake on lane

static spinlock_t star_lock;

#if defined (USE_RX_TASKLET) && defined (USE_RX_NAPI)
#warning "USE_RX_NAPI and USE_RX_TASKLET can not be used at the same time"
#undef USE_RX_TASKLET
#endif

#define SUPPORT_SUSPEND
/* ============= Global variable ============= */
static struct net_device *this_device = NULL;
static u32 eth_base = 0;
static u32 pinmux_base = 0;
static u32 pdwnc_base = 0;
#if IS_MT8580
static u32 eth_pdwnc_base = 0;
static u32 eth_chksum_base = 0;
static u32 ipll_base1 = 0;
static u32 ipll_base2 = 0;
#endif
static u32 bsp_base = 0;
static int eth_phy_id = 32;
static StarDev *star_dev = NULL;
static struct sk_buff *skb_dbg; /* skb pointer used for debugging */
//static int multicast_filter_limit = 32;
static u32 fgNetHWActive = 1;
static u32 fgStarOpen=false;
//static u32 disable_ethernet=false;
 
int star_dbg_lvl = STAR_DBG_LVL_DEFAULT;
/* ============= Global Definition ============= */
//#define DEFAULT_MAC_ADDRESS         { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 }
#define DEFAULT_MAC_ADDRESS         {0x00, 0x0C, 0xE7, 0x06, 0x00, 0x00}
#define DEFAULT_BT_MAC_ADDRESS    DEFAULT_MAC_ADDRESS

#define IRQ_SWITCH			    6

#define N_WRITE32(_reg_, _val_)   (*((volatile u32*)(0xfd000000+_reg_)) = (_val_))
#define N_REG32(_reg_)			(*((volatile u32*)(0xfd000000+_reg_)))
#define N_Mask(_reg_,_val_,_mask_ ) N_WRITE32(_reg_, ( N_REG32( _reg_ )&(~ _mask_))|(_val_ & _mask_) )

//int Ethernet_suspend(void);
//int Ethernet_resume(void);



/* same as netif_rx_complete, except that local_irq_save(flags)
 * has already been issued
 */
static inline void __netif_rx_complete(struct net_device *dev,
				       struct napi_struct *napi)
{
	__napi_complete(napi);
}

/* Test if receive needs to be scheduled but only if up */
static inline int netif_rx_schedule_prep(struct net_device *dev,
					 struct napi_struct *napi)
{
	return napi_schedule_prep(napi);
}

/* Add interface to tail of rx poll list. This assumes that _prep has
 * already been called and returned 1.
 */
static inline void __netif_rx_schedule(struct net_device *dev,
				       struct napi_struct *napi)
{
	__napi_schedule(napi);
}




void star_if_queue_up_down(struct up_down_queue_par *par);

#ifdef USE_RX_NAPI

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
static int star_poll(struct napi_struct *napi, int budget)
//#else
//static int star_poll(struct net_device *dev, int *budget)
//#endif
#else
static void star_receive(struct net_device *dev)
#endif
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
#ifdef USE_RX_NAPI	
	int npackets = 0;
#if 0 //(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8550)	
    u32 intrStatus = 0;
#endif    
//#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,10)
//	int quota = min(dev->quota, *budget);
//#endif
	int done = 0;
#endif

#if (defined USE_RX_NAPI) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	struct net_device *dev;
	starPrv = container_of(napi, StarPrivate, napi);
	dev = starPrv->dev;
#else
	starPrv = netdev_priv(dev);
#endif
	starDev = &starPrv->stardev;

#ifdef USE_RX_NAPI
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	while(npackets < budget)
//#else
//	while (npackets < quota)
//#endif
#else
	do
#endif
    {
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
		struct sk_buff *currSkb;
		struct sk_buff *newSkb;

		retval = StarDmaRxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);

		if (retval >= 0 && extBuf != 0)
		{
			currSkb = (struct sk_buff *)extBuf;
			dma_unmap_single(NULL, dmaBuf, skb_tailroom(currSkb), DMA_FROM_DEVICE);

			if (StarDmaRxValid(ctrlLen))
			{
#ifdef CHECKSUM_OFFLOAD
				int cksumOk = 0;

				if (StarDmaRxProtocolIP(ctrlLen) && !StarDmaRxIPCksumErr(ctrlLen)) /* IP packet & IP checksum correct */
				{
					//cksumOk = 1;
					if (StarDmaRxProtocolTCP(ctrlLen) || StarDmaRxProtocolUDP(ctrlLen)) /* TCP/UDP packet */
					{
						if (StarDmaRxL4CksumErr(ctrlLen)) /* L4 checksum error */
						{
							cksumOk = 0;
						}
						else
						{
							cksumOk = 1;
						}
					}
				}
#endif
				len = StarDmaRxLength(ctrlLen);

				if (len < ETH_MAX_LEN_PKT_COPY) /* Allocate new skb */
				{
					newSkb = currSkb;
#ifdef FASTPATH
					currSkb = __dev_alloc_skb((len + 2 + ETH_HDR_LEN_FOR_FASTPATH),(GFP_ATOMIC|__GFP_NOWARN));
					if (currSkb) skb_reserve(currSkb, 2 + ETH_HDR_LEN_FOR_FASTPATH);
#else
					currSkb = __dev_alloc_skb((len + 2),(GFP_ATOMIC|__GFP_NOWARN));
					if (currSkb) skb_reserve(currSkb, 2);
#endif
	
					if (!currSkb) /* No skb can be allocate */
					{
						currSkb = newSkb;
						newSkb = NULL;
					} else
					{
						memcpy(currSkb->data, newSkb->data, len);
					}
				} else
				{
#ifdef FASTPATH
					newSkb = __dev_alloc_skb((ETH_MAX_FRAME_SIZE + ETH_HDR_LEN_FOR_FASTPATH),(GFP_ATOMIC|__GFP_NOWARN));
#else
					newSkb = __dev_alloc_skb(ETH_MAX_FRAME_SIZE,(GFP_ATOMIC|__GFP_NOWARN));
#endif

					/* Shift to 16 byte alignment */
                    /*
					if ((u32)(newSkb->tail) & 0xf) 
					{
						u32 shift = ((u32)(newSkb->tail) & 0xf);
						skb_reserve(newSkb, shift);
					}
					*/
				if(newSkb)
				{
                    if((u32)(newSkb->tail) & (ETH_SKB_ALIGNMENT-1))
                    {
                        u32 offset = ((u32)(newSkb->tail) & (ETH_SKB_ALIGNMENT-1));
                        skb_reserve(newSkb, ETH_SKB_ALIGNMENT - offset);
                    }
				}
				else
				{		
				  // STAR_MSG(STAR_ERR, "star_receive mem alloc fail(1), packet dropped\n");
				}

					
					/* for zero copy */
#ifdef FASTPATH
					if (newSkb) skb_reserve(newSkb, 2 + ETH_HDR_LEN_FOR_FASTPATH);
#else
					if (newSkb) skb_reserve(newSkb, 2);
#endif
				}

				if (!newSkb) /* No skb can be allocated -> packet drop */
				{
					if (printk_ratelimit())
					{
					//	STAR_MSG(STAR_ERR, "star_receive mem alloc fail, packet dropped\n");
					}
					starDev->stats.rx_dropped ++;
					newSkb = currSkb;
				} else
				{
					skb_put(currSkb, len);
#ifdef CHECKSUM_OFFLOAD
					if (cksumOk) {currSkb->ip_summed = CHECKSUM_UNNECESSARY;} else {currSkb->ip_summed = CHECKSUM_NONE;}
#else
					currSkb->ip_summed = CHECKSUM_NONE;
#endif
					currSkb->dev = dev;
					currSkb->protocol = eth_type_trans(currSkb, dev);

#ifdef FASTPATH
					skb_push(currSkb, ETH_HLEN);
					if (!fastpath_in(FASTPATH_ID, currSkb))
					{
						skb_pull(currSkb, ETH_HLEN);
#endif

#ifdef USE_RX_NAPI					
						netif_receive_skb(currSkb);     /* send the packet up protocol stack */
#else
						netif_rx(currSkb);
#endif

#ifdef FASTPATH
					}
#endif
					dev->last_rx = jiffies;   		/* set the time of the last receive */
					starDev->stats.rx_packets ++;
					starDev->stats.rx_bytes += len;
#ifdef USE_RX_NAPI
					npackets ++;
#endif
				}
			} else
			{   /* Error packet */
				newSkb = currSkb;
				starDev->stats.rx_errors ++;
				starDev->stats.rx_crc_errors += StarDmaRxCrcErr(ctrlLen);
			}

			dmaBuf = dma_map_single(NULL, 
                                    newSkb->tail - 2/*Because Star Ethernet buffer must 16 byte align*/, 
                                    skb_tailroom(newSkb), 
                                    DMA_FROM_DEVICE);
			StarDmaRxSet(starDev, dmaBuf, skb_tailroom(newSkb), (u32)newSkb);
#ifdef USE_RX_NAPI
		}else
		{
			done = 1;
			break;
#endif
		}
    }
#ifndef USE_RX_NAPI
	while (retval >= 0);
#endif
    star_mb();
	StarDmaRxResume(starDev);

    /* Note: 
     *      MT8550 ethernet interrupt is edge trigger.
     *      If INT_STATUS is raised before INT is enabled,
     *      the interrupt may not be triggered. So we need
     *      to poll the interrupt status again after enabling
     *      the interrupt, and before exiting the service 
     *      routine.
     */
                
#ifdef USE_RX_NAPI
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	if (done)
	{
#if 0 //(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8550)	
	
		local_irq_disable();
		__netif_rx_complete(dev, napi);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */
        /* poll status before exiting */
        intrStatus = StarIntrStatus(starDev);
		local_irq_enable();
        /* Check if there is missing interrupt due to Edge trigger */
        if (intrStatus & STAR_INT_STA_RXC)
        {
            if (netif_rx_schedule_prep(dev, &starPrv->napi))
            {
                StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
                __netif_rx_schedule(dev, &starPrv->napi);
            }
        }
        if (intrStatus & STAR_INT_STA_TXC)
        {
#if defined (USE_TX_TASKLET)
            starPrv->tsk_tx = 1;
            StarIntrClear(starDev, STAR_INT_STA_TXC); /* Clear tx interrupt */
            tasklet_schedule(&starPrv->dsr);
#else
            star_finish_xmit(dev);
#endif /* USE_TX_TASKLET */

        }
        
#else  //other chip

       	local_irq_disable();
		__netif_rx_complete(dev, napi);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */        
		local_irq_enable();
#endif        
        
	}
	return(npackets);

#if 0
#error "linux version below 2.6.26 ----- yun";

//#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */
	*budget -= npackets;
	dev->quota -= npackets;
	if (done) /* All packets have been processed */
	{
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8550)		
		local_irq_disable();
		__netif_rx_complete(dev);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */
        /* poll status before exiting */
        intrStatus = StarIntrStatus(starDev);
		local_irq_enable();
        /* Check if there is missing interrupt due to Edge trigger */
        if (intrStatus & STAR_INT_STA_RXC)
        {
			if (netif_rx_schedule_prep(dev))
			{
				StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
				__netif_rx_schedule(dev);
			}
        }
        if (intrStatus & STAR_INT_STA_TXC)
        {
#if defined (USE_TX_TASKLET)
            starPrv->tsk_tx = 1;
            StarIntrClear(starDev, STAR_INT_STA_TXC); /* Clear tx interrupt */
            tasklet_schedule(&starPrv->dsr);
#else
            star_finish_xmit(dev);
#endif /* USE_TX_TASKLET */
        }
        
        
#else //other chip
       	local_irq_disable();
		__netif_rx_complete(dev);
		StarIntrRxEnable(starDev);   /* Enable rx interrupt */        
		local_irq_enable();
		 

#endif        
        return 0;
	}
	/* there are another packets need process */
	return 1;
//#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26) */
#endif
#endif /* USE_RX_NAPI */
}

static void star_finish_xmit(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	int wake = 0;

	STAR_MSG(STAR_VERB, "star_finish_xmit(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	do {
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
		unsigned long flags;
		
		spin_lock_irqsave(&starPrv->lock, flags);
		retval = StarDmaTxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);
		spin_unlock_irqrestore(&starPrv->lock, flags);

		if (retval >= 0 && extBuf != 0)
		{
			len = StarDmaTxLength(ctrlLen);
			dma_unmap_single(NULL, dmaBuf, len, DMA_TO_DEVICE);
			STAR_MSG(STAR_VERB, 
                     "star_finish_xmit(%s) - get tx descriptor %d for skb 0x%08x, length = %08x\n", 
                     dev->name, retval, extBuf, len);

			/* ??????????? reuse skb */

			dev_kfree_skb_irq((struct sk_buff *)extBuf);

			/* Tx statistics, use MIB? */
			starDev->stats.tx_bytes += len;
			starDev->stats.tx_packets ++;
			
			wake = 1;
		}
	} while (retval >= 0);

	if (wake)
	{
		netif_wake_queue(dev);
	}
}

#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
static void star_dsr(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	STAR_MSG(STAR_VERB, "star_dsr(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

#ifdef USE_TX_TASKLET
	if (starPrv->tsk_tx)
	{
		starPrv->tsk_tx = 0;
		star_finish_xmit(dev);
	}
#endif
#ifdef USE_RX_TASKLET
	if (starPrv->tsk_rx)
	{
		starPrv->tsk_rx = 0;
		star_receive(dev);
	}
#endif
}
#endif


static irqreturn_t star_isr(int irq, void *dev_id) 
{
    struct net_device *dev = (struct net_device *)dev_id;
    StarPrivate *starPrv = NULL;
    StarDev *starDev = NULL;
    u32 intrStatus;

    if (!dev)
    {
        STAR_MSG(STAR_ERR, "star_isr - unknown device\n");
        return IRQ_NONE;
    }

	STAR_MSG(STAR_VERB, "star_isr(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	intrStatus = StarIntrStatus(starDev);
#ifdef USE_RX_NAPI
	StarIntrClear(starDev, (intrStatus & (~STAR_INT_STA_RXC)));
#else
	StarIntrClear(starDev, intrStatus);
#endif

	do 
    {
		STAR_MSG(STAR_VERB, "star_isr - interrupt status = 0x%08x\n", intrStatus);

		if (intrStatus & STAR_INT_STA_RXC) // Rx Complete
		{
			STAR_MSG(STAR_VERB, "rx complete\n");
#ifdef USE_RX_NAPI
	//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
			if (netif_rx_schedule_prep(dev, &starPrv->napi))
			{
				StarIntrRxDisable(starDev); /* Disable rx interrupts */
				StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
				__netif_rx_schedule(dev, &starPrv->napi);
			}
	//#else
	//		if (netif_rx_schedule_prep(dev))
	//		{
	//			StarIntrRxDisable(starDev); /* Disable rx interrupts */
	//			StarIntrClear(starDev, STAR_INT_STA_RXC); /* Clear rx interrupt */
	//			__netif_rx_schedule(dev);
	//		}
	//#endif
#else
	#ifdef USE_RX_TASKLET
			starPrv->tsk_rx = 1;
	#else
			star_receive(dev);
	#endif
#endif
		}
		
		if (intrStatus & STAR_INT_STA_RXQF) // Rx Queue Full 
		{
			STAR_MSG(STAR_VERB, "rx queue full\n");
		}

		if (intrStatus & STAR_INT_STA_RXFIFOFULL) // Rx FIFO Full
		{
			STAR_MSG(STAR_VERB, "rx fifo full\n");
		}
		
		if (intrStatus & STAR_INT_STA_TXC) //Tx Complete 
		{
			STAR_MSG(STAR_VERB, " tx complete\n");
#ifdef USE_TX_TASKLET
			starPrv->tsk_tx = 1;
#else
			star_finish_xmit(dev);
#endif
		}

		if (intrStatus & STAR_INT_STA_TXQE) // Tx Queue Empty 
		{
			STAR_MSG(STAR_VERB, "tx queue empty\n");

		}

		if (intrStatus & STAR_INT_STA_RX_PCODE) 
		{
		 	STAR_MSG(STAR_DBG, "Rx PCODE\n");
		} 

		if (intrStatus & STAR_INT_STA_MAGICPKT) // Receive magic packet 
		{
			STAR_MSG(STAR_WARN, "magic packet received\n");
		}

		if (intrStatus & STAR_INT_STA_MIBCNTHALF) // MIB counter reach 2G (0x80000000) 
		{
			STAR_MSG(STAR_VERB, " mib counter reach 2G\n");
			StarMibInit(starDev);
		}

		if (intrStatus & STAR_INT_STA_PORTCHANGE) // Port status change 
		{
			
			STAR_MSG(STAR_DBG, "port status change\n");
            StarLinkStatusChange(starDev);
				
		}

		intrStatus = StarIntrStatus(starDev);  // read interrupt requests came during interrupt handling 
#ifdef USE_RX_NAPI
        StarIntrClear(starDev, (intrStatus & (~STAR_INT_STA_RXC)));
#else
        StarIntrClear(starDev, intrStatus);
#endif
    }
#ifdef USE_RX_NAPI
    while ((intrStatus & (~STAR_INT_STA_RXC)) != 0);
#else
    while (intrStatus != 0);
#endif 

	STAR_MSG(STAR_VERB, "star_isr return\n");
	
#if defined (USE_TX_TASKLET) && defined (USE_RX_TASKLET)
	if (starPrv->tsk_tx || starPrv->tsk_rx)
#elif defined (USE_TX_TASKLET)
	if (starPrv->tsk_tx)
#elif defined (USE_RX_TASKLET)
	if (starPrv->tsk_rx)
#endif
    {
#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
		tasklet_schedule(&starPrv->dsr);
#endif	
    }
	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void star_netpoll(struct net_device *dev)
{
	StarPrivate *tp   = netdev_priv(dev);
	StarDev     *pdev = tp->mii.dev;

	disable_irq(pdev->irq);
	star_isr(pdev->irq, dev);
	enable_irq(pdev->irq);
	STAR_MSG(STAR_ERR, "hank:%s(%d)\n",__FUNCTION__,__LINE__);
}
#endif

static int star_open(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	u32 intrStatus;


	if(fgStarOpen == true) return -1;
	STAR_MSG(STAR_DBG, "star_open(%s)\n", dev->name);

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;
		
    /* Start RX FIFO receive */
    StarNICPdSet(starDev, 0);
    star_mb();
	

	StarIntrDisable(starDev);
	star_mb();
	StarDmaTxStop(starDev);
	StarDmaRxStop(starDev);
	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);

    /* init carrier to off */
	starDev->linkUp = 0;
    netif_carrier_off(dev);
    
	/* Register NIC interrupt */
	STAR_MSG(STAR_DBG, "request interrupt vector=%d\n", dev->irq);

	if (request_irq(dev->irq, star_isr, IRQF_TRIGGER_FALLING, dev->name, dev) != 0)	/* request non-shared interrupt */
	{
		STAR_MSG(STAR_ERR, "interrupt %d request fail\n", dev->irq);
		return -ENODEV;
	}

	/* ============== Initialization due to Pin Trap in ASIC mode ==============  */
	/* ===== There are 3 modes: Internal switch/internal phy/RvMII(IC+175C) ===== */
	if (1) //(StarIsASICMode(starDev))
	{

		/* Force SMI Enable */
		StarSetBit(STAR_ETHPHY(starDev->base), STAR_ETHPHY_FRC_SMI_EN);

		switch (StarPhyMode(starDev))
		{
			case INT_PHY_MODE:
			{
				STAR_MSG(STAR_DBG, "Internal PHY mode\n");
				break;
			}
			case INT_SWITCH_MODE:
			{
				break;
			}
			case EXT_MII_MODE:
			{
				// TODO: External MII device initialization
				STAR_MSG(STAR_DBG, "External PHY mode\n");
				break;
			}
			case EXT_RGMII_MODE:
			{
				// TODO: Giga-Ethernet initialization
				break;
			}
			case EXT_RVMII_MODE:
			{
				u32 phyIdentifier;

				phyIdentifier = StarMdcMdioRead(starDev, 0, 3);
				if (phyIdentifier == 0x0d80)
				{	/* IC+ 175C */
					u16 ctrlRegValue = StarMdcMdioRead(starDev, 31, 5);
					ctrlRegValue |= 0x8000; /* Enable P4EXT */
					ctrlRegValue |= 0x0800; /* Enable MII0 mac mode */
					ctrlRegValue &= 0xfbff; /* Disable MII0 RMII mode */
					StarMdcMdioWrite(starDev, 31, 5, ctrlRegValue);
				}
				break;
			}
			default:
			{
				STAR_MSG(STAR_ERR, "star_open unknown Eth mode!\n\r");
				return -1;
			}
		}
	}

	STAR_MSG(STAR_WARN, "MAC Initialization\n");
	if (StarMacInit(starDev, dev->dev_addr) != 0)     /* MAC Initialization */
	{
		STAR_MSG(STAR_ERR, "MAC init fail\n");
		return -ENODEV;		
	}

    STAR_MSG(STAR_WARN, "StarDmaInit virAddr=0x%08x, dmaAddr=0x%08x\n", 
                starPrv->desc_virAddr, starPrv->desc_dmaAddr);
	if (StarDmaInit(starDev, starPrv->desc_virAddr, starPrv->desc_dmaAddr) != 0)    /* DMA Initialization */
	{
		STAR_MSG(STAR_ERR, "DMA init fail\n");
		return -ENODEV;
	}
	STAR_MSG(STAR_WARN, "PHY Control Initialization\n");
	STAR_MSG(STAR_VERB, "PHY Control Initialization:\n");
	if (StarPhyCtrlInit(starDev, 1/*Enable PHY auto-polling*/, starPrv->phy_addr) != 0)
	{
		STAR_MSG(STAR_ERR, "PHY Control init fail\n");
		return -ENODEV;
	}

	do { /* pre-allocate Rx buffer */
		dma_addr_t dmaBuf;
#ifdef FASTPATH
		struct sk_buff *skb = dev_alloc_skb(ETH_MAX_FRAME_SIZE + ETH_HDR_LEN_FOR_FASTPATH);
#else
		struct sk_buff *skb = dev_alloc_skb(ETH_MAX_FRAME_SIZE);
#endif

		/* Shift to 16 byte alignment */
        if((u32)(skb->tail) & (ETH_SKB_ALIGNMENT-1))
        {
            u32 offset = ((u32)(skb->tail) & (ETH_SKB_ALIGNMENT-1));
            skb_reserve(skb, ETH_SKB_ALIGNMENT - offset);
        }


		/* Reserve 2 bytes for zero copy */
#ifdef FASTPATH		
		if (skb) {skb_reserve(skb, 2 + ETH_HDR_LEN_FOR_FASTPATH);}
#else
        /* Reserving 2 bytes makes the skb->data points to
           a 16-byte aligned address after eth_type_trans is called.
           Since eth_type_trans will extracts the pointer (ETH_LEN)
           14 bytes. With this 2 bytes reserved, the skb->data
           can be 16-byte aligned before passing to upper layer. */
		if (skb) {skb_reserve(skb, 2);}
#endif
		
		if (skb == NULL)
		{
			STAR_MSG(STAR_ERR, "Error! No momory for rx sk_buff!\n");
			/* TODO */
			return -ENOMEM;
		}

        /* Note:
            We pass to dma addr with skb->tail-2 (4N aligned). But
            the RX_OFFSET_2B_DIS has to be set to 0, making DMA to write
            tail (4N+2) addr. 
         */
		dmaBuf = dma_map_single(NULL, 
                                skb->tail - 2/*Because Star Ethernet buffer must 16 byte align*/, 
                                skb_tailroom(skb), 
                                DMA_FROM_DEVICE);
		retval = StarDmaRxSet(starDev, dmaBuf, skb_tailroom(skb), (u32)skb);
		STAR_MSG(STAR_WARN, "rx descriptor idx:%d for skb %p\n", retval, skb);
		
		if (retval < 0)
		{
			dma_unmap_single(NULL, dmaBuf, skb_tailroom(skb), DMA_FROM_DEVICE);
			dev_kfree_skb(skb);
		}
	} while (retval >= 0);
    STAR_MSG(STAR_WARN, "rx descriptor done\n");  

#ifdef USE_RX_NAPI
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
		STAR_MSG(STAR_WARN, "napi_enable()\n"); 
		napi_enable(&starPrv->napi);
//#endif
#endif
#if USE_INTERNAL_PHY	
	StarGainEdgeSet(starDev);
    /* PHY reset */
	STAR_MSG(STAR_WARN, "StarPhyReset()\n"); 
	star_mb();


    StarPhyReset(starDev);
    /* wait for a while until PHY ready */
    msleep(50);
#endif
    star_mb();


	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);
	star_mb();
	StarIntrEnable(starDev);
	star_mb();
		
    
	vDmaTxStartAndResetTXDesc(starDev);
	vDmaRxStartAndResetRXDesc(starDev);
    star_mb();
#if USE_INTERNAL_PHY

    {
        /* NOTE by sarick---------------------
           This is a workaround for the problem that, when powering on 
           platform and the Ethernet is connected with a 10Mbps Hub, the
           platform could fail to get IP.
           We found if disabling and then enabling the Auto-Negotiation,
           the problem does not appear.
         */
        u32 val = 0;
        
        STAR_MSG(STAR_VERB, "Re-enabling AN\n");    
        val = StarGetReg(STAR_PHY_CTRL1(starDev->base));
        val &= ~(STAR_PHY_CTRL1_ANEN);
		val |= (starPrv->phy_addr<STAR_PHY_CTRL1_PHYADDR_OFFSET);
        StarSetReg(STAR_PHY_CTRL1(starDev->base), val);
		star_mb();
        val |= STAR_PHY_CTRL1_ANEN;
        StarSetReg(STAR_PHY_CTRL1(starDev->base), val);
    }

#endif

	netif_start_queue(dev);
	fgNetHWActive = 1;
	fgStarOpen = true ;
    STAR_MSG(STAR_WARN, "star_open done\n");	
	return 0;
}

static int star_stop(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	int retval;
	u32 intrStatus;


    if(fgStarOpen == false) return -1 ;
	STAR_MSG(STAR_DBG, "star_stop(%s)\n", dev->name);
	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	
	   
	netif_stop_queue(dev);

#ifdef USE_RX_NAPI
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	napi_disable(&starPrv->napi);
//#endif
#endif
    star_mb();
	StarIntrDisable(starDev);
	star_mb();

	StarDmaTxStop(starDev);
	StarDmaRxStop(starDev);
	intrStatus = StarIntrStatus(starDev);
	StarIntrClear(starDev, intrStatus);

	free_irq(dev->irq, dev);      /* free nic irq */

	do { /* Free Tx descriptor */		
		u32 extBuf;
		u32 ctrlLen;
		u32 len;
		dma_addr_t dmaBuf;
	
		retval = StarDmaTxGet(starDev, &dmaBuf, &ctrlLen, &extBuf);
		if (retval >= 0 && extBuf != 0)
		{
			len = StarDmaTxLength(ctrlLen);
			dma_unmap_single(NULL, dmaBuf, len, DMA_TO_DEVICE);
			STAR_MSG(STAR_DBG, "star_stop - get tx descriptor idx:%d for skb 0x%08x\n", retval, extBuf);
			dev_kfree_skb((struct sk_buff *)extBuf);
		}
	} while (retval >= 0);

	do { /* Free Rx descriptor */
		u32 extBuf;
		dma_addr_t dmaBuf;

		retval = StarDmaRxGet(starDev, &dmaBuf, NULL, &extBuf);
		if (retval >= 0 && extBuf != 0)
		{
			dma_unmap_single(NULL, dmaBuf, skb_tailroom((struct sk_buff *)extBuf), DMA_FROM_DEVICE);
			STAR_MSG(STAR_VERB, "star-stop - get rx descriptor idx:%d for skb 0x%08x\n", retval, extBuf);
			dev_kfree_skb((struct sk_buff *)extBuf);
		}
	} while (retval >= 0);

	
    star_mb();
    /* Stop RX FIFO receive */
    StarNICPdSet(starDev, 1);

    fgStarOpen = false ;
	return 0;
}

 int star_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	dma_addr_t dmaBuf;
	unsigned long flags;
	int retval;

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;


	/* If frame size > Max frame size, drop this packet */
	if (skb->len > ETH_MAX_FRAME_SIZE)
	{
		STAR_MSG(STAR_WARN, "start_xmit(%s) - Tx frame length is oversized: %d bytes\n", dev->name, skb->len);
		dev_kfree_skb(skb);
		starDev->stats.tx_dropped ++;
		return 0;
	}

#ifdef FASTPATH
	fastpath_out(FASTPATH_ID, skb);
#endif

	//STAR_MSG(STAR_VERB, "hard_start_xmit\n");

	/* ?????? force to send or return NETDEV_TX_BUSY */

	dmaBuf = dma_map_single(NULL, skb->data, skb->len, DMA_TO_DEVICE);
	
	spin_lock_irqsave(&starPrv->lock, flags);
	
	retval = StarDmaTxSet(starDev, dmaBuf, skb->len, (u32)skb);

    /* StarDmaTxSet fail,  no packet will be transmitted;Tx descriptor ring full
         the two cases call netif_stop_queue*/
    if ((retval < 0)||(starDev->txNum == starDev->txRingSize))
	{
        netif_stop_queue(dev);
        STAR_MSG(STAR_VERB, "StarDmaTxSet fail or Tx descriptor full\n");
	}

	spin_unlock_irqrestore(&starPrv->lock, flags);
    star_mb();
	StarDmaTxResume(starDev);
	dev->trans_start = jiffies;

    if(retval < 0)
    {
        STAR_MSG(STAR_ERR, "NETDEV_TX_BUSY\n");
        return NETDEV_TX_BUSY;
    }
    else
    {
        return NETDEV_TX_OK;
    }

}

static struct net_device_stats *star_get_stats(struct net_device *dev)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
  
	STAR_MSG(STAR_VERB, "get_stats\n");

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	return &starDev->stats;
}
#if 0
static void star_set_multicast_list(struct net_device *dev)
{
#define STAR_HTABLE_SIZE		(512)
	unsigned long flags;

	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
  
	STAR_MSG(STAR_VERB, "star_set_multicast_list\n");

	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	spin_lock_irqsave(&star_lock, flags);

	if (dev->flags & IFF_PROMISC)
	{
		STAR_MSG(STAR_WARN, "%s: Promiscuous mode enabled.\n", dev->name);
		StarArlPromiscEnable(starDev);
	} 
    else if ((netdev_mc_count(dev) > multicast_filter_limit) || (dev->flags & IFF_ALLMULTI))
	{
		u32 hashIdx;
		for (hashIdx = 0; hashIdx < STAR_HTABLE_SIZE; hashIdx ++)
		{
			StarSetHashBit(starDev, hashIdx, 1);
		}
	} 
    else
	{
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
		struct netdev_hw_addr *ha;
		netdev_for_each_mc_addr(ha, dev) {
			u32 hashAddr;
			hashAddr = (u32)(((ha->addr[0] & 0x1) << 8) + (u32)(ha->addr[5]));
			StarSetHashBit(starDev, hashAddr, 1);
		}
//#else // < linux kernel 2.6.34
//		u32 mcIdx;
//		struct dev_mc_list *mclist;
//		for (mcIdx = 0, mclist = dev->mc_list; mclist && mcIdx < netdev_mc_count(dev); mcIdx++, mclist = mclist->next)
//		{
//			u32 hashAddr;
//			hashAddr = (u32)(((mclist->dmi_addr[0] & 0x1) << 8) + (u32)(mclist->dmi_addr[5]));
//			StarSetHashBit(starDev, hashAddr, 1);
//		}
//#endif
	}

	spin_unlock_irqrestore(&star_lock, flags);
}
#endif
#define AN_ADV_REG   0x04
#define ETH_PHY_AN_10M_HALF  (1<<5)
#define ETH_PHY_AN_10M_FULL  (1<<6)
#define ETH_PHY_AN_100M_HALF  (1<<7)
#define ETH_PHY_AN_100M_FULL  (1<<8)

#define PHY_MODE_CTRL 0x00
#define RESTART_AN   (1<<9)


void vSetSpeedDuplexMode(StarDev *dev, u32 u32Port, u32 speedduplex )
{
  u32 u32Reg;

  u32Reg = StarMdcMdioRead(dev,u32Port,AN_ADV_REG);
  u32Reg &= ~(ETH_PHY_AN_10M_HALF | ETH_PHY_AN_10M_FULL | ETH_PHY_AN_100M_HALF | ETH_PHY_AN_100M_FULL) ;

  switch(speedduplex)
  {
	 case ETH_SPEED_10M_HALF :
	     u32Reg |=  ETH_PHY_AN_10M_HALF ;
		 STAR_MSG(STAR_ERR,"Star:ioctl:ETH_SPEED_10M_HALF\n");
		  break;

	 case ETH_SPEED_10M_FULL :
	       u32Reg |=  ETH_PHY_AN_10M_FULL |ETH_PHY_AN_10M_HALF;
		 STAR_MSG(STAR_ERR,"Star:ioctl:ETH_SPEED_10M_FULL\n");
	      break;

	 case ETH_SPEED_100M_HALF :
	 	  u32Reg |=  ETH_PHY_AN_100M_HALF ;
		  STAR_MSG(STAR_ERR,"Star:ioctl:ETH_SPEED_100M_HALF\n");
	       break;

	 case ETH_SPEED_100M_FULL :
	      u32Reg |=  ETH_PHY_AN_100M_FULL | ETH_PHY_AN_100M_HALF ;
	     STAR_MSG(STAR_ERR,"Star:ioctl:ETH_SPEED_100M_FULL\n");
		 break;

	 case ETH_SPEED_AUTO:
	  default:
	  	  {
            u32Reg |= (ETH_PHY_AN_10M_HALF | ETH_PHY_AN_10M_FULL | ETH_PHY_AN_100M_HALF | ETH_PHY_AN_100M_FULL);
          }
	  STAR_MSG(STAR_ERR,"Star:ioctl:ETH_SPEED_AUTO\n");
	  break;

  }

  StarMdcMdioWrite(dev, u32Port, AN_ADV_REG, u32Reg);
  star_mb();
  ////restart AN
  StarMdcMdioWrite(dev, u32Port, 0x00, (1<<9)| StarMdcMdioRead(dev, u32Port, 0x00));
  
 // u32Reg=StarMdcMdioRead(dev,u32Port,PHY_MODE_CTRL);
 // u32Reg|=RESTART_AN;
 // StarMdcMdioWrite(dev, u32Port, PHY_MODE_CTRL, u32Reg);


}


void vGetSpeedDuplexMode(StarDev *dev, u32 u32Port, u32 * pspeedduplex )
{
  u16 data;

  u32 base = dev->base;
  data = (u16)StarGetBitMask(STAR_PHY_CTRL1(base), STAR_PHY_CTRL1_STA_SPD_DPX_MASK, STAR_PHY_CTRL1_STA_SPD_OFFSET);

  switch(data)
  {
    case STAR_PHY_CTRL1_STA_10M_HALF ://000
        *pspeedduplex = ETH_SPEED_10M_HALF;
        STAR_MSG(STAR_ERR,"ioctl:ETH_SPEED_10M_HALF\n");
        break;
    case STAR_PHY_CTRL1_STA_100M_HALF ://001
        *pspeedduplex = ETH_SPEED_100M_HALF;
        STAR_MSG(STAR_ERR,"ioctl:ETH_SPEED_100M_HALF\n");
        break;
    case STAR_PHY_CTRL1_STA_10M_FULL :// 100
        *pspeedduplex = ETH_SPEED_10M_FULL;
        STAR_MSG(STAR_ERR,"ioctl:ETH_SPEED_10M_FULL\n");
        break;
    case STAR_PHY_CTRL1_STA_100M_FULL :// 101
        *pspeedduplex = ETH_SPEED_100M_FULL;
        STAR_MSG(STAR_ERR,"ioctl:ETH_SPEED_100M_FULL\n");
        break;
	  default:
	  break;

  }


}


static int star_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;
	unsigned long flags;
	int rc = 0;
	struct ioctl_eth_mac_cmd *mac_cmd= NULL;
	starPrv = netdev_priv(dev);
	starDev = &starPrv->stardev;

	if (!netif_running(dev))
		return -EINVAL;

	if (StarIsASICMode(starDev))
	{
		if (StarPhyMode(starDev) != INT_SWITCH_MODE)
		{
			if (starPrv->mii.phy_id == 32)
				return -EINVAL;
		}
	} 
    else
	{
		if (starPrv->mii.phy_id == 32)
			return -EINVAL;
	}

    switch (cmd)
    {
    	
    	case SIOC_WRITE_MAC_ADDR_CMD:
    	rc = i4WriteEthMacAddr(dev, &req->ifr_hwaddr);
    	break;
    	
    	case SIOC_THROUGHPUT_GET_CMD:
    	
    	break;
    	
			case SIOC_GLINKSTATE_CMD:
			{
					char *link_status ;
					link_status = req->ifr_data;
					//  if (!ioctl_GetLinkState_cmd->wr_cmd)
					{
						unsigned long val = StarGetReg(STAR_PHY_CTRL1(starDev->base));
						
						//*link_status= ((val & STAR_PHY_CTRL1_STA_LINK)? 0: 1) ;
						*link_status= starDev->linkUp ;//((val & STAR_PHY_CTRL1_STA_LINK)? 1:0) ;
						*link_status |= ((val & STAR_PHY_CTRL1_STA_FULL)? 0: (1<<3)) ;
						*link_status |=((((val >> STAR_PHY_CTRL1_STA_SPD_OFFSET) & STAR_PHY_CTRL1_STA_SPD_MASK)==0)?(1<<1):0);
						//printk("\nSIOC_GLINKSTATE_CMD %x\n",(int)*link_status);
					}
			}
	    rc = 0; 
			break;
    	
    	case SIOC_ETH_MAC_CMD:
    	 mac_cmd = (struct ioctl_eth_mac_cmd *)&req->ifr_data;
    	
         if(mac_cmd->eth_cmd == ETH_MAC_REG_READ)
         {
         	
         }
         else if(mac_cmd->eth_cmd == ETH_MAC_REG_READ)	
         {
         	
         }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_REG_WRITE)	
    	 {
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_TX_DESCRIPTOR_READ)	
    	 {
    	 	
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_RX_DESCRIPTOR_READ)	
    	 {
    	 	
    	 }	
    	 else if(mac_cmd->eth_cmd == ETH_MAC_UP_DOWN_QUEUE)	
    	 {
    	   star_if_queue_up_down(&mac_cmd->ifr_ifru.up_down_queue);	
    	 	
    	 }	
       	
        rc = 0; 
    	break;
    	
    	
        case SIOC_MDIO_CMD: /* for linux ethernet diag command */
        {
            struct ioctl_mdio_cmd *mdio_cmd = (struct ioctl_mdio_cmd *)&req->ifr_data;
            if (mdio_cmd->wr_cmd)
            {
                StarMdcMdioWrite(starDev, starPrv->mii.phy_id, mdio_cmd->reg, mdio_cmd->val);
            }
            else
            {
                mdio_cmd->val = StarMdcMdioRead(starDev, starPrv->mii.phy_id, mdio_cmd->reg);
            }
        }
            rc = 0;
            break;
		case SIOC_SET_SPEED_CMD:
        {
			STAR_MSG(STAR_ERR,"Star:ioctl:SIOC_SET_SPEED_CMD\n");
			mac_cmd = (struct ioctl_eth_mac_cmd *)&req->ifr_data;
			StarPhyDisableAtPoll(starDev);
	        vSetSpeedDuplexMode(starDev,starPrv->mii.phy_id,mac_cmd->eth_cmd);
			StarPhyEnableAtPoll(starDev);
            rc = 0;
            break;
		}
        case SIOC_GET_SPEED_DUPLEX_CMD:
        {
			STAR_MSG(STAR_ERR,"ioctl:SIOC_GET_SPEED_DUPLEX_CMD\n");
			mac_cmd = (struct ioctl_eth_mac_cmd *)&req->ifr_data;
	        vGetSpeedDuplexMode(starDev,starPrv->mii.phy_id,&(mac_cmd->eth_cmd));
            rc = 0;
            break;
        }

		case SIOC_PHY_CTRL_CMD: /* for linux ethernet diag command */
#if (USE_INTERNAL_PHY)
        {


            struct ioctl_phy_ctrl_cmd *pc_cmd = (struct ioctl_phy_ctrl_cmd *)&req->ifr_data;
            StarDisablePortCHGInt(starDev);
			star_mb();
            StarPhyDisableAtPoll(starDev);
			star_mb();
			if (pc_cmd->wr_cmd)
            {
                switch(pc_cmd->Prm)
                {
                 case ETH_PHY_DACAMP_CTRL:  //100M Amp

					STAR_MSG(STAR_ERR, "vSet100Amp(%d) \n",pc_cmd->val);
				 	vSet100Amp(starDev,pc_cmd->val);
				 	break;
					
				 case ETH_PHY_10MAMP_CTRL:
				 	STAR_MSG(STAR_ERR, "vSet10MAmp(%d) \n",pc_cmd->val);
				 	vStarSet10MAmp(starDev,pc_cmd->val);
				 	break;

				 case ETH_PHY_IN_BIAS_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetInputBias(%d) \n",pc_cmd->val);		
				   	vStarSetInputBias(starDev,pc_cmd->val);	 
					break;
				   
				 case ETH_PHY_OUT_BIAS_CTRL:
				 	STAR_MSG(STAR_ERR, "vStarSetOutputBias(%d) \n",pc_cmd->val);
					 vStarSetOutputBias(starDev,pc_cmd->val); 	 
				   break;

				 case ETH_PHY_FEDBAK_CAP_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetFeedbackCap(%d) \n",pc_cmd->val);	 
					 vStarSetFeedBackCap(starDev,pc_cmd->val);
				 	break;

				 case ETH_PHY_SLEW_RATE_CTRL:
				 	STAR_MSG(STAR_ERR, "vSetSlewRate(%d) \n",pc_cmd->val);
					 vStarSetSlewRate(starDev,pc_cmd->val);
				   break;
				   
				 case ETH_PHY_EYE_OPEN_CTRL:
					 STAR_MSG(STAR_ERR, "MT8560 do not have this setting \n");

				   break;

				 case ETH_PHY_BW_50P_CTRL:
				 	STAR_MSG(STAR_ERR, "vSet50percentBW(%d) \n",pc_cmd->val);
					 vStarSet50PercentBW(starDev,pc_cmd->val); 	 
				  break;
				  
                 default:
                  STAR_MSG(STAR_ERR, "set nothing \n");
					break;
					
                }

            }
            else
            {
				switch(pc_cmd->Prm)
				 {
				  case ETH_PHY_DACAMP_CTRL:  //100M Amp
					 STAR_MSG(STAR_ERR, "vGet100Amp() \n");
					 vGet100Amp(starDev,&(pc_cmd->val));				
					 break;
					 
				  case ETH_PHY_10MAMP_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGet10MAmp() \n");
					 vStarGet10MAmp(starDev,&(pc_cmd->val));
					 break;
				
				  case ETH_PHY_IN_BIAS_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetInputBias() \n");	 
					 vStarGetInputBias(starDev,&(pc_cmd->val));  
					 break;
					
				  case ETH_PHY_OUT_BIAS_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetOutputBias() \n");
					  vStarGetOutputBias(starDev,&(pc_cmd->val));	  
					break;
				
				  case ETH_PHY_FEDBAK_CAP_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetFeedBackCap() \n");	  
					  vStarGetFeedBackCap(starDev,&(pc_cmd->val));
					 break;
				
				  case ETH_PHY_SLEW_RATE_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGetSlewRate() \n");
					  vStarGetSlewRate(starDev,&(pc_cmd->val));
					break;
					
				  case ETH_PHY_EYE_OPEN_CTRL:
					  STAR_MSG(STAR_ERR, " do not have this setting \n");
					break;
				
				  case ETH_PHY_BW_50P_CTRL:
					 STAR_MSG(STAR_ERR, "vStarGet50PercentBW() \n");
					  vStarGet50PercentBW(starDev,&(pc_cmd->val));   
				   break;
				   
				  default:
				   STAR_MSG(STAR_ERR, "Get nothing \n");
					 break;
					 
				 }

            }
			 star_mb();
			 StarPhyEnableAtPoll(starDev);
			 star_mb();
			 StarIntrClear(starDev, STAR_INT_STA_PORTCHANGE);
			 star_mb();
             StarEnablePortCHGInt(starDev);
        rc = 0;

		}
#endif

        break;

        default:
            spin_lock_irqsave(&star_lock, flags);
            //rc = generic_mii_ioctl(&starPrv->mii, if_mii(req), cmd, NULL);
            spin_unlock_irqrestore(&star_lock, flags);
            break;
    }

	return rc;
}

static int mdcMdio_read(struct net_device *dev, int phy_id, int location)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	starPrv = netdev_priv(dev);  
	starDev = &starPrv->stardev;

    return (StarMdcMdioRead(starDev, phy_id, location));
}

static void mdcMdio_write(struct net_device *dev, int phy_id, int location, int val)
{
	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	starPrv = netdev_priv(dev);  
	starDev = &starPrv->stardev;


    StarMdcMdioWrite(starDev, phy_id, location, val);
}

const struct net_device_ops star_netdev_ops = {
	.ndo_open		= star_open,
	.ndo_stop		= star_stop,
	.ndo_start_xmit		= star_start_xmit,
	.ndo_get_stats 		= star_get_stats,
	//.ndo_set_multicast_list = star_set_multicast_list,
    .ndo_do_ioctl           = star_ioctl,
#ifdef CONFIG_NET_POLL_CONTROLLER
    .ndo_poll_controller	= star_netpoll,
#endif
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};


//#ifdef SUPPORT_SUSPEND
#if 0
static  int star_suspend(struct device *dev, pm_message_t state)
{	
#if ETH_SUPPORT_WOL
		       StarPrivate *starPrv = NULL;
			   StarDev *starDev = NULL;
			   starPrv = netdev_priv(this_device);
	           starDev = &starPrv->stardev;
		   StarSetReg(STAR_MAC_CFG(starDev->base),StarGetReg(STAR_MAC_CFG(starDev->base))|STAR_MAC_CFG_WOLEN);
		   STAR_MSG(STAR_DBG, "%s 0x33008=0x%x	\n", __FUNCTION__,StarGetReg(STAR_MAC_CFG(starDev->base)));
		   StarSetReg(STAR_INT_MASK(starDev->base),StarGetReg(STAR_INT_MASK(starDev->base))&(~STAR_INT_STA_MAGICPKT));
		   STAR_MSG(STAR_DBG, "%s 0x33054=0x%x	\n", __FUNCTION__,StarGetReg(STAR_INT_MASK(starDev->base)));
    StarSetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase),
                StarGetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase))|(UP_CONF_ETEN_EN));
	STAR_MSG(STAR_DBG, "%s 0x24188=0x%x  \n", __FUNCTION__,StarGetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase)));
#else
	if(fgStarOpen == FALSE) return 0 ;
	STAR_MSG(STAR_DBG, "%s entered\n", __FUNCTION__);
    if(this_device!=NULL)
    {
       #if 1 //(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8580)
        star_stop(this_device);
	    star_mb();
		N_Mask(0x2404C,(1<<11),(1<<11) );
		star_mb();
		N_Mask(0x24038,0x00,0x03 );
		//Ethernet_suspend();
	   #endif
	   #if 0//((CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563))
        star_stop(this_device);
	    star_mb();
		N_Mask(0x2404C,(1<<11),(1<<11) );
		star_mb();
		N_Mask(0x24038,0x00,0x103 );
	   #endif
	   #if 0//(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8560) && defined(CONFIG_ETH_PM_8560SDK)          //for 8560 SDK version. 

	 	StarPrivate *starPrv = NULL;
	    StarDev *starDev = NULL;
		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;

		#if USE_INTERNAL_PHY

	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0080); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<13)));//PL_BIAS_CTRL_MODE 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<11)));// 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<5)));//

		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)|((1<<6)));// PLL power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)|((1<<4)|(1<<5)));// MDI power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)|((1<<14)|(1<<13)));// BG power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page

		#endif

		fgNetHWActive = 0;
		#endif
    }
#endif
	return 0;
}	
static  int star_resume(struct device *dev)
{
#if ETH_SUPPORT_WOL
			StarPrivate *starPrv = NULL;
			StarDev *starDev = NULL;
			starPrv = netdev_priv(this_device);
			starDev = &starPrv->stardev;
		StarSetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase),
					StarGetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase))&(~UP_CONF_ETEN_EN));
		STAR_MSG(STAR_DBG, "%s 0x24188=0x%x  \n", __FUNCTION__,StarGetReg(PDWNC_REG_UP_CONF_OFFSET(starDev->pdwncBase)));
        StarSetReg(STAR_MAC_CFG(starDev->base),StarGetReg(STAR_MAC_CFG(starDev->base))&(~STAR_MAC_CFG_WOLEN));
		STAR_MSG(STAR_DBG, "%s 0x33008=0x%x  \n", __FUNCTION__,StarGetReg(STAR_MAC_CFG(starDev->base)));
        StarSetReg(STAR_INT_MASK(starDev->base),StarGetReg(STAR_INT_MASK(starDev->base))|(STAR_INT_STA_MAGICPKT));
		STAR_MSG(STAR_DBG, "%s 0x33054=0x%x  \n", __FUNCTION__,StarGetReg(STAR_INT_MASK(starDev->base)));
#else
	if(fgStarOpen == TRUE) return 0;
	STAR_MSG(STAR_DBG, "%s entered\n", __FUNCTION__);
	if(this_device!=NULL)
	{
		StarPrivate *starPrv = NULL;
	    StarDev *starDev = NULL;
		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;
		
		#if 1//(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8580)
		N_Mask(0x24038,0x03,0x03 );
		msleep(5);
		star_mb();
		N_Mask(0x2404C,(0<<11),(1<<11));
		msleep(5);
		star_mb();
		StarHwInit(starDev);
		star_mb();
	    star_open(this_device);
		star_mb();
		#endif
		#if 0 //((CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) || (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563))
		N_Mask(0x24038,0x103,0x103 );
		msleep(5);
		star_mb();
		N_Mask(0x2404C,(0<<11),(1<<11));
		msleep(5);
		star_mb();
		StarHwInit(starDev);
		star_mb();
	    star_open(this_device);
		star_mb();
	   // STAR_MSG(STAR_DBG, "[0x2404C]=0x%x ;[0x24188]=0x%x ;\n",N_REG32(0x2404C),N_REG32(0x24188));
		//Ethernet_resume();
		#endif
#if 0 //(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8560) && defined(CONFIG_ETH_PM_8560SDK)
		StarPrivate *starPrv = NULL;
		StarDev *starDev = NULL;

		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;

		#if USE_INTERNAL_PHY
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page		
		if(StarMdcMdioRead(starDev, starPrv->phy_addr, 0x01)!= 0x0000)
		{		   
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<13)));//PL_BIAS_CTRL_MODE 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<11)));// 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<5)));//
		
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)&(~(1<<6)));// PLL power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)&(~((1<<4)|(1<<5))));// MDI power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)&(~((1<<14)|(1<<13))));// BG power down
		}		
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page
		#endif
        fgNetHWActive = 1 ;
       #endif
	}
#endif	
	return 0;
}
	
#endif
#if 0//(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
int Ethernet_suspend(void)
{	
    STAR_MSG(STAR_ERR,"star:suspend\n");
	
#if 0 //def SUPPORT_SUSPEND

	if(this_device!=NULL )
	{
	 	StarPrivate *starPrv = NULL;
	    StarDev *starDev = NULL;
		starPrv = netdev_priv(this_device);

		//starPrv = netdev_priv(dev);  
		starDev = &starPrv->stardev;

		#if USE_INTERNAL_PHY
        StarPhyDisableAtPoll(starDev);
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page
        star_mb();
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0080); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<13)));//PL_BIAS_CTRL_MODE 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<11)));// 
	    StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<5)));//

		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)|((1<<6)));// PLL power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)|((1<<4)|(1<<5)));// MDI power down
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)|((1<<14)|(1<<13)));// BG power down
        star_mb();
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page
		//StarSetBit(STAR_PHY_CTRL1(dev->base),SWH_CK_PWN | INTER_PYH_PD);
		//StarSetBit(STAR_MAC_CFG(dev->base), NIC_PD);
		StarPhyEnableAtPoll(starDev);
		#endif


		//N_Mask(0x0304,(0<<19),(1<<19) ) ;  ///disable phy clock
		//N_Mask(0x0300,(0<<7),(1<<7) ) ;   ///disable mac clock
		fgNetHWActive = 0;

	}
	
#endif

	return 0;
}	

int Ethernet_resume(void)
{
	STAR_MSG(STAR_ERR,"star:resume\n");
	
#if 0//def SUPPORT_SUSPEND
	if( this_device!=NULL && fgNetHWActive == 0)
	{
		StarPrivate *starPrv = NULL;
		StarDev *starDev = NULL;

		//struct net_device *star_device = dev_get_drvdata(dev) ;
		starPrv = netdev_priv(this_device);
		starDev = &starPrv->stardev;

		#if USE_INTERNAL_PHY
		StarPhyDisableAtPoll(starDev);
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1f, 0x2a30);  //test page
		star_mb();
		if(StarMdcMdioRead(starDev, starPrv->phy_addr, 0x01)!= 0x0000)
		{
		   
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x00, 0x00);  
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x01, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x02, 0x0000); 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<13)));//PL_BIAS_CTRL_MODE 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)&(~(1<<11)));// 
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x12, StarMdcMdioRead(starDev, starPrv->phy_addr,0x12)|((1<<5)));//
		
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x13, StarMdcMdioRead(starDev, starPrv->phy_addr,0x13)&(~(1<<6)));// PLL power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x14, StarMdcMdioRead(starDev, starPrv->phy_addr,0x14)&(~((1<<4)|(1<<5))));// MDI power down
			StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x16, StarMdcMdioRead(starDev, starPrv->phy_addr,0x16)&(~((1<<14)|(1<<13))));// BG power down
		}
		star_mb();
		StarMdcMdioWrite(starDev, starPrv->phy_addr, 0x1F, 0x00);  //main page
        StarPhyEnableAtPoll(starDev);
		//StarIntPhyInit(starDev);
		#endif

        fgNetHWActive = 1 ;
			//StarClearBit(STAR_PHY_CTRL1(dev->base),SWH_CK_PWN | INTER_PYH_PD);
			//StarClearBit(STAR_MAC_CFG(dev->base), NIC_PD);
       
	}
#endif
	
	return 0;
}


#endif
#if 0
static int proc_star_show_cli(char *page, char **start,
                             off_t off, int count,
                             int *eof, void *data)
{

	star_show_verify_cmd();
	*eof = 1;
    return 0;
}

static void star_cli_exec(unsigned long data)
{
	struct cli_cmd *cmd;
	cmd =(struct cli_cmd*)data;
	exec_cli(cmd);
}
static int proc_verify_cli(struct file *file,
                             const char *buffer,
                             unsigned long count,
                             void *data)
{
    char *buf;
	//struct cli_cmd cmd;
    
	buf = kmalloc(count + 1, GFP_KERNEL);
	memset(buf,0,count+1);
	if(copy_from_user(buf, buffer, count))
            return -EFAULT;
	
	parse_cmd(buf,&cmd_data);
	if(star_dev==NULL)
		{
			printk("star_dev is null,not be init\n");
			return 0;
		}
	cmd_data.stardev=star_dev;
    exec_cli(&cmd_data);
		//tasklet_schedule(&cli_task);
    kfree(buf);
    return count;
}

static int __init star_init_procfs(void)
{
	STAR_MSG(STAR_ERR, "%s entered\n", __FUNCTION__);
	 star_proc_dir = proc_mkdir("driver/star", NULL);
    if(star_proc_dir == NULL) {
		 STAR_MSG(STAR_ERR, "star_proc_dir create failed\n" );
        goto proc_init_fail;
    }
   
	// create cli file
	star_cli_entry = create_proc_entry("cli", 0755, star_proc_dir);
    if(star_cli_entry == NULL) 
	{
		STAR_MSG(STAR_ERR, "star_slt_test_entry create failed\n" );
        goto proc_init_fail;
    }
    star_cli_entry->read_proc = proc_star_show_cli;
	star_cli_entry->write_proc = proc_verify_cli;
	
    return 0;
 proc_init_fail:
     return -1;
}
#endif

//int star_init(void)
void __iomem	*star_base;

//static int  star_init(struct platform_device *pdev)
static int  star_init(void)

{
	int result = 0;
	u8 macAddr[ETH_ADDR_LEN] = DEFAULT_MAC_ADDRESS;

	StarPrivate *starPrv = NULL;
	StarDev *starDev = NULL;

	//int	irq;
	//struct resource *regs;

	STAR_MSG(STAR_VERB, "%s entered 1\n", __FUNCTION__);
	//return 0;
	enable_clock(MT_CG_PERI_ETH,"Ethernet");
	STAR_MSG(STAR_DBG,"Enable ethernet clock\n");

	printk("hank:%s(%d)\n",__FUNCTION__,__LINE__);
#if 0
	if(!request_mem_region(pdev->resource[0].start, 
						   pdev->resource[0].end-pdev->resource[0].start + 1, 
						   pdev->name)){
		dev_err(&pdev->dev,"request_mem_region busy.\n");
		return -EBUSY;
	}
	
	regs = platform_get_resource ( pdev,IORESOURCE_MEM, 0 );
	printk("hank:%s(%d)regs->start =0x%x,regs->end=0x%x\n",__FUNCTION__,__LINE__,regs->start,regs->end);
	if(!regs){
		dev_err(&pdev->dev,"get resource regs NULL.\n");
		return -ENXIO;
	}
	
	//star_base = ioremap(regs->start, resource_size(regs));
	//printk("hank:%s(%d)irq=0x%x\n",__FUNCTION__,__LINE__,(int)star_base);
	//	if (!star_base) {
	//		dev_err(&pdev->dev, "ioremap failed\n");
	//		return -ENOMEM;
	//	}

	irq = platform_get_irq(pdev,0);
	
	printk("hank:%s(%d)irq=%d\n",__FUNCTION__,__LINE__,irq);
	if(irq  < 0) {
		dev_err(&pdev->dev,"platform_get_irq error. get invalid irq\n");
		return irq;
	}
#endif
	this_device = alloc_etherdev(sizeof(StarPrivate));
	if (!this_device)
	{
		result = -ENOMEM;
		goto out;
	}

	/* Base Register of  Ethernet */
	//eth_base = (u32)(ioremap_nocache(ETH_BASE, 320)); 
	eth_base =ETH_BASE;//IO_VIRT_TO_PHYS(ETH_BASE);
	//eth_base	=	(u32)ioremap(regs->start, resource_size(regs));
	if (!eth_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "eth_base=0x%08x\n", eth_base);

	/* Pinmux Register of Ethernet */
	//pinmux_base = (u32)(ioremap_nocache(PINMUX_BASE, 320)); 
	pinmux_base =PINMUX_BASE;//IO_VIRT_TO_PHYS(PINMUX_BASE);
	if (!pinmux_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "pinmux_base=0x%08x\n", pinmux_base);

#if IS_MT8580
	/* PDWNC base Register */
	pdwnc_base = (u32)(ioremap_nocache(PDWNC_BASE+PDWNC_BASE_CFG_OFFSET, 320)); 
	if (!pdwnc_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "pdwnc_base=0x%08x\n", pdwnc_base);

	/* Ethernet PDWNC Register */
	eth_pdwnc_base = (u32)(ioremap_nocache(ETHERNET_PDWNC_BASE, 320)); 
	if (!eth_pdwnc_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "eth_pdwnc_base=0x%08x\n", eth_pdwnc_base);

	/* Ethernet checksum Register */
	eth_chksum_base = (u32)(ioremap_nocache(ETH_CHKSUM_BASE, 320)); 
	if (!eth_chksum_base)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "eth_chksum_base=0x%08x\n", eth_chksum_base);

	/* IPLL1 Register */
	ipll_base1 = (u32)(ioremap_nocache(IPLL1_BASE, 320)); 
	if (!ipll_base1)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "ipll_base1=0x%08x\n", ipll_base1);

	/* IPLL2 Register */
	ipll_base2 = (u32)(ioremap_nocache(IPLL2_BASE, 320)); 
	if (!ipll_base2)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "ipll_base2=0x%08x\n", ipll_base2);
#endif

   //bsp_base = (u32)(ioremap_nocache(BSP_BASE, 16));
   bsp_base =BSP_BASE;//IO_VIRT_TO_PHYS(BSP_BASE);
    if (!bsp_base)
    {
        result = -ENOMEM;
        goto out;
    }

	STAR_MSG(STAR_DBG, "bsp_base=0x%08x\n", bsp_base);

  	starPrv = netdev_priv(this_device);
	memset(starPrv, 0, sizeof(StarPrivate));
	starPrv->dev = this_device;

#if defined (USE_TX_TASKLET) || defined (USE_RX_TASKLET)
	tasklet_init(&starPrv->dsr, star_dsr, (unsigned long)this_device);
#endif

    /* Init system locks */
	spin_lock_init(&starPrv->lock);
    spin_lock_init(&star_lock);
    spin_lock_init(&starPrv->tsk_lock);

	starPrv->desc_virAddr = (u32)dma_alloc_coherent(
                                    NULL, 
                                    TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                                    &(starPrv->desc_dmaAddr), 
                                    GFP_KERNEL);
	if (!starPrv->desc_virAddr)
	{
		result = -ENOMEM;
		goto out;
	}
	STAR_MSG(STAR_DBG, "desc addr: 0x%08x(virtual)/0x%08x(physical)\n", 
             starPrv->desc_virAddr, 
             starPrv->desc_dmaAddr);
    
  	starDev = &starPrv->stardev;
	StarDevInit(starDev, eth_base);
    starDev->pdwncBase = pdwnc_base;
    starDev->pinmuxBase = pinmux_base;
#if IS_MT8580
    starDev->ethPdwncBase = eth_pdwnc_base;
    starDev->ethChksumBase = eth_chksum_base;
    starDev->ethIPLL1Base = ipll_base1;
    starDev->ethIPLL2Base = ipll_base2;
#endif

    starDev->bspBase = bsp_base;

    starDev->starPrv = starPrv;

	StarNICPdSet(starDev, 0);
	STAR_MSG(STAR_ERR, "Ethernet disable powerdown!\n");


    /* Init hw related settings (eg. pinmux, clock ...etc) */
    if (StarHwInit(starDev) != 0)
    {
        STAR_MSG(STAR_ERR, "Ethernet MAC HW init fail!\n");
        result = -ENODEV;
        goto out;
    }
	
	//StarNICPdSet(starDev, 0);
		STAR_MSG(STAR_ERR, "Ethernet disable powerdown!\n");

	/* Get PHY ID */
	if (eth_phy_id == 32)
	{
		eth_phy_id = StarDetectPhyId(starDev);
        if (eth_phy_id == 32)
        {
            eth_phy_id = ETH_DEFAULT_PHY_ADDR;
            STAR_MSG(STAR_ERR, "Cannot detect PHY addr, default to %d\n", eth_phy_id);
        }
        else
            STAR_MSG(STAR_DBG, "PHY addr = 0x%04x\n", eth_phy_id);
		
	}
	starPrv->phy_addr = (u32)eth_phy_id;

	starPrv->mii.dev = this_device;
	starPrv->mii.mdio_read = mdcMdio_read;
	starPrv->mii.mdio_write = mdcMdio_write;
	starPrv->mii.phy_id_mask = 0x1f;
	starPrv->mii.reg_num_mask = 0x1f00;
	starPrv->mii.phy_id = eth_phy_id;

	this_device->addr_len = ETH_ADDR_LEN;
	memcpy(this_device->dev_addr, macAddr, this_device->addr_len);		 /* Set MAC address */


	this_device->irq = ETH_IRQ;
	this_device->base_addr = eth_base;
	this_device->netdev_ops = &star_netdev_ops;
#ifdef USE_RX_NAPI
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	netif_napi_add(this_device, &starPrv->napi, star_poll, 64);
//#else
//	this_device->poll = star_poll;
//	this_device->weight = 64;
//#endif
#endif

	/* Check Ethernet Hardware is star or not by AHB Burst Type Register */
	if (StarGetReg(STAR_AHB_BURSTTYPE(eth_base)) != 0x77)
	{
	    STAR_MSG(STAR_ERR, "Ethernet MAC HW is unknown!\n");
		result = -EFAULT;
		goto out;
	}

	if ((result = register_netdev(this_device)) != 0)
	{
		unregister_netdev(this_device);
		goto out;
	}

    #if 0//def SUPPORT_SUSPEND	
	dev= &(this_device->dev);
	dev->class->suspend = star_suspend;
	dev->class->resume = star_resume;
    #endif
    
    STAR_MSG(STAR_WARN, "Star MAC init done\n");
    skb_dbg = NULL;
    star_dev = starDev;
	
#if 0//(CONFIG_ETH_OFF == 1)
		Ethernet_suspend();
        fgNetHWActive = 1;
#endif
	//star_init_procfs();

	return 0;
	
out:

    if (starPrv && starPrv->desc_virAddr)
    {
	    dma_free_coherent(NULL, 
                        TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                        (void *)starPrv->desc_virAddr, 
                        starPrv->desc_dmaAddr);
    }
#if IS_MT8580

    if (bsp_base)
    {
        iounmap((u32 *)bsp_base);
        bsp_base = 0;
    }
    if (ipll_base2)
    {
        iounmap((u32 *)ipll_base2);
        ipll_base2 = 0;
    }
    if (ipll_base1)
    {
        iounmap((u32 *)ipll_base1);
        ipll_base1 = 0;
    }
    if (eth_chksum_base)
    {
        iounmap((u32 *)eth_chksum_base);
        eth_chksum_base = 0;
    }
    if (eth_pdwnc_base)
    {
        iounmap((u32 *)eth_pdwnc_base);
        eth_pdwnc_base = 0;
    }
    if (pdwnc_base)
    {
        iounmap((u32 *)pdwnc_base);
        pdwnc_base = 0;
    }
    if (pinmux_base)
    {
        iounmap((u32 *)pinmux_base);
        pinmux_base = 0;
    }
    if (eth_base)
    {
        iounmap((u32 *)eth_base);
        eth_base = 0;
    }
#endif	
    if (this_device)
    {
		unregister_netdev(this_device);
        free_netdev(this_device);
        this_device = NULL;
    }

	STAR_MSG(STAR_ERR, "Star MAC init fail\n");	
    return result;
	
}



void star_cleanup(void)
{
	if (this_device)
	{
		StarPrivate *starPrv;

		starPrv = netdev_priv(this_device);

		unregister_netdev(this_device);
		dma_free_coherent(NULL, 
                          TX_DESC_TOTAL_SIZE + RX_DESC_TOTAL_SIZE, 
                          (void *)starPrv->desc_virAddr, 
                          starPrv->desc_dmaAddr);
#if IS_MT8580

		if (bsp_base)
        {
            iounmap((u32 *)bsp_base);
            bsp_base = 0;
        }
        if (ipll_base2)
        {
            iounmap((u32 *)ipll_base2);
            ipll_base2 = 0;
        }
        if (ipll_base1)
        {
            iounmap((u32 *)ipll_base1);
            ipll_base1 = 0;
        }
        if (eth_chksum_base)
        {
            iounmap((u32 *)eth_chksum_base);
            eth_chksum_base = 0;
        }
        if (eth_pdwnc_base)
        {
            iounmap((u32 *)eth_pdwnc_base);
            eth_pdwnc_base = 0;
        }
        if (pdwnc_base)
        {
            iounmap((u32 *)pdwnc_base);
            pdwnc_base = 0;
        }
        if (pinmux_base)
        {
            iounmap((u32 *)pinmux_base);
            pinmux_base = 0;
        }
        if (eth_base)
        {
            iounmap((u32 *)eth_base);
            eth_base = 0;
        }
#endif		
		free_netdev(this_device);
	} 
    star_dev = NULL;
}

int star_if_up_down(int up)
{
    struct net_device * dev = this_device;

    if (!dev)
        return -1;
    if(up)
    {
       return  star_open(dev);
    }
    else
    {
       return  star_stop(dev);
    }

	return 0 ;
}


void star_if_queue_up_down(struct up_down_queue_par *par)
{
  struct net_device * dev = this_device;

  if(par->up)
  {
    STAR_MSG(STAR_DBG, "star_wake_queue(%s)\n", dev->name);
    netif_wake_queue(dev);
    	
  }	
  else
  {
    STAR_MSG(STAR_DBG, "star_stop_queue(%s)\n", dev->name);
    netif_stop_queue(dev);
    
  } 	
	
}


//EXPORT_SYMBOL(star_update_slt_test_result);
EXPORT_SYMBOL(star_if_up_down);
EXPORT_SYMBOL(star_cleanup);

module_param(eth_phy_id, int, S_IRUGO);
module_init(star_init)
module_exit(star_cleanup)

#if 0
struct platform_driver mt_ethernet_driver = {
	.driver = {
		.name = "mt-ethernet",
		.owner = THIS_MODULE,
	},
	.probe = star_init,
	.suspend = NULL,
	.resume = NULL,
	.remove = NULL,
};
static int __init mt_ethernet_init ( void )
{
	int ret;
	STAR_MSG(STAR_WARN ,"%s enter",__FUNCTION__);
	ret = platform_driver_register ( &mt_ethernet_driver );
	return ret;
}

static void __init mt_ethernet_exit ( void )
{
	platform_driver_unregister ( &mt_ethernet_driver );
}

module_init ( mt_ethernet_init );
module_exit ( mt_ethernet_exit );
#endif


