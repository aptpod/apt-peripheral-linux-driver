MOD = ./module
TOOLS ?= ./tools

.PHONY: module tools

all: module tools

module:
	$(MAKE) -C $(MOD) netdev

tools:
	$(MAKE) -C $(TOOLS)

install: install_module install_tools

install_module:
	$(MAKE) install -C $(MOD)

install_tools:
	$(MAKE) install -C $(TOOLS)

uninstall: uninstall_module uninstall_tools

uninstall_module:
	$(MAKE) uninstall -C $(MOD)

uninstall_tools:
	$(MAKE) uninstall -C $(TOOLS)

clean: clean_module clean_tools

clean_module:
	$(MAKE) clean -C $(MOD)

clean_tools:
	$(MAKE) clean -C $(TOOLS)
