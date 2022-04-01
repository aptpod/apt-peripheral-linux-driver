// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for sending and receiving data to and from
 * EDGEPLANT CAN-USB interface.
 *
 * Copyright (C) 2020 aptpod Inc.
 */

#include <linux/version.h>
#include <linux/can/dev.h>

#include "ap_ct2a_core.h"
#include "ap_ct2a_cmd_def.h"
#include "ap_ct2a_msg.h"

/*!
 * @brief init stats
 */
void apt_usbtrx_init_stats(apt_usbtrx_stats_t *stats)
{
	stats->kt = ktime_get_real();
	stats->timestamp = 0;
	stats->num = 0;
	stats->total_num = 0;
}

/*!
 * @brief update stats
 */
static void apt_usbtrx_update_stats(apt_usbtrx_stats_t *stats, unsigned long now, u32 count)
{
	if (count == 0) {
		return;
	}

	stats->num = count - stats->total_num;
	stats->total_num = count;
	stats->timestamp = now;
	stats->kt = ktime_get_real();
}

#ifdef SUPPORT_NETDEV
/*!
 * @brief dispatch message
 */
static void apt_usbtrx_unique_can_rx_can_msg(apt_usbtrx_dev_t *dev,
					     apt_usbtrx_payload_notify_recv_can_frame_t *recv_can_frame)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	struct net_device *netdev = unique_data->netdev;
	struct sk_buff *skb;
	struct can_frame *cf;
	struct timespec64 ts;
	struct skb_shared_hwtstamps *hwts;

	if (!netif_device_present(netdev)) {
		return;
	}

	skb = alloc_can_skb(unique_data->netdev, &cf);
	if (skb == NULL) {
		return;
	}

	apt_usbtrx_convert_timestamp_to_timespec64(&recv_can_frame->timestamp, &ts);
	hwts = skb_hwtstamps(skb);
	hwts->hwtstamp = timespec64_to_ktime(ts);

	cf->can_id = recv_can_frame->id[0];
	cf->can_id |= recv_can_frame->id[1] << 8;
	cf->can_id |= recv_can_frame->id[2] << 16;
	cf->can_id |= recv_can_frame->id[3] << 24;
	cf->can_dlc = recv_can_frame->dlc & APT_USBTRX_DLC_MASK;

	memcpy(cf->data, &recv_can_frame->data[0], cf->can_dlc);

	netdev->stats.rx_packets++;
	netdev->stats.rx_bytes += cf->can_dlc;

	netif_rx(skb);

	return;
}
#endif

/*!
 * @brief dispatch message
 */
