// SPDX-License-Identifier: GPL-2.0
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/kthread.h>

#include "apt_usbtrx_def.h"
#include "apt_usbtrx_core.h"
#include "apt_usbtrx_msg.h"
#include "apt_usbtrx_cmd.h"
#include "apt_usbtrx_ringbuffer.h"

#include <linux/can/dev.h>

/*!
 * @brief dump message
 */
static void apt_usbtrx_dump_msg(apt_usbtrx_msg_t *msg)
{
	int n = 0;

	WMSG_RL("id  : 0x%02x", msg->id);
	WMSG_RL("size: %d", msg->payload_size);
	while (n < msg->payload_size) {
		int remain = msg->payload_size - n;

		if (remain >= 8) {
			WMSG_RL("data: %02x %02x %02x %02x - %02x %02x %02x %02x", msg->payload[n], msg->payload[n + 1],
				msg->payload[n + 2], msg->payload[n + 3], msg->payload[n + 4], msg->payload[n + 5],
				msg->payload[n + 6], msg->payload[n + 7]);
		} else {
			switch (remain) {
			case 0:
				break;
			case 1:
				WMSG_RL("data: %02x", msg->payload[n]);
				break;
			case 2:
				WMSG_RL("data: %02x %02x", msg->payload[n], msg->payload[n + 1]);
				break;
			case 3:
				WMSG_RL("data: %02x %02x %02x", msg->payload[n], msg->payload[n + 1],
					msg->payload[n + 2]);
				break;
			case 4:
				WMSG_RL("data: %02x %02x %02x %02x", msg->payload[n], msg->payload[n + 1],
					msg->payload[n + 2], msg->payload[n + 3]);
				break;
			case 5:
				WMSG_RL("data: %02x %02x %02x %02x - %02x", msg->payload[n], msg->payload[n + 1],
					msg->payload[n + 2], msg->payload[n + 3], msg->payload[n + 4]);
				break;
			case 6:
				WMSG_RL("data: %02x %02x %02x %02x - %02x %02x", msg->payload[n], msg->payload[n + 1],
					msg->payload[n + 2], msg->payload[n + 3], msg->payload[n + 4],
					msg->payload[n + 5]);
				break;
			case 7:
				WMSG_RL("data: %02x %02x %02x %02x - %02x %02x %02x", msg->payload[n],
					msg->payload[n + 1], msg->payload[n + 2], msg->payload[n + 3],
					msg->payload[n + 4], msg->payload[n + 5], msg->payload[n + 6]);
				break;
			}
		}

		n += 8;
	}
}

/*!
 * @brief get request command from response command
 */
static inline int get_request_command_from_response_command(int response)
{
	switch (response) {
	case APT_USBTRX_CMD_ResponseGetDeviceId:
		return APT_USBTRX_CMD_GetDeviceId;

	case APT_USBTRX_CMD_ResponseGetSerialNo:
		return APT_USBTRX_CMD_GetSerialNo;

	case APT_USBTRX_CMD_ResponseGetFWVersion:
		return APT_USBTRX_CMD_GetFWVersion;

	case APT_USBTRX_CMD_ResponseGetFWVersionRevision:
		return APT_USBTRX_CMD_GetFWVersionRevision;

	default:
		return APT_USBTRX_CMD_Unknown;
	}
}

/*!
 * @brief dispatch message
 */
