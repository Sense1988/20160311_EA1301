# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.
#
# MTK Build System
#
BUILD_KERNEL = yes
	# default settings: yes for target; else no (e.g. emulator)
	# candidate settings: yes; no
	# used for MTK build system

BUILD_PRELOADER = yes
	# default settings: yes for target; else no (e.g. emulator)
	# candidate settings: yes; no
	# used for MTK build system

BUILD_UBOOT = no
	# default settings: yes for target; else no (e.g. emulator)
	# candidate settings: yes; no
	# used for MTK build system
BUILD_LK = yes

#CUSTOM_MODEM = mt6577_hspa_gemini
#
# Customize for PRELOADER
#


#
# Customize for UBOOT
#
CUSTOM_UBOOT_LCM = CPT_CLAP070WP03XG_LVDS 
CUSTOM_LK_LCM = CPT_CLAP070WP03XG_LVDS 
LCM_WIDTH = 800

LCM_HEIGHT = 1280
BOOT_LOGO=wxga

# Customize for HAL
#
CUSTOM_HAL_AUDIOFLINGER = audio

CUSTOM_HAL_BLUETOOTH = bluetooth

CUSTOM_HAL_CAMERA = camera

CUSTOM_HAL_EEPROM = dummy_eeprom

CUSTOM_HAL_FLASHLIGHT = constant_flashlight

# temp solution for custom folder
CUSTOM_HAL_IMGSENSOR = ov5647_raw ov2659_yuv
CUSTOM_KERNEL_IMGSENSOR = ov5647_raw ov2659_yuv

CUSTOM_KERNEL_LENS = fm50af sensordrive dummy_lens
CUSTOM_KERNEL_MAIN_LENS = fm50af
CUSTOM_KERNEL_MAIN_BACKUP_LENS = 
CUSTOM_KERNEL_SUB_LENS = dummy_lens
CUSTOM_KERNEL_SUB_BACKUP_LENS =

CUSTOM_HAL_LENS = fm50af sensordrive dummy_lens
CUSTOM_HAL_MAIN_LENS = fm50af
CUSTOM_HAL_MAIN_BACKUP_LENS = sensordrive
CUSTOM_HAL_SUB_LENS = dummy_lens
CUSTOM_HAL_SUB_BACKUP_LENS =


CUSTOM_HAL_MAIN_IMGSENSOR = ov5647_raw
CUSTOM_HAL_MAIN2_IMGSENSOR= 

CUSTOM_HAL_MAIN_BACKUP_IMGSENSOR = 

CUSTOM_HAL_SUB_IMGSENSOR = ov2659_yuv

CUSTOM_HAL_SUB_BACKUP_IMGSENSOR =


#
# Customize for Kernel
#
#CUSTOM_KERNEL_HEADSET = accdet


CUSTOM_KERNEL_CAMERA = camera

CUSTOM_KERNEL_BATTERY = battery

#CUSTOM_KERNEL_SOUND = amp_dummy
CUSTOM_KERNEL_SOUND = amp_6323pmic_spk

CUSTOM_KERNEL_CORE = src
	# default settings: src
	# candidate settings: src
	# board configuation. .

CUSTOM_KERNEL_DCT = dct

CUSTOM_KERNEL_EEPROM = dummy_eeprom

CUSTOM_KERNEL_FLASHLIGHT = constant_flashlight

#CUSTOM_KERNEL_JOGBALL = evqwjn

CUSTOM_KERNEL_KPD = kpd
	# default settings: kpd
	# candidate settings: kpd
	# For Keypad

CUSTOM_KERNEL_LCM = CPT_CLAP070WP03XG_LVDS 

CUSTOM_KERNEL_LEDS = mt65xx

# Disable HWUI on FPGA


#CUSTOM_KERNEL_OFN =


CUSTOM_KERNEL_MAIN_IMGSENSOR = ov5647_raw

CUSTOM_KERNEL_MAIN2_IMGSENSOR = 

CUSTOM_KERNEL_MAIN_BACKUP_IMGSENSOR = 

