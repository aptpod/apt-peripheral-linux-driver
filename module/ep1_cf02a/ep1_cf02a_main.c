// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/can/dev.h>

#include "ep1_cf02a_main.h"
#include "ep1_cf02a_def.h"
#include "ep1_cf02a_cmd.h"
#include "ep1_cf02a_fops.h"

static const struct can_bittiming_const ep1_cf02a_bittiming_const = {
	.name = "apt_usbtrx",
	.tseg1_min = 1,
	.tseg1_max = 256,
	.tseg2_min = 1,
	.tseg2_max = 128,
	.sjw_max = 128,
	.brp_min = 1,
	.brp_max = 512,
	.brp_inc = 1,
};

static const struct can_bittiming_const ep1_cf02a_data_bittiming_const = {
	.name = "apt_usbtrx",
	.tseg1_min = 1,
	.tseg1_max = 32,
	.tseg2_min = 1,
	.tseg2_max = 16,
	.sjw_max = 16,
	.brp_min = 1,
	.brp_max = 32,
	.brp_inc = 1,
};

#ifdef SUPPORT_NETDEV
/*!
 * @brief net device operation structure
 */
static const struct net_device_ops ep1_cf02a_netdev_ops = {
	.ndo_open = ep1_cf02a_netdev_open,
	.ndo_stop = ep1_cf02a_netdev_close,
	.ndo_start_xmit = ep1_cf02a_netdev_start_xmit,
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 19, 0)
	.ndo_change_mtu = can_change_mtu,
#endif
	.ndo_get_stats64 = ep1_cf02a_netdev_get_stats64,
};

static void ep1_cf02a_bit_timing_to_can_bittiming(apt_usbtrx_dev_t *dev, const ep1_cf02a_msg_bit_timing_t *src,
						  struct can_bittiming *dst)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	dst->prop_seg = src->prop_seg;
	dst->phase_seg1 = src->phase_seg1;
	dst->phase_seg2 = src->phase_seg2;
	dst->sjw = src->sjw;
	dst->brp = src->brp;

	{
		int sync_seg = EP1_CF02A_CAN_SYNC_SEG;
		int prop_seg = dst->prop_seg;
		int phase_seg1 = dst->phase_seg1;
		int phase_seg2 = dst->phase_seg2;
		int brp = dst->brp;
		int all_tseg = sync_seg + prop_seg + phase_seg1 + phase_seg2;

		dst->bitrate = unique_data->can_clock / (brp * all_tseg);
		dst->sample_point = (1000 * (sync_seg + prop_seg + phase_seg1)) / all_tseg;
		dst->tq = div_u64((u64)brp * 1000000000, unique_data->can_clock);
	}
}

/*!
 * @brief create candev
 */
static int ep1_cf02a_create_candev(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	struct net_device *netdev;
	ep1_cf02a_candev_t *candev;
	int err = 0;

	netdev = alloc_candev(sizeof(ep1_cf02a_candev_t), 1);
	if (!netdev) {
		EMSG("ep1_cf02a_create_candev().. Error, couldn't alloc candev");
		return -ENOMEM;
	}

	/* candev init */
	candev = netdev_priv(netdev);
	candev->dev = dev;
	candev->reset_ts.tv_sec = 0;
	candev->reset_ts.tv_nsec = 0;
	atomic64_set(&candev->rx_packets, 0);
	atomic64_set(&candev->rx_bytes, 0);
	atomic64_set(&candev->tx_packets, 0);
	atomic64_set(&candev->tx_bytes, 0);
	atomic64_set(&candev->fw_rx_dropped, 0);
	INIT_DELAYED_WORK(&candev->statistics_work, ep1_cf02a_statistics_work_func);

	/* can_priv init */
	candev->can.state = CAN_STATE_STOPPED;
	candev->can.ctrlmode_supported = CAN_CTRLMODE_LISTENONLY | CAN_CTRLMODE_FD | CAN_CTRLMODE_FD_NON_ISO;
	candev->can.clock.freq = unique_data->can_clock;
	candev->can.bittiming_const = unique_data->bittiming_const;
	candev->can.do_set_bittiming = ep1_cf02a_netdev_set_bittiming;
	candev->can.do_set_mode = ep1_cf02a_netdev_set_mode;
	candev->can.do_get_state = ep1_cf02a_netdev_get_state;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
	candev->can.fd.data_bittiming_const = unique_data->data_bittiming_const;
	candev->can.fd.do_set_data_bittiming = ep1_cf02a_netdev_set_data_bittiming;
#else
	candev->can.data_bittiming_const = unique_data->data_bittiming_const;
	candev->can.do_set_data_bittiming = ep1_cf02a_netdev_set_data_bittiming;
#endif

	/* set default bittiming */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
	ep1_cf02a_bit_timing_to_can_bittiming(dev, unique_data->data_bittiming, &candev->can.fd.data_bittiming);
#else
	ep1_cf02a_bit_timing_to_can_bittiming(dev, unique_data->data_bittiming, &candev->can.data_bittiming);
#endif

	/* netdev init */
	netdev->flags |= IFF_ECHO; /* we support local echo */
	netdev->netdev_ops = &ep1_cf02a_netdev_ops;

	SET_NETDEV_DEV(netdev, &intf->dev);
	netdev->dev_id = dev->ch;

	unique_data->netdev = netdev;

	err = register_candev(netdev);
	if (err) {
		unique_data->netdev = NULL;
		free_candev(netdev);
		EMSG("register_candev().. Error");
		return err;
	}

	netdev_info(netdev, "device %s registered\n", netdev->name);

	return 0;
}
#endif /* SUPPORT_NETDEV */

