/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux.
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_FOPS_DEF_H__
#define __APT_USBTRX_FOPS_DEF_H__

#if !defined(__KERNEL__)
#define __user
#endif

#include <linux/ioctl.h>
#include <linux/version.h>

#ifdef __KERNEL__
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
struct timespec {
	__kernel_old_time_t tv_sec;
	long tv_nsec;
};

#if __BITS_PER_LONG == 64
static inline struct timespec timespec64_to_timespec(const struct timespec64 ts64)
{
	return *(const struct timespec *)&ts64;
}
static inline struct timespec64 timespec_to_timespec64(const struct timespec ts)
{
	return *(const struct timespec64 *)&ts;
}
#else
static inline struct timespec timespec64_to_timespec(const struct timespec64 ts64)
{
	struct timespec ret;

	ret.tv_sec = (__kernel_old_time_t)ts64.tv_sec;
	ret.tv_nsec = ts64.tv_nsec;
	return ret;
}
static inline struct timespec64 timespec_to_timespec64(const struct timespec ts)
{
	struct timespec64 ret;

	ret.tv_sec = ts.tv_sec;
	ret.tv_nsec = ts.tv_nsec;
	return ret;
}
#endif
#endif
#endif

/* ----------------------------------------------------------- */
/* ----------------------- All Device ------------------------ */
/* ----------------------------------------------------------- */

#define APT_USBTRX_DEVICE_ID_LENGTH (4)

/**
 * struct apt_usbtrx_ioctl_get_device_id_s - Unique device id definition.
 * @channel: Channel number.
 * @id: Unique device id.
 */
struct apt_usbtrx_ioctl_get_device_id_s {
	int channel;
	unsigned char id[APT_USBTRX_DEVICE_ID_LENGTH];
};

/**
 * typedef apt_usbtrx_ioctl_get_device_id_t - Alias struct apt_usbtrx_ioctl_get_device_id_s.
 */
typedef struct apt_usbtrx_ioctl_get_device_id_s apt_usbtrx_ioctl_get_device_id_t;

#define APT_USBTRX_SERIAL_NO_LENGTH (14)

/**
 * struct apt_usbtrx_ioctl_get_serial_no_s - Serial number definition.
 * @channel: Channel number.
 * @sync_pulse: Synchronization pulse, see enum APT_USBTRX_SYNC_PULSE.
 * @sn: Serial number.
 */
struct apt_usbtrx_ioctl_get_serial_no_s {
	int channel;
	int sync_pulse;
	unsigned char sn[APT_USBTRX_SERIAL_NO_LENGTH];
};

/**
 * typedef apt_usbtrx_ioctl_get_serial_no_t - Alias struct apt_usbtrx_ioctl_get_serial_no_s.
 */
typedef struct apt_usbtrx_ioctl_get_serial_no_s apt_usbtrx_ioctl_get_serial_no_t;

/**
 * struct apt_usbtrx_ioctl_get_fw_version_s - Firmware version definition.
 * @major: Major version.
 * @minor: Minor version.
 */
struct apt_usbtrx_ioctl_get_fw_version_s {
	int major;
	int minor;
};

/**
 * typedef apt_usbtrx_ioctl_get_fw_version_t - Alias struct apt_usbtrx_ioctl_get_fw_version_s.
 */
typedef struct apt_usbtrx_ioctl_get_fw_version_s apt_usbtrx_ioctl_get_fw_version_t;

/**
 * struct apt_usbtrx_ioctl_get_fw_version_revison_s - Firmware version definition.
 * @major: Major version.
 * @minor: Minor version.
 * @revision: Patch version.
 */
struct apt_usbtrx_ioctl_get_fw_version_revision_s {
	int major;
	int minor;
	int revision;
};

/**
 * typedef apt_usbtrx_ioctl_get_fw_version_revision_t - Alias struct apt_usbtrx_ioctl_get_fw_version_revision_s.
 */
typedef struct apt_usbtrx_ioctl_get_fw_version_revision_s apt_usbtrx_ioctl_get_fw_version_revision_t;

/**
 * struct apt_usbtrx_ioctl_reset_ts_s - Timestamp definition
 * @ts: The time when the timestamp was reset. It is elapsed time since the host system started.
 */
struct apt_usbtrx_ioctl_reset_ts_s {
	struct timespec ts;
};

/**
 * typedef apt_usbtrx_ioctl_reset_ts_t - Alias struct apt_usbtrx_ioctl_reset_ts_s.
 */
typedef struct apt_usbtrx_ioctl_reset_ts_s apt_usbtrx_ioctl_reset_ts_t;

/**
 * struct apt_usbtrx_ioctl_get_basetime_s - Getting basetime definition.
 * @basetime: Current base time. It is elapsed time since the host system started.
 */
struct apt_usbtrx_ioctl_get_basetime_s {
	struct timespec basetime;
};

/**
 * typedef apt_usbtrx_ioctl_get_basetime_t - Alias struct apt_usbtrx_ioctl_get_basetime_s.
 */
typedef struct apt_usbtrx_ioctl_get_basetime_s apt_usbtrx_ioctl_get_basetime_t;

/**
 * struct apt_usbtrx_ioctl_set_basetime_s - Setting basetime definition.
 * @basetime: The time to set as base time.
 */
struct apt_usbtrx_ioctl_set_basetime_s {
	struct timespec basetime;
};

/**
 * typedef apt_usbtrx_ioctl_set_basetime_t - Alias struct apt_usbtrx_ioctl_set_basetime_s.
 */
