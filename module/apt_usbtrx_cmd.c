// SPDX-License-Identifier: GPL-2.0
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#include <linux/slab.h>

#include "apt_usbtrx_cmd_def.h"
#include "apt_usbtrx_cmd.h"
#include "apt_usbtrx_msg.h"
#include "apt_usbtrx_core.h"

/*!
 * @brief get device id
 */
int apt_usbtrx_get_device_id(apt_usbtrx_dev_t *dev, char *device_id, int device_id_size, int *channel)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_GET_DEVICE_ID;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_RESPONSE_GET_DEVICE_ID;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;

	CHKMSG("ENTER");

	if (device_id == NULL) {
		EMSG("device_id is NULL");
		return RESULT_Failure;
	}

	if (device_id_size < APT_USBTRX_CMD_DEVICE_ID_LENGTH) {
		EMSG("device id size is not enough, <size:%d>", device_id_size);
		return RESULT_Failure;
	}

	if (channel == NULL) {
		EMSG("channel is NULL");
		return RESULT_Failure;
	}

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_GetDeviceId;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ResponseGetDeviceId, APT_USBTRX_CMD_Unknown, resp_data,
				     resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_response_get_device_id(resp_msg.payload, resp_msg.payload_size, device_id,
							     channel);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_response_get_device_id().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief get fw version
 */
int apt_usbtrx_get_fw_version(apt_usbtrx_dev_t *dev, int *major_version, int *minor_version)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_GET_FW_VERSION;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_RESPONSE_GET_FW_VERSION;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_GetFWVersion;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ResponseGetFWVersion, APT_USBTRX_CMD_Unknown, resp_data,
				     resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_response_get_fw_version(resp_msg.payload, resp_msg.payload_size, major_version,
							      minor_version);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_response_get_fw_version().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief get fw version revision
 */
int apt_usbtrx_get_fw_version_revision(apt_usbtrx_dev_t *dev, int *major_version, int *minor_version, int *revision)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_GET_FW_VERSION_REVISION;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_RESPONSE_GET_FW_VERSION_REVISION;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_GetFWVersionRevision;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ResponseGetFWVersionRevision, APT_USBTRX_CMD_Unknown,
				     resp_data, resp_data_size);
	if (result != RESULT_Success) {
		/* Some devices do not support this command, so it is not an error. */
		//EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_response_get_fw_version_revision(resp_msg.payload, resp_msg.payload_size,
								       major_version, minor_version, revision);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_response_get_fw_version_revision().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief enable reset timestamp
 */
