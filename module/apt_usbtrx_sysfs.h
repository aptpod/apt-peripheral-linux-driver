/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux.
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_SYSFS_H__
#define __APT_USBTRX_SYSFS_H__

/*!
 * @brief sysfs initialize
 */
int apt_usbtrx_sysfs_init(struct device *dev);

/*!
 * @brief sysfs terminate
 */
int apt_usbtrx_sysfs_term(struct device *dev);

#endif /* __APT_USBTRX_SYSFS_H__ */
