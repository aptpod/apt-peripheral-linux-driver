/*!
 * Copyright (C) 2020 aptpod Inc.
 */

#include <kunit/test.h>

#include "test_apt_usbtrx.h"
#include "test_ep1_ag08a.h"

#include "../apt_usbtrx/apt_usbtrx_msg.h"
#include "../apt_usbtrx/apt_usbtrx_fops.h"
#include "../apt_usbtrx/apt_usbtrx_ioctl.h"
#include "../apt_usbtrx/ep1_ag08a/ep1_ag08a.h"
#include "../apt_usbtrx/ep1_ag08a/ep1_ag08a_cmd_def.h"

/* test data */
struct payload_ep1_ag08a_notify_analog_input_1ch {
	apt_usbtrx_timestamp_t timestamp;
	u8 channel;
	u16 data;
} __attribute__((packed));

struct payload_ep1_ag08a_notify_analog_input_8ch {
	apt_usbtrx_timestamp_t timestamp;
	u8 channel;
	u16 data[8];
} __attribute__((packed));

struct payload_ep1_ag08a_notify_analog_input_1ch payload_notify_analog_input_1ch = {
	.timestamp.ts_sec = 1,
	.timestamp.ts_usec = 2,
	.channel = 0x08,
	.data = 0x7FFF,
};

struct payload_ep1_ag08a_notify_analog_input_8ch payload_notify_analog_input_8ch = {
	.timestamp.ts_sec = 3,
	.timestamp.ts_usec = 4,
	.channel = 0xFF,
	.data =
		{
			0x0000,
			0xFFFF,
			0x0001,
			0xFFFE,
			0x00FF,
			0xFF00,
			0x7FFF,
			0x8000,
		},
};

/* For mock data from FW, see mock/mock_ep1_ag08a.c */
ep1_ag08a_ioctl_get_status_t exp_ioctl_get_status = {
	.in.cfg.send_rate = 10000000,
	.in.cfg.vmin = { -10000, -10000, -5000, -5000, -5000, 0, 0, 0 },
	.in.cfg.vmax = { 10000, 10000, 5000, 5000, 5000, 5000, 5000, 5000 },
	.in.ctrl.start = { true, false, true, false, false, true, false, true },

	.out.cfg.waveform_type = 0x01,
	.out.cfg.voltage = 5000,
	.out.cfg.frequency = 100000,
	.out.ctrl.start = true,
};

void test_ep1_ag08a_dispatch_msg_notify_analog_input(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;
	int result;

	fake_dev_init(test, dev, EP1_AG08A);

	/* simple send/recv test */
	{
		u8 cmd_id = EP1_AG08A_CMD_NotifyAnalogInput;
		struct payload_ep1_ag08a_notify_analog_input_1ch exp_payload = payload_notify_analog_input_1ch;
		struct payload_ep1_ag08a_notify_analog_input_1ch act_payload;
		const size_t exp_payload_size = sizeof(exp_payload);
		const size_t act_payload_size = sizeof(act_payload);

		send_and_recv_message(test, dev, cmd_id, (u8 *)&exp_payload, exp_payload_size, (u8 *)&act_payload,
				      act_payload_size);
	}
	{
		u8 cmd_id = EP1_AG08A_CMD_NotifyAnalogInput;
		struct payload_ep1_ag08a_notify_analog_input_8ch exp_payload = payload_notify_analog_input_8ch;
		struct payload_ep1_ag08a_notify_analog_input_8ch act_payload;
		const size_t exp_payload_size = sizeof(exp_payload);
		const size_t act_payload_size = sizeof(act_payload);

		send_and_recv_message(test, dev, cmd_id, (u8 *)&exp_payload, exp_payload_size, (u8 *)&act_payload,
				      act_payload_size);
	}

	/* ringbuffer overflow test */
	{
		u8 cmd_id = EP1_AG08A_CMD_NotifyAnalogInput;
		struct payload_ep1_ag08a_notify_analog_input_8ch exp_payload = payload_notify_analog_input_8ch;
		struct payload_ep1_ag08a_notify_analog_input_8ch act_payload;
		const size_t exp_payload_size = sizeof(exp_payload);
		ssize_t act_payload_size = sizeof(act_payload);
		int total_send_size = 0;
		ssize_t rsize;

		/* overflow check */
		while (total_send_size + exp_payload_size <= dev->rx_data_size) {
			result = send_message(test, dev, cmd_id, (u8 *)&exp_payload, exp_payload_size);
			KUNIT_EXPECT_EQ(test, RESULT_Success, result);
			KUNIT_ASSERT_EQ(test, (u64)0, dev->rx_data.skip_count);
			total_send_size += exp_payload_size;
		}
		result = send_message(test, dev, cmd_id, (u8 *)&exp_payload, exp_payload_size);
		KUNIT_EXPECT_EQ(test, RESULT_Success, result);
		KUNIT_ASSERT_EQ(test, (u64)exp_payload_size, dev->rx_data.skip_count);

		/* all read check */
		while (total_send_size > 0) {
			rsize = recv_message(test, dev, (u8 *)&act_payload, act_payload_size);
			KUNIT_EXPECT_EQ(test, act_payload_size, rsize);
			expect_eq_all(test, (u8 *)&exp_payload, exp_payload_size, (u8 *)&act_payload, act_payload_size);
			total_send_size -= act_payload_size;
		}
	}

	fake_dev_terminate(test, dev);
}

