ifneq ($(KERNELRELEASE),)

obj-m += my_timer.o
my_timer-y := src/my_timer.o

else

KDIR ?= /lib/modules/$(shell uname -r)/build

.DEFAULT_GOAL := all

all:
	$(MAKE) -C "$(KDIR)" M="$(CURDIR)" modules

clean:
	$(MAKE) -C "$(KDIR)" M="$(CURDIR)" clean

.PHONY: all clean

endif