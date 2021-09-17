// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include <linux/slab.h>

#include "../apt_usbtrx_core.h" /* send_msg(), wait_msg() */
#include "ep1_ch02a_cmd.h"
#include "ep1_ch02a_msg.h"

/*!
 * @brief get status (copy from ap_ct2a)
 */
int ep1_ch02a_get_status(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_resp_get_status_t *status)
{
	int req_data_size = EP1_CH02A_CMD_LENGTH_GET_STATUS;
	char *req_data;
	int resp_data_size = EP1_CH02A_CMD_LENGTH_RESPONSE_GET_STATUS;
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
	req_msg.id = EP1_CH02A_CMD_GetStatus;
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
	result = apt_usbtrx_wait_msg(dev, EP1_CH02A_CMD_ResponseGetStatus, APT_USBTRX_CMD_Unknown, resp_data,
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

	result = ep1_ch02a_msg_parse_response_get_status(resp_msg.payload, resp_msg.payload_size, status);
	if (result != RESULT_Success) {
		EMSG("ep1_ch02a_msg_parse_response_get_status().. Error");
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
 * @brief set bittiming
 */
int ep1_ch02a_set_bit_timing(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_set_bit_timing_t *param, bool *success)
{
	int req_data_size = EP1_CH02A_CMD_LENGTH_SET_BIT_TIMING;
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
	req_msg.id = EP1_CH02A_CMD_SetBitTiming;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	result = ep1_ch02a_msg_pack_set_bit_timing(param, req_msg.payload, req_msg.payload_size);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_msg_pack_set_bit_timing().. Error");
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
	if (id != EP1_CH02A_CMD_SetBitTiming) {
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

int ep1_ch02a_get_bit_timing(apt_usbtrx_dev_t *dev, ep1_ch02a_msg_resp_get_bit_timing_t *status)
{
	int req_data_size = EP1_CH02A_CMD_LENGTH_GET_BIT_TIMING;
	char *req_data;
	int resp_data_size = EP1_CH02A_CMD_LENGTH_RESPONSE_GET_BIT_TIMING;
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
	req_msg.id = EP1_CH02A_CMD_GetBitTiming;
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
	result = apt_usbtrx_wait_msg(dev, EP1_CH02A_CMD_ResponseGetBitTiming, APT_USBTRX_CMD_Unknown, resp_data,
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

	result = ep1_ch02a_msg_parse_response_get_bit_timing(resp_msg.payload, resp_msg.payload_size, status);
	if (result != RESULT_Success) {
		EMSG("ep1_ch02a_msg_parse_response_get_bit_timing().. Error");
		kfree(req_data);
		kfree(resp_data);
		return RESULT_Failure;
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}
