/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_AG08A_IIO_H__
#define __EP1_AG08A_IIO_H__

#include <linux/version.h>
#include <linux/usb.h>

/*!
 * @brief create iiodev
 */
int ep1_ag08a_create_iiodev(struct usb_interface *intf, const struct usb_device_id *id);

/*!
 * @brief  iio_get_time_ns aliases
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
#define wrap_iio_get_time_ns(indio_dev)  iio_get_time_ns(indio_dev)
#else
#define wrap_iio_get_time_ns(indio_dev)  iio_get_time_ns()
#endif

#endif /* __EP1_AG08A_IIO_H__ */
