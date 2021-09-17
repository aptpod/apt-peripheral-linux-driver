// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT USB peripherals.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/usb.h>
#include <linux/uaccess.h>

#include "apt_usbtrx_fops.h"
#include "apt_usbtrx_core.h"
#include "apt_usbtrx_ringbuffer.h"
#include "apt_usbtrx_ioctl.h"
#include "apt_usbtrx_cmd.h"
#include "apt_usbtrx_msg.h"

extern struct usb_driver apt_usbtrx_driver;

/*!
 * @brief is read enable
 */
static bool apt_usbtrx_is_read_enable(apt_usbtrx_dev_t *dev)
{
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return true;
	}

	if (apt_usbtrx_ringbuffer_is_empty(&dev->rx_data) != true) {
		return true;
	}

	return false;
}

/*!
 * @brief parse rbmsg
 */
int apt_usbtrx_parse_rbmsg(apt_usbtrx_dev_t *dev, u8 *payload, size_t size, u64 relative_time_ns)
{
	apt_usbtrx_timestamp_t *timestamp = NULL;
	ssize_t remain = size;
	u64 dev_time_us = 0;
	u64 hst_time_us = 0;
	int ts_mode = dev->timestamp_mode;

	if (payload == NULL) {
		EMSG("payload is NULL");
		return RESULT_Failure;
	}

	do_div(relative_time_ns, NSEC_PER_USEC);
	hst_time_us = relative_time_ns;

	if (ts_mode == APT_USBTRX_TIMESTAMP_MODE_HOST) {
		dev_time_us = hst_time_us;
	}

	/* parse rbmsg */
	while (remain > 0) {
		int payload_size = 0;

		timestamp = dev->unique_func.get_read_payload_timestamp(payload);
		if (timestamp == NULL) {
			EMSG("failed to get timestamp");
			return RESULT_Failure;
		}

		/* overrite time stamp */
		if (ts_mode != APT_USBTRX_TIMESTAMP_MODE_DEVICE) {
			u64 v64 = dev_time_us;
			do_div(v64, USEC_PER_SEC);
			timestamp->ts_sec = (u32)(v64);
			timestamp->ts_usec = (u32)(dev_time_us - (timestamp->ts_sec * USEC_PER_SEC));
		}

		payload_size = dev->unique_func.get_read_payload_size(payload);
		if (payload_size < 0) {
			EMSG("failed to get payload_size");
			return RESULT_Failure;
		}
		if (payload_size > APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH)) {
			EMSG("length is over max payload size, <payload_size:%d>", payload_size);
			return RESULT_Failure;
		}

		remain -= payload_size;
		if (remain < 0) {
			EMSG("invalid payload_size");
		} else {
			payload += payload_size;
		}
	}

	return RESULT_Success;
}

/*!
 * @brief open
 */
int apt_usbtrx_open(struct inode *inode, struct file *file)
{
	apt_usbtrx_dev_t *dev;
	struct usb_interface *intf;
	int minor;
	bool onopening;
	bool onclosing;
	int result;

	CHKMSG("ENTER");

	minor = iminor(inode);
	DMSG("minor=%d", minor);

	intf = usb_find_interface(&apt_usbtrx_driver, minor);
	if (intf == NULL) {
		EMSG("usb_find_interface().. Error");
		return -ENODEV;
	}

	dev = usb_get_intfdata(intf);
	if (dev == NULL) {
		EMSG("usb_get_intfdata().. Error");
		return -ENODEV;
	}

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., open cansel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., open cansel");
		return -ESHUTDOWN;
	}
#if 0
	/*
	 * !! FIXME !! permission denied (no:13) if exec
	 */
	result = usb_autopm_get_interface(intf);
	if (result != 0) {
		EMSG("usb_autopm_get_interface().. Error");
		return result;
	}
#endif

	result = dev->unique_func.open(dev);
	if (result < 0) {
		EMSG("open failed");
		return result;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	file->private_data = dev;

	CHKMSG("LEAVE");
	return 0;
}

/*!
 * @brief release
 */
int apt_usbtrx_release(struct inode *inode, struct file *file)
{
	apt_usbtrx_dev_t *dev;
	int minor;
	bool onopening;
	bool onclosing;
	int retval = -ENODEV;

	CHKMSG("ENTER");

	minor = iminor(inode);
	DMSG("minor=%d", minor);

	dev = file->private_data;
	if (dev == NULL) {
		EMSG("dev is NULL");
		goto exit;
	}

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., close cansel");
		goto exit;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., close cansel");
		retval = -ESHUTDOWN;
		goto exit;
	}

	retval = dev->unique_func.close(dev);

