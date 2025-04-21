/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_DEF_H__
#define __EP1_CF02A_DEF_H__

#include <linux/types.h>

/*!
 * @brief product id
 */
#define EP1_CF02A_PRODUCT_ID 0x0101

/*!
 * @brief const value
 */
#define EP1_CF02A_FW_DATA_SIZE (384 * 1024)
#define EP1_CF02A_RXDATA_BUFFER_SIZE (8 * 4 * 1024)
#define EP1_CF02A_STORE_DATA_BUFFER_SIZE (8 * 4 * 1024)

#define EP1_CF02A_CAN_SYNC_SEG 1
#define EP1_CF02A_CAN_CALC_MAX_ERROR 50 /* in one-tenth of a percent */

/*!
 * @brief stats
 */
struct ep1_cf02a_stats_s {
	ktime_t kt;
	unsigned long timestamp;
	u32 num;
	u32 total_num;
};
typedef struct ep1_cf02a_stats_s ep1_cf02a_stats_t;

/*!
 * @brief can summary
 */
struct ep1_cf02a_can_summary_s {
	ep1_cf02a_stats_t dat_std;
	ep1_cf02a_stats_t dat_ext;
	ep1_cf02a_stats_t rtr_std;
	ep1_cf02a_stats_t rtr_ext;
	ep1_cf02a_stats_t err;
};
typedef struct ep1_cf02a_can_summary_s ep1_cf02a_can_summary_t;

/*!
 * @brief device unique data
 */
struct ep1_cf02a_unique_data_s {
	const struct can_bittiming_const *bittiming_const;
	const struct can_bittiming_const *data_bittiming_const;
	ep1_cf02a_can_summary_t summary;
	apt_usbtrx_ringbuffer_t rx_store_data;
	size_t rx_store_data_size;
	struct completion rx_store_data_done;
};
typedef struct ep1_cf02a_unique_data_s ep1_cf02a_unique_data_t;

#endif /* __EP1_CF02A_DEF_H__ */
