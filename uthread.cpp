/* Write your core functionality in this file. */
// #include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "uthread.h"
#include <cstdlib>

// namespace std {}
// using std::cout;
struct thread_info{
	void (*func) (void*);
	void* arg;
	int id;
	int priority;
	void* stack;
	void* reg;
	bool running;
};

struct thread_info* temp5;
void* stackg;
void* regg;

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
	bool isEmpty(){
		return head==NULL;
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
	struct thread_info* pop(){
		temp5 = (struct thread_info *) malloc(sizeof(thread_info *));
		temp5->id=-1;
		if(this->head != NULL){
			temp5 = &this->head->val;
			this->head = head->next;
			return temp5;
		} 
		
		return NULL;
	}

};


pthread_t list[1000];
int index=0;
int* ret;
int turn=0;
LinkedList *ready = (LinkedList*) malloc(sizeof(LinkedList*));
uthread_policy sch_policy = UTHREAD_DIRECT_PTHREAD;
__thread thread_info* current_uthread;
__thread thread_info* next_uthread;
unsigned long out=0;

void* handler(void* arg) {
    while(true) {
        /* Select a user-space thread from the ready queue */
        //printf("ready queue is : %d\n",ready.isEmpty());
    	while(ready->isEmpty()) ;
        /* Take the thread off the ready queue */
    	next_uthread = ready->pop();
    	printf("next uthread is : %lu\n", (unsigned long)next_uthread);
        /* Run the task of the user-space thread */
        current_uthread = next_uthread;
        printf("current uthread is : %lu\n", (unsigned long)current_uthread);
        //how to save all the registers of current thread?
        if(next_uthread!=NULL){
        	 unsigned long b = (unsigned long)(current_uthread->reg) + (unsigned long) 4096;
        	 printf("storing stack reg at: %lu\n",b);
        // __asm__ __volatile__ (
        // 		"mov %%rsp, %0\n\t;"
        // 		"mov %%rbp, %0\n\t;"
        		
        // 		// "push %es;"
        // 		"push %%rax;"
        // 		// "push %ebp;"
        // 		"push %%rcx;"
        // 		"push %%rdx;"
        // 		"push %%rbx;"
        // 		"push %%rsi;"
        // 		"push %%rdi;"
        // 		"push %%rsp;"
        // 		"push %%rbp;"
        // 		"push %%r8;"
        // 		"push %%r9;"
        // 		"push %%r10;"
        // 		"push %%r11;"
        // 		"push %%r12;"
        // 		"push %%r13;"
        // 		"push %%r14;"
        // 		"push %%r15;"
        // 		// "push %edi;"
        // 		// "push %esi;"
        // 		// "push %edx;"
        // 		// "push %ecx;"
        // 		// "push %ebx;"
        // 		// "movl %edx,%ds;"
        // 		// "movl %edx,%ds;"
        // 		// "push %es;"
        // 		// "push %es;"

        // 		// "mov %0, %%rbp\n\t;"
        // 		// : "=r" ( a )
        // 		:"=r" ( b )
        // 		);
        }
       

        //move_sp(a+4096);
        if(next_uthread!=NULL && !next_uthread->running){
        	unsigned long a = (unsigned long)(current_uthread->stack) + (unsigned long) 4096;
        	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
        	printf("correct order stack is : %lu\n", a);
        	
        	//this is causing segmenatation falut, is there issue with the assembly code or the value of stack pointer?
        	// fixed by changing mov order to %%rsp, %0.
        	__asm__ __volatile__ (
        		"mov %1, %%rsp\n\t;"
        		"mov %1, %%rbp \n\t;"
        		"mov %%rsp, %0"
        		:"=r" ( out )
        		:"r" (a)
        		:"%eax"
        		);
        	//__asm__ ("movl %0, %%rbp\n\t;" : "=r"( a ));
        	printf("running function in user thread, out: %d\n",out);
        	next_uthread->running=true;
        	
        	current_uthread->func(current_uthread->arg);
        } else if(next_uthread!=NULL){
        	unsigned long b = (unsigned long)(current_uthread->reg) + (unsigned long) 4096;
        	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
        	printf("restoring regg : %lu\n", b);
        	//restoring registers
        	// __asm__ __volatile__ (
        	// 	"mov %%rsp, %0\n\t;"
        	// 	"mov %%rbp, %0\n\t;"
        		
        	// 	"pop %%r15;"
        	// 	// "push %ebp;"
        	// 	"pop %%r14;"
        	// 	"pop %%r13;"
        	// 	"pop %%r12;"
        	// 	"pop %%r11;"
        	// 	"pop %%r10;"
        	// 	"pop %%r9;"
        	// 	"pop %%r8;"
        	// 	"pop %%rbp;"
        	// 	"pop %%rsp;"
        	// 	"pop %%rdi;"
        	// 	"pop %%rsi;"
        	// 	"pop %%rbx;"
        	// 	"pop %%rdx;"
        	// 	"push %%rcx;"
        	// 	"pop %%rax;"
        	// 	:"=r" (out)
        	// 	// :"r" ( b )
        	// 	// :"%eax"
        	// 	);
        }
        
        // __asm__("addq %1,%2" : "=r" (%rsp) : "r" (a), "0" (4096));
        // __asm__("addq %1,%2" : "=r" (%rbp) : "r" (a), "0" (4096));
        //     	"mov %1, %%rbp\n\t"

    	
    	// uthread_yield();
    }
    return NULL;
}

void uthread_set_policy(enum uthread_policy policy){
	sch_policy = policy;
	printf("Policy is: %d \n",sch_policy);
	return;
}

void uthread_init(void)
{
	pthread_t t1;
	
	/* Use pthread_create() to create 4 kernel threads which will schedule any user-space thread to run. */
	if(sch_policy == UTHREAD_PRIORITY){
		printf("Initializing UTHREAD_PRIORITY \n");
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
		printf("This is UTHREAD_DIRECT_PTHREAD \n");
		pthread_create(&t1, NULL,(void* (*) (void*)) func, (void*) arg);
		list[index]=t1;
		index++;
	}
	else if(sch_policy == UTHREAD_PRIORITY){
		// cout << "UTHREAD_PRIORITY";
		// ready.push(??);
		printf("Added to ready queue \n");

		stackg = malloc(4096);
		regg = malloc (4096);
		tinfo.reg = regg;
		tinfo.stack = stackg;

		tinfo.id=turn;
		tinfo.priority=0;
		tinfo.func= func;
		tinfo.arg=arg;
		tinfo.running=false;
		ready->push(tinfo);
		turn++;
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
	for(int i=0;i<index;i++)
		{
			pthread_join(list[i],NULL);
		}
}

void uthread_set_param(int param)
{

}
