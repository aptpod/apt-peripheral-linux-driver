// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT USB peripherals.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#include "apt_usbtrx_ringbuffer.h"
#include "apt_usbtrx_def.h"

/*!
 * @brief initial instance
 */
static int apt_usbtrx_ringbuffer_init_instance(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return RESULT_Failure;
	}

	ringbuffer->buffer = NULL;
	ringbuffer->buffer_size = 0;
	ringbuffer->begin = NULL;
	ringbuffer->end = NULL;
	ringbuffer->read = NULL;
	ringbuffer->write = NULL;
	ringbuffer->skip_count = 0;
	init_waitqueue_head(&ringbuffer->wq);
	ringbuffer->log_write_buffer_is_full = true;

	return RESULT_Success;
}

/*!
 * @brief init
 */
int apt_usbtrx_ringbuffer_init(apt_usbtrx_ringbuffer_t *ringbuffer, size_t size)
{
	u8 *buffer;
	int result;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return RESULT_Failure;
	}

	result = apt_usbtrx_ringbuffer_init_instance(ringbuffer);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_ringbuffer_init_instance().. Error");
		return RESULT_Failure;
	}

	buffer = kzalloc(size, GFP_KERNEL);
	if (buffer == NULL) {
		EMSG("vmalloc().. Error");
		return RESULT_Failure;
	}

	ringbuffer->buffer = buffer;
	ringbuffer->buffer_size = size;

	ringbuffer->begin = ringbuffer->buffer;
	ringbuffer->end = ringbuffer->buffer + ringbuffer->buffer_size;
	ringbuffer->read = ringbuffer->buffer;
	ringbuffer->write = ringbuffer->buffer;

	ringbuffer->log_write_buffer_is_full = true;

	return RESULT_Success;
}

/*!
 * @brief term
 */
int apt_usbtrx_ringbuffer_term(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return RESULT_Failure;
	}

	if (ringbuffer->buffer != NULL) {
		kfree(ringbuffer->buffer);

		ringbuffer->buffer = NULL;
		ringbuffer->buffer_size = 0;
		ringbuffer->begin = NULL;
		ringbuffer->end = NULL;
		ringbuffer->read = NULL;
		ringbuffer->write = NULL;
	}

	return RESULT_Success;
}

/*!
 * @brief read
 */
ssize_t apt_usbtrx_ringbuffer_read(apt_usbtrx_ringbuffer_t *ringbuffer, u8 *buffer, size_t size)
{
	size_t read_size = 0;
	u8 *pread;
	u8 *pwrite;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return -1;
	}

	if (buffer == NULL) {
		EMSG("buffer is NULL");
		return -1;
	}

	if (size == 0) {
		EMSG("size is Zero");
		return 0;
	}

	pread = ringbuffer->read;
	pwrite = ringbuffer->write;

	if (pread < pwrite) {
		size_t todo;

		if ((pread + size) <= pwrite) {
			todo = size;
		} else {
			todo = pwrite - pread;
		}

		if (copy_to_user(buffer, pread, todo) != 0) {
			EMSG("copy_to_user().. Error");
			return -1;
		}
		pread += todo;
		read_size = todo;
	} else if (pread > pwrite) {
		if ((pread + size) <= ringbuffer->end) {
			if (copy_to_user(buffer, pread, size) != 0) {
				EMSG("copy_to_user().. Error");
				return -1;
			}
			pread += size;
			read_size = size;
		} else {
			size_t first = ringbuffer->end - pread;
			size_t second = size - first;

			if ((ringbuffer->begin + second) > pwrite) {
				second = pwrite - ringbuffer->begin;
			}

			if (first > 0) {
				if (copy_to_user(buffer, pread, first) != 0) {
					EMSG("copy_to_user().. Error");
					return -1;
				}
			}
			if (second > 0) {
				if (copy_to_user(buffer + first, ringbuffer->begin, second) != 0) {
					EMSG("copy_to_user().. Error");
					return -1;
				}
			}
			pread = ringbuffer->begin + second;
			read_size = first + second;
		}
	} else {
		read_size = 0;
	}

	if (read_size > 0) {
		ringbuffer->read = pread;
	}

	return read_size;
}

/*!
 * @brief raw read
 */
