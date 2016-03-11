GEMINI=no
BUILD_KERNEL = yes
BUILD_PRELOADER=no
BUILD_UBOOT=no
BUILD_LK=no
MTK_INTERNAL = yes
MTK_LDVT_SUPPORT=yes
MTK_AUTO_SANITY=no
BUILD_TINY_ANDROID = true
# For audio flinger's customization folder
CUSTOM_HAL_AUDIOFLINGER=audio

# User space cameara flashlight driver.You can use this driver to choose cameara flashlight type.
CUSTOM_HAL_FLASHLIGHT=constant_flashlight

# User space image sensor driver. Define  project used all image sensors. The value is combination of CUSTOM_HAL_MAIN_IMGSENSOR, CUSTOM_HAL_MAIN_BACKUP_IMGSENSOR, CUSTOM_HAL_SUB_IMGSENSOR, and CUSTOM_HAL_SUB_BACKUP_IMGSENSOR
CUSTOM_HAL_IMGSENSOR=ov5647_raw ov2659_yuv

# = CUSTOM_HAL_MAIN_LENS +CUSTOM_HAL_MAIN_BACKUP_LENS + CUSTOM_HAL_SUB_LENS + CUSTOM_HAL_SUB_BACKUP_LENS
CUSTOM_HAL_LENS = fm50af sensordrive dummy_lens

CUSTOM_HAL_MAIN2_IMGSENSOR=
CUSTOM_HAL_MAIN_BACKUP_IMGSENSOR=
CUSTOM_HAL_SUB_LENS = dummy_lens

# lens driver config for main camera (2nd solution)
CUSTOM_HAL_MAIN_BACKUP_LENS = sensordrive

# User space image sensor  driver: Main camera (rear camera) used sensor related tuning, setting and calibration information.Value is used main sensor name.
CUSTOM_HAL_MAIN_IMGSENSOR=ov5647_raw

# lens driver config for main camera
CUSTOM_HAL_MAIN_LENS = fm50af
CUSTOM_HAL_SUB_BACKUP_IMGSENSOR=

# lens driver config for video telephony camera (2nd solution)
CUSTOM_HAL_SUB_BACKUP_LENS=

# User space driver: Sub camera (front camera )used sensor related tuning, setting and calibration information. Value is used sub sensor name.
CUSTOM_HAL_SUB_IMGSENSOR=ov2659_yuv

# accelerometer sensor to detect accelerometer from x y z axis.
CUSTOM_KERNEL_ACCELEROMETER=mpu6050g

# ALSPS sensor driverto detect ambint light and the object is close or far awary from device
CUSTOM_KERNEL_ALSPS=APDS9930

# The battery feature is the MUST include feature and can not be disable.This feature provides battery monitor and charging. The system can not boot up without battery.
CUSTOM_KERNEL_BATTERY=battery

# Kernel space cameara flashlight driver. You can use this driver to choose cameara flashlight type.
CUSTOM_KERNEL_FLASHLIGHT=constant_flashlight

# gyro driver
CUSTOM_KERNEL_GYROSCOPE=mpu6050gy

# Kernel space image sensor driver. Define  project used all image sensors .The value is combination of CUSTOM_KERNEL_MAIN_IMGSENSOR, CUSTOM_KERNEL_MAIN_BACKUP_IMGSENSOR, CUSTOM_KERNEL_SUB_IMGSENSOR, and CUSTOM_KERNEL_SUB_BACKUP_IMGSENSOR
CUSTOM_KERNEL_IMGSENSOR=ov5647_raw ov2659_yuv

# same as CUSTOM_HAL_LENS
CUSTOM_KERNEL_LENS = fm50af sensordrive dummy_lens

# compass driver to detect compass raw data and report orientatino data
CUSTOM_KERNEL_MAGNETOMETER=akm09911
CUSTOM_KERNEL_MAIN2_BACKUP_IMGSENSOR=
CUSTOM_KERNEL_MAIN2_IMGSENSOR=

# Kernel space image sensor  driver:Main camera (rear camera) used backup sensor driver.Value is used main backup sensor name.
CUSTOM_KERNEL_MAIN_BACKUP_IMGSENSOR=

# lens driver config for main camera (2nd solution)
CUSTOM_KERNEL_MAIN_BACKUP_LENS=

# Kernel space image sensor driver:Main camera (rear camera) used sensor driver.Value is used main sensor name.
CUSTOM_KERNEL_MAIN_IMGSENSOR=ov5647_raw

# lens driver config for main camera
CUSTOM_KERNEL_MAIN_LENS = fm50af

# For audio kernel driver's speaker customization folder definition.
CUSTOM_KERNEL_SOUND=amp_6323pmic_spk
CUSTOM_KERNEL_SUB_BACKUP_IMGSENSOR=

# lens driver config for video telephony camera (2nd solution)
CUSTOM_KERNEL_SUB_BACKUP_LENS=

# Kernel space image sensor driver:Sub camera (front camera) used sensor driver.Value is used sub sensor name.
CUSTOM_KERNEL_SUB_IMGSENSOR=ov2659_yuv