STATIC int apt_usbtrx_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	bool rx_ongoing = atomic_read(&dev->rx_ongoing);
	if (!rx_ongoing) {
		return RESULT_Success;
	}

	switch (msg->id) {
	case APT_USBTRX_CMD_NotifyBufferStatus: {
		int rate;
		int result;

		result = apt_usbtrx_msg_parse_notify_buffer_status(msg->payload, msg->payload_size, &rate);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_msg_parse_notify_buffer_status().. Error");
			break;
		}
		atomic_set(&dev->tx_buffer_rate, rate);
		if (rate > APT_USBTRX_TX_TRANSFER_LIMIT_RATE) {
			WMSG("(%s-if%02d) buffer status:%d", dev->serial_no, dev->ch, rate);
		}
		break;
	}
	case APT_USBTRX_CMD_ResponseGetDeviceId:
	case APT_USBTRX_CMD_ResponseGetSerialNo:
	case APT_USBTRX_CMD_ResponseGetFWVersion:
	case APT_USBTRX_CMD_ResponseGetFWVersionRevision: {
		if (dev->rx_complete.id == get_request_command_from_response_command(msg->id)) {
			DMSG("complete!, <id:0x%02x> length=%d", msg->id,
			     APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
			dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
			memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
			complete(&dev->rx_complete.complete);
		}
		break;
	}
	case APT_USBTRX_CMD_ACK:
	case APT_USBTRX_CMD_NACK: {
		int id;
		int result;

		result = apt_usbtrx_msg_parse_ack(msg->payload, msg->payload_size, &id);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_msg_parse_ack().. Error");
			break;
		}
		if (id == APT_USBTRX_CMD_ResetTS && msg->id == APT_USBTRX_CMD_ACK) {
			dev->basetime = *dev->resettime;
		}
		if (dev->rx_complete.id == id) {
			DMSG("complete!, <id:0x%02x> length=%d", msg->id,
			     APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
			dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
			memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
			complete(&dev->rx_complete.complete);
		}
		break;
	}
	default: {
		int result;
		result = dev->unique_func.dispatch_msg(dev, data, msg);
		if (result != RESULT_Success) {
			EMSG_RL("dispatch_msg().. Error");
			apt_usbtrx_dump_msg(msg);
			return RESULT_Failure;
		}
		break;
	}
	}

	return RESULT_Success;
}

/*!
 * @brief rx bulk callback
 */
static void apt_usbtrx_read_bulk_callback(struct urb *urb)
{
	apt_usbtrx_dev_t *dev = urb->context;
	int recv_size;
	int result;
	u8 *buf = dev->rx_transfer.buffer;
	int total_size;
	int remain_size;
	int processed_size;
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return;
	}

	switch (urb->status) {
	case 0:
		break;
	case -ENOENT:
	case -EPIPE:
	case -EPROTO:
	case -ESHUTDOWN:
		return;
	default:
		usb_fill_bulk_urb(urb, /*!< struct urb *urb */
				  dev->udev, /*!< strut usb_device *dev */
				  usb_rcvbulkpipe(dev->udev, dev->bulk_in->bEndpointAddress), /*!< unsigned int pipe */
				  urb->transfer_buffer, /*!< void * transfer_buffer */
				  RX_BUFFER_SIZE, /*!< int buffer_length */
				  apt_usbtrx_read_bulk_callback, /*!< usb_complete_t complete_fn */
				  dev); /*!< void* context */
		result = usb_submit_urb(urb, GFP_ATOMIC);
		if (result != 0) {
			EMSG("usb_submit_urb().. Error, <errno:%d>", result);
			return;
		}
		return;
	}

	recv_size = urb->actual_length;
	memcpy(&buf[dev->rx_transfer.data_size], urb->transfer_buffer, recv_size);
	total_size = dev->rx_transfer.data_size + recv_size;

	processed_size = 0;
	remain_size = total_size;

	while (remain_size >= APT_USBTRX_CMD_MIN_LENGTH) {
		apt_usbtrx_msg_t msg;

		result = apt_usbtrx_msg_parse(&buf[processed_size], remain_size, &msg);
		if (result != RESULT_Success) {
			if (result == RESULT_NotEnough) {
				break;
			}
			processed_size++;
			remain_size--;
			continue;
		}

		result = apt_usbtrx_dispatch_msg(dev, &buf[processed_size], &msg);
		if (result != RESULT_Success) {
		}

#if 0
		DMSG("msg is coming!, <id:0x%02x> length=%d", msg.id, APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size));
#endif
		processed_size += APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size);
		remain_size -= APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size);
	}

	if (remain_size > 0) {
		if (processed_size > 0) {
			memmove(buf, &buf[processed_size], remain_size);
		}
		dev->rx_transfer.data_size = remain_size;
	} else {
		dev->rx_transfer.data_size = 0;
	}

	usb_fill_bulk_urb(urb, dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in->bEndpointAddress),
			  urb->transfer_buffer, RX_BUFFER_SIZE, apt_usbtrx_read_bulk_callback, dev);
	result = usb_submit_urb(urb, GFP_ATOMIC);
	if (result != 0) {
		EMSG("usb_submit_rub().. Error, <errno:%d>", result);
		return;
	}
}

