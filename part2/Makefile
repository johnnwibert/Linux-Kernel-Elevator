ifneq ($(KERNELRELEASE),)
obj-m := my_timer.o
my_timer-objs := src/my_timer.o
else
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

load: all
	sudo insmod my_timer.ko

unload:
	sudo rmmod my_timer

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all load unload clean
endif
