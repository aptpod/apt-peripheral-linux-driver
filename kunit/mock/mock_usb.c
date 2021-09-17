#include <linux/types.h>

#include "apt_usbtrx_core.h"
#include "ep1_ag08a/ep1_ag08a_cmd_def.h"
#include "ep1_ch02a/ep1_ch02a_cmd_def.h"

#include "mock_ep1_ag08a.h"
#include "mock_ep1_ch02a.h"

/* drivers/usb/core/usb.c */
struct usb_interface *usb_find_interface(struct usb_driver *drv, int minor)
{
	/* not implimented */
	return NULL;
}
void *usb_alloc_coherent(struct usb_device *dev, size_t size, gfp_t mem_flags, dma_addr_t *dma)
{
	/* not implimented */
	return NULL;
}
void usb_free_coherent(struct usb_device *dev, size_t size, void *addr, dma_addr_t dma)
{
}

/* drivers/usb/core/urb.c */
struct urb *usb_alloc_urb(int iso_packets, gfp_t mem_flags)
{
	/* not implimented */
	return NULL;
}
void usb_free_urb(struct urb *urb)
{
}
void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor)
{
}
void usb_unanchor_urb(struct urb *urb)
{
}
int usb_submit_urb(struct urb *urb, gfp_t mem_flags)
{
	/* not implimented */
	return -1;
}
void usb_kill_anchored_urbs(struct usb_anchor *anchor)
{
}

/* drivers/usb/core/file.c */
int usb_register_dev(struct usb_interface *intf, struct usb_class_driver *class_driver)
{
	/* not implimented */
	return -1;
}
void usb_deregister_dev(struct usb_interface *intf, struct usb_class_driver *class_driver)
{
}

/* drivers/usb/core/driver.c */
int usb_register_driver(struct usb_driver *new_driver, struct module *owner, const char *mod_name)
{
	/* not implimented */
	return -1;
}
void usb_deregister(struct usb_driver *driver)
{
}

/* drivers/usb/core/message.c */
int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout)
{
	u8 *pdata = data;
	u8 cmd_id = pdata[2];

	/* get dev */
	apt_usbtrx_dev_t *dev = usb_dev->dev.driver_data;

	switch (cmd_id) {
	case EP1_AG08A_CMD_GetStatus: {
		apt_usbtrx_msg_t msg;
		struct mock_ep1_ag08a_response_get_status resp_data;
		build_response_get_status(&resp_data, &msg);

		/* proc interrupt context */
		apt_usbtrx_dispatch_msg(dev, (u8 *)&resp_data, &msg);
		break;
	}
	case EP1_CH02A_CMD_GetStatus: {
		apt_usbtrx_msg_t msg;
		struct mock_ep1_ch02a_response_get_status resp_data;
		build_response_ep1_ch02a_get_status(&resp_data, &msg);

		/* proc interrupt context */
		apt_usbtrx_dispatch_msg(dev, (u8 *)&resp_data, &msg);
		break;
	}
	case EP1_CH02A_CMD_SetBitTiming: {
		apt_usbtrx_msg_t msg;
		struct mock_msg_ack_nack ack;

		create_mock_ack(&ack, EP1_CH02A_CMD_SetBitTiming);

		msg.id = APT_USBTRX_CMD_ACK;
		msg.payload_size = 1;
		msg.payload[0] = ack.recv_cmd;

		mock_request_ep1_ch02a_set_bit_timing(data, len);

		/* proc interrupt context */
		apt_usbtrx_dispatch_msg(dev, (u8 *)&ack, &msg);
		break;
	}
	case EP1_CH02A_CMD_GetBitTiming: {
		apt_usbtrx_msg_t msg;
		struct mock_ep1_ch02a_response_get_bit_timing resp_data;
		build_response_ep1_ch02a_get_bit_timing(&resp_data, &msg);

		/* proc interrupt context */
		apt_usbtrx_dispatch_msg(dev, (u8 *)&resp_data, &msg);
		break;
	}
	}

	return 0;
}
int usb_string(struct usb_device *dev, int index, char *buf, size_t size)
{
	/* not implimented */
	return -1;
}
