#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "wrappers.h"

/* Rejection sampling avoids modulo bias for both four pets and five floors. */
static int rnd(int min, int max)
{
	unsigned int span = (unsigned int)(max - min + 1);
	unsigned long range = (unsigned long)RAND_MAX + 1;
	unsigned long limit = range - range % span;
	unsigned long value;

	do {
		value = (unsigned long)rand();
	} while (value >= limit);
	return min + (int)(value % span);
}

int main(int argc, char **argv)
{
	char *end;
	long num, i, ret;
	int type, start, dest, failed = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s number_of_pets\n", argv[0]);
		return 1;
	}
	errno = 0;
	num = strtol(argv[1], &end, 10);
	if (errno || end == argv[1] || *end || num < 0 || num > INT_MAX) {
		fprintf(stderr, "number_of_pets must be an integer from 0 to %d\n", INT_MAX);
		return 1;
	}
	srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
	for (i = 0; i < num; i++) {
		type = rnd(0, 3);
		start = rnd(1, 5);
		do {
			dest = rnd(1, 5);
		} while (dest == start);
		ret = issue_request(start, dest, type);
		printf("Issue (%d, %d, %d) returned %ld\n", start, dest, type, ret);
		if (ret != 0) {
			failed = 1;
			if (ret == -1)
				perror("issue_request");
		}
	}
	return failed;
}
