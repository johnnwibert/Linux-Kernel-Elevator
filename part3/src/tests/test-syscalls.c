#include <stdio.h>
#include "../user/wrappers.h"

static int check(const char *name, long result)
{
	if (result == 0) {
		printf("%s system call exists.\n", name);
		return 0;
	}
	printf("%s test failed (returned %ld).\n", name, result);
	if (result == -1)
		perror(name);
	return 1;
}

int main(void)
{
	int failures = 0;

	/* Run with syscheck.ko loaded, and elevator.ko unloaded. */
	failures += check("start_elevator", start_elevator());
	failures += check("issue_request", issue_request(1, 2, 3));
	failures += check("stop_elevator", stop_elevator());
	return failures ? 1 : 0;
}
