/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_MSG_H__
#define __EP1_CF02A_MSG_H__

#include "ep1_cf02a_cmd_def.h"

/*!
 * @brief parse (Response Get Silent Mode)
 */
int ep1_cf02a_msg_parse_response_get_silent_mode(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Silent Mode)
 */
int ep1_cf02a_msg_pack_set_silent_mode(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get FD Mode)
 */
int ep1_cf02a_msg_parse_response_get_fd_mode(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set FD Mode)
 */
int ep1_cf02a_msg_pack_set_fd_mode(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get ISO Mode)
 */
int ep1_cf02a_msg_parse_response_get_iso_mode(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set ISO Mode)
 */
int ep1_cf02a_msg_pack_set_iso_mode(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Bit Timing)
 */
int ep1_cf02a_msg_parse_response_get_bit_timing(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Bit Timing)
 */
int ep1_cf02a_msg_pack_set_bit_timing(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Data Bit Timing)
 */
int ep1_cf02a_msg_parse_response_get_data_bit_timing(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Data Bit Timing)
 */
int ep1_cf02a_msg_pack_set_data_bit_timing(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Tx Rx Control)
 */
int ep1_cf02a_msg_parse_response_get_tx_rx_control(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Tx Rx Control)
 */
int ep1_cf02a_msg_pack_set_tx_rx_control(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get CAN Clock)
 */
int ep1_cf02a_msg_parse_response_get_can_clock(u8 *data, int data_size, void *msg);

/*!
 * @brief parse (Response Get Device Timestamp Reset Time)
 */
int ep1_cf02a_msg_parse_response_get_device_timestamp_reset_time(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Host Timestamp Reset Time)
 */
int ep1_cf02a_msg_pack_set_host_timestamp_reset_time(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get RTC Time)
 */
int ep1_cf02a_msg_parse_response_get_rtc_time(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set RTC Time)
 */
int ep1_cf02a_msg_pack_set_rtc_time(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Current Store Data State)
 */
int ep1_cf02a_msg_parse_response_get_current_store_data_state(u8 *data, int data_size, void *msg);

/*!
 * @brief parse (Response Get Store Data ID List Count)
 */
int ep1_cf02a_msg_parse_response_get_store_data_id_list_count(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Get Store Data ID)
 */
int ep1_cf02a_msg_pack_get_store_data_id(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Store Data ID)
 */
int ep1_cf02a_msg_parse_response_get_store_data_id(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Get Store Data Meta)
 */
int ep1_cf02a_msg_pack_get_store_data_meta(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Store Data Meta)
 */
int ep1_cf02a_msg_parse_response_get_store_data_meta(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Get Store Data Rx Control)
 */
int ep1_cf02a_msg_pack_get_store_data_rx_control(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Store Data Rx Control)
 */
int ep1_cf02a_msg_parse_response_get_store_data_rx_control(u8 *data, int data_size, void *msg);

/*!
 * @brief pack (Set Store Data Rx Control)
 */
int ep1_cf02a_msg_pack_set_store_data_rx_control(void *msg, u8 *data, int data_size);

/*!
 * @brief pack (Delete Store Data)
 */
int ep1_cf02a_msg_pack_delete_store_data(void *msg, u8 *data, int data_size);

/*!
 * @brief parse (Response Get Capabilities)
 */
int ep1_cf02a_msg_parse_response_get_capabilities(u8 *data, int data_size, void *msg);

/*!
 * @brief parse (Nofity Receive CAN Summary)
 */
int ep1_cf02a_msg_parse_notify_recv_can_summary(u8 *data, int data_size, void *msg);

#endif /* __EP1_CF02A_MSG_H__ */
