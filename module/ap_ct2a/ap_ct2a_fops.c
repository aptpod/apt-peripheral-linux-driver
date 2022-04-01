// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/version.h>
#include <linux/uaccess.h>

#include "../apt_usbtrx_fops.h" /* apt_usbtrx_write_tx_rb() */
#include "ap_ct2a_fops.h"
#include "ap_ct2a_cmd_def.h"
#include "ap_ct2a_cmd.h"
#include "ap_ct2a_def.h"

/*!
 * @brief get read-payload size
 *
 * FIXME: It is assumed that it can be acquired
 */
int apt_usbtrx_unique_can_get_read_payload_size(const void *payload)
{
	if (payload == NULL) {
		return -1;
	}

	/* AP-CT2A returns a fixed size */
	return sizeof(apt_usbtrx_payload_notify_recv_can_frame_t);
}

/*!
 * @brief get write-payload size
 *
 * FIXME: It is assumed that it can be acquired
 */
int apt_usbtrx_unique_can_get_write_payload_size(const void *payload)
{
	if (payload == NULL) {
		return -1;
	}

	/* AP-CT2A returns a fixed size */
	return sizeof(apt_usbtrx_payload_send_can_frame_t);
}

/*!
 * @brief get read-payload timestamp
 *
 * FIXME: It is assumed that it can be acquired
 */
apt_usbtrx_timestamp_t *apt_usbtrx_unique_can_get_read_payload_timestamp(const void *payload)
{
	apt_usbtrx_payload_notify_recv_can_frame_t *read_payload;

	if (payload == NULL) {
		return NULL;
	}

	read_payload = (apt_usbtrx_payload_notify_recv_can_frame_t *)payload;

	return &read_payload->timestamp;
}

/*!
 * @brief get write cmd id
 */
int apt_usbtrx_unique_can_get_write_cmd_id(void)
{
	return APT_USBTRX_CMD_SendCANFrame;
}

/*!
 * @brief get fw size
 */
int apt_usbtrx_unique_can_get_fw_size(void)
{
	return APT_USBTRX_FW_DATA_SIZE;
}

/*!
 * @brief start can
 */
int apt_usbtrx_unique_can_start_can(apt_usbtrx_dev_t *dev)
{
	int result;
	bool success;

	result = apt_usbtrx_ringbuffer_clear(&dev->rx_data);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_clear().. Error");
		return RESULT_Failure;
	}

	result = apt_usbtrx_start_stop_can(dev, true, &success);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_start_stop_can().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("apt_usbtrx_start_stop_can().. Error, Exec failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief stop can
 */
int apt_usbtrx_unique_can_stop_can(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_msg_resp_get_status_t status;
	int result;
	bool success;
	int retry = 3;

	do {
		result = apt_usbtrx_start_stop_can(dev, false, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_start_stop_can().. Error");
			return RESULT_Failure;
		}
		if (success != true) {
			EMSG("apt_usbtrx_start_stop_can().. Error, Exec failed");
			return RESULT_Failure;
		}

		result = apt_usbtrx_get_status(dev, &status);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_status().. Error");
			return RESULT_Failure;
		}
		if (status.mode.start == false) {
			break;
		}
		WMSG("apt_usbtrx_start_stop_can().. Retry(%d)", retry);
		mdelay(1);
	} while (--retry > 0);
	if (retry <= 0) {
		WMSG("apt_usbtrx_start_stop_can().. Error, not change status");
	}
	return RESULT_Success;
}

/*!
 * @brief set mode
 */
