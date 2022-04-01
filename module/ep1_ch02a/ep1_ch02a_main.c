// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/can/dev.h>

#include "../apt_usbtrx_fops.h"
#include "../ap_ct2a/ap_ct2a_def.h" /* inherit from ap_ct2a(candev) */
#include "../ap_ct2a/ap_ct2a_fops.h" /* inherit netdev functions from ap_ct2a */

#include "ep1_ch02a_main.h"
#include "ep1_ch02a_cmd.h"
#include "ep1_ch02a_fops.h"

/*!
 * @brief is support bittiming command
 */
bool ep1_ch02a_is_support_bittiming_command(const apt_usbtrx_dev_t *dev)
{
	bool support = false;
	if (dev->fw_ver.major > 2) {
		support = true;
	} else if (dev->fw_ver.major == 2 && dev->fw_ver.minor >= 3) {
		support = true;
	}

	return support;
}

#ifdef SUPPORT_NETDEV
/*!
 * @brief net device operation structure
 */
static const struct net_device_ops ep1_ch02a_netdev_ops = {
	.ndo_open = apt_usbtrx_unique_can_netdev_open,
	.ndo_stop = apt_usbtrx_unique_can_netdev_close,
	.ndo_start_xmit = apt_usbtrx_unique_can_netdev_start_xmit,
	.ndo_change_mtu = can_change_mtu,
};

static const struct can_bittiming_const ep1_ch02a_netdev_bittiming_const = {
	.name = "ep1_ch02a",
	.tseg1_min = 1,
	.tseg1_max = 16,
	.tseg2_min = 1,
	.tseg2_max = 8,
	.sjw_max = 4,
	.brp_min = 1,
	.brp_max = 512,
	.brp_inc = 1,
};

static void ep1_ch02a_set_current_bittiming(apt_usbtrx_dev_t *dev, apt_usbtrx_candev_t *candev)
{
	ep1_ch02a_msg_resp_get_bit_timing_t bit_timing;

	candev->can.bittiming.bitrate = AP_CT2A_DEFAULT_BAUDRATE;
	candev->can.clock.freq = AP_CT2A_CLOCK;

	if (ep1_ch02a_is_support_bittiming_command(dev)) {
		if (ep1_ch02a_get_bit_timing(dev, &bit_timing) == RESULT_Success) {
			int nominal = 1 + bit_timing.params.prop_seg + bit_timing.params.phase_seg1 +
				      bit_timing.params.phase_seg2;

			candev->can.clock.freq = bit_timing.can_clock;
			candev->can.bittiming.prop_seg = bit_timing.params.prop_seg;
			candev->can.bittiming.phase_seg1 = bit_timing.params.phase_seg1;
			candev->can.bittiming.phase_seg2 = bit_timing.params.phase_seg2;
			candev->can.bittiming.sjw = bit_timing.params.sjw;
			candev->can.bittiming.brp = bit_timing.params.brp;
			candev->can.bittiming.sample_point = (nominal - bit_timing.params.phase_seg2) * 1000 / nominal;
			candev->can.bittiming.tq =
				(1000 / (bit_timing.can_clock / (1000 * 1000))) * bit_timing.params.brp;
		}
	}
}

/*!
 * @brief create candev
 */
static int ep1_ch02a_create_candev(struct usb_interface *intf, const struct usb_device_id *id)
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
	candev->can.bittiming_const = &ep1_ch02a_netdev_bittiming_const;
	candev->can.do_set_bittiming = ep1_ch02a_netdev_set_bittiming;
	candev->can.do_set_mode = apt_usbtrx_unique_can_netdev_set_mode;

	/* set default bittiming */
	ep1_ch02a_set_current_bittiming(dev, candev);

	/* netdev init */
	netdev->flags |= IFF_ECHO; /* we support local echo */
	netdev->netdev_ops = &ep1_ch02a_netdev_ops;

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
#endif /* SUPPORT_NETDEV */

/*!
 * @brief initialize
 */
int ep1_ch02a_init(struct usb_interface *intf, const struct usb_device_id *id)
{
#ifdef SUPPORT_NETDEV
	int result;

	result = ep1_ch02a_create_candev(intf, id);
	if (result != RESULT_Success) {
		EMSG("ep1_usbtrx_create_candev() failed");
		return RESULT_Failure;
	}
#endif

	return RESULT_Success;
}

/*!
 * @brief terminalte
 */
int ep1_ch02a_terminate(apt_usbtrx_dev_t *dev)
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
