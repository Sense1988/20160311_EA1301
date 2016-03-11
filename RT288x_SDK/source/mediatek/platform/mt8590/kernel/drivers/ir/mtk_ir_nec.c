/* rc-dvb0700-big.c - Keytable for devices in dvb0700
 *
 * Copyright (c) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * TODO: This table is a real mess, as it merges RC codes from several
 * devices into a big table. It also has both RC-5 and NEC codes inside.
 * It should be broken into small tables, and the protocols should properly
 * be indentificated.
 *
 * The table were imported from dib0700_devices.c.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>

#include "mtk_ir_common.h"
#include "mtk_ir_core.h"
#include "mtk_ir_cus_nec.h" // include customer's key map
#include "mtk_ir_regs.h"    // include ir registers





#define MTK_NEC_CONFIG      (IRRX_CH_END_15 + IRRX_CH_IGSYN + IRRX_CH_HWIR)
#define MTK_NEC_SAPERIOD    (0x00F) //
#define MTK_NEC_THRESHOLD   (0x1)  

#define MTK_NEC_EXP_POWE_KEY1   0x00000000
#define MTK_NEC_EXP_POWE_KEY2   0x00000000

/*
for BD     (8/3MHZ) * MTK_NEC_SAPERIOD 
for 8127 (1/32KHZ)*MTK_NEC_SAPERIOD
*/


#define MTK_NEC_1ST_PULSE_REPEAT  (3)
#define MTK_NEC_BITCNT_NORMAL    (33)
#define MTK_NEC_BITCNT_REPEAT    (1)
#define MTK_NEC_BIT8_VERIFY      (0xff)

//set deglitch with the min number. when glitch < (33*6 = 198us,ignore)
 



#define NEC_INFO_TO_BITCNT(u4Info)      ((u4Info & IRRX_CH_BITCNT_MASK)    >> IRRX_CH_BITCNT_BITSFT)
#define NEC_INFO_TO_1STPULSE(u4Info)    ((u4Info & IRRX_CH_1ST_PULSE_MASK) >> IRRX_CH_1ST_PULSE_BITSFT)
#define NEC_INFO_TO_2NDPULSE(u4Info)    ((u4Info & IRRX_CH_2ND_PULSE_MASK) >> IRRX_CH_2ND_PULSE_BITSFT)
#define NEC_INFO_TO_3RDPULSE(u4Info)    ((u4Info & IRRX_CH_3RD_PULSE_MASK) >> IRRX_CH_3RD_PULSE_BITSFT)



static int mtk_ir_nec_init_hw(void);
static int mtk_ir_nec_uninit_hw(void);
static u32 mtk_ir_nec_decode( void * preserve);

static u32 _u4PrevKey = BTN_NONE;   // pre key


static struct rc_map_list mtk_nec_map = {
	.map = {
		.scan    = mtk_nec_table,   // here for custom to modify
		.size    = ARRAY_SIZE(mtk_nec_table),
		.rc_type = RC_TYPE_NEC,
		.name    = RC_MAP_MTK_NEC,
	}
};
  
  
#ifdef CONFIG_HAS_EARLYSUSPEND

static void mtk_ir_nec_early_suspend(void * preserve )
{
	IR_LOG_ALWAYS("\n");
}

static void mtk_ir_nec_late_resume(void * preserve )
{
	IR_LOG_ALWAYS("\n");
}
#else

#define mtk_ir_nec_early_suspend NULL
#define mtk_ir_nec_late_resume NULL
#endif


#ifdef CONFIG_PM_SLEEP

static int mtk_ir_nec_suspend(void * preserve )
{
	IR_LOG_ALWAYS("\n");
	return 0;
}

static int mtk_ir_nec_resume(void * preserve )
{
	IR_LOG_ALWAYS("\n");
	return 0;
}

#else

#define mtk_ir_nec_suspend NULL
#define mtk_ir_nec_resume NULL

#endif

static struct mtk_ir_core_platform_data mtk_ir_pdata_nec = {
	
	.input_name = MTK_INPUT_NEC_DEVICE_NAME,
	.p_map_list = &mtk_nec_map,
	.init_hw = mtk_ir_nec_init_hw,
	.uninit_hw = mtk_ir_nec_uninit_hw,
	.ir_hw_decode = mtk_ir_nec_decode,
	
	#ifdef CONFIG_HAS_EARLYSUSPEND
	.early_suspend = mtk_ir_nec_early_suspend,
	.late_resume =   mtk_ir_nec_late_resume,
	#endif

	#ifdef CONFIG_PM_SLEEP
    .suspend = mtk_ir_nec_suspend,
    .resume = mtk_ir_nec_resume,
	#endif
	
};


