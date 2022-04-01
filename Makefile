MOD = ./module
TOOLS ?= ./tools

all:
	$(MAKE) -C $(MOD) netdev
	$(MAKE) -C $(TOOLS)/apt_usbtrx_enablets
	$(MAKE) -C $(TOOLS)/apt_usbtrx_resetts
	$(MAKE) -C $(TOOLS)/apt_usbtrx_serial_no

install:
	$(MAKE) install -C $(MOD)
	$(MAKE) install -C $(TOOLS)/apt_usbtrx_enablets
	$(MAKE) install -C $(TOOLS)/apt_usbtrx_resetts
	$(MAKE) install -C $(TOOLS)/apt_usbtrx_serial_no

clean:
	$(MAKE) clean -C $(MOD)
	$(MAKE) clean -C $(TOOLS)/apt_usbtrx_enablets
	$(MAKE) clean -C $(TOOLS)/apt_usbtrx_resetts
	$(MAKE) clean -C $(TOOLS)/apt_usbtrx_serial_no
