/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#include "hid.h"
#include "bluetooth_hid_struct.h"
#include "bttypes.h"
#include "hidi.h"
#include "bt_os_api.h"
#include "bt_mmi.h"
#include <math.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define HID_KEY_RESERVE	2
#define HID_KEY_PRESS 		1
#define HID_KEY_RELEASE	0
#define HID_POINTER_X	3
#define HID_POINTER_Y	4
#define HID_WHEEL	5

#define KEYBOARD_USAGEPAGE   0x07
#define BUTTON_USAGEPAGE   0x09
#define GENERIC_DESKTOP_USAGEPAGE   0x01

//#define MOUSE_SUPPORT 

typedef struct
{
	U16	usage;
	U8	keyStatus;
}keyEvent;

char	ItemSize[4] = {0,1,2,4};

void  ShowBytes( void *  s,  int  n)
{
    U8 *  start  =  (U8* )s;
	int i=0;
	bt_prompt_trace(MOD_BT,"[HID][OFFSET] ADDRESS: VALUE\n\n");
 
 for  (  i  =   0 ; i  <  n; i ++ )
 {
        bt_prompt_trace(MOD_BT, "[HID][%4d] %8x: %2x\n ",i,start+i,*(start+i));
  
  if  ((i  +   1 )  %   4   ==   0 )
  {
           bt_prompt_trace(MOD_BT, " ----------------------\n " );
        } 
    } // for 
 } 


U8 Hid_Init_Parser(HidParseData*	tmpHidParser)
{
	//HidParseData*	tmpHidParser = (HidParseData*)hid_malloc(sizeof(HidParseData));
	if(!tmpHidParser)
	{
		bt_prompt_trace(MOD_BT,"[HID]HidParseData hid_malloc error");
		return 0;
	}
	
	tmpHidParser->reportDesc = NULL;
	tmpHidParser->usageQueue = (HidUsage*)hid_malloc(HID_USAGE_MAX * sizeof(HidUsage));
	if(!tmpHidParser->usageQueue)
	{
		bt_prompt_trace(MOD_BT,"[HID]usageQueue hid_malloc error");
		return 0;
	}

	btmtk_os_memset((U8*) tmpHidParser->usageQueue, 0, sizeof(HID_USAGE_MAX * sizeof(HidUsage)));

	tmpHidParser->usageSize = 0;
	tmpHidParser->reportDescLength = 0;
	tmpHidParser->logicalMAX = 0;
	tmpHidParser->logicalMIN = 0;
	tmpHidParser->physicalMAX = 0;
	tmpHidParser->physicalMIN = 0;
	tmpHidParser->reportCount = 0;
	tmpHidParser->reportSize = 0;
	tmpHidParser->reportID = 0;
	tmpHidParser->usagePage = 0;
	tmpHidParser->unitExponent = 0;
	tmpHidParser->unit = 0;

	tmpHidParser->pos = 0;
	//return tmpHidParser;
	return 1;
}

void Hid_Reset_UsageQueue(HidParseData  *hpd)
{
	hpd->usageSize = 0;
	btmtk_os_memset((U8*) hpd->usageQueue, 0 ,HID_USAGE_MAX *sizeof(HidUsage));
}

S32 Hid_Format_Value(S32 value, U8 size)
{
	if(size == 1)
		value = (S32)(S8)value;
	else if(size == 2)
		value = (S32)(S16)value;
	return value;
}

void Hid_Set_Usage_Page(HidParseData *hpd)
{
	if((hpd->item & SIZE_MASK) >2)
		hpd->usageQueue[hpd->usageSize].usagepage = (U16)(hpd->value >>16);
	else
		hpd->usageQueue[hpd->usageSize].usagepage = hpd->usagePage;
}
	
