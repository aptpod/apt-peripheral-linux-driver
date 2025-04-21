/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_FOPS_H__
#define __EP1_CF02A_FOPS_H__

#include "../apt_usbtrx_def.h"

/*!
 * @brief unique function prototype
 */
int ep1_cf02a_get_read_payload_size(const void *payload);
int ep1_cf02a_get_write_payload_size(const void *payload);
apt_usbtrx_timestamp_t *ep1_cf02a_get_read_payload_timestamp(const void *payload);
int ep1_cf02a_get_write_cmd_id(void);
int ep1_cf02a_get_fw_size(void);
long ep1_cf02a_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int ep1_cf02a_is_device_start(apt_usbtrx_dev_t *dev, bool *start);
int ep1_cf02a_open(apt_usbtrx_dev_t *dev);
int ep1_cf02a_close(apt_usbtrx_dev_t *dev);

#endif /* __EP1_CF02A_FOPS_H__ */
