
KVER ?= `uname -r`
KERNEL_SRC ?= /lib/modules/$(KVER)/build
PWD := $(shell pwd)

MODNAME = apt_usbtrx
MODDIR = /lib/modules/$(KVER)/kernel/drivers/misc

obj-m := $(MODNAME).o
ccflags-y += -I$(PWD)
#ccflags-y += -DDEBUG
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
					ep1_ag08a/ep1_ag08a_sysfs.o \
					ep1_ag08a/ep1_ag08a_iio.o

apt_usbtrx-objs +=  ep1_ch02a/ep1_ch02a_main.o \
                    ep1_ch02a/ep1_ch02a_fops.o \
					ep1_ch02a/ep1_ch02a_cmd.o \
					ep1_ch02a/ep1_ch02a_core.o \
					ep1_ch02a/ep1_ch02a_msg.o

all:
#	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

netdev:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) ccflags-y+="-DSUPPORT_NETDEV" modules

install:
	install -p -m 644 $(MODNAME).ko $(MODDIR)
	/sbin/depmod -a ${KVER}

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f *~