BtStatus Hid_SdpParse(HidChannel *channel, HidParseData  *hpd)
{
	U16		reportOffset[HID_REPORT_ID_MAX][3] = {{0}};
	U16		i;

	if(!channel || !hpd)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty Channel or Parser data");
		return BT_STATUS_FAILED;
	}			
	
	//channel->reportCons = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
	if(!channel->reportCons)
	{
		bt_prompt_trace(MOD_BT,"[HID]reportCons hid_malloc error");
		return BT_STATUS_FAILED;
	}
	
	hpd->reportDesc = (U8*)hid_malloc(channel->queryRsp.descriptorLen);
	if(!hpd->reportDesc)
	{
		bt_prompt_trace(MOD_BT,"[HID]reportDesc hid_malloc error");
		return BT_STATUS_FAILED;
	}
	btmtk_os_memset(hpd->reportDesc, 0 ,channel->queryRsp.descriptorLen);
	btmtk_os_memcpy(hpd->reportDesc, channel->queryRsp.descriptorList, channel->queryRsp.descriptorLen);
	hpd->reportDescLength = channel->queryRsp.descriptorLen;
	//btmtk_os_memset((U8*) channel->reportCons, 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
	for(i=0; i<HID_REPORT_ID_MAX; i++)
	{
		channel->reportCons[i].reportID = 0;
		//btmtk_os_memset((U8*) channel->reportCons[i].reportFormat, 0, sizeof(HidReportFormat*));
		channel->reportCons[i].reportFormat = NULL;
	}
	
	while(hpd->pos < hpd->reportDescLength)
	{
		hpd->item = hpd->reportDesc[hpd->pos++];
		hpd->value = 0;
		btmtk_os_memcpy((U8*)&hpd->value, &hpd->reportDesc[hpd->pos], ItemSize[hpd->item & SIZE_MASK]);
		//Pos on next item
		hpd->pos += ItemSize[hpd->item & SIZE_MASK];

		switch(hpd->item & ITEM_MASK)
		{
			case	ITEM_UPAGE:
			{
				hpd->usagePage = (U16)hpd->value;
				break;
			}
			case ITEM_USAGE:
			{
				if(hpd->usageSize == HID_USAGE_MAX)
				{					
					bt_prompt_trace(MOD_BT,"[HID]realloc usageQueue");
					hpd->usageQueue=(HidUsage*)realloc(hpd->usageQueue, 2*HID_USAGE_MAX * sizeof(HidUsage));
					if(!hpd->usageQueue)
					{
						bt_prompt_trace(MOD_BT,"[HID]usageQueue realloc error");
						break;
					}
				}
				Hid_Set_Usage_Page(hpd);
				if ( hpd->usageQueue )  //klocwork warning.
				{
    				hpd->usageQueue[hpd->usageSize].usageID.usageIDValue= (U16)(hpd->value & 0xFFFF);
	    			hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_SINGLE;
    			}
				hpd->usageSize++;
				break;
			}
			case ITEM_USAGE_MIN:
			{
				Hid_Set_Usage_Page(hpd);
				if ( hpd->usageQueue )  //klocwork warning.
				{
    				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN = (U16)(hpd->value & 0xFFFF);
	    			hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_RANGE;
		    		hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN_Set = TRUE;
    				if(hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX_Set == TRUE)
	    				hpd->usageSize++;
				}
				break;
			}
			case ITEM_USAGE_MAX:
			{
				Hid_Set_Usage_Page(hpd);
                if ( hpd->usageQueue )  //klocwork warning.
                {
    				hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX = (U16)(hpd->value & 0xFFFF);
	    			hpd->usageQueue[hpd->usageSize].usageIDType = HID_USAGE_ID_RANGE;
		    		hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMAX_Set = TRUE;
			    	if(hpd->usageQueue[hpd->usageSize].usageID.usageRange.usageMIN_Set == TRUE)
				    	hpd->usageSize++;
		    	}
				break;
			}
			case ITEM_COLLECTION:
			case ITEM_END_COLLECTION:
			{
				Hid_Reset_UsageQueue(hpd);
				break;
			}
			case ITEM_REP_ID:
			{
				hpd->reportID = (U8)hpd->value;
				break;
			}
			case ITEM_REP_COUNT:
			{
				hpd->reportCount = (U16)hpd->value;
				break;
			}
			case ITEM_REP_SIZE:
			{
				hpd->reportSize = (U16)hpd->value;
				break;
			}
			case ITEM_LOG_MIN:
			{
				hpd->logicalMIN = Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_LOG_MAX:
			{
				hpd->logicalMAX = Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_PHY_MIN:
			{
				hpd->physicalMIN= Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_PHY_MAX:
			{
				hpd->physicalMAX= Hid_Format_Value(hpd->value,ItemSize[hpd->item & SIZE_MASK]);
				break;
			}
			case ITEM_UNIT:
			{
				hpd->unit = hpd->value;
				break;
			}
			case ITEM_UNIT_EXP:
			{
				hpd->unitExponent = hpd->value;
				break;
			}
			case ITEM_INPUT:
			case ITEM_OUTPUT:
			case ITEM_FEATURE:
			{
				U8 	formatIndex = 0;
				if(channel->reportCons[hpd->reportID].reportID != hpd->reportID)
				{
					channel->reportCons[hpd->reportID].reportID = hpd->reportID;

					channel->reportCons[hpd->reportID].reportFormat = (HidReportFormat*)hid_malloc(3 * sizeof(HidReportFormat));
					if(!channel->reportCons[hpd->reportID].reportFormat)
					{
						bt_prompt_trace(MOD_BT,"[HID]reportFormat hid_malloc error");
						return BT_STATUS_FAILED;
					}
					
					btmtk_os_memset((U8*) channel->reportCons[hpd->reportID].reportFormat, 0, 3 * sizeof(HidReportFormat));
				}
			
				if((hpd->item & ITEM_MASK) == ITEM_INPUT)
				{
					formatIndex = 0;
				}
				else if((hpd->item & ITEM_MASK) == ITEM_OUTPUT)
				{
					formatIndex = 1;
				}
				else if((hpd->item & ITEM_MASK) == ITEM_FEATURE)
				{
					formatIndex = 2;
				}
				
				if(channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportType == HID_REPORT_OTHER)
				{
					if(formatIndex == 0)
						channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_INPUT;
					else if(formatIndex == 1)
						channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_OUTPUT;
					else if(formatIndex == 2)
						channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportType = HID_REPORT_FEATURE;

					channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit = (HidReportUnit*)hid_malloc(HID_REPORT_UNIT_MAX_SIZE * sizeof(HidReportUnit));
					if(!channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit)
					{
						bt_prompt_trace(MOD_BT,"[HID]reportUnit hid_malloc error");
						return BT_STATUS_FAILED;
					}
					btmtk_os_memset((U8*) channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit, 0, HID_REPORT_UNIT_MAX_SIZE * sizeof(HidReportUnit));
					
				}					

				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage = (HidUsage*)hid_malloc(HID_USAGE_MAX * sizeof(HidUsage));
				if(!channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage)
				{
					bt_prompt_trace(MOD_BT,"[HID]reportUnit::hidUsage hid_malloc error");
					return BT_STATUS_FAILED;
				}				
				btmtk_os_memset((U8*)channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage, 0, HID_USAGE_MAX*sizeof(HidUsage));

				if(hpd->value & MAIN_ITEM_ARG_VAR)
					channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].arrayAttr = HID_VAR_ATTRIBUTE;	
				else
					channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].arrayAttr = HID_ARRAY_ATTRIBUTE;
				
				btmtk_os_memcpy((U8*)channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].hidUsage, (U8*)hpd->usageQueue, hpd->usageSize*sizeof(HidUsage));
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].usageSize = hpd->usageSize;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].logicalMIN= hpd->logicalMIN;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].logicalMAX= hpd->logicalMAX;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].physicalMIN= hpd->physicalMIN;							
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].physicalMAX= hpd->physicalMAX;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].unit= hpd->unit;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].unitExponent= hpd->unitExponent;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].offset = (reportOffset[hpd->reportID][formatIndex]) /8 ;
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].reportCount= hpd->reportCount; 						
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].reportUnit[channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum].reportSize= hpd->reportSize; 						
						
				reportOffset[hpd->reportID][formatIndex] += (hpd->reportCount) * (hpd->reportSize);
						
				channel->reportCons[hpd->reportID].reportFormat[formatIndex].unitNum++; 				
				Hid_Reset_UsageQueue(hpd);
				break;
			}
			default:
				break;
		}
	}
	return BT_STATUS_SUCCESS;
}

