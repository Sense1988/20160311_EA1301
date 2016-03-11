#!/bin/sh

if [ ! -d "lib" ]; then
	tar zxvf lib.tgz
	rm lib.tgz
fi

if [ ! -d "uClibc++-0.2.4" ]; then
	tar zxvf uClibc++-0.2.4.tgz
	rm uClibc++-0.2.4.tgz
fi

if [ ! -d "uClibc-0.9.33.2" ]; then
	tar zxvf uClibc-0.9.33.2.tgz
	rm uClibc-0.9.33.2.tgz
fi

if [ ! -d "uClibc++" ]; then
	tar zxvf uClibc++.tgz
	rm uClibc++.tgz
fi

if [ ! -f /usr/bin/flex ]; then
	sudo apt-get install flex
fi

if [ "$1" = "1" ]; then
	make menuconfig
elif [ "$1" = "2" ]; then
	make dep;make
else
        echo "Usage:mybuild.sh [1|2]"
	echo "./mybuild.sh 1 [make menuconfig]"
	echo "./mybuild.sh 2 [make dep;make]"
fi
