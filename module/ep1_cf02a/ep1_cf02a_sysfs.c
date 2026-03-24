// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_cf02a_sysfs.h"
#include "ep1_cf02a_def.h"
#include "ep1_cf02a_cmd.h"

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
 * @brief fw rx frame errors
 */
static ssize_t ep1_cf02a_sysfs_fw_rx_dropped_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_msg_get_can_statistics_t statistics;
	int result;

	if (apt_usbtrx_is_dfu(usbtrx_dev->interface)) {
		return sprintf(buf, "0\n");
	}

	result = ep1_cf02a_get_can_statistics(usbtrx_dev, &statistics);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_can_statistics().. Error");
		return sprintf(buf, "0\n");
	}

	return sprintf(buf, "%u\n", statistics.rx_dropped);
}

static DEVICE_ATTR(fw_rx_dropped, S_IRUGO, ep1_cf02a_sysfs_fw_rx_dropped_show, NULL);

/*!
 * @brief fw can state
 */
static ssize_t ep1_cf02a_sysfs_can_state_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	ep1_cf02a_msg_get_can_statistics_t statistics;
	int result;
	const char *state_str;

	if (apt_usbtrx_is_dfu(usbtrx_dev->interface)) {
		return sprintf(buf, "unknown\n");
	}

	result = ep1_cf02a_get_can_statistics(usbtrx_dev, &statistics);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_can_statistics().. Error");
		return sprintf(buf, "unknown\n");
	}

	switch (statistics.can_state) {
	case 0:
		state_str = "ERROR-ACTIVE";
		break;
	case 1:
		state_str = "ERROR-WARNING";
		break;
	case 2:
		state_str = "ERROR-PASSIVE";
		break;
	case 3:
		state_str = "BUS-OFF";
		break;
	default:
		state_str = "unknown";
		break;
	}

	return sprintf(buf, "%s\n", state_str);
}

static DEVICE_ATTR(can_state, S_IRUGO, ep1_cf02a_sysfs_can_state_show, NULL);

/*!
 * @brief reset fw statistics
 */
static ssize_t ep1_cf02a_sysfs_reset_fw_statistics_store(struct device *dev, struct device_attribute *attr,
							 const char *buf, size_t count)
{
	apt_usbtrx_dev_t *usbtrx_dev = dev_get_drvdata(dev);
	bool success;
	int result;

	if (apt_usbtrx_is_dfu(usbtrx_dev->interface)) {
		return count;
	}

	result = ep1_cf02a_reset_can_statistics(usbtrx_dev, &success);
	if (result != RESULT_Success || !success) {
		EMSG("ep1_cf02a_reset_can_statistics().. Error");
		return -EIO;
	}

	return count;
}

/* NOTE: writable attrs must be listed in conf/30-apt-usb.rules */
static DEVICE_ATTR(reset_fw_statistics, S_IWUSR, NULL, ep1_cf02a_sysfs_reset_fw_statistics_store);

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
	result = device_create_file(dev, &dev_attr_fw_rx_dropped);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "fw_rx_dropped");
	}
	result = device_create_file(dev, &dev_attr_can_state);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "can_state");
	}
	result = device_create_file(dev, &dev_attr_reset_fw_statistics);
	if (result != 0) {
		EMSG("device_create_file().. Error, <name:%s>", "reset_fw_statistics");
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
	device_remove_file(dev, &dev_attr_fw_rx_dropped);
	device_remove_file(dev, &dev_attr_can_state);
	device_remove_file(dev, &dev_attr_reset_fw_statistics);
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
