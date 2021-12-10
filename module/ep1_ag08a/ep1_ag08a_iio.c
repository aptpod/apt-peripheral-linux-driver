// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for receiving data from EDGEPLANT ANALOG-USB interface.
 *
 * Copyright (C) 2021 aptpod Inc.
 */

#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/kfifo_buf.h>

#include "../apt_usbtrx_def.h"
#include "ep1_ag08a_def.h"
#include "ep1_ag08a_cmd.h"
#include "ep1_ag08a_fops.h"
#include "ep1_ag08a_msg.h"

enum ep1_ag08a_in_volt_range_modes_enum {
	EP1_AG08A_IN_VOLTAGE_RANGE_MODES_ENUM_PM10V,
	EP1_AG08A_IN_VOLTAGE_RANGE_MODES_ENUM_PM5V
};
enum ep1_ag08a_in_volt_samp_freq_modes_enum {
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_10K,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_5000,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_2500,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_1250,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0625,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0312_5,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0156_25,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0010,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0001,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0000_1,
	EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0000_01
};
enum ep1_ag08a_out_volt_waveform_type_modes_enum {
	EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_RANDOM,
	EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_SINE,
	EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_TRIANGLE,
	EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_SQUARE,
	EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_DC,
};

static const char *const ep1_ag08a_in_volt_range_modes[] = { "pm10v", "pm5v" };
static const char *const ep1_ag08a_in_volt_samp_freq_modes[] = { "10000.0", "5000.0", "2500.0", "1250.0",
								 "625.0",   "312.5",  "156.25", "10.0",
								 "1.0",	    "0.1",    "0.01" };
static const char *const ep1_ag08a_out_volt_waveform_type_modes[] = { "random", "sine", "triangle", "square", "dc" };
static const char *const ep1_ag08a_hw_timestamp_modes[] = { "0", "1" };

struct ep1_ag08a_modes_tbl {
	int value;
	unsigned int mode;
};

static const struct ep1_ag08a_modes_tbl ep1_ag08a_in_volt_range_modes_tbl[] = {
	{ EP1_AG08A_INPUT_VOLTAGE_MINUS_10_PLUS_10, EP1_AG08A_IN_VOLTAGE_RANGE_MODES_ENUM_PM10V },
	{ EP1_AG08A_INPUT_VOLTAGE_MINUS_5_PLUS_5, EP1_AG08A_IN_VOLTAGE_RANGE_MODES_ENUM_PM5V },
};
static const struct ep1_ag08a_modes_tbl ep1_ag08a_in_volt_samp_freq_modes_tbl[] = {
	{ 10000000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_10K },
	{ 5000000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_5000 },
	{ 2500000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_2500 },
	{ 1250000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_1250 },
	{ 625000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0625 },
	{ 312500, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0312_5 },
	{ 156250, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0156_25 },
	{ 10000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0010 },
	{ 1000, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0001 },
	{ 100, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0000_1 },
	{ 10, EP1_AG08A_IN_VOLT_SAMP_FREQ_MODES_ENUM_HZ_0000_01 },
};
static const struct ep1_ag08a_modes_tbl ep1_ag08a_out_volt_waveform_type_modes_tbl[] = {
	{ EP1_AG08A_OUTPUT_WAVEFORM_TYPE_RANDOM, EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_RANDOM },
	{ EP1_AG08A_OUTPUT_WAVEFORM_TYPE_SINE, EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_SINE },
	{ EP1_AG08A_OUTPUT_WAVEFORM_TYPE_TRIANGLE, EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_TRIANGLE },
	{ EP1_AG08A_OUTPUT_WAVEFORM_TYPE_SQUARE, EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_SQUARE },
	{ EP1_AG08A_OUTPUT_WAVEFORM_TYPE_DC, EP1_AG08A_OUT_VOLT_WAVEFORM_TYPE_MODES_ENUM_DC },
};

static int ep1_ag08a_get_in_volt_range_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ch = chan->channel;
	int ret, i, voltage;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	voltage = ep1_ag08a_msg_convert_input_voltage_to_value(status.in.cfg.vmin[ch], status.in.cfg.vmax[ch]);

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_in_volt_range_modes_tbl); ++i) {
		if (ep1_ag08a_in_volt_range_modes_tbl[i].value == voltage)
			return ep1_ag08a_in_volt_range_modes_tbl[i].mode;
	}

	return -EINVAL;
}

