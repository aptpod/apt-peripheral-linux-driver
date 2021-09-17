/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_CORE_H__
#define __EP1_AG08A_CORE_H__

#include "../apt_usbtrx_def.h"
#include "../apt_usbtrx_msg.h"

/*!
 * @brief unique function prototype
 */
int ep1_ag08a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg);

/*!
 * @brief write bulk callback
 */
void ep1_ag08a_write_bulk_callback(struct urb *urb);

#endif /* __EP1_AG08A_CORE_H__ */
