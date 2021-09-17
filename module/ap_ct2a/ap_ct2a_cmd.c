// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/slab.h>

#include "../apt_usbtrx_core.h" /* send_msg(), wait_msg() */
#include "ap_ct2a_cmd.h"
#include "ap_ct2a_msg.h"

/*!
 * @brief set mode
 */
int apt_usbtrx_set_mode(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_set_mode_t *param, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_SET_MODE;
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
	req_msg.id = APT_USBTRX_CMD_SetMode;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack_set_mode(param, req_msg.payload, req_msg.payload_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack_set_mode().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	{
		char linebuf[3 * req_msg.payload_size + 1];

		hex_dump_to_buffer(req_msg.payload, req_msg.payload_size, 16, 1, linebuf, sizeof(linebuf), 0);
		DMSG("%s: %s", "req", linebuf);
	}

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
	if (id != APT_USBTRX_CMD_SetMode) {
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
 * @brief get status
 */
int apt_usbtrx_get_status(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_resp_get_status_t *status)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_GET_STATUS;
	char *req_data;
	int resp_data_size = APT_USBTRX_CMD_LENGTH_RESPONSE_GET_STATUS;
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
	req_msg.id = APT_USBTRX_CMD_GetStatus;
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
	result = apt_usbtrx_wait_msg(dev, APT_USBTRX_CMD_ResponseGetStatus, APT_USBTRX_CMD_Unknown, resp_data,
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

	{
		char linebuf[3 * resp_msg.payload_size + 1];

		hex_dump_to_buffer(resp_msg.payload, resp_msg.payload_size, 16, 1, linebuf, sizeof(linebuf), 0);
		DMSG("%s: %s", "resp", linebuf);
	}

	result = apt_usbtrx_msg_parse_response_get_status(resp_msg.payload, resp_msg.payload_size, status);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_parse_response_get_status().. Error");
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
 * @brief reset CAN summary
 */
int apt_usbtrx_reset_can_summary(apt_usbtrx_dev_t *dev, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_RESET_CAN_SUMMARY;
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
	req_msg.id = APT_USBTRX_CMD_ResetCANSummary;
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
	if (id != APT_USBTRX_CMD_ResetCANSummary) {
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
 * @brief start/stop can
 */
int apt_usbtrx_start_stop_can(apt_usbtrx_dev_t *dev, bool start, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_START_STOP_CAN;
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
	req_msg.id = APT_USBTRX_CMD_StartStopCAN;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack_start_stop_can(start, req_msg.payload, req_msg.payload_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack_start_stop_can().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	{
		char linebuf[3 * req_msg.payload_size + 1];

		hex_dump_to_buffer(req_msg.payload, req_msg.payload_size, 16, 1, linebuf, sizeof(linebuf), 0);
		DMSG("%s: %s", "req", linebuf);
	}

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
	if (id != APT_USBTRX_CMD_StartStopCAN) {
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

	return RESULT_Success;
}

/*!
 * @brief set trigger
 */
int apt_usbtrx_set_trigger(apt_usbtrx_dev_t *dev, apt_usbtrx_msg_set_trigger_t *param, bool *success)
{
	int req_data_size = APT_USBTRX_CMD_LENGTH_SET_TRIGGER;
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
	req_msg.id = APT_USBTRX_CMD_SetTrigger;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = apt_usbtrx_msg_pack_set_trigger(param, req_msg.payload, req_msg.payload_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack_set_trigger().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	{
		char linebuf[3 * req_msg.payload_size + 1];

		hex_dump_to_buffer(req_msg.payload, req_msg.payload_size, 16, 1, linebuf, sizeof(linebuf), 0);
		DMSG("%s(): %s", "req", linebuf);
	}

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
	if (id != APT_USBTRX_CMD_SetTrigger) {
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