typedef struct apt_usbtrx_ioctl_set_basetime_s apt_usbtrx_ioctl_set_basetime_t;

/**
 * struct apt_usbtrx_ioctl_set_timestamp_mode_s - Timestamping definition
 * @timestamp_mode: How to timestamp receiving data, see APT_USBTRX_TIMESTAMP_MODE.
 */
struct apt_usbtrx_ioctl_set_timestamp_mode_s {
	int timestamp_mode;
};

/**
 * typedef apt_usbtrx_ioctl_set_timestamp_mode_t - Alias struct apt_usbtrx_ioctl_set_timestamp_mode_s.
 */
typedef struct apt_usbtrx_ioctl_set_timestamp_mode_s apt_usbtrx_ioctl_set_timestamp_mode_t;

/**
 * struct apt_usbtrx_ioctl_get_timestamp_mode_s - Timestamping definition
 * @timestamp_mode: How to timestamp receiving data, see APT_USBTRX_TIMESTAMP_MODE.
 */
struct apt_usbtrx_ioctl_get_timestamp_mode_s {
	int timestamp_mode;
};

/**
 * typedef apt_usbtrx_ioctl_get_timestamp_mode_t - Alias struct apt_usbtrx_ioctl_get_timestamp_mode_s.
 */
typedef struct apt_usbtrx_ioctl_get_timestamp_mode_s apt_usbtrx_ioctl_get_timestamp_mode_t;

/**
 * struct apt_usbtrx_ioctl_get_firmware_size_s - Device firmware definition
 * @firmware_size: Device firmware size.
 */
struct apt_usbtrx_ioctl_get_firmware_size_s {
	int firmware_size;
};

/**
 * typedef apt_usbtrx_ioctl_get_firmware_size_t - Alias struct apt_usbtrx_ioctl_get_firmware_size_s.
 */
typedef struct apt_usbtrx_ioctl_get_firmware_size_s apt_usbtrx_ioctl_get_firmware_size_t;

/**
 * enum APT_USBTRX_TIMESTAMP_MODE - Timestamp mode
 * @APT_USBTRX_TIMESTAMP_MODE_DEVICE: Use device to timestamping.
 * @APT_USBTRX_TIMESTAMP_MODE_HOST: Use host to timestamping.
 */
enum APT_USBTRX_TIMESTAMP_MODE {
	APT_USBTRX_TIMESTAMP_MODE_DEVICE = 0,
	APT_USBTRX_TIMESTAMP_MODE_HOST,
	APT_USBTRX_TIMESTAMP_MODE_MAX
};

/**
 * enum APT_USBTRX_SYNC_PULSE - Synchronization pulse status
 * @APT_USBTRX_SYNC_PULSE_SOURCE: Use own clock and provide synchronization pulse to other devices.
 * @APT_USBTRX_SYNC_PULSE_EXTERNAL: Use external clock and synchronization pulse.
 */
enum APT_USBTRX_SYNC_PULSE {
	APT_USBTRX_SYNC_PULSE_SOURCE = 0,
	APT_USBTRX_SYNC_PULSE_EXTERNAL,
};

/* ----------------------------------------------------------- */
/* ------------------------- AP-CT2A ------------------------- */
/* ----------------------------------------------------------- */
/**
 * struct apt_usbtrx_ioctl_set_mode_s - CAN settings definition.
 * @silent: Device is listen only or not.
 * @baudrate: CAN bitrate in kbps: 33,50,83,100,125,250,500,1000.
 */
struct apt_usbtrx_ioctl_set_mode_s {
	bool silent;
	int baudrate;
};

/**
 * typedef apt_usbtrx_ioctl_set_mode_t - Alias struct apt_usbtrx_ioctl_set_mode_s.
 */
typedef struct apt_usbtrx_ioctl_set_mode_s apt_usbtrx_ioctl_set_mode_t;

/**
 * struct apt_usbtrx_ioctl_trigger_s
 */
struct apt_usbtrx_ioctl_trigger_s {
	bool enable;
	int logic;
	int id;
};

/**
 * typedef apt_usbtrx_ioctl_trigger_t - Alias struct apt_usbtrx_ioctl_trigger_s.
 */
typedef struct apt_usbtrx_ioctl_trigger_s apt_usbtrx_ioctl_trigger_t;

/**
 * struct apt_usbtrx_ioctl_set_trigger_s
 */
struct apt_usbtrx_ioctl_set_trigger_s {
	apt_usbtrx_ioctl_trigger_t input;
	apt_usbtrx_ioctl_trigger_t output;
	int pulse_width;
	int pulse_interval;
};

/**
 * typedef apt_usbtrx_ioctl_set_trigger_t - Alias struct apt_usbtrx_ioctl_set_trigger_s.
 */
typedef struct apt_usbtrx_ioctl_set_trigger_s apt_usbtrx_ioctl_set_trigger_t;

/**
 * struct apt_usbtrx_ioctl_get_status_s
 */
struct apt_usbtrx_ioctl_get_status_s {
	bool silent;
	bool start;
	int baudrate;
	bool enable_trigger;
	apt_usbtrx_ioctl_set_trigger_t trigger;
};

/**
 * typedef apt_usbtrx_ioctl_get_status_t - Alias struct apt_usbtrx_ioctl_get_status_s.
 */
typedef struct apt_usbtrx_ioctl_get_status_s apt_usbtrx_ioctl_get_status_t;

/* ----------------------------------------------------------- */
/* ------------------------ EP1-CH02A ------------------------ */
/* ----------------------------------------------------------- */

