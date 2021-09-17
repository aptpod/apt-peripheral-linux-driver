/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_CORE_H__
#define __EP1_CH02A_CORE_H__

#include "../apt_usbtrx_def.h"
#include "../apt_usbtrx_msg.h"

/*!
 * @brief unique function prototype
 */
int ep1_ch02a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg);

#endif /* __EP1_CH02A_CORE_H__ */
