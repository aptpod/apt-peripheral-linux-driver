/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2024 aptpod Inc.
 */
#ifndef __EP1_CF02A_CMD_DEF_H__
#define __EP1_CF02A_CMD_DEF_H__

#include <linux/types.h>
#include <linux/can.h>
#include "../apt_usbtrx_def.h" /* apt_usbtrx_timestamp_t */

/*!
 * @brief command
 */
enum EP1_CF02A_CMD {
	/*** Request ***/
	EP1_CF02A_CMD_SetSilentMode = 0x30,
	EP1_CF02A_CMD_GetSilentMode = 0x31,
	EP1_CF02A_CMD_SetISOMode = 0x32,
	EP1_CF02A_CMD_GetISOMode = 0x33,
	EP1_CF02A_CMD_SetBitTiming = 0x34,
	EP1_CF02A_CMD_GetBitTiming = 0x35,
	EP1_CF02A_CMD_SetDataBitTiming = 0x36,
	EP1_CF02A_CMD_GetDataBitTiming = 0x37,
	EP1_CF02A_CMD_SetTxRxControl = 0x38,
	EP1_CF02A_CMD_GetTxRxControl = 0x39,
	EP1_CF02A_CMD_SendCANFrame = 0x3A,
	EP1_CF02A_CMD_SetCANClock = 0x3C, /* not use */
	EP1_CF02A_CMD_GetCANClock = 0x3D,
	EP1_CF02A_CMD_GetDeviceTimestampResetTime = 0x3E,
	EP1_CF02A_CMD_SetHostTimeStampResetTime = 0x40,
	EP1_CF02A_CMD_SetRTCTime = 0x41,
	EP1_CF02A_CMD_GetRTCTime = 0x42,
	EP1_CF02A_CMD_GetCurrentStoreDataState = 0x43,
	EP1_CF02A_CMD_GetStoreDataIDListCount = 0x44,
	EP1_CF02A_CMD_GetStoreDataID = 0x45,
	EP1_CF02A_CMD_GetStoreDataMeta = 0x46,
	EP1_CF02A_CMD_SetStoreDataRxControl = 0x47,
	EP1_CF02A_CMD_GetStoreDataRxControl = 0x48,
	EP1_CF02A_CMD_DeleteStoreData = 0x4A,
	EP1_CF02A_CMD_InitStoreDataMedia = 0x4B,
	EP1_CF02A_CMD_SetFDMode = 0x4C,
	EP1_CF02A_CMD_GetFDMode = 0x4D,
	EP1_CF02A_CMD_GetCapabilities = 0xE0,
	/*** Notify ***/
	EP1_CF02A_CMD_NotifyCANSummary = 0x2B, /* same as AP-CT2A/EP1-CH02A */
	EP1_CF02A_CMD_NotifyRecvCANFrame = 0x3B,
	EP1_CF02A_CMD_NotifyStoreDataRecvCanFrame = 0x49,
	/*** Response ***/
	EP1_CF02A_CMD_ResponseGetSilentMode = 0x61,
	EP1_CF02A_CMD_ResponseGetISOMode = 0x63,
	EP1_CF02A_CMD_ResponseGetBitTiming = 0x65,
	EP1_CF02A_CMD_ResponseGetDataBitTiming = 0x67,
	EP1_CF02A_CMD_ResponseGetTxRxControl = 0x69,
	EP1_CF02A_CMD_ResponseGetCANClock = 0x6D,
	EP1_CF02A_CMD_ResponseGetDeviceTimestampResetTime = 0x6E,
	EP1_CF02A_CMD_ResponseGetRTCTime = 0x72,
	EP1_CF02A_CMD_ResponseGetCurrentStoreDataState = 0x73,
	EP1_CF02A_CMD_ResponseGetStoreDataIDListCount = 0x74,
	EP1_CF02A_CMD_ResponseGetStoreDataID = 0x75,
	EP1_CF02A_CMD_ResponseGetStoreDataMeta = 0x76,
	EP1_CF02A_CMD_ResponseGetStoreDataRxControl = 0x78,
	EP1_CF02A_CMD_ResponseGetFDMode = 0x7D,
	EP1_CF02A_CMD_ResponseGetCapabilities = 0xF0,
};

/*!
 * @brief command length (not payload size)
 */