CUSTOM_KERNEL_MAIN2_BACKUP_IMGSENSOR =

CUSTOM_KERNEL_SUB_IMGSENSOR = ov2659_yuv

CUSTOM_KERNEL_SUB_BACKUP_IMGSENSOR =

CUSTOM_KERNEL_TOUCHPANEL = GT9XXTB_hotknot
	# cy8ctma300 GT818B-fpga
	# default settings: generic 
	# candidate settings: generic ;eeti_pcap7200
	# select the panel used by certain project.

CUSTOM_KERNEL_USB = mt6577

#CUSTOM_KERNEL_WIFI =

CUSTOM_KERNEL_RTC = rtc
        # default settings: rtc
        # candidate settings: rtc
        # For RTC

# Android sensor device
MTK_SENSOR_SUPPORT = yes

CUSTOM_KERNEL_MAGNETOMETER = akm09911

CUSTOM_KERNEL_ACCELEROMETER = mpu6050g

CUSTOM_KERNEL_ALSPS = APDS9930

CUSTOM_KERNEL_GYROSCOPE = mpu6050gy

CUSTOM_HAL_SENSORS = sensor

#
# Misc
#
EVB = no
	# default settings: yes for evb; else no
	# candidate settings: yes/no
	# For EVB used

#GEMINI = no
	# default settings: no
	# candidate settings: yes/no
	# For gemini;currently no projects is set gemini on

#MTK_GEMINI_ENHANCEMENT = no
	# default settings: no
	# candidate settings: yes/no
	# If project could use gemini modem, this value should be yes

#OPTR_SPEC_SEG_DEF = NONE 
	# default settings: NONE

#
# MTK Specified Feature
#
#HAVE_AEE_FEATURE = yes



HAVE_MATV_FEATURE = no

MTK_CHIP_VER = S01
	# default settings: S01
	# candidate settings: S01
	# S01 for MT6516

MTK_FM_CHIP = MT6627_FM
	# candidate settings: AR1000_FM/MT6616_E3_FM/MT6620_FM

# MTK_ATV_CHIP = MTK_MT5193

MTK_WLAN_CHIP = 


MTK_PLATFORM=MT8127
	# default settings: MT6516
	# candidate settings: MT6516
	# MT6516 is available

#
# MTK APP
#

MTK_INPUTMETHOD_PINYINIME_APP = no


MTK_CAMERA_APP = no

MTK_ENGINEERMODE_APP = yes


MTK_LIVEWALLPAPER_APP = yes


MTK_FMRADIO_APP = yes


MTK_ACWFDIALOG_APP = yes



MTK_DM_APP = no

MTK_ANDROIDFACTORYMODE_APP = yes



#
# MTK SUPPORT
#

MTK_TVOUT_SUPPORT = no



MTK_FM_SUPPORT = yes

MTK_FM_TX_SUPPORT = no

MTK_FM_SHORT_ANTENNA_SUPPORT = no




MTK_WAPPUSH_SUPPORT = yes

MTK_WLANBT_SINGLEANT = no


MTK_LCM_PHYSICAL_ROTATION = 0 
  # set this value as 90 if the lcm is physically rotated on the device

MTK_SCREEN_OFF_WIFI_OFF = no


MTK_M4U_SUPPORT = yes


MTK_LOCKSCREEN_TYPE = 1
  # default=1, slide=2

MTK_NETWORK_TYPE_DISPLAY = no

MTK_MATV_ANALOG_SUPPORT = yes
   # For I2S, use: no
   # For Audio Line-in, use: yes

# add for IME feature






MTK_HEADSET_ICON_SUPPORT = no

MTK_OMA_DOWNLOAD_SUPPORT = yes

MTK_SMARTSWITCH_SUPPORT = no

#RESOURCE_OVERLAY_SUPPORT = generic navbar tablet 

#MTK_OMACP_SUPPORT = yes




MTK_SEARCH_DB_SUPPORT = yes

MTK_DIALER_SEARCH_SUPPORT = yes

MTK_WIFI_P2P_SUPPORT = yes














