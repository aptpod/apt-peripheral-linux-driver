/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_CMD_H__
#define __EP1_CH02A_CMD_H__

#include "ep1_ch02a_cmd_def.h"

/*!
 * @brief get status
 */
int ep1_ch02a_get_status(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_resp_get_status_t *status);

/*!
 * @brief set bittiming
 */
int ep1_ch02a_set_bit_timing(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_set_bit_timing_t *param, bool *success);

/*!
 * @brief get bittiming
 */
int ep1_ch02a_get_bit_timing(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_resp_get_bit_timing_t *param);

#endif /* __EP1_CH02A_CMD_H__ */
