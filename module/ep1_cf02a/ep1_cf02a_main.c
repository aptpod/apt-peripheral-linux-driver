// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/can/dev.h>

#include "ep1_cf02a_main.h"
#include "ep1_cf02a_def.h"
#include "ep1_cf02a_core.h" /* apt_usbtrx_init_stats() */
#include "ep1_cf02a_cmd.h"

static const struct can_bittiming_const ep1_cf02a_bittiming_const = {
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

static const struct can_bittiming_const ep1_cf02a_data_bittiming_const = {
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

	ep1_cf02a_init_stats(&unique_data->summary.dat_std);
	ep1_cf02a_init_stats(&unique_data->summary.dat_ext);
	ep1_cf02a_init_stats(&unique_data->summary.rtr_std);
	ep1_cf02a_init_stats(&unique_data->summary.rtr_std);
	ep1_cf02a_init_stats(&unique_data->summary.err);

	unique_data->bittiming_const = &ep1_cf02a_bittiming_const;
	unique_data->data_bittiming_const = &ep1_cf02a_data_bittiming_const;

	unique_data->rx_store_data_size = EP1_CF02A_STORE_DATA_BUFFER_SIZE;
	result = apt_usbtrx_ringbuffer_init(&unique_data->rx_store_data, unique_data->rx_store_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init().. Error");
		return RESULT_Failure;
	}
	init_completion(&unique_data->rx_store_data_done);

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

	kfree(unique_data);
	unique_data = NULL;

	return RESULT_Success;
}

/*!
 * @brief initialize
 */
int ep1_cf02a_init(struct usb_interface *intf, const struct usb_device_id *id)
{
	return RESULT_Success;
}

/*!
 * @brief terminate
 */
int ep1_cf02a_terminate(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

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
