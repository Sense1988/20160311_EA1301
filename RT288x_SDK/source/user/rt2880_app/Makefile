ra_dir_$(CONFIG_RALINKAPP_REG_RW)	+= reg
ra_dir_$(CONFIG_RALINKAPP_ATED)		+= ated_MT7615
#ra_dir_$(CONFIG_RALINKAPP_CSR)		+= csr_test
ra_dir_$(CONFIG_RALINKAPP_FLASH)	+= flash
ra_dir_$(CONFIG_RALINKAPP_HWNAT)	+= hw_nat
#ra_dir_$(CONFIG_RALINKAPP_SWNAT)	+= sw_nat
ra_dir_$(CONFIG_RALINKAPP_MII_MGR)	+= mii_mgr
ra_dir_$(CONFIG_RALINKAPP_NVRAM)	+= nvram
ra_dir_$(CONFIG_RALINKAPP_GPIO)		+= gpio
ra_dir_$(CONFIG_RALINKAPP_SPI)		+= spicmd
ra_dir_$(CONFIG_RALINKAPP_I2C)		+= i2ccmd
ra_dir_$(CONFIG_RALINKAPP_SCRIPTS)	+= scripts
ra_dir_$(CONFIG_RALINKAPP_QDMA)         += qdma 
#ra_dir_$(CONFIG_RALINKAPP_MEMORY_USAGE)	+= mem_usage
ra_dir_$(CONFIG_RALINKAPP_SWITCH)	+= switch
ra_dir_$(CONFIG_RALINKAPP_I2S)		+= i2scmd
ra_dir_$(CONFIG_RALINKAPP_SPDIF)        += spdifcmd
ra_dir_$(CONFIG_RALINKAPP_PCM)		+= pcmcmd
#ra_dir_$(CONFIG_RALINKAPP_PCIE_PHY)	+= pcie_phy
#ra_dir_$(CONFIG_RALINKAPP_CLS)		+= clscmd
ra_dir_$(CONFIG_RALINKAPP_MPLAYER)	+= mplayer
#ra_dir_$(CONFIG_RALINKAPP_SUPERDMZ)	+= super_dmz
#ra_dir_$(CONFIG_RALINKAPP_SWQOS)	+= sw_qos
#ra_dir_$(CONFIG_RALINKAPP_HWQOS)	+= hw_qos
#ra_dir_$(CONFIG_RALINKAPP_SWQOS)	+= qos
#ra_dir_$(CONFIG_RALINKAPP_HWQOS)	+= qos
#ra_dir_$(CONFIG_RALINKAPP_WAPI)		+= wapi
#ra_dir_$(CONFIG_RALINKAPP_PLCFG)	+= plconfig
#ra_dir_$(CONFIG_RALINKAPP_PLCFG)	+= plc_test
ra_dir_$(CONFIG_RALINKAPP_AUTO_WAN)	+= auto_wan
ra_dir_$(CONFIG_RALINKAPP_BUSARB)	+= busarb
ra_dir_$(CONFIG_RALINKAPP_ETHPHY)	+= ethphy
#ra_dir_$(CONFIG_RALINKAPP_KPROFILE)	+= kprofile
#ra_dir_$(CONFIG_UFSD_TOOL)		+= ufsd_tools
ra_dir_$(CONFIG_RALINKAPP_HOTSPOT)	+= hotspot
ra_dir_$(CONFIG_USER_NFCSD)		+= nfc
ra_dir_$(CONFIG_USER_NFCHOD)		+= handover
ra_dir_$(CONFIG_USER_BLUEANGEL)	        += blueangel
#ra_dir_$(CONFIG_USER_ROUTER_PLAYER)	+= btut
ra_dir_$(CONFIG_RALINKAPP_NVRAM_TEST)	+= nvram_test
ra_dir_$(CONFIG_RALINKAPP_NVRAM_TEST2)	+= nvram_test2
#ra_dir_$(CONFIG_USER_MET)       	+= met
#ra_dir_$(CONFIG_RALINKAPP_LONG_LOOP)    += long_loop
#ra_dir_$(CONFIG_USER_AB151X_TOOL)       += ab151xd
#ra_dir_$(CONFIG_RALINKAPP_DUMP_MT7530_PHY) += dump_mt7530_phy
#ra_dir_$(CONFIG_RALINKAPP_SMART_EEE)    += smart_eee
#ra_dir_$(CONFIG_BLUEANGEL)		+= blueangel_apsoc

#ra_dir_y += eip93_apps/SLAD_TestTool/build

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(ra_dir_y) ; do make -C $$i clean ; done

