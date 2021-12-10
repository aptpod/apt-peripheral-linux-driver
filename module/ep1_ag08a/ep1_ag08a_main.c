// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/slab.h>

#include "ep1_ag08a_main.h"
#include "ep1_ag08a_def.h"
#include "ep1_ag08a_iio.h"

/*!
 * @brief initialize unique data
 */
int ep1_ag08a_init_data(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_unique_data_t *unique_data;

	if (dev->unique_data != NULL) {
		EMSG("unique_data is already initialized");
		return RESULT_Failure;
	}

	dev->unique_data = kzalloc(sizeof(ep1_ag08a_unique_data_t), GFP_KERNEL);
	if (dev->unique_data == NULL) {
		EMSG("devm_kzalloc().. Error, <size:%zu>", sizeof(ep1_ag08a_unique_data_t));
		return RESULT_Failure;
	}

	unique_data = get_unique_data(dev);
	atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_NONE);
	unique_data->indio_dev = NULL;

	return RESULT_Success;
}

/*!
 * @brief free unique data
 */
int ep1_ag08a_free_data(apt_usbtrx_dev_t *dev)
{
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);

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
int ep1_ag08a_init(struct usb_interface *intf, const struct usb_device_id *id)

{
	int result;

	result = ep1_ag08a_create_iiodev(intf, id);
	if (result != RESULT_Success) {
		EMSG("ep1_ag08a_create_iiodev() failed");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief terminate
 */
int ep1_ag08a_terminate(apt_usbtrx_dev_t *dev)
{
	//ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);
	return RESULT_Success;
}

/*!
 * @brief is need init reset ts
 */
bool ep1_ag08a_is_need_init_reset_ts(apt_usbtrx_dev_t *dev)
{
	/* always resettable as there is only 1ch */
	return dev->ch == 0 ? true : false;
}