static int ep1_ag08a_set_in_volt_range_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan,
					    unsigned int mode)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ch = chan->channel;
	int ret, i, vmin, vmax, voltage = EP1_AG08A_INPUT_VOLTAGE_Unknown;

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_in_volt_range_modes_tbl); ++i) {
		if (ep1_ag08a_in_volt_range_modes_tbl[i].mode == mode) {
			voltage = ep1_ag08a_in_volt_range_modes_tbl[i].value;
			break;
		}
	}

	if (voltage == EP1_AG08A_INPUT_VOLTAGE_Unknown) {
		return -EIO;
	}

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	ret = ep1_ag08a_msg_convert_input_voltage_from_value(voltage, &vmin, &vmax);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	status.in.cfg.vmin[ch] = vmin;
	status.in.cfg.vmax[ch] = vmax;

	ret = ep1_ag08a_set_device_input(dev, &status.in.cfg);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	return 0;
}

static int ep1_ag08a_get_in_volt_samp_freq_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret, i;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_in_volt_samp_freq_modes_tbl); ++i) {
		if (ep1_ag08a_in_volt_samp_freq_modes_tbl[i].value == status.in.cfg.send_rate)
			return ep1_ag08a_in_volt_samp_freq_modes_tbl[i].mode;
	}

	return -EINVAL;
}

static int ep1_ag08a_set_in_volt_samp_freq_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan,
						unsigned int mode)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret, i, send_rate = EP1_AG08A_SEND_RATE_HZ_Unknown;

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_in_volt_samp_freq_modes_tbl); ++i) {
		if (ep1_ag08a_in_volt_samp_freq_modes_tbl[i].mode == mode) {
			send_rate = ep1_ag08a_in_volt_samp_freq_modes_tbl[i].value;
			break;
		}
	}

	if (send_rate == EP1_AG08A_SEND_RATE_HZ_Unknown) {
		return -EIO;
	}

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	status.in.cfg.send_rate = send_rate;

	ret = ep1_ag08a_set_device_input(dev, &status.in.cfg);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	return 0;
}

static int ep1_ag08a_get_out_volt_waveform_type_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret, i;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_out_volt_waveform_type_modes_tbl); ++i) {
		if (ep1_ag08a_out_volt_waveform_type_modes_tbl[i].value == status.out.cfg.waveform_type)
			return ep1_ag08a_out_volt_waveform_type_modes_tbl[i].mode;
	}

	return -EINVAL;
}

static int ep1_ag08a_set_out_volt_waveform_type_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan,
						     unsigned int mode)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret, i, waveform_type = EP1_AG08A_OUTPUT_WAVEFORM_TYPE_Unknown;

	for (i = 0; i < ARRAY_SIZE(ep1_ag08a_out_volt_waveform_type_modes_tbl); ++i) {
		if (ep1_ag08a_out_volt_waveform_type_modes_tbl[i].mode == mode) {
			waveform_type = ep1_ag08a_out_volt_waveform_type_modes_tbl[i].value;
			break;
		}
	}

	if (waveform_type == EP1_AG08A_OUTPUT_WAVEFORM_TYPE_Unknown) {
		return -EIO;
	}

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	status.out.cfg.waveform_type = waveform_type;

	ret = ep1_ag08a_set_device_output(dev, &status.out.cfg);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	return 0;
}
static int ep1_ag08a_get_hw_timestamp_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	return priv->hw_timestamp;
}

static int ep1_ag08a_set_hw_timestamp_mode(struct iio_dev *indio_dev, const struct iio_chan_spec *chan,
					   unsigned int mode)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	priv->hw_timestamp = mode;
	return 0;
}

static const struct iio_enum ep1_ag08a_in_volt_range_enum = {
	.items = ep1_ag08a_in_volt_range_modes,
	.num_items = ARRAY_SIZE(ep1_ag08a_in_volt_range_modes),
	.get = ep1_ag08a_get_in_volt_range_mode,
	.set = ep1_ag08a_set_in_volt_range_mode,
};

static const struct iio_enum ep1_ag08a_in_volt_samp_freq_enum = {
	.items = ep1_ag08a_in_volt_samp_freq_modes,
	.num_items = ARRAY_SIZE(ep1_ag08a_in_volt_samp_freq_modes),
	.get = ep1_ag08a_get_in_volt_samp_freq_mode,
	.set = ep1_ag08a_set_in_volt_samp_freq_mode,
};

static const struct iio_enum ep1_ag08a_out_volt_waveform_type_enum = {
	.items = ep1_ag08a_out_volt_waveform_type_modes,
	.num_items = ARRAY_SIZE(ep1_ag08a_out_volt_waveform_type_modes),
	.get = ep1_ag08a_get_out_volt_waveform_type_mode,
	.set = ep1_ag08a_set_out_volt_waveform_type_mode,
};

static const struct iio_enum ep1_ag08a_hw_timestamp_en_enum = {
	.items = ep1_ag08a_hw_timestamp_modes,
	.num_items = ARRAY_SIZE(ep1_ag08a_hw_timestamp_modes),
	.get = ep1_ag08a_get_hw_timestamp_mode,
	.set = ep1_ag08a_set_hw_timestamp_mode,
};

