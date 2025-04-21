// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/slab.h>

#include "../apt_usbtrx_core.h" /* send_msg(), wait_msg() */
#include "ep1_cf02a_cmd.h"
#include "ep1_cf02a_msg.h"

/*!
 * @brief cmd common
 */
static int ep1_cf02a_cmd_common(apt_usbtrx_dev_t *dev, void *req_message, void *resp_message, int req_data_size,
				int resp_data_size, int req_cmd_id, int resp_cmd_id_ack, int resp_cmd_id_nack,
				int (*pack_req)(void *req_message, u8 *data, int data_size),
				int (*parse_resp)(u8 *data, int data_size, void *resp_message), bool *success,
				unsigned int timeout_msec)
{
	int result;
	char *req_data;
	char *resp_data;
	apt_usbtrx_msg_t req_msg;
	apt_usbtrx_msg_t resp_msg;
	int resp_id;

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
	req_msg.id = req_cmd_id;
	req_msg.payload_size = APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(req_data_size);
	if (pack_req != NULL) {
		result = pack_req(req_message, req_msg.payload, req_msg.payload_size);
		if (result != RESULT_Success) {
			EMSG("pack_req().. Error");
			kfree(req_data);
			kfree(resp_data);
			return RESULT_Failure;
		}
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
	result = apt_usbtrx_wait_msg_timeout(dev, resp_cmd_id_ack, resp_cmd_id_nack, resp_data, resp_data_size,
					     timeout_msec);
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

	if (parse_resp != NULL) {
		result = parse_resp(resp_msg.payload, resp_msg.payload_size, resp_message);
		if (result != RESULT_Success) {
			EMSG("parse_resp().. Error");
			kfree(req_data);
			kfree(resp_data);
			return RESULT_Failure;
		}
	} else {
		result = apt_usbtrx_msg_parse_ack(resp_msg.payload, resp_msg.payload_size, &resp_id);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_msg_parse_ack().. Error");
			kfree(req_data);
			kfree(resp_data);
			return RESULT_Failure;
		}
		if (resp_id != req_cmd_id) {
			EMSG("%s(): ack/nack is not match, <id:0x%02x>", __func__, resp_id);
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
	}

	kfree(req_data);
	kfree(resp_data);

	CHKMSG("LEAVE");
	return RESULT_Success;
}

/*!
 * @brief get common
 */
static int ep1_cf02a_get_common(apt_usbtrx_dev_t *dev, void *resp_message, int req_data_size, int resp_data_size,
				int req_cmd_id, int resp_cmd_id_ack,
				int (*parse_resp)(u8 *data, int data_size, void *resp_message),
				unsigned int timeout_msec)
{
	return ep1_cf02a_cmd_common(dev, NULL, resp_message, req_data_size, resp_data_size, req_cmd_id, resp_cmd_id_ack,
				    APT_USBTRX_CMD_Unknown, NULL, parse_resp, NULL, timeout_msec);
}

/*!
 * @brief set common
 */
static int ep1_cf02a_set_common(apt_usbtrx_dev_t *dev, void *req_message, int req_data_size, int req_cmd_id,
				int (*pack_req)(void *req_message, u8 *data, int data_size), bool *success,
				unsigned int timeout_msec)
{
	return ep1_cf02a_cmd_common(dev, req_message, NULL, req_data_size, APT_USBTRX_CMD_LENGTH_ACK, req_cmd_id,
				    APT_USBTRX_CMD_ACK, APT_USBTRX_CMD_NACK, pack_req, NULL, success, timeout_msec);
}

/*!
 * @brief get silent mode
 */
int ep1_cf02a_get_silent_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_silent_mode_t *mode)
{
	return ep1_cf02a_get_common(dev, mode, EP1_CF02A_CMD_LENGTH_GET_SILENT_MODE,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_SILENT_MODE, EP1_CF02A_CMD_GetSilentMode,
				    EP1_CF02A_CMD_ResponseGetSilentMode, ep1_cf02a_msg_parse_response_get_silent_mode,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set silent mode
 */
int ep1_cf02a_set_silent_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_silent_mode_t *mode, bool *success)
{
	return ep1_cf02a_set_common(dev, mode, EP1_CF02A_CMD_LENGTH_SET_SILENT_MODE, EP1_CF02A_CMD_SetSilentMode,
				    ep1_cf02a_msg_pack_set_silent_mode, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get fd mode
 */
int ep1_cf02a_get_fd_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_fd_mode_t *mode)
{
	return ep1_cf02a_get_common(dev, mode, EP1_CF02A_CMD_LENGTH_GET_FD_MODE,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_FD_MODE, EP1_CF02A_CMD_GetFDMode,
				    EP1_CF02A_CMD_ResponseGetFDMode, ep1_cf02a_msg_parse_response_get_fd_mode,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set fd mode
 */
int ep1_cf02a_set_fd_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_fd_mode_t *mode, bool *success)
{
	return ep1_cf02a_set_common(dev, mode, EP1_CF02A_CMD_LENGTH_SET_FD_MODE, EP1_CF02A_CMD_SetFDMode,
				    ep1_cf02a_msg_pack_set_fd_mode, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get iso mode
 */
int ep1_cf02a_get_iso_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_iso_mode_t *mode)
{
	return ep1_cf02a_get_common(dev, mode, EP1_CF02A_CMD_LENGTH_GET_ISO_MODE,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_ISO_MODE, EP1_CF02A_CMD_GetISOMode,
				    EP1_CF02A_CMD_ResponseGetISOMode, ep1_cf02a_msg_parse_response_get_iso_mode,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set iso mode
 */
int ep1_cf02a_set_iso_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_iso_mode_t *mode, bool *success)
{
	return ep1_cf02a_set_common(dev, mode, EP1_CF02A_CMD_LENGTH_SET_ISO_MODE, EP1_CF02A_CMD_SetISOMode,
				    ep1_cf02a_msg_pack_set_iso_mode, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get bit timing
 */
int ep1_cf02a_get_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_bit_timing_t *timing)
{
	return ep1_cf02a_get_common(dev, timing, EP1_CF02A_CMD_LENGTH_GET_BIT_TIMING,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_BIT_TIMING, EP1_CF02A_CMD_GetBitTiming,
				    EP1_CF02A_CMD_ResponseGetBitTiming, ep1_cf02a_msg_parse_response_get_bit_timing,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set bit timing
 */
int ep1_cf02a_set_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_bit_timing_t *timing, bool *success)
{
	return ep1_cf02a_set_common(dev, timing, EP1_CF02A_CMD_LENGTH_SET_BIT_TIMING, EP1_CF02A_CMD_SetBitTiming,
				    ep1_cf02a_msg_pack_set_bit_timing, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get data bit timing
 */
int ep1_cf02a_get_data_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_data_bit_timing_t *timing)
{
	return ep1_cf02a_get_common(dev, timing, EP1_CF02A_CMD_LENGTH_GET_DATA_BIT_TIMING,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_DATA_BIT_TIMING, EP1_CF02A_CMD_GetDataBitTiming,
				    EP1_CF02A_CMD_ResponseGetDataBitTiming,
				    ep1_cf02a_msg_parse_response_get_data_bit_timing, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set data bit timing
 */
int ep1_cf02a_set_data_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_data_bit_timing_t *timing, bool *success)
{
	return ep1_cf02a_set_common(dev, timing, EP1_CF02A_CMD_LENGTH_SET_DATA_BIT_TIMING,
				    EP1_CF02A_CMD_SetDataBitTiming, ep1_cf02a_msg_pack_set_data_bit_timing, success,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get tx rx control
 */
int ep1_cf02a_get_tx_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_tx_rx_control_t *control)
{
	return ep1_cf02a_get_common(dev, control, EP1_CF02A_CMD_LENGTH_GET_TX_RX_CONTROL,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_TX_RX_CONTROL, EP1_CF02A_CMD_GetTxRxControl,
				    EP1_CF02A_CMD_ResponseGetTxRxControl,
				    ep1_cf02a_msg_parse_response_get_tx_rx_control, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set tx rx control
 */
int ep1_cf02a_set_tx_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_tx_rx_control_t *control, bool *success)
{
	return ep1_cf02a_set_common(dev, control, EP1_CF02A_CMD_LENGTH_SET_TX_RX_CONTROL, EP1_CF02A_CMD_SetTxRxControl,
				    ep1_cf02a_msg_pack_set_tx_rx_control, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get can clock
 */
int ep1_cf02a_get_can_clock(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_can_clock_t *can_clock)
{
	return ep1_cf02a_get_common(dev, can_clock, EP1_CF02A_CMD_LENGTH_GET_CAN_CLOCK,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CAN_CLOCK, EP1_CF02A_CMD_GetCANClock,
				    EP1_CF02A_CMD_ResponseGetCANClock, ep1_cf02a_msg_parse_response_get_can_clock,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get device timestamp reset time
 */
int ep1_cf02a_get_device_timestamp_reset_time(apt_usbtrx_dev_t *dev,
					      ep1_cf02a_msg_get_device_timestamp_reset_time_t *time)
{
	return ep1_cf02a_get_common(dev, time, EP1_CF02A_CMD_LENGTH_GET_DEVICE_TIMESTAMP_RESET_TIME,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_DEVICE_TIMESTAMP_RESET_TIME,
				    EP1_CF02A_CMD_GetDeviceTimestampResetTime,
				    EP1_CF02A_CMD_ResponseGetDeviceTimestampResetTime,
				    ep1_cf02a_msg_parse_response_get_device_timestamp_reset_time,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set host timestamp reset time
 */
int ep1_cf02a_set_host_timestamp_reset_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_host_timestamp_reset_time_t *time,
					    bool *success)
{
	return ep1_cf02a_set_common(dev, time, EP1_CF02A_CMD_LENGTH_SET_HOST_TIMESTAMP_RESET_TIME,
				    EP1_CF02A_CMD_SetHostTimeStampResetTime,
				    ep1_cf02a_msg_pack_set_host_timestamp_reset_time, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get rtc time
 */
int ep1_cf02a_get_rtc_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_rtc_time_t *time)
{
	return ep1_cf02a_get_common(dev, time, EP1_CF02A_CMD_LENGTH_GET_RTC_TIME,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_RTC_TIME, EP1_CF02A_CMD_GetRTCTime,
				    EP1_CF02A_CMD_ResponseGetRTCTime, ep1_cf02a_msg_parse_response_get_rtc_time,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set rtc time
 */
int ep1_cf02a_set_rtc_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_rtc_time_t *time, bool *success)
{
	return ep1_cf02a_set_common(dev, time, EP1_CF02A_CMD_LENGTH_SET_RTC_TIME, EP1_CF02A_CMD_SetRTCTime,
				    ep1_cf02a_msg_pack_set_rtc_time, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get current store data state
 */
int ep1_cf02a_get_current_store_data_state(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_current_store_data_state_t *state)
{
	return ep1_cf02a_get_common(dev, state, EP1_CF02A_CMD_LENGTH_GET_CURRENT_STORE_DATA_STATE,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CURRENT_STORE_DATA_STATE,
				    EP1_CF02A_CMD_GetCurrentStoreDataState,
				    EP1_CF02A_CMD_ResponseGetCurrentStoreDataState,
				    ep1_cf02a_msg_parse_response_get_current_store_data_state, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get store data id list count
 */
int ep1_cf02a_get_store_data_id_list_count(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_id_list_count_t *count)
{
	return ep1_cf02a_get_common(dev, count, EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_ID_LIST_COUNT,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_ID_LIST_COUNT,
				    EP1_CF02A_CMD_GetStoreDataIDListCount,
				    EP1_CF02A_CMD_ResponseGetStoreDataIDListCount,
				    ep1_cf02a_msg_parse_response_get_store_data_id_list_count, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get store data id
 */
int ep1_cf02a_get_store_data_id(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_id_request_t *id_req,
				ep1_cf02a_msg_get_store_data_id_response_t *id_res)
{
	return ep1_cf02a_cmd_common(dev, id_req, id_res, EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_ID,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_ID, EP1_CF02A_CMD_GetStoreDataID,
				    EP1_CF02A_CMD_ResponseGetStoreDataID, APT_USBTRX_CMD_Unknown,
				    ep1_cf02a_msg_pack_get_store_data_id,
				    ep1_cf02a_msg_parse_response_get_store_data_id, NULL, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get store data meta
 */
int ep1_cf02a_get_store_data_meta(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_meta_request_t *meta_req,
				  ep1_cf02a_msg_get_store_data_meta_response_t *meta_res)
{
	return ep1_cf02a_cmd_common(dev, meta_req, meta_res, EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_META,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_META, EP1_CF02A_CMD_GetStoreDataMeta,
				    EP1_CF02A_CMD_ResponseGetStoreDataMeta, APT_USBTRX_CMD_Unknown,
				    ep1_cf02a_msg_pack_get_store_data_meta,
				    ep1_cf02a_msg_parse_response_get_store_data_meta, NULL, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief get store data rx control
 */
int ep1_cf02a_get_store_data_rx_control(apt_usbtrx_dev_t *dev,
					ep1_cf02a_msg_get_store_data_rx_control_request_t *control_req,
					ep1_cf02a_msg_get_store_data_rx_control_response_t *control_res)
{
	return ep1_cf02a_cmd_common(dev, control_req, control_res, EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_RX_CONTROL,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_RX_CONTROL,
				    EP1_CF02A_CMD_GetStoreDataRxControl, EP1_CF02A_CMD_ResponseGetStoreDataRxControl,
				    APT_USBTRX_CMD_Unknown, ep1_cf02a_msg_pack_get_store_data_rx_control,
				    ep1_cf02a_msg_parse_response_get_store_data_rx_control, NULL,
				    APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief set store data rx control
 */
int ep1_cf02a_set_store_data_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_store_data_rx_control_t *control,
					bool *success)
{
	return ep1_cf02a_set_common(dev, control, EP1_CF02A_CMD_LENGTH_SET_STORE_DATA_RX_CONTROL,
				    EP1_CF02A_CMD_SetStoreDataRxControl, ep1_cf02a_msg_pack_set_store_data_rx_control,
				    success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief delete store data
 */
int ep1_cf02a_delete_store_data(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_delete_store_data_t *data, bool *success)
{
	return ep1_cf02a_set_common(dev, data, EP1_CF02A_CMD_LENGTH_DELETE_STORE_DATA, EP1_CF02A_CMD_DeleteStoreData,
				    ep1_cf02a_msg_pack_delete_store_data, success, APT_USBTRX_RECV_TIMEOUT);
}

/*!
 * @brief init store data media
 */
int ep1_cf02a_init_store_data_media(apt_usbtrx_dev_t *dev, bool *success)
{
	/* Extend timeout because initialization takes time */
	unsigned int timeout_msec = APT_USBTRX_RECV_TIMEOUT * 10;

	return ep1_cf02a_set_common(dev, NULL, EP1_CF02A_CMD_LENGTH_INIT_STORE_DATA_MEDIA,
				    EP1_CF02A_CMD_InitStoreDataMedia, NULL, success, timeout_msec);
}

/*!
 * @brief get capabilities
 */
int ep1_cf02a_get_capabilities(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_capabilities_t *capabilities)
{
	return ep1_cf02a_get_common(dev, capabilities, EP1_CF02A_CMD_LENGTH_GET_CAPABILITIES,
				    EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CAPABILITIES, EP1_CF02A_CMD_GetCapabilities,
				    EP1_CF02A_CMD_ResponseGetCapabilities,
				    ep1_cf02a_msg_parse_response_get_capabilities, APT_USBTRX_RECV_TIMEOUT);
}
