/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_DEF_H__
#define __EP1_CF02A_DEF_H__

#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/can/dev.h>
#include "ep1_cf02a_cmd_def.h"

/*!
 * @brief product id
 */
#define EP1_CF02A_PRODUCT_ID 0x0101
#define EP1_CF02A_MODEL_NAME "EP1-CF02A"

/*!
 * @brief const value
 */
#define EP1_CF02A_FW_DATA_SIZE (256 * 1024)
#define EP1_CF02A_RXDATA_BUFFER_SIZE (128 * 4 * 1024)
#define EP1_CF02A_STORE_DATA_BUFFER_SIZE (8 * 4 * 1024)

#define EP1_CF02A_CAN_SYNC_SEG 1
#define EP1_CF02A_CAN_CALC_MAX_ERROR 50 /* in one-tenth of a percent */

/*!
 * @brief interface type
 */
enum EP1_CF02A_IF_TYPE { EP1_CF02A_IF_TYPE_NONE, EP1_CF02A_IF_TYPE_FILE, EP1_CF02A_IF_TYPE_NET };

/*!
 * @brief device unique data
 */
struct ep1_cf02a_unique_data_s {
	u32 can_clock;
	const struct can_bittiming_const *bittiming_const;
	const struct can_bittiming_const *data_bittiming_const;
	ep1_cf02a_msg_bit_timing_t *bittiming;
	ep1_cf02a_msg_bit_timing_t *data_bittiming;
	apt_usbtrx_ringbuffer_t rx_store_data;
	size_t rx_store_data_size;
	u64 can_frame_count_per_request;
	atomic_t received_store_data_size;
	struct completion rx_store_data_done;
	bool notify_store_data_recv_can_frame_complete;
	bool is_store_data_enabled;
	atomic_t file_open_count;
	atomic_t if_type;
	struct net_device *netdev;
	atomic_t on_terminating;
};
typedef struct ep1_cf02a_unique_data_s ep1_cf02a_unique_data_t;

/*!
 * @brief device info structure
 */
struct ep1_cf02a_candev_s {
	struct can_priv can; /* must be the first member */
	u8 tx_data_size;
	apt_usbtrx_dev_t *dev;
	struct timespec64 reset_ts;
	atomic64_t rx_packets;
	atomic64_t rx_bytes;
	atomic64_t tx_packets;
	atomic64_t tx_bytes;
	atomic64_t fw_rx_dropped;
	struct delayed_work statistics_work;
};
typedef struct ep1_cf02a_candev_s ep1_cf02a_candev_t;

#endif /* __EP1_CF02A_DEF_H__ */
