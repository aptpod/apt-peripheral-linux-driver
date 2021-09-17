/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux.
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_MSG_H__
#define __APT_USBTRX_MSG_H__

#include <linux/types.h>

#include "apt_usbtrx_cmd_def.h"

/*!
 * @brief message
 */
#define APT_USBTRX_MSG_SOB (0xF7)
#define APT_USBTRX_MSG_EOB (0xFA)
#define APT_USBTRX_MSG_MAX_LENGTH (128)

/*!
 * @brief message length macro
 */
#define APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(length) ((length)-4)
#define APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(length) ((length) + 4)

/*!
 * @brief msg structure
 */
struct apt_usbtrx_msg_s {
	u8 id;
	u8 payload[APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH)];
	u8 payload_size;
};
typedef struct apt_usbtrx_msg_s apt_usbtrx_msg_t;

/*!
 * @brief parse
 */
int apt_usbtrx_msg_parse(u8 *data, int data_size, apt_usbtrx_msg_t *msg);

/*!
 * @brief pack
 */
int apt_usbtrx_msg_pack(apt_usbtrx_msg_t *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Device Id)
 */
int apt_usbtrx_msg_parse_response_get_device_id(u8 *data, int data_size, char *device_id, int *channel);

/*!
 * @brief parse (Response Get Serial No)
 */
int apt_usbtrx_msg_parse_response_get_serial_no(u8 *data, int data_size, char *serial_no, int *channel,
						int *sync_pulse);

/*!
 * @brief parse (Response Get FW Version)
 */
int apt_usbtrx_msg_parse_response_get_fw_version(u8 *data, int data_size, int *major_version, int *minor_version);

/*!
 * @brief parse (Response Get FW Version Revision)
 */
int apt_usbtrx_msg_parse_response_get_fw_version_revision(u8 *data, int data_size, int *major_version,
							  int *minor_version, int *revision);
/*!
 * @brief parse (ACK/NACK)
 */
int apt_usbtrx_msg_parse_ack(u8 *data, int data_size, int *id);

/*!
 * @brief parse (Notify Buffer Status)
 */
int apt_usbtrx_msg_parse_notify_buffer_status(u8 *data, int data_size, int *rate);

/*!
 * @brief get length
 */
int apt_usbtrx_msg_get_length(u8 *data, int data_size, u8 *length);

#endif /* __APT_USBTRX_MSG_H__ */
