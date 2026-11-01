#ifndef ELEVATOR_WRAPPERS_H
#define ELEVATOR_WRAPPERS_H

#include <unistd.h>
#include <sys/syscall.h>

#define ELEVATOR_START_NR 548
#define ELEVATOR_REQUEST_NR 549
#define ELEVATOR_STOP_NR 550

static inline long start_elevator(void)
{
	return syscall(ELEVATOR_START_NR);
}

static inline long issue_request(int start, int dest, int type)
{
	return syscall(ELEVATOR_REQUEST_NR, start, dest, type);
}

static inline long stop_elevator(void)
{
	return syscall(ELEVATOR_STOP_NR);
}

#endif
