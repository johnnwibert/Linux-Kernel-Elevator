/* Deterministic requests for checking FIFO, weight, and invalid inputs. */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user/wrappers.h"

int main(int argc, char **argv)
{
	int values[3], i;
	char *end;
	long value, ret;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s start_floor destination_floor type\n", argv[0]);
		return 1;
	}
	for (i = 0; i < 3; i++) {
		errno = 0;
		value = strtol(argv[i + 1], &end, 10);
		if (errno || end == argv[i + 1] || *end || value < INT_MIN || value > INT_MAX) {
			fprintf(stderr, "Arguments must be integers.\n");
			return 1;
		}
		values[i] = (int)value;
	}
	ret = issue_request(values[0], values[1], values[2]);
	printf("Issue (%d, %d, %d) returned %ld\n", values[0], values[1], values[2], ret);
	if (ret == -1)
		perror("issue_request");
	return ret == 0 ? 0 : 1;
}
