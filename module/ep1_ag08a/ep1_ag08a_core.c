// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/iio/buffer.h>

#include "ep1_ag08a_core.h"
#include "ep1_ag08a_msg.h"

static void ep1_ag08a_iio_data_copy(apt_usbtrx_msg_t *msg, ep1_ag08a_iio_data_t *priv)
{
	ep1_ag08a_payload_notify_analog_input_t *p = (ep1_ag08a_payload_notify_analog_input_t *)msg->payload;
	int exclude_size = sizeof(p->timestamp) + sizeof(p->channel);
	int data_size = msg->payload_size - exclude_size;

	if (data_size <= EP1_AG08A_IIO_DATA_SIZE) {
		memcpy(priv->buffer, &(p->data), data_size);
	} else {
		EMSG("invalid iio data size = %d", data_size);
	}
}

/*!
 * @brief dispatch message
 */
int ep1_ag08a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);

	switch (msg->id) {
	case EP1_AG08A_CMD_NotifyAnalogInput: {
		int if_type = atomic_read(&unique_data->if_type);

		if (if_type == EP1_AG08A_IF_TYPE_FILE) {
			bool empty;
			empty = apt_usbtrx_ringbuffer_is_empty(&dev->rx_data);
			apt_usbtrx_ringbuffer_write(&dev->rx_data, msg->payload, msg->payload_size);

			if (empty) {
				wake_up_interruptible(&dev->rx_data.wq);
			}
		} else if (if_type == EP1_AG08A_IF_TYPE_IIO) {
			struct iio_dev *indio_dev = unique_data->indio_dev;
			ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
			ep1_ag08a_payload_notify_analog_input_t *p =
				(ep1_ag08a_payload_notify_analog_input_t *)msg->payload;
			s64 time_ns;

			ep1_ag08a_iio_data_copy(msg, priv);

			if (priv->hw_timestamp) {
				time_ns = p->timestamp.ts_sec * NSEC_PER_SEC + p->timestamp.ts_usec * NSEC_PER_USEC;
			} else {
				time_ns = iio_get_time_ns(indio_dev);
			}
			iio_push_to_buffers_with_timestamp(indio_dev, priv->buffer, time_ns);
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
