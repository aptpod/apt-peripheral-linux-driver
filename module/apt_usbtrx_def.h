/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_DEF_H__
#define __APT_USBTRX_DEF_H__

#include <linux/types.h>
#include <linux/usb.h>
#include <linux/kref.h>
#include <linux/version.h>
#include <linux/time.h>

#include "apt_usbtrx_ringbuffer.h"
#include "apt_usbtrx_ioctl.h"
#include "apt_usbtrx_msg.h"

/*!
 * @brief unit test macro
 */
#ifdef UNIT_TEST
#define STATIC /* enabling static functions to be called from KUnit tests */
#else
#define STATIC static
#endif

/*!
 * @biref log output macro
 */
#define APT_MSG_KEY "apt_usbtrx"
#define __file__ (strrchr(__FILE__, '/') + 1)

#define IMSG(format, arg...) pr_info(APT_MSG_KEY ": " format "\n", ##arg)
#define DMSG(format, arg...) pr_debug(APT_MSG_KEY ": %s[%d] " format "\n", __file__, __LINE__, ##arg)
#define WMSG(format, arg...) pr_warn(APT_MSG_KEY " WARN: %s[%d] " format "\n", __file__, __LINE__, ##arg)
#define EMSG(format, arg...) pr_err(APT_MSG_KEY " ERROR: %s[%d] " format "\n", __file__, __LINE__, ##arg)
#define CHKMSG(str) DMSG("[%s] %s()", str, __func__)

#define IMSG_RL(format, arg...) pr_info_ratelimited(APT_MSG_KEY ": " format "\n", ##arg)
#define DMSG_RL(format, arg...) pr_debug_ratelimited(APT_MSG_KEY ": %s[%d] " format "\n", __file__, __LINE__, ##arg)
#define WMSG_RL(format, arg...) pr_warn_ratelimited(APT_MSG_KEY " WARN: %s[%d] " format "\n", __file__, __LINE__, ##arg)
#define EMSG_RL(format, arg...) pr_err_ratelimited(APT_MSG_KEY " ERROR: %s[%d] " format "\n", __file__, __LINE__, ##arg)

/*!
 * @brief result code
 */
enum RESULT { RESULT_Failure = -1, RESULT_Success = 0, RESULT_NotEnough = 1, RESULT_Timeout = 2 };

/*!
 * @brief const value
 */
#define MAX_RX_URBS (4)
#define MAX_TX_URBS (4)
#define RX_BUFFER_SIZE (1024)
#define APT_USBTRX_RECV_TIMEOUT (1000)
#define APT_USBTRX_SEND_TIMEOUT (1000)
#define APT_USBTRX_TXDATA_BUFFER_SIZE (256 * 1024)
#define APT_USBTRX_TX_TOKEN_EXPIRED_TIME (1)
#define APT_USBTRX_TX_TOKEN_CAN_SIZE (16)
#define APT_USBTRX_DEVICE_ID_LENGTH (4)
#define APT_USBTRX_SERIAL_NO_LENGTH (14)
#define APT_USBTRX_TX_TRANSFER_LIMIT_RATE (80)

/*!
 * @brief vendor id
 */
#define APTPOD_VENDOR_ID 0x32B2
#define APTPOD_DEVP_VENDOR_ID 0x0483

/*!
 * @brief minor
 */
#define APT_USBTRX_MINOR_BASE 0

/*!
 * @brief  get_raw_monotonic_ts64 aliases
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
#define get_raw_monotonic_ts64(ts) ktime_get_raw_ts64(ts)
#else
#define get_raw_monotonic_ts64(ts) getrawmonotonic64(ts)
#endif

/*!
 * @brief rx bulk transfer structure
 */
struct apt_usbtrx_rx_transfer_s {
	u8 *buffer;
	int buffer_size;
	int data_size;
};
typedef struct apt_usbtrx_rx_transfer_s apt_usbtrx_rx_transfer_t;

/*!
 * @brief rx completion structure
 */
struct apt_usbtrx_rx_complete_s {
	struct completion complete;
	u8 id;
	u8 *buffer;
	int buffer_size;
	int data_size;
};
typedef struct apt_usbtrx_rx_complete_s apt_usbtrx_rx_complete_t;

/*!
 * @brief timestamp structure
 */
struct apt_usbtrx_timestamp_s {
	u32 ts_sec;
	u32 ts_usec;
} __attribute__((packed));
typedef struct apt_usbtrx_timestamp_s apt_usbtrx_timestamp_t;

/*!
 * @brief device unique function
 */