int apt_usbtrx_unique_can_dispatch_msg(apt_usbtrx_dev_t *dev, u8 *data, apt_usbtrx_msg_t *msg)
{
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);

	switch (msg->id) {
	case APT_USBTRX_CMD_NotifyRecvCANFrame: {
		bool empty;
		int if_type = atomic_read(&unique_data->if_type);

		if (if_type == APT_USBTRX_CAN_IF_TYPE_FILE) {
			empty = apt_usbtrx_ringbuffer_is_empty(&dev->rx_data);
			apt_usbtrx_ringbuffer_write(&dev->rx_data, msg->payload, msg->payload_size);

			if (empty == true) {
				wake_up_interruptible(&dev->rx_data.wq);
			}
		} else if (if_type == APT_USBTRX_CAN_IF_TYPE_NET) {
#ifdef SUPPORT_NETDEV
			apt_usbtrx_unique_can_rx_can_msg(dev,
							 (apt_usbtrx_payload_notify_recv_can_frame_t *)msg->payload);
#endif
		}
		break;
	}
	case APT_USBTRX_CMD_NotifyCANSummary: {
		u32 count;
		struct can_frame frame;
		int result;

		result = apt_usbtrx_msg_parse_notify_recv_can_summary(msg->payload, msg->payload_size, &count, &frame);
		if (result != RESULT_Success) {
			EMSG("apt_usbtrx_msg_parse_notify_recv_can_summary().. Error");
			break;
		}

		if (frame.can_id & CAN_ERR_FLAG) {
			apt_usbtrx_update_stats(&unique_data->summary.err, jiffies, count);
		} else if (frame.can_id & CAN_RTR_FLAG) {
			if (frame.can_id & CAN_EFF_FLAG) {
				apt_usbtrx_update_stats(&unique_data->summary.rtr_ext, jiffies, count);
			} else {
				apt_usbtrx_update_stats(&unique_data->summary.rtr_std, jiffies, count);
			}
		} else {
			if (frame.can_id & CAN_EFF_FLAG) {
				apt_usbtrx_update_stats(&unique_data->summary.dat_ext, jiffies, count);
			} else {
				apt_usbtrx_update_stats(&unique_data->summary.dat_std, jiffies, count);
			}
		}
		break;
	}
	case APT_USBTRX_CMD_ResponseGetStatus: {
		if (dev->rx_complete.id == APT_USBTRX_CMD_GetStatus) {
			DMSG("complete!, <id:0x%02x> length=%d", msg->id,
			     APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size));
			dev->rx_complete.data_size = APT_USBTRX_PAYLOAD_LENGTH_TO_MSG(msg->payload_size);
			memcpy(dev->rx_complete.buffer, data, dev->rx_complete.data_size);
			complete(&dev->rx_complete.complete);
		}
		break;
	}
	default:
		return RESULT_Failure;
	}

	return RESULT_Success;
}

/*!
 * @brief write bulk callback
 */
void apt_usbtrx_unique_can_write_bulk_callback(struct urb *urb)
{
#ifdef SUPPORT_NETDEV
	apt_usbtrx_dev_t *dev = urb->context;
	apt_usbtrx_unique_data_can_t *unique_data = get_unique_data(dev);
	struct net_device *netdev = unique_data->netdev;
	struct net_device_stats *stats = &netdev->stats;
	apt_usbtrx_candev_t *candev = netdev_priv(netdev);

	if (!netif_device_present(netdev)) {
		return;
	}

	stats->tx_packets++;
	stats->tx_bytes += candev->tx_data_size;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0)
	can_get_echo_skb(netdev, 0, NULL);
#else
	can_get_echo_skb(netdev, 0);
#endif
	netif_wake_queue(netdev);
#endif

	return;
}

#if 0 /* not use */
/*!
 * @brief show summary
 */
static void apt_usbtrx_show_summary(apt_usbtrx_can_summary_t *summary, unsigned long now)
{
	u32 dat_std;
	u32 dat_ext;
	u32 rtr_std;
	u32 rtr_ext;
	u32 err;

	if (time_after(now, summary->dat_std.timestamp + msecs_to_jiffies(1000))) {
		dat_std = 0;
	} else {
		dat_std = summary->dat_std.num;
	}

	if (time_after(now, summary->dat_ext.timestamp + msecs_to_jiffies(1000))) {
		dat_ext = 0;
	} else {
		dat_ext = summary->dat_ext.num;
	}

	if (time_after(now, summary->rtr_std.timestamp + msecs_to_jiffies(1000))) {
		rtr_std = 0;
	} else {
		rtr_std = summary->rtr_std.num;
	}

	if (time_after(now, summary->rtr_ext.timestamp + msecs_to_jiffies(1000))) {
		rtr_ext = 0;
	} else {
		rtr_ext = summary->rtr_ext.num;
	}

	if (time_after(now, summary->err.timestamp + msecs_to_jiffies(1000))) {
		err = 0;
	} else {
		err = summary->err.num;
	}

	IMSG("can summary: DAT-STD(%u), DAT-EXT(%u), RTR-STD(%u), RTR-EXT(%u), ERR(%u)", dat_std, dat_ext, rtr_std,
	     rtr_ext, err);
}
#endif
