// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include "../apt_usbtrx_def.h"
#include "ep1_cf02a_msg.h"

/*!
 * @brief parse (Response Get Silent Mode)
 */
int ep1_cf02a_msg_parse_response_get_silent_mode(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_silent_mode_t *mode = (ep1_cf02a_msg_get_silent_mode_t *)msg;

	mode->silent = (data[0] & 0x01) == 0 ? true : false;

	return RESULT_Success;
}

/*!
 * @brief pack (Set Silent Mode)
 */
int ep1_cf02a_msg_pack_set_silent_mode(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_silent_mode_t *mode = (ep1_cf02a_msg_set_silent_mode_t *)msg;

	data[0] = (mode->silent == true) ? 0x00 : 0x01;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get FD Mode)
 */
int ep1_cf02a_msg_parse_response_get_fd_mode(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_fd_mode_t *mode = (ep1_cf02a_msg_get_fd_mode_t *)msg;

	mode->fd = (data[0] & 0x01) == 0 ? false : true;

	return RESULT_Success;
}

/*!
 * @brief pack (Set FD Mode)
 */
int ep1_cf02a_msg_pack_set_fd_mode(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_fd_mode_t *mode = (ep1_cf02a_msg_set_fd_mode_t *)msg;

	data[0] = (mode->fd == true) ? 0x01 : 0x00;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get ISO Mode)
 */
int ep1_cf02a_msg_parse_response_get_iso_mode(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_iso_mode_t *mode = (ep1_cf02a_msg_get_iso_mode_t *)msg;

	mode->non_iso_mode = (data[0] & 0x01) == 0 ? false : true;

	return RESULT_Success;
}

/*!
 * @brief pack (Set ISO Mode)
 */
int ep1_cf02a_msg_pack_set_iso_mode(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_iso_mode_t *mode = (ep1_cf02a_msg_set_iso_mode_t *)msg;

	data[0] = (mode->non_iso_mode == true) ? 0x01 : 0x00;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Bit Timing)
 */
int ep1_cf02a_msg_parse_response_get_bit_timing(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_bit_timing_t *timing = (ep1_cf02a_msg_get_bit_timing_t *)msg;
	int n = 0;

	timing->prop_seg = data[n];
	timing->prop_seg |= data[n + 1] << 8;
	n = n + 2;

	timing->phase_seg1 = data[n];
	timing->phase_seg1 |= data[n + 1] << 8;
	n = n + 2;

	timing->phase_seg2 = data[n];
	timing->phase_seg2 |= data[n + 1] << 8;
	n = n + 2;

	timing->sjw = data[n];
	timing->sjw |= data[n + 1] << 8;
	n = n + 2;

	timing->brp = data[n];
	timing->brp |= data[n + 1] << 8;
	n = n + 2;

	return RESULT_Success;
}

/*!
 * @brief pack (Set Bit Timing)
 */
int ep1_cf02a_msg_pack_set_bit_timing(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_bit_timing_t *timing = (ep1_cf02a_msg_set_bit_timing_t *)msg;
	int n = 0;

	data[n] = timing->prop_seg;
	data[n + 1] = timing->prop_seg >> 8;
	n = n + 2;

	data[n] = timing->phase_seg1;
	data[n + 1] = timing->phase_seg1 >> 8;
	n = n + 2;

	data[n] = timing->phase_seg2;
	data[n + 1] = timing->phase_seg2 >> 8;
	n = n + 2;

	data[n] = timing->sjw;
	data[n + 1] = timing->sjw >> 8;
	n = n + 2;

	data[n] = timing->brp;
	data[n + 1] = timing->brp >> 8;
	n = n + 2;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Data Bit Timing)
 */
int ep1_cf02a_msg_parse_response_get_data_bit_timing(u8 *data, int data_size, void *msg)
{
	return ep1_cf02a_msg_parse_response_get_bit_timing(data, data_size, msg);
}

/*!
 * @brief pack (Set Data Bit Timing)
 */
int ep1_cf02a_msg_pack_set_data_bit_timing(void *msg, u8 *data, int data_size)
{
	return ep1_cf02a_msg_pack_set_bit_timing(msg, data, data_size);
}

/*!
 * @brief parse (Response Get Tx Rx Control)
 */