void Hid_Free_Parser(HidParseData *hpd)
{	
	hid_free(hpd->usageQueue);
	hid_free(hpd->reportDesc);
	hid_free(hpd);
}

 BtStatus Hid_Ary_Report_To_Driver(int fd, U8 usage, U8 reportSize)
{
	static keyEvent*keyAry;
	static U8 keyAryIndex = 0;
	static U8 firstAry = 0;
	U8	tmpUsage;
	static int	capsLockPress=1;
	static int	numLockPress=1;
	U8 	i;

	if(fd<0)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty hid fd");
		return BT_STATUS_FAILED;
	}
		
	if(firstAry == 0)
	{
		keyAry = (keyEvent*)hid_malloc(reportSize*sizeof(keyEvent));
		btmtk_os_memset((U8*)keyAry, 0 ,reportSize*sizeof(keyEvent));
		firstAry = 1;
	}

	
	if(keyAryIndex == 0)
	{
		for(i=0; i<reportSize; i++)
		{
			if(keyAry[i].usage != 0)
				keyAry[i].keyStatus = HID_KEY_RELEASE;
		}

	}
	

	bt_prompt_trace(MOD_BT,"[HID]Report Array Usage %x",usage);

	if(usage != 0)
	{
		for(i=0; i<reportSize; i++)
		{
			//find position
			if(usage == keyAry[i].usage)
			{
				keyAry[i].keyStatus=HID_KEY_RESERVE;
				break;
			}
		}
		//not find
		if(i == reportSize)
		{	i=keyAryIndex;
			while(keyAry[i].usage != 0)
				i++;
			keyAry[i].usage=usage;
			keyAry[i].keyStatus=HID_KEY_PRESS;
		}
			
	}

	keyAryIndex++;
	
	if(keyAryIndex == reportSize)
	{
		keyAryIndex = 0;
		for(i=0; i<reportSize; i++)
		{
			if(keyAry[i].usage != 0)
			{
				if(keyAry[i].keyStatus == HID_KEY_PRESS)
				{
					if(ioctl(fd,HID_KEY_PRESS, &keyAry[i].usage)<0)	
					{
						bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
						return BT_STATUS_FAILED;
					}
					
					switch(keyAry[i].usage)
					{
						case KEY_CAPS_LOCK_USAGE:
							tmpUsage=KEY_LEFT_SHIFT_USAGE;
							ioctl(fd,HID_KEY_PRESS, &tmpUsage);
							ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							if(capsLockPress%2 == 1)
							{
								ioctl(fd,HID_KEY_PRESS, &tmpUsage);
								ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							}
							capsLockPress++;
							break;
						case KEY_NUM_LOCK_USAGE:
							tmpUsage=KEY_LEFT_ALT_USAGE;
							ioctl(fd,HID_KEY_PRESS, &tmpUsage);
							ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							if(numLockPress%2 == 1)
							{
								ioctl(fd,HID_KEY_PRESS, &tmpUsage);
								ioctl(fd,HID_KEY_RELEASE, &tmpUsage);
							}
							numLockPress++;
							break;
						default:
							break;
					}
				}
				if(keyAry[i].keyStatus == HID_KEY_RELEASE)
				{
					if(ioctl(fd,HID_KEY_RELEASE, &keyAry[i].usage)<0)	
					{
						bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
						return BT_STATUS_FAILED;
					}
					keyAry[i].usage = 0;
				}
			}
		}
	}
		
	return BT_STATUS_SUCCESS;
}
 
