/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_MAIN_H__
#define __EP1_CF02A_MAIN_H__

#include "../apt_usbtrx_def.h"

/*!
 * @brief unique function prototype
 */
int ep1_cf02a_init_data(apt_usbtrx_dev_t *dev);
int ep1_cf02a_free_data(apt_usbtrx_dev_t *dev);
int ep1_cf02a_init(struct usb_interface *intf, const struct usb_device_id *id);
int ep1_cf02a_terminate(apt_usbtrx_dev_t *dev);
bool ep1_cf02a_is_need_init_reset_ts(apt_usbtrx_dev_t *dev);

#endif /* __EP1_CF02A_MAIN_H__ */
