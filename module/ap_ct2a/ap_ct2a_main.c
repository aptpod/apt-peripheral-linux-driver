// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/can/dev.h>

#include "ap_ct2a_main.h"
#include "ap_ct2a_def.h"
#include "ap_ct2a_core.h" /* apt_usbtrx_init_stats() */
#include "ap_ct2a_fops.h"

#ifdef SUPPORT_NETDEV
/*!
 * @brief net device operation structure
 */
static const struct net_device_ops apt_usbtrx_netdev_ops = {
	.ndo_open = apt_usbtrx_unique_can_netdev_open,
	.ndo_stop = apt_usbtrx_unique_can_netdev_close,
	.ndo_start_xmit = apt_usbtrx_unique_can_netdev_start_xmit,
	.ndo_change_mtu = can_change_mtu,
};

static const struct can_bittiming_const apt_usbtrx_netdev_bittiming_const = {
	.name = "apt_usbtrx",
	.tseg1_min = 1,
	.tseg1_max = 16,
	.tseg2_min = 1,
	.tseg2_max = 8,
	.sjw_max = 4,
	.brp_min = 1,
	.brp_max = 1024,
	.brp_inc = 1,
};

/*!
 * @brief create candev
 */
static int apt_usbtrx_unique_can_create_candev(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	struct net_device *netdev;
	apt_usbtrx_candev_t *candev;
	int err = 0;

	netdev = alloc_candev(sizeof(apt_usbtrx_candev_t), 1);
	if (!netdev) {
		EMSG("apt_usbtrx_create_candev().. Error, couldn't alloc candev");
		return -ENOMEM;
	}

	/* candev init */
	candev = netdev_priv(netdev);
	candev->dev = dev;

	/* can_priv init */
	candev->can.state = CAN_STATE_STOPPED;
	candev->can.ctrlmode_supported = CAN_CTRLMODE_LISTENONLY;
	candev->can.clock.freq = AP_CT2A_CLOCK;
	candev->can.bittiming_const = &apt_usbtrx_netdev_bittiming_const;
	candev->can.do_set_bittiming = apt_usbtrx_unique_can_netdev_set_bittiming;
	candev->can.do_set_mode = apt_usbtrx_unique_can_netdev_set_mode;

	/* set default baudrate */
	candev->can.bittiming.bitrate = AP_CT2A_DEFAULT_BAUDRATE;

	/* netdev init */
	netdev->flags |= IFF_ECHO; /* we support local echo */
	netdev->netdev_ops = &apt_usbtrx_netdev_ops;

	SET_NETDEV_DEV(netdev, &intf->dev);
	netdev->dev_id = dev->ch;

	err = register_candev(netdev);
	if (err) {
		free_candev(netdev);
		EMSG("register_candev().. Error");
		return err;
	}

	unique_data->netdev = netdev;

	netdev_info(netdev, "device %s registered\n", netdev->name);

	return 0;
}
#endif

/*!
 * @brief initialize unique data
 */
int apt_usbtrx_unique_can_init_data(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_unique_data_can_t *unique_data;

	if (dev->unique_data != NULL) {
		EMSG("unique_data is already initialized");
		return RESULT_Failure;
	}

	dev->unique_data = kzalloc(sizeof(apt_usbtrx_unique_data_can_t), GFP_KERNEL);
	if (dev->unique_data == NULL) {
		EMSG("devm_kzalloc().. Error, <size:%zu>", sizeof(apt_usbtrx_unique_data_can_t));
		return RESULT_Failure;
	}

	unique_data = get_unique_data(dev);

	unique_data->if_type = APT_USBTRX_CAN_IF_TYPE_NONE;
	apt_usbtrx_init_stats(&unique_data->summary.dat_std);
	apt_usbtrx_init_stats(&unique_data->summary.dat_ext);
	apt_usbtrx_init_stats(&unique_data->summary.rtr_std);
	apt_usbtrx_init_stats(&unique_data->summary.rtr_std);
	apt_usbtrx_init_stats(&unique_data->summary.err);
	unique_data->netdev = NULL;

	return RESULT_Success;
}

/*!
 * @brief free unique data
 */
int apt_usbtrx_unique_can_free_data(apt_usbtrx_dev_t *dev)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);

	if (unique_data == NULL) {
		EMSG("unique_data is NULL");
		return RESULT_Failure;
	}

	kfree(unique_data);
	unique_data = NULL;

	return RESULT_Success;
}

/*!
 * @brief initialize
 */
int apt_usbtrx_unique_can_init(struct usb_interface *intf, const struct usb_device_id *id)
{
#ifdef SUPPORT_NETDEV
	int result;

	result = apt_usbtrx_unique_can_create_candev(intf, id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_unique_can_create_candev() failed");
		return RESULT_Failure;
	}
#endif

	return RESULT_Success;
}

/*!
 * @brief terminalte
 */
int apt_usbtrx_unique_can_terminate(apt_usbtrx_dev_t *dev)
{
#ifdef SUPPORT_NETDEV
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);

	if (unique_data->netdev != NULL) {
		unregister_netdev(unique_data->netdev);
		free_candev(unique_data->netdev);
		unique_data->netdev = NULL;
	}
#endif

	return RESULT_Success;
}

/*!
 * @brief is need init reset ts
 */
bool apt_usbtrx_unique_can_is_need_init_reset_ts(apt_usbtrx_dev_t *dev)
{
	/* initial reset when find 2ch */
	return dev->ch == 1 ? true : false;
}
