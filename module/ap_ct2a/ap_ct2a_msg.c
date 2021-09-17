// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ap_ct2a_msg.h"

/*!
 * @brief pack (Set Trigger)
 */
int apt_usbtrx_msg_pack_set_trigger(apt_usbtrx_msg_set_trigger_t *param, u8 *data, int data_size)
{
	*data = (u8)(param->input.id);
	*(data + 1) = (u8)((param->input.id) >> 8) & 0x07;
	if (param->input.enable == 1) {
		*(data + 1) |= 0x80;
	}
	if (param->input.logic == 0) {
		*(data + 1) |= 0x40;
	}
	*(data + 2) = (u8)(param->output.id);
	*(data + 3) = (u8)((param->output.id) >> 8) & 0x07;
	if (param->output.enable == 1) {
		*(data + 3) |= 0x80;
	}
	if (param->output.logic == 0) {
		*(data + 3) |= 0x40;
	}
	*(data + 4) = (u8)(param->pulse_width);
	*(data + 5) = (u8)((param->pulse_width) >> 8);
	*(data + 6) = (u8)(param->pulse_interval);
	*(data + 7) = (u8)((param->pulse_interval) >> 8);

	return RESULT_Success;
}

/*!
 * @brief covert baudrate mode
 */
static int apt_usbtrx_convert_baudrate_mode(int baudrate)
{
	int mode;

	switch (baudrate) {
	case 33:
		mode = APT_USBTRX_BITRATE_MODE_33K;
		break;
	case 50:
		mode = APT_USBTRX_BITRATE_MODE_50K;
		break;
	case 83:
		mode = APT_USBTRX_BITRATE_MODE_83K;
		break;
	case 100:
		mode = APT_USBTRX_BITRATE_MODE_100K;
		break;
	case 125:
		mode = APT_USBTRX_BITRATE_MODE_125K;
		break;
	case 250:
		mode = APT_USBTRX_BITRATE_MODE_250K;
		break;
	case 500:
		mode = APT_USBTRX_BITRATE_MODE_500K;
		break;
	case 1000:
		mode = APT_USBTRX_BITRATE_MODE_1M;
		break;
	default:
		EMSG("bitrate is invalid, <baudrate:%d>", baudrate);
		return APT_USBTRX_BITRATE_MODE_Unknown;
	}

	return mode;
}

/*!
 * @brief covert baudrate value
 */
static int apt_usbtrx_convert_baudrate_value(int mode)
{
	int baudrate = -1;

	switch (mode) {
	case APT_USBTRX_BITRATE_MODE_None:
		baudrate = 0;
		break;
	case APT_USBTRX_BITRATE_MODE_33K:
		baudrate = 33;
		break;
	case APT_USBTRX_BITRATE_MODE_50K:
		baudrate = 50;
		break;
	case APT_USBTRX_BITRATE_MODE_83K:
		baudrate = 83;
		break;
	case APT_USBTRX_BITRATE_MODE_100K:
		baudrate = 100;
		break;
	case APT_USBTRX_BITRATE_MODE_125K:
		baudrate = 125;
		break;
	case APT_USBTRX_BITRATE_MODE_250K:
		baudrate = 250;
		break;
	case APT_USBTRX_BITRATE_MODE_500K:
		baudrate = 500;
		break;
	case APT_USBTRX_BITRATE_MODE_1M:
		baudrate = 1000;
		break;
	default:
		EMSG("bitrate mode is invalid, <baudrate mode:%d>", mode);
		return -1;
	}

	return baudrate;
}

/*!
 * @brief pack (Set Mode)
 */
