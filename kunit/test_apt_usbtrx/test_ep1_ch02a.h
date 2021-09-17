#pragma once

#include "test_apt_usbtrx.h"

void test_ep1_ch02a_ioctl_set_trigger_is_invalid(struct kunit *test);
void test_ep1_ch02a_ioctl_invalid_cmd(struct kunit *test);
void test_ep1_ch02a_ioctl_get_status(struct kunit *test);

void test_ep1_ch02a_ioctl_set_bit_timing(struct kunit *test);
void test_ep1_ch02a_ioctl_get_bit_timing(struct kunit *test);