int ep1_cf02a_msg_parse_response_get_tx_rx_control(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_tx_rx_control_t *control = (ep1_cf02a_msg_get_tx_rx_control_t *)msg;

	control->start = data[0] == 0x01 ? true : false;

	return RESULT_Success;
}

/*!
 * @brief pack (Set Tx Rx Control)
 */
int ep1_cf02a_msg_pack_set_tx_rx_control(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_tx_rx_control_t *control = (ep1_cf02a_msg_set_tx_rx_control_t *)msg;

	data[0] = control->start == true ? 0x01 : 0x00;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get CAN Clock)
 */
int ep1_cf02a_msg_parse_response_get_can_clock(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_can_clock_t *can_clock = (ep1_cf02a_msg_get_can_clock_t *)msg;

	can_clock->can_clock = data[0];
	can_clock->can_clock |= data[1] << 8;
	can_clock->can_clock |= data[2] << 16;
	can_clock->can_clock |= data[3] << 24;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Device Timestamp Reset Time)
 */
int ep1_cf02a_msg_parse_response_get_device_timestamp_reset_time(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_device_timestamp_reset_time_t *reset_time = (ep1_cf02a_msg_get_device_timestamp_reset_time_t *)msg;
	int n = 0;

	reset_time->ts.ts_sec = data[n];
	reset_time->ts.ts_sec |= data[n + 1] << 8;
	reset_time->ts.ts_sec |= data[n + 2] << 16;
	reset_time->ts.ts_sec |= data[n + 3] << 24;
	n = n + 4;

	reset_time->ts.ts_usec = data[n];
	reset_time->ts.ts_usec |= data[n + 1] << 8;
	reset_time->ts.ts_usec |= data[n + 2] << 16;
	reset_time->ts.ts_usec |= data[n + 3] << 24;
	n = n + 4;

	return RESULT_Success;
}

/*!
 * @brief pack (Set Host Timestamp Reset Time)
 */
int ep1_cf02a_msg_pack_set_host_timestamp_reset_time(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_host_timestamp_reset_time_t *reset_time = (ep1_cf02a_msg_set_host_timestamp_reset_time_t *)msg;
	int n = 0;

	data[n] = reset_time->ts.tv_sec;
	data[n + 1] = reset_time->ts.tv_sec >> 8;
	data[n + 2] = reset_time->ts.tv_sec >> 16;
	data[n + 3] = reset_time->ts.tv_sec >> 24;
	n = n + 4;

	data[n] = reset_time->ts.tv_nsec;
	data[n + 1] = reset_time->ts.tv_nsec >> 8;
	data[n + 2] = reset_time->ts.tv_nsec >> 16;
	data[n + 3] = reset_time->ts.tv_nsec >> 24;
	n = n + 4;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get RTC Time)
 */
int ep1_cf02a_msg_parse_response_get_rtc_time(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_rtc_time_t *rtc_time = (ep1_cf02a_msg_get_rtc_time_t *)msg;
	int n = 0;

	rtc_time->weekday = data[n];
	n = n + 1;
	rtc_time->month = data[n];
	n = n + 1;
	rtc_time->date = data[n];
	n = n + 1;
	rtc_time->year = data[n];
	n = n + 1;
	rtc_time->hour = data[n];
	n = n + 1;
	rtc_time->minute = data[n];
	n = n + 1;
	rtc_time->second = data[n];
	n = n + 1;

	rtc_time->microsecond = data[n];
	rtc_time->microsecond |= data[n + 1] << 8;
	rtc_time->microsecond |= data[n + 2] << 16;
	rtc_time->microsecond |= data[n + 3] << 24;
	n = n + 4;

	return RESULT_Success;
}

/*!
 * @brief pack (Set RTC Time)
 */
int ep1_cf02a_msg_pack_set_rtc_time(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_rtc_time_t *rtc_time = (ep1_cf02a_msg_set_rtc_time_t *)msg;
	int n = 0;

	data[n] = rtc_time->weekday;
	n = n + 1;
	data[n] = rtc_time->month;
	n = n + 1;
	data[n] = rtc_time->date;
	n = n + 1;
	data[n] = rtc_time->year;
	n = n + 1;
	data[n] = rtc_time->hour;
	n = n + 1;
	data[n] = rtc_time->minute;
	n = n + 1;
	data[n] = rtc_time->second;
	n = n + 1;

	data[n] = rtc_time->microsecond;
	data[n + 1] = rtc_time->microsecond >> 8;
	data[n + 2] = rtc_time->microsecond >> 16;
	data[n + 3] = rtc_time->microsecond >> 24;
	n = n + 4;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Current Store Data State)
 */
