CC = gcc
CPPFLAGS += -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -std=c11

all: producer consumer

producer: producer.c wrappers.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ producer.c

consumer: consumer.c wrappers.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ consumer.c

clean:
	rm -f producer consumer

.PHONY: all clean
