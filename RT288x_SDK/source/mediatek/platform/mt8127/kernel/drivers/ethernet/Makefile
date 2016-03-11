#
# Makefile for linux/drivers/net/star
#

#EXTRA_CFLAGS = -D__KERNEL__ -DMODULE -O -Wall -I$(INCLUDEDIR)
#EXTRA_CFLAGS += -O2 -Werror
EXTRA_CFLAGS += -Werror
EXTRA_CFLAGS += -D USE_TX_TASKLET \
#               -D USE_RX_NAPI \
                -D USE_RX_TASKLET
#               -D USE_INTERNAL_PHY=1 \

obj-y += star_mac.o
star_mac-objs := star.o star_dev.o
		
