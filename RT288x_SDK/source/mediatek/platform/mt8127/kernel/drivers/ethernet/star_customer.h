#ifndef _STAR_CUSTOMER_H_
#define _STAR_CUSTOMER_H_

#ifndef __KERNEL_GPIO_PINMUX__
#define __KERNEL_GPIO_PINMUX__
#endif
#include "star.h"
#if IS_MT8580 
#include <mach/ic_version.h>
#endif
#include <mach/mt85xx_gpio_pinmux.h>

extern int bsp_pinset(unsigned pinmux_sel,unsigned function);
extern int gpio_configure(unsigned gpio,int dir,int value);

//Customer setting  start:
// enable the ethernet  led for your choose

#define CUSTOMER_SETTING  0

#define SELECT_ELED_0 1
#define SELECT_ELED_1 1
#define SELECT_ELED_2 0
#define SELECT_ELED_3 0

//choose the HW PIN name
#define PIN_LED_LINK			0//PIN_LED0
#define PIN_LED_ACTIVITY   		0//PIN_LED1

//define the gpio function for your choose at HW Pin
#define ELED_0_PINMUX  			ETLED_0_SEL        	//pin  number
#define ELED_0_FUNCTION   		6                  	//pin function  number

#define ELED_1_PINMUX  			ETLED_1_SEL        	//pin  number
#define ELED_1_FUNCTION   		6        			//pin function  number

#define ELED_2_PINMUX  			ETLED_2_SEL        	//pin  number
#define ELED_2_FUNCTION   		0        			//pin function  number

#define ELED_3_PINMUX  			ETLED_3_SEL       	//pin  number
#define ELED_3_FUNCTION   		0        			//pin function  number

//define the every LED polarity and function(link or activity)
#define ELED_0_POLARITY    1						//0: active low, pin will output 0 voltage; 1: active high, pin will output 1 voltage
#define ELED_0_EVENT	   0						//0: link mode; 1: activity mode

#define ELED_1_POLARITY    1						//0: active low, pin will output 0 voltage; 1: active high, pin will output 1 voltage
#define ELED_1_EVENT	   1						//0: link mode; 1: activity mode

#define ELED_2_POLARITY    0						//0: active low, pin will output 0 voltage; 1: active high, pin will output 1 voltage
#define ELED_2_EVENT	   0						//0: link mode; 1: activity mode

#define ELED_3_POLARITY    0						//0: active low, pin will output 0 voltage; 1: active high, pin will output 1 voltage
#define ELED_3_EVENT	   0						//0: link mode; 1: activity mode

//Customer setting end



//register:link  event define
#define LINK_1000M			(1<<0)
#define LINK_100M			(1<<1)
#define LINK_10M			(1<<2)
#define LINK_DOWN			(1<<3)
#define LINK_FULL_DUPLEX	(1<<4)
#define LINK_HALF_DUPLEX	(1<<5)
#define LINK_FORCE_ON		(1<<6)
#define LED_POLARITY		(1<<14)           //0: active low, pin will output 0 voltage; 1: active high, pin will output 1 voltage
#define LED_ENABLE			(1<<15)

#define LINK_EVENT			(LINK_10M|LINK_100M|LINK_1000M)      

// register:activity event define
#define ACTIVITY_1000M_TX			(1<<0)
#define ACTIVITY_1000M_RX			(1<<1)
#define ACTIVITY_100M_TX			(1<<2)
#define ACTIVITY_100M_RX			(1<<3)
#define ACTIVITY_10M_TX				(1<<4)
#define ACTIVITY_10M_RX				(1<<5)
#define ACTIVITY_COLISION			(1<<6)
#define ACTIVITY_RX_CRC_ERROR		(1<<7)
#define ACTIVITY_RX_IDLE_ERROR		(1<<8)
#define ACTIVITY_FORCE_BLINKS		(1<<9)

#define ACTIVITY_EVENT				(ACTIVITY_10M_RX|ACTIVITY_10M_TX|ACTIVITY_100M_RX|ACTIVITY_100M_TX|ACTIVITY_1000M_RX|ACTIVITY_1000M_TX)



void star_led_set_linkup(void);
void star_led_set_linkdown(void);
void star_led_set_activity(StarDev *dev);
void star_led_init(StarDev *dev);

void star_led_set_linkup(void)
{
#if !CUSTOMER_SETTING
	return ;
#endif
#if 0//(CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)           		//for 8550,8555, just need set activity function for Eethernet LED, because the 10M link status is incorrect(HW issue)
						
				#if SELECT_ELED_0
							gpio_configure(PIN_LED_LINK,OUTPUT,ELED_0_POLARITY);  //link mode , light LED	  
				#endif
				#if SELECT_ELED_1
							gpio_configure(PIN_LED_LINK,OUTPUT,ELED_1_POLARITY);  //link mode , light LED	  
				#endif
				#if SELECT_ELED_2
							gpio_configure(PIN_LED_LINK,OUTPUT,ELED_2_POLARITY);  //link mode , light LED	  
				#endif
				#if SELECT_ELED_1
							gpio_configure(PIN_LED_LINK,OUTPUT,ELED_3_POLARITY);  //link mode , light LED	  
				#endif

				//add other GPIO control if customer need
#else
             // other IC
#endif

}
void star_led_set_linkdown(void)
{
#if !CUSTOMER_SETTING
		return ;
#endif

#if 0//(CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)           		//for 8550,8555, just need set activity function for Eethernet LED, because the 10M link status is incorrect(HW issue)
					
				#if SELECT_ELED_0
						gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_0_POLARITY);  //link mode , close LED	  
				#endif
				#if SELECT_ELED_1
						gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_1_POLARITY);  //link mode , close LED	  
				#endif
				#if SELECT_ELED_2
						gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_2_POLARITY);  //link mode , close LED	  
				#endif
				#if SELECT_ELED_1
						gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_3_POLARITY);  //link mode , close LED	  
				#endif
				
				//add other GPIO control if customer need