int apt_usbtrx_enable_reset_ts(apt_usbtrx_dev_t *dev, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_ENABLE_RESET_TS;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_ACK;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;
	int id;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_EnableResetTS;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ACK, APT_USBTRX_CMD_NACK, resp_data, resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_ack(resp_msg.payload, resp_msg.payload_size, &id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_ack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	if (id != APT_USBTRX_CMD_EnableResetTS) {
		EMSG("%s(): ack/nack is not match, <id:0x%02x>", __func__, id);
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	switch (resp_msg.id) {
	case APT_USBTRX_CMD_ACK:
		*success = true;
		break;
	case APT_USBTRX_CMD_NACK:
		*success = false;
		break;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief reset timestamp
 */
int apt_usbtrx_reset_ts(apt_usbtrx_dev_t *dev, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_RESET_TS;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_ACK;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;
	int id;
	struct timespec64 ts;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** set resettime ***/
	get_raw_monootnic_ts64(&ts);
	*dev->resettime = ts;
	IMSG("resettime: %lld.%09ld", (s64)ts.tv_sec, ts.tv_nsec);

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_ResetTS;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ACK, APT_USBTRX_CMD_NACK, resp_data, resp_data_size);
	if (result != RESULT_Success) {
		if (result == RESULT_Timeout) {
			WMSG("maybe connect sync cable ?");
			*success = true;
			kfree(req_data);
			kfree(resp_data);
			return RESULT_Timeout;
		}
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_ack(resp_msg.payload, resp_msg.payload_size, &id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_ack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	if (id != APT_USBTRX_CMD_ResetTS) {
		EMSG("%s(): ack/nack is not match, <id:0x%02x>", __func__, id);
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	switch (resp_msg.id) {
	case APT_USBTRX_CMD_ACK:
		*success = true;
		break;
	case APT_USBTRX_CMD_NACK:
		*success = false;
		break;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief reset device
 */
int apt_usbtrx_reset_device(apt_usbtrx_dev_t *dev, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_RESET_DEVICE;
	char *req_data;
	int result;
	apt_usbtrx_msg_t req_msg;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_ResetDevice;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	*success = true;
#if 0
	int resp_data_size = APT_USBTRX_CMD_LENGTH_ACK;
	char resp_data[resp_data_size];
	apt_usbtrx_msg_t resp_msg;
	int id;

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ACK, APT_USBTRX_CMD_NACK, resp_data, resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_ack(resp_msg.payload, resp_msg.payload_size, &id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_ack().. Error");
		return RESULT_Failure;
	}
	if (id != APT_USBTRX_CMD_ResetDevice) {
		EMSG("%s(): ack/nack is not match, <id:0x%02x>", __func__, id);
		return RESULT_Failure;
	}
	switch (resp_msg.id) {
	case APT_USBTRX_CMD_ACK:
		*success = true;
		break;
	case APT_USBTRX_CMD_NACK:
		*success = false;
		break;
	}
#endif

	kfree(req_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief get serial no
 */
int apt_usbtrx_get_serial_no(apt_usbtrx_dev_t *dev, char *serial_no, int serial_no_size, int *channel, int *sync_pulse)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_GET_SERIAL_NO;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_RESPONSE_GET_SERIAL_NO;
	char *resp_data;
	int result;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;

	CHKMSG("ENTER");

	if (serial_no == NULL) {
		EMSG("serial_no is NULL");
		return RESULT_Failure;
	}

	if (serial_no_size < APT_USBTRX_CMD_SERIAL_NO_LENGTH) {
		EMSG("device id size is not enough, <size:%d>", serial_no_size);
		return RESULT_Failure;
	}

	if (channel == NULL) {
		EMSG("channel is NULL");
		return RESULT_Failure;
	}

	if (sync_pulse == NULL) {
		EMSG("sync_pulse is NULL");
		return RESULT_Failure;
	}

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	resp_data = kzalloc(resp_data_size, GFP_KERNEL);
	if (resp_data == NULL) {
		EMSG("kzalloc().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_GetSerialNo;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ResponseGetSerialNo, APT_USBTRX_CMD_Unknown, resp_data,
				     resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_response_get_serial_no(resp_msg.payload, resp_msg.payload_size, serial_no,
							     channel, sync_pulse);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_response_get_serial_no().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

int apt_usbtrx_move_dfu(apt_usbtrx_dev_t *dev, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_MOVE_DFU;
	char *req_data;
	int result;
	apt_usbtrx_msg_t req_msg;

	CHKMSG("ENTER");

	req_data = kzalloc(req_data_size, GFP_KERNEL);
	if (req_data == NULL) {
		EMSG("kzalloc().. Error");
		return RESULT_Failure;
	}

	/*** request ***/
	req_msg.id = APT_USBTRX_CMD_MoveDFU;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack(&req_msg, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_send_msg(dev, req_data, req_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_send_msg().. Error");
		kfree(req_data);
		return RESULT_Failure;
	}

	*success = true;

#if 0
	int resp_data_size = APT_USBTRX_CMD_LENGTH_ACK;
	char *resp_data;
	apt_usbtrx_msg_t resp_msg;
	int id;

	/*** response ***/
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ACK, APT_USBTRX_CMD_NACK, resp_data, resp_data_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_wait_msg().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse(resp_data, resp_data_size, &resp_msg);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	result = apt_usbtrx_msg_parse_ack(resp_msg.payload, resp_msg.payload_size, &id);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_ack().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	if (id != APT_USBTRX_CMD_MoveDFU) {
		EMSG("%s(): ack/nack is not match, <id:0x%02x>", __func__, id);
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}
	switch (resp_msg.id) {
	case APT_USBTRX_CMD_ACK:
		*success = true;
		break;
	case APT_USBTRX_CMD_NACK:
		*success = false;
		break;
	}
#endif

	kfree(req_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}