#if 0
	if (dev->interface != NULL) {
		/*
		 * !! FIXME !! permission denied (no:13) if exec
		 */
		usb_autopm_put_interface(dev->interface);
	}
#endif

exit:
	/* decrement the count on our device */
	kref_put(&dev->kref, apt_usbtrx_delete);

	CHKMSG("LEAVE");
	return retval;
}

/*!
 * @brief read
 */
ssize_t apt_usbtrx_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	apt_usbtrx_dev_t *dev;
	ssize_t rsize;
	int result;
	bool onopening;
	bool onclosing;
	u64 relative_time_ns;

	dev = file->private_data;
	if (dev == NULL) {
		EMSG("dev is NULL");
		return -ENODEV;
	}

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., read cansel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., read cansel");
		complete(&dev->rx_done);
		return -ESHUTDOWN;
	}

	result = wait_event_interruptible(dev->rx_data.wq, apt_usbtrx_is_read_enable(dev) == true);
	if (result != 0) {
		if (result != -ERESTARTSYS) {
			EMSG("wait_event_interruptible().. Error, <errno:%d>", result);
		}
		return result;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., read cansel");
		complete(&dev->rx_done);
		return -ESHUTDOWN;
	}

	if (dev->timestamp_mode == APT_USBTRX_TIMESTAMP_MODE_DEVICE) {
		/* copy to user memory space */
		rsize = apt_usbtrx_ringbuffer_read(&dev->rx_data, buffer, count);
		if (rsize < 0) {
			EMSG("apt_usbtrx_ringbuffer_read().. Error");
			return -EIO;
		}
	} else {
		/* copy to kernel memory space and then to user memory space */
		size_t rbmsg_rsize = apt_usbtrx_ringbuffer_get_used_size(&dev->rx_data);
		if (rbmsg_rsize > count) {
			rbmsg_rsize = count;
		}

		rsize = apt_usbtrx_ringbuffer_rawread(&dev->rx_data, dev->rx_rbmsg, rbmsg_rsize);
		if (rsize < 0) {
			EMSG("apt_usbtrx_ringbuffer_rawread().. Error");
			return -EIO;
		} else if (rsize != 0) {
			relative_time_ns = apt_usbtrx_get_relative_time_ns(&dev->basetime);
			result = apt_usbtrx_parse_rbmsg(dev, dev->rx_rbmsg, rsize, relative_time_ns);
			if (result != RESULT_Success) {
				EMSG("apt_usbtrx_parse_rbmsg().. Error");
				return -EIO;
			}
			if (copy_to_user(buffer, dev->rx_rbmsg, rsize) != 0) {
				EMSG("copy_to_user().. Error");
				return -EIO;
			}
		}
	}

	if (onclosing == true) {
		complete(&dev->rx_done);
	}
	return rsize;
}

/*!
 * @brief write tx ringbuffer
 */
ssize_t apt_usbtrx_write_tx_rb(apt_usbtrx_dev_t *dev, const void *payload, const u8 payload_size)
{
	u8 data[APT_USBTRX_CMD_MAX_LENGTH];
	u8 msg_size;
	apt_usbtrx_msg_t msg;
	int result;
	bool onopening;
	bool onclosing;
	bool empty;
	ssize_t wsize;
	size_t free_size;

	if (payload_size == 0) {
		return 0;
	}

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., write cansel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., write cansel");
		return -ESHUTDOWN;
	}

	if (payload_size > APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH)) {
		EMSG("invalid payload_size <size:%d> ..., write cansel", payload_size);
		return -EIO;
	}

	/*
	* FIXME: Currently, only one message can be written per write() call.
	* TODO: Allow multiple messages to be written at once in a single write() call.
	*/
	msg_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(payload_size);

	memcpy(msg.payload, payload, payload_size);
	msg.id = dev->unique_func.get_write_cmd_id();
	msg.payload_size = payload_size;
	result = apt_usbtrx_msg_pack(&msg, data, msg_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		return -EIO;
	}

	empty = apt_usbtrx_ringbuffer_is_empty(&dev->tx_data);
	free_size = apt_usbtrx_ringbuffer_get_free_size(&dev->tx_data);
	if (free_size < msg_size) {
		EMSG("write buffer is full");
		return -EIO;
	}

	wsize = apt_usbtrx_ringbuffer_write(&dev->tx_data, data, msg_size);
	if (wsize < 0) {
		EMSG("apt_usbtrx_ringbuffer_write().. Error");
		return -EIO;
	}

	if (empty == true) {
		wake_up_interruptible(&dev->tx_data.wq);
	}

	return payload_size;
}
/*!
 * @brief write
 */
