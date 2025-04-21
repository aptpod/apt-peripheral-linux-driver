/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_SYSFS_H__
#define __EP1_CF02A_SYSFS_H__

#include <linux/device.h>

/*!
 * @brief unique function prototype
 */
int ep1_cf02a_sysfs_init(struct device *dev);
int ep1_cf02a_sysfs_term(struct device *dev);

#endif /* __EP1_CF02A_SYSFS_H__ */
