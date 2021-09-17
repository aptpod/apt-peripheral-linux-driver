// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_ag08a_msg.h"

/*!
 * @brief convert send rate to value
 */
static int ep1_ag08a_msg_convert_send_rate_to_value(int send_rate_milli_hz)
{
	int send_rate;

	switch (send_rate_milli_hz) {
	case 10000000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_10K;
		break;
	case 5000000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_5000;
		break;
	case 2500000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_2500;
		break;
	case 1250000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_1250;
		break;
	case 625000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0625;
		break;
	case 312500:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0312_5;
		break;
	case 156250:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0156_25;
		break;
	case 10000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0010;
		break;
	case 1000:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0001;
		break;
	case 100:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0000_1;
		break;
	case 10:
		send_rate = EP1_AG08A_SEND_RATE_HZ_0000_01;
		break;
	default:
		EMSG("send_rate_milli_hz is invalid, <send_rate_milli_hz:%d>", send_rate_milli_hz);
		return EP1_AG08A_SEND_RATE_HZ_Unknown;
	}

	return send_rate;
}

/*!
 * @brief convert send rate from value
 */
static int ep1_ag08a_msg_convert_send_rate_from_value(int send_rate)
{
	int send_rate_milli_hz;

	switch (send_rate) {
	case EP1_AG08A_SEND_RATE_HZ_10K:
		send_rate_milli_hz = 10000000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_5000:
		send_rate_milli_hz = 5000000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_2500:
		send_rate_milli_hz = 2500000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_1250:
		send_rate_milli_hz = 1250000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0625:
		send_rate_milli_hz = 625000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0312_5:
		send_rate_milli_hz = 312500;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0156_25:
		send_rate_milli_hz = 156250;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0010:
		send_rate_milli_hz = 10000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0001:
		send_rate_milli_hz = 1000;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0000_1:
		send_rate_milli_hz = 100;
		break;
	case EP1_AG08A_SEND_RATE_HZ_0000_01:
		send_rate_milli_hz = 10;
		break;
	default:
		EMSG("send_rate is invalid, <send_rate:%d>", send_rate);
		return -1;
	}

	return send_rate_milli_hz;
}

/*!
 * @brief convert input voltage to value
 */
static int ep1_ag08a_msg_convert_input_voltage_to_value(int vmin, int vmax)
{
	int voltage;

	if (vmin == -10000 && vmax == 10000) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_10_PLUS_10;
	} else if (vmin == -5000 && vmax == 5000) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_5_PLUS_5;
	} else if (vmin == -2500 && vmax == 2500) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_2_5_PLUS_2_5;
	} else if (vmin == -1250 && vmax == 1250) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_1_25_PLUS_1_25;
	} else if (vmin == -800 && vmax == 800) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_0_8_PLUS_0_8;
	} else if (vmin == -400 && vmax == 400) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_0_4_PLUS_0_4;
	} else if (vmin == -200 && vmax == 200) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_0_2_PLUS_0_2;
	} else if (vmin == -100 && vmax == 100) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_MINUS_0_1_PLUS_0_1;
	} else if (vmin == 0 && vmax == 5000) {
		voltage = EP1_AG08A_INPUT_VOLTAGE_5;
	} else {
		EMSG("vmin or vmax is invalid, <vmin:%d> <vmax:%d>", vmin, vmax);
		return EP1_AG08A_INPUT_VOLTAGE_Unknown;
	}

	return voltage;
}

/*!
 * @brief convert input voltage from value
 */
static int ep1_ag08a_msg_convert_input_voltage_from_value(int voltage, int *vmin, int *vmax)
{
	if (vmin == NULL || vmax == NULL) {
		EMSG("vmin or vmax is NULL");
		return RESULT_Failure;
	}

	switch (voltage) {
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_10_PLUS_10:
		*vmin = -10000;
		*vmax = 10000;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_5_PLUS_5:
		*vmin = -5000;
		*vmax = 5000;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_2_5_PLUS_2_5:
		*vmin = -2500;
		*vmax = 2500;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_1_25_PLUS_1_25:
		*vmin = -1250;
		*vmax = 1250;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_0_8_PLUS_0_8:
		*vmin = -800;
		*vmax = 800;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_0_4_PLUS_0_4:
		*vmin = -400;
		*vmax = 400;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_0_2_PLUS_0_2:
		*vmin = -200;
		*vmax = 200;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_MINUS_0_1_PLUS_0_1:
		*vmin = -100;
		*vmax = 100;
		break;
	case EP1_AG08A_INPUT_VOLTAGE_5:
		*vmin = 0;
		*vmax = 5000;
		break;
	default:
		EMSG("voltage is invalid, <voltage:%d>", voltage);
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Status)
 */
int ep1_ag08a_msg_parse_response_get_status(u8 *data, int data_size, ep1_ag08a_msg_resp_get_status_t *status)
{
	int n = 0;
	int ch = 0;
	int send_rate;
	int send_rate_milli_hz;
	int result;
	int voltage;
	int vmin;
	int vmax;

	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (data_size != APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(EP1_AG08A_CMD_LENGTH_RESPONSE_GET_STATUS)) {
		EMSG("data_size is invalid...<size:%d>", data_size);
		return RESULT_Failure;
	}
	if (status == NULL) {
		EMSG("status is NULL");
		return RESULT_Failure;
	}

	send_rate = data[n];
	send_rate_milli_hz = ep1_ag08a_msg_convert_send_rate_from_value(send_rate);
	if (send_rate_milli_hz == -1) {
		EMSG("ep1_ag08a_msg_convert_send_rate_from_value().. Error, <send_rate:0x%02x>", send_rate);
		return RESULT_Failure;
	}
	status->in.cfg.send_rate = send_rate_milli_hz;
	n += 1;

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		voltage = data[n];
		result = ep1_ag08a_msg_convert_input_voltage_from_value(voltage, &vmin, &vmax);
		if (result != RESULT_Success) {
			EMSG("ep1_ag08a_msg_convert_input_voltage_from_value().. Error, <voltage:0x%02x>", voltage);
			return RESULT_Failure;
		}

		status->in.cfg.vmin[ch] = vmin;
		status->in.cfg.vmax[ch] = vmax;
		n += 1;
	}

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		if (data[n] & (1 << ch)) {
			status->in.ctrl.start[ch] = true;
		} else {
			status->in.ctrl.start[ch] = false;
		}
	}
	n += 1;

	status->out.cfg.waveform_type = data[n];
	n += 1;

	/* convert output voltage [mV] x20 */
	status->out.cfg.voltage = data[n] * 20;
	n += 1;

	/* convert output frequency [mHz] x1000 */
	status->out.cfg.frequency = data[n] * 1000;
	n += 1;

	if (data[n] & 0x01) {
		status->out.ctrl.start = true;
	} else {
		status->out.ctrl.start = false;
	}

	return RESULT_Success;
}

