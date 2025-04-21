// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/can/netlink.h>

#include "../ap_ct2a/ap_ct2a_cmd.h" /* apt_usbtrx_reset_can_summary() */
#include "ep1_cf02a_fops.h"
#include "ep1_cf02a_cmd_def.h"
#include "ep1_cf02a_cmd.h"
#include "ep1_cf02a_def.h"

/*!
 * @brief get read-payload size
 */
int ep1_cf02a_get_read_payload_size(const void *payload)
{
	if (payload == NULL) {
		return -1;
	}

	/* EP1-CF02A returns a fixed size */
	return sizeof(ep1_cf02a_payload_notify_recv_can_frame_t);
}

/*!
 * @brief get write-payload size
 */
int ep1_cf02a_get_write_payload_size(const void *payload)
{
	if (payload == NULL) {
		return -1;
	}

	/* EP1-CF02A returns a fixed size */
	return sizeof(ep1_cf02a_payload_send_can_frame_t);
}

/*!
 * @brief get read-payload timestamp
 */
apt_usbtrx_timestamp_t *ep1_cf02a_get_read_payload_timestamp(const void *payload)
{
	ep1_cf02a_payload_notify_recv_can_frame_t *read_payload = NULL;

	if (payload == NULL) {
		return NULL;
	}

	read_payload = (ep1_cf02a_payload_notify_recv_can_frame_t *)payload;

	return &read_payload->timestamp;
}

/*!
 * @brief get write cmd id
 */
int ep1_cf02a_get_write_cmd_id(void)
{
	return EP1_CF02A_CMD_SendCANFrame;
}

/*!
 * @brief get fw size
 */
int ep1_cf02a_get_fw_size(void)
{
	return EP1_CF02A_FW_DATA_SIZE;
}

/*!
 * @brief ioctl - get silent mode
 */
static long ep1_cf02a_ioctl_get_silent_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_silent_mode_t param;
	ep1_cf02a_msg_get_silent_mode_t mode;
	int result;

	result = ep1_cf02a_get_silent_mode(dev, &mode);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_silent_mode().. Error");
		return -EIO;
	}

	param.silent = mode.silent;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_silent_mode_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set silent mode
 */
static long ep1_cf02a_ioctl_set_silent_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_silent_mode_t param;
	ep1_cf02a_msg_set_silent_mode_t mode;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_silent_mode_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	mode.silent = param.silent;

	result = ep1_cf02a_set_silent_mode(dev, &mode, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_silent_mode().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_silent_mode().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get fd mode
 */
static long ep1_cf02a_ioctl_get_fd_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_fd_mode_t param;
	ep1_cf02a_msg_get_fd_mode_t mode;
	int result;

	result = ep1_cf02a_get_fd_mode(dev, &mode);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_fd_mode().. Error");
		return -EIO;
	}

	param.fd = mode.fd;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_fd_mode_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set fd mode
 */
static long ep1_cf02a_ioctl_set_fd_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_fd_mode_t param;
	ep1_cf02a_msg_set_fd_mode_t mode;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_fd_mode_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	mode.fd = param.fd;

	result = ep1_cf02a_set_fd_mode(dev, &mode, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_fd_mode().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_fd_mode().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get iso mode
 */
static long ep1_cf02a_ioctl_get_iso_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_iso_mode_t param;
	ep1_cf02a_msg_get_iso_mode_t mode;
	int result;

	result = ep1_cf02a_get_iso_mode(dev, &mode);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_iso_mode().. Error");
		return -EIO;
	}

	param.non_iso_mode = mode.non_iso_mode;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_iso_mode_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set iso mode
 */
