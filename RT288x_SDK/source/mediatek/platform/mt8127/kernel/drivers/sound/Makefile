ifeq ($(MTK_PMIC_MT6397),yes)
	obj-$(CONFIG_MTK_SOUND) += AudDrv_Kernel.o AudDrv_Clk.o AudDrv_Afe.o AudDrv_Ana_6397.o yusu_android_speaker.o
else
	obj-$(CONFIG_MTK_SOUND) += AudDrv_Kernel.o AudDrv_Clk.o AudDrv_Afe.o AudDrv_Ana_6323.o yusu_android_speaker.o
endif
