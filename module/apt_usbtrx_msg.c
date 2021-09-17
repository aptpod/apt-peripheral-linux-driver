// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT USB peripherals.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/slab.h>

#include "apt_usbtrx_def.h"
#include "apt_usbtrx_msg.h"

/*!
 * @brief parse
 */
int apt_usbtrx_msg_parse(u8 *data, int data_size, apt_usbtrx_msg_t *msg)
{
	u8 sob;
	u8 length;
	u8 id;
	u8 payload_size;
	u8 eob;
	int n = 0;

	if (msg == NULL) {
		EMSG("msg is NULL");
		return RESULT_Failure;
	}

	sob = data[n];
	if (sob != APT_USBTRX_MSG_SOB) {
#if 0
		DMSG("start data is not SOB, <0x%02x>", sob);
#endif
		return RESULT_Failure;
	}
	n = n + 1;

	length = data[n];
	if (length < APT_USBTRX_CMD_MIN_LENGTH) {
#if 0
		EMSG("paket length is under min cmd length, <length:%d>", length);
#endif
		return RESULT_Failure;
	}
	if (length > APT_USBTRX_MSG_MAX_LENGTH) {
		EMSG("packet length is over max msg length, <length:%d>", length);
		return RESULT_Failure;
	}
	if (length > APT_USBTRX_CMD_MAX_LENGTH) {
		EMSG("paket length is over max cmd length, <length:%d>", length);
		return RESULT_Failure;
	}
	if (length > data_size) {
#if 0
		EMSG("data size is not enough, <length:%d> data size=%d", length, data_size);
#endif
		return RESULT_NotEnough;
	}
	if (data[length - 1] != APT_USBTRX_MSG_EOB) {
		return RESULT_Failure;
	}
	n = n + 1;

	id = data[n];
	n = n + 1;

	payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(length);
	if (payload_size > APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH)) {
		EMSG("length is over max payload size, <payload_size:%d>", payload_size);
		return RESULT_Failure;
	}
	memcpy(msg->payload, &data[n], payload_size);
	n = n + payload_size;

	eob = data[n];
	if (eob != APT_USBTRX_MSG_EOB) {
		EMSG("end data is not EOB, <0x%02x>", eob);
		return RESULT_Failure;
	}
	n = n + 1;

	msg->id = id;
	msg->payload_size = payload_size;

	return RESULT_Success;
}

/*!
 * @brief pack
 */
int apt_usbtrx_msg_pack(apt_usbtrx_msg_t *msg, u8 *data, int data_size)
{
	int n = 0;
	int length;

	if (msg == NULL) {
		EMSG("msg is NULL");
		return RESULT_Failure;
	}

	length = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
	if (length > data_size) {
		EMSG("data size is not enough, <length:%d> data size=%d", length, data_size);
		return RESULT_Failure;
	}

	data[n] = APT_USBTRX_MSG_SOB;
	n = n + 1;

	data[n] = length;
	n = n + 1;

	data[n] = msg->id;
	n = n + 1;

	memcpy(&data[n], msg->payload, msg->payload_size);
	n = n + msg->payload_size;

	data[n] = APT_USBTRX_MSG_EOB;
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Device Id)
 */
int apt_usbtrx_msg_parse_response_get_device_id(u8 *data, int data_size, char *device_id, int *channel)
{
	int n = 0;

	memcpy(device_id, &data[n], 4);
	n = n + 4;

	*channel = data[n] & 0x01;
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Serial No)
 */
int apt_usbtrx_msg_parse_response_get_serial_no(u8 *data, int data_size, char *serial_no, int *channel, int *sync_pulse)
{
	int n = 0;

	memcpy(serial_no, &data[n], 14);
	n = n + 14;

	*channel = data[n] & 0x01;
	*sync_pulse = (data[n] >> 1) & 0x01;
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get FW Version)
 */
int apt_usbtrx_msg_parse_response_get_fw_version(u8 *data, int data_size, int *major_version, int *minor_version)
{
	int n = 0;

	*major_version = data[n];
	n = n + 1;

	*minor_version = data[n];
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get FW Version Revision)
 */
int apt_usbtrx_msg_parse_response_get_fw_version_revision(u8 *data, int data_size, int *major_version,
							  int *minor_version, int *revision)
{
	int n = 0;

	*major_version = data[n];
	n = n + 1;

	*minor_version = data[n];
	n = n + 1;

	*revision = data[n];

	return RESULT_Success;
}

/*!
 * @brief parse (ACK/NACK)
 */
int apt_usbtrx_msg_parse_ack(u8 *data, int data_size, int *id)
{
	int n = 0;

	*id = data[n];
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Notify Buffer Status)
 */
int apt_usbtrx_msg_parse_notify_buffer_status(u8 *data, int data_size, int *rate)
{
	int n = 0;

	*rate = data[n];
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief get length
 */
int apt_usbtrx_msg_get_length(u8 *data, int data_size, u8 *length)
{
	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (length == NULL) {
		EMSG("length is NULL");
		return RESULT_Failure;
	}
	if (data_size < APT_USBTRX_CMD_MIN_LENGTH) {
		EMSG("invalid data_size");
		return RESULT_Failure;
	}

	*length = data[1];

	return RESULT_Success;
}
