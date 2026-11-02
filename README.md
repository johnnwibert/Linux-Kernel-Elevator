# Pet Elevator Project (Project 2)

## Description
This project aims to provide hands-on experience with system calls, kernel programming, concurrency, synchronization, and elevator scheduling algorithms. It consists of parts that are built upon each other to deepen your knowledge and skills in these areas. You can do the work on your own computer (using a hypervisor like VirtualBox) or the CS Proxmox cluster. Using a virtual machine allows you to modify and boot the Linux kernel with the ability to roll back the bootable image should any bugs crash the OS and render it unusable.

In Part 1, by adding system calls to a C program and verifying their correctness using the strace tool, you gain hands-on experience with system call integration and learn about the available system calls for your machine. This part lays the foundation for understanding how system calls interact with the kernel.

Part 2 takes you further into kernel programming. You will develop a kernel module called my_timer that retrieves and stores the current time using the ktime_get_real_ts64() function. This module creates a proc entry and allows you to read the current time and elapsed time since the last call. This part helps you understand how kernel modules work, how to interact with kernel functions, and how to use proc interfaces for communication.

Part 3 focuses on a more complex task: implementing a scheduling algorithm for a pet elevator. You create a kernel module representing the elevator, supporting operations like starting, stopping, and issuing requests. The module also provides a /proc/elevator entry to display important elevator information. This part challenges you to manage concurrency, synchronization, and efficient scheduling within the kernel environment.

Each part of the project builds upon the knowledge and skills gained in the previous parts. Part 1 introduces you to system calls and their integration, which forms the basis for kernel programming in Part 2. Part 2 expands your understanding of kernel modules and communication through proc interfaces, setting the stage for the more advanced concepts explored in Part 3.

By completing this project, you acquire practical experience in system calls, kernel programming, concurrency, synchronization, and scheduling algorithms. These are essential skills for developing efficient and robust software systems, particularly in operating systems and low-level programming domains. Understanding system calls and kernel programming enables you to interact with and extend the functionality of the operating system, while concurrency, synchronization, and scheduling concepts are crucial for efficient resource management and multitasking in complex systems.

## Group Members
- **Robert Began**: rgb23a@fsu.edu
- **Armani Ruiz**: arj22c@fsu.edu
- **John Noel Wibert**: jnw22c@fsu.edu
## Division of Labor

### Part 1: [System-Call Tracing]
- **Responsibilities**: Added five system calls and traced them with strace.
- **Assigned to**: [Robert Began]

### Part 2: [Timer Kernel Module]
- **Responsibilities**: Built the `/proc/timer` kernel module.
- **Assigned to**: [Robert Began]

### Part 3: [Elevator Module]
- **Responsibilities**:
Add System Calls - Armani Ruiz,
Compile and Install the Kernel - Armani Ruiz,
Test System Calls - Armani Ruiz,
Implement Elevator - John Noel Wibert,
Write to the Proc File - John Noel Wibert,
Test Elevator - John Noel Wibert,
- **Assigned to**: Armani Ruiz, John Noel Wibert

## File Listing
```
root/
├── part1/
| ├── empty.c
| ├── empty.trace
| ├── part1.c
| ├── part1.trace
| ├── Makefile
├── part2/
| ├── src/
| |  ├── my_timer.c
| ├── Makefile
├── part3/
| ├── src
| |  ├── elevator/
| |  |  ├── elevator.h
| |  |  ├── elevator_main.c
| |  |  ├── elevator_requests.c
| |  |  ├── elevator_thread.c
| |  |  ├── Makefile
| |  ├── tests/
| |  |  ├── request.c
| |  |  ├── syscheck.c
| |  |  ├── test-syscalls.c
| |  |  ├── Makefile
| |  ├── user/
| |  |  ├── consumer.c
| |  |  ├── producer.c
| |  |  ├── wrappers.h
| |  |  ├── Makefile
| ├── syscalls.c
| ├── Makefile
├── Makefile
└── README.md


```
## How to Compile & Execute

### Requirements
- **Compiler**: GCC and GNU Make
- Linux kernel version 7.2.12
- strace for Part 1
- Kernel module modifiers (insmod, rmmod, lsmod)
- Installation of custom kernel system calls:
    - 548: start_elevator()
    - 549: issue_request()
    - 550: stop_elevator()

### Compilation
Run "make" from the root of the project

### Execution
From project root:
sudo insmod part3/elevator.ko
(for monitoring, from another terminal): watch -n 0.5 cat /proc/elevator
to start: ./part3/src/user/consumer --start
For generating pet requests: ./part3/src/user/producer (number of pets)
to stop: ./part3/src/user/consumer --stop
cleanup: make clean

## Development Log
Each member records their contributions here.

### Robert Began

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2026-10-17 | Wrote empty.c, part1.c and the Part 1 Makefile; generated and verified strace output (5 extra system calls) |
| 2026-10-17 | Wrote my_timer kernel module and Part 2 Makefile |
| 2026-10-22 | Pushed Part 1 and Part 2 to the group repository |

### Armani Ruiz

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2026-10-25 | Downloaded and prepped local VM for development, downloaded linux kernel, wrote new syscalls (start_elevator, issue_request, stop_elevator) and configured kernel to compile with them. Began kernel compilation. |
| 2026-10-26 | Completed kernel compilation. Ran some brief tests to ensure that the kernel compiled properly and the custom syscalls were working as intended. Wrote a rough skeleton for the elevator implementation (namely kthreads) before passing the rest off to John.  |


### John Noel Wibert

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2026-10-20 | Downloaded and prepped virtual machine for future development  |
| 2026-10-27 | Installation and preparation of custom kernel  |
| 2026-10-30 | Development of elevator implementation  |
| 2026-10-31 | Development and testing of elevator implementation  |
| 2026-11-01 | Finishing touches on Part 3, file structuring, uploading and documenting work  |


## Meetings
Document in-person meetings, their purpose, and what was discussed.

Discussion was ongoing through direct messaging and brief in-person summarizations of progress.

