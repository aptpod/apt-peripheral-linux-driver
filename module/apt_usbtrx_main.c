// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT USB peripherals.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/completion.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/kthread.h>

#include "version.h"
#include "apt_usbtrx_def.h"
#include "apt_usbtrx_fops.h"
#include "apt_usbtrx_cmd.h"
#include "apt_usbtrx_core.h"
#include "apt_usbtrx_sysfs.h"
#include "apt_usbtrx_ioctl.h"

#include "ap_ct2a/ap_ct2a.h"
#include "ep1_ch02a/ep1_ch02a.h"
#include "ep1_cf02a/ep1_cf02a.h"
#include "ep1_ag08a/ep1_ag08a.h"

/*!
 * @brief reset time (global val)
 */
static struct timespec64 g_resettime;

/*!
 * @brief file operation structure
 */
static const struct file_operations apt_usbtrx_fops = {
	.owner = THIS_MODULE,
	.read = apt_usbtrx_read,
	.write = apt_usbtrx_write,
	.open = apt_usbtrx_open,
	.release = apt_usbtrx_release,
	.unlocked_ioctl = apt_usbtrx_ioctl,
	.compat_ioctl = apt_usbtrx_ioctl,
};

/*!
 * @brief class driver structure
 */
static struct usb_class_driver apt_usbtrx_class = {
	.name = "aptUSB%d",
	.fops = &apt_usbtrx_fops,
	.minor_base = APT_USBTRX_MINOR_BASE,
};

/*!
 * @brief file operation structure (DFU mode)
 */
static const struct file_operations apt_usbtrx_dfu_fops = {
	.owner = THIS_MODULE,
	.read = apt_usbtrx_read,
	.write = apt_usbtrx_write_fw_data,
	.open = apt_usbtrx_open,
	.release = apt_usbtrx_release,
	.unlocked_ioctl = apt_usbtrx_ioctl,
	.compat_ioctl = apt_usbtrx_ioctl,
};

/*!
 * @brief class driver structure (DFU mode)
 */
static struct usb_class_driver apt_usbtrx_dfu_class = {
	.name = "aptDFU%d",
	.fops = &apt_usbtrx_dfu_fops,
	.minor_base = APT_USBTRX_MINOR_BASE,
};

/*!
 * @brief device table
 */
static const struct usb_device_id apt_usbtrx_table[] = {
	// clang-format off
	{ USB_DEVICE(APTPOD_VENDOR_ID, AP_CT2A_PRODUCT_ID) },
	{ USB_DEVICE(APTPOD_DEVP_VENDOR_ID, AP_CT2A_DEVP_PRODUCT_ID) },
	{ USB_DEVICE(APTPOD_VENDOR_ID, EP1_CH02A_PRODUCT_ID) },
	{ USB_DEVICE(APTPOD_VENDOR_ID, EP1_AG08A_PRODUCT_ID) },
	{ USB_DEVICE(APTPOD_VENDOR_ID, EP1_CF02A_PRODUCT_ID) },
	{ }
	// clang-format on
};
MODULE_DEVICE_TABLE(usb, apt_usbtrx_table);

void apt_usbtrx_delete(struct kref *kref)
{
	apt_usbtrx_dev_t *dev = to_apt_usbtrx_dev(kref);
	kfree(dev);
}

STATIC bool apt_usbtrx_is_dfu(struct usb_interface *intf)
{
	const struct usb_host_interface *iface_desc = intf->cur_altsetting;
	return iface_desc->desc.bInterfaceClass == 0xFF && iface_desc->desc.bInterfaceSubClass == 1;
}

STATIC void apt_usntrx_write_bulk_callback_null(struct urb *urb)
{
	return;
}

/*!
 * @brief initialize function pointer
 */