struct apt_usbtrx_dev_s;
struct apt_usbtrx_device_unique_function_s {
	int (*init_data)(struct apt_usbtrx_dev_s *dev);
	int (*free_data)(struct apt_usbtrx_dev_s *dev);
	int (*init)(struct usb_interface *intf, const struct usb_device_id *id);
	int (*terminate)(struct apt_usbtrx_dev_s *dev);
	bool (*is_need_init_reset_ts)(struct apt_usbtrx_dev_s *dev);
	int (*dispatch_msg)(struct apt_usbtrx_dev_s *dev, u8 *data, apt_usbtrx_msg_t *msg);
	int (*get_read_payload_size)(const void *payload);
	int (*get_write_payload_size)(const void *payload);
	apt_usbtrx_timestamp_t *(*get_read_payload_timestamp)(const void *payload);
	int (*get_write_cmd_id)(void);
	int (*get_fw_size)(void);
	long (*ioctl)(struct file *file, unsigned int cmd, unsigned long arg);
	int (*is_device_input_start)(struct apt_usbtrx_dev_s *dev, bool *start);
	int (*sysfs_init)(struct device *dev);
	int (*sysfs_term)(struct device *dev);
	int (*open)(struct apt_usbtrx_dev_s *dev);
	int (*close)(struct apt_usbtrx_dev_s *dev);
	void (*write_bulk_callback)(struct urb *urb);
};
typedef struct apt_usbtrx_device_unique_function_s apt_usbtrx_device_unique_function_t;

/*!
 * @brief firmware version
 */
struct apt_usbtrx_firmware_version_s {
	int major;
	int minor;
	int revision;
} __attribute__((packed));
typedef struct apt_usbtrx_firmware_version_s apt_usbtrx_firmware_version_t;

/*!
 * @brief device type
 */
enum APT_USBTRX_DEVICE_TYPE {
	APT_USBTRX_DEVICE_TYPE_CAN = 0,
	APT_USBTRX_DEVICE_TYPE_ANALOG,
	APT_USBTRX_DEVICE_TYPE_CAN_FD,
	APT_USBTRX_DEVICE_TYPE_MAX
};

/*!
 * @brief device info structure
 */
struct apt_usbtrx_dev_s {
	struct usb_device *udev; /*!< */
	struct usb_interface *interface; /*!< */
	struct usb_endpoint_descriptor *bulk_in; /*!< */
	struct usb_endpoint_descriptor *bulk_out; /*!< */
	struct usb_anchor rx_submitted; /*!< */
	struct usb_anchor tx_submitted; /*!< */
	unsigned int max_rx_urbs; /*!< */
	void *rxbuf[MAX_RX_URBS]; /*!< */
	dma_addr_t rxbuf_dma[MAX_RX_URBS]; /*!< */
	apt_usbtrx_ringbuffer_t rx_data; /*!< */
	apt_usbtrx_rx_transfer_t rx_transfer; /*!< */
	apt_usbtrx_rx_complete_t rx_complete; /*!< */
	atomic_t rx_ongoing; /*!< */
	int basetime_clock_id; /*!< */
	struct timespec64 basetime; /*!< */
	atomic_t onopening; /*!< */
	atomic_t onclosing; /*!< */
	struct timespec64 *resettime; /*!< */
	int fw_count; /*!< */
	struct semaphore tx_usb_transfer_sem; /*!< */
	apt_usbtrx_ringbuffer_t tx_data; /*!< */
	unsigned long tx_transfer_expired; /*!< */
	int tx_transfer_max_token; /*!< */
	int tx_transfer_token; /*!< */
	struct task_struct *tx_thread; /*!< */
	int ch; /*!< */
	char serial_no[APT_USBTRX_SERIAL_NO_LENGTH + 1]; /*!< */
	enum APT_USBTRX_SYNC_PULSE sync_pulse; /*!< */
	apt_usbtrx_firmware_version_t fw_ver; /*!< */
	atomic_t tx_buffer_rate; /*!< */
	struct completion rx_done; /*!< */
	struct kref kref;
	enum APT_USBTRX_TIMESTAMP_MODE timestamp_mode; /*!< */
	enum APT_USBTRX_DEVICE_TYPE device_type; /*!< */
	size_t rx_data_size; /*!< */
	u8 *rx_rbmsg; /*!< */
	void *unique_data; /*!< */

	/* device unique function */
	apt_usbtrx_device_unique_function_t unique_func;
};
typedef struct apt_usbtrx_dev_s apt_usbtrx_dev_t;

/*!
 * @brief kref stuff
 */
#define to_apt_usbtrx_dev(d) container_of(d, apt_usbtrx_dev_t, kref)
void apt_usbtrx_delete(struct kref *kref);

/*!
 * @brief get unique data
 */
static inline void *get_unique_data(const apt_usbtrx_dev_t *dev)
{
	return dev->unique_data;
}

/*!
 * @brief get ts64
 */
static inline void get_ts64(const apt_usbtrx_dev_t *dev, struct timespec64 *now)
{
	if (dev->basetime_clock_id == CLOCK_MONOTONIC) {
		ktime_get_ts64(now);
	} else {
		get_raw_monotonic_ts64(now);
	}
}

/*!
 * @brief get relative time (nsec)
 */
static inline u64 apt_usbtrx_get_relative_time_ns(const apt_usbtrx_dev_t *dev, struct timespec64 *basetime)
{
	struct timespec64 now;

	get_ts64(dev, &now);
	return (timespec64_to_ns(&now) - timespec64_to_ns(basetime));
}

static inline void apt_usbtrx_convert_timestamp_to_timespec64(apt_usbtrx_timestamp_t *timestamp, struct timespec64 *ts)
{
	ts->tv_sec = timestamp->ts_sec;
	ts->tv_nsec = timestamp->ts_usec * 1000;
}

#endif /* __APT_USBTRX_DEF_H__ */
