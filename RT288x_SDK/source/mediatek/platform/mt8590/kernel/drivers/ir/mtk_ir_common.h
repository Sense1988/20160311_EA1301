
#ifndef __MTK_IR_COMMON_H__
#define __MTK_IR_COMMON_H__


#include "mtk_ir_cus_define.h"

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/kernel.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#endif


struct mtk_ir_msg
{   
    u32	scancode; // rc scan code
	u32	keycode;  // linux input code
    //u32 rep; // repeat times ,driver not care repeat status to userspace
};


#define BTN_NONE                    0XFFFFFFFF
#define BTN_INVALID_KEY             -1

#define MTK_IR_CHUNK_SIZE sizeof(struct mtk_ir_msg)

#define MTK_IR_DEVICE_NODE   1   //whther has mtk_ir_dev.c


#ifdef  __KERNEL__ 


#define IR_LOG_TAG      "[mt8127]"

extern int ir_log_debug_on;
extern void mtk_ir_core_log_always(const char *fmt, ...);

#define IR_W_REGS_LOG(fmt, arg...) \
    do { \
        if (ir_log_debug_on)  printk(IR_LOG_TAG"%s, line(%d)\n"fmt, __FUNCTION__,__LINE__, ##arg); \
    }while (0)


#define IR_LOG_DEBUG(fmt, arg...) \
	do { \
		if (ir_log_debug_on) mtk_ir_core_log_always(IR_LOG_TAG"%s, line(%d)\n"fmt, __FUNCTION__,__LINE__, ##arg); \
	}while (0)

#define IR_LOG_DEBUG_TO_KERNEL(fmt, arg...) \
			do { \
				if (ir_log_debug_on) printk(IR_LOG_TAG"%s, line(%d)\n"fmt, __FUNCTION__,__LINE__, ##arg); \
			}while (0)

#if 1
#define IR_LOG_ALWAYS(fmt, arg...)  \
	do { \
		 printk(IR_LOG_TAG"%s, line(%d)\n"fmt, __FUNCTION__,__LINE__, ##arg);	 \
	}while (0)

#endif

#if 0
#define IR_LOG_ALWAYS(fmt, arg...)  \
	do { \
		 mtk_ir_core_log_always(IR_LOG_TAG"%s, line(%d)\n"fmt, __FUNCTION__,__LINE__, ##arg);\
	}while (0)
#endif

#if 1
#define IR_LOG_KEY(fmt, arg...)  \
	do { \
		 mtk_ir_core_log_always(""fmt, ##arg);\
	}while (0)
#endif



#if 0
#define IR_LOG_ALWAYS(fmt, arg...)  \
	do { \
		 mtk_ir_core_log_always(IR_LOG_TAG""fmt, ##arg);\
	}while (0)
#endif


#define IR_LOG_TO_KERNEL(fmt, arg...)  \
		do { \
			printk(IR_LOG_TAG"%s, line =%d\n"fmt, __FUNCTION__,__LINE__, ##arg);	 \
		}while (0)
	 	
extern void Assert(const char* szExpress, const char* szFile, int i4Line);

#undef ASSERT
#define ASSERT(x)		((x) ? (void)0 : Assert(#x, __FILE__, __LINE__))
#else     // usr space use

#define IR_LOG_TAG      "[mt8127_ir_usr]"

#define IR_LOG_ALWAYS(fmt, arg...) \
			 printf(IR_LOG_TAG"%s,line(%d)\n"fmt, __FUNCTION__, __LINE__, ##arg)

extern void Assert(const char* szExpress, const char* szFile, int i4Line);

#define ASSERT(x)        ((x) ? (void)0 : Assert(#x, __FILE__, __LINE__))

#endif



#endif