int apt_usbtrx_msg_pack_set_mode(apt_usbtrx_msg_set_mode_t *param, u8 *data, int data_size)
{
	int baudrate_mode;
	int n = 0;

	baudrate_mode = apt_usbtrx_convert_baudrate_mode(param->baudrate);
	if (baudrate_mode == APT_USBTRX_BITRATE_MODE_Unknown) {
		EMSG("apt_usbtrx_convert_baudrate_mode().. Error, <baudrate:%d>", param->baudrate);
		return RESULT_Failure;
	}

	data[n] = ((param->silent == false) ? 1 : 0) << 6;
	data[n] |= ((param->start == true) ? 1 : 0) << 5;
	data[n] |= baudrate_mode;
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief pack (Start/Stop CAN)
 */
int apt_usbtrx_msg_pack_start_stop_can(bool start, u8 *data, int data_size)
{
	int n = 0;

	data[n] = (start == true) ? 1 : 0;
	n = n + 1;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Status)
 */
int apt_usbtrx_msg_parse_response_get_status(u8 *data, int data_size, apt_usbtrx_msg_resp_get_status_t *status)
{
	int n = 0;
	int baudrate_mode;
	int baudrate;
	int input_lower_id;
	int input_upper_id;
	int output_lower_id;
	int output_upper_id;

	status->enable_trigger = (((data[n] >> 7) & 0x01) == 1) ? true : false;
	status->mode.silent = (((data[n] >> 6) & 0x01) == 1) ? false : true;
	status->mode.start = (((data[n] >> 5) & 0x01) == 1) ? true : false;
	baudrate_mode = data[n] & 0x1f;
	baudrate = apt_usbtrx_convert_baudrate_value(baudrate_mode);
	if (baudrate == -1) {
		EMSG("apt_usbtrx_convert_baudrate_value().. Error, <mode:0x%02x>", baudrate_mode);
		return RESULT_Failure;
	}
	status->mode.baudrate = baudrate;
	n = n + 1;

	input_lower_id = data[n];
	n = n + 1;

	input_upper_id = data[n] & 0x07;
	status->trigger.input.id = input_lower_id | (input_upper_id << 8);
	status->trigger.input.enable = (((data[n] >> 7) & 0x01) == 1) ? true : false;
	status->trigger.input.logic =
		(((data[n] >> 6) & 0x01) == 1) ? APT_USBTRX_TRIGGER_LOGIC_Negative : APT_USBTRX_TRIGGER_LOGIC_Positive;
	n = n + 1;

	output_lower_id = data[n];
	n = n + 1;

	output_upper_id = data[n] & 0x07;
	status->trigger.output.id = output_lower_id | (output_upper_id << 8);
	status->trigger.output.enable = (((data[n] >> 7) & 0x01) == 1) ? true : false;
	status->trigger.output.logic =
		(((data[n] >> 6) & 0x01) == 1) ? APT_USBTRX_TRIGGER_LOGIC_Negative : APT_USBTRX_TRIGGER_LOGIC_Positive;
	n = n + 1;

	status->trigger.pulse_width = data[n] | (data[n + 1] << 8);
	n = n + 2;

	status->trigger.pulse_interval = data[n] | (data[n + 1] << 8);
	n = n + 2;

	return RESULT_Success;
}

/*!
 * @brief parse (Notify Receive CAN Frame)
 */
int apt_usbtrx_msg_parse_notify_recv_can_frame(u8 *data, int data_size, struct timespec *ts, struct can_frame *frame)
{
	int n = 0;

	/*** timestamp (sec) ***/
	ts->tv_sec = data[n];
	ts->tv_sec |= data[n + 1] << 8;
	ts->tv_sec |= data[n + 2] << 16;
	ts->tv_sec |= data[n + 3] << 24;
	n = n + 4;

	/*** timestamp (sec) ***/
	ts->tv_nsec = data[n];
	ts->tv_nsec |= data[n + 1] << 8;
	ts->tv_nsec |= data[n + 2] << 16;
	ts->tv_nsec |= data[n + 3] << 24;
	n = n + 4;

	/*** id ***/
	frame->can_id = data[n];
	frame->can_id |= data[n + 1] << 8;
	frame->can_id |= data[n + 2] << 16;
	frame->can_id |= data[n + 3] << 24;
	n = n + 4;

	/*** dlc ***/
	frame->can_dlc = data[n] & 0x0f;
	n = n + 1;

	/*** pad, res0, res1 ***/
	frame->__pad = 0;
	frame->__res0 = 0;
	frame->__res1 = 0;

	/*** data ***/
	memcpy(frame->data, &data[n], 8);
	n = n + 8;

	return RESULT_Success;
}

/*
 * @brief parse (Notify Receive CAN Summary)
 */
int apt_usbtrx_msg_parse_notify_recv_can_summary(u8 *data, int data_size, u32 *count, struct can_frame *frame)
{
	int n = 0;

	/*** count ***/
	*count = data[n];
	*count |= data[n + 1] << 8;
	*count |= data[n + 2] << 16;
	*count |= data[n + 3] << 24;
	n = n + 4;

	/*** id ***/
	frame->can_id = data[n];
	frame->can_id |= data[n + 1] << 8;
	frame->can_id |= data[n + 2] << 16;
	frame->can_id |= data[n + 3] << 24;
	n = n + 4;

	/*** dlc ***/
	frame->can_dlc = data[n] & 0x0f;
	n = n + 1;

	/*** pad, res0, res1 ***/
	frame->__pad = 0;
	frame->__res0 = 0;
	frame->__res1 = 0;

	/*** data ***/
	memcpy(frame->data, &data[n], 8);
	n = n + 8;

	return RESULT_Success;
}