static const struct iio_chan_spec_ext_info ep1_ag08a_in_volt_ext_info[] = {
	IIO_ENUM("range", IIO_SEPARATE, &ep1_ag08a_in_volt_range_enum),
	IIO_ENUM_AVAILABLE("range", &ep1_ag08a_in_volt_range_enum),
	IIO_ENUM("sampling_frequency", IIO_SHARED_BY_TYPE, &ep1_ag08a_in_volt_samp_freq_enum),
	IIO_ENUM_AVAILABLE("sampling_frequency", &ep1_ag08a_in_volt_samp_freq_enum),
	IIO_ENUM("hardware_timestamp_en", IIO_SHARED_BY_ALL, &ep1_ag08a_hw_timestamp_en_enum),
	{},
};

static const struct iio_chan_spec_ext_info ep1_ag08a_out_volt_ext_info[] = {
	IIO_ENUM("waveform_type", IIO_SHARED_BY_TYPE, &ep1_ag08a_out_volt_waveform_type_enum),
	IIO_ENUM_AVAILABLE("waveform_type", &ep1_ag08a_out_volt_waveform_type_enum),
	{},
};

// clang-format off
#define EP1_AG08A_IIO_CHANNEL_IN_VOLT(_ch)  \
{  \
    .type           = IIO_VOLTAGE,  \
    .channel        = _ch,  \
    .datasheet_name = "channel"#_ch,  \
    .indexed        = 1,  \
    .info_mask_separate	= BIT(IIO_CHAN_INFO_SCALE),  \
    .scan_index     = _ch,  \
    .scan_type = {  \
        .sign = 's',  \
        .realbits = 16,  \
        .storagebits = 16,  \
        .endianness = IIO_LE,  \
    },  \
    .ext_info = ep1_ag08a_in_volt_ext_info,  \
}

#define EP1_AG08A_IIO_CHANNEL_OUT_VOLT(_ch)  \
{  \
    .type           = IIO_VOLTAGE,  \
    .channel        = _ch,  \
    .output = 1,  \
    .info_mask_separate	= BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_ENABLE) | BIT(IIO_CHAN_INFO_FREQUENCY),  \
    .scan_index = -1,  \
    .ext_info = ep1_ag08a_out_volt_ext_info,  \
}

static const struct iio_chan_spec ep1_ag08a_channels[] = {
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(0),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(1),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(2),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(3),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(4),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(5),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(6),
    EP1_AG08A_IIO_CHANNEL_IN_VOLT(7),
    IIO_CHAN_SOFT_TIMESTAMP(8),
    EP1_AG08A_IIO_CHANNEL_OUT_VOLT(0)
};
// clang-format on

static int ep1_ag08a_read_raw(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int *val, int *val2,
			      long mask)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret;
	int ch = chan->channel;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	switch (mask) {
	case IIO_CHAN_INFO_SCALE: {
		int voltage = 0;

		voltage = ep1_ag08a_msg_convert_input_voltage_to_value(status.in.cfg.vmin[ch], status.in.cfg.vmax[ch]);
		if (voltage == EP1_AG08A_INPUT_VOLTAGE_Unknown) {
			return -EINVAL;
		}
		*val2 = ep1_ag08a_msg_convert_input_voltage_to_nano_scale(voltage);
		if (*val2 < 0) {
			return -EINVAL;
		}
		*val = 0;
		return IIO_VAL_INT_PLUS_NANO;
	}
	case IIO_CHAN_INFO_RAW: {
		if (!chan->output) {
			return -EINVAL;
		}
		*val = status.out.cfg.voltage / 1000;
		*val2 = (status.out.cfg.voltage % 1000) * 1000;
		return IIO_VAL_INT_PLUS_MICRO;
	}
	case IIO_CHAN_INFO_FREQUENCY: {
		*val = status.out.cfg.frequency / 1000;
		*val2 = (status.out.cfg.frequency % 1000) * 1000;
		return IIO_VAL_INT_PLUS_MICRO;
	}
	case IIO_CHAN_INFO_ENABLE: {
		*val = status.out.ctrl.start;
		return IIO_VAL_INT;
	}
	default:
		break;
	}

	return -EINVAL;
}