MTK_FILEMANAGER_APP = yes


MTK_NAND_PAGE_SIZE = 2K

# Enable dual mic support
MTK_DUAL_MIC_SUPPORT = no



MTK_MFV_MPEG4_EXTRA = yes

MTK_VLW_APP = yes

#MTK_MODEM_SUPPORT = modem_3g
#MTK_MODEM_SUPPORT=

MTK_RESOURCE_OPTIMIZATION =


MTK_INTERNAL = yes

MTK_THEMEMANAGER_APP = no


MTK_CALENDAR_IMPORTER_APP = yes



MTK_DSPIRDBG = no

MTK_SEC_USBDL = ATTR_SUSBDL_ONLY_ENABLE_ON_SCHIP
MTK_SEC_CHIP_SUPPORT = yes

MTK_SEC_BOOT = ATTR_SBOOT_ONLY_ENABLE_ON_SCHIP

MTK_SEC_MODEM_ENCODE = no

MTK_SEC_MODEM_AUTH = no

MTK_FOTA_SUPPORT = no


MTK_PHONE_VT_VOICE_ANSWER = no

MTK_PHONE_VOICE_RECORDING = yes



#MTK_FD_SUPPORT = yes
MTK_FD_SUPPORT = no


MTK_DM_ENTRY_DISPLAY = no


MTK_DRM_APP = no


HAVE_XLOG_FEATURE = yes



MTK_RTP_OVER_RTSP_SUPPORT = yes

MTK_SPECIAL_FACTORY_RESET = no

# CUSTOM_HAL_MATV = matv

# CUSTOM_KERNEL_MATV = mt5193

MTK_MAV_SUPPORT = no




MTK_CAMERA_BSP_SUPPORT = yes


MTK_GEMINI_3G_SWITCH = no

MTK_DITHERING_SUPPORT = yes

MTK_WEATHER_PROVIDER_APP = yes

#MTK_LOG2SERVER_APP = yes


MTK_AUTORAMA_SUPPORT = yes

MTK_PRODUCT_LOCALES=en_US es_ES zh_CN zh_TW ru_RU pt_BR fr_FR de_DE tr_TR it_IT in_ID ms_MY vi_VN ar_EG hi_IN th_TH bn_IN pt_PT ur_PK fa_IR nl_NL el_GR hu_HU tl_PH ro_RO cs_CZ iw_IL my_MM km_KH ko_KR


#MTK_USES_STAGEFRIGHT_DEFAULT_CODE = no
#MTK_USE_ANDROID_MM_DEFAULT_CODE = yes

MTK_AUDIO_BLOUD_CUSTOMPARAMETER_REV = MTK_AUDIO_BLOUD_CUSTOMPARAMETER_V4

MTK_WB_SPEECH_SUPPORT = yes

HAVE_AWBENCODE_FEATURE = yes

HAVE_AACENCODE_FEATURE = yes

MTK_MULTI_STORAGE_SUPPORT = yes

MTK_BICR_SUPPORT=yes

MTK_EMULATOR_SUPPORT = no


MTK_SHARE_MODEM_SUPPORT = 2



#MTK_SHARE_MODEM_CURRENT=1
#MTK_SHARE_MODEM_SUPPORT=1




MTK_EAP_SIM_AKA = no

MTK_NFC_SUPPORT = yes

MTK_NEW_IPTABLES_SUPPORT = yes

MTK_ASD_SUPPORT = yes

MTK_EMMC_SUPPORT = yes

MTK_FM_RECORDING_SUPPORT = yes


MTK_USES_HD_VIDEO = yes


MTK_AVI_PLAYBACK_SUPPORT = yes









MTK_TB_APP_LANDSCAPE_SUPPORT = no


HAVE_VORBISENC_FEATURE = yes

MTK_BRAZIL_CUSTOMIZATION_TIM = no

#MTK_FD_FORCE_REL_SUPPORT = yes





MTK_BRAZIL_CUSTOMIZATION = no

MTK_BRAZIL_CUSTOMIZATION_CLARO = no


