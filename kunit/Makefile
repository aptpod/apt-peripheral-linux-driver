obj-y := apt_usbtrx.o
apt_usbtrx-objs := 	apt_usbtrx_main.o \
					apt_usbtrx_core.o \
					apt_usbtrx_fops.o \
					apt_usbtrx_cmd.o \
					apt_usbtrx_msg.o \
					apt_usbtrx_ringbuffer.o \
					apt_usbtrx_sysfs.o

apt_usbtrx-objs += 	ap_ct2a/ap_ct2a_main.o \
					ap_ct2a/ap_ct2a_core.o \
					ap_ct2a/ap_ct2a_fops.o \
					ap_ct2a/ap_ct2a_cmd.o \
					ap_ct2a/ap_ct2a_msg.o \
					ap_ct2a/ap_ct2a_sysfs.o

apt_usbtrx-objs += 	ep1_ag08a/ep1_ag08a_main.o \
					ep1_ag08a/ep1_ag08a_core.o \
					ep1_ag08a/ep1_ag08a_fops.o \
					ep1_ag08a/ep1_ag08a_cmd.o \
					ep1_ag08a/ep1_ag08a_msg.o \
					ep1_ag08a/ep1_ag08a_sysfs.o

apt_usbtrx-objs +=  ep1_ch02a/ep1_ch02a_main.o \
					ep1_ch02a/ep1_ch02a_fops.o \
					ep1_ch02a/ep1_ch02a_cmd.o \
					ep1_ch02a/ep1_ch02a_core.o \
					ep1_ch02a/ep1_ch02a_msg.o

apt_usbtrx-objs +=  mock_usb.o mock_can.o mock_net.o \
					mock_common.o \
					mock_ep1_ag08a.o \
					mock_ep1_ch02a.o

ccflags-y += -DUNIT_TEST