#else
				// other IC
#endif


}
void star_led_set_activity(StarDev *dev)
{


	u16 linkSetting, activitySetting;	
	u16 data;
	StarPrivate *starPrv = dev->starPrv;

	linkSetting=0;
	activitySetting=0;	
	data=0;
#if !CUSTOMER_SETTING
			return ;
#endif

#if 0//(CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)  
		data =(1<<0);        //LED_EN=1;

	    /* extend page */
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x01);   

    
				#if SELECT_ELED_0
					#if ELED_0_POLARITY								
				     data |=(1<<2); 
					#endif
				#endif
				#if SELECT_ELED_1
					#if ELED_1_POLARITY
					data |=(1<<2); 
					#endif
				#endif
				#if SELECT_ELED_2
					#if ELED_2_POLARITY
					data |=(1<<2); 
					#endif
				#endif
				#if SELECT_ELED_3
					#if ELED_3_POLARITY
					data |=(1<<2); 
					#endif
				#endif

    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x10, data);
    STAR_MSG(STAR_ERR, "LED 0x10=0x%x\n", StarMdcMdioRead(dev, starPrv->phy_addr, 0x10));
    /* main page */
    StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1F, 0x00);	

#else
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x0002);
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x00, ((0x1<<15)|StarMdcMdioRead(dev, starPrv->phy_addr, 0x00)));	//extended mode
			
		#if SELECT_ELED_0
			#if ELED_0_POLARITY
					linkSetting |=LED_POLARITY;
			#endif
			#if !ELED_0_EVENT
					linkSetting |=(LED_ENABLE|LINK_EVENT);
			#else
					linkSetting |=(LED_ENABLE);
				    activitySetting |=ACTIVITY_EVENT;
			#endif
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x03, linkSetting);  
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x04,activitySetting ); 
		#endif
			
		#if SELECT_ELED_1
			#if ELED_1_POLARITY
					linkSetting |=LED_POLARITY;
			#endif
			#if !ELED_1_EVENT
					linkSetting |=(LED_ENABLE|LINK_EVENT);
			#else
					linkSetting |=(LED_ENABLE);
				    activitySetting |=ACTIVITY_EVENT;
			#endif
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x05, linkSetting);  
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x06,activitySetting ); 
		#endif
			
		#if SELECT_ELED_2
			#if ELED_2_POLARITY
					linkSetting |=LED_POLARITY;
			#endif
			#if !ELED_2_EVENT
					linkSetting |=(LED_ENABLE|LINK_EVENT);
			#else
					linkSetting |=(LED_ENABLE);
				    activitySetting |=ACTIVITY_EVENT;
			#endif
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x07, linkSetting);  
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x08,activitySetting ); 
		#endif
			
		#if SELECT_ELED_3
			#if ELED_3_POLARITY
					linkSetting |=LED_POLARITY;
			#endif
			#if !ELED_3_EVENT
					linkSetting |=(LED_ENABLE|LINK_EVENT);
			#else
					linkSetting |=(LED_ENABLE);
				    activitySetting |=ACTIVITY_EVENT;
			#endif
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x09, linkSetting);  
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x0a,activitySetting ); 
		#endif
			
			STAR_MSG(STAR_ERR, "LED Basic Control Register(0x00)=0x%x \n",StarMdcMdioRead(dev, starPrv->phy_addr, 0x00));
			StarMdcMdioWrite(dev, starPrv->phy_addr, 0x1f, 0x0);
#endif

}
void star_led_init(StarDev *dev)
{



#if !CUSTOMER_SETTING
		return ;
#endif

#if 0//(CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)           		//for 8550,8555, just need set activity function for Eethernet LED, because the 10M link status is incorrect(HW issue)
				
				#if SELECT_ELED_0
					#if ELED_0_EVENT								//activity mode
					bsp_pinset(ELED_0_PINMUX,ELED_0_FUNCTION);	
					#else
					gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_0_POLARITY);  //link mode , close LED    
					#endif
				#endif
				#if SELECT_ELED_1
					#if ELED_1_EVENT
					bsp_pinset(ELED_1_PINMUX,ELED_1_FUNCTION);	
					#else
					gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_1_POLARITY);  //link mode , close LED    
					#endif
				#endif
				#if SELECT_ELED_2
					#if ELED_2_EVENT
					bsp_pinset(ELED_2_PINMUX,ELED_2_FUNCTION);
					#else
					gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_2_POLARITY);  //link mode , close LED    
					#endif
				#endif
				#if SELECT_ELED_3
					#if ELED_3_EVENT
					bsp_pinset(ELED_3_PINMUX,ELED_3_FUNCTION);
					#else
					gpio_configure(PIN_LED_LINK,OUTPUT,!ELED_3_POLARITY);  //link mode , close LED    
					#endif
				#endif
#else
			{//config gpio
				#if SELECT_ELED_0
					bsp_pinset(ELED_0_PINMUX,ELED_0_FUNCTION);	
				#endif
				#if SELECT_ELED_1
					bsp_pinset(ELED_1_PINMUX,ELED_1_FUNCTION);	
				#endif
				#if SELECT_ELED_2
					bsp_pinset(ELED_2_PINMUX,ELED_2_FUNCTION);	
				#endif
				#if SELECT_ELED_3
					bsp_pinset(ELED_3_PINMUX,ELED_3_FUNCTION);	
				#endif
	
			}
#endif			
			star_led_set_activity(dev);

}
#endif /* _STAR_CUSTOMER_H_ */