STATIC int apt_usbtrx_init_function(struct usb_interface *intf, apt_usbtrx_dev_t *dev)
{
	struct usb_device *usb_dev = interface_to_usbdev(intf);
	const u16 product_id = le16_to_cpu(usb_dev->descriptor.idProduct);
	DMSG("product_id = 0x%04x", product_id);

	switch (product_id) {
	case AP_CT2A_PRODUCT_ID:
	case AP_CT2A_DEVP_PRODUCT_ID:
		dev->device_type = APT_USBTRX_DEVICE_TYPE_CAN;
		dev->rx_data_size = AP_CT2A_RXDATA_BUFFER_SIZE;
		dev->unique_func = (apt_usbtrx_device_unique_function_t){
			.init_data = apt_usbtrx_unique_can_init_data,
			.free_data = apt_usbtrx_unique_can_free_data,
			.init = apt_usbtrx_unique_can_init,
			.terminate = apt_usbtrx_unique_can_terminate,
			.is_need_init_reset_ts = apt_usbtrx_unique_can_is_need_init_reset_ts,
			.dispatch_msg = apt_usbtrx_unique_can_dispatch_msg,
			.get_read_payload_size = apt_usbtrx_unique_can_get_read_payload_size,
			.get_write_payload_size = apt_usbtrx_unique_can_get_write_payload_size,
			.get_read_payload_timestamp = apt_usbtrx_unique_can_get_read_payload_timestamp,
			.get_write_cmd_id = apt_usbtrx_unique_can_get_write_cmd_id,
			.get_fw_size = apt_usbtrx_unique_can_get_fw_size,
			.is_device_input_start = apt_usbtrx_unique_can_is_device_input_start,
			.ioctl = apt_usbtrx_unique_can_ioctl,
			.sysfs_init = apt_usbtrx_unique_can_sysfs_init,
			.sysfs_term = apt_usbtrx_unique_can_sysfs_term,
			.open = apt_usbtrx_unique_can_open,
			.close = apt_usbtrx_unique_can_close,
			.write_bulk_callback = apt_usbtrx_unique_can_write_bulk_callback,
		};
		break;
	case EP1_CH02A_PRODUCT_ID:
		dev->device_type = APT_USBTRX_DEVICE_TYPE_CAN;
		dev->rx_data_size = AP_CT2A_RXDATA_BUFFER_SIZE;
		dev->unique_func = (apt_usbtrx_device_unique_function_t){
			.init_data = apt_usbtrx_unique_can_init_data,
			.free_data = apt_usbtrx_unique_can_free_data,
			.init = ep1_ch02a_init,
			.terminate = ep1_ch02a_terminate,
			.is_need_init_reset_ts = apt_usbtrx_unique_can_is_need_init_reset_ts,
			.dispatch_msg = ep1_ch02a_dispatch_msg,
			.get_read_payload_size = apt_usbtrx_unique_can_get_read_payload_size,
			.get_write_payload_size = apt_usbtrx_unique_can_get_write_payload_size,
			.get_read_payload_timestamp = apt_usbtrx_unique_can_get_read_payload_timestamp,
			.get_write_cmd_id = apt_usbtrx_unique_can_get_write_cmd_id,
			.get_fw_size = apt_usbtrx_unique_can_get_fw_size,
			.is_device_input_start = apt_usbtrx_unique_can_is_device_input_start,
			.ioctl = ep1_ch02a_ioctl,
			.sysfs_init = apt_usbtrx_unique_can_sysfs_init,
			.sysfs_term = apt_usbtrx_unique_can_sysfs_term,
			.open = apt_usbtrx_unique_can_open,
			.close = apt_usbtrx_unique_can_close,
			.write_bulk_callback = apt_usbtrx_unique_can_write_bulk_callback,
		};
		break;
	case EP1_CF02A_PRODUCT_ID:
		dev->device_type = APT_USBTRX_DEVICE_TYPE_CAN_FD;
		dev->rx_data_size = EP1_CF02A_RXDATA_BUFFER_SIZE;
		dev->unique_func = (apt_usbtrx_device_unique_function_t){
			.init_data = ep1_cf02a_init_data,
			.free_data = ep1_cf02a_free_data,
			.init = ep1_cf02a_init,
			.terminate = ep1_cf02a_terminate,
			.is_need_init_reset_ts = ep1_cf02a_is_need_init_reset_ts,
			.dispatch_msg = ep1_cf02a_dispatch_msg,
			.get_read_payload_size = ep1_cf02a_get_read_payload_size,
			.get_write_payload_size = ep1_cf02a_get_write_payload_size,
			.get_read_payload_timestamp = ep1_cf02a_get_read_payload_timestamp,
			.get_write_cmd_id = ep1_cf02a_get_write_cmd_id,
			.get_fw_size = ep1_cf02a_get_fw_size,
			.is_device_input_start = ep1_cf02a_is_device_start,
			.ioctl = ep1_cf02a_ioctl,
			.sysfs_init = ep1_cf02a_sysfs_init,
			.sysfs_term = ep1_cf02a_sysfs_term,
			.open = ep1_cf02a_open,
			.close = ep1_cf02a_close,
			.write_bulk_callback = ep1_cf02a_write_bulk_callback,
		};
		break;
	case EP1_AG08A_PRODUCT_ID:
		dev->device_type = APT_USBTRX_DEVICE_TYPE_ANALOG;
		dev->rx_data_size = EP1_AG08A_RXDATA_BUFFER_SIZE;
		dev->unique_func = (apt_usbtrx_device_unique_function_t){
			.init_data = ep1_ag08a_init_data,
			.free_data = ep1_ag08a_free_data,
			.init = ep1_ag08a_init,
			.terminate = ep1_ag08a_terminate,
			.is_need_init_reset_ts = ep1_ag08a_is_need_init_reset_ts,
			.dispatch_msg = ep1_ag08a_dispatch_msg,
			.get_read_payload_size = ep1_ag08a_get_read_payload_size,
			.get_write_payload_size = ep1_ag08a_get_write_payload_size,
			.get_read_payload_timestamp = ep1_ag08a_get_read_payload_timestamp,
			.get_write_cmd_id = ep1_ag08a_get_write_cmd_id,
			.get_fw_size = ep1_ag08a_get_fw_size,
			.is_device_input_start = ep1_ag08a_is_device_input_start,
			.ioctl = ep1_ag08a_ioctl,
			.sysfs_init = ep1_ag08a_sysfs_init,
			.sysfs_term = ep1_ag08a_sysfs_term,
			.open = ep1_ag08a_open,
			.close = ep1_ag08a_close,
			.write_bulk_callback = ep1_ag08a_write_bulk_callback,
		};
		break;
	default:
		EMSG("Unsupported product ID");
		return RESULT_Failure;
	}

	/* TODO: setup DFU functions */
	if (apt_usbtrx_is_dfu(intf)) {
		dev->unique_func.write_bulk_callback = apt_usntrx_write_bulk_callback_null;
	}

	return RESULT_Success;
}