# touch driver  need to report correct touch axes
CUSTOM_KERNEL_TOUCHPANEL=GT911

# To choose LK LCM driver name
CUSTOM_LK_LCM=CPT_CLAP070WP03XG_LVDS

# Add these variables to define the default input method and default input method languages.
DEFAULT_LATIN_IME_LANGUAGES=en-US fr ru
DISABLE_EARPIECE=yes
MODEM_NFC_SUPPORT=yes

# yes: enable this feature
# no: disable this feature
MTK_AAL_SUPPORT=yes
MTK_ACWFDIALOG_APP=yes
MTK_APKINSTALLER_APP=yes

# This feature enables switching between different audio tracks of multi-audio track multimedia file.
MTK_AUDIO_CHANGE_SUPPORT=yes
MTK_BEAM_PLUS_SUPPORT=yes
MTK_BICR_SUPPORT=yes

# BQ24158 charger support
MTK_BQ24158_SUPPORT=no
MTK_DATAUSAGELOCKSCREENCLIENT_SUPPORT=no
MTK_ENABLE_MD1=no

# FM short antenna feature option: For FM Radio, when there's no headset,  if short antenna is available,  FM app will switch to short antenna automaticlly
#  For FM Transmitter, short antenna is the default transmit antenna. If target PCB provides short antenna, please set this compile option to "yes", or else set to "no"
MTK_FM_SHORT_ANTENNA_SUPPORT=no

# define FM TX type digital or analog
MTK_FM_TX_AUDIO=FM_DIGITAL_OUTPUT
MTK_GPU_SUPPORT=yes
MTK_HEADSET_ICON_SUPPORT=no
MTK_HW_ENHANCE=yes

# Support build modem database file into system image under the path: "system/etc/mddb".
MTK_INCLUDE_MODEM_DB_IN_IMAGE=no
MTK_INTERNAL_HDMI_SUPPORT=yes

# Instant Power On is a software solution, device can boot up in a configurable time without extra hardware cost
#  It leverages the Android suspend feature to create a new user experience.
MTK_IPO_SUPPORT=no

# it has two values - 1 or 2. 1 stands for google default lockscreen, which we drag the lock bar to right, then the phone can be unlocked. 2 stands for SlideLockScreen,  which we slide the screen up, then the phone can be unlocked.
MTK_LOCKSCREEN_TYPE=1

# The feature can provide a picutre is like 3D picture. You can wave device left and right then can see the picture display multi-Angle for this image
MTK_MAV_SUPPORT=no

# It's used to control MDLogger which output the Modem Log and Memory dump information.
MTK_MDLOGGER_SUPPORT=no

# enable/disable ogm and mtkps playback feature
MTK_MTKPS_PLAYBACK_SUPPORT=yes

# Control NEON HW support or not
MTK_NEON_SUPPORT=yes
MTK_NFC_ADDON_SUPPORT=yes
MTK_NFC_APP_SUPPORT=yes

# enable smartcardservice
# If MTK_NFC_OMAAC_GEMALTO is set as yes, OMAAC for GEMALTO is supported
MTK_NFC_OMAAC_GEMALTO=yes

# enable smartcardservice
# If MTK_NFC_OMAAC_SUPPORTis set as yes, OMAAC is supported.
MTK_NFC_OMAAC_SUPPORT=yes

# We use this option (MTK_NFC_SE_NUM) to know the number of secure elements and the type of secure elements for the projects with NFC.
MTK_NFC_SE_NUM=0

# NFC stands for near field communication. It is a short-range wireless communication technology, which can be used in data exchange, information download, payment application, etc
MTK_NFC_SUPPORT=yes

# enable/disable ogm and mtkps playback feature
MTK_OGM_PLAYBACK_SUPPORT=yes

# It is the feature option of a apk called Omacp whose source code path is:  alps/mediatek/source/packages/Omacp.  OMA CP is Client Provisioning, and it is used to modify the application's settings over push message usually from the operator, such as browser's homepage and bookmark, add apn settings, add email account and so on.
MTK_OMACP_SUPPORT=no

# Define hardware platfrom
MTK_PLATFORM=MT8590
MTK_POST_PROCESS_FRAMEWORK_SUPPORT=no

# To control resource build except locales. The value would be assigned to PRODUCT_LOCALES under build/target/product/XXX.mk.
MTK_PRODUCT_AAPT_CONFIG = hdpi

# "yes" means enable QQbrowser feature and "no" disable.
MTK_QQBROWSER_SUPPORT = no

# This feature option is to define if we support features for RAT WCDMA PREFERRED network mode MTK_RAT_WCDMA_PREFERRED = yes means that RAT WCDMA PREFERRED network mode is enabled in current project, vice versa
MTK_RAT_WCDMA_PREFERRED=no

# Define release package for differernt codebase of different customer.(Different customer may get different number of source files)
MTK_RELEASE_PACKAGE=rel_customer_basic rel_customer_platform_mt8590 rel_customer_tablet
MTK_RESOURCE_OPTIMIZATION=

