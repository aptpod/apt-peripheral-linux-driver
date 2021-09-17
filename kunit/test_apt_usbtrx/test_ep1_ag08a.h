#pragma once

#include "test_apt_usbtrx.h"

void test_ep1_ag08a_dispatch_msg_notify_analog_input(struct kunit *test);
void test_ep1_ag08a_dispatch_msg_invalid_id(struct kunit *test);
void test_ep1_ag08a_ioctl_get_status(struct kunit *test);
void test_ep1_ag08a_ioctl_invalid_cmd(struct kunit *test);
