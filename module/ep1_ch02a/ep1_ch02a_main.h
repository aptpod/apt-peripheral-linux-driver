/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_MAIN_H__
#define __EP1_CH02A_MAIN_H__

#include "../apt_usbtrx_def.h"

bool ep1_ch02a_is_support_bittiming_command(const apt_usbtrx_dev_t *dev);
int ep1_ch02a_init(struct usb_interface *intf, const struct usb_device_id *id);
int ep1_ch02a_terminate(apt_usbtrx_dev_t *dev);

#endif /* __EP1_CH02A_MAIN_H__ */
