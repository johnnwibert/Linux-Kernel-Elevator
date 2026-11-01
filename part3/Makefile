ifneq ($(KERNELRELEASE),)

obj-m += elevator.o

elevator-y := src/elevator/elevator_main.o \
              src/elevator/elevator_thread.o \
              src/elevator/elevator_requests.o

else

KDIR ?= /lib/modules/$(shell uname -r)/build

.DEFAULT_GOAL := all

all: module tools

module:
	$(MAKE) -C "$(KDIR)" M="$(CURDIR)" modules

tools:
	$(MAKE) -C src/user

clean:
	$(MAKE) -C "$(KDIR)" M="$(CURDIR)" clean
	$(MAKE) -C src/user clean

.PHONY: all module tools clean

endif