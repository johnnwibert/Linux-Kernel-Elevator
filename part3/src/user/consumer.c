#include <stdio.h>
#include <string.h>
#include "wrappers.h"

int main(int argc, char **argv)
{
	long ret;

	if (argc != 2 || (strcmp(argv[1], "--start") && strcmp(argv[1], "--stop"))) {
		fprintf(stderr, "Usage: %s --start|--stop\n", argv[0]);
		return 1;
	}
	if (!strcmp(argv[1], "--start")) {
		ret = start_elevator();
		printf("Start elevator returned %ld\n", ret);
	} else {
		ret = stop_elevator();
		printf("Stop elevator returned %ld\n", ret);
	}
	if (ret == -1)
		perror("elevator syscall");
	return ret == -1 ? 1 : 0;
}
