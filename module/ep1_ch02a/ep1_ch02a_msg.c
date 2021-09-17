// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_ch02a_msg.h"

/*!
 * TODO this function is copied from ap_ct2a
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
 * @brief parse (Response Get Status)
 */
int ep1_ch02a_msg_parse_response_get_status(u8 *data, int data_size, ep1_ch02a_msg_resp_get_status_t *status)
{
	int n = 0;
	int baudrate_mode;
	int baudrate;

	status->enable_sync = (((data[n] >> 7) & 0x01) == 1) ? true : false;
	status->mode.silent = (((data[n] >> 6) & 0x01) == 1) ? false : true;
	status->mode.start = (((data[n] >> 5) & 0x01) == 1) ? true : false;
	baudrate_mode = data[n] & 0x1f;
	baudrate = apt_usbtrx_convert_baudrate_value(baudrate_mode);
	if (baudrate == -1) {
		EMSG("apt_usbtrx_convert_baudrate_value().. Error, <mode:0x%02x>", baudrate_mode);
		return RESULT_Failure;
	}
	status->mode.baudrate = baudrate;

	return RESULT_Success;
}

/*!
 * @brief pack (Set BitTiming)
 */
int ep1_ch02a_msg_pack_set_bit_timing(ep1_ch02a_msg_set_bit_timing_t *param, u8 *data, int data_size)
{
	int n = 0;

	data[n] = param->prop_seg;
	n = n + 1;

	data[n] = param->phase_seg1;
	n = n + 1;

	data[n] = param->phase_seg2;
	n = n + 1;

	data[n] = param->sjw;
	n = n + 1;

	data[n] = param->brp;
	data[n + 1] = (param->brp >> 8);

	return RESULT_Success;
}

int ep1_ch02a_msg_parse_response_get_bit_timing(u8 *data, int data_size,
						ep1_ch02a_msg_resp_get_bit_timing_t *bit_timing)
{
	int n = 0;

	bit_timing->can_clock = data[n] | (data[n + 1] << 8) | (data[n + 2] << 16) | (data[n + 3] << 24);
	n = n + 4;

	bit_timing->params.prop_seg = data[n];
	n = n + 1;

	bit_timing->params.phase_seg1 = data[n];
	n = n + 1;

	bit_timing->params.phase_seg2 = data[n];
	n = n + 1;

	bit_timing->params.sjw = data[n];
	n = n + 1;

	bit_timing->params.brp = data[n] | (data[n + 1] << 8);

	return RESULT_Success;
}