static long ep1_cf02a_ioctl_set_iso_mode(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_iso_mode_t param;
	ep1_cf02a_msg_set_iso_mode_t mode;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_iso_mode_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	mode.non_iso_mode = param.non_iso_mode;

	result = ep1_cf02a_set_iso_mode(dev, &mode, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_iso_mode().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_iso_mode().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief calc bitrate
 */
static int ep1_cf02a_calc_bitrate(apt_usbtrx_dev_t *dev, ep1_cf02a_msg_get_bit_timing_t *timing, int *bitrate,
				  int *sample_point)
{
	ep1_cf02a_msg_get_can_clock_t can_clock;
	int result;

	int sync_seg = EP1_CF02A_CAN_SYNC_SEG;
	int prop_seg = timing->prop_seg;
	int phase_seg1 = timing->phase_seg1;
	int phase_seg2 = timing->phase_seg2;
	int brp = timing->brp;
	int all_tseg = sync_seg + prop_seg + phase_seg1 + phase_seg2;

	result = ep1_cf02a_get_can_clock(dev, &can_clock);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_data_bit_timing().. Error");
		return RESULT_Failure;
	}

	*bitrate = can_clock.can_clock / (brp * all_tseg);
	*sample_point = (1000 * (sync_seg + prop_seg + phase_seg1)) / all_tseg;

	DMSG("Calculate Bitrate: [IN] can_clock: %d, prop_seg: %d, phase_seg1: %d, phase_seg2: %d, brp: %d",
	     can_clock.can_clock, prop_seg, phase_seg1, phase_seg2, brp);
	DMSG("Calculate Bitrate: [OUT] bitrate: %d, sample_point: %d", *bitrate, *sample_point);

	return RESULT_Success;
}

/*!
 * @note These functions for bit timing calculations are based on the Linux kernel.
 */
static void ep1_cf02a_can_sjw_set_default(struct can_bittiming *bt)
{
	if (bt->sjw)
		return;

	/* If user space provides no sjw, use sane default of phase_seg2 / 2 */
	bt->sjw = max(1U, min(bt->phase_seg1, bt->phase_seg2 / 2));
}

static int ep1_cf02a_can_sjw_check(const struct can_bittiming *bt, const struct can_bittiming_const *btc)
{
	if (bt->sjw > btc->sjw_max) {
		EMSG("sjw: %u greater than max sjw: %u", bt->sjw, btc->sjw_max);
		return -EINVAL;
	}

	if (bt->sjw > bt->phase_seg1) {
		EMSG("sjw: %u greater than phase-seg1: %u", bt->sjw, bt->phase_seg1);
		return -EINVAL;
	}

	if (bt->sjw > bt->phase_seg2) {
		EMSG("sjw: %u greater than phase-seg2: %u", bt->sjw, bt->phase_seg2);
		return -EINVAL;
	}

	return 0;
}

static inline unsigned int ep1_cf02a_can_bit_time(const struct can_bittiming *bt)
{
	return EP1_CF02A_CAN_SYNC_SEG + bt->prop_seg + bt->phase_seg1 + bt->phase_seg2;
}

/*
 * Bit-timing calculation derived from:
 *
 * Code based on LinCAN sources and H8S2638 project
 * Copyright 2004-2006 Pavel Pisa - DCE FELK CVUT cz
 * Copyright 2005      Stanislav Marek
 * email: pisa@cmp.felk.cvut.cz
 *
 * Calculates proper bit-timing parameters for a specified bit-rate
 * and sample-point, which can then be used to set the bit-timing
 * registers of the CAN controller. You can find more information
 * in the header file linux/can/netlink.h.
 */
static int ep1_cf02a_can_update_sample_point(const struct can_bittiming_const *btc,
					     const unsigned int sample_point_nominal, const unsigned int tseg,
					     unsigned int *tseg1_ptr, unsigned int *tseg2_ptr,
					     unsigned int *sample_point_error_ptr)
{
	unsigned int sample_point_error, best_sample_point_error = UINT_MAX;
	unsigned int sample_point, best_sample_point = 0;
	unsigned int tseg1, tseg2;
	int i;

	for (i = 0; i <= 1; i++) {
		tseg2 = tseg + EP1_CF02A_CAN_SYNC_SEG -
			(sample_point_nominal * (tseg + EP1_CF02A_CAN_SYNC_SEG)) / 1000 - i;
		tseg2 = clamp(tseg2, btc->tseg2_min, btc->tseg2_max);
		tseg1 = tseg - tseg2;
		if (tseg1 > btc->tseg1_max) {
			tseg1 = btc->tseg1_max;
			tseg2 = tseg - tseg1;
		}

		sample_point = 1000 * (tseg + EP1_CF02A_CAN_SYNC_SEG - tseg2) / (tseg + EP1_CF02A_CAN_SYNC_SEG);
		sample_point_error = abs(sample_point_nominal - sample_point);

		if (sample_point <= sample_point_nominal && sample_point_error < best_sample_point_error) {
			best_sample_point = sample_point;
			best_sample_point_error = sample_point_error;
			*tseg1_ptr = tseg1;
			*tseg2_ptr = tseg2;
		}
	}

	if (sample_point_error_ptr)
		*sample_point_error_ptr = best_sample_point_error;

	return best_sample_point;
}

static int ep1_cf02a_can_calc_bittiming(u32 clock_freq, struct can_bittiming *bt, const struct can_bittiming_const *btc)
{
	unsigned int bitrate; /* current bitrate */
	unsigned int bitrate_error; /* difference between current and nominal value */
	unsigned int best_bitrate_error = UINT_MAX;
	unsigned int sample_point_error; /* difference between current and nominal value */
	unsigned int best_sample_point_error = UINT_MAX;
	unsigned int sample_point_nominal; /* nominal sample point */
	unsigned int best_tseg = 0; /* current best value for tseg */
	unsigned int best_brp = 0; /* current best value for brp */
	unsigned int brp, tsegall, tseg, tseg1 = 0, tseg2 = 0;
	u64 v64;
	int err;

	/* Use CiA recommended sample points */
	if (bt->sample_point) {
		sample_point_nominal = bt->sample_point;
	} else {
		if (bt->bitrate > 800000)
			sample_point_nominal = 750;
		else if (bt->bitrate > 500000)
			sample_point_nominal = 800;
		else
			sample_point_nominal = 875;
	}

	/* tseg even = round down, odd = round up */
	for (tseg = (btc->tseg1_max + btc->tseg2_max) * 2 + 1; tseg >= (btc->tseg1_min + btc->tseg2_min) * 2; tseg--) {
		tsegall = EP1_CF02A_CAN_SYNC_SEG + tseg / 2;

		/* Compute all possible tseg choices (tseg=tseg1+tseg2) */
		brp = clock_freq / (tsegall * bt->bitrate) + tseg % 2;

		/* choose brp step which is possible in system */
		brp = (brp / btc->brp_inc) * btc->brp_inc;
		if (brp < btc->brp_min || brp > btc->brp_max)
			continue;

		bitrate = clock_freq / (brp * tsegall);
		bitrate_error = abs(bt->bitrate - bitrate);

		/* tseg brp biterror */
		if (bitrate_error > best_bitrate_error)
			continue;

		/* reset sample point error if we have a better bitrate */
		if (bitrate_error < best_bitrate_error)
			best_sample_point_error = UINT_MAX;

		ep1_cf02a_can_update_sample_point(btc, sample_point_nominal, tseg / 2, &tseg1, &tseg2,
						  &sample_point_error);
		if (sample_point_error >= best_sample_point_error)
			continue;

		best_sample_point_error = sample_point_error;
		best_bitrate_error = bitrate_error;
		best_tseg = tseg / 2;
		best_brp = brp;

		if (bitrate_error == 0 && sample_point_error == 0)
			break;
	}

	if (best_bitrate_error) {
		/* Error in one-tenth of a percent */
		v64 = (u64)best_bitrate_error * 1000;
		do_div(v64, bt->bitrate);
		bitrate_error = (u32)v64;
		if (bitrate_error > EP1_CF02A_CAN_CALC_MAX_ERROR) {
			EMSG("bitrate error: %u.%u%% too high", bitrate_error / 10, bitrate_error % 10);
			return -EINVAL;
		}
		WMSG("bitrate error: %u.%u%%", bitrate_error / 10, bitrate_error % 10);
	}

	/* real sample point */
	bt->sample_point =
		ep1_cf02a_can_update_sample_point(btc, sample_point_nominal, best_tseg, &tseg1, &tseg2, NULL);

	v64 = (u64)best_brp * 1000 * 1000 * 1000;
	do_div(v64, clock_freq);
	bt->tq = (u32)v64;
	bt->prop_seg = tseg1 / 2;
	bt->phase_seg1 = tseg1 - bt->prop_seg;
	bt->phase_seg2 = tseg2;

	ep1_cf02a_can_sjw_set_default(bt);

	err = ep1_cf02a_can_sjw_check(bt, btc);
	if (err)
		return err;

	bt->brp = best_brp;

	/* real bitrate */
	bt->bitrate = clock_freq / (bt->brp * ep1_cf02a_can_bit_time(bt));

	return 0;
}

/*!
 * @brief calc bit timing
 */
static int ep1_cf02a_calc_bit_timing(apt_usbtrx_dev_t *dev, int bitrate, int sample_point,
				     ep1_cf02a_msg_bit_timing_t *timing)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	ep1_cf02a_msg_get_can_clock_t can_clock;
	struct can_bittiming bt;
	int result;
	int err;

	result = ep1_cf02a_get_can_clock(dev, &can_clock);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_data_bit_timing().. Error");
		return RESULT_Failure;
	}

	bt.bitrate = bitrate;
	bt.sample_point = sample_point;

	err = ep1_cf02a_can_calc_bittiming(can_clock.can_clock, &bt, unique_data->bittiming_const);
	if (err) {
		EMSG("ep1_cf02a_can_calc_bittiming().. Error");
		return RESULT_Failure;
	}

	timing->prop_seg = bt.prop_seg;
	timing->phase_seg1 = bt.phase_seg1;
	timing->phase_seg2 = bt.phase_seg2;
	timing->sjw = bt.sjw;
	timing->brp = bt.brp;

	DMSG("Calculate Bit Timing: [IN] bitrate: %d, sample_point: %d", bitrate, sample_point);
	DMSG("Calculate Bit Timing: [OUT] prop_seg: %d, phase_seg1: %d, phase_seg2: %d, sjw: %d, brp: %d",
	     timing->prop_seg, timing->phase_seg1, timing->phase_seg2, timing->sjw, timing->brp);

	return RESULT_Success;
}

