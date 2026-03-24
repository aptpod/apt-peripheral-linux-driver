// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 aptpod Inc.
 */

#include <linux/slab.h>
#include <linux/version.h>
#include <linux/can/dev.h>

#include "ep1_cf02a_core.h"
#include "ep1_cf02a_cmd_def.h"
#include "ep1_cf02a_msg.h"

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

#ifdef SUPPORT_NETDEV
/*!
 * @brief dispatch message
 */
static void ep1_cf02a_rx_can_msg(apt_usbtrx_dev_t *dev, ep1_cf02a_payload_notify_recv_can_frame_t *recv_can_frame)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	struct net_device *netdev = unique_data->netdev;
	ep1_cf02a_candev_t *candev = netdev_priv(netdev);
	struct sk_buff *skb;
	struct canfd_frame *cfd;
	struct can_frame *cf;
	struct timespec64 ts;
	struct skb_shared_hwtstamps *hwts;
	bool is_canfd;

	if (!netif_device_present(netdev)) {
		return;
	}

	apt_usbtrx_convert_timestamp_to_timespec64(&recv_can_frame->timestamp, &ts);
	ts = timespec64_sub(ts, candev->reset_ts);

	is_canfd = recv_can_frame->flags & EP1_CF02A_CAN_FRAME_FLAG_FDF;

	if (is_canfd) {
		skb = alloc_canfd_skb(netdev, &cfd);
		if (skb == NULL) {
			return;
		}

		hwts = skb_hwtstamps(skb);
		hwts->hwtstamp = timespec64_to_ktime(ts);

		cfd->can_id = recv_can_frame->id[0];
		cfd->can_id |= recv_can_frame->id[1] << 8;
		cfd->can_id |= recv_can_frame->id[2] << 16;
		cfd->can_id |= recv_can_frame->id[3] << 24;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
		cfd->len = can_fd_dlc2len(recv_can_frame->dlc & 0x0F);
#else
		cfd->len = can_dlc2len(recv_can_frame->dlc & 0x0F);
#endif

		cfd->flags = 0;
		if (recv_can_frame->flags & EP1_CF02A_CAN_FRAME_FLAG_BRS) {
			cfd->flags |= CANFD_BRS;
		}
		if (recv_can_frame->flags & EP1_CF02A_CAN_FRAME_FLAG_ESI) {
			cfd->flags |= CANFD_ESI;
		}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
		if (recv_can_frame->flags & EP1_CF02A_CAN_FRAME_FLAG_FDF) {
			cfd->flags |= CANFD_FDF;
		}
#endif
		memcpy(cfd->data, &recv_can_frame->data[0], cfd->len);

		atomic64_inc(&candev->rx_packets);
		atomic64_add(cfd->len, &candev->rx_bytes);
	} else {
		skb = alloc_can_skb(netdev, &cf);
		if (skb == NULL) {
			return;
		}

		hwts = skb_hwtstamps(skb);
		hwts->hwtstamp = timespec64_to_ktime(ts);

		cf->can_id = recv_can_frame->id[0];
		cf->can_id |= recv_can_frame->id[1] << 8;
		cf->can_id |= recv_can_frame->id[2] << 16;
		cf->can_id |= recv_can_frame->id[3] << 24;

		cf->can_dlc = recv_can_frame->dlc & 0x0F;
		memcpy(cf->data, &recv_can_frame->data[0], cf->can_dlc);

		atomic64_inc(&candev->rx_packets);
		atomic64_add(cf->can_dlc, &candev->rx_bytes);
	}

	netif_rx(skb);
}
#endif

/*!
 * @brief dispatch message
 */
int ep1_cf02a_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);

	switch (msg->id) {
	case EP1_CF02A_CMD_NotifyRecvCANFrame: {
		int if_type = atomic_read(&unique_data->if_type);

		if (if_type == EP1_CF02A_IF_TYPE_FILE) {
			apt_usbtrx_ringbuffer_write(&dev->rx_data, msg->payload, msg->payload_size);
			wake_up_interruptible(&dev->rx_data.wq);
		} else if (if_type == EP1_CF02A_IF_TYPE_NET) {
#ifdef SUPPORT_NETDEV
			ep1_cf02a_rx_can_msg(dev, (ep1_cf02a_payload_notify_recv_can_frame_t *)msg->payload);
#endif
		}
		break;
	}
	case EP1_CF02A_CMD_NotifyStoreDataRecvCanFrame: {
		ssize_t write_size =
			apt_usbtrx_ringbuffer_write(&unique_data->rx_store_data, msg->payload, msg->payload_size);
		if (write_size > 0) {
			atomic_add(write_size, &unique_data->received_store_data_size);
		}
		wake_up_interruptible(&unique_data->rx_store_data.wq);
		break;
	}
	case EP1_CF02A_CMD_NotifyStoreDataRecvCanFrameComplete:
		unique_data->notify_store_data_recv_can_frame_complete = true;
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
	case EP1_CF02A_CMD_ResponseGetStoreEnable:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreEnable);
		break;
	case EP1_CF02A_CMD_ResponseGetStoreMaxDuration:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetStoreMaxDuration);
		break;
	case EP1_CF02A_CMD_ResponseGetCapabilities:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetCapabilities);
		break;
	case EP1_CF02A_CMD_ResponseGetCanStatistics:
		ep1_cf02a_dispatch_msg_common_response(dev, data, msg, EP1_CF02A_CMD_GetCanStatistics);
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
#ifdef SUPPORT_NETDEV
	apt_usbtrx_dev_t *dev = urb->context;
	ep1_cf02a_unique_data_t *unique_data = get_unique_data(dev);
	struct net_device *netdev = unique_data->netdev;
	ep1_cf02a_candev_t *candev = netdev_priv(netdev);

	if (!netif_device_present(netdev)) {
		return;
	}

	atomic64_inc(&candev->tx_packets);
	atomic64_add(candev->tx_data_size, &candev->tx_bytes);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0)
	can_get_echo_skb(netdev, 0, NULL);
#else
	can_get_echo_skb(netdev, 0);
#endif
	netif_wake_queue(netdev);
#endif

	return;
}
