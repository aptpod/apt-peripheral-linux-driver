// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/uaccess.h>

#include "ep1_ag08a_fops.h"

static int count_bits(int n)
{
	int sum;
	sum = (n >> 1) & 03333333333;
	sum = n - sum - ((sum >> 1) & 03333333333);
	sum = ((sum + (sum >> 3)) & 0707070707) % 077;
	return sum;
}

/*!
 * @brief get read-payload size
 *
 * FIXME: It is assumed that it can be acquired
 */
int ep1_ag08a_get_read_payload_size(const void *payload)
{
	ep1_ag08a_payload_notify_analog_input_t *p = NULL;
	int no_data_ch_count = 0;
	int cmd_size = 0;

	if (payload == NULL) {
		EMSG("read-payload is NULL");
		return -1;
	}

	p = (ep1_ag08a_payload_notify_analog_input_t *)payload;

	if (p->channel == 0) {
		EMSG("read-payload channel is zero");
		return -1;
	}

	no_data_ch_count = 8 - count_bits(p->channel);
	cmd_size = EP1_AG08A_CMD_LENGTH_NOTIFY_ANALOG_INPUT - (sizeof(u16) * no_data_ch_count);

	return APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(cmd_size);
}

/*!
 * @brief get write-payload size
 *
 */
int ep1_ag08a_get_write_payload_size(const void *payload)
{
	if (payload == NULL) {
		return -1;
	}

	EMSG("write is not supported.. Error");
	return -1;
}

/*!
 * @brief get read-payload timestamp
 *
 * FIXME: It is assumed that it can be acquired
 */
apt_usbtrx_timestamp_t *ep1_ag08a_get_read_payload_timestamp(const void *payload)
{
	ep1_ag08a_payload_notify_analog_input_t *read_payload;

	if (payload == NULL) {
		return NULL;
	}

	read_payload = (ep1_ag08a_payload_notify_analog_input_t *)payload;

	return &read_payload->timestamp;
}

/*!
 * @brief get write cmd id
 */
int ep1_ag08a_get_write_cmd_id(void)
{
	EMSG("write is not supported.. Error");
	return -1;
}

/*!
 * @brief get fw size
 */
int ep1_ag08a_get_fw_size(void)
{
	return EP1_AG08A_FW_DATA_SIZE;
}

/*!
 * @brief ioctl
 */
