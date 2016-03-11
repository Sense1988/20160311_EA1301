
ccflags-y += -I$(MTK_PATH_PLATFORM)/drivers/hdmi/
             
ifeq ($(MTK_IN_HOUSE_TEE_SUPPORT), yes)
ccflags-y += -I$(srctree)/../mediatek/trustzone/mtk/include/ -Werror
endif

obj-y := hdmitx.o hdmi_drv.o hdmi_ctrl.o hdmiavd.o hdmicec.o hdmiddc.o hdmiedid.o hdmihdcp.o hdmicmd.o hdmictrl.o

ifeq ($(MTK_IN_HOUSE_TEE_SUPPORT), yes)
obj-y += hdmi_ca.o
endif

