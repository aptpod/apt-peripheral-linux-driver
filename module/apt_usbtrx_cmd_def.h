/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_CMD_DEF_H__
#define __APT_USBTRX_CMD_DEF_H__

#include <linux/types.h>

/*!
 * @brief command
 */
enum APT_USBTRX_CMD {
	APT_USBTRX_CMD_Unknown = -1,
	/*** Request ***/
	APT_USBTRX_CMD_GetDeviceId = 0x00,
	APT_USBTRX_CMD_GetFWVersion = 0x01,
	APT_USBTRX_CMD_GetFWVersionRevision = 0x0D,
	APT_USBTRX_CMD_EnableResetTS = 0x02,
	APT_USBTRX_CMD_ResetTS = 0x07,
	APT_USBTRX_CMD_ResetDevice = 0x08,
	APT_USBTRX_CMD_GetSerialNo = 0x0C,
	APT_USBTRX_CMD_MoveDFU = 0x09,
	APT_USBTRX_CMD_SendFWData = 0x56,
	APT_USBTRX_CMD_SendFWDataS = 0x55,
	APT_USBTRX_CMD_SendFWDataE = 0x57,
	/*** Notify ***/
	APT_USBTRX_CMD_NotifyBufferStatus = 0x28,
	/*** Response ***/
	APT_USBTRX_CMD_ResponseGetDeviceId = 0x20,
	APT_USBTRX_CMD_ResponseGetSerialNo = 0x2C,
	APT_USBTRX_CMD_ResponseGetFWVersion = 0x21,
	APT_USBTRX_CMD_ResponseGetFWVersionRevision = 0x22,
	APT_USBTRX_CMD_ACK = 0x25,
	APT_USBTRX_CMD_NACK = 0x26,
};

/*!
 * @brief command length (not payload size)
 */
/*** Request ***/
#define APT_USBTRX_CMD_LENGTH_GET_DEVICE_ID (4)
#define APT_USBTRX_CMD_LENGTH_GET_FW_VERSION (4)
#define APT_USBTRX_CMD_LENGTH_GET_FW_VERSION_REVISION (4)
#define APT_USBTRX_CMD_LENGTH_ENABLE_RESET_TS (4)
#define APT_USBTRX_CMD_LENGTH_RESET_TS (4)
#define APT_USBTRX_CMD_LENGTH_RESET_DEVICE (4)
#define APT_USBTRX_CMD_LENGTH_GET_SERIAL_NO (4)
#define APT_USBTRX_CMD_LENGTH_MOVE_DFU (4)
#define APT_USBTRX_CMD_LENGTH_SEND_FW_DATA (36)
/*** Notify ***/
#define APT_USBTRX_CMD_LENGTH_NOTIFY_BUFFER_STATUS (5)
/*** Response ***/
#define APT_USBTRX_CMD_LENGTH_RESPONSE_GET_DEVICE_ID (9)
#define APT_USBTRX_CMD_LENGTH_RESPONSE_GET_SERIAL_NO (19)
#define APT_USBTRX_CMD_LENGTH_RESPONSE_GET_FW_VERSION (6)
#define APT_USBTRX_CMD_LENGTH_RESPONSE_GET_FW_VERSION_REVISION (7)
#define APT_USBTRX_CMD_LENGTH_ACK (5)
#define APT_USBTRX_CMD_LENGTH_NACK (5)

#define APT_USBTRX_CMD_MIN_LENGTH (4)
#define APT_USBTRX_CMD_MAX_LENGTH (36)

/*!
 * @brief command data length (not payload size)
 */
#define APT_USBTRX_CMD_DEVICE_ID_LENGTH APT_USBTRX_DEVICE_ID_LENGTH
#define APT_USBTRX_CMD_SERIAL_NO_LENGTH APT_USBTRX_SERIAL_NO_LENGTH

#endif /* __APT_USBTRX_CMD_DEF_H__ */