ssize_t apt_usbtrx_ringbuffer_rawread(apt_usbtrx_ringbuffer_t *ringbuffer, u8 *buffer, size_t size)
{
	size_t read_size = 0;
	u8 *pread;
	u8 *pwrite;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return -1;
	}

	if (buffer == NULL) {
		EMSG("buffer is NULL");
		return -1;
	}

	if (size == 0) {
		EMSG("size is Zero");
		return 0;
	}

	pread = ringbuffer->read;
	pwrite = ringbuffer->write;

	if (pread < pwrite) {
		size_t todo;

		if ((pread + size) <= pwrite) {
			todo = size;
		} else {
			todo = pwrite - pread;
		}

		memcpy(buffer, pread, todo);
		pread += todo;
		read_size = todo;
	} else if (pread > pwrite) {
		if ((pread + size) <= ringbuffer->end) {
			memcpy(buffer, pread, size);
			pread += size;
			read_size = size;
		} else {
			size_t first = ringbuffer->end - pread;
			size_t second = size - first;

			if ((ringbuffer->begin + second) > pwrite) {
				second = pwrite - ringbuffer->begin;
			}

			if (first > 0) {
				memcpy(buffer, pread, first);
			}
			if (second > 0) {
				memcpy(buffer + first, ringbuffer->begin, second);
			}
			pread = ringbuffer->begin + second;
			read_size = first + second;
		}
	} else {
		read_size = 0;
	}

	if (read_size > 0) {
		ringbuffer->read = pread;
	}

	return read_size;
}

/*!
 * @brief write
 */
ssize_t apt_usbtrx_ringbuffer_write(apt_usbtrx_ringbuffer_t *ringbuffer, const u8 *buffer, size_t size)
{
	u8 *pread;
	u8 *pwrite;
	size_t skip_count = 0;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return -1;
	}

	if (buffer == NULL) {
		EMSG("buffer is NULL");
		return -1;
	}

	if (size == 0) {
		EMSG("size is Zero");
		return 0;
	}

	pread = ringbuffer->read;
	pwrite = ringbuffer->write;

	if (pread <= pwrite) {
		if ((pwrite + size) <= ringbuffer->end) {
			memcpy(pwrite, buffer, size);
			pwrite += size;
		} else {
			size_t first = ringbuffer->end - pwrite;
			size_t second = size - first;

			if ((ringbuffer->begin + second) < pread) {
				if (first > 0) {
					memcpy(pwrite, buffer, first);
				}
				if (second > 0) {
					memcpy(ringbuffer->begin, buffer + first, second);
				}
				pwrite = ringbuffer->begin + second;
			} else {
				skip_count = size;
			}
		}
	} else {
		if ((pwrite + size) < pread) {
			memcpy(pwrite, buffer, size);
			pwrite += size;
		} else {
			skip_count = size;
		}
	}

	ringbuffer->write = pwrite;
	if (skip_count > 0) {
		ringbuffer->skip_count += skip_count;
		if (ringbuffer->log_write_buffer_is_full) {
			EMSG("ringbuffer is full");
			/* disable log continue output */
			ringbuffer->log_write_buffer_is_full = false;
		}
		return -1;
	}

	return size;
}

/*!
 * @brief is empty
 */
bool apt_usbtrx_ringbuffer_is_empty(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	bool empty;
	u8 *pread;
	u8 *pwrite;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return false;
	}

	pread = ringbuffer->read;
	pwrite = ringbuffer->write;

	if (pread == pwrite) {
		empty = true;
	} else {
		empty = false;
	}

	return empty;
}

/*!
 * @brief clear
 */
int apt_usbtrx_ringbuffer_clear(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return RESULT_Failure;
	}

	ringbuffer->read = ringbuffer->buffer;
	ringbuffer->write = ringbuffer->buffer;

	ringbuffer->log_write_buffer_is_full = true;

	return RESULT_Success;
}

/*!
 * @brief get used_size
 */
size_t apt_usbtrx_ringbuffer_get_used_size(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	u8 *pread;
	u8 *pwrite;
	size_t used_size;

	if (ringbuffer == NULL) {
		EMSG("ringbuffer is NULL");
		return 0;
	}

	pread = ringbuffer->read;
	pwrite = ringbuffer->write;

	if (pwrite >= pread) {
		used_size = pwrite - pread;
	} else {
		used_size = (pwrite - ringbuffer->begin) + (ringbuffer->end - pread);
	}

	return used_size;
}

/*!
 * @brief get free size
 */
size_t apt_usbtrx_ringbuffer_get_free_size(apt_usbtrx_ringbuffer_t *ringbuffer)
{
	size_t used_size = apt_usbtrx_ringbuffer_get_used_size(ringbuffer);
	return (ringbuffer->buffer_size - used_size);
}