/*!
 * @brief initialize unique data
 */
int ep1_cf02a_init_data(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data;
	int result;

	if (dev->unique_data != NULL) {
		EMSG("unique_data is already initialized");
		return RESULT_Failure;
	}

	dev->unique_data = kzalloc(sizeof(ep1_cf02a_unique_data_t), GFP_KERNEL);
	if (dev->unique_data == NULL) {
		EMSG("devm_kzalloc().. Error, <size:%zu>", sizeof(ep1_cf02a_unique_data_t));
		return RESULT_Failure;
	}

	// EP1-CF02A does not implement control based on token size, so a fixed maximum value is set.
	dev->tx_transfer_max_token = (8 * 1000 * 1000 / 8) / EP1_CF02A_CMD_LENGTH_SEND_CAN_FRAME / 1000;

	unique_data = get_unique_data(dev);

	unique_data->bittiming_const = &ep1_cf02a_bittiming_const;
	unique_data->data_bittiming_const = &ep1_cf02a_data_bittiming_const;

	unique_data->can_clock = 0;

	unique_data->bittiming = kzalloc(sizeof(ep1_cf02a_msg_bit_timing_t), GFP_KERNEL);
	if (unique_data->bittiming == NULL) {
		EMSG("kzalloc().. Error, <size:%zu>", sizeof(ep1_cf02a_msg_bit_timing_t));
		kfree(unique_data);
		return RESULT_Failure;
	}

	unique_data->data_bittiming = kzalloc(sizeof(ep1_cf02a_msg_bit_timing_t), GFP_KERNEL);
	if (unique_data->data_bittiming == NULL) {
		EMSG("kzalloc().. Error, <size:%zu>", sizeof(ep1_cf02a_msg_bit_timing_t));
		kfree(unique_data->bittiming);
		kfree(unique_data);
		return RESULT_Failure;
	}

	unique_data->rx_store_data_size = EP1_CF02A_STORE_DATA_BUFFER_SIZE;
	unique_data->can_frame_count_per_request =
		(unique_data->rx_store_data_size - 1) / sizeof(ep1_cf02a_payload_notify_recv_can_frame_t);
	atomic_set(&unique_data->received_store_data_size, 0);
	result = apt_usbtrx_ringbuffer_init(&unique_data->rx_store_data, unique_data->rx_store_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init().. Error");
		kfree(unique_data->bittiming);
		kfree(unique_data->data_bittiming);
		kfree(unique_data);
		return RESULT_Failure;
	}
	init_completion(&unique_data->rx_store_data_done);
	unique_data->notify_store_data_recv_can_frame_complete = false;
	unique_data->is_store_data_enabled = false;

	atomic_set(&unique_data->file_open_count, 0);
	atomic_set(&unique_data->if_type, EP1_CF02A_IF_TYPE_NONE);
	unique_data->netdev = NULL;
	atomic_set(&unique_data->on_terminating, false);

	return RESULT_Success;
}

/*!
 * @brief free unique data
 */