ssize_t apt_usbtrx_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	apt_usbtrx_dev_t *dev;
	u8 payload_size;
	apt_usbtrx_msg_t msg;
	int result;

	dev = file->private_data;
	if (dev == NULL) {
		EMSG("dev is NULL");
		return -ENODEV;
	}

	if (count == 0) {
		return 0;
	}

	/* copy from user */
	payload_size = dev->unique_func.get_write_payload_size(buffer); /* buffer not used */
	if (payload_size > APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH)) {
		EMSG("invalid payload_size <size:%d> ..., write cansel", payload_size);
		return -EIO;
	}

	if (payload_size != count) {
		EMSG("write size is not payload size..., write cansel");
		return -EIO;
	}

	result = copy_from_user(msg.payload, buffer, payload_size);
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EIO;
	}

	result = apt_usbtrx_write_tx_rb(dev, msg.payload, payload_size);
	if (result < 0) {
		EMSG("apt_usbtrx_write_rb().. Error");
		return result;
	}

	return count;
}

/*!
 * @brief ioctl
 */
long apt_usbtrx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	apt_usbtrx_dev_t *dev;
	int result;
	bool onopening;
	bool onclosing;

	CHKMSG("ENTER");

	dev = file->private_data;
	if (dev == NULL) {
		EMSG("dev is NULL");
		return -ENODEV;
	}

	DMSG("%s(): cmd=0x%02x", __func__, _IOC_NR(cmd));

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., ioctl cansel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., ioctl cansel");
		return -ESHUTDOWN;
	}

	switch (cmd) {
	case APT_USBTRX_IOCTL_GET_DEVICE_ID: {
		apt_usbtrx_ioctl_get_device_id_t param;
		int device_id_size = 4;
		char device_id[device_id_size];
		int channel;

		result = apt_usbtrx_get_device_id(dev, device_id, device_id_size, &channel);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_device_id().. Error");
			return -EIO;
		}

		param.channel = channel;
		memcpy(param.id, device_id, device_id_size);

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_device_id_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_SERIAL_NO: {
		apt_usbtrx_ioctl_get_serial_no_t param;
		int serial_no_size = 14;
		char serial_no[serial_no_size];
		int channel;
		int sync_pulse;

		result = apt_usbtrx_get_serial_no(dev, serial_no, serial_no_size, &channel, &sync_pulse);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_device_id().. Error");
			return -EIO;
		}

		param.channel = channel;
		param.sync_pulse = sync_pulse;
		memcpy(param.sn, serial_no, serial_no_size);

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_serial_no_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_FW_VERSION: {
		apt_usbtrx_ioctl_get_fw_version_t param;
		int major;
		int minor;

		result = apt_usbtrx_get_fw_version(dev, &major, &minor);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_fw_version().. Error");
			return -EIO;
		}

		param.major = major;
		param.minor = minor;

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_fw_version_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION: {
		apt_usbtrx_ioctl_get_fw_version_revision_t param;
		int major;
		int minor;
		int revision;

		result = apt_usbtrx_get_fw_version_revision(dev, &major, &minor, &revision);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_fw_version_revision().. Error");
			return -EIO;
		}

		param.major = major;
		param.minor = minor;
		param.revision = revision;

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_fw_version_revision_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_ENABLE_RESET_TS: {
		bool success;

		result = apt_usbtrx_enable_reset_ts(dev, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_enable_reset_ts().. Error");
			return -EIO;
		}
		if (success != true) {
			EMSG("apt_usbtrx_enable_reset_ts().. Error, Exec failed");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_RESET_TS: {
		apt_usbtrx_ioctl_reset_ts_t param;
		bool success;

		result = apt_usbtrx_reset_ts(dev, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_reset_ts().. Error, <result:%d>", result);
			return -EIO;
		}
		if (success != true) {
			WMSG("apt_usbtrx_reset_ts().. Error, Exec failed");
			return -EIO;
		}

		param.ts = dev->basetime;
		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_reset_ts_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_RESET_DEVICE: {
		bool success;

		result = apt_usbtrx_reset_device(dev, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_reset_device().. Error");
			return -EIO;
		}
		if (success != true) {
			EMSG("apt_usbtrx_reset_device().. Error, Exec failed");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_BASETIME: {
		apt_usbtrx_ioctl_get_basetime_t param;

		param.basetime = dev->basetime;
		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_basetime_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_SET_BASETIME: {
		apt_usbtrx_ioctl_set_basetime_t param;

		result = copy_from_user(&param, (void __user *)arg, sizeof(apt_usbtrx_ioctl_set_basetime_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}
		dev->basetime = param.basetime;
		break;
	}
	case APT_USBTRX_IOCTL_MOVE_DFU: {
		bool success;
		bool start;

		result = dev->unique_func.is_device_input_start(dev, &start);
		if (result != RESULT_Success) {
			EMSG("is_device_input_start().. Error");
			return -EIO;
		}
		if (start) {
			EMSG("stop device, then move dfu");
			return -EBUSY;
		}

		result = apt_usbtrx_move_dfu(dev, &success);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_move_dfu().. Error");
			return -EIO;
		}
		if (success != true) {
			EMSG("apt_usbtrx_move_dfu().. Error, Exec failed");
			return -EIO;
		}
		break;
	}
	case APT_USBTRX_IOCTL_SET_TIMESTAMP_MODE: {
		apt_usbtrx_ioctl_set_timestamp_mode_t param;
		bool start;

		result = copy_from_user(&param, (void __user *)arg, sizeof(apt_usbtrx_ioctl_set_timestamp_mode_t));
		if (result != 0) {
			EMSG("copy_from_user().. Error");
			return -EFAULT;
		}

		// check params
		if (param.timestamp_mode < 0 || APT_USBTRX_TIMESTAMP_MODE_MAX <= param.timestamp_mode) {
			EMSG("invalid timestamp mode");
			return -EINVAL;
		}

		result = dev->unique_func.is_device_input_start(dev, &start);
		if (result != RESULT_Success) {
			EMSG("is_device_input_start().. Error");
			return -EIO;
		}
		if (start) {
			EMSG("stop device, then set timestamp_mode");
			return -EBUSY;
		}

		dev->timestamp_mode = param.timestamp_mode;
		DMSG("%s(): timestamp_mode=%d", __func__, param.timestamp_mode);
		break;
	}
	case APT_USBTRX_IOCTL_GET_TIMESTAMP_MODE: {
		apt_usbtrx_ioctl_get_timestamp_mode_t param;
		param.timestamp_mode = dev->timestamp_mode;

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_timestamp_mode_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	case APT_USBTRX_IOCTL_GET_FIRMWARE_SIZE: {
		apt_usbtrx_ioctl_get_firmware_size_t param;
		param.firmware_size = dev->unique_func.get_fw_size();

		result = copy_to_user((void __user *)arg, &param, sizeof(apt_usbtrx_ioctl_get_firmware_size_t));
		if (result != 0) {
			EMSG("copy_to_user().. Error");
			return -EFAULT;
		}
		break;
	}
	default:
		return dev->unique_func.ioctl(file, cmd, arg);
	}

	CHKMSG("LEAVE");
	return 0;
}

/*!
 * @brief write fw data
 */
ssize_t apt_usbtrx_write_fw_data(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	apt_usbtrx_dev_t *dev;
	u8 data_size = APT_USBTRX_CMD_LENGTH_SEND_FW_DATA;
	u8 data[data_size];
	u8 payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_LENGTH_SEND_FW_DATA);
	apt_usbtrx_msg_t msg;
	int result;
	bool onopening;
	bool onclosing;

	dev = file->private_data;
	if (dev == NULL) {
		EMSG("dev is NULL");
		return -ENODEV;
	}

	if (count == 0) {
		return 0;
	}

	if (count != 32) {
		return -EIO;
	}

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., write cansel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., write cansel");
		return -ESHUTDOWN;
	}

	result = copy_from_user(msg.payload, buffer, count);
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EIO;
	}

	result = down_interruptible(&dev->tx_usb_transfer_sem);
	if (result != 0) {
		EMSG("down_interruptible().. Error");
		return -EIO;
	}

	dev->fw_count += count;
	if (dev->fw_count == count) {
		msg.id = APT_USBTRX_CMD_SendFWDataS;
		IMSG("fw update...");
	} else if (dev->fw_count == dev->unique_func.get_fw_size()) {
		msg.id = APT_USBTRX_CMD_SendFWDataE;
		IMSG("fw update... done");
	} else {
		msg.id = APT_USBTRX_CMD_SendFWData;
	}

	msg.payload_size = payload_size;
	result = apt_usbtrx_msg_pack(&msg, data, data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		up(&dev->tx_usb_transfer_sem);
		return -EIO;
	}

	result = apt_usbtrx_setup_tx_urb(dev, data, data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_setup_tx_urb().. Error");
		up(&dev->tx_usb_transfer_sem);
		return -EIO;
	}

	return count;
}