/*!
 * @brief pack (Set Analog Input)
 */
int ep1_ag08a_msg_pack_set_analog_input(ep1_ag08a_msg_set_analog_input_t *param, u8 *data, int data_size)
{
	int n = 0;
	int ch = 0;
	int send_rate;
	int voltage;

	if (param == NULL) {
		EMSG("param is NULL");
		return RESULT_Failure;
	}
	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (data_size != APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(EP1_AG08A_CMD_LENGTH_SET_ANALOG_INPUT)) {
		EMSG("data_size is invalid...<size:%d>", data_size);
		return RESULT_Failure;
	}

	send_rate = ep1_ag08a_msg_convert_send_rate_to_value(param->send_rate);
	if (send_rate == EP1_AG08A_SEND_RATE_HZ_Unknown) {
		EMSG("ep1_ag08a_msg_convert_send_rate_to_value().. Error, <send_rate:%d>", param->send_rate);
		return RESULT_Failure;
	}

	data[n] = send_rate;
	n += 1;

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		voltage = ep1_ag08a_msg_convert_input_voltage_to_value(param->vmin[ch], param->vmax[ch]);
		if (voltage == EP1_AG08A_INPUT_VOLTAGE_Unknown) {
			EMSG("ep1_ag08a_msg_convert_input_voltage_to_value().. Error, <vmin:%d> <vmax:%d>",
			     param->vmin[ch], param->vmax[ch]);
			return RESULT_Failure;
		}
		data[n] = voltage;
		n++;
	}

	return RESULT_Success;
}

/*!
 * @brief pack (Control Analog Input)
 */
int ep1_ag08a_msg_pack_control_analog_input(ep1_ag08a_msg_control_analog_input_t *param, u8 *data, int data_size)
{
	int ch = 0;

	if (param == NULL) {
		EMSG("param is NULL");
		return RESULT_Failure;
	}
	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (data_size != APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(EP1_AG08A_CMD_LENGTH_CONTROL_ANALOG_INPUT)) {
		EMSG("data_size is invalid...<size:%d>", data_size);
		return RESULT_Failure;
	}

	data[0] = 0;

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		if (param->start[ch]) {
			data[0] |= 1 << ch;
		}
	}

	return RESULT_Success;
}

/*!
 * @brief pack (Set Analog Output)
 */
int ep1_ag08a_msg_pack_set_analog_output(ep1_ag08a_msg_set_analog_output_t *param, u8 *data, int data_size)
{
	int n = 0;

	if (param == NULL) {
		EMSG("param is NULL");
		return RESULT_Failure;
	}
	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (data_size != APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(EP1_AG08A_CMD_LENGTH_SET_ANALOG_OUTPUT)) {
		EMSG("data_size is invalid...<size:%d>", data_size);
		return RESULT_Failure;
	}

	data[n] = param->waveform_type;
	n += 1;

	/* convert output voltage [mV] /20 */
	data[n] = param->voltage / 20;
	n += 1;

	/* convert output frequency [mHz] /1000 */
	data[n] = param->frequency / 1000;

	return RESULT_Success;
}

/*!
 * @brief pack (Control Analog Output)
 */
int ep1_ag08a_msg_pack_control_analog_output(ep1_ag08a_msg_control_analog_output_t *param, u8 *data, int data_size)
{
	if (param == NULL) {
		EMSG("param is NULL");
		return RESULT_Failure;
	}
	if (data == NULL) {
		EMSG("data is NULL");
		return RESULT_Failure;
	}
	if (data_size != APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(EP1_AG08A_CMD_LENGTH_CONTROL_ANALOG_OUTPUT)) {
		EMSG("data_size is invalid...<size:%d>", data_size);
		return RESULT_Failure;
	}

	if (param->start) {
		data[0] = 0x01;
	} else {
		data[0] = 0x00;
	}

	return RESULT_Success;
}