/*** Request ***/
#define EP1_CF02A_CMD_LENGTH_RESET_CAN_SUMMARY (4)
#define EP1_CF02A_CMD_LENGTH_SET_SILENT_MODE (5)
#define EP1_CF02A_CMD_LENGTH_GET_SILENT_MODE (4)
#define EP1_CF02A_CMD_LENGTH_SET_FD_MODE (5)
#define EP1_CF02A_CMD_LENGTH_GET_FD_MODE (4)
#define EP1_CF02A_CMD_LENGTH_SET_ISO_MODE (5)
#define EP1_CF02A_CMD_LENGTH_GET_ISO_MODE (4)
#define EP1_CF02A_CMD_LENGTH_SET_BIT_TIMING (14)
#define EP1_CF02A_CMD_LENGTH_GET_BIT_TIMING (4)
#define EP1_CF02A_CMD_LENGTH_SET_DATA_BIT_TIMING (14)
#define EP1_CF02A_CMD_LENGTH_GET_DATA_BIT_TIMING (4)
#define EP1_CF02A_CMD_LENGTH_SET_TX_RX_CONTROL (5)
#define EP1_CF02A_CMD_LENGTH_GET_TX_RX_CONTROL (4)
#define EP1_CF02A_CMD_LENGTH_SEND_CAN_FRAME (74)
#define EP1_CF02A_CMD_LENGTH_SET_CAN_CLOCK (8)
#define EP1_CF02A_CMD_LENGTH_GET_CAN_CLOCK (4)
#define EP1_CF02A_CMD_LENGTH_GET_DEVICE_TIMESTAMP_RESET_TIME (4)
#define EP1_CF02A_CMD_LENGTH_SET_HOST_TIMESTAMP_RESET_TIME (12)
#define EP1_CF02A_CMD_LENGTH_SET_RTC_TIME (15)
#define EP1_CF02A_CMD_LENGTH_GET_RTC_TIME (4)
#define EP1_CF02A_CMD_LENGTH_GET_CURRENT_STORE_DATA_STATE (4)
#define EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_ID_LIST_COUNT (4)
#define EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_ID (8)
#define EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_META (36)
#define EP1_CF02A_CMD_LENGTH_SET_STORE_DATA_RX_CONTROL (39)
#define EP1_CF02A_CMD_LENGTH_GET_STORE_DATA_RX_CONTROL (36)
#define EP1_CF02A_CMD_LENGTH_DELETE_STORE_DATA (36)
#define EP1_CF02A_CMD_LENGTH_INIT_STORE_DATA_MEDIA (4)
#define EP1_CF02A_CMD_LENGTH_GET_CAPABILITIES (4)
/*** Notify ***/
#define EP1_CF02A_CMD_LENGTH_NOTIFY_CAN_SUMMARY (78)
#define EP1_CF02A_CMD_LENGTH_NOTIFY_RECV_CAN_FRAME (82)
#define EP1_CF02A_CMD_LENGTH_NOTIFY_STORE_DATA_RECV_CAN_FRAME (82)
/*** Response ***/
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_SILENT_MODE (5)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_FD_MODE (5)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_ISO_MODE (5)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_BIT_TIMING (14)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_DATA_BIT_TIMING (14)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_TX_RX_CONTROL (5)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CAN_CLOCK (8)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_DEVICE_TIMESTAMP_RESET_TIME (12)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_RTC_TIME (15)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CURRENT_STORE_DATA_STATE (37)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_ID_LIST_COUNT (8)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_ID (36)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_META (47)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_STORE_DATA_RX_CONTROL (7)
#define EP1_CF02A_CMD_LENGTH_RESPONSE_GET_CAPABILITIES (5)

#define EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH (32)

#pragma pack(push, 1)
/*!
 * @brief message structure - silent mode
 */
struct ep1_cf02a_msg_silent_mode_s {
	bool silent;
};
typedef struct ep1_cf02a_msg_silent_mode_s ep1_cf02a_msg_get_silent_mode_t;
typedef struct ep1_cf02a_msg_silent_mode_s ep1_cf02a_msg_set_silent_mode_t;

/*!
 * @brief message structure - fd mode
 */
struct ep1_cf02a_msg_fd_mode_s {
	bool fd;
};
typedef struct ep1_cf02a_msg_fd_mode_s ep1_cf02a_msg_get_fd_mode_t;
typedef struct ep1_cf02a_msg_fd_mode_s ep1_cf02a_msg_set_fd_mode_t;

/*!
 * @brief message structure - iso mode
 */
