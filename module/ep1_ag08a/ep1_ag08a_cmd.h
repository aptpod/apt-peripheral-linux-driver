/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_CMD_H__
#define __EP1_AG08A_CMD_H__

#include "ep1_ag08a_cmd_def.h"

/*!
 * @brief get status
 */
int ep1_ag08a_get_status(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_resp_get_status_t *status);

/*!
 * @brief set analog input
 */
int ep1_ag08a_set_analog_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_input_t *cfg, bool *success);

/*!
 * @brief control analog input
 */
int ep1_ag08a_control_analog_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_input_t *cfg, bool *success);

/*!
 * @brief set analog output
 */
int ep1_ag08a_set_analog_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_output_t *cfg, bool *success);

/*!
 * @brief control analog output
 */
int ep1_ag08a_control_analog_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_output_t *param, bool *success);

#endif /* __EP1_AG08A_CMD_H__ */
