/* This is the main body of the test program.
 * DO NOT write any of your core functionality here.
 * You are free to modify this file but be aware that
 * this file will be replace during grading. */

#include <stdio.h>
#include <unistd.h>
#include "uthread.h"

void thread1(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 1, i: %d\n",i);
        usleep(1000);
    }
    uthread_exit();
}

void thread2(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 2, i: %d\n",i);
        usleep(1000);
    }
    uthread_exit();
}

int main(int argc, const char** argv)
{
    uthread_set_policy(UTHREAD_PRIORITY);
    printf("test: Set policy\n");
    uthread_init();
    printf("test: init\n");
    uthread_create(thread1, NULL);
    printf("test: thread 1\n");
    uthread_create(thread2, NULL);
    printf("test: thread 2\n");
    uthread_cleanup();
    return 0;
}
