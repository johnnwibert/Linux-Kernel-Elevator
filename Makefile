.DEFAULT_GOAL := all

all: part1 part2 part3

part1:
	$(MAKE) -C part1

part2:
	$(MAKE) -C part2

part3:
	$(MAKE) -C part3

clean:
	$(MAKE) -C part1 clean
	$(MAKE) -C part2 clean
	$(MAKE) -C part3 clean

.PHONY: all part1 part2 part3 clean