#include "mock_common.h"

static struct mock_msg_ack_nack mock_data_response_ack_nack = {
	.header.sob = APT_USBTRX_MSG_SOB,
	.header.length = sizeof(struct mock_msg_ack_nack),
	.header.cmd = APT_USBTRX_CMD_ACK,

	/* mock data */
	.recv_cmd = 0x00,

	.footer.eob = APT_USBTRX_MSG_EOB,
};

void create_mock_ack(struct mock_msg_ack_nack *mock, int cmd)
{
	*mock = mock_data_response_ack_nack;
	mock->recv_cmd = cmd;
}
