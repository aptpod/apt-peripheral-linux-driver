/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_FOPS_H__
#define __EP1_AG08A_FOPS_H__

#include "../apt_usbtrx_def.h"
#include "ep1_ag08a_cmd.h"

/*!
 * @brief unique function prototype
 */
int ep1_ag08a_get_read_payload_size(const void *payload);
int ep1_ag08a_get_write_payload_size(const void *payload);
apt_usbtrx_timestamp_t *ep1_ag08a_get_read_payload_timestamp(const void *payload);
int ep1_ag08a_get_write_cmd_id(void);
int ep1_ag08a_get_fw_size(void);
long ep1_ag08a_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int ep1_ag08a_set_device_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_input_t *cfg);
int ep1_ag08a_set_device_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_set_analog_output_t *cfg);
int ep1_ag08a_control_device_input(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_input_t *ctrl);
int ep1_ag08a_control_device_output(apt_usbtrx_dev_t *dev, ep1_ag08a_msg_control_analog_output_t *ctrl);
int ep1_ag08a_stop_device_input(apt_usbtrx_dev_t *dev);
int ep1_ag08a_stop_device_output(apt_usbtrx_dev_t *dev);
int ep1_ag08a_is_device_input_start(apt_usbtrx_dev_t *dev, bool *start);
int ep1_ag08a_is_device_output_start(apt_usbtrx_dev_t *dev, bool *start);
int ep1_ag08a_open(apt_usbtrx_dev_t *dev);
int ep1_ag08a_close(apt_usbtrx_dev_t *dev);

#endif /* __EP1_AG08A_FOPS_H__ */
