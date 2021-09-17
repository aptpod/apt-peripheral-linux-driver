#include "mock_ep1_ch02a.h"
#include "ep1_ch02a/ep1_ch02a_cmd_def.h"

static struct mock_ep1_ch02a_response_get_status mock_data_response_ep1_ch02a_get_status = {
	.header.sob = APT_USBTRX_MSG_SOB,
	.header.length = sizeof(struct mock_ep1_ch02a_response_get_status),
	.header.cmd = EP1_CH02A_CMD_ResponseGetStatus,

	/* mock data */
	.payload.silent = 0x00,
	.payload.start = 0x00,
	.payload.baudrate = 0x00,
	.payload.enable_sync = 0x00,

	.footer.eob = APT_USBTRX_MSG_EOB,
};

void set_mock_ep1_ch02a_get_status(const struct mock_ep1_ch02a_response_get_status_payload *payload)
{
	mock_data_response_ep1_ch02a_get_status.payload = *payload;
}

void build_response_ep1_ch02a_get_status(struct mock_ep1_ch02a_response_get_status *resp_data, apt_usbtrx_msg_t *msg)
{
	*resp_data = mock_data_response_ep1_ch02a_get_status;
	msg->id = EP1_CH02A_CMD_ResponseGetStatus;
	msg->payload_size = sizeof(struct mock_ep1_ch02a_response_get_status_payload);
	memcpy(msg->payload, &resp_data->payload, msg->payload_size);
}

static struct mock_ep1_ch02a_set_bit_timing_payload mock_data_request_set_bit_timing = { 0, 0, 0, 0, 0 };

void mock_request_ep1_ch02a_set_bit_timing(const void *data, int data_size)
{
	memcpy(&mock_data_request_set_bit_timing, data + 3, data_size - 4);
}

void get_mock_ep1_ch02a_set_bit_timing_payload(struct mock_ep1_ch02a_set_bit_timing_payload *payload)
{
	*payload = mock_data_request_set_bit_timing;
}

static struct mock_ep1_ch02a_response_get_bit_timing mock_data_response_ep1_ch02a_get_bit_timing = {
	.header.sob = APT_USBTRX_MSG_SOB,
	.header.length = sizeof(struct mock_ep1_ch02a_response_get_bit_timing),
	.header.cmd = EP1_CH02A_CMD_ResponseGetBitTiming,

	/* mock data */
	.payload.can_clock = 7500000,
	.payload.prop_seg = 8,
	.payload.phase_seg1 = 3,
	.payload.phase_seg2 = 2,
	.payload.sjw = 4,
	.payload.brp = 4,

	.footer.eob = APT_USBTRX_MSG_EOB,
};

void build_response_ep1_ch02a_get_bit_timing(struct mock_ep1_ch02a_response_get_bit_timing *resp_data,
					     apt_usbtrx_msg_t *msg)
{
	*resp_data = mock_data_response_ep1_ch02a_get_bit_timing;
	msg->id = EP1_CH02A_CMD_ResponseGetBitTiming;
	msg->payload_size = sizeof(struct mock_ep1_ch02a_response_get_bit_timing_payload);
	memcpy(msg->payload, &resp_data->payload, msg->payload_size);
}