int apt_usbtrx_unique_can_set_mode(apt_usbtrx_dev_t *dev, int baudrate, bool silent)
{
	apt_usbtrx_msg_set_mode_t mode;
	int max_token;
	int result;
	bool success;

	mode.silent = silent;
	mode.start = false;
	mode.baudrate = baudrate;
	DMSG("%s(): silent=%d, baudrate=%d", __func__, mode.silent, mode.baudrate);
	max_token = (mode.baudrate * 1024 / 8) / APT_USBTRX_TX_TOKEN_CAN_SIZE /
		    1000; /* transfer bytes / can size / 1000msec */
	dev->tx_transfer_max_token = (max_token > 0) ? max_token : 1;

	result = apt_usbtrx_set_mode(dev, &mode, &success);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_set_mode().. Error");
		return -EIO;
	}

	if (success != true) {
		EMSG("apt_usbtrx_set_mode().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl
 */
long apt_usbtrx_unique_can_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	apt_usbtrx_dev_t *dev = file->private_data;
	int result;

	switch (cmd) {
	case APT_USBTRX_IOCTL_SET_MODE: {
		apt_usbtrx_ioctl_set_mode_t param;

		result = copy_from_user(&param, (void __user *)arg, sizeof(apt_usbtrx_ioctl_set_mode_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		result = apt_usbtrx_unique_can_set_mode(dev, param.baudrate, param.silent);
		if (result != 0) {
			EMSG("apt_usbtrx_unique_can_set_mode().. Error");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_STATUS: {
		apt_usbtrx_ioctl_get_status_t param;
		apt_usbtrx_msg_resp_get_status_t status;

		result = apt_usbtrx_get_status(dev, &status);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_status().. Error");
			return -EIO;
		}

		param.silent = status.mode.silent;
		param.start = status.mode.start;
		param.baudrate = status.mode.baudrate;
		param.enable_trigger = status.enable_trigger;
		param.trigger.input.enable = status.trigger.input.enable;
		param.trigger.input.logic = status.trigger.input.logic;
		param.trigger.input.id = status.trigger.input.id;
		param.trigger.output.enable = status.trigger.output.enable;
		param.trigger.output.logic = status.trigger.output.logic;
		param.trigger.output.id = status.trigger.output.id;
		param.trigger.pulse_width = status.trigger.pulse_width;
		param.trigger.pulse_interval = status.trigger.pulse_interval;

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_status_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_RESET_CAN_SUMMARY: {
		bool success;

		result = apt_usbtrx_reset_can_summary(dev, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_reset_can_summary().. Error");
			return -EIO;
		}
		if (success != true) {
			EMSG("apt_usbtrx_reset_can_summary().. Error, Exec failed");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_START_CAN: {
		result = apt_usbtrx_unique_can_start_can(dev);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_unique_can_start_can().. Error");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_STOP_CAN: {
		result = apt_usbtrx_unique_can_stop_can(dev);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_unique_can_stop_can().. Error");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_SET_TRIGGER: {
		apt_usbtrx_ioctl_set_trigger_t param;
		apt_usbtrx_msg_set_trigger_t trigger;
		bool success;

		result = copy_from_user(&param, (void __user *)arg, sizeof(apt_usbtrx_ioctl_set_trigger_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		trigger.input.enable = param.input.enable;
		trigger.input.logic = (param.input.logic == 1) ? APT_USBTRX_TRIGGER_LOGIC_Negative :
								 APT_USBTRX_TRIGGER_LOGIC_Positive;
		trigger.input.id = param.input.id;
		trigger.output.enable = param.output.enable;
		trigger.output.logic = (param.output.logic == 1) ? APT_USBTRX_TRIGGER_LOGIC_Negative :
								   APT_USBTRX_TRIGGER_LOGIC_Positive;
		trigger.output.id = param.output.id;
		trigger.pulse_width = param.pulse_width;
		trigger.pulse_interval = param.pulse_interval;
		DMSG("%s(): input: enable=%d, logic=%d, id=0x%02x", __func__, param.input.enable, param.input.logic,
		     param.input.id);
		DMSG("%s(): output: enable=%d, logic=%d, id=0x%02x", __func__, param.output.enable, param.output.logic,
		     param.output.id);
		DMSG("%s(): pulse: width=%d, interval=%d", __func__, param.pulse_width, param.pulse_interval);

		result = apt_usbtrx_set_trigger(dev, &trigger, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_set_trigger().. Error");
			return -EIO;
		}
		if (success != true) {
			EMSG("apt_usbtrx_set_trigger().. Error, Exec failed");
			return -EIO;
		}
		break;
	}
	default:
		EMSG("not supported, <ioctl:0x%02x>", cmd);
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief is device start
 */
int apt_usbtrx_unique_can_is_device_input_start(apt_usbtrx_dev_t *dev, bool *start)
{
	apt_usbtrx_msg_resp_get_status_t status;
	int result;

	result = apt_usbtrx_get_status(dev, &status);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_get_status().. Error");
		return RESULT_Failure;
	}

	*start = status.mode.start;

	return RESULT_Success;
}

/*!
 * @brief open (file operation)
 */
int apt_usbtrx_unique_can_open(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	int if_type = atomic_read(&unique_data->if_type);

	if (if_type != APT_USBTRX_CAN_IF_TYPE_NONE) {
		EMSG("Device is already in use");
		return -EBUSY;
	}

	atomic_set(&unique_data->if_type, APT_USBTRX_CAN_IF_TYPE_FILE);
	return 0;
}

/*!
 * @brief close (file operation)
 */
int apt_usbtrx_unique_can_close(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	atomic_set(&unique_data->if_type, APT_USBTRX_CAN_IF_TYPE_NONE);
	return 0;
}

#ifdef SUPPORT_NETDEV
/*!
 * @brief set bittiming (netdev)
 */
int apt_usbtrx_unique_can_netdev_set_bittiming(struct net_device *netdev)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	apt_usbtrx_dev_t *dev = candev->dev;
	int result;

	int baudrate = candev->can.bittiming.bitrate / 1000; // Kbps
	bool silent = candev->can.ctrlmode & CAN_CTRLMODE_LISTENONLY ? true : false;

	result = apt_usbtrx_unique_can_set_mode(dev, baudrate, silent);
	if (result != 0) {
		EMSG("apt_usbtrx_unique_can_set_mode().. Error");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief start interface
 */
static int apt_usbtrx_unique_can_netdev_start(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	apt_usbtrx_candev_t *candev = netdev_priv(unique_data->netdev);
	int result;

	result = candev->can.do_set_bittiming(unique_data->netdev);
	if (result != 0) {
		EMSG("can.do_set_bittiming().. Error");
		return -EIO;
	}

	result = apt_usbtrx_unique_can_start_can(dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_unique_can_start_can().. Error");
		return -EIO;
	}

	candev->can.state = CAN_STATE_ERROR_ACTIVE;
	atomic_set(&unique_data->if_type, APT_USBTRX_CAN_IF_TYPE_NET);

	return 0;
}

/*!
 * @brief open (netdev operation)
 */
int apt_usbtrx_unique_can_netdev_open(struct net_device *netdev)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	apt_usbtrx_dev_t *dev = candev->dev;
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	int if_type = atomic_read(&unique_data->if_type);
	int err;

	if (if_type != APT_USBTRX_CAN_IF_TYPE_NONE) {
		EMSG("Device is already in use");
		return -EBUSY;
	}

	/* common open */
	err = open_candev(netdev);
	if (err) {
		netdev_err(netdev, "candev open failed: %d\n", err);
		return err;
	}

	/* finally start device */
	err = apt_usbtrx_unique_can_netdev_start(dev);
	if (err) {
		netdev_err(netdev, "couldn't start device: %d\n", err);
		close_candev(netdev);
		return err;
	}

	netif_start_queue(netdev);

	return 0;
}

/*!
 * @brief close (netdev operation)
 */
int apt_usbtrx_unique_can_netdev_close(struct net_device *netdev)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	apt_usbtrx_dev_t *dev = candev->dev;
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	int result;

	result = apt_usbtrx_unique_can_stop_can(dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_unique_can_stop_can().. Error");
		return -EIO;
	}

	netif_stop_queue(netdev);
	close_candev(netdev);

	candev->can.state = CAN_STATE_STOPPED;
	atomic_set(&unique_data->if_type, APT_USBTRX_CAN_IF_TYPE_NONE);

	return 0;
}

netdev_tx_t apt_usbtrx_unique_can_netdev_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	struct can_frame *cf = (struct can_frame *)skb->data;
	apt_usbtrx_payload_send_can_frame_t send_cf;
	int tx_data_size;
	int result;
	bool silent = candev->can.ctrlmode & CAN_CTRLMODE_LISTENONLY ? true : false;

	if (silent) {
		return NETDEV_TX_OK;
	}

	if (can_dropped_invalid_skb(netdev, skb)) {
		return NETDEV_TX_OK;
	}

	tx_data_size = sizeof(send_cf.data) / sizeof(send_cf.data[0]);
	if (tx_data_size < cf->can_dlc) {
		EMSG("tx error. unsupported tx data size.");
		return NETDEV_TX_BUSY;
	}

	memset(&send_cf, 0, sizeof(send_cf));

	send_cf.id[0] = cf->can_id & 0xFF;
	send_cf.id[1] = (cf->can_id & 0xFF00) >> 8;
	send_cf.id[2] = (cf->can_id & 0xFF0000) >> 16;
	send_cf.id[3] = (cf->can_id & 0xFF000000) >> 24;

	send_cf.dlc = cf->can_dlc;

	memcpy(send_cf.data, cf->data, cf->can_dlc);

	candev->tx_data_size = tx_data_size;

	netif_stop_queue(netdev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0)
	can_put_echo_skb(skb, netdev, 0, 0);
#else
	can_put_echo_skb(skb, netdev, 0);
#endif

	result = apt_usbtrx_write_tx_rb(candev->dev, &send_cf, sizeof(send_cf));
	if (result < 0) {
		EMSG("apt_usbtrx_write_rb().. Error");
		return NETDEV_TX_BUSY;
	}

	return NETDEV_TX_OK;
}

/*!
 * @brief set mode (netdev)
 */
int apt_usbtrx_unique_can_netdev_set_mode(struct net_device *netdev, enum can_mode mode)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	apt_usbtrx_dev_t *dev = candev->dev;
	int result;

	switch (mode) {
	case CAN_MODE_START:
		/* restart */
		result = apt_usbtrx_unique_can_stop_can(dev);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_unique_can_stop_can().. Error");
			return -EIO;
		}

		result = apt_usbtrx_unique_can_netdev_start(dev);
		if (result) {
			netdev_err(netdev, "couldn't start device: %d\n", result);
			return result;
		}
		break;

	default:
		return -EOPNOTSUPP;
	}

	return 0;
}
#endif
