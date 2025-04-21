// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/slab.h>

#include "ep1_cf02a_core.h"
#include "ep1_cf02a_cmd_def.h"
#include "ep1_cf02a_msg.h"

/*!
 * @brief init stats
 */
void ep1_cf02a_init_stats(ep1_cf02a_stats_t *stats)
{
	stats->kt = ktime_get_real();
	stats->timestamp = 0;
	stats->num = 0;
	stats->total_num = 0;
}

/*!
 * @brief update stats
 */
static void ep1_cf02a_update_stats(ep1_cf02a_stats_t *stats, unsigned long now, u32 count)
{
	if (count == 0) {
		return;
	}

	stats->num = count - stats->total_num;
	stats->total_num = count;
	stats->timestamp = now;
	stats->kt = ktime_get_real();
}

static void ep1_cf02a_dispatch_msg_common_response(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg,
						   int request_id)
{
	if (dev->rx_complete.id == request_id) {
		DMSG("complete! <id:0x%02x> length=%d", msg->id, APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
		dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
		memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
		complete(&dev->rx_complete.complete);
	}
}

static void ep1_cf02a_dispatch_msg_notify_can_summary(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	ep1_cf02a_payload_notify_recv_can_summary_t summary;
	int result;

	result = ep1_cf02a_msg_parse_notify_recv_can_summary(msg->payload, msg->payload_size, &summary);
	if (result != RESULT_Success) {
		EMSG("ep1_cf02a_msg_parse_notify_recv_can_summary().. Error");
		return;
	}

	if (summary.frame.can_id & CAN_ERR_FLAG) {
		ep1_cf02a_update_stats(&unique_data->summary.err, jiffies, summary.rx_count);
	} else if (summary.frame.can_id & CAN_RTR_FLAG) {
		if (summary.frame.can_id & CAN_EFF_FLAG) {
			ep1_cf02a_update_stats(&unique_data->summary.rtr_ext, jiffies, summary.rx_count);
		} else {
			ep1_cf02a_update_stats(&unique_data->summary.rtr_std, jiffies, summary.rx_count);
		}
	} else {
		if (summary.frame.can_id & CAN_EFF_FLAG) {
			ep1_cf02a_update_stats(&unique_data->summary.dat_ext, jiffies, summary.rx_count);
		} else {
			ep1_cf02a_update_stats(&unique_data->summary.dat_std, jiffies, summary.rx_count);
		}
	}
}

/*!
 * @brief dispatch message
 */
int ep1_cf02a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	switch (msg->id) {
	case EP1_CF02A_CMD_NotifyRecvCANFrame:
		apt_usbtrx_ringbuffer_write(&dev->rx_data, msg->payload, msg->payload_size);
		wake_up_interruptible(&dev->rx_data.wq);
		break;
	case EP1_CF02A_CMD_NotifyStoreDataRecvCanFrame:
		apt_usbtrx_ringbuffer_write(&unique_data->rx_store_data, msg->payload, msg->payload_size);
		wake_up_interruptible(&unique_data->rx_store_data.wq);
		break;
	case EP1_CF02A_CMD_ResponseGetSilentMode:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetSilentMode);
		break;
	case EP1_CF02A_CMD_ResponseGetFDMode:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetFDMode);
		break;
	case EP1_CF02A_CMD_ResponseGetISOMode:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetISOMode);
		break;
	case EP1_CF02A_CMD_ResponseGetBitTiming:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetBitTiming);
		break;
	case EP1_CF02A_CMD_ResponseGetDataBitTiming:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetDataBitTiming);
		break;
	case EP1_CF02A_CMD_ResponseGetTxRxControl:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetTxRxControl);
		break;
	case EP1_CF02A_CMD_ResponseGetCANClock:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetCANClock);
		break;
	case EP1_CF02A_CMD_ResponseGetDeviceTimestampResetTime:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetDeviceTimestampResetTime);
		break;
	case EP1_CF02A_CMD_ResponseGetRTCTime:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetRTCTime);
		break;
	case EP1_CF02A_CMD_ResponseGetCurrentStoreDataState:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetCurrentStoreDataState);
		break;
	case EP1_CF02A_CMD_ResponseGetStoreDataIDListCount:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreDataIDListCount);
		break;
	case EP1_CF02A_CMD_ResponseGetStoreDataID:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreDataID);
		break;
	case EP1_CF02A_CMD_ResponseGetStoreDataMeta:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreDataMeta);
		break;
	case EP1_CF02A_CMD_ResponseGetStoreDataRxControl:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreDataRxControl);
		break;
	case EP1_CF02A_CMD_ResponseGetCapabilities:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetCapabilities);
		break;
	case EP1_CF02A_CMD_NotifyCANSummary:
		ep1_cf02a_dispatch_msg_notify_can_summary(dev, data, msg);
		break;
	default:
		WMSG("msg in not support, <id:0x%02x>", msg->id);
		return RESULT_Failure;
	}
	return RESULT_Success;
}

/*!
 * @brief write bulk callback
 */
void ep1_cf02a_write_bulk_callback(struct urb *urb)
{
	return;
}