struct mtk_ir_device  mtk_ir_dev_nec = { 
   .dev_platform = {
   	  .name		  = MTK_IR_DRIVER_NAME, // here must be equal to 
	  .id		  = MTK_IR_ID_NEC,
	  .dev = {
	           .platform_data = &mtk_ir_pdata_nec,
			   .release       = release,
	         },
   	},
   	
};

static int mtk_ir_nec_uninit_hw(void)
{   
   
    // disable ir interrupt
	IR_WRITE_MASK(IRRX_IRINT_EN,IRRX_INTEN_MASK,IRRX_INTCLR_OFFSET,0x0); 
	mtk_ir_core_clear_hwirq_stat();  
	rc_map_unregister(&mtk_nec_map);
	
	return 0;
}


static int mtk_ir_nec_init_hw(void)
{  
   rc_map_register(&mtk_nec_map);

   //first setting power key//
  
   #if 1
   IR_WRITE32(IRRX_EXP_IRM1,MTK_NEC_EXP_POWE_KEY1);
   IR_WRITE32(IRRX_EXP_IRL1,MTK_NEC_EXP_POWE_KEY2);

   // disable interrupt 
   IR_WRITE_MASK(IRRX_IRINT_EN,IRRX_INTEN_MASK,IRRX_INTCLR_OFFSET,0x0);
   
   IR_WRITE32(IRRX_CONFIG_HIGH_REG,  MTK_NEC_CONFIG); // 0xf0021
   IR_WRITE32(IRRX_CONFIG_LOW_REG,  MTK_NEC_SAPERIOD);
   IR_WRITE32(IRRX_THRESHOLD_REG,  MTK_NEC_THRESHOLD);

   mtk_ir_core_clear_hwirq_stat();   
   // enable ir interrupt
   IR_WRITE_MASK(IRRX_IRINT_EN,IRRX_INTEN_MASK,IRRX_INTCLR_OFFSET,0x1);
   #endif
   
	  
   return 0;
	
}

u32  mtk_ir_nec_decode( void * preserve)
{
   u32 _au4IrRxData[2];   
   u32 _u4Info = IR_READ32(IRRX_COUNT_HIGH_REG);
   u32 u4BitCnt = NEC_INFO_TO_BITCNT(_u4Info);
   char *pu1Data = (char *)_au4IrRxData;
   
   _au4IrRxData[0] = IR_READ32(IRRX_COUNT_MID_REG);//NEC 's code data is in this register
   _au4IrRxData[1] = IR_READ32(IRRX_COUNT_LOW_REG);
	
	 if((0 != _au4IrRxData[0]) || (0 != _au4IrRxData[1]) || _u4Info != 0)
    {
	    IR_LOG_KEY( "RxIsr Info:0x%08x data: 0x%08x%08x\n", _u4Info,
	 	                          _au4IrRxData[1], _au4IrRxData[0]);
    }
	else
	{
		return BTN_INVALID_KEY; 
	}

     /* Check repeat key. */
    if ((u4BitCnt == MTK_NEC_BITCNT_REPEAT) )
    {
        if (((NEC_INFO_TO_1STPULSE(_u4Info) == MTK_NEC_1ST_PULSE_REPEAT) ||
        	(NEC_INFO_TO_1STPULSE(_u4Info) == MTK_NEC_1ST_PULSE_REPEAT - 1) ||
            (NEC_INFO_TO_1STPULSE(_u4Info) == MTK_NEC_1ST_PULSE_REPEAT + 1)) &&
            (NEC_INFO_TO_2NDPULSE(_u4Info) == 0) &&
            (NEC_INFO_TO_3RDPULSE(_u4Info) == 0))
        {
            goto end;		   // repeat key
        }
        else
        {
            IR_LOG_KEY("invalid repeat key!!!\n");
			_u4PrevKey = BTN_NONE;
		    goto end;
        }
    }

    /* Check invalid pulse. */
    if (u4BitCnt != MTK_NEC_BITCNT_NORMAL)  
    {
        IR_LOG_KEY("u4BitCnt(%d), should be(%d)!!!\n",u4BitCnt, MTK_NEC_BITCNT_NORMAL );
        _u4PrevKey = BTN_NONE;
		goto end;
    }
	/* Check invalid key. */
    if ((pu1Data[2] + pu1Data[3]) != MTK_NEC_BIT8_VERIFY)
    {
        IR_LOG_KEY("invalid nec key code!!!\n");
        _u4PrevKey = BTN_NONE;
		goto end;
    }
	
    _u4PrevKey = pu1Data[2];
	
end:
	
    return _u4PrevKey;
	
}






