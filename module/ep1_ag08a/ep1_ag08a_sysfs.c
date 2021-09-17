// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_ag08a_sysfs.h"
#include "ep1_ag08a_def.h"

/*!
 * @brief sysfs initialize
 */
int ep1_ag08a_sysfs_init(struct device *dev)
{
	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief sysfs terminate
 */
int ep1_ag08a_sysfs_term(struct device *dev)
{
	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	return RESULT_Success;
}
