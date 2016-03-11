
#ifndef __MTK_IR_RECV_H__
#define __MTK_IR_RECV_H__

#include <media/lirc_dev.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/types.h>
#include "mtk_ir_common.h"



#define RC_MAP_MTK_NEC "rc_map_mtk_nec"
#define RC_MAP_MTK_RC6 "rc_map_mtk_rc6"
#define RC_MAP_MTK_RC5 "rc_map_mtk_rc5"



#define MTK_IR_DRIVER_NAME	"mtk_ir"

#define MTK_INPUT_NEC_DEVICE_NAME	"NEC_Remote_Controller" // here is for input device name 
#define MTK_INPUT_RC6_DEVICE_NAME	"RC6_Remote_Controller" // here is for input device name 
#define MTK_INPUT_RC5_DEVICE_NAME	"RC5_Remote_Controller" // here is for input device name 


#define SEMA_STATE_LOCK   (0)
#define SEMA_STATE_UNLOCK (1)

#define SEMA_LOCK_OK                    ((int)   0)
#define SEMA_LOCK_TIMEOUT               ((int)  -1)
#define SEMA_LOCK_FAIL                  ((int)  -2)


struct mtk_rc_core {

	int irq;          // ir irq number	
	bool irq_register; // whether ir irq has been registered
    struct rc_dev *rcdev;//  current rcdev
    struct lirc_driver *drv; // lirc_driver
    struct platform_device *dev_current; // current activing device    
    struct task_struct * k_thread ;   // input thread
   
};

struct mtk_ir_device
{
	struct list_head list;
	struct platform_device dev_platform;
};

struct mtk_ir_core_platform_data {
	
	const char * input_name;      // /proc/bus/devices/input/input_name
	struct rc_map_list *p_map_list;   // rc map list		
	int (*init_hw)(void);         // init ir_hw
	int (*uninit_hw)(void);       //uint ir_hw	
	u32 (*ir_hw_decode)(void * preserve); // decode function. preserve for future use

	void (*early_suspend)(void * preserve);
	void (*late_resume)(void * preserve);

	int (*suspend)(void * preserve);
	int (*resume)(void * preserve);	
		
};

extern struct mtk_rc_core mtk_rc_core;

extern struct mtk_ir_device mtk_ir_dev_nec;
extern struct mtk_ir_device mtk_ir_dev_rc6;
extern struct mtk_ir_device mtk_ir_dev_rc5;


extern struct list_head mtk_ir_device_list;

 
	 

extern void	release(struct device *dev);


extern int mtk_ir_core_create_thread (
							 int (*threadfn)(void *data),
							 void *data,
							 const char* ps_name,
							 struct task_struct ** p_struct_out,
							 unsigned int  ui4_priority );

extern void mtk_ir_core_send_scancode(u32 scancode);
extern void mtk_ir_core_send_mapcode(u32 mapcode, int stat);

extern void mtk_ir_core_send_mapcode_auto_up(u32 mapcode,u32 ms);

extern void mtk_ir_core_get_msg_by_scancode(u32 scancode, struct mtk_ir_msg *msg);
extern void mtk_ir_core_clear_hwirq_stat(void);



#endif
