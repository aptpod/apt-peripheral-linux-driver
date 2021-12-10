/*!
 * Copyright (C) 2020 aptpod Inc.
 */

#include <kunit/test.h>

#include "test_apt_usbtrx.h"
#include "test_ep1_ag08a.h"
#include "test_ep1_ch02a.h"

#include "../apt_usbtrx/apt_usbtrx_core.h"
#include "../apt_usbtrx/apt_usbtrx_fops.h"
#include "../apt_usbtrx/ap_ct2a/ap_ct2a.h"
#include "../apt_usbtrx/ep1_ag08a/ep1_ag08a.h"

/* apt_usbtrx_main.c static func */
extern int apt_usbtrx_init_function(struct usb_interface *intf, apt_usbtrx_dev_t *dev);
extern int apt_usbtrx_init_instance(apt_usbtrx_dev_t *dev);

struct usb_interface intf;
struct usb_device usb_dev;
struct usb_host_interface iface_desc;
struct usb_endpoint_descriptor bulk_in;
struct usb_endpoint_descriptor bulk_out;

void fake_dev_init(struct kunit *test, apt_usbtrx_dev_t *dev, enum DeviceType device_type)
{
	/* FIXME: Not enough initialization process, add more if necessary. */

	int result;

	memset(dev, 0, sizeof(apt_usbtrx_dev_t));

	switch (device_type) {
	case AP_CT2A:
		usb_dev.descriptor.idProduct = AP_CT2A_PRODUCT_ID;
		dev->rx_data_size = AP_CT2A_RXDATA_BUFFER_SIZE;
		break;
	case EP1_AG08A: {
		usb_dev.descriptor.idProduct = EP1_AG08A_PRODUCT_ID;
		dev->rx_data_size = EP1_AG08A_RXDATA_BUFFER_SIZE;
		dev->unique_data = kunit_kzalloc(test, sizeof(ep1_ag08a_unique_data_t), GFP_KERNEL);
		KUNIT_ASSERT_NOT_ERR_OR_NULL(test, dev->unique_data);
		break;
	}
	case EP1_CH02A:
		usb_dev.descriptor.idProduct = EP1_CH02A_PRODUCT_ID;
		break;
	}
	intf.dev.parent = &usb_dev.dev; /* used in apt_usbtrx_init_function() */
	usb_dev.dev.driver_data = dev; /* used in mock */

	intf.cur_altsetting = &iface_desc;
	iface_desc.desc.bInterfaceClass = 0xFF;
	iface_desc.desc.bInterfaceSubClass = 0x00;

	result = apt_usbtrx_init_function(&intf, dev);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);

	result = apt_usbtrx_init_instance(dev);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);

	/* for test init */
	atomic_set(&dev->onopening, false);
	atomic_set(&dev->rx_ongoing, true);
	dev->udev = &usb_dev;
	dev->interface = &intf;
	dev->bulk_in = &bulk_in;
	dev->bulk_out = &bulk_out;
	switch (device_type) {
	case EP1_AG08A: {
		ep1_ag08a_unique_data_t *unique_data;
		unique_data = dev->unique_data;
		atomic_set(&unique_data->if_type, EP1_AG08A_IF_TYPE_FILE);
		break;
	}
	}

	result = apt_usbtrx_ringbuffer_init(&dev->rx_data, dev->rx_data_size);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);

	dev->rx_complete.buffer = kunit_kzalloc(test, dev->rx_complete.buffer_size, GFP_KERNEL);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, dev->rx_complete.buffer);
}

void fake_dev_terminate(struct kunit *test, apt_usbtrx_dev_t *dev)
{
	int result;

	if (dev->unique_data != NULL) {
		kunit_kfree(test, dev->unique_data);
	}

	if (dev->rx_complete.buffer != NULL) {
		kunit_kfree(test, dev->rx_complete.buffer);
		dev->rx_complete.buffer = NULL;
	}

	result = dev->unique_func.free_data(dev);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);

	result = apt_usbtrx_ringbuffer_term(&dev->rx_data);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);

	result = dev->unique_func.free_data(dev);
	KUNIT_EXPECT_EQ(test, RESULT_Success, result);
}

static void _build_data_and_message(struct kunit *test, u8 cmd, u8 *payload, u8 payload_size, void *data,
				    apt_usbtrx_msg_t *msg)
{
	struct msg_header *header = (struct msg_header *)data;
	u8 *data_payload = (u8 *)data + sizeof(struct msg_header);
	struct msg_footer *footer = (struct msg_footer *)((u8 *)data + sizeof(struct msg_header) + payload_size);

	KUNIT_ASSERT_PTR_NE(test, (void *)payload, NULL);
	KUNIT_ASSERT_LE(test, (int)payload_size, (int)APT_USBTRX_MSG_LENGTH_TO_PAYLOAD(APT_USBTRX_CMD_MAX_LENGTH));
	KUNIT_ASSERT_PTR_NE(test, data, NULL);
	KUNIT_ASSERT_PTR_NE(test, (void *)msg, NULL);

	/* build data */
	header->sob = APT_USBTRX_MSG_SOB;
	header->length = sizeof(struct msg_header) + payload_size + sizeof(struct msg_footer);
	header->cmd = cmd;

	memcpy(data_payload, payload, payload_size);

	footer->eob = APT_USBTRX_MSG_EOB;

	/* build msg */
	msg->id = cmd;
	memcpy(msg->payload, payload, payload_size);
	msg->payload_size = payload_size;
}

