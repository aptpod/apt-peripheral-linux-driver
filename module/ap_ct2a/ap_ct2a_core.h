/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_CORE_H__
#define __AP_CT2A_CORE_H__

#include "../apt_usbtrx_def.h"
#include "../apt_usbtrx_msg.h"
#include "ap_ct2a_def.h"

/*!
 * @brief unique function prototype
 */
int apt_usbtrx_unique_can_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg);

/*!
 * @brief init stats
 */
void apt_usbtrx_init_stats(apt_usbtrx_stats_t *stats);

/*!
 * @brief write bulk callback
 */
void apt_usbtrx_unique_can_write_bulk_callback(struct urb *urb);

#endif /* __AP_CT2A_CORE_H__ */
