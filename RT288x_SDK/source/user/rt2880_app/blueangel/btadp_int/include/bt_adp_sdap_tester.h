
#ifndef __BT_ADP_SDAP_TESTER_H__
#define __BT_ADP_SDAP_TESTER_H__

typedef struct {
	U32 cmd;
	U8 sm_count;
	BD_ADDR bdAddr;
	SdpQueryToken local_sqt;
	sdap_service_search_all_struct sa_search_pattern;
	sdap_service_search_multi_attribute_struct sm_search_pattern;
} SdapTesterParams;

#endif
