#pragma once

#include <linux/types.h>
#include "../apt_usbtrx/apt_usbtrx_def.h"

enum DeviceType {
	AP_CT2A = 0,
	EP1_AG08A,
	EP1_CH02A,
};

struct apt_usbtrx_test_data {
	apt_usbtrx_dev_t *dev;
};

struct msg_header {
	u8 sob;
	u8 length;
	u8 cmd;
} __attribute__((packed));

struct msg_footer {
	u8 eob;
} __attribute__((packed));

void fake_dev_init(struct kunit *test, apt_usbtrx_dev_t *dev, enum DeviceType device_type);
void fake_dev_terminate(struct kunit *test, apt_usbtrx_dev_t *dev);
int send_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 cmd_id, u8 *payload, size_t payload_size);
ssize_t recv_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 *act_payload, size_t act_payload_size);
void expect_eq_all(struct kunit *test, u8 *exp, size_t exp_size, u8 *act, size_t act_size);
void send_and_recv_message(struct kunit *test, apt_usbtrx_dev_t *dev, u8 cmd_id, u8 *exp_payload,
			   size_t exp_payload_size, u8 *act_payload, size_t act_payload_size);
long check_ioctl(struct kunit *test, apt_usbtrx_dev_t *dev, unsigned int cmd, unsigned long arg, long exp_ret);
