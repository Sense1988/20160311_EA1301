#!/bin/sh
cp ../../images/*_uImage root_uImage
cp ../../../../Uboot/uboot.bin .
make -f Makefile.8M