U8 Find_Used_Usage(U8* usageArray, U8 usage, U8 reportSize)
{
	U16 i;
	for(i = 0; i<reportSize; i++)
	{
		if(usageArray[i] == usage)
			break;
	}
	return i;
}

BtStatus Hid_Var_Report_To_Driver(int fd,U8 usage, S16 value, U8 reportSize, U16 usagePage)
{
	static U8 *usageUsed;
	static U8 usageVarIndex = 0;
	static U8 firstVar = 0;
	if(firstVar == 0)
	{
		usageUsed = (U8*)hid_malloc(reportSize*sizeof(U8));
		btmtk_os_memset(usageUsed, 0 ,reportSize*sizeof(U8));
		firstVar=1;
	}
	if(fd<0)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty hid fd");
		return BT_STATUS_FAILED;
	}
	//bt_prompt_trace(MOD_BT,"[HID]Handle usagePage %x in Hid_Var_Report_To_Driver", usagePage);

	if(usagePage == KEYBOARD_USAGEPAGE || usagePage == BUTTON_USAGEPAGE)
	{
		if(value!=0)
		{
			bt_prompt_trace(MOD_BT,"[HID]Report Var Usage %x",usage);
			if(Find_Used_Usage(usageUsed, usage, reportSize) == reportSize)
			{
				usageUsed[usageVarIndex] = usage;
				usageVarIndex++;
				if(usageVarIndex == reportSize)
					usageVarIndex = 0;
			}

			if(ioctl(fd,HID_KEY_PRESS, &usage)<0)	
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return BT_STATUS_FAILED;
			}

		}	
		else if(value == 0)
		{
			if(Find_Used_Usage(usageUsed, usage, reportSize) < reportSize)
			{				
				usageUsed[Find_Used_Usage(usageUsed, usage, reportSize)] = 0;

				bt_prompt_trace(MOD_BT,"[HID]Find_Used_Usage ok");

	   			if(ioctl(fd,HID_KEY_RELEASE, &usage)<0)	
				{
					bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
					return BT_STATUS_FAILED;
				}
			}
		}
	}
	else if(usagePage == GENERIC_DESKTOP_USAGEPAGE)
	{
		//bt_prompt_trace(MOD_BT,"[HID]usage is %x, value is %d", usage, value);
		if(usage == POINTER_X_USAGE)
		{
			//value = value *2/3;
			if(ioctl(fd,HID_POINTER_X, &value)<0) 
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return BT_STATUS_FAILED;
			}
		}
		else if(usage == POINTER_Y_USAGE)
		{
			//value = value *2/3;
			if(ioctl(fd,HID_POINTER_Y, &value)<0) 
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return BT_STATUS_FAILED;
			}
		}
		else if(usage == WHEEL_USAGE)
		{
			if(ioctl(fd,HID_WHEEL, &value)<0) 
			{
				bt_prompt_trace(MOD_BT,"[HID]Fail hid ioctl");
				return BT_STATUS_FAILED;
			}
		}		
	}
	return BT_STATUS_SUCCESS;
}

