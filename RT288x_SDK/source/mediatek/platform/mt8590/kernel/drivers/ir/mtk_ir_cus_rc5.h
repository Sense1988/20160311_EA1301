/* rc-dvb0700-big.c - Keytable for devices in dvb0700
 *
 * Copyright (c) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * TODO: This table is a real mess, as it merges RC codes from several
 * devices into a big table. It also has both RC-5 and NEC codes inside.
 * It should be broken into small tables, and the protocols should properly
 * be indentificated.
 *
 * The table were imported from dib0700_devices.c.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <media/rc-map.h>


static struct rc_map_table mtk_rc5_table[] = {
	{0x00, KEY_X}, 
	{0x01, KEY_ESC}, 
	{0x02, KEY_W}, 
	{0x03, KEY_LEFT}, 
	{0x04, KEY_SELECT},
	{0x05, KEY_RIGHT}, 
	{0x06, KEY_VOLUMEDOWN}, 
	{0x07, KEY_VOLUMEUP}, 
	{0x0e, KEY_POWER}, 
	{0x4c, KEY_HOMEPAGE}, 
	{0x5c, KEY_ENTER}, 
};