MTK_BRAZIL_CUSTOMIZATION_VIVO = no





MTK_HDMI_SUPPORT = yes

MTK_HDMI_HDCP_SUPPORT = yes

MTK_INTERNAL_HDMI_SUPPORT = yes

MTK_INTERNAL_MHL_SUPPORT = no


MTK_FOTA_ENTRY = no

MTK_SCOMO_ENTRY = no

MTK_OGM_PLAYBACK_SUPPORT = yes

MTK_MTKPS_PLAYBACK_SUPPORT = yes



MTK_SEND_RR_SUPPORT = yes


MTK_RAT_WCDMA_PREFERRED = no


MTK_FACEBEAUTY_SUPPORT = yes


MTK_FM_RX_SUPPORT = yes


MTK_FM_TX_AUDIO = FM_DIGITAL_OUTPUT


MTK_FM_RX_AUDIO = FM_DIGITAL_INPUT

MTK_DEFAULT_DATA_OFF = no


DEFAULT_LATIN_IME_LANGUAGES = en-US fr ru

CUSTOM_KERNEL_VIBRATOR = vibrator

CUSTOM_SEC_SIGNTOOL_SUPPORT = no

CUSTOM_SEC_AUTH_SUPPORT = no


MTK_TABLET_PLATFORM = no

MTK_TB_APP_CALL_FORCE_SPEAKER_ON = no


#MTK_FAN5405_SUPPORT = yes
MTK_BQ24158_SUPPORT = yes



MTK_NEON_SUPPORT = yes



NO_INIT_PERMISSION_CHECK=yes
MTK_GPU_SUPPORT = yes

MTK_ION_SUPPORT = yes

MTK_SWCHR_SUPPORT = yes

MTK_IPO_POWERPATH_SUPPORT = no

MTK_BATLOWV_NO_PANEL_ON_EARLY = yes
MTK_APKINSTALLER_APP = yes
MTK_SMSREG_APP = no
MTK_RELEASE_PACKAGE = rel_customer_basic rel_customer_platform_mt8127 rel_customer_tablet



MULTI_CH_PLAYBACK_SUPPORT = yes 




DISABLE_EARPIECE = yes

MTK_VOICE_UI_SUPPORT = yes 

MTK_TABLET_DRAM=yes


MTK_TELEPHONY_MODE = 0

MTK_PRODUCT_AAPT_CONFIG = hdpi

MTK_HW_ENHANCE = yes  

MTK_IPO_SUPPORT = no
MTK_IPOH_SUPPORT = no

MTK_AUDIO_CHANGE_SUPPORT = yes 


MTK_NFC_SE_NUM = 0

MTK_BEAM_PLUS_SUPPORT = yes  

MTK_NFC_ADDON_SUPPORT = yes 

MTK_NFC_APP_SUPPORT = yes

MTK_NFC_OMAAC_GEMALTO = yes 

MTK_NFC_OMAAC_SUPPORT = yes 

MODEM_NFC_SUPPORT = yes

MTK_DATAUSAGE_SUPPORT = yes 


MTK_DATAUSAGELOCKSCREENCLIENT_SUPPORT = no 

MTK_AAL_SUPPORT = yes

MTK_SIP_SUPPORT = yes

MTK_SYSTEM_UPDATE_SUPPORT = yes

MTK_VOIP_ENHANCEMENT_SUPPORT = yes

MTK_MOBILE_MANAGEMENT = yes 

MTK_PERMISSION_CONTROL = yes

MTK_SAFEMEDIA_SUPPORT = yes 

MTK_AUDIO_APE_SUPPORT = yes

MTK_TABLET_HARDWARE = mt8127

MTK_SUBTITLE_SUPPORT = yes

MTK_ENABLE_MD1=no

# Support build modem database file into system image under the path: "system/etc/mddb".
MTK_INCLUDE_MODEM_DB_IN_IMAGE=no

# It's used to control MDLogger which output the Modem Log and Memory dump information.
MTK_MDLOGGER_SUPPORT=no