/*!
 * @brief setup rx urbs
 */
int apt_usbtrx_setup_rx_urbs(apt_usbtrx_dev_t *dev)
{
	int result;
	int idx;

	CHKMSG("ENTER");

	memset(dev->rx_transfer.buffer, 0, dev->rx_transfer.buffer_size);
	dev->rx_transfer.data_size = 0;

	for (idx = 0; idx < MAX_RX_URBS; idx++) {
		struct urb *urb = NULL;
		u8 *buf = NULL;
		dma_addr_t buf_dma;

		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (urb == NULL) {
			EMSG("usb_alloc_urb().. Error");
			break;
		}

		buf = usb_alloc_coherent(dev->udev, RX_BUFFER_SIZE, GFP_KERNEL, &buf_dma);
		if (buf == NULL) {
			EMSG("usb_alloc_coherent().. Error");
			usb_free_urb(urb);
			break;
		}

		usb_fill_bulk_urb(urb, dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in->bEndpointAddress), buf,
				  RX_BUFFER_SIZE, apt_usbtrx_read_bulk_callback, dev);
		urb->transfer_dma = buf_dma;
		urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		usb_anchor_urb(urb, &dev->rx_submitted);

		result = usb_submit_urb(urb, GFP_KERNEL);
		if (result != 0) {
			EMSG("usb_submit_urb().. Error, <errno:%d>", result);
			usb_unanchor_urb(urb);
			usb_free_coherent(dev->udev, RX_BUFFER_SIZE, buf, buf_dma);
			usb_free_urb(urb);
			break;
		}

		dev->rxbuf[idx] = buf;
		dev->rxbuf_dma[idx] = buf_dma;
		usb_free_urb(urb);
	}

	if (idx < MAX_RX_URBS) {
		EMSG("%s: setup error, <idx:%d>", __func__, idx);
		return RESULT_Failure;
	}

	atomic_set(&dev->rx_ongoing, true);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief tx bulk callback
 */
static void apt_usbtrx_write_bulk_callback(struct urb *urb)
{
	apt_usbtrx_dev_t *dev = urb->context;
	int status = urb->status;

	switch (status) {
	case 0:
		break;
	case -ENOENT:
	case -ECONNRESET:
	case -ESHUTDOWN:
		break;
	default:
		EMSG("write bulk error, <status:%d>", status);
		break;
	}

	usb_free_coherent(urb->dev, urb->transfer_buffer_length, urb->transfer_buffer, urb->transfer_dma);
	up(&dev->tx_usb_transfer_sem);

	dev->unique_func.write_bulk_callback(urb);
}

/*!
 * @brief setup tx urb
 */
int apt_usbtrx_setup_tx_urb(apt_usbtrx_dev_t *dev, u8 *data, int data_size)
{
	struct urb *urb = NULL;
	u8 *buf = NULL;
	int result;

	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (urb == NULL) {
		EMSG("usb_alloc_urb().. Error");
		return RESULT_Failure;
	}

	buf = usb_alloc_coherent(dev->udev, data_size, GFP_KERNEL, &urb->transfer_dma);
	if (buf == NULL) {
		EMSG("usb_alloc_coherent().. Error, <size:%d>", data_size);
		usb_free_urb(urb);
		return RESULT_Failure;
	}

	memcpy(buf, data, data_size);
	usb_fill_bulk_urb(urb, dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out->bEndpointAddress), buf, data_size,
			  apt_usbtrx_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &dev->tx_submitted);

	result = usb_submit_urb(urb, GFP_KERNEL);
	if (result != 0) {
		EMSG("usb_submit_urb().. Error, <errno:%d>", result);
		usb_unanchor_urb(urb);
		usb_free_coherent(dev->udev, data_size, buf, urb->transfer_dma);
		usb_free_urb(urb);
		return RESULT_Failure;
	}

	usb_free_urb(urb);

	return RESULT_Success;
}

/*!
 * @brief send message
 */
