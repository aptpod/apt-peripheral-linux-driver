/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_FOPS_H__
#define __APT_USBTRX_FOPS_H__

#include <linux/fs.h>
#include "apt_usbtrx_def.h"

/*!
 * @brief open
 */
int apt_usbtrx_open(struct inode *inode, struct file *file);

/*!
 * @brief release
 */
int apt_usbtrx_release(struct inode *inode, struct file *file);

/*!
 * @brief read
 */
ssize_t apt_usbtrx_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);

/*!
 * @brief write
 */
ssize_t apt_usbtrx_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);

/*!
 * @brief write tx ringbuffer
 */
ssize_t apt_usbtrx_write_tx_rb(apt_usbtrx_dev_t *dev, const void *payload, const u8 payload_size);

/*!
 * @brief ioctl
 */
long apt_usbtrx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*!
 * @brief write fw data
 */
ssize_t apt_usbtrx_write_fw_data(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);

/*!
 * @brief parse rbmsg
 */
int apt_usbtrx_parse_rbmsg(apt_usbtrx_dev_t *dev, u8 *payload, size_t size, u64 relative_time_ns);

#endif /* __APT_USBTRX_FOPS_H__ */
