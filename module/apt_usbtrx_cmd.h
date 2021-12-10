/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_CMD_H__
#define __APT_USBTRX_CMD_H__

#include "apt_usbtrx_def.h"

/*!
 * @brief get device id
 */
int apt_usbtrx_get_device_id(apt_usbtrx_dev_t *dev, char *device_id, int device_id_size, int *channel);

/*!
 * @brief get fw version
 */
int apt_usbtrx_get_fw_version(apt_usbtrx_dev_t *dev, int *major_version, int *minor_version);

/*!
 * @brief get fw version revision
 */
int apt_usbtrx_get_fw_version_revision(apt_usbtrx_dev_t *dev, int *major_version, int *minor_version, int *revision);

/*!
 * @brief enable reset timestamp
 */
int apt_usbtrx_enable_reset_ts(apt_usbtrx_dev_t *dev, bool *success);

/*!
 * @brief reset timestamp
 */
int apt_usbtrx_reset_ts(apt_usbtrx_dev_t *dev, bool *success);

/*!
 * @brief reset device
 */
int apt_usbtrx_reset_device(apt_usbtrx_dev_t *dev, bool *success);

/*!
 * @brief get serial no
 */
int apt_usbtrx_get_serial_no(apt_usbtrx_dev_t *dev, char *serial_no, int serial_no_size, int *channel, int *sync_pulse);

/*!
 * @brief move DFU mode
 */
int apt_usbtrx_move_dfu(apt_usbtrx_dev_t *dev, bool *success);

#endif /* __APT_USBTRX_CMD_H__ */
