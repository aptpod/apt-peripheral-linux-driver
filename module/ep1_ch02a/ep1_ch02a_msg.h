/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_MSG_H__
#define __EP1_CH02A_MSG_H__

#include <linux/can.h>
#include "ep1_ch02a_cmd_def.h"

/*!
 * @brief parse (Response Get Status)
 */
int ep1_ch02a_msg_parse_response_get_status(u8 *data, int data_size, ep1_ch02a_msg_resp_get_status_t *status);

/*!
 * @brief pack (Set BitTiming)
 */
int ep1_ch02a_msg_pack_set_bit_timing(ep1_ch02a_msg_set_bit_timing_t *param, u8 *data, int data_size);

/*!
 * @brief parse (Response Get BitTiming)
 */
int ep1_ch02a_msg_parse_response_get_bit_timing(u8 *data, int data_size,
						ep1_ch02a_msg_resp_get_bit_timing_t *bit_timing);

#endif /* __EP1_CH02A_MSG_H__ */