static int ep1_ag08a_write_raw(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int val, int val2,
			       long mask)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ret;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		return -EIO;
	}

	switch (mask) {
	case IIO_CHAN_INFO_RAW: {
		status.out.cfg.voltage = val * 1000 + val2 / 1000;
		ret = ep1_ag08a_set_device_output(dev, &status.out.cfg);
		if (ret != RESULT_Success) {
			return -EIO;
		}
		return 0;
	}
	case IIO_CHAN_INFO_FREQUENCY: {
		status.out.cfg.frequency = val * 1000 + val2 / 1000;
		ret = ep1_ag08a_set_device_output(dev, &status.out.cfg);
		if (ret != RESULT_Success) {
			return -EIO;
		}
		return 0;
	}
	case IIO_CHAN_INFO_ENABLE: {
		status.out.ctrl.start = val;
		ret = ep1_ag08a_control_device_output(dev, &status.out.ctrl);
		if (ret != RESULT_Success) {
			return -EIO;
		}
		return 0;
	}
	default:
		break;
	}

	return -EINVAL;
}

static const struct iio_info ep1_ag08a_iio_info = {
	.read_raw = ep1_ag08a_read_raw,
	.write_raw = ep1_ag08a_write_raw,
};

static int ep1_ag08a_buffer_preenable(struct iio_dev *indio_dev)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(priv->dev);
	int if_type = atomic_read(&unique_data->if_type);

	if (if_type != EP1_AG08A_IF_TYPE_NONE) {
		EMSG("Device is already in use");
		return -EBUSY;
	}

	atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_IIO);

	return 0;
}

static int ep1_ag08a_buffer_postenable(struct iio_dev *indio_dev)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(priv->dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_msg_resp_get_status_t status;
	int ch, ret;

	ret = ep1_ag08a_get_status(dev, &status);
	if (ret != RESULT_Success) {
		atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_NONE);
		return -EIO;
	}

	for (ch = 0; ch < EP1_AG08A_CH_NUM; ch++) {
		status.in.ctrl.start[ch] = *indio_dev->active_scan_mask & (1 << ch);
	}

	ret = ep1_ag08a_control_device_input(dev, &status.in.ctrl);
	if (ret != RESULT_Success) {
		atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_NONE);
		return -EIO;
	}

	return 0;
}

static int ep1_ag08a_buffer_predisable(struct iio_dev *indio_dev)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return -ESHUTDOWN;
	}

	ep1_ag08a_stop_device_input(dev);

	return 0;
}

static int ep1_ag08a_buffer_postdisable(struct iio_dev *indio_dev)
{
	ep1_ag08a_iio_data_t *priv = iio_priv(indio_dev);
	apt_usbtrx_dev_t *dev = priv->dev;
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(priv->dev);
	bool onclosing;

	onclosing = atomic_read(&dev->onclosing);
	if (onclosing == true) {
		return -ESHUTDOWN;
	}

	atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_NONE);

	return 0;
}

static const struct iio_buffer_setup_ops ep1_ag08a_buffer_ops = {
	.preenable = &ep1_ag08a_buffer_preenable,
	.postenable = &ep1_ag08a_buffer_postenable,
	.predisable = &ep1_ag08a_buffer_predisable,
	.postdisable = &ep1_ag08a_buffer_postdisable,
};

/*!
 * @brief create iiodev
 */
int ep1_ag08a_create_iiodev(struct usb_interface *intf, const struct usb_device_id *id)
{
	apt_usbtrx_dev_t *dev = usb_get_intfdata(intf);
	ep1_ag08a_unique_data_t *unique_data = get_unique_data(dev);
	ep1_ag08a_iio_data_t *priv;
	struct iio_dev *indio_dev;
	struct iio_buffer *buffer;
	int ret;

	indio_dev = devm_iio_device_alloc(&intf->dev, sizeof(*priv));
	if (!indio_dev) {
		return -ENOMEM;
	}

	priv = iio_priv(indio_dev);
	priv->dev = dev;
	priv->hw_timestamp = true;

	indio_dev->dev.parent = &intf->dev;
	indio_dev->name = devm_kasprintf(&intf->dev, GFP_KERNEL, "%s-%s", EP1_AG08A_PRODUCT_NAME, dev_name(&intf->dev));
	if (!indio_dev->name) {
		return -ENOMEM;
	}
	indio_dev->modes = INDIO_BUFFER_SOFTWARE;
	indio_dev->info = &ep1_ag08a_iio_info;
	indio_dev->channels = ep1_ag08a_channels;
	indio_dev->num_channels = ARRAY_SIZE(ep1_ag08a_channels);
	indio_dev->setup_ops = &ep1_ag08a_buffer_ops;

	buffer = devm_iio_kfifo_allocate(&intf->dev);
	if (!buffer) {
		return -ENOMEM;
	}
	iio_device_attach_buffer(indio_dev, buffer);

	ret = devm_iio_device_register(&intf->dev, indio_dev);
	if (ret) {
		EMSG("failed to register iio device: %d", ret);
		return ret;
	}

	unique_data->indio_dev = indio_dev;

	return 0;
}
