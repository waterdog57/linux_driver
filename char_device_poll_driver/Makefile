CONFIG_MODULE_SIG=n
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build

obj-m := my_driver.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
