#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "elevator.h"

int elevator_thread_fn(void *data)
{
    // kthread_should_stop() returns true if kthread_stop() has been called
    while (!kthread_should_stop())
    {
        mutex_lock(&elev.lock);
        /*
        1. unload riders whos dest is current floor
        2. load waiting pets from this floor, FIFO, if count/weight allows
        3. pick next direction; 
        */
        mutex_unlock(&elev.lock);
        msleep(1000);
    }
    return 0;
}