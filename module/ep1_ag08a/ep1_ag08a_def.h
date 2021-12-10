/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __EP1_AG08A_DEF_H__
#define __EP1_AG08A_DEF_H__

#include <linux/types.h>
#include <linux/iio/iio.h>

/*!
 * @brief product name
 */
#define EP1_AG08A_PRODUCT_NAME "ep1_ag08a"

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
#define EP1_AG08A_IIO_DATA_SIZE (sizeof(u16) * EP1_AG08A_CH_NUM + sizeof(s64))
/*!
 * @brief interface type
 */
enum EP1_AG08A_IF_TYPE { EP1_AG08A_IF_TYPE_NONE, EP1_AG08A_IF_TYPE_FILE, EP1_AG08A_IF_TYPE_IIO };

/*!
 * @brief device unique data
 */
struct ep1_ag08a_unique_data_s {
	atomic_t if_type;
	struct iio_dev *indio_dev;
};
typedef struct ep1_ag08a_unique_data_s ep1_ag08a_unique_data_t;

/*!
 * @brief IIO device private data structure
 */
struct ep1_ag08a_iio_data_s {
	apt_usbtrx_dev_t *dev;
	bool hw_timestamp;
	u8 buffer[EP1_AG08A_IIO_DATA_SIZE];
};
typedef struct ep1_ag08a_iio_data_s ep1_ag08a_iio_data_t;

#endif /* __EP1_AG08A_DEF_H__ */
