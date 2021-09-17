/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */
#ifndef __EP1_CH02A_CMD_DEF_H__
#define __EP1_CH02A_CMD_DEF_H__

#include "../ap_ct2a/ap_ct2a_cmd_def.h" /* inherit from ap_ct2a */

/*!
 * @brief command
 */
enum EP1_CH02A_CMD {
	/*** Request ***/
	EP1_CH02A_CMD_GetStatus = APT_USBTRX_CMD_GetStatus, // 0x05
	EP1_CH02A_CMD_SetBitTiming = 0x0B,
	EP1_CH02A_CMD_GetBitTiming = 0x0E,
	/*** Response ***/
	EP1_CH02A_CMD_ResponseGetStatus = APT_USBTRX_CMD_ResponseGetStatus, // 0x27
	EP1_CH02A_CMD_ResponseGetBitTiming = 0x2E,
};

/*!
 * @brief command length (not payload size)
 */
/*** Request ***/
#define EP1_CH02A_CMD_LENGTH_GET_STATUS APT_USBTRX_CMD_LENGTH_GET_STATUS
#define EP1_CH02A_CMD_LENGTH_SET_BIT_TIMING (10)
#define EP1_CH02A_CMD_LENGTH_GET_BIT_TIMING (4)
/*** Response ***/
#define EP1_CH02A_CMD_LENGTH_RESPONSE_GET_STATUS (5)
#define EP1_CH02A_CMD_LENGTH_RESPONSE_GET_BIT_TIMING (14)

/*!
 * @brief message structure (get status)
 */
struct ep1_ch02a_msg_resp_get_status_s {
	apt_usbtrx_msg_set_mode_t mode;
	bool enable_sync;
};
typedef struct ep1_ch02a_msg_resp_get_status_s ep1_ch02a_msg_resp_get_status_t;

/*!
 * @brief message structure (set bit-timing)
 */
struct ep1_ch02a_msg_set_bit_timing_s {
	int prop_seg;
	int phase_seg1;
	int phase_seg2;
	int sjw;
	int brp;
};
typedef struct ep1_ch02a_msg_set_bit_timing_s ep1_ch02a_msg_set_bit_timing_t;

/*!
 * @brief message structure (get bit-timing)
 */
struct ep1_ch02a_msg_resp_get_bit_timing_s {
	int can_clock;
	ep1_ch02a_msg_set_bit_timing_t params;
};
typedef struct ep1_ch02a_msg_resp_get_bit_timing_s ep1_ch02a_msg_resp_get_bit_timing_t;

#endif /* __EP1_CH2A_CMD_DEF_H__ */
