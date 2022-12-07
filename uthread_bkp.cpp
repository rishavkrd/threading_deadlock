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
	//void* reg;
	void* reg_th;
	bool running;
	unsigned long sch_rsp;
	unsigned long sch_rbp;
	unsigned long thread_rsp;
};

struct thread_info* temp5;
void* stackg;
void* regt;

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
	Node* peek(){
		return this->head;
	}
	void push(struct thread_info t){
		Node *temp = (Node*) malloc(sizeof(Node));
		temp->val = t;
		temp->next = NULL;
		// temp->next = head;
		// this->head = temp;
		// printf("Thread %lu pushed to ready queue\n",(unsigned long)t.arg);
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
		//temp5->id=-1;
		if(this->head != NULL){
			temp5 = &this->head->val;
			this->head = head->next;
			return temp5;
		} 
		
		return NULL;
	}
	void insert(Node* pos, struct thread_info t){
		Node *temp = (Node*) malloc(sizeof(Node));
		temp->val = t;
		// printf("Thread %lu inserted to ready queue\n",(unsigned long)t.arg);
		// printf("__insert: current_uthread->priority : %d \n",t.priority);
		if(pos==NULL){
			temp->next = head;
			this->head = temp;
		} else{
			
			temp->next = pos->next;
			pos->next = temp;
		}
	}
	void print_list(){
		Node* pos = this->head;
		int count = 0;
		printf("\n");
		while(pos!=NULL){
			printf("thread #%lu P:%d ",(unsigned long) pos->val.arg, pos->val.priority);

			count++;
			pos = pos->next;
		}
		printf("\n");
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
// int cln = 0;
// int *cleanup = (int*) malloc(sizeof(int*));
bool cleanup = false;
unsigned long rsp1, rbp1;


void* handler(void* arg) {
	bool yielded = false;
    while(!cleanup || !ready->isEmpty()) {
    	__asm__ __volatile__ (
        		// "push %%rbp\n"
        		// "push %2\n"
        		//	//save sch rsp, rbp
        		"mov %%rsp, %%r15\n\t;"
        		"mov %%rbp, %%r14 \n\t;"

        		:"=a" ( out )		//0
        		:"0" ( out )	//1

        		// :"%eax"
        		);

        /* Select a user-space thread from the ready queue */

        //if(*cleanup==1) return NULL;
        // printf("ready queue is : %d\n",ready->isEmpty());

    	// ready->print_list();
    	while(ready->isEmpty() && !cleanup) ;
    	
        /* Take the thread off the ready queue */
    	next_uthread = ready->pop();
    	//printf("next uthread is : %lu\n", (unsigned long)next_uthread);
        /* Run the task of the user-space thread */
        current_uthread = next_uthread;
        //printf("current uthread is : %lu\n", (unsigned long)current_uthread);
        //how to save all the registers of current thread?
        
        if(current_uthread!=NULL){
        	//saving scheduler registers
        	 //printf("pushing scheduler register to stack at: %lu\n",b);
        // 	__asm__ __volatile__ (

        // 		//save sch rsp, rbp
        // 		"mov %%rsp, %2\n\t;"
        // 		"mov %%rbp, %3 \n\t;"
        // 		//restore sch rsp, rbp
        // 		// "mov %%r15, %%rsp\n\t;"
        // 		// "mov %%r14, %%rbp \n\t;"
        // 		//
        // 		//"mov %%rax, %0"
        // 		:"=a" ( out )		//0
        // 		:"0" ( current_uthread->id ),	//1
        // 		"D" ((unsigned long) current_uthread->sch_rsp),	//2
        // 		"S"((unsigned long) current_uthread->sch_rbp)	//3
        // 		// :"%eax"
        // 		);
        // 	//printf("pushed scheduler registers, rsp: %lu\n",current_uthread->sch_rsp);
        // }
        	
    		__asm__ __volatile__ (		//store rsp

        		// "mov %0, %%rbp\n\t;"
        		"mov %%r15, %0;"
        		// "mov %%r14, %1;"
        		:"=r" ( current_uthread->sch_rsp )
        		:"r" (current_uthread->sch_rbp)
        		:"%eax"
        		);
    		__asm__ __volatile__ (

    			// "push %%rsp;"
       //  		"push %%rbp;"
        		// "push %%rax;"        		
        		// // // "push %ebp;"
        		// "push %%rcx;"
        		// "push %%rdx;"
        		// "push %%rbx;"
        		// "push %%rsi;"
        		// "push %%rdi;"
        		
        		// "push %%r8;"
        		// "push %%r9;"
        		// "push %%r10;"
        		// "push %%r11;"
        		// "push %%r12;"
        		// "push %%r13;"
        		// "push %%r14;"
        		// "push %%r15;"

        		// "mov %0, %%rbp\n\t;"
        		"mov %%r14, %0;"
        		// "mov %%r14, %1;"
        		:"=r" ( current_uthread->sch_rbp )
        		:"r" (current_uthread->sch_rbp)
        		:"%eax"
        		);
    		// current_uthread = next_uthread;
    		// printf("scheduling next thread\n");
    	}

        //move_sp(a+4096);
        if(current_uthread!=NULL && !current_uthread->running){
        	current_uthread->running=true;
        	unsigned long a = (unsigned long)(current_uthread->stack) + (unsigned long) 4096;
        	//printf("moving to thread stack : %lu\n", a);
        	//printf("\x1B[36m arg: %lu \e[0m\n", (unsigned long) current_uthread->arg);


        	__asm__ __volatile__ (
        		// "push %%rbp\n"
        		// "push %2\n"
        		//save sch rsp, rbp
        		// "mov %%rsp, %%r15\n\t;"
        		// "mov %%rbp, %%r14 \n\t;"
        		//

        		"mov %3, %%rsp\n\t;"
        		"mov %3, %%rbp \n\t;"
        		"call *%1\n"
        		//restore sch rsp, rbp
        		"mov %%r15, %%rsp\n\t;"
        		"mov %%r14, %%rbp \n\t;"
        		//
        		//"mov %%rax, %0"
        		:"=a" ( out )		//0
        		:"0" ( current_uthread->func ),	//1
        		"D" ((unsigned long) current_uthread->arg),	//2
        		"S"(a)
        		// :"%eax"
        		);
        	printf("\x1B[36m returned from thread \e[0m\n");

        	// __asm__ __volatile__ (		//restore sch registers

        	// 	// //save sch rsp, rbp
        	// 	// "mov %%rsp, %2\n\t;"
        	// 	// "mov %%rbp, %3 \n\t;"
        	// 	// restore sch rsp, rbp
        	// 	"mov %2, %%rsp\n\t;"
        	// 	"mov %3, %%rbp \n\t;"
        		
        	// 	//"mov %%rax, %0"
        	// 	:"=a" ( out )		//0
        	// 	:"0" ( current_uthread->id ),	//1
        	// 	"D" ((unsigned long) current_uthread->sch_rsp),	//2
        	// 	"S"((unsigned long) current_uthread->sch_rbp)	//3
        	// 	// :"%eax"
        	// 	);
        	// printf("\x1B[36m return value of thread is: %lu \e[0m\n", (unsigned long) out);
        	//printf("assembly running function in user thread\n");

        } 
        else if(current_uthread!=NULL && current_uthread->running ){
        	//restoring thread's stack for previously yielded thread.
        	yielded = true;
        	//unsigned long thread_stack_bottom = (unsigned long)(current_uthread->stack) + (unsigned long) (4096-128+48);		//4096-128 bytes for 16 registers
        	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
        	//printf("restoring previously yielded thread's stack : %lu\n", current_uthread->thread_rsp);
        	//restoring thread registers
        	__asm__ __volatile__ (
        		"mov %%rsp, %%r15;"
        		"mov %%rbp, %%r14;"
        		"mov %1, %%rsp\n\t"
        		//"mov %1, %%rbp \n\t;"
        		
        		"pop %%r13;"
        		// "push %ebp;"
        		"pop %%r13;"
        		"pop %%r13;"
        		"pop %%r12;"
        		"pop %%r11;"
        		"pop %%r10;"
        		"pop %%r9;"
        		"pop %%r8;"
        		"pop %%rbp;"
        		"pop %%rsp;"
        		"pop %%rdi;"
        		"pop %%rsi;"
        		"pop %%rbx;"
        		"pop %%rdx;"
        		"pop %%rcx;"
        		"pop %%rax;"
        		// "mov %%rsp, %0;"
        		"mov %%r15, %%rsp;"
        		"mov %%r14, %%rbp;"
        		:"=r" (out)
        		:"r" ( current_uthread->thread_rsp )
        		:"%eax"
        		);
        	// printf("\x1B[32m restored previously yielded thread's stack succesfully, calling function \e[0m\n");
        	// __asm__ __volatile__ (
        	// 	// "push %%rbp\n"
        	// 	// "push %2\n"
        	// 	"call *%1\n"
        	// 	//"mov %%rax, %0"
        	// 	:"=a" ( out )		//0
        	// 	:"0" ( current_uthread->func )//,	//1
        	// 	// "D" ((unsigned long) current_uthread->arg)	//2
        	// 	// :"%eax"
        	// 	);
        	

        	// printf("\x1B[32m Done succesfully continued the thread and completed context switch  \e[0m\n");
        	

        	//printf("popping thread's registers rsp is at: %lu\n",out);
        }

        

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
			//index=4;
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
		//printf("Added to ready queue \n");

		stackg = malloc(4096);
		//regg = malloc (4096);
		regt = malloc(4096);

		tinfo.reg_th = regt;
		//tinfo.reg = regg;
		tinfo.stack = stackg;

		tinfo.id=turn;
		tinfo.priority=0;
		tinfo.func= func;
		tinfo.arg= (void*) arg;
		tinfo.running=false;
		ready->push(tinfo);
		turn++;
		//printf("\x1B[36m arg: %lu \e[0m\n", (unsigned long)tinfo.arg);
	}
	

	// pthread_create(&thread1, NULL, &access_memory, (void*) 1);
}

void uthread_exit(void)
{
	//how to pass the value for return withing the thread execution, since the stack pointers are changed, 
	//is this step really necessary? program works without this.
	//One issue can be that if a thread needs to be stopped, we cant do it. The thread will complete only then it can be stopped.

	//pthread_exit(&ret);
}

void uthread_yield(void)
{
	if(sch_policy == UTHREAD_DIRECT_PTHREAD){
		printf("yielded the thread, UTHREAD_DIRECT_PTHREAD\n");
		pthread_yield();

	}
	else if (sch_policy == UTHREAD_PRIORITY){
		unsigned long thread_stack_top = (unsigned long)(current_uthread->stack) + (unsigned long) (4096);
		__asm__ __volatile__ (
					// "mov %%rsp, %%r11\n"
	        		// "mov %1, %%rsp\n\t;"
	        		"push %%rax;"
	        		// // "push %ebp;"
	        		"push %%rcx;"
	        		"push %%rdx;"
	        		"push %%rbx;"
	        		"push %%rsi;"
	        		"push %%rdi;"
	        		"push %%rsp;"
	        		"push %%rbp;"
	        		"push %%r8;"
	        		"push %%r9;"
	        		"push %%r10;"
	        		"push %%r11;"
	        		"push %%r12;"
	        		"push %%r13;"
	        		"push %%r14;"
	        		"push %%r15;"
	        		"mov %%rsp, %0;"
	        		// "mov %%r11, %%rsp\n\t;"
	        		:"=r" ( current_uthread->thread_rsp )
	        		:"r" ( thread_stack_top )
	        		);
		
		//yielded = true;
		
		//printf("yielded the user thread, pushed thread's register and rsp at: current_uthread->thread_rsp: %lu\n",current_uthread->thread_rsp);

		
		struct Node* pos = ready->peek();
		struct Node* prev = NULL;
		bool skip=false;
		//For priority based scheduling
		// printf("@yield: current_uthread->priority : %d \n",current_uthread->priority);
		while(pos!=NULL && pos->val.priority <= current_uthread->priority){
			if(pos->val.id == current_uthread->id){
				skip=true;
				break;
			} 
			// printf("@itr:  : %d ",pos->val.priority);
			prev = pos;
			pos=pos->next;
		}
		// printf("\n");
		if(!skip) ready->insert(prev, *current_uthread);
		
		//ready->push(*current_uthread);		//For FCFS scheduling

		// next_uthread = ready->pop();
		
		

		// printf("yield function is restoring scheduler reg from : %lu\n", current_uthread->sch_rsp);
		    	//restoring registers
		    	__asm__ __volatile__ (
		    		"mov %1, %%rsp\n\t;"
		    		"mov %2, %%rbp\n\t;"
		    		
		    		"call *%3"
		    		// "pop %%r15;"
		    		// // "push %ebp;"
		    		// "pop %%r14;"
		    		// "pop %%r13;"
		    		// "pop %%r12;"
		    		// "pop %%r11;"
		    		// "pop %%r10;"
		    		// "pop %%r9;"
		    		// "pop %%r8;"
		    		
		    		// "pop %%rdi;"
		    		// "pop %%rsi;"
		    		// "pop %%rbx;"
		    		// "pop %%rdx;"
		    		// "pop %%rcx;"
		    		// "pop %%rax;"

		    		// "pop %%rbp;"
		    		// "pop %%rsp;"
		    		// "mov %%rbp, %0"
		    		:"=r" (out)
		    		:"r" ( current_uthread->sch_rsp ),
		    		"D" (current_uthread->sch_rbp),
		    		"S" (handler)
		    		:"%eax"
		    		);
		// return;
		//current_uthread = ready->pop();
		//pthread_yield();
	}
	
}

void uthread_cleanup(void)
{
	/* Wait for all 4 pthreads to exit */
	// cln=1;
	// *cleanup = 1;
	cleanup=true;
	for(int i=0;i<index;i++)
		{
			printf("Cleanup in progress, i: %d \n",i);
			pthread_join(list[i],NULL);
		}
}

void uthread_set_param(int param)
{
	// printf("set priority to : %d \n",param);
	current_uthread->priority=param;
	// printf("@Set param: current_uthread->priority : %d \n",current_uthread->priority);
}
