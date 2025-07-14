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
	result = apt_usbtrx_ringbuffer_init(&unique_data->rx_store_data, unique_data->rx_store_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init().. Error");
		kfree(unique_data->bittiming);
		kfree(unique_data->data_bittiming);
		kfree(unique_data);
		return RESULT_Failure;
	}
	init_completion(&unique_data->rx_store_data_done);
	unique_data->is_store_data_enabled = false;

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