S32 Hid_Interpreter(int fd,HidReportConstructor *hrc, U8 *data, U16 data_len,HidReportType reportType)
{
	U8	reportID;
	U8	unitIndex;
	U8	reportIndex;
	U8	*reportData;
	U8	*tmpReportData;
	U8	*data_8 = NULL;
	S16	data_16 = 0;
	S16	value;
	U8 	bitIndex;
	U8 	byteIndex;
	U32	maskBase;
	U8 	byteChange = 1;
	HidReportUnit*	tmpReportUnit;
	U8	usage=0;
	U8  returnUsage=0;
	int	size = 0;
//	U8	i = 0;

	//btmtk_os_memset(&tmpReportUnit, 0 , sizeof(tmpReportUnit));
	if(!hrc )
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty report constructor");
		return -1;
	}	
	if(!data)
	{
		bt_prompt_trace(MOD_BT,"[HID]Empty report data");
		return -1;
	}
	if(reportType != HID_REPORT_INPUT &&
		reportType != HID_REPORT_OUTPUT &&
		reportType != HID_REPORT_FEATURE)
	{
		bt_prompt_trace(MOD_BT,"[HID]Other report type");
		return -1;
	}
	reportData = (U8*)hid_malloc(data_len);
	tmpReportData = (U8*)hid_malloc(data_len);
	//&data_8 = (S8*)hid_malloc(sizeof(S8));
	//&data_16 = (S16*)hid_malloc(sizeof(S16));
	btmtk_os_memcpy(reportData, data, data_len);
	//bt_prompt_trace(MOD_BT,"[HID]---------reportData-----------");
	//ShowBytes(reportData,data_len);


	/*First 8 bits is report ID*/
	btmtk_os_memcpy(&reportID,reportData,1);

	//bt_prompt_trace(MOD_BT,"[HID]report ID=%d",reportID);
	//bt_prompt_trace(MOD_BT,"[HID]hrc[reportID].report ID=%d",hrc[reportID].reportID);
	if(reportID == 0 || hrc[reportID].reportID!=reportID)
	{	
		bt_prompt_trace(MOD_BT,"[HID]Invalid report ID");
		goto fail;
	}

	for(unitIndex = 0; unitIndex<hrc[reportID].reportFormat[reportType-1].unitNum; unitIndex++)
	{		
		tmpReportUnit=&hrc[reportID].reportFormat[reportType-1].reportUnit[unitIndex];
		//bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit=%x",tmpReportUnit);
		//bt_prompt_trace(MOD_BT,"[HID]tmpReportUnit->hidUsage[0].usagepage:%x",tmpReportUnit->hidUsage[0].usagepage);
		if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_NONE 
			|| !(tmpReportUnit->hidUsage[0].usagepage ==KEYBOARD_USAGEPAGE
#ifdef MOUSE_SUPPORT
				|| tmpReportUnit->hidUsage[0].usagepage ==BUTTON_USAGEPAGE
				|| tmpReportUnit->hidUsage[0].usagepage ==GENERIC_DESKTOP_USAGEPAGE
#endif
				))
		{
			bt_prompt_trace(MOD_BT,"[HID]empty usage or not keyboard/button/pointer data");
			continue;
		}
		//btmtk_os_memcpy(&tmpReportUnit, &hrc[reportID].reportFormat[reportType-1].reportUnit[unitIndex], sizeof(HidReportUnit));
		
		bitIndex = 0;
		byteIndex = 0;	
		byteChange = 1;
		size = (int)((tmpReportUnit->reportCount*tmpReportUnit->reportSize +8 -1) /8);
		maskBase=pow(2, tmpReportUnit->reportSize) -1;

		btmtk_os_memcpy(tmpReportData, reportData+1+tmpReportUnit->offset, size);
		//bt_prompt_trace(MOD_BT,"[HID]---------tmpReportData-----------");
		//ShowBytes(tmpReportData, size);
		for(reportIndex = 0; reportIndex<tmpReportUnit->reportCount; reportIndex++)
		{
			if(tmpReportUnit->reportSize>8)
			{			
				if(bitIndex>=8)
				{
					byteIndex+=bitIndex/8;
					bitIndex = 0;
					byteChange = 1;
					hid_free(data_8);
					data_8 = NULL;
				}
				if(byteChange == 1)
				{
					data_8 = (U8*)hid_malloc((tmpReportUnit->reportSize-1)/8+1);
					btmtk_os_memcpy(data_8, tmpReportData+byteIndex,((tmpReportUnit->reportSize-1)/8+1));
					byteChange = 0;
				}			
				
				if(reportIndex == 0)
				{
					//data_16 = (*data_8 & 0x00FF) | ((U16)((*(data_8+1)) & (0xFF << (8 - (tmpReportUnit->reportSize % 8)))) << (tmpReportUnit->reportSize % 8));
					int tmp_offset=((tmpReportUnit->reportSize % 8)==0?8:tmpReportUnit->reportSize % 8);
					//data_16 = ((U16)*data_8 << tmp_offset) | ((U16)((*(data_8+1)) & (0xFF << (8 - tmp_offset))) >> (8 - tmp_offset));
					data_16 = (U16)((*(data_8+1)) & (0xFF >> (8 - tmp_offset))) << 8 | *data_8;
					//bt_prompt_trace(MOD_BT,"[HID]---------[1]data_16=%x-----------",data_16);
				}
				else
				{
					//data_16 = (*data_8 & (0xFF >> (tmpReportUnit->reportSize % 8)) & 0x00FF) | ((U16)*(data_8+1) << (8 - (tmpReportUnit->reportSize % 8)));
				//	data_16 = ((U16)(*data_8 & (0xFF >> (tmpReportUnit->reportSize % 8))) << 8) | ((U16)*(data_8+1) & 0x00FF);
					data_16 = (*data_8 >> (tmpReportUnit->reportSize % 8)) | ((U16)*(data_8+1) << (8 - (tmpReportUnit->reportSize % 8)));
					//bt_prompt_trace(MOD_BT,"[HID]---------[2]data_16=%x-----------",data_16);
				}

				value = data_16;
				//bt_prompt_trace(MOD_BT,"[HID]---------value=%d-----------",value);
			}
			else 	if(tmpReportUnit->reportSize<=8)
			{			
				if(bitIndex>=8)
				{
					bitIndex = 0;
					byteIndex+=1;
					byteChange = 1;
					hid_free(data_8);
					data_8 = NULL;
				}
				if(byteChange == 1)
				{
					data_8 = (U8*)hid_malloc(1);
					btmtk_os_memcpy(data_8, tmpReportData+byteIndex,1);
					//bt_prompt_trace(MOD_BT,"[HID]---------data_8-----------");
					//ShowBytes(&data_8,1);
					byteChange = 0;
				}
				value = (*data_8&(maskBase<<bitIndex))>>bitIndex;
			}
			bitIndex+=tmpReportUnit->reportSize;
			if(value >= pow(2, tmpReportUnit->reportSize - 1))
				value -= pow(2, tmpReportUnit->reportSize);

/*
			if(value>tmpReportUnit->logicalMAX || value<tmpReportUnit->logicalMIN)
			{
				bt_prompt_trace(MOD_BT,"[HID]Wrong report data");
				bt_prompt_trace(MOD_BT,"[HID]Wrong report data is:-----------%d",value);

				continue;
			}
*/
			if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_ID_SINGLE)
			{
				if(tmpReportUnit->arrayAttr == HID_ARRAY_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[value -tmpReportUnit->logicalMIN].usageID.usageIDValue;
					Hid_Ary_Report_To_Driver(fd,usage, tmpReportUnit->reportCount);
				}
				else if(tmpReportUnit->arrayAttr == HID_VAR_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[reportIndex].usageID.usageIDValue;
					Hid_Var_Report_To_Driver(fd,usage, value, tmpReportUnit->reportCount, tmpReportUnit->hidUsage[0].usagepage);
				}
			}
			else if(tmpReportUnit->hidUsage[0].usageIDType==HID_USAGE_ID_RANGE)
			{					
				if(tmpReportUnit->arrayAttr == HID_ARRAY_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[0].usageID.usageRange.usageMIN + value -tmpReportUnit->logicalMIN;
					Hid_Ary_Report_To_Driver(fd,usage, tmpReportUnit->reportCount);
				}
				else if(tmpReportUnit->arrayAttr == HID_VAR_ATTRIBUTE)
				{
					usage=tmpReportUnit->hidUsage[0].usageID.usageRange.usageMIN + reportIndex;
					Hid_Var_Report_To_Driver(fd,usage, value, tmpReportUnit->reportCount, tmpReportUnit->hidUsage[0].usagepage);
				}
			}
			if(usage==KEY_NUM_LOCK_USAGE||usage==KEY_CAPS_LOCK_USAGE||usage==KEY_SCROLL_LOCK_USAGE)
			{
				returnUsage=usage;
			}
		}
	}
fail:
	//btmtk_os_memset(&tmpReportUnit, 0 , sizeof(tmpReportUnit));
	btmtk_os_memset(reportData, 0, data_len);
	btmtk_os_memset(tmpReportData, 0, data_len);
	hid_free(reportData);
	hid_free(tmpReportData);
	hid_free(data_8);
	tmpReportUnit=NULL;

	return returnUsage;
}