void test_ep1_ag08a_dispatch_msg_invalid_id(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;
	int result;

	fake_dev_init(test, dev, EP1_AG08A);

	{
		u8 cmd_id = 0xFF;
		struct payload_ep1_ag08a_notify_analog_input_1ch exp_payload = payload_notify_analog_input_1ch;
		const size_t exp_payload_size = sizeof(exp_payload);

		result = send_message(test, dev, cmd_id, (u8 *)&exp_payload, exp_payload_size);
		KUNIT_EXPECT_EQ(test, RESULT_Failure, result);
	}
	fake_dev_terminate(test, dev);
}

void test_ep1_ag08a_ioctl_get_status(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;
	int i;

	fake_dev_init(test, dev, EP1_AG08A);

	{
		unsigned int cmd = EP1_AG08A_IOCTL_GET_STATUS;
		ep1_ag08a_ioctl_get_status_t exp_status = exp_ioctl_get_status;
		ep1_ag08a_ioctl_get_status_t act_status;
		check_ioctl(test, dev, cmd, (unsigned long)&act_status, 0);

		KUNIT_EXPECT_EQ(test, exp_status.in.cfg.send_rate, act_status.in.cfg.send_rate);
		for (i = 0; i < EP1_AG08A_CH_NUM; ++i) {
			KUNIT_EXPECT_EQ(test, exp_status.in.cfg.vmin[i], act_status.in.cfg.vmin[i]);
			KUNIT_EXPECT_EQ(test, exp_status.in.cfg.vmax[i], act_status.in.cfg.vmax[i]);
			KUNIT_EXPECT_EQ(test, exp_status.in.ctrl.start[i], act_status.in.ctrl.start[i]);
		}
		KUNIT_EXPECT_EQ(test, exp_status.out.cfg.waveform_type, act_status.out.cfg.waveform_type);
		KUNIT_EXPECT_EQ(test, exp_status.out.cfg.voltage, act_status.out.cfg.voltage);
		KUNIT_EXPECT_EQ(test, exp_status.out.cfg.frequency, act_status.out.cfg.frequency);
		KUNIT_EXPECT_EQ(test, exp_status.out.ctrl.start, act_status.out.ctrl.start);
	}

	fake_dev_terminate(test, dev);
}

void test_ep1_ag08a_ioctl_invalid_cmd(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;

	fake_dev_init(test, dev, EP1_AG08A);

	{
		unsigned int cmd = 0xFF;
		unsigned long arg = 0;
		check_ioctl(test, dev, cmd, arg, -EFAULT);
	}

	fake_dev_terminate(test, dev);
}
