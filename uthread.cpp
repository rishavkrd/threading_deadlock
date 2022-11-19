/* Write your core functionality in this file. */

#include <pthread.h>
#include "uthread.h"

pthread_t list[500];
int index=0;
int* ret;

void uthread_init(void)
{
	return;
}

void uthread_create(void (*func) (void*), void* arg)
{	
	pthread_t t1;
	//void* (*func) (void*) f = &func;
	
	pthread_create(&t1, NULL,(void* (*) (void*)) func, (void*) arg);
	list[index]=t1;
	index++;

	// pthread_create(&thread1, NULL, &access_memory, (void*) 1);
}

void uthread_exit(void)
{
	pthread_exit(&ret);
}

void uthread_yield(void)
{
	pthread_yield();
}

void uthread_cleanup(void)
{
	for(int i=0;i<=index;i++)
		{
			pthread_join(list[i],NULL);
		}
}

void uthread_set_policy(enum uthread_policy policy)
{
}

void uthread_set_param(int param)
{
}