#define EP1_CH02A_CAN_PACKET_SIZE (21)

/**
 * typedef ep1_ch02a_ioctl_set_mode_t - Alias struct apt_usbtrx_ioctl_set_mode_t.
 */
typedef apt_usbtrx_ioctl_set_mode_t ep1_ch02a_ioctl_set_mode_t;

/**
 * struct ep1_ch02a_ioctl_get_status_s - Device status definition.
 * @silent: Device is listen only or not.
 * @start: CAN transceiver is working or not.
 * @baudrate: CAN bitrate in kbps.
 * @enable_sync: Timestamp resetting is enable or disable.
 */
struct ep1_ch02a_ioctl_get_status_s {
	bool silent;
	bool start;
	int baudrate;
	bool enable_sync;
};

/**
 * typedef ep1_ch02a_ioctl_get_status_t - Alias struct ep1_ch02a_ioctl_get_status_s
 */
typedef struct ep1_ch02a_ioctl_get_status_s ep1_ch02a_ioctl_get_status_t;

/**
 * struct ep1_ch02a_ioctl_set_bit_timing_s - CAN Bit timing definition.
 * @prop_seg: Propagation segment.
 * @phase_seg1: Phase segment 1.
 * @phase_seg2: Phase segment 2.
 * @sjw: Synchronization Jump Width.
 * @brp: Bitrate prescaler.
 */
struct ep1_ch02a_ioctl_set_bit_timing_s {
	int prop_seg;
	int phase_seg1;
	int phase_seg2;
	int sjw;
	int brp;
};

/**
 * typedef ep1_ch02a_ioctl_set_bit_timing_t - Alias struct ep1_ch02a_ioctl_set_bit_timing_s
 */
typedef struct ep1_ch02a_ioctl_set_bit_timing_s ep1_ch02a_ioctl_set_bit_timing_t;

/**
 * struct ep1_ch02a_ioctl_get_bit_timing_s - CAN Bit timing definition.
 * @can_clock: CAN system clock frequency in Hz.
 * @cfg: Bit timing configurations, see struct ep1_ch02a_ioctl_set_bit_timing_s
 */
struct ep1_ch02a_ioctl_get_bit_timing_s {
	int can_clock;
	ep1_ch02a_ioctl_set_bit_timing_t cfg;
};

/**
 * typedef ep1_ch02a_ioctl_get_bit_timing_t - Alias struct ep1_ch02a_ioctl_get_bit_timing_s
 */
typedef struct ep1_ch02a_ioctl_get_bit_timing_s ep1_ch02a_ioctl_get_bit_timing_t;

/* ----------------------------------------------------------- */
/* ------------------------ EP1-CF02A ------------------------ */
/* ----------------------------------------------------------- */

#define EP1_CF02A_CAN_PACKET_SIZE (78)
#define EP1_CF02A_STORE_DATA_ID_MAX_LENGTH (32)

/**
 * struct ep1_cf02a_ioctl_silent_mode_s - Silent mode definition.
 * @silent: Device is listen only or not.
 */
struct ep1_cf02a_ioctl_silent_mode_s {
	bool silent;
};
typedef struct ep1_cf02a_ioctl_silent_mode_s ep1_cf02a_ioctl_get_silent_mode_t;
typedef struct ep1_cf02a_ioctl_silent_mode_s ep1_cf02a_ioctl_set_silent_mode_t;

/**
 * struct ep1_cf02a_ioctl_fd_mode_s - FD mode definition.
 * @fd: Enable CAN FD.
 */
struct ep1_cf02a_ioctl_fd_mode_s {
	bool fd;
};
typedef struct ep1_cf02a_ioctl_fd_mode_s ep1_cf02a_ioctl_get_fd_mode_t;
typedef struct ep1_cf02a_ioctl_fd_mode_s ep1_cf02a_ioctl_set_fd_mode_t;

/**
 * struct ep1_cf02a_ioctl_iso_mode_s - ISO mode definition.
 * @non_iso_mode: ISO compliant(false) or non-ISO compliant(true).
 */
struct ep1_cf02a_ioctl_iso_mode_s {
	bool non_iso_mode;
};
typedef struct ep1_cf02a_ioctl_iso_mode_s ep1_cf02a_ioctl_get_iso_mode_t;
typedef struct ep1_cf02a_ioctl_iso_mode_s ep1_cf02a_ioctl_set_iso_mode_t;

/**
 * struct ep1_cf02a_ioctl_bitrate_s - Arbitration phase bitrate definition.
 * @bitrate: bitrate bps.
 * @sample_point: sample point (e.g. 875 means 87.5%). If set to 0, the recommended value by CiA will be used.
 */
struct ep1_cf02a_ioctl_bitrate_s {
	int bitrate;
	int sample_point;
};
typedef struct ep1_cf02a_ioctl_bitrate_s ep1_cf02a_ioctl_get_bitrate_t;
typedef struct ep1_cf02a_ioctl_bitrate_s ep1_cf02a_ioctl_set_bitrate_t;
typedef struct ep1_cf02a_ioctl_bitrate_s ep1_cf02a_ioctl_get_data_bitrate_t;
typedef struct ep1_cf02a_ioctl_bitrate_s ep1_cf02a_ioctl_set_data_bitrate_t;

/**
 * struct ep1_cf02a_ioctl_bit_timing_s - CAN Bit timing definition.
 * @prop_seg: Propagation segment.
 * @phase_seg1: Phase segment 1.
 * @phase_seg2: Phase segment 2.
 * @sjw: Synchronization Jump Width.
 * @brp: Bitrate prescaler.
 */