/*!
 * @brief initialize device info structure
 */
STATIC int apt_usbtrx_init_instance(apt_usbtrx_dev_t *dev)
{
	int idx;
	int result;

	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	dev->udev = NULL;
	dev->interface = NULL;
	dev->bulk_in = NULL;
	dev->bulk_out = NULL;
	/*** dev->rx_submitted ***/
	/*** dev->tx_submitted ***/
	dev->max_rx_urbs = MAX_RX_URBS;
	for (idx = 0; idx < MAX_RX_URBS; idx++) {
		dev->rxbuf[idx] = NULL;
	}
	memset(dev->rxbuf_dma, 0, sizeof(dma_addr_t) * MAX_RX_URBS);
	/*** rx_transfer ***/
	dev->rx_transfer.buffer_size = RX_BUFFER_SIZE * 2;
	dev->rx_transfer.buffer = NULL;
	dev->rx_transfer.data_size = 0;
	/*** rx_complete ***/
	init_completion(&dev->rx_complete.complete);
	dev->rx_complete.buffer_size = RX_BUFFER_SIZE;
	dev->rx_complete.buffer = NULL;
	dev->rx_complete.data_size = 0;

	atomic_set(&dev->rx_ongoing, false);
	dev->basetime_clock_id = CLOCK_MONOTONIC_RAW;
	dev->basetime.tv_sec = 0;
	dev->basetime.tv_nsec = 0;
	atomic_set(&dev->onopening, true);
	atomic_set(&dev->onclosing, false);
	dev->resettime = &g_resettime;
	dev->fw_count = 0;
	sema_init(&dev->tx_usb_transfer_sem, MAX_TX_URBS);
	dev->tx_transfer_expired = jiffies;
	dev->tx_transfer_max_token = 0;
	dev->tx_transfer_token = 0;
	dev->tx_thread = NULL;
	dev->ch = 0;
	memset(dev->serial_no, '\0', APT_USBTRX_SERIAL_NO_LENGTH + 1);
	dev->sync_pulse = APT_USBTRX_SYNC_PULSE_SOURCE;
	dev->fw_ver.major = 0;
	dev->fw_ver.minor = 0;
	dev->fw_ver.revision = 0;
	atomic_set(&dev->tx_buffer_rate, 0);
	init_completion(&dev->rx_done);
	dev->timestamp_mode = APT_USBTRX_TIMESTAMP_MODE_DEVICE;
	dev->rx_rbmsg = NULL;
	dev->unique_data = NULL;

	result = dev->unique_func.init_data(dev);
	if (result != RESULT_Success) {
		EMSG("init_data().. Error");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief get endpoint info
 */
static int apt_usbtrx_get_endpoints(const struct usb_interface *intf, struct usb_endpoint_descriptor **in,
				    struct usb_endpoint_descriptor **out)
{
	const struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int idx;

	CHKMSG("ENTER");

	iface_desc = intf->cur_altsetting;
	for (idx = 0; idx < iface_desc->desc.bNumEndpoints; idx++) {
		endpoint = &iface_desc->endpoint[idx].desc;
		if (*in == NULL && usb_endpoint_is_bulk_in(endpoint)) {
			*in = endpoint;
			DMSG("endpoint bulk in is found, <idx:%d> address=0x%02x, max packet size=%d", idx,
			     endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
		}
		if (*out == NULL && usb_endpoint_is_bulk_out(endpoint)) {
			*out = endpoint;
			DMSG("endpoint bulk out is found, <idx:%d> address=0x%02x, max packet size=%d", idx,
			     endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
		}
		if (*in != NULL && *out != NULL) {
			CHKMSG("LEAVE");
			return RESULT_Success;
		}
	}

	EMSG("%s: endpoint is not found", __func__);
	return RESULT_Failure;
}

/*!
 * @brief is support revision command
 */
static int is_support_revision_command(int device_type, int major, int minor)
{
	bool support = false;
	switch (device_type) {
	case APT_USBTRX_DEVICE_TYPE_CAN:
		if (major > 2) {
			support = true;
		} else if (major == 2 && minor >= 2) {
			support = true;
		}
		break;
	case APT_USBTRX_DEVICE_TYPE_ANALOG:
	case APT_USBTRX_DEVICE_TYPE_CAN_FD:
		support = true;
		break;
	}

	return support;
}

/*!
 * @brief init
 */
static int apt_usbtrx_init(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	const struct usb_host_interface *iface_desc;
	int result;
	int device_id_size = APT_USBTRX_DEVICE_ID_LENGTH;
	char device_id[device_id_size];
	int serial_no_size = APT_USBTRX_SERIAL_NO_LENGTH + 1;
	char serial_no[serial_no_size];
	int ch;
	int sync_pulse;
	int retry = 3;
	bool success;
	struct timespec64 ts;
	bool dfu = false;

	CHKMSG("ENTER");

	init_usb_anchor(&dev->rx_submitted);
	init_usb_anchor(&dev->tx_submitted);

	iface_desc = intf->cur_altsetting;
	DMSG("%s(): InterfaceClass(0x%02X:0x%02X)", __func__, iface_desc->desc.bInterfaceClass,
	     iface_desc->desc.bInterfaceSubClass);
	if (iface_desc->desc.bInterfaceClass == 0xFF) {
		switch (iface_desc->desc.bInterfaceSubClass) {
		case 0:
			result = usb_register_dev(intf, &apt_usbtrx_class);
			if (result != 0) {
				EMSG("usb_register_dev().. Error, apt_usbtrx_class");
				return RESULT_Failure;
			}
			dfu = false;
			break;
		case 1:
			result = usb_register_dev(intf, &apt_usbtrx_dfu_class);
			if (result != 0) {
				EMSG("usb_register_dev().. Error, apt_usbtrx_dfu_class");
				return RESULT_Failure;
			}
			dfu = true;
			break;
		default:
			EMSG("not support InterfaceClass, (0x%02X:0x%02X)", iface_desc->desc.bInterfaceClass,
			     iface_desc->desc.bInterfaceSubClass);
			return RESULT_Failure;
		}
	} else {
		EMSG("not support InterfaceClass, (0x%02X:0x%02X)", iface_desc->desc.bInterfaceClass,
		     iface_desc->desc.bInterfaceSubClass);
		return RESULT_Failure;
	}

	result = apt_usbtrx_ringbuffer_init(&dev->rx_data, dev->rx_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init().. Error");
		return RESULT_Failure;
	}

	result = apt_usbtrx_setup_rx_urbs(dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_setup_rx_urbs().. Error");
		return RESULT_Failure;
	}

	/*** set basetime ***/
	get_ts64(dev, &ts);
	dev->basetime = ts;
	IMSG("inittime: %lld.%09ld", (s64)ts.tv_sec, ts.tv_nsec);

	if (dfu == true) {
		IMSG("DFU mode...");
		atomic_set(&dev->onopening, false);
		return RESULT_Success;
	}

	/*** retry command !! only immediately after plug ***/
	mdelay(10);
	do {
		memset(device_id, 0, device_id_size);
		result = apt_usbtrx_get_device_id(dev, device_id, device_id_size, &ch);
		if (result == RESULT_Success) {
			break;
		}
		WMSG("apt_usbtrx_get_device_id().. Retry(%d)", retry);
	} while (--retry > 0);

	if (retry <= 0) {
		EMSG("apt_usbtrx_get_device_id().. Error");
		return RESULT_Failure;
	}
	dev->ch = ch;

	{
		char linebuf[3 * APT_USBTRX_CMD_DEVICE_ID_LENGTH + 1];

		hex_dump_to_buffer(device_id, APT_USBTRX_CMD_DEVICE_ID_LENGTH, 16, 1, linebuf, sizeof(linebuf), 0);
		IMSG("ch=%d, device id=%s", ch, linebuf);
	}

	memset(serial_no, '\0', serial_no_size);
	result = apt_usbtrx_get_serial_no(dev, serial_no, serial_no_size, &ch, &sync_pulse);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_get_serial_no().. Error");
		return RESULT_Failure;
	}
	memcpy(dev->serial_no, serial_no, serial_no_size);
	dev->sync_pulse = sync_pulse;
	IMSG("ch=%d, sync pulse=%d, serial no=%s", ch, sync_pulse, serial_no);

	/* version logging */
	result = apt_usbtrx_get_fw_version(dev, &dev->fw_ver.major, &dev->fw_ver.minor);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_get_fw_version().. Error");
		return RESULT_Failure;
	}
	if (is_support_revision_command(dev->device_type, dev->fw_ver.major, dev->fw_ver.minor)) {
		result = apt_usbtrx_get_fw_version_revision(dev, &dev->fw_ver.major, &dev->fw_ver.minor,
							    &dev->fw_ver.revision);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_get_fw_version_revision().. Error");
			return RESULT_Failure;
		} else {
			IMSG("FW ver.%d.%d.%d", dev->fw_ver.major, dev->fw_ver.minor, dev->fw_ver.revision);
		}
	} else {
		IMSG("FW ver.%d.%d", dev->fw_ver.major, dev->fw_ver.minor);
	}

	result = apt_usbtrx_ringbuffer_init(&dev->tx_data, APT_USBTRX_TXDATA_BUFFER_SIZE);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init().. Error");
		return RESULT_Failure;
	}

	dev->tx_thread = kthread_run(apt_usbtrx_tx_thread_func, dev, "apt_tx_thread");
	if (dev->tx_thread == NULL) {
		EMSG("kthread_run().. Error");
		return RESULT_Failure;
	}

	result = apt_usbtrx_enable_reset_ts(dev, &success);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_enable_reset_ts().. Error");
		return RESULT_Failure;
	}
	if (success != true) {
		EMSG("apt_usbtrx_enable_reest_ts().. Error, Exec Faled");
		return RESULT_Failure;
	}

	if (dev->unique_func.is_need_init_reset_ts(dev) && sync_pulse == 0) {
		result = apt_usbtrx_reset_ts(dev, &success);
		if (result != RESULT_Success) {
			if (result == RESULT_Timeout) {
				WMSG("apt_usbtrx_reset_ts().. Timeout");
			} else {
				EMSG("apt_usbtrx_reset_ts().. Error");
				return RESULT_Failure;
			}
		}
		if (success != true) {
			WMSG("apt_usbtrx_reset_ts().. Error, Exec Faled");
		}
	}

	result = dev->unique_func.init(intf, id);
	if (result != RESULT_Success) {
		EMSG("init().. Error");
		return RESULT_Failure;
	}

	atomic_set(&dev->onopening, false);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief terminate
 */
