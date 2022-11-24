/* Write your core functionality in this file. */
// #include <iostream>
#include <pthread.h>
#include "uthread.h"
#include <cstdlib>

// using namespace std;
struct thread_info{
	void (*func) (void*);
	void* arg;
	int id;
	int priority;
};

class Node{
public:
	Node *next;
	struct thread_info val;

	Node(){
		// val=(struct thread_info)0;
		this->next=NULL;
	}
	Node(struct thread_info val){
		this->val = val;
		this->next = NULL;
	}
};
class LinkedList{
	Node* head;
	Node* tail;
public:
	LinkedList(){
		head=NULL;
		tail=NULL;
	}
	void push(struct thread_info t){
		Node *temp = (Node*) malloc(sizeof(Node));
		temp->val = t;
		temp->next = NULL;
		// temp->next = head;
		// this->head = temp;
		if(this->head==NULL){
			head=temp;
			tail=temp;
		}else{
			tail->next=temp;
			tail = tail->next;
		}
		return;
	}
	struct thread_info pop(){
		struct thread_info t;
		t.id=-1;
		if(this->head != NULL){
			t = this->head->val;
			this->head = head->next;
		} 
		// else{
		// 	return -1;
		// }
		return t;
	}

};


pthread_t list[1000];
int index=0;
int* ret;
int turn=0;
LinkedList ready;
uthread_policy sch_policy = UTHREAD_DIRECT_PTHREAD;



void* handler(void* arg) {
    while(true) {
        /* Select a user-space thread from the ready queue */
    	struct thread_info t;
        /* Take the thread off the ready queue */
    	t = ready.pop();
        /* Run the task of the user-space thread */
    	t.func(t.arg);
    	// uthread_yield();
    }
    return NULL;
}

void uthread_set_policy(enum uthread_policy policy){
	sch_policy = policy;
	return;
}

void uthread_init(void)
{
	pthread_t t1;
	/* Use pthread_create() to create 4 kernel threads which will schedule any user-space thread to run. */
	if(sch_policy = UTHREAD_PRIORITY){
		for(int i=0;i<4;i++){
				pthread_create(&t1, NULL,(void* (*) (void*)) handler, NULL);
				list[index]=t1;
				index++;
			}
			index=4;
		}
	return;
}

void uthread_create(void (*func) (void*), void* arg)
{	
	pthread_t t1;
	struct thread_info tinfo;
	//void* (*func) (void*) f = &func;
	if(sch_policy == UTHREAD_DIRECT_PTHREAD){
		//cout << "UTHREAD_DIRECT_PTHREAD";
		pthread_create(&t1, NULL,(void* (*) (void*)) func, (void*) arg);
		list[index]=t1;
		index++;
	}
	else if(sch_policy = UTHREAD_PRIORITY){
		//cout << "UTHREAD_PRIORITY";
		// ready.push(??);
			tinfo.id=turn;
			tinfo.priority=0;
			tinfo.func= func;
			tinfo.arg=arg;
			ready.push(tinfo);
	}
	

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
	/* Wait for all 4 pthreads to exit */
	for(int i=0;i<=index;i++)
		{
			pthread_join(list[i],NULL);
		}
}

void uthread_set_param(int param)
{

}