int apt_usbtrx_send_msg(apt_usbtrx_dev_t *dev, u8 *data, int data_size)
{
	int send_size;
	int result;

	CHKMSG("ENTER");
	DMSG("%s(): data size=%d, data=%02x, %02x, %02x, ...", __func__, data_size, data[0], data[1], data[2]);

	dev->rx_complete.id = data[2];
	init_completion(&dev->rx_complete.complete);
	result = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out->bEndpointAddress), data, data_size,
			      &send_size, APT_USBTRX_SEND_TIMEOUT);
	if (result != 0) {
		EMSG("usb_bulk_msg().. Error, <errno:%d> data size=%d>", result, data_size);
		return RESULT_Failure;
	}

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief wait for message with timeout
 */
int apt_usbtrx_wait_msg_timeout(apt_usbtrx_dev_t *dev, u8 ack_id, u8 nack_id, u8 *data, int data_size,
				unsigned int timeout_msec)
{
	u8 *buf;
	int recv_size;
	int result;
	int pos;
	unsigned long timeout = jiffies + msecs_to_jiffies(timeout_msec);

	CHKMSG("ENTER");
	DMSG("%s(): ack=0x%02x, nack=0x%02x, data size=%d", __func__, ack_id, nack_id, data_size);

	buf = kzalloc(RX_BUFFER_SIZE, GFP_KERNEL);
	if (buf == NULL) {
		EMSG("kzalloc().. Error, <size:%d>", RX_BUFFER_SIZE);
		return RESULT_Failure;
	}

	do {
		bool rx_ongoing;

		rx_ongoing = atomic_read(&dev->rx_ongoing);
		if (rx_ongoing == true) {
			DMSG("wait_for_completion_timeout()..");
			result =
				wait_for_completion_timeout(&dev->rx_complete.complete, msecs_to_jiffies(timeout_msec));
			if (result == 0) {
				WMSG("wait_for_completion_timeout().. Error, <errno:%d>", result);
				kfree(buf);
				return RESULT_Timeout;
			}
			recv_size = dev->rx_complete.buffer_size;
			memcpy(buf, dev->rx_complete.buffer, recv_size);
			DMSG("wait_for_completion_timeout().. Success");
		} else {
			DMSG("usb_bulk_msg()..");
			result = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in->bEndpointAddress),
					      buf, data_size, &recv_size, timeout_msec);
			if (result != 0) {
				EMSG("usb_bulk_msg().. Error, <errno:%d> data size=%d>", result, data_size);
				kfree(buf);
				return RESULT_Failure;
			}
			DMSG("usb_bulk_msg().. Success");
		}

		pos = 0;
		while (pos < recv_size) {
			apt_usbtrx_msg_t msg;

			result = apt_usbtrx_msg_parse(&buf[pos], recv_size - pos, &msg);
			if (result != RESULT_Success) {
				DMSG("apt_usbtrx_msg_parse().. Error, <pos:%d>", pos);
				pos = pos + 1;
				continue;
			}

			if (msg.id == ack_id) {
				DMSG("%s(): coming ack, <id:0x%02x> data size=%d", __func__, msg.id, msg.payload_size);
				memcpy(data, buf, APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size));
				kfree(buf);
				CHKMSG("LEAVE");
				return RESULT_Success;
			}

			if (msg.id == nack_id) {
				DMSG("%s(): coming nack, <id:0x%02x> data size=%d", __func__, msg.id, msg.payload_size);
				memcpy(data, buf, APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size));
				kfree(buf);
				CHKMSG("LEAVE");
				return RESULT_Success;
			}

			pos = pos + APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg.payload_size);
			WMSG("%s(): msg is not coming.. RETRY!, <id:0x%02x != 0x%02x or 0x%02x>", __func__, msg.id,
			     ack_id, nack_id);
		}
	} while (time_before(jiffies, timeout));

	EMSG("%s(): msg is not coming, <id:0x%02x, 0x%02x>", __func__, ack_id, nack_id);
	kfree(buf);
	return RESULT_Failure;
}

/*!
 * @brief wait for message
 */