static int apt_usbtrx_term(struct usb_interface *intf)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	const struct usb_host_interface *iface_desc;
	int idx;
	int result;
	bool dfu = false;

	CHKMSG("ENTER");

	iface_desc = intf->cur_altsetting;
	if (iface_desc->desc.bInterfaceClass == 0xFF) {
		switch (iface_desc->desc.bInterfaceSubClass) {
		case 0:
			dfu = false;
			break;
		case 1:
			dfu = true;
			break;
		}
	}

	result = dev->unique_func.terminate(dev);
	if (result != RESULT_Success) {
		EMSG("terminate().. Error");
	}

	atomic_set(&dev->onclosing, true);
	atomic_set(&dev->rx_ongoing, false);

	wake_up_interruptible(&dev->rx_data.wq);
	wait_for_completion_interruptible_timeout(&dev->rx_done, msecs_to_jiffies(100));
	if (dfu == false) {
		if (dev->tx_thread != NULL) {
			wake_up_interruptible(&dev->tx_data.wq);
			kthread_stop(dev->tx_thread);
		}
	}

	usb_kill_anchored_urbs(&dev->rx_submitted);
	usb_kill_anchored_urbs(&dev->tx_submitted);

	for (idx = 0; idx < MAX_RX_URBS; idx++) {
		if (dev->rxbuf[idx] != NULL) {
			usb_free_coherent(dev->udev, RX_BUFFER_SIZE, dev->rxbuf[idx], dev->rxbuf_dma[idx]);
			dev->rxbuf[idx] = NULL;
		}
	}

	result = apt_usbtrx_ringbuffer_term(&dev->rx_data);
	if (result != RESULT_Success) {
		WMSG("apt_usbtrx_ringbuffer_term().. Error");
	}
	if (dfu == false) {
		result = apt_usbtrx_ringbuffer_term(&dev->tx_data);
		if (result != RESULT_Success) {
			WMSG("apt_usbtrx_ringbuffer_term().. Error");
		}
		usb_deregister_dev(intf, &apt_usbtrx_class);
	} else {
		usb_deregister_dev(intf, &apt_usbtrx_dfu_class);
	}

	if (dev->rx_transfer.buffer != NULL) {
		kfree(dev->rx_transfer.buffer);
	}
	if (dev->rx_complete.buffer != NULL) {
		kfree(dev->rx_complete.buffer);
	}
	if (dev->rx_rbmsg != NULL) {
		kfree(dev->rx_rbmsg);
	}
	result = dev->unique_func.free_data(dev);
	if (result != RESULT_Success) {
		EMSG("free_data().. Error");
	}

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief probe
 */
