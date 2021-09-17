/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_MSG_H__
#define __AP_CT2A_MSG_H__

#include <linux/can.h>
#include "ap_ct2a_cmd_def.h"

/*!
 * @brief pack (Set Trigger)
 */
int apt_usbtrx_msg_pack_set_trigger(apt_usbtrx_msg_set_trigger_t *param, u8 *data, int data_size);

/*!
 * @brief pack (Set Mode)
 */
int apt_usbtrx_msg_pack_set_mode(apt_usbtrx_msg_set_mode_t *param, u8 *data, int data_size);

/*!
 * @brief pack (Start/Stop CAN)
 */
int apt_usbtrx_msg_pack_start_stop_can(bool start, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Status)
 */
int apt_usbtrx_msg_parse_response_get_status(u8 *data, int data_size, apt_usbtrx_msg_resp_get_status_t *status);

/*!
 * @brief parse (Notify Receive CAN Frame)
 */
int apt_usbtrx_msg_parse_notify_recv_can_frame(u8 *data, int data_size, struct timespec *ts, struct can_frame *frame);

/*!
 * @brief parse (Notify Receive CAN Summary)
 */
int apt_usbtrx_msg_parse_notify_recv_can_summary(u8 *data, int data_size, u32 *count, struct can_frame *frame);

#endif /* __AP_CT2A_MSG_H__ */