# It is the feature option of a apk called Omacp whose source code path is:  alps/mediatek/source/packages/Omacp.  OMA CP is Client Provisioning, and it is used to modify the application's settings over push message usually from the operator, such as browser's homepage and bookmark, add apn settings, add email account and so on.
MTK_OMACP_SUPPORT=no

# This feature option is to define if we support features for RAT WCDMA PREFERRED network mode MTK_RAT_WCDMA_PREFERRED = yes means that RAT WCDMA PREFERRED network mode is enabled in current project, vice versa
MTK_RAT_WCDMA_PREFERRED=no

# This feature option is to define if we support features for SIM Recovery
MTK_SIM_RECOVERY=no

# if MTK_SMSREG_APP=yes,build SmsReg.apk
MTK_SMSREG_APP=no

# 1. wifi_only : only supports WIFI, not support 2G/3G Data link,2. 3gdata_only: supports WIFI & 3G data link, but not support 3G telephony & sms functions, 3. 3gdata_sms: supports WIFI, 3G data link, & 3G sms, but not support telephony function,4. 3g_full : supports WIFI, 3G data link, & 3G telephony & sms functions.
MTK_TB_WIFI_3G_MODE=WIFI_ONLY

# For TTY function enable option.
MTK_TTY_SUPPORT=no

# use to differentiate the project UI which use the some density resource . Just like the QHD resolution and WQVA resolution , they all use the hdpi resources, if you want to support this two projects ,  you do not need two code base ,you can use  RESOURCE_OVERLAY_SUPPORT  = hdpi to differentiate the QHD and WQVA in one code base. It could overlay the resources in build stage.
RESOURCE_OVERLAY_SUPPORT=generic navbar tablet generic_WIFIONLY

# Define which modem will be used.
CUSTOM_MODEM=

# For Platform: MT6573, MT6575 If it is equal to "modem_2g", means only support 2g modem. If it is equal to "modem_3g", means  support 2g and 3g modem
MTK_MODEM_SUPPORT=no

# enable Video telephony
MTK_VT3G324M_SUPPORT=no

# Support GEMINI or not
GEMINI=no

# Enable or disable the GEMINI enhancement
MTK_GEMINI_ENHANCEMENT=no

# Extend our Gemini feature to support daul RIL with single modem logistic.
MTK_SHARE_MODEM_CURRENT=1

MTK_EAP_SIM_AKA=no


MTK_DATAUSAGE_SUPPORT = yes 


MTK_DATAUSAGELOCKSCREENCLIENT_SUPPORT = no 

MTK_GEMINI_3G_SWITCH = no

MTK_GEMINI_SMART_3G_SWITCH = 0

MTK_HWUI_SUPPORT = yes
MTK_HWC_SUPPORT = yes
USE_OPENGL_RENDERER = true
MTK_MOBILE_MANAGEMENT = yes 

MTK_PMIC_MT6397 = yes
MTK_EXTERNAL_LDO = no




MTK_QQBROWSER_SUPPORT = no

MTK_POST_PROCESS_FRAMEWORK_SUPPORT = no
MTK_IN_HOUSE_TEE_SUPPORT=yes
MTK_DRM_KEY_MNG_SUPPORT=yes
MTK_SHARED_SDCARD = yes
MTK_OWNER_SDCARD_ONLY_SUPPORT = yes

MTK_HUIYOU_GAMEHALL_APP = no


MTK_HUIYOU_LOVEFISHING_APP = no


MTK_HUIYOU_SYJT_APP = no

MTK_VIDEO_HEVC_SUPPORT=yes

MTK_HUIYOU_WABAOJINGYING_APP = no

MTK_HOTKNOT_SUPPORT = yes

MTK_EXTERNAL_DONGLE_SUPPORT = yes

MTK_MULTI_WINDOW_SUPPORT = no 

MTK_DOLBY_DAP_SUPPORT = yes

MTK_AUDIO_DDPLUS_SUPPORT = yes

MTK_MALI_UNIV = yes

MTK_MPEG_DASH_SUPPORT = no
