// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ap_ct2a_sysfs.h"
#include "ap_ct2a_def.h"

/*!
 * @brief can data (std)
 */
static ssize_t apt_usbtrx_sysfs_datcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.dat_std.total_num);
}

static DEVICE_ATTR(datcnt, S_IRUGO, apt_usbtrx_sysfs_datcnt_show, NULL);

/*!
 * @brief can data (ext)
 */
static ssize_t apt_usbtrx_sysfs_ext_datcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.dat_ext.total_num);
}

static DEVICE_ATTR(ext_datcnt, S_IRUGO, apt_usbtrx_sysfs_ext_datcnt_show, NULL);

/*!
 * @brief rtr data (std)
 */
static ssize_t apt_usbtrx_sysfs_rtrcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.rtr_std.total_num);
}

static DEVICE_ATTR(rtrcnt, S_IRUGO, apt_usbtrx_sysfs_rtrcnt_show, NULL);

/*!
 * @brief rtr data (ext)
 */
static ssize_t apt_usbtrx_sysfs_ext_rtrcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.rtr_ext.total_num);
}

static DEVICE_ATTR(ext_rtrcnt, S_IRUGO, apt_usbtrx_sysfs_ext_rtrcnt_show, NULL);

/*!
 * @brief err data
 */
static ssize_t apt_usbtrx_sysfs_errcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.err.total_num);
}

static DEVICE_ATTR(errcnt, S_IRUGO, apt_usbtrx_sysfs_errcnt_show, NULL);

/*!
 * @brief cnt timestamp
 */
static ssize_t apt_usbtrx_sysfs_cnt_timestamp_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(usbtrx_dev);

	struct timespec64 ts;
	ktime_t kt = ktime_set(0, 0);

	if (ktime_compare(kt, unique_data->summary.dat_std.kt) < 0) {
		kt = unique_data->summary.dat_std.kt;
	}
	if (ktime_compare(kt, unique_data->summary.dat_ext.kt) < 0) {
		kt = unique_data->summary.dat_ext.kt;
	}
	if (ktime_compare(kt, unique_data->summary.rtr_std.kt) < 0) {
		kt = unique_data->summary.rtr_std.kt;
	}
	if (ktime_compare(kt, unique_data->summary.rtr_ext.kt) < 0) {
		kt = unique_data->summary.rtr_ext.kt;
	}
	if (ktime_compare(kt, unique_data->summary.err.kt) < 0) {
		kt = unique_data->summary.err.kt;
	}

	ts = ktime_to_timespec64(kt);
	return sprintf(buf, "%lld\n", (s64)ts.tv_sec);
}

static DEVICE_ATTR(cnt_timestamp, S_IRUGO, apt_usbtrx_sysfs_cnt_timestamp_show, NULL);

/*!
 * @brief sysfs initialize
 */
int apt_usbtrx_unique_can_sysfs_init(struct device *dev)
{
	int result;

	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	result = device_create_file(dev, &dev_attr_datcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "datcnt");
	}
	result = device_create_file(dev, &dev_attr_ext_datcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "ext_datcnt");
	}
	result = device_create_file(dev, &dev_attr_rtrcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "rtrcnt");
	}
	result = device_create_file(dev, &dev_attr_ext_rtrcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "ext_rtrcnt");
	}
	result = device_create_file(dev, &dev_attr_errcnt);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "errcnt");
	}
	result = device_create_file(dev, &dev_attr_cnt_timestamp);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "cnt_timestamp");
	}

	return RESULT_Success;
}

/*!
 * @brief sysfs terminate
 */
int apt_usbtrx_unique_can_sysfs_term(struct device *dev)
{
	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	device_remove_file(dev, &dev_attr_datcnt);
	device_remove_file(dev, &dev_attr_ext_datcnt);
	device_remove_file(dev, &dev_attr_rtrcnt);
	device_remove_file(dev, &dev_attr_ext_rtrcnt);
	device_remove_file(dev, &dev_attr_errcnt);
	device_remove_file(dev, &dev_attr_cnt_timestamp);

	return RESULT_Success;
}