long ep1_ag08a_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	apt_usbtrx_dev_t *dev = file->private_data;
	//ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);
	int result;

	switch (cmd) {
	case EP1_AG08A_IOCTL_GET_STATUS: {
		ep1_ag08a_ioctl_get_status_t param;
		ep1_ag08a_msg_resp_get_status_t status;
		int ch;

		result = ep1_ag08a_get_status(dev, &status);
		if (result != RESULT_Success) {
			EMSG("ep1_ag08a_get_status().. Error");
			return -EIO;
		}

		param.in.cfg.send_rate = status.in.cfg.send_rate;
		for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
			param.in.cfg.vmin[ch] = status.in.cfg.vmin[ch];
			param.in.cfg.vmax[ch] = status.in.cfg.vmax[ch];
			param.in.ctrl.start[ch] = status.in.ctrl.start[ch];
		}
		param.out.cfg.waveform_type = status.out.cfg.waveform_type;
		param.out.cfg.voltage = status.out.cfg.voltage;
		param.out.cfg.frequency = status.out.cfg.frequency;
		param.out.ctrl.start = status.out.ctrl.start;

		result = copy_to_user((void __user *)arg, &param, sizeof(ep1_ag08a_ioctl_get_status_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case EP1_AG08A_IOCTL_SET_ANALOG_INPUT: {
		ep1_ag08a_ioctl_set_analog_input_t param;
		ep1_ag08a_msg_set_analog_input_t cfg;
		int ch = 0;

		result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_ag08a_ioctl_set_analog_input_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		cfg.send_rate = param.send_rate;
		DMSG("%s(): send_rate=%d", __func__, cfg.send_rate);
		for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
			cfg.vmin[ch] = param.vmin[ch];
			cfg.vmax[ch] = param.vmax[ch];
			DMSG("%s(): ch=%d, vmin=%d, vax=%d", __func__, ch, cfg.vmin[ch], cfg.vmax[ch]);
		}

		result = ep1_ag08a_set_device_input(dev, &cfg);
		if (result != RESULT_Success) {
			return -EIO;
		}
		break;
	}
	case EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT: {
		ep1_ag08a_ioctl_control_analog_input_t param;
		ep1_ag08a_msg_control_analog_input_t ctrl;
		bool start = false;
		int ch = 0;

		result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_ag08a_ioctl_control_analog_input_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
			ctrl.start[ch] = param.start[ch];
			DMSG("%s(): start[%d]=%d", __func__, ch, ctrl.start[ch]);
			if (ctrl.start[ch]) {
				start = true;
			}
		}

		if (start) {
			result = apt_usbtrx_ringbuffer_clear(&dev->rx_data);
			if (result != RESULT_Success) {
				EMSG("apt_usbtrx_ringbuffer_clear().. Error");
				return -EIO;
			}
		}

		result = ep1_ag08a_control_device_input(dev, &ctrl);
		if (result != RESULT_Success) {
			return -EIO;
		}
		break;
	}
	case EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT: {
		ep1_ag08a_ioctl_set_analog_output_t param;
		ep1_ag08a_msg_set_analog_output_t cfg;

		result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_ag08a_ioctl_set_analog_output_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		cfg.waveform_type = param.waveform_type;
		cfg.voltage = param.voltage;
		cfg.frequency = param.frequency;
		DMSG("%s(): waveform_type=%d voltage=%d frequency=%d", __func__, cfg.waveform_type, cfg.voltage,
		     cfg.frequency);

		result = ep1_ag08a_set_device_output(dev, &cfg);
		if (result != RESULT_Success) {
			return -EIO;
		}
		break;
	}
	case EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT: {
		ep1_ag08a_ioctl_control_analog_output_t param;
		ep1_ag08a_msg_control_analog_output_t ctrl;

		result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_ag08a_ioctl_control_analog_output_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		ctrl.start = param.start;
		DMSG("%s(): start=%d", __func__, ctrl.start);

		result = ep1_ag08a_control_device_output(dev, &ctrl);
		if (result != RESULT_Success) {
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
 * @brief set device input config
 */
int ep1_ag08a_set_device_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_input_t *cfg)
{
	int result;
	bool start = true;
	bool success = false;

	/* input stop check */
	result = ep1_ag08a_is_device_input_start(dev, &start);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_is_device_input_start().. Error");
		return RESULT_Failure;
	}
	if (start) {
		EMSG("stop device, then set analog input");
		return RESULT_Failure;
	}

	result = ep1_ag08a_set_analog_input(dev, cfg, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_set_analog_input().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("ep1_ag08a_set_analog_input().. Error, Exec failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief set device output config
 */
int ep1_ag08a_set_device_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_output_t *cfg)
{
	int result;
	bool start = true;
	bool success = false;

	/* output stop check */
	result = ep1_ag08a_is_device_output_start(dev, &start);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_is_device_output_start().. Error");
		return RESULT_Failure;
	}
	if (start) {
		EMSG("stop device, then set analog output");
		return RESULT_Failure;
	}

	result = ep1_ag08a_set_analog_output(dev, cfg, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_set_analog_output().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("ep1_ag08a_set_analog_output().. Error, Exec failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief control device input
 */
int ep1_ag08a_control_device_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_input_t *ctrl)
{
	int result;
	bool success = false;

	result = ep1_ag08a_control_analog_input(dev, ctrl, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_control_analog_input().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("ep1_ag08a_control_analog_input().. Error, Exec failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief control device output
 */
int ep1_ag08a_control_device_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_output_t *ctrl)
{
	int result;
	bool success = false;

	result = ep1_ag08a_control_analog_output(dev, ctrl, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_control_analog_output().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("ep1_ag08a_control_analog_output().. Error, Exec failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief stop device input
 */
int ep1_ag08a_stop_device_input(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_msg_control_analog_input_t ctrl;
	int ch;

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		ctrl.start[ch] = false;
	}

	return ep1_ag08a_control_device_input(dev, &ctrl);
}

/*!
 * @brief stop device output
 */
int ep1_ag08a_stop_device_output(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_msg_control_analog_output_t ctrl;

	ctrl.start = false;

	return ep1_ag08a_control_device_output(dev, &ctrl);
}

/*!
 * @brief is device input start
 */
int ep1_ag08a_is_device_input_start(apt_usbtrx_dev_t *dev, bool *start)
{
	ep1_ag08a_msg_resp_get_status_t status;
	int result;
	int ch;

	result = ep1_ag08a_get_status(dev, &status);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_get_status().. Error");
		return RESULT_Failure;
	}

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		if (status.in.ctrl.start[ch]) {
			*start = true;
			return RESULT_Success;
		}
	}

	*start = false;
	return RESULT_Success;
}

/*!
 * @brief is device output start
 */
int ep1_ag08a_is_device_output_start(apt_usbtrx_dev_t *dev, bool *start)
{
	ep1_ag08a_msg_resp_get_status_t status;
	int result;

	result = ep1_ag08a_get_status(dev, &status);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_get_status().. Error");
		return RESULT_Failure;
	}

	if (status.out.ctrl.start) {
		*start = true;
	} else {
		*start = false;
	}

	return RESULT_Success;
}

/*!
 * @brief open (file operation)
 */
int ep1_ag08a_open(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);
	int if_type = atomic_read(&unique_data->if_type);

	if (if_type != EP1_AG08A_IF_TYPE_NONE) {
		EMSG("Device is already in use");
		return -EBUSY;
	}

	atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_FILE);

	return 0;
}

/*!
 * @brief close (file operation)
 */
int ep1_ag08a_close(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);
	atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_NONE);
	return 0;
}
