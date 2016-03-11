
#ifndef __BT_SDAP_TESTER_STRUCT_H__
#define __BT_SDAP_TESTER_STRUCT_H__

#define SDAP_TESTER_CMD_SA_2			1
#define SDAP_TESTER_CMD_SA_FULL			2
#define SDAP_TESTER_CMD_SM_2			3
#define SDAP_TESTER_CMD_SM_FULL			4
#define SDAP_TESTER_CMD_SA_SM			5
#define SDAP_TESTER_CMD_SA_SM_FULL		6
#define SDAP_TESTER_CMD_SA_OVERFLOW		7
#define SDAP_TESTER_CMD_SM_OVERFLOW		8

typedef bt_addr_struct bt_sdap_tester_addr_t;

/**********************************
 * SDAP Tester Message Structures
 **********************************/
/* MSG_ID_BT_SDAP_TESTER_CMD */
typedef struct {
	LOCAL_PARA_HDR
	U32 cmd;
	U8 addr[6];
} bt_sdap_tester_cmd_struct;

#endif // __BT_SDAP_TESTER_STRUCT_H__
