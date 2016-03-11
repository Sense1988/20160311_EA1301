#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
extern void CMGR_SetDualPCMSCOAcceptValue(U8 value);
extern void MeWriteDualPCMSetValue(U8 *value, U16 len);
extern void MeWriteDualPCMSwitch(U8 *value, U16 len);

void BTSetSCOAcceptSettingCnf(void)
{
    bt_set_sco_accept_cnf_struct *cnf_p = NULL;
    cnf_p = construct_local_para(sizeof(bt_set_sco_accept_cnf_struct), TD_UL);
    cnf_p->value = 0;
    BT_SendMessage(MSG_ID_BT_SET_SCO_ACCEPT_CNF, MOD_MMI, cnf_p, sizeof(bt_set_sco_accept_cnf_struct));
}

void BTSetDualPCMSettingCnf(void)
{
    BT_SendMessage(MSG_ID_BT_SET_DUAL_PCM_SETTING_CNF, MOD_MMI, NULL, 0);
}

void BTSetDualPCMSwitchCnf(void)
{
    BT_SendMessage(MSG_ID_BT_SET_DUAL_PCM_SWITCH_CNF, MOD_MMI, NULL, 0);
}

void BTSetSCOAcceptSetting(U8 setting)
{
    CMGR_SetDualPCMSCOAcceptValue(setting);
    BTSetSCOAcceptSettingCnf();
}

void BTSetDualPCMSetting(U8 *value, U16 len)
{
    MeWriteDualPCMSetValue(value, len);
}

void BTSetDualPCMSwitch(U8 *value, U16 len)
{
    MeWriteDualPCMSwitch(value, len);
}
#endif