/*!
 * @brief ioctl - get bitrate
 */
static long ep1_cf02a_ioctl_get_bitrate(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_bitrate_t param;
	ep1_cf02a_msg_get_bit_timing_t timing;
	int result;

	result = ep1_cf02a_get_bit_timing(dev, &timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_bit_timing().. Error");
		return -EIO;
	}

	result = ep1_cf02a_calc_bitrate(dev, (ep1_cf02a_msg_get_bit_timing_t *)&timing, &param.bitrate,
					&param.sample_point);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_calc_bitrate().. Error");
		return -EFAULT;
	}

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_bitrate_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set bitrate
 */
static long ep1_cf02a_ioctl_set_bitrate(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_bitrate_t param;
	ep1_cf02a_msg_set_bit_timing_t timing;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_bitrate_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	result = ep1_cf02a_calc_bit_timing(dev, param.bitrate, param.sample_point,
					   (ep1_cf02a_msg_bit_timing_t *)&timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_calc_bit_timing().. Error");
		return -EFAULT;
	}

	result = ep1_cf02a_set_bit_timing(dev, &timing, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_bit_timing().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_bit_timing().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get data bitrate
 */
static long ep1_cf02a_ioctl_get_data_bitrate(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_data_bitrate_t param;
	ep1_cf02a_msg_get_data_bit_timing_t timing;
	int result;

	result = ep1_cf02a_get_data_bit_timing(dev, &timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_data_bit_timing().. Error");
		return -EIO;
	}

	result = ep1_cf02a_calc_bitrate(dev, (ep1_cf02a_msg_get_bit_timing_t *)&timing, &param.bitrate,
					&param.sample_point);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_calc_bitrate().. Error");
		return -EFAULT;
	}

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_data_bitrate_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set data bitrate
 */
static long ep1_cf02a_ioctl_set_data_bitrate(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_data_bitrate_t param;
	ep1_cf02a_msg_set_data_bit_timing_t timing;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_data_bitrate_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	result = ep1_cf02a_calc_bit_timing(dev, param.bitrate, param.sample_point,
					   (ep1_cf02a_msg_bit_timing_t *)&timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_calc_bit_timing().. Error");
		return -EFAULT;
	}

	result = ep1_cf02a_set_data_bit_timing(dev, &timing, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_data_bit_timing().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_data_bit_timing().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get bit timing
 */
static long ep1_cf02a_ioctl_get_bit_timing(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_bit_timing_t param;
	ep1_cf02a_msg_get_bit_timing_t timing;
	int result;

	result = ep1_cf02a_get_bit_timing(dev, &timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_bit_timing().. Error");
		return -EIO;
	}

	param.prop_seg = timing.prop_seg;
	param.phase_seg1 = timing.phase_seg1;
	param.phase_seg2 = timing.phase_seg2;
	param.sjw = timing.sjw;
	param.brp = timing.brp;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_bit_timing_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set bit timing
 */
static long ep1_cf02a_ioctl_set_bit_timing(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_bit_timing_t param;
	ep1_cf02a_msg_set_bit_timing_t timing;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_bit_timing_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	timing.prop_seg = param.prop_seg;
	timing.phase_seg1 = param.phase_seg1;
	timing.phase_seg2 = param.phase_seg2;
	timing.sjw = param.sjw;
	timing.brp = param.brp;

	result = ep1_cf02a_set_bit_timing(dev, &timing, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_bit_timing().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_bit_timing().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get data bit timing
 */
static long ep1_cf02a_ioctl_get_data_bit_timing(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_data_bit_timing_t param;
	ep1_cf02a_msg_get_data_bit_timing_t timing;
	int result;

	result = ep1_cf02a_get_data_bit_timing(dev, &timing);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_data_bit_timing().. Error");
		return -EIO;
	}

	param.prop_seg = timing.prop_seg;
	param.phase_seg1 = timing.phase_seg1;
	param.phase_seg2 = timing.phase_seg2;
	param.sjw = timing.sjw;
	param.brp = timing.brp;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_data_bit_timing_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set data bit timing
 */
static long ep1_cf02a_ioctl_set_data_bit_timing(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_data_bit_timing_t param;
	ep1_cf02a_msg_set_data_bit_timing_t timing;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_data_bit_timing_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	timing.prop_seg = param.prop_seg;
	timing.phase_seg1 = param.phase_seg1;
	timing.phase_seg2 = param.phase_seg2;
	timing.sjw = param.sjw;
	timing.brp = param.brp;

	result = ep1_cf02a_set_data_bit_timing(dev, &timing, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_data_bit_timing().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_data_bit_timing().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get tx rx control
 */
static long ep1_cf02a_ioctl_get_tx_rx_control(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_tx_rx_control_t param;
	ep1_cf02a_msg_get_tx_rx_control_t control;
	int result;

	result = ep1_cf02a_get_tx_rx_control(dev, &control);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_tx_rx_control().. Error");
		return -EIO;
	}

	param.start = control.start;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_tx_rx_control_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set tx rx control
 */
static long ep1_cf02a_ioctl_set_tx_rx_control(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_set_tx_rx_control_t param;
	ep1_cf02a_msg_set_tx_rx_control_t control;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_tx_rx_control_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	control.start = param.start;

	if (control.start) {
		result = apt_usbtrx_ringbuffer_clear(&dev->rx_data);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_ringbuffer_clear().. Error");
			return -EIO;
		}
	}

	/* wait is added to ensure that the written CAN frame is transmitted. */
	if (!control.start) {
		msleep(10);
	}

	result = ep1_cf02a_set_tx_rx_control(dev, &control, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_tx_rx_control().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_tx_rx_control().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - reset can summary
 */
static long ep1_cf02a_ioctl_reset_can_summary(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	int result;
	bool success;

	result = apt_usbtrx_reset_can_summary(dev, &success);
	if (result != RESULT_Success) {
		EMSG("apt_usbtrx_reset_can_summary().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("apt_usbtrx_reset_can_summary().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get device timestamp reset time
 */
static long ep1_cf02a_ioctl_get_device_timestamp_reset_time(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_device_timestamp_reset_time_t param;
	ep1_cf02a_msg_get_device_timestamp_reset_time_t time;
	int result;

	result = ep1_cf02a_get_device_timestamp_reset_time(dev, &time);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_device_timestamp_reset_time().. Error");
		return -EIO;
	}

	param.ts.tv_sec = time.ts.ts_sec;
	param.ts.tv_nsec = time.ts.ts_usec * 1000;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_device_timestamp_reset_time_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set host timestamp reset time
 */
static long ep1_cf02a_ioctl_set_host_timestamp_reset_time(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_msg_set_host_timestamp_reset_time_t time;
	int result;
	bool success;

	time.ts.tv_sec = dev->basetime.tv_sec;
	time.ts.tv_nsec = dev->basetime.tv_nsec;

	result = ep1_cf02a_set_host_timestamp_reset_time(dev, &time, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_host_timestamp_reset_time().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_host_timestamp_reset_time().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get rtc time
 */
static long ep1_cf02a_ioctl_get_rtc_time(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_rtc_time_t param;
	ep1_cf02a_msg_get_rtc_time_t time;
	int result;

	result = ep1_cf02a_get_rtc_time(dev, &time);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_rtc_time().. Error");
		return -EIO;
	}

	param.weekday = time.weekday;
	param.month = time.month;
	param.date = time.date;
	param.year = time.year;
	param.hour = time.hour;
	param.minute = time.minute;
	param.second = time.second;
	param.microsecond = time.microsecond;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_rtc_time_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
// Copied from kernel v5.14 to support older kernels
// https://github.com/torvalds/linux/blob/master/kernel/time/timeconv.c

// SPDX-License-Identifier: LGPL-2.0+
/*
 * Copyright (C) 1993, 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 * Contributed by Paul Eggert (eggert@twinsun.com).
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU C Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Converts the calendar time to broken-down time representation
 *
 * 2009-7-14:
 *   Moved from glibc-2.6 to kernel by Zhaolei<zhaolei@cn.fujitsu.com>
 * 2021-06-02:
 *   Reimplemented by Cassio Neri <cassio.neri@gmail.com>
 */

#include <linux/time.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define SECS_PER_HOUR	(60 * 60)
#define SECS_PER_DAY	(SECS_PER_HOUR * 24)

/**
 * time64_to_tm - converts the calendar time to local broken-down time
 *
 * @totalsecs:	the number of seconds elapsed since 00:00:00 on January 1, 1970,
 *		Coordinated Universal Time (UTC).
 * @offset:	offset seconds adding to totalsecs.
 * @result:	pointer to struct tm variable to receive broken-down time
 */
void time64_to_tm(time64_t totalsecs, int offset, struct tm *result)
{
	u32 u32tmp, day_of_century, year_of_century, day_of_year, month, day;
	u64 u64tmp, udays, century, year;
	bool is_Jan_or_Feb, is_leap_year;
	long days, rem;
	int remainder;

	days = div_s64_rem(totalsecs, SECS_PER_DAY, &remainder);
	rem = remainder;
	rem += offset;
	while (rem < 0) {
		rem += SECS_PER_DAY;
		--days;
	}
	while (rem >= SECS_PER_DAY) {
		rem -= SECS_PER_DAY;
		++days;
	}

	result->tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	result->tm_min = rem / 60;
	result->tm_sec = rem % 60;

	/* January 1, 1970 was a Thursday. */
	result->tm_wday = (4 + days) % 7;
	if (result->tm_wday < 0)
		result->tm_wday += 7;

	/*
	 * The following algorithm is, basically, Proposition 6.3 of Neri
	 * and Schneider [1]. In a few words: it works on the computational
	 * (fictitious) calendar where the year starts in March, month = 2
	 * (*), and finishes in February, month = 13. This calendar is
	 * mathematically convenient because the day of the year does not
	 * depend on whether the year is leap or not. For instance:
	 *
	 * March 1st		0-th day of the year;
	 * ...
	 * April 1st		31-st day of the year;
	 * ...
	 * January 1st		306-th day of the year; (Important!)
	 * ...
	 * February 28th	364-th day of the year;
	 * February 29th	365-th day of the year (if it exists).
	 *
	 * After having worked out the date in the computational calendar
	 * (using just arithmetics) it's easy to convert it to the
	 * corresponding date in the Gregorian calendar.
	 *
	 * [1] "Euclidean Affine Functions and Applications to Calendar
	 * Algorithms". https://arxiv.org/abs/2102.06959
	 *
	 * (*) The numbering of months follows tm more closely and thus,
	 * is slightly different from [1].
	 */

	udays	= ((u64) days) + 2305843009213814918ULL;

	u64tmp		= 4 * udays + 3;
	century		= div64_u64_rem(u64tmp, 146097, &u64tmp);
	day_of_century	= (u32) (u64tmp / 4);

	u32tmp		= 4 * day_of_century + 3;
	u64tmp		= 2939745ULL * u32tmp;
	year_of_century	= upper_32_bits(u64tmp);
	day_of_year	= lower_32_bits(u64tmp) / 2939745 / 4;

	year		= 100 * century + year_of_century;
	is_leap_year	= year_of_century ? !(year_of_century % 4) : !(century % 4);

	u32tmp		= 2141 * day_of_year + 132377;
	month		= u32tmp >> 16;
	day		= ((u16) u32tmp) / 2141;

	/*
	 * Recall that January 1st is the 306-th day of the year in the
	 * computational (not Gregorian) calendar.
	 */
	is_Jan_or_Feb	= day_of_year >= 306;

	/* Convert to the Gregorian calendar and adjust to Unix time. */
	year		= year + is_Jan_or_Feb - 6313183731940000ULL;
	month		= is_Jan_or_Feb ? month - 12 : month;
	day		= day + 1;
	day_of_year	+= is_Jan_or_Feb ? -306 : 31 + 28 + is_leap_year;

	/* Convert to tm's format. */
	result->tm_year = (long) (year - 1900);
	result->tm_mon  = (int) month;
	result->tm_mday = (int) day;
	result->tm_yday = (int) day_of_year;
}
#endif

/*!
 * @brief get_real_rtc_time
 */
static void get_real_rtc_time(ep1_cf02a_msg_set_rtc_time_t *rtc_time)
{
    struct timespec64 ts;
    struct tm tm;

    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);

    rtc_time->year = (u8)((tm.tm_year + 1900) - 2000);
    rtc_time->month = (u8)(tm.tm_mon + 1);
    rtc_time->date = (u8)(tm.tm_mday);

    if (tm.tm_wday == 0)
        rtc_time->weekday = (u8)7;
    else
        rtc_time->weekday = (u8)(tm.tm_wday);

    rtc_time->hour = (u8)(tm.tm_hour);
    rtc_time->minute = (u8)(tm.tm_min);
    rtc_time->second = (u8)(tm.tm_sec);
    rtc_time->microsecond = (u32)(ts.tv_nsec / 1000);
}

/*!
 * @brief ioctl - set rtc time
 */
static int ep1_cf02a_ioctl_set_rtc_time(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_msg_set_rtc_time_t time;
	int result;
	bool success;

	get_real_rtc_time((ep1_cf02a_msg_set_rtc_time_t *)&time);

	result = ep1_cf02a_set_rtc_time(dev, &time, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_rtc_time().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_rtc_time().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - get current store data state
 */
static long ep1_cf02a_ioctl_get_current_store_data_state(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_current_store_data_state_t param;
	ep1_cf02a_msg_get_current_store_data_state_t state;
	int result;

	result = ep1_cf02a_get_current_store_data_state(dev, &state);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_current_store_data_state().. Error");
		return -EIO;
	}

	param.state = state.state;
	memcpy(param.id, state.id, sizeof(param.id));

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_current_store_data_state_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - get store data id list count
 */
static long ep1_cf02a_ioctl_get_store_data_id_list_count(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_store_data_id_list_count_t param;
	ep1_cf02a_msg_get_store_data_id_list_count_t count;
	int result;

	result = ep1_cf02a_get_store_data_id_list_count(dev, &count);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_store_data_id_list_count().. Error");
		return -EIO;
	}

	param.count = count.count;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_store_data_id_list_count_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - get store data id list
 */
static long ep1_cf02a_ioctl_get_store_data_id_list(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_store_data_id_list_t param;
	ep1_cf02a_msg_get_store_data_id_list_t id_list;
	ep1_cf02a_msg_get_store_data_id_request_t id_req;
	ep1_cf02a_msg_get_store_data_id_response_t id_res;
	int result, i;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_get_store_data_id_list_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	id_list.count = param.count;
	id_list.id_list = kzalloc(EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH * id_list.count, GFP_KERNEL);
	if (id_list.id_list == NULL) {
		EMSG("kzalloc().. Error");
		return -ENOMEM;
	}

	for (i = 0; i < param.count; i++) {
		id_req.index = i;
		result = ep1_cf02a_get_store_data_id(dev, &id_req, &id_res);
		if (result != RESULT_Success) {
			EMSG("ep1_cf02a_get_store_data_id().. Error. index:%d", i);
			kfree(id_list.id_list);
			return -EIO;
		}

		strncpy(id_list.id_list[i], id_res.id, EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH);
	}

	result = copy_to_user((void __user *)param.id_list, id_list.id_list,
			      EP1_CF02A_CMD_STORE_DATA_ID_MAX_LENGTH * id_list.count);
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		kfree(id_list.id_list);
		return -EFAULT;
	}

	kfree(id_list.id_list);

	return 0;
}

/*!
 * @brief ioctl - get store data meta
 */
static long ep1_cf02a_ioctl_get_store_data_meta(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_store_data_meta_t param;
	ep1_cf02a_msg_get_store_data_meta_request_t meta_req;
	ep1_cf02a_msg_get_store_data_meta_response_t meta_res;
	int result;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_get_store_data_meta_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	memcpy(meta_req.id, param.id, sizeof(meta_req.id));

	result = ep1_cf02a_get_store_data_meta(dev, &meta_req, &meta_res);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_store_data_meta().. Error");
		return -EIO;
	}

	param.start_time.rtc.weekday = meta_res.start_time.rtc.weekday;
	param.start_time.rtc.month = meta_res.start_time.rtc.month;
	param.start_time.rtc.date = meta_res.start_time.rtc.date;
	param.start_time.rtc.year = meta_res.start_time.rtc.year;
	param.start_time.rtc.hour = meta_res.start_time.rtc.hour;
	param.start_time.rtc.minute = meta_res.start_time.rtc.minute;
	param.start_time.rtc.second = meta_res.start_time.rtc.second;
	param.start_time.rtc.microsecond = meta_res.start_time.rtc.microsecond;

	param.start_time.ts.tv_sec = meta_res.start_time.ts.ts_sec;
	param.start_time.ts.tv_nsec = meta_res.start_time.ts.ts_usec * 1000;

	param.reset_time.device_ts.tv_sec = meta_res.reset_time.device_ts.ts_sec;
	param.reset_time.device_ts.tv_nsec = meta_res.reset_time.device_ts.ts_usec * 1000;

	param.reset_time.host_monotonic.tv_sec = meta_res.reset_time.host_monotonic.tv_sec;
	param.reset_time.host_monotonic.tv_nsec = meta_res.reset_time.host_monotonic.tv_nsec;

	param.can_frame_count = meta_res.can_frame_count;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_store_data_meta_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - get store data rx control
 */
static long ep1_cf02a_ioctl_get_store_data_rx_control(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_get_store_data_rx_control_t param;
	ep1_cf02a_msg_get_store_data_rx_control_request_t control_req;
	ep1_cf02a_msg_get_store_data_rx_control_response_t control_res;
	int result;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_get_store_data_rx_control_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	memcpy(control_req.id, param.id, sizeof(control_req.id));

	result = ep1_cf02a_get_store_data_rx_control(dev, &control_req, &control_res);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_store_data_rx_control().. Error");
		return -EIO;
	}

	param.start = control_res.start;
	param.interval = control_res.interval;

	result = copy_to_user((void __user *)arg, &param, sizeof(ep1_cf02a_ioctl_get_store_data_rx_control_t));
	if (result != 0) {
		EMSG("copy_to_user().. Error");
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief ioctl - set store data rx control
 */
static long ep1_cf02a_ioctl_set_store_data_rx_control(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	ep1_cf02a_ioctl_set_store_data_rx_control_t param;
	ep1_cf02a_msg_set_store_data_rx_control_t control;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_set_store_data_rx_control_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	memcpy(control.id, param.id, sizeof(control.id));
	control.start = param.start;
	control.interval = param.interval;

	if (control.start) {
		result = apt_usbtrx_ringbuffer_clear(&unique_data->rx_store_data);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_ringbuffer_clear().. Error");
			return -EIO;
		}
	}

	result = ep1_cf02a_set_store_data_rx_control(dev, &control, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_set_store_data_rx_control().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_set_store_data_rx_control().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief rx store data is read enable
 */
static bool ep1_cf02a_rx_store_data_is_read_enable(apt_usbtrx_dev_t *dev)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return true;
	}

	if (apt_usbtrx_ringbuffer_is_empty(&unique_data->rx_store_data) != true) {
		return true;
	}

	return false;
}

/*!
 * @brief ioctl - read store data
 */
static long ep1_cf02a_ioctl_read_store_data(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	ep1_cf02a_ioctl_read_store_data_t param;
	int result;
	ssize_t rsize;
	bool onopening;
	bool onclosing;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_read_store_data_t));

	onopening = atomic_read(&dev->onopening);
	if (onopening == true) {
		IMSG("connect..., store data read cancel");
		return -ENODEV;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., store data read cancel");
		complete(&unique_data->rx_store_data_done);
		return -ESHUTDOWN;
	}

	result = wait_event_interruptible(unique_data->rx_store_data.wq,
					  ep1_cf02a_rx_store_data_is_read_enable(dev) == true);
	if (result != 0) {
		if (result != -ERESTARTSYS) {
			EMSG("wait_event_interruptible().. Error, <errno:%d>", result);
		}
		return result;
	}

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		IMSG("disconnect..., store data read cancel");
		complete(&unique_data->rx_store_data_done);
		return -ESHUTDOWN;
	}

	/* copy to user memory space */
	rsize = apt_usbtrx_ringbuffer_read(&unique_data->rx_store_data, param.buffer, param.count);
	if (rsize < 0) {
		EMSG("apt_usbtrx_ringbuffer_read().. Error");
		return -EIO;
	}

	if (onclosing == true) {
		complete(&unique_data->rx_store_data_done);
	}

	return rsize;
}

/*!
 * @brief ioctl - delete store data
 */
static long ep1_cf02a_ioctl_delete_store_data(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	ep1_cf02a_ioctl_delete_store_data_t param;
	ep1_cf02a_msg_delete_store_data_t data;
	int result;
	bool success;

	result = copy_from_user(&param, (void __user *)arg, sizeof(ep1_cf02a_ioctl_delete_store_data_t));
	if (result != 0) {
		EMSG("copy_from_user().. Error");
		return -EFAULT;
	}

	memcpy(data.id, param.id, sizeof(data.id));

	result = ep1_cf02a_delete_store_data(dev, &data, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_delete_store_data().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_delete_store_data().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl - init store data media
 */
static long ep1_cf02a_ioctl_init_store_data_media(apt_usbtrx_dev_t *dev, unsigned long arg)
{
	int result;
	bool success;

	result = ep1_cf02a_init_store_data_media(dev, &success);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_init_store_data_media().. Error");
		return -EIO;
	}
	if (success != true) {
		EMSG("ep1_cf02a_init_store_data_media().. Error, Exec failed");
		return -EIO;
	}

	return 0;
}

/*!
 * @brief ioctl
 */
long ep1_cf02a_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	apt_usbtrx_dev_t *dev = file->private_data;

	switch (cmd) {
	case EP1_CF02A_IOCTL_GET_SILENT_MODE:
		return ep1_cf02a_ioctl_get_silent_mode(dev, arg);
	case EP1_CF02A_IOCTL_SET_SILENT_MODE:
		return ep1_cf02a_ioctl_set_silent_mode(dev, arg);
	case EP1_CF02A_IOCTL_GET_FD_MODE:
		return ep1_cf02a_ioctl_get_fd_mode(dev, arg);
	case EP1_CF02A_IOCTL_SET_FD_MODE:
		return ep1_cf02a_ioctl_set_fd_mode(dev, arg);
	case EP1_CF02A_IOCTL_GET_ISO_MODE:
		return ep1_cf02a_ioctl_get_iso_mode(dev, arg);
	case EP1_CF02A_IOCTL_SET_ISO_MODE:
		return ep1_cf02a_ioctl_set_iso_mode(dev, arg);
	case EP1_CF02A_IOCTL_GET_BITRATE:
		return ep1_cf02a_ioctl_get_bitrate(dev, arg);
	case EP1_CF02A_IOCTL_SET_BITRATE:
		return ep1_cf02a_ioctl_set_bitrate(dev, arg);
	case EP1_CF02A_IOCTL_GET_DATA_BITRATE:
		return ep1_cf02a_ioctl_get_data_bitrate(dev, arg);
	case EP1_CF02A_IOCTL_SET_DATA_BITRATE:
		return ep1_cf02a_ioctl_set_data_bitrate(dev, arg);
	case EP1_CF02A_IOCTL_GET_BIT_TIMING:
		return ep1_cf02a_ioctl_get_bit_timing(dev, arg);
	case EP1_CF02A_IOCTL_SET_BIT_TIMING:
		return ep1_cf02a_ioctl_set_bit_timing(dev, arg);
	case EP1_CF02A_IOCTL_GET_DATA_BIT_TIMING:
		return ep1_cf02a_ioctl_get_data_bit_timing(dev, arg);
	case EP1_CF02A_IOCTL_SET_DATA_BIT_TIMING:
		return ep1_cf02a_ioctl_set_data_bit_timing(dev, arg);
	case EP1_CF02A_IOCTL_GET_TX_RX_CONTROL:
		return ep1_cf02a_ioctl_get_tx_rx_control(dev, arg);
	case EP1_CF02A_IOCTL_SET_TX_RX_CONTROL:
		return ep1_cf02a_ioctl_set_tx_rx_control(dev, arg);
	case EP1_CF02A_IOCTL_RESET_CAN_SUMMARY:
		return ep1_cf02a_ioctl_reset_can_summary(dev, arg);
	case EP1_CF02A_IOCTL_GET_DEVICE_TIMESTAMP_RESET_TIME:
		return ep1_cf02a_ioctl_get_device_timestamp_reset_time(dev, arg);
	case EP1_CF02A_IOCTL_SET_HOST_TIMESTAMP_RESET_TIME:
		return ep1_cf02a_ioctl_set_host_timestamp_reset_time(dev, arg);
	case EP1_CF02A_IOCTL_GET_RTC_TIME:
		return ep1_cf02a_ioctl_get_rtc_time(dev, arg);
	case EP1_CF02A_IOCTL_SET_RTC_TIME:
		return ep1_cf02a_ioctl_set_rtc_time(dev, arg);
	case EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE:
		return ep1_cf02a_ioctl_get_current_store_data_state(dev, arg);
	case EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT:
		return ep1_cf02a_ioctl_get_store_data_id_list_count(dev, arg);
	case EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST:
		return ep1_cf02a_ioctl_get_store_data_id_list(dev, arg);
	case EP1_CF02A_IOCTL_GET_STORE_DATA_META:
		return ep1_cf02a_ioctl_get_store_data_meta(dev, arg);
	case EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL:
		return ep1_cf02a_ioctl_get_store_data_rx_control(dev, arg);
	case EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL:
		return ep1_cf02a_ioctl_set_store_data_rx_control(dev, arg);
	case EP1_CF02A_IOCTL_READ_STORE_DATA:
		return ep1_cf02a_ioctl_read_store_data(dev, arg);
	case EP1_CF02A_IOCTL_DELETE_STORE_DATA:
		return ep1_cf02a_ioctl_delete_store_data(dev, arg);
	case EP1_CF02A_IOCTL_INIT_STORE_DATA_MEDIA:
		return ep1_cf02a_ioctl_init_store_data_media(dev, arg);
	default:
		EMSG("not supported, <ioctl:0x%02x>", cmd);
		return -EFAULT;
	}

	return 0;
}

/*!
 * @brief is device start
 */
int ep1_cf02a_is_device_start(apt_usbtrx_dev_t *dev, bool *start)
{
	ep1_cf02a_msg_get_tx_rx_control_t control;
	int result;

	result = ep1_cf02a_get_tx_rx_control(dev, &control);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_get_tx_rx_control().. Error");
		return RESULT_Failure;
	}

	*start = control.start;

	return RESULT_Success;
}

/*!
 * @brief open (file operation)
 */
int ep1_cf02a_open(apt_usbtrx_dev_t *dev)
{
	return 0;
}

/*!
 * @brief close (file operation)
 */
int ep1_cf02a_close(apt_usbtrx_dev_t *dev)
{
	return 0;
}