static int apt_usbtrx_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = NULL;
	int result;
	int retval = -ENOMEM;

	IMSG("EDGEPLANT USB Interface Device Driver Ver.%s", PRODUCT_VERSION);

	CHKMSG("ENTER");

	usb_set_intfdata(intf, NULL);
	dev = kzalloc(sizeof(apt_usbtrx_dev_t), GFP_KERNEL);
	if (dev == NULL) {
		EMSG("devm_kzalloc().. Error, <size:%zu>", sizeof(apt_usbtrx_dev_t));
		goto error;
	}

	kref_init(&dev->kref);

	result = apt_usbtrx_init_function(intf, dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_init_function().. Error");
		goto error;
	}

	result = apt_usbtrx_init_instance(dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_init_instance().. Error");
		goto error;
	}

	dev->rx_transfer.buffer = kzalloc(dev->rx_transfer.buffer_size, GFP_KERNEL);
	if (dev->rx_transfer.buffer == NULL) {
		EMSG("kzalloc().. Error, <size:%d>", dev->rx_transfer.buffer_size);
		goto error;
	}

	dev->rx_complete.buffer = kzalloc(dev->rx_complete.buffer_size, GFP_KERNEL);
	if (dev->rx_complete.buffer == NULL) {
		EMSG("kzalloc().. Error, <size:%d>", dev->rx_complete.buffer_size);
		goto error;
	}

	dev->rx_rbmsg = kzalloc(dev->rx_data_size, GFP_KERNEL);
	if (dev->rx_rbmsg == NULL) {
		EMSG("kzalloc().. Error, <size:%zu>", dev->rx_data_size);
		goto error;
	}

	result = apt_usbtrx_get_endpoints(intf, &dev->bulk_in, &dev->bulk_out);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_get_endponts().. Error");
		goto error;
	}

	dev->udev = interface_to_usbdev(intf);
	dev->interface = intf;
	usb_set_intfdata(intf, dev);

	result = apt_usbtrx_init(intf, id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_init().. Error");
		result = apt_usbtrx_term(intf);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_term().. Eror");
		}
		usb_set_intfdata(intf, NULL);
		retval = -EIO;
		goto error;
	}

	result = apt_usbtrx_sysfs_init(&intf->dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_sysfs_init().. Error");
	}

	DMSG("%s(): minor=%d", __func__, intf->minor);
	CHKMSG("LEAVE");
	return 0;

