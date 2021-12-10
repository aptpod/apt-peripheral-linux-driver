/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_MSG_H__
#define __EP1_AG08A_MSG_H__

#include "ep1_ag08a_cmd_def.h"

#define OUTPUT_CONVERT_SCALE_VOLTAGE 20
#define OUTPUT_CONVERT_SCALE_FREQUENCY 1000

/*!
 * @brief convert output voltage from value
 */
static inline int ep1_ag08a_msg_convert_output_voltage_from_value(int value)
{
	return value * OUTPUT_CONVERT_SCALE_VOLTAGE;
}

/*!
 * @brief convert output voltage to value
 */
static inline int ep1_ag08a_msg_convert_output_voltage_to_value(int voltage)
{
	return voltage / OUTPUT_CONVERT_SCALE_VOLTAGE;
}

/*!
 * @brief convert output frequency from value
 */
static inline int ep1_ag08a_msg_convert_output_frequency_from_value(int value)
{
	return value * OUTPUT_CONVERT_SCALE_FREQUENCY;
}

/*!
 * @brief convert output frequency to value
 */
static inline int ep1_ag08a_msg_convert_output_frequency_to_value(int frequency)
{
	return frequency / OUTPUT_CONVERT_SCALE_FREQUENCY;
}

/*!
 * @brief convert send rate to value
 */
int ep1_ag08a_msg_convert_send_rate_to_value(int send_rate_milli_hz);

/*!
 * @brief convert send rate from value
 */
int ep1_ag08a_msg_convert_send_rate_from_value(int send_rate);

/*!
 * @brief convert input voltage to nano scale
 */
int ep1_ag08a_msg_convert_input_voltage_to_nano_scale(int voltage);

/*!
 * @brief convert input voltage to value
 */
int ep1_ag08a_msg_convert_input_voltage_to_value(int vmin, int vmax);

/*!
 * @brief convert input voltage from value
 */
int ep1_ag08a_msg_convert_input_voltage_from_value(int voltage, int *vmin, int *vmax);

/*!
 * @brief parse (Response Get Status)
 */
int ep1_ag08a_msg_parse_response_get_status(u8 *data, int data_size, ep1_ag08a_msg_resp_get_status_t *status);

/*!
 * @brief pack (Set Analog Input)
 */
int ep1_ag08a_msg_pack_set_analog_input(ep1_ag08a_msg_set_analog_input_t *param, u8 *data, int data_size);

/*!
 * @brief pack (Control Analog Input)
 */
int ep1_ag08a_msg_pack_control_analog_input(ep1_ag08a_msg_control_analog_input_t *param, u8 *data, int data_size);

/*!
 * @brief pack (Set Analog Output)
 */
int ep1_ag08a_msg_pack_set_analog_output(ep1_ag08a_msg_set_analog_output_t *param, u8 *data, int data_size);

/*!
 * @brief pack (Control Analog Output)
 */
int ep1_ag08a_msg_pack_control_analog_output(ep1_ag08a_msg_control_analog_output_t *param, u8 *data, int data_size);

#endif /* __EP1_AG08A_MSG_H__ */
