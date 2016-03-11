#!/bin/sh

if [ "$1" = "1" ]; then
	make menuconfig
elif [ "$1" = "2" ]; then
	make
elif [ "$1" = "3" ]; then
	make distclean
else
        echo "Usage:mybuild.sh [1|2|3]"
	echo "./mybuild.sh 1 [make menuconfig]"
	echo "./mybuild.sh 2 [make]"
	echo "./mybuild.sh 3 [make distclean]"
fi
