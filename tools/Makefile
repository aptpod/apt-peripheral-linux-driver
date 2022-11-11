
DESTDIR ?= /usr/local

subdirs := apt_usbtrx_enablets apt_usbtrx_resetts apt_usbtrx_serial_no

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
