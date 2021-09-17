#include "mock_ep1_ag08a.h"
#include "ep1_ag08a/ep1_ag08a_cmd_def.h"

struct mock_ep1_ag08a_response_get_status mock_data_response_get_status = {
	.header.sob = APT_USBTRX_MSG_SOB,
	.header.length = sizeof(struct mock_ep1_ag08a_response_get_status),
	.header.cmd = EP1_AG08A_CMD_ResponseGetStatus,

	/* mock data */
	.payload.in.cfg.send_rate = 0x01,
	.payload.in.cfg.input_voltage = { 0x00, 0x00, 0x01, 0x01, 0x01, 0x09, 0x09, 0x09 },
	.payload.in.ctrl.act = 0xA5,

	.payload.out.cfg.waveform_type = 0x01,
	.payload.out.cfg.voltage = 0xFA,
	.payload.out.cfg.frequency = 0x64,
	.payload.out.ctrl.act = 0x01,

	.footer.eob = APT_USBTRX_MSG_EOB,
};

void build_response_get_status(struct mock_ep1_ag08a_response_get_status *resp_data, apt_usbtrx_msg_t *msg)
{
	*resp_data = mock_data_response_get_status;
	msg->id = EP1_AG08A_CMD_ResponseGetStatus;
	msg->payload_size = sizeof(struct mock_ep1_ag08a_response_get_status_payload);
	memcpy(msg->payload, &resp_data->payload, msg->payload_size);
}
