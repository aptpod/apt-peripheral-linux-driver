/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_CMD_H__
#define __AP_CT2A_CMD_H__

#include "ap_ct2a_cmd_def.h"

/*!
 * @brief set mode
 */
int apt_usbtrx_set_mode(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_set_mode_t *param, bool *success);

/*!
 * @brief get status
 */
int apt_usbtrx_get_status(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_resp_get_status_t *status);

/*!
 * @brief reset CAN summary
 */
int apt_usbtrx_reset_can_summary(apt_usbtrx_dev_t *dev, bool *success);

/*!
 * @brief start/stop can
 */
int apt_usbtrx_start_stop_can(apt_usbtrx_dev_t *dev, bool start, bool *success);

/*!
 * @brief set trigger
 */
int apt_usbtrx_set_trigger(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_set_trigger_t *param, bool *success);

#endif /* __AP_CT2A_CMD_H__ */
