/* This is the main body of the test program.
 * DO NOT write any of your core functionality here.
 * You are free to modify this file but be aware that
 * this file will be replace during grading. */

#include <stdio.h>
#include <unistd.h>
#include "uthread.h"

void thread1(void* arg)
{
    for (int i = 0; i < 100; i++) {
        printf("This is thread 1, i: %d\n",i);
        usleep(1000);
    }
    uthread_exit();
}

void thread2(void* arg)
{
    for (int i = 0; i < 100; i++) {
        printf("This is thread 2, i: %d\n",i);
        usleep(1000);
    }
    uthread_exit();
}

int main(int argc, const char** argv)
{
    uthread_init();
    uthread_create(thread1, NULL);
    uthread_create(thread2, NULL);
    uthread_cleanup();
    return 0;
}
