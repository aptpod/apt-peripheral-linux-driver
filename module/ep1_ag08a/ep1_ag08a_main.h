/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_MAIN_H__
#define __EP1_AG08A_MAIN_H__

#include "../apt_usbtrx_def.h"

/*!
 * @brief unique function prototype
 */
int ep1_ag08a_init_data(apt_usbtrx_dev_t *dev);
int ep1_ag08a_free_data(apt_usbtrx_dev_t *dev);
int ep1_ag08a_init(struct usb_interface *intf, const struct usb_device_id *id);
int ep1_ag08a_terminate(apt_usbtrx_dev_t *dev);
bool ep1_ag08a_is_need_init_reset_ts(apt_usbtrx_dev_t *dev);

#endif /* __EP1_AG08A_MAIN_H__ */