int ep1_cf02a_msg_parse_response_get_current_store_data_state(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_current_store_data_state_t *state = (ep1_cf02a_msg_get_current_store_data_state_t *)msg;

	state->state = data[0];
	memcpy(state->id, &data[1], EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Store Data ID List Count)
 */
int ep1_cf02a_msg_parse_response_get_store_data_id_list_count(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_store_data_id_list_count_t *count = (ep1_cf02a_msg_get_store_data_id_list_count_t *)msg;

	count->count = data[0];
	count->count |= data[1] << 8;
	count->count |= data[2] << 16;
	count->count |= data[3] << 24;

	return RESULT_Success;
}

/*!
 * @brief pack (Get Store Data ID)
 */
int ep1_cf02a_msg_pack_get_store_data_id(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_get_store_data_id_request_t *id_req = (ep1_cf02a_msg_get_store_data_id_request_t *)msg;

	data[0] = id_req->index;
	data[1] = id_req->index >> 8;
	data[2] = id_req->index >> 16;
	data[3] = id_req->index >> 24;

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Store Data ID)
 */
int ep1_cf02a_msg_parse_response_get_store_data_id(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_store_data_id_response_t *id_res = (ep1_cf02a_msg_get_store_data_id_response_t *)msg;

	memcpy(id_res->id, data, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);

	return RESULT_Success;
}

/*!
 * @brief pack (Get Store Data Meta)
 */
int ep1_cf02a_msg_pack_get_store_data_meta(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_get_store_data_meta_request_t *meta_req = (ep1_cf02a_msg_get_store_data_meta_request_t *)msg;

	memcpy(data, meta_req->id, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Store Data Meta)
 */
int ep1_cf02a_msg_parse_response_get_store_data_meta(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_store_data_meta_response_t *meta_res = (ep1_cf02a_msg_get_store_data_meta_response_t *)msg;
	int n = 0;
	int size = 0;

	size = sizeof(meta_res->start_time.rtc);
	ep1_cf02a_msg_parse_response_get_rtc_time(&data[n], size, &meta_res->start_time.rtc);
	n = n + size;

	meta_res->start_time.ts.ts_sec = data[n];
	meta_res->start_time.ts.ts_sec |= data[n + 1] << 8;
	meta_res->start_time.ts.ts_sec |= data[n + 2] << 16;
	meta_res->start_time.ts.ts_sec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->start_time.ts.ts_usec = data[n];
	meta_res->start_time.ts.ts_usec |= data[n + 1] << 8;
	meta_res->start_time.ts.ts_usec |= data[n + 2] << 16;
	meta_res->start_time.ts.ts_usec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->reset_time.device_ts.ts_sec = data[n];
	meta_res->reset_time.device_ts.ts_sec |= data[n + 1] << 8;
	meta_res->reset_time.device_ts.ts_sec |= data[n + 2] << 16;
	meta_res->reset_time.device_ts.ts_sec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->reset_time.device_ts.ts_usec = data[n];
	meta_res->reset_time.device_ts.ts_usec |= data[n + 1] << 8;
	meta_res->reset_time.device_ts.ts_usec |= data[n + 2] << 16;
	meta_res->reset_time.device_ts.ts_usec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->reset_time.host_monotonic.tv_sec = data[n];
	meta_res->reset_time.host_monotonic.tv_sec |= data[n + 1] << 8;
	meta_res->reset_time.host_monotonic.tv_sec |= data[n + 2] << 16;
	meta_res->reset_time.host_monotonic.tv_sec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->reset_time.host_monotonic.tv_nsec = data[n];
	meta_res->reset_time.host_monotonic.tv_nsec |= data[n + 1] << 8;
	meta_res->reset_time.host_monotonic.tv_nsec |= data[n + 2] << 16;
	meta_res->reset_time.host_monotonic.tv_nsec |= data[n + 3] << 24;
	n = n + 4;

	meta_res->can_frame_count = ((u64)data[n]);
	meta_res->can_frame_count |= ((u64)data[n + 1]) << 8;
	meta_res->can_frame_count |= ((u64)data[n + 2]) << 16;
	meta_res->can_frame_count |= ((u64)data[n + 3]) << 24;
	meta_res->can_frame_count |= ((u64)data[n + 4]) << 32;
	meta_res->can_frame_count |= ((u64)data[n + 5]) << 40;
	meta_res->can_frame_count |= ((u64)data[n + 6]) << 48;
	meta_res->can_frame_count |= ((u64)data[n + 7]) << 56;
	n = n + 8;

	return RESULT_Success;
}

/*!
 * @brief pack (Get Store Data Rx Control)
 */
int ep1_cf02a_msg_pack_get_store_data_rx_control(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_get_store_data_rx_control_request_t *control_req = (ep1_cf02a_msg_get_store_data_rx_control_request_t *)msg;

	memcpy(data, control_req->id, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Store Data Rx Control)
 */
int ep1_cf02a_msg_parse_response_get_store_data_rx_control(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_store_data_rx_control_response_t *control_res = (ep1_cf02a_msg_get_store_data_rx_control_response_t *)msg;
	int n = 0;

	control_res->start = data[n] == 0x01 ? true : false;
	n = n + 1;

	control_res->interval = data[n];
	control_res->interval |= data[n + 1] << 8;
	n = n + 2;

	return RESULT_Success;
}

/*!
 * @brief pack (Set Store Data Rx Control)
 */
int ep1_cf02a_msg_pack_set_store_data_rx_control(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_set_store_data_rx_control_t *control = (ep1_cf02a_msg_set_store_data_rx_control_t *)msg;
	int n = 0;

	memcpy(data, control->id, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);
	n = n + EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH;

	data[n] = control->start == true ? 0x01 : 0x00;
	n = n + 1;

	data[n] = control->interval;
	data[n + 1] = control->interval >> 8;
	n = n + 2;

	return RESULT_Success;
}

/*!
 * @brief pack (Delete Store Data)
 */
int ep1_cf02a_msg_pack_delete_store_data(void *msg, u8 *data, int data_size)
{
	ep1_cf02a_msg_delete_store_data_t *delete_data = (ep1_cf02a_msg_delete_store_data_t *)msg;

	memcpy(data, delete_data->id, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);

	return RESULT_Success;
}

/*!
 * @brief parse (Response Get Capabilities)
 */
int ep1_cf02a_msg_parse_response_get_capabilities(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_msg_get_capabilities_t *capabilities = (ep1_cf02a_msg_get_capabilities_t *)msg;

	memcpy(capabilities, data, sizeof(ep1_cf02a_msg_get_capabilities_t));

	return RESULT_Success;
}

static u8 can_dlc2len(u8 dlc) {
    static const u8 dlc_to_len[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8,
        12, 16, 20, 24, 32, 48, 64
    };
    if (dlc >= 0 && dlc <= 15) {
        return dlc_to_len[dlc];
    } else {
		EMSG("can_dlc2len().. Error, <dlc:%d>", dlc);
        return 0;
    }
}

/*!
 * @brief parse (Nofity Receive CAN Summary)
 */
int ep1_cf02a_msg_parse_notify_recv_can_summary(u8 *data, int data_size, void *msg)
{
	ep1_cf02a_payload_notify_recv_can_summary_t *summary = (ep1_cf02a_payload_notify_recv_can_summary_t *)msg;
	int n = 0;

	summary->rx_count = data[n];
	summary->rx_count |= data[n + 1] << 8;
	summary->rx_count |= data[n + 2] << 16;
	summary->rx_count |= data[n + 3] << 24;
	n = n + 4;

	summary->frame.can_id = data[n];
	summary->frame.can_id |= data[n + 1] << 8;
	summary->frame.can_id |= data[n + 2] << 16;
	summary->frame.can_id |= data[n + 3] << 24;
	n = n + 4;

	summary->frame.len = can_dlc2len(data[n]);
	n = n + 1;

	memcpy(&summary->frame.data, &data[n], 64);
	n = n + 64;

	return RESULT_Success;
}