int apt_usbtrx_wait_msg(apt_usbtrx_dev_t *dev, u8 ack_id, u8 nack_id, u8 *data, int data_size)
{
	return apt_usbtrx_wait_msg_timeout(dev, ack_id, nack_id, data, data_size, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief is write enable
 */
static bool apt_usbtrx_is_write_enable(apt_usbtrx_dev_t *dev)
{
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return true;
	}

	if (apt_usbtrx_ringbuffer_is_empty(&dev->tx_data) != true) {
		return true;
	}

	return false;
}

/*!
 * @brief tx thread func
 */
int apt_usbtrx_tx_thread_func(void *arg)
{
	apt_usbtrx_dev_t *dev = (apt_usbtrx_dev_t *)arg;

	while (!kthread_should_stop()) {
		u8 msg_length;
		u8 buffer[APT_USBTRX_CMD_MAX_LENGTH];
		int remain;
		u8 *p_buffer;
		ssize_t rsize;
		int result;
		int tx_buffer_rate;

		result = wait_event_interruptible_timeout(dev->tx_data.wq, (apt_usbtrx_is_write_enable(dev) == true),
							  msecs_to_jiffies(1000));
		if (result < 0) {
			if (result != -ERESTARTSYS) {
				EMSG("wait_event_interruptible().. Error, <errno:%d>", result);
			}
			continue;
		}

		if (time_after(jiffies, dev->tx_transfer_expired)) {
			dev->tx_transfer_expired = jiffies + msecs_to_jiffies(APT_USBTRX_TX_TOKEN_EXPIRED_TIME);
			dev->tx_transfer_token = dev->tx_transfer_max_token * APT_USBTRX_TX_TOKEN_EXPIRED_TIME;
			tx_buffer_rate = atomic_read(&dev->tx_buffer_rate);
			if (tx_buffer_rate > APT_USBTRX_TX_TRANSFER_LIMIT_RATE) {
				dev->tx_transfer_token = 0;
			}
		}

		if (dev->tx_transfer_token <= 0) {
			long timeout = dev->tx_transfer_expired - jiffies;
			if (timeout > 0) {
				unsigned int timeout_usecs = jiffies_to_usecs(timeout);
				unsigned int min_timeout_usecs = (timeout_usecs < 100) ? timeout_usecs : 100;
				usleep_range(min_timeout_usecs, timeout_usecs);
			}
			continue;
		}

		result = down_interruptible(&dev->tx_usb_transfer_sem);
		if (result != 0) {
			continue;
		}

		/* get minimum length to get message length */
		rsize = apt_usbtrx_ringbuffer_rawread(&dev->tx_data, buffer, APT_USBTRX_CMD_MIN_LENGTH);
		if (rsize == 0) {
			up(&dev->tx_usb_transfer_sem);
			continue;
		} else if (rsize < 0 || rsize != APT_USBTRX_CMD_MIN_LENGTH) {
			EMSG("apt_usbtrx_ringbuffer_rawread().. Error, <size:%zd>", rsize);
			up(&dev->tx_usb_transfer_sem);
			continue;
		}

		result = apt_usbtrx_msg_get_length(buffer, APT_USBTRX_CMD_MIN_LENGTH, &msg_length);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_msg_get_length().. Error");
			up(&dev->tx_usb_transfer_sem);
			continue;
		}
		if (msg_length < APT_USBTRX_CMD_MIN_LENGTH || msg_length > APT_USBTRX_CMD_MAX_LENGTH) {
			EMSG("invalid msg_length = %d", msg_length);
			up(&dev->tx_usb_transfer_sem);
			continue;
		}

		/* get full message (APT_USBTRX_CMD_MIN_LENGTH has already been read)*/
		p_buffer = &buffer[0] + APT_USBTRX_CMD_MIN_LENGTH;
		remain = msg_length - APT_USBTRX_CMD_MIN_LENGTH;
		while (remain > 0) {
			rsize = apt_usbtrx_ringbuffer_rawread(&dev->tx_data, p_buffer, remain);
			if (rsize < 0) {
				EMSG("apt_usbtrx_ringbuffer_rawread().. Error, <size:%zd>", rsize);
				break;
			}
			p_buffer += rsize;
			remain -= rsize;
		}
		if (remain != 0) {
			EMSG("remain is not zero.. Error, <remain:%d>", remain);
			up(&dev->tx_usb_transfer_sem);
			continue;
		}

		result = apt_usbtrx_setup_tx_urb(dev, buffer, msg_length);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_setup_tx_urb().. Error");
			up(&dev->tx_usb_transfer_sem);
			continue;
		}
		dev->tx_transfer_token--;
	}

	return 0;
}