# sensor config to sensor port sensor feature in project
MTK_SENSOR_SUPPORT=yes

# This feature option is to define if we support features for SIM Recovery
MTK_SIM_RECOVERY=no
MTK_SIP_SUPPORT=yes
MTK_SMARTSWITCH_SUPPORT=no

# Gallery video display can support internal / external subtitle
MTK_SUBTITLE_SUPPORT=yes
MTK_SWCHR_SUPPORT=no
MTK_IPO_POWERPATH_SUPPORT = no

MTK_BATLOWV_NO_PANEL_ON_EARLY = yes
MTK_SYSTEM_UPDATE_SUPPORT=yes

# separate Phone and Tablet Dram parameter
MTK_TABLET_DRAM=yes
MTK_TABLET_HARDWARE=mt8590

# use MTK_TABLET_PLATFORM to select MDL file
MTK_TABLET_PLATFORM=no

# 1. wifi_only : only supports WIFI, not support 2G/3G Data link,2. 3gdata_only: supports WIFI & 3G data link, but not support 3G telephony & sms functions, 3. 3gdata_sms: supports WIFI, 3G data link, & 3G sms, but not support telephony function,4. 3g_full : supports WIFI, 3G data link, & 3G telephony & sms functions.
MTK_TB_WIFI_3G_MODE=WIFI_ONLY

# support MTK's speech enhancement for VoIP phone call.
MTK_VOIP_ENHANCEMENT_SUPPORT=yes

# Define the wlan chip type
MTK_WLAN_CHIP=

# multi-channel audio playback support
MULTI_CH_PLAYBACK_SUPPORT=yes
NO_INIT_PERMISSION_CHECK=yes

# use to differentiate the project UI which use the some density resource . Just like the QHD resolution and WQVA resolution , they all use the hdpi resources, if you want to support this two projects ,  you do not need two code base ,you can use  RESOURCE_OVERLAY_SUPPORT  = hdpi to differentiate the QHD and WQVA in one code base. It could overlay the resources in build stage.
RESOURCE_OVERLAY_SUPPORT=generic navbar tablet generic_WIFIONLY
USE_OPENGL_RENDERER=true

# Support Fast Dormancy or not
MTK_FD_SUPPORT=no

# For Platform: MT6573, MT6575 If it is equal to "modem_2g", means only support 2g modem. If it is equal to "modem_3g", means  support 2g and 3g modem
MTK_MODEM_SUPPORT=no

# enable Video telephony
MTK_VT3G324M_SUPPORT=no

# Used in FM driver and Native lib makefiles to decide which chip driver will be build
MTK_FM_CHIP=MT6627_FM

# FM transmitter switch: If "no" FM TX related features will be removed
#   if "yes", FM TX feature will be built in system.
MTK_FM_TX_SUPPORT=no

# To control whether enable or disable HDMI feature. If choose yes,  phone's screen can be demonstrated on TV via HDMI cable.
MTK_HDMI_SUPPORT=yes

# to choose which boot logo folder would be used, the boot logo folder path is mediatek/custom/common/uboot/logo/
BOOT_LOGO=wxga

# To choose kernel LCM driver name
CUSTOM_KERNEL_LCM=CPT_CLAP070WP03XG_LVDS

# To choose uboot LCM driver name
CUSTOM_UBOOT_LCM=CPT_CLAP070WP03XG_LVDS

# To set LCM resolution height size
LCM_HEIGHT=1280

# To set LCM resolution width size
LCM_WIDTH=800

# To control resource build. The value would be assigned to PRODUCT_LOCALES under build/target/product/XXX.mk (XXX is each project's name)
MTK_PRODUCT_LOCALES=en_US es_ES zh_CN zh_TW ru_RU pt_BR fr_FR de_DE tr_TR it_IT in_ID ms_MY vi_VN ar_EG hi_IN th_TH bn_IN pt_PT ur_PK fa_IR nl_NL el_GR hu_HU tl_PH ro_RO cs_CZ iw_IL my_MM km_KH ko_KR

# mATV audio path mode selection: analog path:  MTK_MATV_ANALOG_SUPPORT = yes I2S path: MTK_MATV_ANALOG_SUPPORT = no
MTK_MATV_ANALOG_SUPPORT=yes
MTK_IN_HOUSE_TEE_SUPPORT=no
MTK_DRM_KEY_MNG_SUPPORT=no

MTK_HUIYOU_GAMEHALL_APP = no


MTK_HUIYOU_LOVEFISHING_APP = no


MTK_HUIYOU_SYJT_APP = no


MTK_HUIYOU_WABAOJINGYING_APP = no

MTK_PUMP_EXPRESS_SUPPORT = yes

CUSTOM_KERNEL_SUB_LENS = dummy_lens

#FIX-ME early porting
HAVE_AEE_FEATURE = no
MTK_AUTO_SANITY = no
MTK_LDVT_SUPPORT=yes
MTK_ENABLE_MD1 = no