obj-m += my_kobject.o
KVERSION := $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
	@echo "Module installed. You may need to run 'depmod -a' to update module dependencies."
uninstall:
	@echo "Uninstalling module..."
	@sudo rmmod my_kobject
	@echo "Module removed. You may need to run 'depmod -a' to update module dependencies."
.PHONY: all clean install uninstall
# This Makefile is used to build, clean, install, and uninstall the my_kobject kernel module.
# It uses the kernel build system to compile the module against the currently running kernel.
# The 'all' target builds the module, 'clean' removes build artifacts,
# 'install' installs the module, and 'uninstall' removes it.
# The KDIR variable points to the kernel build directory, and PWD is the current working directory.
