// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT USB peripherals.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/device.h>

#include "apt_usbtrx_def.h"

/*!
 * @brief skip data
 */
static ssize_t apt_usbtrx_sysfs_skipcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = NULL;

	usbtrx_dev = dev_get_drvdata(dev);
	return sprintf(buf, "%llu\n", usbtrx_dev->rx_data.skip_count);
}
static DEVICE_ATTR(skipcnt, S_IRUGO, apt_usbtrx_sysfs_skipcnt_show, NULL);

/*!
 * @brief timestamp mode
 */
static ssize_t apt_usbtrx_sysfs_timestamp_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = NULL;

	usbtrx_dev = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", usbtrx_dev->timestamp_mode);
}
static DEVICE_ATTR(timestamp_mode, S_IRUGO, apt_usbtrx_sysfs_timestamp_mode_show, NULL);

/*!
 * @brief basetime_clock_id
 */
static ssize_t apt_usbtrx_sysfs_basetime_clock_id_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = NULL;
	const char *basetime_clock_id_str = NULL;

	usbtrx_dev = dev_get_drvdata(dev);

	switch (usbtrx_dev->basetime_clock_id) {
	case CLOCK_MONOTONIC:
		basetime_clock_id_str = "CLOCK_MONOTONIC";
		break;
	case CLOCK_MONOTONIC_RAW:
		basetime_clock_id_str = "CLOCK_MONOTONIC_RAW";
		break;
	default:
		EMSG("Invalid clock id is set.");
		return -EINVAL;
	}

	return sprintf(buf, "%s\n", basetime_clock_id_str);
}
static ssize_t apt_usbtrx_sysfs_basetime_clock_id_store(struct device *dev, struct device_attribute *attr,
							const char *buf, size_t count)
{
	apt_usbtrx_dev_t *usbtrx_dev = NULL;

	usbtrx_dev = dev_get_drvdata(dev);

	if (sysfs_streq(buf, "CLOCK_MONOTONIC") || sysfs_streq(buf, "clock_monotonic") ||
	    sysfs_streq(buf, "CLOCK-MONOTONIC") || sysfs_streq(buf, "clock-monotonic")) {
		usbtrx_dev->basetime_clock_id = CLOCK_MONOTONIC;
	} else if (sysfs_streq(buf, "CLOCK_MONOTONIC_RAW") || sysfs_streq(buf, "clock_monotonic_raw") ||
		   sysfs_streq(buf, "CLOCK-MONOTONIC-RAW") || sysfs_streq(buf, "clock-monotonic-raw")) {
		usbtrx_dev->basetime_clock_id = CLOCK_MONOTONIC_RAW;
	} else {
		EMSG("Only \"CLOCK_MONOTONIC\" or \"COCK_MONOTONIC_RAW\" available");
		return -EINVAL;
	}

	return count;
}
static DEVICE_ATTR(basetime_clock_id, S_IWUSR | S_IRUGO, apt_usbtrx_sysfs_basetime_clock_id_show,
		   apt_usbtrx_sysfs_basetime_clock_id_store);

/*!
 * @brief sysfs initialize
 */
int apt_usbtrx_sysfs_init(struct device *dev)
{
	int result;
	apt_usbtrx_dev_t *usbtrx_dev = NULL;

	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	result = device_create_file(dev, &dev_attr_skipcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "skipcnt");
	}

	result = device_create_file(dev, &dev_attr_timestamp_mode);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "timestamp_mode");
	}

	result = device_create_file(dev, &dev_attr_basetime_clock_id);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "basetime_clock_id");
	}

	usbtrx_dev = dev_get_drvdata(dev);
	if (usbtrx_dev == NULL) {
		EMSG("dev_get_drvdata().. Error");
		return RESULT_Failure;
	}

	result = usbtrx_dev->unique_func.sysfs_init(dev);
	if (result != RESULT_Success) {
		EMSG("sysfs_init().. Error");
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief sysfs terminate
 */
int apt_usbtrx_sysfs_term(struct device *dev)
{
	int result;
	apt_usbtrx_dev_t *usbtrx_dev = NULL;

	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	device_remove_file(dev, &dev_attr_skipcnt);
	device_remove_file(dev, &dev_attr_timestamp_mode);
	device_remove_file(dev, &dev_attr_basetime_clock_id);

	usbtrx_dev = dev_get_drvdata(dev);
	if (usbtrx_dev == NULL) {
		EMSG("dev_get_drvdata().. Error");
		return RESULT_Failure;
	}

	result = usbtrx_dev->unique_func.sysfs_term(dev);
	if (result != RESULT_Success) {
		EMSG("sysfs_term().. Error");
		return RESULT_Failure;
	}

	return RESULT_Success;
}