int send_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 cmd_id, u8 *payload, size_t payload_size)
{
	apt_usbtrx_msg_t msg;
	int result;

	const size_t data_size = sizeof(struct msg_header) + payload_size + sizeof(struct msg_footer);
	u8 *data = kunit_kzalloc(test, data_size, GFP_KERNEL);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, data);

	_build_data_and_message(test, cmd_id, payload, payload_size, data, &msg);

	result = apt_usbtrx_dispatch_msg(dev, data, &msg);

	if (data != NULL) {
		kunit_kfree(test, data);
		data = NULL;
	}

	return result;
}

ssize_t recv_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 *act_payload, size_t act_payload_size)
{
	struct file file = {
		.private_data = dev,
	};

	ssize_t rsize = apt_usbtrx_read(&file, (char *)act_payload, act_payload_size, NULL);
	return rsize;
}

void expect_eq_all(struct kunit *test, u8 *exp, size_t exp_size, u8 *act, size_t act_size)
{
	int i;

	KUNIT_ASSERT_EQ(test, exp_size, act_size);

	for (i = 0; i < exp_size; ++i) {
		KUNIT_EXPECT_EQ(test, exp[i], act[i]);
	}
}

void send_and_recv_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 cmd_id, u8 *exp_payload,
			   size_t exp_payload_size, u8 *act_payload, size_t act_payload_size)
{
	int result;
	ssize_t rsize;
	ssize_t exp_rsize = act_payload_size;

	result = send_message(test, dev, cmd_id, exp_payload, exp_payload_size);
	KUNIT_ASSERT_EQ(test, RESULT_Success, result);
	KUNIT_ASSERT_EQ(test, (u64)0, dev->rx_data.skip_count);

	rsize = recv_message(test, dev, act_payload, act_payload_size);
	KUNIT_EXPECT_EQ(test, exp_rsize, rsize);

	expect_eq_all(test, exp_payload, exp_payload_size, act_payload, act_payload_size);
}

long check_ioctl(struct kunit *test, apt_usbtrx_dev_t *dev, unsigned int cmd, unsigned long arg, long exp_ret)
{
	long act_ret;
	struct file file = {
		.private_data = dev,
	};

	act_ret = apt_usbtrx_ioctl(&file, cmd, arg);
	KUNIT_EXPECT_EQ(test, exp_ret, act_ret);

	return act_ret;
}

static int apt_usbtrx_test_init(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data;

	test_data = kunit_kzalloc(test, sizeof(struct apt_usbtrx_test_data), GFP_KERNEL);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_data);

	test_data->dev = kunit_kzalloc(test, sizeof(apt_usbtrx_dev_t), GFP_KERNEL);
	KUNIT_ASSERT_NOT_ERR_OR_NULL(test, test_data->dev);

	test->priv = test_data;

	return 0;
}

static void apt_usbtrx_test_exit(struct kunit *test)
{
	struct apt_usbtrx_test_data *test_data = test->priv;

	if (test_data->dev != NULL) {
		kunit_kfree(test, test_data->dev);
		test_data->dev = NULL;
	}
	if (test_data != NULL) {
		kunit_kfree(test, test_data);
		test_data = NULL;
	}

	test->priv = NULL;
}

static struct kunit_case apt_usbtrx_test_cases[] = {
	// EP1-AG08A
	KUNIT_CASE(test_ep1_ag08a_dispatch_msg_notify_analog_input),
	KUNIT_CASE(test_ep1_ag08a_dispatch_msg_invalid_id),
	KUNIT_CASE(test_ep1_ag08a_ioctl_get_status),
	KUNIT_CASE(test_ep1_ag08a_ioctl_invalid_cmd),
	// EP1-CH02A
	KUNIT_CASE(test_ep1_ch02a_ioctl_set_trigger_is_invalid),
	KUNIT_CASE(test_ep1_ch02a_ioctl_invalid_cmd),
	KUNIT_CASE(test_ep1_ch02a_ioctl_get_status),
	KUNIT_CASE(test_ep1_ch02a_ioctl_set_bit_timing),
	KUNIT_CASE(test_ep1_ch02a_ioctl_get_bit_timing),
	{}
};

static struct kunit_suite apt_usbtrx_test_suite = {
	.name = "aptusbtrx test",
	.init = apt_usbtrx_test_init,
	.exit = apt_usbtrx_test_exit,
	.test_cases = apt_usbtrx_test_cases,
};
kunit_test_suite(apt_usbtrx_test_suite);
