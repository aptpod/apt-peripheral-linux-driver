// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_cf02a_sysfs.h"
#include "ep1_cf02a_def.h"

/*!
 * @brief store data enabled
 */
static ssize_t ep1_cf02a_sysfs_store_data_enabled_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->is_store_data_enabled ? 1 : 0);
}
static DEVICE_ATTR(store_data_enabled, S_IRUGO, ep1_cf02a_sysfs_store_data_enabled_show, NULL);

/*!
 * @brief can data (std)
 */
static ssize_t ep1_cf02a_sysfs_datcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.dat_std.total_num);
}

static DEVICE_ATTR(datcnt, S_IRUGO, ep1_cf02a_sysfs_datcnt_show, NULL);

/*!
 * @brief can data (ext)
 */
static ssize_t ep1_cf02a_sysfs_ext_datcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.dat_ext.total_num);
}

static DEVICE_ATTR(ext_datcnt, S_IRUGO, ep1_cf02a_sysfs_ext_datcnt_show, NULL);

/*!
 * @brief rtr data (std)
 */
static ssize_t ep1_cf02a_sysfs_rtrcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.rtr_std.total_num);
}

static DEVICE_ATTR(rtrcnt, S_IRUGO, ep1_cf02a_sysfs_rtrcnt_show, NULL);

/*!
 * @brief rtr data (ext)
 */
static ssize_t ep1_cf02a_sysfs_ext_rtrcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.rtr_ext.total_num);
}

static DEVICE_ATTR(ext_rtrcnt, S_IRUGO, ep1_cf02a_sysfs_ext_rtrcnt_show, NULL);

/*!
 * @brief err data
 */
static ssize_t ep1_cf02a_sysfs_errcnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%u\n", unique_data->summary.err.total_num);
}

static DEVICE_ATTR(errcnt, S_IRUGO, ep1_cf02a_sysfs_errcnt_show, NULL);

/*!
 * @brief cnt timestamp
 */
static ssize_t ep1_cf02a_sysfs_cnt_timestamp_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

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

static DEVICE_ATTR(cnt_timestamp, S_IRUGO, ep1_cf02a_sysfs_cnt_timestamp_show, NULL);

/*!
 * @brief can clock
 */
static ssize_t ep1_cf02a_sysfs_can_clock_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->can_clock);
}
static DEVICE_ATTR(can_clock, S_IRUGO, ep1_cf02a_sysfs_can_clock_show, NULL);

/*!
 * @brief bit timing
 */
static ssize_t ep1_cf02a_sysfs_bt_prop_seg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->bittiming->prop_seg);
}
static DEVICE_ATTR(bt_prop_seg, S_IRUGO, ep1_cf02a_sysfs_bt_prop_seg_show, NULL);

static ssize_t ep1_cf02a_sysfs_bt_phase_seg1_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->bittiming->phase_seg1);
}
static DEVICE_ATTR(bt_phase_seg1, S_IRUGO, ep1_cf02a_sysfs_bt_phase_seg1_show, NULL);

static ssize_t ep1_cf02a_sysfs_bt_phase_seg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->bittiming->phase_seg2);
}
static DEVICE_ATTR(bt_phase_seg2, S_IRUGO, ep1_cf02a_sysfs_bt_phase_seg2_show, NULL);

static ssize_t ep1_cf02a_sysfs_bt_sjw_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->bittiming->sjw);
}
static DEVICE_ATTR(bt_sjw, S_IRUGO, ep1_cf02a_sysfs_bt_sjw_show, NULL);

static ssize_t ep1_cf02a_sysfs_bt_brp_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->bittiming->brp);
}
static DEVICE_ATTR(bt_brp, S_IRUGO, ep1_cf02a_sysfs_bt_brp_show, NULL);

/*!
 * @brief data bit timing
 */
static ssize_t ep1_cf02a_sysfs_dbt_prop_seg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->data_bittiming->prop_seg);
}
static DEVICE_ATTR(dbt_prop_seg, S_IRUGO, ep1_cf02a_sysfs_dbt_prop_seg_show, NULL);

