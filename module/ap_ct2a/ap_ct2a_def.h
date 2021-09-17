/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_DEF_H__
#define __AP_CT2A_DEF_H__

#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/can/dev.h>

/*!
 * @brief product id
 */
#define AP_CT2A_PRODUCT_ID 0x0100
#define AP_CT2A_DEVP_PRODUCT_ID 0x5740
#define EP1_CH02A_PRODUCT_ID 0x0105

/*!
 * @brief const value
 */
#define APT_USBTRX_FW_DATA_SIZE (384 * 1024)
#define AP_CT2A_CLOCK 10000000 /* TODO: bittiming settings implement */
#define AP_CT2A_DEFAULT_BAUDRATE 500000
#define AP_CT2A_RXDATA_BUFFER_SIZE (4 * 1024)

/*!
 * @brief stats
 */
struct apt_usbtrx_stats_s {
	ktime_t kt;
	unsigned long timestamp;
	u32 num;
	u32 total_num;
};
typedef struct apt_usbtrx_stats_s apt_usbtrx_stats_t;

/*!
 * @brief can summary
 */
struct apt_usbtrx_can_summary_s {
	apt_usbtrx_stats_t dat_std;
	apt_usbtrx_stats_t dat_ext;
	apt_usbtrx_stats_t rtr_std;
	apt_usbtrx_stats_t rtr_ext;
	apt_usbtrx_stats_t err;
};
typedef struct apt_usbtrx_can_summary_s apt_usbtrx_can_summary_t;

/*!
 * @brief interface type
 */
enum APT_USBTRX_CAN_IF_TYPE {
	APT_USBTRX_CAN_IF_TYPE_NONE = -1,
	APT_USBTRX_CAN_IF_TYPE_FILE = 0,
	APT_USBTRX_CAN_IF_TYPE_NET
};

/*!
 * @brief device unique data
 */
struct apt_usbtrx_unique_data_can_s {
	apt_usbtrx_can_summary_t summary;
	enum APT_USBTRX_CAN_IF_TYPE if_type;

	/* socketcan */
	struct net_device *netdev;
};
typedef struct apt_usbtrx_unique_data_can_s apt_usbtrx_unique_data_can_t;

/*!
 * @brief device info structure
 */
struct apt_usbtrx_candev_s {
	struct can_priv can; /* must be the first member */
	u8 tx_data_size;
	apt_usbtrx_dev_t *dev;
};
typedef struct apt_usbtrx_candev_s apt_usbtrx_candev_t;

#endif /* __AP_CT2A_DEF_H__ */
