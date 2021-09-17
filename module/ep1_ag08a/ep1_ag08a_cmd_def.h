/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_CMD_DEF_H__
#define __EP1_AG08A_CMD_DEF_H__

#include <linux/types.h>
#include "../apt_usbtrx_def.h" /* apt_usbtrx_timestamp_t */
#include "ep1_ag08a_def.h" /* EP1_AG08A_CH_NUM */

/*!
 * @brief command
 */
enum EP1_AG08A_CMD {
	/*** Request ***/
	EP1_AG08A_CMD_GetStatus = 0x15,
	EP1_AG08A_CMD_SetAnalogInput = 0x1F,
	EP1_AG08A_CMD_ControlAnalogInput = 0x16,
	EP1_AG08A_CMD_SetAnalogOutput = 0x1A,
	EP1_AG08A_CMD_ControlAnalogOutput = 0x17,
	/*** Notify ***/
	EP1_AG08A_CMD_NotifyAnalogInput = 0x3F,
	/*** Response ***/
	EP1_AG08A_CMD_ResponseGetStatus = 0x35,
};

/*!
 * @brief command length (not payload size)
 */
/*** Request ***/
#define EP1_AG08A_CMD_LENGTH_GET_STATUS (4)
#define EP1_AG08A_CMD_LENGTH_SET_ANALOG_INPUT (13)
#define EP1_AG08A_CMD_LENGTH_CONTROL_ANALOG_INPUT (5)
#define EP1_AG08A_CMD_LENGTH_SET_ANALOG_OUTPUT (7)
#define EP1_AG08A_CMD_LENGTH_CONTROL_ANALOG_OUTPUT (5)
/*** Notify ***/
#define EP1_AG08A_CMD_LENGTH_NOTIFY_ANALOG_INPUT (29)
/*** Response ***/
#define EP1_AG08A_CMD_LENGTH_RESPONSE_GET_STATUS (18)

/*!
 * @brief send rate
 */
enum EP1_AG08A_SEND_RATE_HZ {
	EP1_AG08A_SEND_RATE_HZ_Unknown = -1,
	EP1_AG08A_SEND_RATE_HZ_10K = 0x01,
	EP1_AG08A_SEND_RATE_HZ_5000 = 0x02,
	EP1_AG08A_SEND_RATE_HZ_2500 = 0x03,
	EP1_AG08A_SEND_RATE_HZ_1250 = 0x04,
	EP1_AG08A_SEND_RATE_HZ_0625 = 0x05,
	EP1_AG08A_SEND_RATE_HZ_0312_5 = 0x06,
	EP1_AG08A_SEND_RATE_HZ_0156_25 = 0x07,
	EP1_AG08A_SEND_RATE_HZ_0010 = 0x11,
	EP1_AG08A_SEND_RATE_HZ_0001 = 0x12,
	EP1_AG08A_SEND_RATE_HZ_0000_1 = 0x13,
	EP1_AG08A_SEND_RATE_HZ_0000_01 = 0x14,
};

/*!
 * @brief input voltage
 */
enum EP1_AG08A_INPUT_VOLTAGE {
	EP1_AG08A_INPUT_VOLTAGE_Unknown = -1,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_10_PLUS_10 = 0x00,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_5_PLUS_5 = 0x01,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_2_5_PLUS_2_5 = 0x02,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_1_25_PLUS_1_25 = 0x03,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_0_8_PLUS_0_8 = 0x04,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_0_4_PLUS_0_4 = 0x05,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_0_2_PLUS_0_2 = 0x06,
	EP1_AG08A_INPUT_VOLTAGE_MINUS_0_1_PLUS_0_1 = 0x07,
	EP1_AG08A_INPUT_VOLTAGE_5 = 0x09,
};

/*!
 * message structure
 */
struct ep1_ag08a_msg_set_analog_input_s {
	int send_rate; /* mHz */
	int vmin[EP1_AG08A_CH_NUM]; /* mV */
	int vmax[EP1_AG08A_CH_NUM]; /* mV */
};
typedef struct ep1_ag08a_msg_set_analog_input_s ep1_ag08a_msg_set_analog_input_t;

struct ep1_ag08a_msg_control_analog_input_s {
	bool start[EP1_AG08A_CH_NUM];
};
typedef struct ep1_ag08a_msg_control_analog_input_s ep1_ag08a_msg_control_analog_input_t;

struct ep1_ag08a_msg_set_analog_output_s {
	int waveform_type;
	int voltage; /* mV */
	int frequency; /* mHz */
};
typedef struct ep1_ag08a_msg_set_analog_output_s ep1_ag08a_msg_set_analog_output_t;

struct ep1_ag08a_msg_control_analog_output_s {
	bool start;
};
typedef struct ep1_ag08a_msg_control_analog_output_s ep1_ag08a_msg_control_analog_output_t;

struct ep1_ag08a_msg_analog_input_s {
	ep1_ag08a_msg_set_analog_input_t cfg;
	ep1_ag08a_msg_control_analog_input_t ctrl;
};
typedef struct ep1_ag08a_msg_analog_input_s ep1_ag08a_msg_analog_input_t;

struct ep1_ag08a_msg_analog_output_s {
	ep1_ag08a_msg_set_analog_output_t cfg;
	ep1_ag08a_msg_control_analog_output_t ctrl;
};
typedef struct ep1_ag08a_msg_analog_output_s ep1_ag08a_msg_analog_output_t;

struct ep1_ag08a_msg_resp_get_status_s {
	ep1_ag08a_msg_analog_input_t in;
	ep1_ag08a_msg_analog_output_t out;
};
typedef struct ep1_ag08a_msg_resp_get_status_s ep1_ag08a_msg_resp_get_status_t;

/*!
 * payload structure
 */
struct ep1_ag08a_payload_notify_analog_input_s {
	apt_usbtrx_timestamp_t timestamp;
	u8 channel;
	u16 *data;
} __attribute__((packed));
typedef struct ep1_ag08a_payload_notify_analog_input_s ep1_ag08a_payload_notify_analog_input_t;

#endif /* __EP1_AG08A_CMD_DEF_H__ */
