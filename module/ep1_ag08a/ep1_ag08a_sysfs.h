/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_SYSFS_H__
#define __EP1_AG08A_SYSFS_H__

#include <linux/device.h>
#include "ep1_ag08a_def.h"

/*!
 * @brief unique function prototype
 */
int ep1_ag08a_sysfs_init(struct device *dev);
int ep1_ag08a_sysfs_term(struct device *dev);

#endif /* __EP1_AG08A_SYSFS_H__ */
