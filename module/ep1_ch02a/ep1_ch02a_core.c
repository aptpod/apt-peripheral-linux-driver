// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include <linux/can/dev.h>

#include "../ap_ct2a/ap_ct2a_core.h" /* inherit from ap_ct2a */

#include "ep1_ch02a_core.h"
#include "ep1_ch02a_cmd_def.h"
#include "ep1_ch02a_msg.h"

/*!
 * @brief dispatch message
 */
int ep1_ch02a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	switch (msg->id) {
	case EP1_CH02A_CMD_ResponseGetBitTiming: {
		if (dev->rx_complete.id == EP1_CH02A_CMD_GetBitTiming) {
			DMSG("complete!, <id:0x%02x> length=%d", msg->id,
			     APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
			dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
			memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
			complete(&dev->rx_complete.complete);
		}
		break;
	}
	default:
		return apt_usbtrx_unique_can_dispatch_msg(dev, data, msg);
	}

	return RESULT_Success;
}
