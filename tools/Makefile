
DESTDIR ?= /usr/local

subdirs := apt_usbtrx_enablets apt_usbtrx_resetts apt_usbtrx_serial_no

subdirs += ep1_cf02a/ep1_cf02a_set_host_resetts \
		   ep1_cf02a/ep1_cf02a_get_rtc_time \
		   ep1_cf02a/ep1_cf02a_set_rtc_time \
		   ep1_cf02a/ep1_cf02a_get_store_data_list \
		   ep1_cf02a/ep1_cf02a_delete_store_data \
		   ep1_cf02a/ep1_cf02a_init_store_data_media \

target_files := \
	apt_usbtrx_fwupdate.py \
	apt_usbtrx_timesync_all.sh

.PHONY: $(subdirs) $(target_files)

all: $(subdirs)

$(subdirs):
	make -C $@ $(MAKECMDGOALS)

clean: $(subdirs)

install uninstall: $(subdirs) $(target_files)

ifeq ($(MAKECMDGOALS), install)
$(target_files):
	install -d $(DESTDIR)/bin
	install $@ $(DESTDIR)/bin
endif

ifeq ($(MAKECMDGOALS), uninstall)
$(target_files):
	$(RM) $(DESTDIR)/bin/$(notdir $@)
endif
