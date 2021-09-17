/*!
 * Copyright (C) 2021 aptpod Inc.
 */

#include <kunit/test.h>

#include "test_apt_usbtrx.h"
#include "test_ep1_ch02a.h"

#include "../apt_usbtrx/apt_usbtrx_msg.h"
#include "../apt_usbtrx/apt_usbtrx_fops.h"
#include "../apt_usbtrx/apt_usbtrx_ioctl.h"
#include "../apt_usbtrx/mock_ep1_ch02a.h"

void test_ep1_ch02a_ioctl_set_trigger_is_invalid(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;

	fake_dev_init(test, dev, EP1_CH02A);

	{
		unsigned int cmd = APT_USBTRX_IOCTL_SET_TRIGGER;
		apt_usbtrx_ioctl_set_trigger_t arg;
		check_ioctl(test, dev, cmd, (unsigned long)&arg, -EFAULT);
	}

	fake_dev_terminate(test, dev);
}

void test_ep1_ch02a_ioctl_invalid_cmd(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;

	fake_dev_init(test, dev, EP1_CH02A);

	{
		unsigned int cmd = 0xFF;
		unsigned long arg = 0;
		check_ioctl(test, dev, cmd, arg, -EFAULT);
	}

	fake_dev_terminate(test, dev);
}

/* For mock data from FW, see mock/mock_ep1_ch02a.c */
struct ep1_ch02a_get_status_test_case {
	struct mock_ep1_ch02a_response_get_status_payload actual;
	ep1_ch02a_ioctl_get_status_t expect;
};

static struct ep1_ch02a_get_status_test_case ioctl_get_status_cases[] = {
	{ { .silent = 0x00, .start = 0x00, .baudrate = 0x10, .enable_sync = 0x00 },
	  { .silent = true, .start = false, .baudrate = 33, .enable_sync = false } },
	{ { .silent = 0x01, .start = 0x01, .baudrate = 0x11, .enable_sync = 0x00 },
	  { .silent = false, .start = true, .baudrate = 50, .enable_sync = false } },
	{ { .silent = 0x01, .start = 0x00, .baudrate = 0x12, .enable_sync = 0x00 },
	  { .silent = false, .start = false, .baudrate = 83, .enable_sync = false } },
	{ { .silent = 0x00, .start = 0x01, .baudrate = 0x13, .enable_sync = 0x00 },
	  { .silent = true, .start = true, .baudrate = 100, .enable_sync = false } },
	{ { .silent = 0x00, .start = 0x00, .baudrate = 0x14, .enable_sync = 0x01 },
	  { .silent = true, .start = false, .baudrate = 125, .enable_sync = true } },
	{ { .silent = 0x01, .start = 0x00, .baudrate = 0x15, .enable_sync = 0x01 },
	  { .silent = false, .start = false, .baudrate = 250, .enable_sync = true } },
	{ { .silent = 0x00, .start = 0x01, .baudrate = 0x16, .enable_sync = 0x01 },
	  { .silent = true, .start = true, .baudrate = 500, .enable_sync = true } },
	{ { .silent = 0x01, .start = 0x01, .baudrate = 0x17, .enable_sync = 0x01 },
	  { .silent = false, .start = true, .baudrate = 1000, .enable_sync = true } },
};

void test_ep1_ch02a_ioctl_get_status(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;
	int i;

	fake_dev_init(test, dev, EP1_CH02A);

	for (i = 0; i < ARRAY_SIZE(ioctl_get_status_cases); ++i) {
		ep1_ch02a_ioctl_get_status_t exp_status = ioctl_get_status_cases[i].expect;
		ep1_ch02a_ioctl_get_status_t act_status;
		unsigned int cmd = EP1_CH02A_IOCTL_GET_STATUS;

		set_mock_ep1_ch02a_get_status(&ioctl_get_status_cases[i].actual);

		check_ioctl(test, dev, cmd, (unsigned long)&act_status, 0);

		KUNIT_EXPECT_EQ(test, exp_status.silent, act_status.silent);
		KUNIT_EXPECT_EQ(test, exp_status.start, act_status.start);
		KUNIT_EXPECT_EQ(test, exp_status.baudrate, act_status.baudrate);
		KUNIT_EXPECT_EQ(test, exp_status.enable_sync, act_status.enable_sync);
	}

	fake_dev_terminate(test, dev);
}

void test_ep1_ch02a_ioctl_set_bit_timing(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;

	fake_dev_init(test, dev, EP1_CH02A);

	{
		ep1_ch02a_ioctl_set_bit_timing_t exp_status = {
			.prop_seg = 5, .phase_seg1 = 2, .phase_seg2 = 1, .sjw = 4, .brp = 8
		};
		ep1_ch02a_ioctl_set_bit_timing_t act_status;
		struct mock_ep1_ch02a_set_bit_timing_payload req_payload;
		unsigned int cmd = EP1_CH02A_IOCTL_SET_BIT_TIMING;

		check_ioctl(test, dev, cmd, (unsigned long)&exp_status, 0);

		get_mock_ep1_ch02a_set_bit_timing_payload(&req_payload);
		act_status.prop_seg = req_payload.prop_seg;
		act_status.phase_seg1 = req_payload.phase_seg1;
		act_status.phase_seg2 = req_payload.phase_seg2;
		act_status.sjw = req_payload.sjw;
		act_status.brp = req_payload.brp;

		KUNIT_EXPECT_EQ(test, exp_status.prop_seg, act_status.prop_seg);
		KUNIT_EXPECT_EQ(test, exp_status.phase_seg1, act_status.phase_seg1);
		KUNIT_EXPECT_EQ(test, exp_status.phase_seg2, act_status.phase_seg2);
		KUNIT_EXPECT_EQ(test, exp_status.sjw, act_status.sjw);
		KUNIT_EXPECT_EQ(test, exp_status.brp, act_status.brp);
	}

	fake_dev_terminate(test, dev);
}

void test_ep1_ch02a_ioctl_get_bit_timing(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;
	apt_usbtrx_dev_t *dev = test_data->dev;

	fake_dev_init(test, dev, EP1_CH02A);

	{
		ep1_ch02a_ioctl_get_bit_timing_t exp_status = {
			.can_clock = 7500000,
			.cfg.prop_seg = 8,
			.cfg.phase_seg1 = 3,
			.cfg.phase_seg2 = 2,
			.cfg.sjw = 4,
			.cfg.brp = 4,
		};
		ep1_ch02a_ioctl_get_bit_timing_t act_status;
		unsigned int cmd = EP1_CH02A_IOCTL_GET_BIT_TIMING;

		check_ioctl(test, dev, cmd, (unsigned long)&act_status, 0);

		KUNIT_EXPECT_EQ(test, exp_status.can_clock, act_status.can_clock);
		KUNIT_EXPECT_EQ(test, exp_status.cfg.prop_seg, act_status.cfg.prop_seg);
		KUNIT_EXPECT_EQ(test, exp_status.cfg.phase_seg1, act_status.cfg.phase_seg1);
		KUNIT_EXPECT_EQ(test, exp_status.cfg.phase_seg2, act_status.cfg.phase_seg2);
		KUNIT_EXPECT_EQ(test, exp_status.cfg.sjw, act_status.cfg.sjw);
		KUNIT_EXPECT_EQ(test, exp_status.cfg.brp, act_status.cfg.brp);
	}

	fake_dev_terminate(test, dev);
}
