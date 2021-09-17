/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux.
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_RINGBUFFER_H__
#define __APT_USBTRX_RINGBUFFER_H__

#include <linux/types.h>

/*!
 * @brief ring buffer structrue
 */
struct apt_usbtrx_ringbuffer_s {
	u8 *buffer; /*!< */
	size_t buffer_size; /*!< */
	u8 *begin; /*!< */
	u8 *end; /*!< */
	u8 *read; /*!< */
	u8 *write; /*!< */
	u64 skip_count; /*!< */
	wait_queue_head_t wq; /*!< */
	bool log_write_buffer_is_full; /*!< */
};
typedef struct apt_usbtrx_ringbuffer_s apt_usbtrx_ringbuffer_t;

/*!
 * @brief init
 */
int apt_usbtrx_ringbuffer_init(apt_usbtrx_ringbuffer_t *ringbuffer, size_t size);

/*!
 * @brief term
 */
int apt_usbtrx_ringbuffer_term(apt_usbtrx_ringbuffer_t *ringbuffer);

/*!
 * @brief read
 */
ssize_t apt_usbtrx_ringbuffer_read(apt_usbtrx_ringbuffer_t *ringbuffer, u8 *buffer, size_t size);

/*!
 * @brief raw read
 */
ssize_t apt_usbtrx_ringbuffer_rawread(apt_usbtrx_ringbuffer_t *ringbuffer, u8 *buffer, size_t size);

/*!
 * @brief write
 */
ssize_t apt_usbtrx_ringbuffer_write(apt_usbtrx_ringbuffer_t *ringbuffer, const u8 *buffer, size_t size);

/*!
 * @brief is buffer empty
 */
bool apt_usbtrx_ringbuffer_is_empty(apt_usbtrx_ringbuffer_t *ringbuffer);

/*!
 * @brief clear
 */
int apt_usbtrx_ringbuffer_clear(apt_usbtrx_ringbuffer_t *ringbuffer);

/*!
 * @brief get used_size
 */
size_t apt_usbtrx_ringbuffer_get_used_size(apt_usbtrx_ringbuffer_t *ringbuffer);

/*!
 * @brief get free size
 */
size_t apt_usbtrx_ringbuffer_get_free_size(apt_usbtrx_ringbuffer_t *ringbuffer);

#endif /* #ifndef __APT_USBTRX_RINGBUFFER_H__ */