struct ep1_cf02a_msg_iso_mode_s {
	bool non_iso_mode;
};
typedef struct ep1_cf02a_msg_iso_mode_s ep1_cf02a_msg_get_iso_mode_t;
typedef struct ep1_cf02a_msg_iso_mode_s ep1_cf02a_msg_set_iso_mode_t;

/*!
 * @brief message structure - bit-timing
 */
struct ep1_cf02a_msg_bit_timing_s {
	int prop_seg;
	int phase_seg1;
	int phase_seg2;
	int sjw;
	int brp;
};
typedef struct ep1_cf02a_msg_bit_timing_s ep1_cf02a_msg_bit_timing_t;
typedef struct ep1_cf02a_msg_bit_timing_s ep1_cf02a_msg_get_bit_timing_t;
typedef struct ep1_cf02a_msg_bit_timing_s ep1_cf02a_msg_set_bit_timing_t;
typedef struct ep1_cf02a_msg_bit_timing_s ep1_cf02a_msg_get_data_bit_timing_t;
typedef struct ep1_cf02a_msg_bit_timing_s ep1_cf02a_msg_set_data_bit_timing_t;

/*!
 * @brief message structure - can clock
 */
struct ep1_cf02a_msg_can_clock_s {
	int can_clock;
};
typedef struct ep1_cf02a_msg_can_clock_s ep1_cf02a_msg_get_can_clock_t;
typedef struct ep1_cf02a_msg_can_clock_s ep1_cf02a_msg_set_can_clock_t;

/*!
 * @brief message structure - tx rx control
 */
struct ep1_cf02a_msg_tx_rx_control_s {
	bool start;
};
typedef struct ep1_cf02a_msg_tx_rx_control_s ep1_cf02a_msg_get_tx_rx_control_t;
typedef struct ep1_cf02a_msg_tx_rx_control_s ep1_cf02a_msg_set_tx_rx_control_t;

/*!
 * @brief message structure - get device timestamp reset time
 */
struct ep1_cf02a_msg_device_timestamp_reset_time_s {
	apt_usbtrx_timestamp_t ts;
};
typedef struct ep1_cf02a_msg_device_timestamp_reset_time_s ep1_cf02a_msg_get_device_timestamp_reset_time_t;

/*!
 * @brief message structure - set host timestamp reset time
 */
struct ep1_cf02a_msg_host_timestamp_reset_time_s {
	struct timespec ts;
};
typedef struct ep1_cf02a_msg_host_timestamp_reset_time_s ep1_cf02a_msg_set_host_timestamp_reset_time_t;

/*!
 * @brief message structure - rtc time
 */
struct ep1_cf02a_msg_rtc_time_s {
	u8 weekday;
	u8 month;
	u8 date;
	u8 year;
	u8 hour;
	u8 minute;
	u8 second;
	u32 microsecond;
};
typedef struct ep1_cf02a_msg_rtc_time_s ep1_cf02a_msg_rtc_time_t;
typedef struct ep1_cf02a_msg_rtc_time_s ep1_cf02a_msg_get_rtc_time_t;
typedef struct ep1_cf02a_msg_rtc_time_s ep1_cf02a_msg_set_rtc_time_t;

/*!
 * @brief message structure - current store data state
 */
