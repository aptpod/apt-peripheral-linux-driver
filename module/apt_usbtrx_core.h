/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_CORE_H__
#define __APT_USBTRX_CORE_H__

#include "apt_usbtrx_def.h"

/*!
 * @brief setup rx urbs
 */
int apt_usbtrx_setup_rx_urbs(apt_usbtrx_dev_t *dev);

/*!
 * @brief setup tx urb
 */
int apt_usbtrx_setup_tx_urb(apt_usbtrx_dev_t *dev, u8 *data, int data_size);

/*!
 * @brief send message
 */
int apt_usbtrx_send_msg(apt_usbtrx_dev_t *dev, u8 *data, int data_size);

/*!
 * @brief wait for message
 */
int apt_usbtrx_wait_msg(apt_usbtrx_dev_t *dev, u8 ack_id, u8 nack_id, u8 *data, int data_size);

/*!
 * @brief tx thread func
 */
int apt_usbtrx_tx_thread_func(void *arg);

#ifdef UNIT_TEST
int apt_usbtrx_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg);
#endif

#endif /* __APT_USBTRX_CORE_H__ */
