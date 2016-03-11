
typedef struct _BT_AMP_HCI_COMMAND_CREATE_PHYSICAL_LINK
{
	U8 physical_link_hdl;
	U8 dedicated_amp_key_length;
	U8 dedicated_amp_key_type;
	U8 dedicated_amp_key[16];
}BT_AMP_HCI_COMMAND_CREATE_PHYSICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_ACCEPT_PHYSICAL_LINK
{
	U8 physical_link_hdl;
	U8 dedicated_amp_key_length;
	U8 dedicated_amp_key_type;
	U8 dedicated_amp_key[16];
}BT_AMP_HCI_COMMAND_ACCEPT_PHYSICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_DISCONNECT_PHYSICAL_LINK
{
	U8 physical_link_hdl;
	U8 reason;
}BT_AMP_HCI_COMMAND_DISCONNECT_PHYSICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_CREATE_LOGICAL_LINK
{
	U8 physical_link_hdl;
	U8 tx_flow_spec[16];
	U8 rx_flow_spec[16];
}BT_AMP_HCI_COMMAND_CREATE_LOGICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_ACCEPT_LOGICAL_LINK
{
	U8 physical_link_hdl;
	U8 tx_flow_spec[16];
	U8 rx_flow_spec[16];
}BT_AMP_HCI_COMMAND_ACCEPT_LOGICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_DISCONNECT_LOGICAL_LINK
{
	U16 logical_link_handle;
}BT_AMP_HCI_COMMAND_DISCONNECT_LOGICAL_LINK;

typedef struct _BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL
{
	U8 physical_link_handle;
	U8 tx_flow_spec_id;
}BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL;

typedef struct _BT_AMP_HCI_COMMAND_FLOW_SPEC_MODIFY
{
	U8 logical_link_handle;
	U8 tx_flow_spec;
	U8 rx_flow_spec;	
}BT_AMP_HCI_COMMAND_FLOW_SPEC_MODIFY;

typedef struct _BT_AMP_HCI_COMMAND_READ_LOCAL_AMP_ASSOC
{
	U8 physical_link_handle;
	U16 length_so_far;
	U16 max_remote_amp_assoc_length;	
}BT_AMP_HCI_COMMAND_READ_LOCAL_AMP_ASSOC;

typedef struct _BT_AMP_HCI_COMMAND_WRITE_REMOTE_AMP_ASSOC
{
	U8 physical_link_handle;
	U16 length_so_far;
	U16 amp_assoc_remaining_length;	
	U16 amp_assoc_fragment;
}BT_AMP_HCI_COMMAND_WRITE_REMOTE_AMP_ASSOC;


typedef struct _BT_AMP_HCI_EVENT_PHYSICAL_LINK_CANCEL
{
	U8 physical_link_handle;
	U8 tx_flow_spec_id;
}BT_AMP_HCI_EVENT_PHYSICAL_LINK_CANCEL;

typedef struct _BT_AMP_HCI_EVENT_PHYSICAL_LINK_COMPLETE
{
	U8 status;
	U8 physical_link_hdl;	
}BT_AMP_HCI_EVENT_PHYSICAL_LINK_COMPLETE;

typedef struct _BT_AMP_HCI_EVENT_CHANNEL_SELECTED
{
	U8 physical_link_hdl;	
}BT_AMP_HCI_EVENT_CHANNEL_SELECTED;

typedef struct _BT_AMP_HCI_EVENT_PHYSICAL_LINK_DISCONNECT
{
	U8 status;
	U8 physical_link_hdl;	
	U8 reason;
}BT_AMP_HCI_EVENT_PHYSICAL_LINK_DISCONNECT;

typedef struct _BT_AMP_HCI_EVENT_PHYSICAL_LINK_WARNING
{
	U8 physical_link_hdl;	
	U8 link_loss_reason;
}BT_AMP_HCI_EVENT_PHYSICAL_LINK_WARNING;


typedef struct _BT_AMP_HCI_EVENT_PHYSICAL_LINK_RECOVERY
{
	U8 physical_link_hdl;	
}BT_AMP_HCI_EVENT_PHYSICAL_LINK_RECOVERY;


typedef struct _BT_AMP_HCI_EVENT_LOGICAL_LINK_COMPLETE
{
	U8 status;
	U16 logical_link_hdl;
	U8 physical_link_hdl;	
	U8 tx_flow_spec_id;
}BT_AMP_HCI_EVENT_LOGICAL_LINK_COMPLETE;

typedef struct _BT_AMP_HCI_EVENT_LOGICAL_LINK_DISCONNECT
{
	U8 status;
	U16 logical_link_hdl;
	U8 reason;
}BT_AMP_HCI_EVENT_LOGICAL_LINK_DISCONNECT;


typedef struct _BT_AMP_HCI_EVENT_FLOW_STATUS_MODIFY
{
	U8 status;
	U16 con_hdl;
}BT_AMP_HCI_EVENT_FLOW_STATUS_MODIFY;

typedef struct _BT_AMP_HCI_EVENT_AMP_STATUS_CHANGE
{
	U8 status;
	U8 amp_Status;
}BT_AMP_HCI_EVENT_AMP_STATUS_CHANGE;

typedef struct _BT_AMP_HCI_EVENT_READ_BLOCK_SIZE
{
	U16 max_ack_data_packet_length;
	U16 data_block_length;
	U16 total_num_data_blocks;	
}BT_AMP_HCI_EVENT_READ_BLOCK_SIZE;

typedef struct _BT_AMP_HCI_EVENT_READ_LOCAL_AMP_INFO
{
	U8 amp_status;
	U32 total_bandwidth;
	U32 max_guarantee_bandwidth;
	U32 min_latency;
	U32 max_pdu_size;
	U8 controller_type;
	U16 pal_capability;
	U16 max_amp_assoc_length;
	U32 max_flush_timeout;
    U32 best_effort_flush_timeout;
}BT_AMP_HCI_EVENT_READ_LOCAL_AMP_INFO;

typedef struct _BT_AMP_HCI_EVENT_READ_LOCAL_AMP_ASSOC
{
	U8 physical_link_handle;
	U16 amp_assoc_remaining_length;
	U8 *amp_assoc_fragment;
}BT_AMP_HCI_EVENT_READ_LOCAL_AMP_ASSOC;

typedef struct _BT_AMP_HCI_EVENT_WRITE_REMOTE_AMP_ASSOC
{
	U8 physical_link_handle;
}BT_AMP_HCI_EVENT_WRITE_REMOTE_AMP_ASSOC;


