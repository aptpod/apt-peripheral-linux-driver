/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_MSG_H__
#define __EP1_AG08A_MSG_H__

#include "ep1_ag08a_cmd_def.h"

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
