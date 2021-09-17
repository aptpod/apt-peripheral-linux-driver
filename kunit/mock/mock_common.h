#pragma once

#include "apt_usbtrx_msg.h"

struct mock_msg_header {
	u8 sob;
	u8 length;
	u8 cmd;
} __attribute__((packed));

struct mock_msg_footer {
	u8 eob;
} __attribute__((packed));

struct mock_msg_ack_nack {
	struct mock_msg_header header;
	u8 recv_cmd;
	struct mock_msg_footer footer;
} __attribute__((packed));

void create_mock_ack(struct mock_msg_ack_nack *mock, int cmd);

#define DEFINE_MOCK_MSG_STRUCT(name)                                                                                   \
	struct name {                                                                                                  \
		struct mock_msg_header header;                                                                         \
		struct name##_payload payload;                                                                         \
		struct mock_msg_footer footer;                                                                         \
	} __attribute__((packed))
