obj-m += syscheck.o
KDIR ?= /lib/modules/$(shell uname -r)/build
CC = gcc

all: module test-syscalls request

module:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

test-syscalls: test-syscalls.c ../user/wrappers.h
	$(CC) -D_DEFAULT_SOURCE -Wall -Wextra -std=c11 -o $@ $<

request: request.c ../user/wrappers.h
	$(CC) -D_DEFAULT_SOURCE -Wall -Wextra -std=c11 -o $@ $<

run: test-syscalls
	./test-syscalls

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
	rm -f test-syscalls request

.PHONY: all module run clean
