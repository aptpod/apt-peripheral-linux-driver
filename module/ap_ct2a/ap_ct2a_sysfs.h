/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_SYSFS_H__
#define __AP_CT2A_SYSFS_H__

#include <linux/device.h>

/*!
 * @brief unique function prototype
 */
int apt_usbtrx_unique_can_sysfs_init(struct device *dev);
int apt_usbtrx_unique_can_sysfs_term(struct device *dev);

#endif /* __AP_CT2A_SYSFS_H__ */
