#pragma once

#include "apt_usbtrx_msg.h"

#include "mock_common.h"

struct mock_ep1_ch02a_response_get_status_payload {
	u8 baudrate : 5, start : 1, silent : 1, enable_sync : 1;
} __attribute__((packed));

DEFINE_MOCK_MSG_STRUCT(mock_ep1_ch02a_response_get_status);

void set_mock_ep1_ch02a_get_status(const struct mock_ep1_ch02a_response_get_status_payload *payload);

void build_response_ep1_ch02a_get_status(struct mock_ep1_ch02a_response_get_status *resp_data, apt_usbtrx_msg_t *msg);

struct mock_ep1_ch02a_set_bit_timing_payload {
	u8 prop_seg;
	u8 phase_seg1;
	u8 phase_seg2;
	u8 sjw;
	u16 brp;
} __attribute__((packed));

void mock_request_ep1_ch02a_set_bit_timing(const void *data, int data_size);
void get_mock_ep1_ch02a_set_bit_timing_payload(struct mock_ep1_ch02a_set_bit_timing_payload *payload);

struct mock_ep1_ch02a_response_get_bit_timing_payload {
	u32 can_clock;
	u8 prop_seg;
	u8 phase_seg1;
	u8 phase_seg2;
	u8 sjw;
	u16 brp;
} __attribute__((packed));

DEFINE_MOCK_MSG_STRUCT(mock_ep1_ch02a_response_get_bit_timing);

void build_response_ep1_ch02a_get_bit_timing(struct mock_ep1_ch02a_response_get_bit_timing *resp_data,
					     apt_usbtrx_msg_t *msg);
