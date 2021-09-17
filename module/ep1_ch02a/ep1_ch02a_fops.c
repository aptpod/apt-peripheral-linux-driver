// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include <linux/uaccess.h>

#include "../apt_usbtrx_fops.h"
#include "../ap_ct2a/ap_ct2a_fops.h" /* inherit from ap_ct2a */
#include "../ap_ct2a/ap_ct2a_def.h" /* inherit from ap_ct2a(candev) */
#include "../ap_ct2a/ap_ct2a_cmd.h" /* inherit from ap_ct2a(can_set_mode) */
#include "ep1_ch02a_cmd.h"
#include "ep1_ch02a_cmd_def.h"

static int ep1_ch02a_ioctl_set_bit_timing(apt_usbtrx_dev_t *dev, const ep1_ch02a_ioctl_set_bit_timing_t *param)
{
	ep1_ch02a_msg_set_bit_timing_t msg;
	bool success;
	int result;

	msg.prop_seg = param->prop_seg;
	msg.phase_seg1 = param->phase_seg1;
	msg.phase_seg2 = param->phase_seg2;
	msg.sjw = param->sjw;
	msg.brp = param->brp;

	result = ep1_ch02a_set_bit_timing(dev, &msg, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_ch02a_set_bit_timing().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("ep1_ch02a_set_bit_timing().. Error, Exec failed");
		return RESULT_Failure;
	}
	return RESULT_Success;
}

/*!
 * @brief ioctl
 */
long ep1_ch02a_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	apt_usbtrx_dev_t *dev = file->private_data;
	int result;

	switch (cmd) {
	case APT_USBTRX_IOCTL_GET_STATUS:
	case APT_USBTRX_IOCTL_SET_TRIGGER: {
		EMSG("not supported, <ioctl:0x%02x>", cmd);
		return -EFAULT;
	}
	case EP1_CH02A_IOCTL_GET_STATUS: {
		ep1_ch02a_ioctl_get_status_t param;
		ep1_ch02a_msg_resp_get_status_t status;

		result = ep1_ch02a_get_status(dev, &status);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_status().. Error");
			return -EIO;
		}

		param.silent = status.mode.silent;
		param.start = status.mode.start;
		param.baudrate = status.mode.baudrate;
		param.enable_sync = status.enable_sync;

		result = copy_to_user((void __user *)arg, &param, sizeof(ep1_ch02a_ioctl_get_status_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case EP1_CH02A_IOCTL_SET_BIT_TIMING: {
		ep1_ch02a_ioctl_set_bit_timing_t param;

		result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_ch02a_ioctl_set_bit_timing_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		if (ep1_ch02a_ioctl_set_bit_timing(dev, &param) != RESULT_Success) {
			return -EIO;
		}
		break;
	}
	case EP1_CH02A_IOCTL_GET_BIT_TIMING: {
		ep1_ch02a_ioctl_get_bit_timing_t param;
		ep1_ch02a_msg_resp_get_bit_timing_t bit_timing;

		result = ep1_ch02a_get_bit_timing(dev, &bit_timing);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_bit_timing().. Error");
			return -EIO;
		}

		param.can_clock = bit_timing.can_clock;
		param.cfg.prop_seg = bit_timing.params.prop_seg;
		param.cfg.phase_seg1 = bit_timing.params.phase_seg1;
		param.cfg.phase_seg2 = bit_timing.params.phase_seg2;
		param.cfg.sjw = bit_timing.params.sjw;
		param.cfg.brp = bit_timing.params.brp;

		result = copy_to_user((void __user *)arg, &param, sizeof(ep1_ch02a_ioctl_get_bit_timing_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	default:
		return apt_usbtrx_unique_can_ioctl(file, cmd, arg);
	}

	return 0;
}

#ifdef SUPPORT_NETDEV
/*!
 * @brief set bittiming (netdev)
 */
int ep1_ch02a_netdev_set_bittiming(struct net_device *netdev)
{
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);
	apt_usbtrx_dev_t *dev = candev->dev;
	int result;

	{
		int baudrate = candev->can.bittiming.bitrate / 1000; // Kbps
		bool silent = candev->can.ctrlmode & CAN_CTRLMODE_LISTENONLY ? true : false;

		result = apt_usbtrx_unique_can_set_mode(dev, baudrate, silent);
		if (result != 0) {
			EMSG("apt_usbtrx_unique_can_set_mode().. Error");
			return -EIO;
		}
	}

	{
		ep1_ch02a_ioctl_set_bit_timing_t param;

		param.prop_seg = candev->can.bittiming.prop_seg;
		param.phase_seg1 = candev->can.bittiming.phase_seg1;
		param.phase_seg2 = candev->can.bittiming.phase_seg2;
		param.sjw = candev->can.bittiming.sjw;
		param.brp = candev->can.bittiming.brp;

		if (ep1_ch02a_ioctl_set_bit_timing(dev, &param) != RESULT_Success) {
			return -EIO;
		}
	}

	return 0;
}
#endif /* SUPPORT_NETDEV */
