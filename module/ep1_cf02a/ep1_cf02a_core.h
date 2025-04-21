/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_CORE_H__
#define __EP1_CF02A_CORE_H__

#include "../apt_usbtrx_def.h"
#include "../apt_usbtrx_msg.h"
#include "ep1_cf02a_def.h"
#include "ep1_cf02a.h"

/*!
 * @brief unique function prototype
 */
void ep1_cf02a_init_stats(ep1_cf02a_stats_t *stats);
int ep1_cf02a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg);
void ep1_cf02a_write_bulk_callback(struct urb *urb);

#endif /* __EP1_CF02A_CORE_H__ */
