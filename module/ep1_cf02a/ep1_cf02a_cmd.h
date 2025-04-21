/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_CMD_H__
#define __EP1_CF02A_CMD_H__

#include "ep1_cf02a_cmd_def.h"

/*!
 * @brief get silent mode
 */
int ep1_cf02a_get_silent_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_silent_mode_t *mode);

/*!
 * @brief set silent mode
 */
int ep1_cf02a_set_silent_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_silent_mode_t *mode, bool *success);


/*!
 * @brief get fd mode
 */
int ep1_cf02a_get_fd_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_fd_mode_t *mode);

/*!
 * @brief set fd mode
 */
int ep1_cf02a_set_fd_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_fd_mode_t *mode, bool *success);

/*!
 * @brief get iso mode
 */
int ep1_cf02a_get_iso_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_iso_mode_t *mode);

/*!
 * @brief set iso mode
 */
int ep1_cf02a_set_iso_mode(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_iso_mode_t *mode, bool *success);

/*!
 * @brief get bit timing
 */
int ep1_cf02a_get_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_bit_timing_t *timing);

/*!
 * @brief set bit timing
 */
int ep1_cf02a_set_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_bit_timing_t *timing, bool *success);

/*!
 * @brief get data bit timing
 */
int ep1_cf02a_get_data_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_data_bit_timing_t *timing);

/*!
 * @brief set data bit timing
 */
int ep1_cf02a_set_data_bit_timing(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_data_bit_timing_t *timing, bool *success);

/*!
 * @brief get tx rx control
 */
int ep1_cf02a_get_tx_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_tx_rx_control_t *control);

/*!
 * @brief set tx rx control
 */
int ep1_cf02a_set_tx_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_tx_rx_control_t *control, bool *success);

/*!
 * @brief get can clock
 */
int ep1_cf02a_get_can_clock(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_can_clock_t *can_clock);

/*!
 * @brief get device timestamp reset time
 */
int ep1_cf02a_get_device_timestamp_reset_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_device_timestamp_reset_time_t *time);


/*!
 * @brief set host timestamp reset time
 */
int ep1_cf02a_set_host_timestamp_reset_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_host_timestamp_reset_time_t *time, bool *success);

/*!
 * @brief get rtc time
 */
int ep1_cf02a_get_rtc_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_rtc_time_t *time);

/*!
 * @brief set rtc time
 */
int ep1_cf02a_set_rtc_time(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_rtc_time_t *time, bool *success);

/*!
 * @brief get current store data state
 */
int ep1_cf02a_get_current_store_data_state(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_current_store_data_state_t *state);

/*!
 * @brief get store data id list count
 */
int ep1_cf02a_get_store_data_id_list_count(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_id_list_count_t *count);

/*!
 * @brief get store data id
 */
int ep1_cf02a_get_store_data_id(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_id_request_t *id_req, ep1_cf02a_msg_get_store_data_id_response_t *id_res);

/*!
 * @brief get store data meta
 */
int ep1_cf02a_get_store_data_meta(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_meta_request_t *meta_req, ep1_cf02a_msg_get_store_data_meta_response_t *meta_res);

/*!
 * @brief get store data rx control
 */
int ep1_cf02a_get_store_data_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_store_data_rx_control_request_t *control_req, ep1_cf02a_msg_get_store_data_rx_control_response_t *control_res);

/*!
 * @brief set store data rx control
 */
int ep1_cf02a_set_store_data_rx_control(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_set_store_data_rx_control_t *control, bool *success);

/*!
 * @brief delete store data
 */
int ep1_cf02a_delete_store_data(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_delete_store_data_t *data, bool *success);

/*!
 * @brief init store data media
 */
int ep1_cf02a_init_store_data_media(apt_usbtrx_dev_t *dev, bool *success);

/*!
 * @brief get capabilities
 */
int ep1_cf02a_get_capabilities(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_capabilities_t *capabilities);

#endif /* __EP1_CF02A_CMD_H__ */