struct ep1_cf02a_ioctl_bit_timing_s {
	int prop_seg;
	int phase_seg1;
	int phase_seg2;
	int sjw;
	int brp;
};
typedef struct ep1_cf02a_ioctl_bit_timing_s ep1_cf02a_ioctl_bit_timing_t;
typedef struct ep1_cf02a_ioctl_bit_timing_s ep1_cf02a_ioctl_get_bit_timing_t;
typedef struct ep1_cf02a_ioctl_bit_timing_s ep1_cf02a_ioctl_set_bit_timing_t;
typedef struct ep1_cf02a_ioctl_bit_timing_s ep1_cf02a_ioctl_get_data_bit_timing_t;
typedef struct ep1_cf02a_ioctl_bit_timing_s ep1_cf02a_ioctl_set_data_bit_timing_t;

/**
 * struct ep1_cf02a_ioctl_tx_rx_control_s - CAN transceiver control definition.
 * @start: CAN transceiver is working or not.
 */
struct ep1_cf02a_ioctl_tx_rx_control_s {
	bool start;
};
typedef struct ep1_cf02a_ioctl_tx_rx_control_s ep1_cf02a_ioctl_get_tx_rx_control_t;
typedef struct ep1_cf02a_ioctl_tx_rx_control_s ep1_cf02a_ioctl_set_tx_rx_control_t;

/**
 * struct ep1_cf02a_ioctl_get_device_timestamp_reset_time_s - Device timestamp reset time definition.
 * @reset_time: The device time when the timestamp was reset.
 */
struct ep1_cf02a_ioctl_get_device_timestamp_reset_time_s {
	struct timespec ts;
};
typedef struct ep1_cf02a_ioctl_get_device_timestamp_reset_time_s ep1_cf02a_ioctl_get_device_timestamp_reset_time_t;

/**
 * struct ep1_cf02a_ioctl_rtc_time_s - RTC time definition.
 * @weekday: Weekday. 1: Monday - 7: Sunday.
 * @month: Month. 1: January - 12: December.
 * @date: Date. 1-31.
 * @year: Year. 0-99. 0 means 2000, 99 means 2099.
 * @hour: Hour. 0-23.
 * @minute: Minute. 0-59.
 * @second: Second. 0-59.
 * @microsecond: Microsecond. 0-999999.
 */
struct ep1_cf02a_ioctl_rtc_time_s {
	unsigned int weekday;
	unsigned int month;
	unsigned int date;
	unsigned int year;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int microsecond;
};
typedef struct ep1_cf02a_ioctl_rtc_time_s ep1_cf02a_ioctl_rtc_time_t;
typedef struct ep1_cf02a_ioctl_rtc_time_s ep1_cf02a_ioctl_get_rtc_time_t;

/**
 * enum EP1_CF02A_STORE_DATA_STATE - Store data state
 * @EP1_CF02A_STORE_DATA_STATE_NOT_STORING: Storing data is supported, but not currently in progress.
 * @EP1_CF02A_STORE_DATA_STATE_STORING: Storing data is supported and currently in progress.
 * @EP1_CF02A_STORE_DATA_STATE_NOT_SUPPORTED: Storing data is not supported, for example, when the store data media is unavailable.
 */
enum EP1_CF02A_STORE_DATA_STATE {
	EP1_CF02A_STORE_DATA_STATE_NOT_STORING = 0,
	EP1_CF02A_STORE_DATA_STATE_STORING,
	EP1_CF02A_STORE_DATA_STATE_NOT_SUPPORTED,
	EP1_CF02A_STORE_DATA_STATE_MAX
};

/**
 * struct ep1_cf02a_ioctl_get_current_store_data_state_s - Current store data state definition.
 * @state: Current store data state. See enum EP1_CF02A_STORE_DATA_STATE.
 * @id: Current store data id. If current store data is not exist, this value is all zero.
 */