static ssize_t ep1_cf02a_sysfs_dbt_phase_seg1_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->data_bittiming->phase_seg1);
}
static DEVICE_ATTR(dbt_phase_seg1, S_IRUGO, ep1_cf02a_sysfs_dbt_phase_seg1_show, NULL);

static ssize_t ep1_cf02a_sysfs_dbt_phase_seg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->data_bittiming->phase_seg2);
}
static DEVICE_ATTR(dbt_phase_seg2, S_IRUGO, ep1_cf02a_sysfs_dbt_phase_seg2_show, NULL);

static ssize_t ep1_cf02a_sysfs_dbt_sjw_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->data_bittiming->sjw);
}
static DEVICE_ATTR(dbt_sjw, S_IRUGO, ep1_cf02a_sysfs_dbt_sjw_show, NULL);

static ssize_t ep1_cf02a_sysfs_dbt_brp_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(usbtrx_dev);

	return sprintf(buf, "%d\n", unique_data->data_bittiming->brp);
}
static DEVICE_ATTR(dbt_brp, S_IRUGO, ep1_cf02a_sysfs_dbt_brp_show, NULL);

/*!
 * @brief sysfs initialize
 */
int ep1_cf02a_sysfs_init(struct device *dev)
{
	int result;

	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}

	result = device_create_file(dev, &dev_attr_store_data_enabled);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "store_data_enabled");
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

	result = device_create_file(dev, &dev_attr_can_clock);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "can_clock");
	}

	result = device_create_file(dev, &dev_attr_bt_prop_seg);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "bt_prop_seg");
	}
	result = device_create_file(dev, &dev_attr_bt_phase_seg1);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "bt_phase_seg1");
	}
	result = device_create_file(dev, &dev_attr_bt_phase_seg2);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "bt_phase_seg2");
	}
	result = device_create_file(dev, &dev_attr_bt_sjw);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "bt_sjw");
	}
	result = device_create_file(dev, &dev_attr_bt_brp);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "bt_brp");
	}

	result = device_create_file(dev, &dev_attr_dbt_prop_seg);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "dbt_prop_seg");
	}
	result = device_create_file(dev, &dev_attr_dbt_phase_seg1);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "dbt_phase_seg1");
	}
	result = device_create_file(dev, &dev_attr_dbt_phase_seg2);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "dbt_phase_seg2");
	}
	result = device_create_file(dev, &dev_attr_dbt_sjw);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "dbt_sjw");
	}
	result = device_create_file(dev, &dev_attr_dbt_brp);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "dbt_brp");
	}

	return RESULT_Success;
}

/*!
 * @brief sysfs terminate
 */
int ep1_cf02a_sysfs_term(struct device *dev)
{
	if (dev == NULL) {
		EMSG("dev is NULL");
		return RESULT_Failure;
	}
	device_remove_file(dev, &dev_attr_store_data_enabled);
	device_remove_file(dev, &dev_attr_datcnt);
	device_remove_file(dev, &dev_attr_ext_datcnt);
	device_remove_file(dev, &dev_attr_rtrcnt);
	device_remove_file(dev, &dev_attr_ext_rtrcnt);
	device_remove_file(dev, &dev_attr_errcnt);
	device_remove_file(dev, &dev_attr_cnt_timestamp);

	device_remove_file(dev, &dev_attr_can_clock);

	device_remove_file(dev, &dev_attr_bt_prop_seg);
	device_remove_file(dev, &dev_attr_bt_phase_seg1);
	device_remove_file(dev, &dev_attr_bt_phase_seg2);
	device_remove_file(dev, &dev_attr_bt_sjw);
	device_remove_file(dev, &dev_attr_bt_brp);

	device_remove_file(dev, &dev_attr_dbt_prop_seg);
	device_remove_file(dev, &dev_attr_dbt_phase_seg1);
	device_remove_file(dev, &dev_attr_dbt_phase_seg2);
	device_remove_file(dev, &dev_attr_dbt_sjw);
	device_remove_file(dev, &dev_attr_dbt_brp);

	return RESULT_Success;
}
