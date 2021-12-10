/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_AG08A_IIO_H__
#define __EP1_AG08A_IIO_H__

#include <linux/usb.h>

/*!
 * @brief create iiodev
 */
int ep1_ag08a_create_iiodev(struct usb_interface *intf, const struct usb_device_id *id);

#endif /* __EP1_AG08A_IIO_H__ */
