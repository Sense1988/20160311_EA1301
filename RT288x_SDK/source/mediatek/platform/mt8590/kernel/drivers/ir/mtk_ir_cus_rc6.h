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


static struct rc_map_table mtk_rc6_table[] = {
	 
	{0x00, KEY_0},	
	{0x01, KEY_1},
    {0x02, KEY_2},   
    {0x03, KEY_3},
    {0x04, KEY_4},
    {0x05, KEY_5},
    {0x06, KEY_6},
    {0x07, KEY_7},
    {0x08, KEY_8},
    {0x09, KEY_9},	  
    
	{0x5a, KEY_LEFT}, 
	{0x5b, KEY_RIGHT},
	{0x58, KEY_UP},	
	{0x59, KEY_DOWN},	
	{0x5c, KEY_ENTER},	
	
		 	
	{0xc7, KEY_POWER}, 
	{0x4c, KEY_HOMEPAGE}, 
	{0x83, KEY_BACKSPACE},
	{0x92, KEY_BACK},	
	
};