error:
	if (dev) {
		/* this frees allocated memory */
		kref_put(&dev->kref, apt_usbtrx_delete);
	}
	return retval;
}

/*!
 * @brief disconnect
 */
static void apt_usbtrx_disconnect(struct usb_interface *intf)
{
	apt_usbtrx_dev_t *dev;
	int result;

	CHKMSG("ENTER");

	IMSG("disconnect: minor=%d", intf->minor);
	DMSG("%s(): minor=%d", __func__, intf->minor);
	dev = usb_get_intfdata(intf);
	if (dev == NULL) {
		WMSG("usb_get_infdata().. Error, dev is NULL");
		return;
	}

	result = apt_usbtrx_term(intf);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_term().. Error");
	}

	result = apt_usbtrx_sysfs_term(&intf->dev);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_sysfs_term().. Error");
	}

	usb_set_intfdata(intf, NULL);
	kref_put(&dev->kref, apt_usbtrx_delete);

	CHKMSG("LEAVE");
}

/*!
 * @biref USB device driver entry
 */
struct usb_driver apt_usbtrx_driver = {
	.name = "apt_usbtrx",
	.id_table = apt_usbtrx_table,
	.probe = apt_usbtrx_probe,
	.disconnect = apt_usbtrx_disconnect,
};

module_usb_driver(apt_usbtrx_driver);

MODULE_AUTHOR("aptpod Inc.");
MODULE_DESCRIPTION("EDGEPLANT USB Transceiver driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(PRODUCT_VERSION);
