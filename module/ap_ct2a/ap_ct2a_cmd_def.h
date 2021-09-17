/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */
#ifndef __AP_CT2A_CMD_DEF_H__
#define __AP_CT2A_CMD_DEF_H__

#include <linux/types.h>
#include "../apt_usbtrx_def.h" /* apt_usbtrx_timestamp_t */

/*!
 * @brief command
 */
enum APT_USBTRX_CMD_CAN {
	/*** Request ***/
	APT_USBTRX_CMD_SendCANFrame = 0x0F,
	APT_USBTRX_CMD_SetMode = 0x0A,
	APT_USBTRX_CMD_GetStatus = 0x05,
	APT_USBTRX_CMD_GetCANSummary = 0x03, /* not use */
	APT_USBTRX_CMD_ResetCANSummary = 0x04,
	APT_USBTRX_CMD_StartStopCAN = 0x06,
	APT_USBTRX_CMD_SetTrigger = 0x0B,
	/*** Notify ***/
	APT_USBTRX_CMD_NotifyRecvCANFrame = 0x2F,
	APT_USBTRX_CMD_NotifyCANSummary = 0x2B,
	/*** Response ***/
	APT_USBTRX_CMD_ResponseGetStatus = 0x27,
};

/*!
 * @brief command length (not payload size)
 */
/*** Request ***/
#define APT_USBTRX_CMD_LENGTH_SEND_CAN_FRAME (17)
#define APT_USBTRX_CMD_LENGTH_SET_MODE (5)
#define APT_USBTRX_CMD_LENGTH_GET_STATUS (4)
#define APT_USBTRX_CMD_LENGTH_GET_CAN_SUMMARY (4)
#define APT_USBTRX_CMD_LENGTH_RESET_CAN_SUMMARY (4)
#define APT_USBTRX_CMD_LENGTH_START_STOP_CAN (5)
#define APT_USBTRX_CMD_LENGTH_SET_TRIGGER (12)
/*** Notify ***/
#define APT_USBTRX_CMD_LENGTH_NOTIFY_RECV_CAN_FRAME (25)
#define APT_USBTRX_CMD_LENGTH_NOTIFY_CAN_SUMMARY (21)
/*** Response ***/
#define APT_USBTRX_CMD_LENGTH_RESPONSE_GET_STATUS (13)

#define APT_USBTRX_DLC_MASK 0x0F

/*!
 * @brief tirgger logic
 */
enum APT_USBTRX_TRIGGER_LOGIC {
	APT_USBTRX_TRIGGER_LOGIC_Unknown = -1,
	APT_USBTRX_TRIGGER_LOGIC_Positive = 0,
	APT_USBTRX_TRIGGER_LOGIC_Negative,
};

/*!
 * @brief trigger structure
 */
struct apt_usbtrx_trigger_s {
	bool enable;
	int logic;
	int id;
};
typedef struct apt_usbtrx_trigger_s apt_usbtrx_trigger_t;

/*!
 * @brief message structure (set trigger)
 */
struct apt_usbtrx_msg_set_trigger_s {
	apt_usbtrx_trigger_t input;
	apt_usbtrx_trigger_t output;
	int pulse_width;
	int pulse_interval;
};
typedef struct apt_usbtrx_msg_set_trigger_s apt_usbtrx_msg_set_trigger_t;

/*!
 * @brief bitrate mode
 */
enum APT_USBTRX_BITRATE_MODE {
	APT_USBTRX_BITRATE_MODE_Unknown = -1,
	APT_USBTRX_BITRATE_MODE_None = 0x00,
	APT_USBTRX_BITRATE_MODE_33K = 0x10,
	APT_USBTRX_BITRATE_MODE_50K = 0x11,
	APT_USBTRX_BITRATE_MODE_83K = 0x12,
	APT_USBTRX_BITRATE_MODE_100K = 0x13,
	APT_USBTRX_BITRATE_MODE_125K = 0x14,
	APT_USBTRX_BITRATE_MODE_250K = 0x15,
	APT_USBTRX_BITRATE_MODE_500K = 0x16,
	APT_USBTRX_BITRATE_MODE_1M = 0x17,
};

/*!
 * @brief message structure (set mode)
 */
struct apt_usbtrx_msg_set_mode_s {
	bool silent;
	bool start;
	int baudrate;
};
typedef struct apt_usbtrx_msg_set_mode_s apt_usbtrx_msg_set_mode_t;

/*!
 * @brief message structure (get status)
 */
struct apt_usbtrx_msg_resp_get_status_s {
	apt_usbtrx_msg_set_mode_t mode;
	bool enable_trigger;
	apt_usbtrx_msg_set_trigger_t trigger;
};
typedef struct apt_usbtrx_msg_resp_get_status_s apt_usbtrx_msg_resp_get_status_t;

/*!
 * @brief payload structure - notify recv can frame
 */
struct apt_usbtrx_payload_notify_recv_can_frame_s {
	apt_usbtrx_timestamp_t timestamp;
	u8 id[4]; /* id[3]bit5:ok/error, id[3]bit6:data/rtr, id[3]bit7:std/ext */
	u8 dlc; /* bit0-3:dlc, bit4-7:reserve */
	u8 data[8];
} __attribute__((packed));
typedef struct apt_usbtrx_payload_notify_recv_can_frame_s apt_usbtrx_payload_notify_recv_can_frame_t;

/*!
 * @brief payload structure - send can frame
 */
struct apt_usbtrx_payload_send_can_frame_s {
	u8 id[4]; /* id[3]bit5:rsv, id[3]bit6:data/rtr, id[3]bit7:std/ext */
	u8 dlc; /* bit0-3:dlc, bit4-7:reserve */
	u8 data[8];
} __attribute__((packed));
typedef struct apt_usbtrx_payload_send_can_frame_s apt_usbtrx_payload_send_can_frame_t;

#endif /* __AP_CT2A_CMD_DEF_H__ */
