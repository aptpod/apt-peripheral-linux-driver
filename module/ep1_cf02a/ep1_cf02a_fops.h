/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_FOPS_H__
#define __EP1_CF02A_FOPS_H__

#include <linux/netdevice.h>
#include <linux/can/dev.h>
#include <linux/version.h>
#include <net/rtnetlink.h>
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

int ep1_cf02a_start_can_interface(apt_usbtrx_dev_t *dev);
int ep1_cf02a_stop_can_interface(apt_usbtrx_dev_t *dev);

#ifdef SUPPORT_NETDEV
int ep1_cf02a_netdev_set_bittiming(struct net_device *netdev);
int ep1_cf02a_netdev_set_data_bittiming(struct net_device *netdev);
int ep1_cf02a_netdev_open(struct net_device *netdev);
int ep1_cf02a_netdev_close(struct net_device *netdev);
netdev_tx_t ep1_cf02a_netdev_start_xmit(struct sk_buff *skb, struct net_device *netdev);
int ep1_cf02a_netdev_set_mode(struct net_device *netdev, enum can_mode mode);
void ep1_cf02a_statistics_work_func(struct work_struct *work);
int ep1_cf02a_netdev_get_state(const struct net_device *netdev, enum can_state *state);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
void
#else
struct rtnl_link_stats64 *
#endif
ep1_cf02a_netdev_get_stats64(struct net_device *netdev, struct rtnl_link_stats64 *storage);
#endif

#endif /* __EP1_CF02A_FOPS_H__ */
