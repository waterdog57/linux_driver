CONFIG_MODULE_SIG=n
BASEINCLUE ?= /lib/modules/$(shell uname -r)/build

obj-m := mydriver.o

all:
	$(MAKE) -C $(BASEINCLUE) M=$(PWD) modules;
clean:
	$(MAKE) -C $(BASEINCLUE) M=$(PWD) clean;