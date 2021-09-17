#pragma once

#include "apt_usbtrx_msg.h"

#include "mock_common.h"

// EP1-AG08A
struct mock_ep1_ag08a_analog_input_set {
	u8 send_rate;
	u8 input_voltage[8];
} __attribute__((packed));

struct mock_ep1_ag08a_analog_input_control {
	u8 act;
} __attribute__((packed));

struct mock_ep1_ag08a_analog_input {
	struct mock_ep1_ag08a_analog_input_set cfg;
	struct mock_ep1_ag08a_analog_input_control ctrl;
} __attribute__((packed));

struct mock_ep1_ag08a_analog_output_set {
	u8 waveform_type;
	u8 voltage;
	u8 frequency;
} __attribute__((packed));

struct mock_ep1_ag08a_analog_output_control {
	u8 act;
} __attribute__((packed));

struct mock_ep1_ag08a_analog_output {
	struct mock_ep1_ag08a_analog_output_set cfg;
	struct mock_ep1_ag08a_analog_output_control ctrl;
} __attribute__((packed));

struct mock_ep1_ag08a_response_get_status_payload {
	struct mock_ep1_ag08a_analog_input in;
	struct mock_ep1_ag08a_analog_output out;
} __attribute__((packed));

struct mock_ep1_ag08a_response_get_status {
	struct mock_msg_header header;
	struct mock_ep1_ag08a_response_get_status_payload payload;
	struct mock_msg_footer footer;
} __attribute__((packed));

void build_response_get_status(struct mock_ep1_ag08a_response_get_status *resp_data, apt_usbtrx_msg_t *msg);