struct ep1_cf02a_ioctl_get_current_store_data_state_s {
	int state;
	char id[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_ioctl_get_current_store_data_state_s ep1_cf02a_ioctl_get_current_store_data_state_t;

/**
 * struct ep1_cf02a_ioctl_get_store_data_id_list_count_s - Store data id list count definition.
 * @count: Store data id list count.
 */
struct ep1_cf02a_ioctl_get_store_data_id_list_count_s {
	unsigned int count;
};
typedef struct ep1_cf02a_ioctl_get_store_data_id_list_count_s ep1_cf02a_ioctl_get_store_data_id_list_count_t;

/**
 * struct ep1_cf02a_ioctl_get_store_data_id_list_s - Store data id list definition.
 * @count: [IN] Store data id list count. User must set this value.
 * @id: [OUT] Store data id list. User must allocate memory for this list.
 *  The id list size must be count * EP1_CF02A_STORE_DATA_ID_MAX_LENGTH.
 */
struct ep1_cf02a_ioctl_get_store_data_id_list_s {
	unsigned int count;
	char (*id_list)[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_ioctl_get_store_data_id_list_s ep1_cf02a_ioctl_get_store_data_id_list_t;

/**
 * struct ep1_cf02a_ioctl_start_time_s - Start time definition.
 * @rtc: RTC time.
 * @ts: Timestamp.
 */
struct ep1_cf02a_ioctl_start_time_s {
	ep1_cf02a_ioctl_rtc_time_t rtc;
	struct timespec ts;
};
typedef struct ep1_cf02a_ioctl_start_time_s ep1_cf02a_ioctl_start_time_t;

/**
 * struct ep1_cf02a_ioctl_timestamp_reset_time_s - Timestamp reset time definition.
 * @device_ts: Device timestamp.
 * @host_monotonic: Host monotonic time.
 */
struct ep1_cf02a_ioctl_timestamp_reset_time_s {
	struct timespec device_ts;
	struct timespec host_monotonic;
};
typedef struct ep1_cf02a_ioctl_timestamp_reset_time_s ep1_cf02a_ioctl_timestamp_reset_time_t;

/**
 * struct ep1_cf02a_ioctl_get_store_data_meta_s - Store data meta definition.
 * @id: [IN] Store data id to get meta.
 * @start_time: [OUT] Start time.
 * @reset_time: [OUT] Timestamp reset time.
 * @can_frame_count: [OUT] CAN frame count.
 */
struct ep1_cf02a_ioctl_get_store_data_meta_s {
	char id[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
	ep1_cf02a_ioctl_start_time_t start_time;
	ep1_cf02a_ioctl_timestamp_reset_time_t reset_time;
	unsigned long long can_frame_count;
};
typedef struct ep1_cf02a_ioctl_get_store_data_meta_s ep1_cf02a_ioctl_get_store_data_meta_t;

/**
 * struct ep1_cf02a_ioctl_store_data_rx_control_s - Store data RX control definition.
 * @id: [IN] Store data id to control.
 * @start: [IN(Set)/OUT(Get)] Start or stop sending store data.
 * @interval: [IN(Set)/OUT(Get)] Sending interval in ms.
 */
struct ep1_cf02a_ioctl_store_data_rx_control_s {
	char id[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
	bool start;
	int interval;
};
typedef struct ep1_cf02a_ioctl_store_data_rx_control_s ep1_cf02a_ioctl_get_store_data_rx_control_t;
typedef struct ep1_cf02a_ioctl_store_data_rx_control_s ep1_cf02a_ioctl_set_store_data_rx_control_t;

/**
 * struct ep1_cf02a_ioctl_read_store_data_s - Read store data definition.
 * @buffer: [OUT] Buffer to store data. User must allocate memory for this buffer.
 * @count: [IN] Buffer size.
 */
struct ep1_cf02a_ioctl_read_store_data_s {
	char __user *buffer;
	size_t count;
};
typedef struct ep1_cf02a_ioctl_read_store_data_s ep1_cf02a_ioctl_read_store_data_t;

/**
 * struct ep1_cf02a_ioctl_delete_store_data_s - Delete store data definition.
 * @id: Store data id to delete.
 */
struct ep1_cf02a_ioctl_delete_store_data_s {
	char id[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
};
typedef struct ep1_cf02a_ioctl_delete_store_data_s ep1_cf02a_ioctl_delete_store_data_t;

/* ----------------------------------------------------------- */
/* ------------------------ EP1-AG08A ------------------------ */
/* ----------------------------------------------------------- */

#define EP1_AG08A_CH_NUM (8)

/**
 * struct ep1_ag08a_ioctl_set_analog_input_s -
 * @send_rate: Sampling frequency in mHz:
 *             0.01Hz, 0.1Hz, 1Hz, 10Hz, 156.25Hz, 312.5Hz, 625Hz, 1.25kHz, 2.5kHz, 5kHz, 10kHz
 * @vmin: Lower limit of input range in mV: -10V, -5V, 0V
 * @vmax: Upper limit of input range in mV: 10V, 5V
 */
struct ep1_ag08a_ioctl_set_analog_input_s {
	int send_rate;
	int vmin[EP1_AG08A_CH_NUM];
	int vmax[EP1_AG08A_CH_NUM];
};

/**
 * typedef ep1_ag08a_ioctl_set_analog_input_t - Alias struct ep1_ag08a_ioctl_set_analog_input_s
 */
typedef struct ep1_ag08a_ioctl_set_analog_input_s ep1_ag08a_ioctl_set_analog_input_t;

/**
 * struct ep1_ag08a_ioctl_control_analog_input_s - Analog input control settings definition.
 * @start: Analog input is working or not for each channel.
 */
struct ep1_ag08a_ioctl_control_analog_input_s {
	bool start[EP1_AG08A_CH_NUM];
};

/**
 * typedef ep1_ag08a_ioctl_control_analog_input_t - Alias struct ep1_ag08a_ioctl_control_analog_input_s
 */
typedef struct ep1_ag08a_ioctl_control_analog_input_s ep1_ag08a_ioctl_control_analog_input_t;

/**
 * struct ep1_ag08a_ioctl_set_analog_output_s - Analog waveform definition.
 * @waveform_type: Output waveform type, see enum EP1_AG08A_WAVEFORM_TYPE.
 * @voltage: Output waveform voltate in mV. Range: 0-5V, resolution: 20mV.
 * @frequency: Output waveform frequency in mHz. Range: 1-100Hz, resolution: 1Hz.
 */
struct ep1_ag08a_ioctl_set_analog_output_s {
	int waveform_type;
	int voltage;
	int frequency;
};

/**
 * typedef ep1_ag08a_ioctl_set_analog_output_t - Alias struct ep1_ag08a_ioctl_set_analog_output_s
 */
typedef struct ep1_ag08a_ioctl_set_analog_output_s ep1_ag08a_ioctl_set_analog_output_t;

/**
 * struct ep1_ag08a_ioctl_control_analog_output_s - Analog output control settings definition.
 * @start: Analog output is working or not.
 */
struct ep1_ag08a_ioctl_control_analog_output_s {
	bool start;
};

/**
 * typedef ep1_ag08a_ioctl_control_analog_output_t - Alias struct ep1_ag08a_ioctl_control_analog_output_s
 */
typedef struct ep1_ag08a_ioctl_control_analog_output_s ep1_ag08a_ioctl_control_analog_output_t;

/**
 * struct ep1_ag08a_ioctl_analog_input_s - Analog input definition.
 * @cfg: Analog input configurations, see struct ep1_ag08a_ioctl_set_analog_input_s.
 * @ctrl: Analog input control settings, see struct ep1_ag08a_ioctl_control_analog_input_s.
 */
struct ep1_ag08a_ioctl_analog_input_s {
	ep1_ag08a_ioctl_set_analog_input_t cfg;
	ep1_ag08a_ioctl_control_analog_input_t ctrl;
};

/**
 * typedef ep1_ag08a_ioctl_analog_input_t - Alias struct ep1_ag08a_ioctl_analog_input_s
 */
typedef struct ep1_ag08a_ioctl_analog_input_s ep1_ag08a_ioctl_analog_input_t;

/**
 * struct ep1_ag08a_ioctl_analog_output_s - Analog output definition.
 * @cfg: Analog waveform configurations, see struct ep1_ag08a_ioctl_set_analog_output_s.
 * @ctrl: Analog output control settings, see struct ep1_ag08a_ioctl_control_analog_output_s.
 */
struct ep1_ag08a_ioctl_analog_output_s {
	ep1_ag08a_ioctl_set_analog_output_t cfg;
	ep1_ag08a_ioctl_control_analog_output_t ctrl;
};

/**
 * typedef ep1_ag08a_ioctl_analog_output_t - Alias struct ep1_ag08a_ioctl_analog_output_s
 */
typedef struct ep1_ag08a_ioctl_analog_output_s ep1_ag08a_ioctl_analog_output_t;

/**
 * struct ep1_ag08a_ioctl_get_status_s - Device status definition.
 * @in: Analog input configurations, see struct ep1_ag08a_ioctl_analog_input_s.
 * @out: Analog output configurations, see struct ep1_ag08a_ioctl_analog_output_s.
 */
struct ep1_ag08a_ioctl_get_status_s {
	ep1_ag08a_ioctl_analog_input_t in;
	ep1_ag08a_ioctl_analog_output_t out;
};

/**
 * typedef ep1_ag08a_ioctl_get_status_t - Alias struct ep1_ag08a_ioctl_get_status_s
 */
typedef struct ep1_ag08a_ioctl_get_status_s ep1_ag08a_ioctl_get_status_t;

/**
 * enum EP1_AG08A_WAVEFORM_TYPE - Analog output waveform type.
 * @EP1_AG08A_WAVEFORM_RANDOM: Pseudo random signal.
 * @EP1_AG08A_WAVEFORM_SINE_WAVE: Sine wave.
 * @EP1_AG08A_WAVEFORM_TRIANGLE_WAVE: Triangle wave.
 * @EP1_AG08A_WAVEFORM_SQUARE_WAVE: Square wave.
 * @EP1_AG08A_WAVEFORM_FIXED: Fixed signal.
 */
enum EP1_AG08A_WAVEFORM_TYPE {
	EP1_AG08A_WAVEFORM_RANDOM = 0,
	EP1_AG08A_WAVEFORM_SINE_WAVE,
	EP1_AG08A_WAVEFORM_TRIANGLE_WAVE,
	EP1_AG08A_WAVEFORM_SQUARE_WAVE,
	EP1_AG08A_WAVEFORM_FIXED = 0x10,
};

/* ----------------------------------------------------------- */
/* ------------------------ PA-AGU081 ------------------------ */
/* ----------------------------------------------------------- */
/*
 * PA-AGU081 is prototype of EP1_AG08A.
 * The following definitions are retained for compatibility.
 */

#define PA_AGU081_CH_NUM EP1_AG08A_CH_NUM

typedef ep1_ag08a_ioctl_set_analog_input_t pa_agu081_ioctl_set_analog_input_t;
typedef ep1_ag08a_ioctl_control_analog_input_t pa_agu081_ioctl_control_analog_input_t;
typedef ep1_ag08a_ioctl_set_analog_output_t pa_agu081_ioctl_set_analog_output_t;
typedef ep1_ag08a_ioctl_control_analog_output_t pa_agu081_ioctl_control_analog_output_t;
typedef ep1_ag08a_ioctl_analog_input_t pa_agu081_ioctl_analog_input_t;
typedef ep1_ag08a_ioctl_get_status_t pa_agu081_ioctl_get_status_t;
typedef ep1_ag08a_ioctl_analog_output_t pa_agu081_ioctl_analog_output_t;

/**
 * ioctl command list
 */
#define APT_USBTRX_IOC_TYPE 'P'

#define APT_USBTRX_IOCTL_SET_MODE _IOW(APT_USBTRX_IOC_TYPE, 0x11, apt_usbtrx_ioctl_set_mode_t)
#define APT_USBTRX_IOCTL_GET_STATUS _IOR(APT_USBTRX_IOC_TYPE, 0x12, apt_usbtrx_ioctl_get_status_t)
#define APT_USBTRX_IOCTL_GET_DEVICE_ID _IOR(APT_USBTRX_IOC_TYPE, 0x13, apt_usbtrx_ioctl_get_device_id_t)
#define APT_USBTRX_IOCTL_GET_FW_VERSION _IOR(APT_USBTRX_IOC_TYPE, 0x14, apt_usbtrx_ioctl_get_fw_version_t)
#define APT_USBTRX_IOCTL_ENABLE_RESET_TS _IO(APT_USBTRX_IOC_TYPE, 0x15)
#define APT_USBTRX_IOCTL_RESET_TS _IOR(APT_USBTRX_IOC_TYPE, 0x16, apt_usbtrx_ioctl_reset_ts_t)
#define APT_USBTRX_IOCTL_RESET_CAN_SUMMARY _IO(APT_USBTRX_IOC_TYPE, 0x17)
#define APT_USBTRX_IOCTL_START_CAN _IO(APT_USBTRX_IOC_TYPE, 0x18)
#define APT_USBTRX_IOCTL_STOP_CAN _IO(APT_USBTRX_IOC_TYPE, 0x19)
#define APT_USBTRX_IOCTL_RESET_DEVICE _IO(APT_USBTRX_IOC_TYPE, 0x1a)
#define APT_USBTRX_IOCTL_SET_TRIGGER _IOW(APT_USBTRX_IOC_TYPE, 0x1b, apt_usbtrx_ioctl_set_trigger_t)
#define APT_USBTRX_IOCTL_GET_BASETIME _IOR(APT_USBTRX_IOC_TYPE, 0x1c, apt_usbtrx_ioctl_get_basetime_t)
#define APT_USBTRX_IOCTL_SET_BASETIME _IOW(APT_USBTRX_IOC_TYPE, 0x1d, apt_usbtrx_ioctl_set_basetime_t)
#define APT_USBTRX_IOCTL_GET_SERIAL_NO _IOR(APT_USBTRX_IOC_TYPE, 0x1e, apt_usbtrx_ioctl_get_serial_no_t)
#define APT_USBTRX_IOCTL_MOVE_DFU _IO(APT_USBTRX_IOC_TYPE, 0x1f)
#define APT_USBTRX_IOCTL_SET_TIMESTAMP_MODE _IOW(APT_USBTRX_IOC_TYPE, 0x20, apt_usbtrx_ioctl_set_timestamp_mode_t)
#define APT_USBTRX_IOCTL_GET_TIMESTAMP_MODE _IOR(APT_USBTRX_IOC_TYPE, 0x21, apt_usbtrx_ioctl_get_timestamp_mode_t)
#define APT_USBTRX_IOCTL_GET_FIRMWARE_SIZE _IOR(APT_USBTRX_IOC_TYPE, 0x27, apt_usbtrx_ioctl_get_firmware_size_t)
#define APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION                                                                       \
	_IOR(APT_USBTRX_IOC_TYPE, 0x28, apt_usbtrx_ioctl_get_fw_version_revision_t)

#define EP1_AG08A_IOCTL_GET_STATUS _IOR(APT_USBTRX_IOC_TYPE, 0x22, ep1_ag08a_ioctl_get_status_t)
#define EP1_AG08A_IOCTL_SET_ANALOG_INPUT _IOW(APT_USBTRX_IOC_TYPE, 0x23, ep1_ag08a_ioctl_set_analog_input_t)
#define EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT _IOW(APT_USBTRX_IOC_TYPE, 0x24, ep1_ag08a_ioctl_control_analog_input_t)
#define EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT _IOW(APT_USBTRX_IOC_TYPE, 0x25, ep1_ag08a_ioctl_set_analog_output_t)
#define EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT _IOW(APT_USBTRX_IOC_TYPE, 0x26, ep1_ag08a_ioctl_control_analog_output_t)

#define PA_AGU081_IOCTL_GET_STATUS EP1_AG08A_IOCTL_GET_STATUS
#define PA_AGU081_IOCTL_SET_ANALOG_INPUT EP1_AG08A_IOCTL_SET_ANALOG_INPUT
#define PA_AGU081_IOCTL_CONTROL_ANALOG_INPUT EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT
#define PA_AGU081_IOCTL_SET_ANALOG_OUTPUT EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT
#define PA_AGU081_IOCTL_CONTROL_ANALOG_OUTPUT EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT

#define EP1_CH02A_IOCTL_GET_STATUS _IOR(APT_USBTRX_IOC_TYPE, 0x12, ep1_ch02a_ioctl_get_status_t)
#define EP1_CH02A_IOCTL_SET_MODE APT_USBTRX_IOCTL_SET_MODE
#define EP1_CH02A_IOCTL_SET_BIT_TIMING _IOW(APT_USBTRX_IOC_TYPE, 0x29, ep1_ch02a_ioctl_set_bit_timing_t)
#define EP1_CH02A_IOCTL_GET_BIT_TIMING _IOR(APT_USBTRX_IOC_TYPE, 0x30, ep1_ch02a_ioctl_get_bit_timing_t)
#define EP1_CH02A_IOCTL_RESET_CAN_SUMMARY APT_USBTRX_IOCTL_RESET_CAN_SUMMARY
#define EP1_CH02A_IOCTL_START_CAN APT_USBTRX_IOCTL_START_CAN
#define EP1_CH02A_IOCTL_STOP_CAN APT_USBTRX_IOCTL_STOP_CAN

#define EP1_CF02A_IOCTL_GET_SILENT_MODE _IOR(APT_USBTRX_IOC_TYPE, 0x31, ep1_cf02a_ioctl_get_silent_mode_t)
#define EP1_CF02A_IOCTL_SET_SILENT_MODE _IOW(APT_USBTRX_IOC_TYPE, 0x32, ep1_cf02a_ioctl_set_silent_mode_t)
#define EP1_CF02A_IOCTL_GET_ISO_MODE _IOR(APT_USBTRX_IOC_TYPE, 0x33, ep1_cf02a_ioctl_get_iso_mode_t)
#define EP1_CF02A_IOCTL_SET_ISO_MODE _IOW(APT_USBTRX_IOC_TYPE, 0x34, ep1_cf02a_ioctl_set_iso_mode_t)
#define EP1_CF02A_IOCTL_GET_BITRATE _IOR(APT_USBTRX_IOC_TYPE, 0x35, ep1_cf02a_ioctl_get_bitrate_t)
#define EP1_CF02A_IOCTL_SET_BITRATE _IOW(APT_USBTRX_IOC_TYPE, 0x36, ep1_cf02a_ioctl_set_bitrate_t)
#define EP1_CF02A_IOCTL_GET_DATA_BITRATE _IOR(APT_USBTRX_IOC_TYPE, 0x37, ep1_cf02a_ioctl_get_data_bitrate_t)
#define EP1_CF02A_IOCTL_SET_DATA_BITRATE _IOW(APT_USBTRX_IOC_TYPE, 0x38, ep1_cf02a_ioctl_set_data_bitrate_t)
#define EP1_CF02A_IOCTL_GET_BIT_TIMING _IOR(APT_USBTRX_IOC_TYPE, 0x39, ep1_cf02a_ioctl_get_bit_timing_t)
#define EP1_CF02A_IOCTL_SET_BIT_TIMING _IOW(APT_USBTRX_IOC_TYPE, 0x3a, ep1_cf02a_ioctl_set_bit_timing_t)
#define EP1_CF02A_IOCTL_GET_DATA_BIT_TIMING _IOR(APT_USBTRX_IOC_TYPE, 0x3b, ep1_cf02a_ioctl_get_data_bit_timing_t)
#define EP1_CF02A_IOCTL_SET_DATA_BIT_TIMING _IOW(APT_USBTRX_IOC_TYPE, 0x3c, ep1_cf02a_ioctl_set_data_bit_timing_t)
#define EP1_CF02A_IOCTL_GET_TX_RX_CONTROL _IOR(APT_USBTRX_IOC_TYPE, 0x3d, ep1_cf02a_ioctl_get_tx_rx_control_t)
#define EP1_CF02A_IOCTL_SET_TX_RX_CONTROL _IOW(APT_USBTRX_IOC_TYPE, 0x3e, ep1_cf02a_ioctl_set_tx_rx_control_t)
#define EP1_CF02A_IOCTL_RESET_CAN_SUMMARY APT_USBTRX_IOCTL_RESET_CAN_SUMMARY
#define EP1_CF02A_IOCTL_GET_DEVICE_TIMESTAMP_RESET_TIME                                                                \
	_IOR(APT_USBTRX_IOC_TYPE, 0x3f, ep1_cf02a_ioctl_get_device_timestamp_reset_time_t)
#define EP1_CF02A_IOCTL_SET_HOST_TIMESTAMP_RESET_TIME _IO(APT_USBTRX_IOC_TYPE, 0x40)
#define EP1_CF02A_IOCTL_GET_RTC_TIME _IOR(APT_USBTRX_IOC_TYPE, 0x41, ep1_cf02a_ioctl_get_rtc_time_t)
#define EP1_CF02A_IOCTL_SET_RTC_TIME _IO(APT_USBTRX_IOC_TYPE, 0x42)
#define EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE                                                                   \
	_IOR(APT_USBTRX_IOC_TYPE, 0x43, ep1_cf02a_ioctl_get_current_store_data_state_t)
#define EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT                                                                   \
	_IOR(APT_USBTRX_IOC_TYPE, 0x44, ep1_cf02a_ioctl_get_store_data_id_list_count_t)
#define EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST _IOWR(APT_USBTRX_IOC_TYPE, 0x45, ep1_cf02a_ioctl_get_store_data_id_list_t)
#define EP1_CF02A_IOCTL_GET_STORE_DATA_META _IOWR(APT_USBTRX_IOC_TYPE, 0x46, ep1_cf02a_ioctl_get_store_data_meta_t)
#define EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL                                                                      \
	_IOWR(APT_USBTRX_IOC_TYPE, 0x47, ep1_cf02a_ioctl_get_store_data_rx_control_t)
#define EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL                                                                      \
	_IOW(APT_USBTRX_IOC_TYPE, 0x48, ep1_cf02a_ioctl_set_store_data_rx_control_t)
#define EP1_CF02A_IOCTL_READ_STORE_DATA _IOWR(APT_USBTRX_IOC_TYPE, 0x49, ep1_cf02a_ioctl_read_store_data_t)
#define EP1_CF02A_IOCTL_DELETE_STORE_DATA _IOW(APT_USBTRX_IOC_TYPE, 0x4a, ep1_cf02a_ioctl_delete_store_data_t)
#define EP1_CF02A_IOCTL_INIT_STORE_DATA_MEDIA _IO(APT_USBTRX_IOC_TYPE, 0x4b)
#define EP1_CF02A_IOCTL_GET_FD_MODE _IOR(APT_USBTRX_IOC_TYPE, 0x4c, ep1_cf02a_ioctl_get_fd_mode_t)
#define EP1_CF02A_IOCTL_SET_FD_MODE _IOW(APT_USBTRX_IOC_TYPE, 0x4d, ep1_cf02a_ioctl_set_fd_mode_t)

#endif /* __APT_USBTRX_FOPS_DEF_H__ */
