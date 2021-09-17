/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_FOPS_H__
#define __EP1_CH02A_FOPS_H__

/*!
 * @brief unique function prototype
 */
long ep1_ch02a_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/* socketcan */
int ep1_ch02a_netdev_set_bittiming(struct net_device *netdev);

#endif /* __EP1_CH02A_FOPS_H__ */
