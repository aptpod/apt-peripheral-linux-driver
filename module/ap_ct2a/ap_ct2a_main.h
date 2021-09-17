/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_MAIN_H__
#define __AP_CT2A_MAIN_H__

#include "../apt_usbtrx_def.h"

/*!
 * @brief unique function prototype
 */
int apt_usbtrx_unique_can_init_data(apt_usbtrx_dev_t *dev);
int apt_usbtrx_unique_can_free_data(apt_usbtrx_dev_t *dev);
int apt_usbtrx_unique_can_init(struct usb_interface *intf, const struct usb_device_id *id);
int apt_usbtrx_unique_can_terminate(apt_usbtrx_dev_t *dev);
bool apt_usbtrx_unique_can_is_need_init_reset_ts(apt_usbtrx_dev_t *dev);

#endif /* __AP_CT2A_MAIN_H__ */
