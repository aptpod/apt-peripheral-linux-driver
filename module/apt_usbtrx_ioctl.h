/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EDGEPLANT USB Peripherals Device Driver for Linux.
 *
 * Copyright (C) 2018 aptpod Inc.
 */
#ifndef __APT_USBTRX_FOPS_DEF_H__
#define __APT_USBTRX_FOPS_DEF_H__

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

#endif /* __APT_USBTRX_FOPS_DEF_H__ */
