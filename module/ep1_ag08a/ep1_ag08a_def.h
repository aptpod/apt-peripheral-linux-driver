/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_DEF_H__
#define __EP1_AG08A_DEF_H__

#include <linux/types.h>

/*!
 * @brief product id
 */
#define EP1_AG08A_PRODUCT_ID 0x102

/*!
 * @brief const value
 */
#define EP1_AG08A_FW_DATA_SIZE (192 * 1024)
#define EP1_AG08A_CH_NUM (8)
#define EP1_AG08A_RXDATA_BUFFER_SIZE (64 * 1024)

/*!
 * @brief device unique data
 */
struct ep1_ag08a_unique_data_s {
	u8 dummy; /* no unique data */
};
typedef struct ep1_ag08a_unique_data_s ep1_ag08a_unique_data_t;

#endif /* __EP1_AG08A_DEF_H__ */
