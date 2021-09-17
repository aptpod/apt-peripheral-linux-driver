// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include "ep1_ag08a_core.h"
#include "ep1_ag08a_msg.h"

/*!
 * @brief dispatch message
 */
int ep1_ag08a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	//ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);

	switch (msg->id) {
	case EP1_AG08A_CMD_NotifyAnalogInput: {
		bool empty;
		empty = apt_usbtrx_ringbuffer_is_empty(&dev->rx_data);
		apt_usbtrx_ringbuffer_write(&dev->rx_data, msg->payload, msg->payload_size);

		if (empty) {
			wake_up_interruptible(&dev->rx_data.wq);
		}
		break;
	}
	case EP1_AG08A_CMD_ResponseGetStatus: {
		if (dev->rx_complete.id == EP1_AG08A_CMD_GetStatus) {
			DMSG("complete!, <id:0x%02x> length=%d", msg->id,
			     APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
			dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
			memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
			complete(&dev->rx_complete.complete);
		}
		break;
	}
	default:
		WMSG("msg in not support, <id:0x%02x>", msg->id);
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief write bulk callback
 */
void ep1_ag08a_write_bulk_callback(struct urb *urb)
{
	return;
}