int ep1_cf02a_free_data(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	int result;

	if (unique_data == NULL) {
		EMSG("unique_data is NULL");
		return RESULT_Failure;
	}

	result = apt_usbtrx_ringbuffer_term(&unique_data->rx_store_data);
	if (result != RESULT_Success) {
		WMSG("apt_usbtrx_ringbuffer_term().. Error");
	}

	kfree(unique_data->bittiming);
	unique_data->bittiming = NULL;

	kfree(unique_data->data_bittiming);
	unique_data->data_bittiming = NULL;

	kfree(unique_data);
	unique_data = NULL;

	return RESULT_Success;
}

static int ep1_cf02a_init_current_settings(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	int result;

	ep1_cf02a_msg_get_can_clock_t can_clock;
	ep1_cf02a_msg_get_bit_timing_t bittiming;
	ep1_cf02a_msg_get_data_bit_timing_t data_bittiming;
	ep1_cf02a_msg_get_current_store_data_state_t state;

	/* Get current CAN clock */
	result = ep1_cf02a_get_can_clock(dev, &can_clock);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_can_clock().. Error");
		return RESULT_Failure;
	}
	unique_data->can_clock = can_clock.can_clock;

	/* Get current bit timing */
	result = ep1_cf02a_get_bit_timing(dev, &bittiming);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_bit_timing().. Error");
		return RESULT_Failure;
	}
	memcpy(unique_data->bittiming, &bittiming, sizeof(ep1_cf02a_msg_bit_timing_t));

	/* Get current data bit timing */
	result = ep1_cf02a_get_data_bit_timing(dev, &data_bittiming);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_data_bit_timing().. Error");
		return RESULT_Failure;
	}
	memcpy(unique_data->data_bittiming, &data_bittiming, sizeof(ep1_cf02a_msg_bit_timing_t));

	/* Get store data state */
	result = ep1_cf02a_get_current_store_data_state(dev, &state);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_current_store_data_state().. Error");
		return RESULT_Failure;
	}

	/* Set store data state */
	if (state.state != EP1_CF02A_STORE_DATA_STATE_NOT_SUPPORTED) {
		unique_data->is_store_data_enabled = true;
	} else {
		unique_data->is_store_data_enabled = false;
	}

	return RESULT_Success;
}

/*!
 * @brief initialize
 */
int ep1_cf02a_init(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	int result;
	bool success;

	result = ep1_cf02a_init_current_settings(dev);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_init_current_settings().. Error");
		return RESULT_Failure;
	}

	if (unique_data->is_store_data_enabled) {
		/* Set Initialize host timestamp reset time */
		/* apt_usbtrx_reset_ts() in apt_usbtrx_init() are executed only for the 2ch of master. */
		if (ep1_cf02a_is_need_init_reset_ts(dev) && dev->sync_pulse == 0) {
			/* Set initial host timestamp reset time */
			ep1_cf02a_msg_set_host_timestamp_reset_time_t time;
			time.ts.tv_sec = dev->basetime.tv_sec;
			time.ts.tv_nsec = dev->basetime.tv_nsec;

			result = ep1_cf02a_set_host_timestamp_reset_time(dev, &time, &success);
			if (result != RESULT_Success) {
				EMSG("ep1_cf02a_set_host_timestamp_reset_time().. Error");
				/* Not return error */
			}
			if (success != true) {
				EMSG("ep1_cf02a_set_host_timestamp_reset_time().. Error, Exec failed");
				/* Not return error */
			}
		}
	}

#ifdef SUPPORT_NETDEV
	result = ep1_cf02a_create_candev(intf, id);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_create_candev() failed");
		return RESULT_Failure;
	}
#endif

	return RESULT_Success;
}

/*!
 * @brief terminate
 */
int ep1_cf02a_terminate(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	atomic_set(&unique_data->on_terminating, true);

#ifdef SUPPORT_NETDEV
	if (unique_data->netdev != NULL) {
		ep1_cf02a_candev_t *candev = netdev_priv(unique_data->netdev);

		/* Cancel statistics polling work before unregister */
		cancel_delayed_work_sync(&candev->statistics_work);

		unregister_netdev(unique_data->netdev);
		free_candev(unique_data->netdev);
		unique_data->netdev = NULL;
	}
#endif

	wake_up_interruptible(&unique_data->rx_store_data.wq);
	wait_for_completion_interruptible_timeout(&unique_data->rx_store_data_done, msecs_to_jiffies(100));

	return RESULT_Success;
}

/*!
 * @brief is need init reset ts
 */
bool ep1_cf02a_is_need_init_reset_ts(apt_usbtrx_dev_t *dev)
{
	/* initial reset when find 2ch */
	return dev->ch == 1 ? true : false;
}
