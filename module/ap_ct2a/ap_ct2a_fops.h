/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_FOPS_H__
#define __AP_CT2A_FOPS_H__

#include <linux/netdevice.h>
#include <linux/can/dev.h>
#include "../apt_usbtrx_def.h"

/*!
 * @brief unique function prototype
 */
int apt_usbtrx_unique_can_get_read_payload_size(const void *payload);
int apt_usbtrx_unique_can_get_write_payload_size(const void *payload);
apt_usbtrx_timestamp_t *apt_usbtrx_unique_can_get_read_payload_timestamp(const void *payload);
int apt_usbtrx_unique_can_get_write_cmd_id(void);
int apt_usbtrx_unique_can_get_fw_size(void);
long apt_usbtrx_unique_can_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int apt_usbtrx_unique_can_is_device_input_start(apt_usbtrx_dev_t *dev, bool *start);
int apt_usbtrx_unique_can_open(apt_usbtrx_dev_t *dev);
int apt_usbtrx_unique_can_close(apt_usbtrx_dev_t *dev);
int apt_usbtrx_unique_can_set_mode(apt_usbtrx_dev_t *dev, int baudrate, bool silent);

/* socketcan */
int apt_usbtrx_unique_can_netdev_set_mode(struct net_device *netdev, enum can_mode mode);
int apt_usbtrx_unique_can_netdev_set_bittiming(struct net_device *netdev);
int apt_usbtrx_unique_can_netdev_close(struct net_device *netdev);
int apt_usbtrx_unique_can_netdev_open(struct net_device *netdev);
netdev_tx_t apt_usbtrx_unique_can_netdev_start_xmit(struct sk_buff *skb, struct net_device *netdev);

#endif /* __AP_CT2A_FOPS_H__ */