struct ep1_cf02a_msg_current_store_data_state_s {
	u8 state;
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_current_store_data_state_s ep1_cf02a_msg_get_current_store_data_state_t;

/*!
 * @brief message structure - get store data id list count
 */
struct ep1_cf02a_msg_store_data_id_list_count_s {
	u32 count;
};
typedef struct ep1_cf02a_msg_store_data_id_list_count_s ep1_cf02a_msg_get_store_data_id_list_count_t;

/*!
 * @brief message structure - get store data id request
 */
struct ep1_cf02a_msg_get_store_data_id_request_s {
	int index;
};
typedef struct ep1_cf02a_msg_get_store_data_id_request_s ep1_cf02a_msg_get_store_data_id_request_t;

/*!
 * @brief message structure - get store data id response
 */
struct ep1_cf02a_msg_get_store_data_id_response_s {
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_get_store_data_id_response_s ep1_cf02a_msg_get_store_data_id_response_t;

/*!
 * @brief message structure - store data id list
 */
struct ep1_cf02a_msg_store_data_id_list_s {
	u32 count;
	char (*id_list)[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_store_data_id_list_s ep1_cf02a_msg_get_store_data_id_list_t;

/*!
 * @brief message structure - get store data meta request
 */
struct ep1_cf02a_msg_get_store_data_meta_request_s {
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_get_store_data_meta_request_s ep1_cf02a_msg_get_store_data_meta_request_t;

/*!
 * @brief message structure - start time
 */
struct ep1_cf02a_msg_start_time_s {
	ep1_cf02a_msg_rtc_time_t rtc;
	apt_usbtrx_timestamp_t ts;
};
typedef struct ep1_cf02a_msg_start_time_s ep1_cf02a_msg_start_time_t;

/*!
 * @brief message structure - timestamp reset time
 */
struct ep1_cf02a_msg_timestamp_reset_time_s {
	apt_usbtrx_timestamp_t device_ts;
	struct timespec host_monotonic;
};
typedef struct ep1_cf02a_msg_timestamp_reset_time_s ep1_cf02a_msg_timestamp_reset_time_t;

/*!
 * @brief message structure - get store data meta response
 */
struct ep1_cf02a_msg_get_store_data_meta_response_s {
	ep1_cf02a_msg_start_time_t start_time;
	ep1_cf02a_msg_timestamp_reset_time_t reset_time;
	u64 can_frame_count;
};
typedef struct ep1_cf02a_msg_get_store_data_meta_response_s ep1_cf02a_msg_get_store_data_meta_response_t;

/*!
 * @brief message structure - set store data rx control
 */
struct ep1_cf02a_msg_set_store_data_rx_control_s {
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
	u8 start;
	u16 interval;
};
typedef struct ep1_cf02a_msg_set_store_data_rx_control_s ep1_cf02a_msg_set_store_data_rx_control_t;

/*!
 * @brief message structure - get store data rx control request
 */
struct ep1_cf02a_msg_get_store_data_rx_control_request_s {
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_get_store_data_rx_control_request_s ep1_cf02a_msg_get_store_data_rx_control_request_t;

/*!
 * @brief message structure - get store data rx control response
 */
struct ep1_cf02a_msg_get_store_data_rx_control_response_s {
	u8 start;
	u16 interval;
};
typedef struct ep1_cf02a_msg_get_store_data_rx_control_response_s ep1_cf02a_msg_get_store_data_rx_control_response_t;

/*!
 * @brief message structure - delete store data
 */
struct ep1_cf02a_msg_delete_store_data_s {
	char id[EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_msg_delete_store_data_s ep1_cf02a_msg_delete_store_data_t;

/*!
 * @brief message structure - get capabilities
 */
struct ep1_cf02a_msg_capabilities_s {
	u64 capabilities;
};
typedef struct ep1_cf02a_msg_capabilities_s ep1_cf02a_msg_get_capabilities_t;

/*!
 * @brief payload structure - notify recv can frame
 */
struct ep1_cf02a_payload_notify_recv_can_frame_s {
	apt_usbtrx_timestamp_t timestamp;
	u8 id[4]; /* id[3]bit5:ok/error, id[3]bit6:data/rtr, id[3]bit7:std/ext */
	u8 dlc; /* bit0-3:dlc, bit4-7:reserve */
	u8 flags; /* bit0:BRS, bit1:ESI, bit2:FDF, bi4-7:reserve */
	u8 data[64];
};
typedef struct ep1_cf02a_payload_notify_recv_can_frame_s ep1_cf02a_payload_notify_recv_can_frame_t;

/*!
 * @brief payload structure - send can frame
 */
struct ep1_cf02a_payload_send_can_frame_s {
	u8 id[4]; /* id[3]bit5:rsv, id[3]bit6:data/rtr, id[3]bit7:std/ext */
	u8 dlc; /* bit0-3:dlc, bit4-7:reserve */
	u8 flags; /* bit0:BRS, bit1:ESI, bit2:FDF, bi4-7:reserve */
	u8 data[64];
};
typedef struct ep1_cf02a_payload_send_can_frame_s ep1_cf02a_payload_send_can_frame_t;

/*!
 * @brief payload structure - notify recv can summary
 */
struct ep1_cf02a_payload_notify_recv_can_summary_s {
	u32 rx_count;
	struct canfd_frame frame;
};
typedef struct ep1_cf02a_payload_notify_recv_can_summary_s ep1_cf02a_payload_notify_recv_can_summary_t;

#pragma pack(pop)

#endif /* __EP1_CF02A_CMD_DEF_H__ */
