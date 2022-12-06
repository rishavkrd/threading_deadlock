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
	void* reg_th;
	bool running;
	unsigned long sch_rsp;
};

struct thread_info* temp5;
void* stackg;
void* regg;
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
// int cln = 0;
// int *cleanup = (int*) malloc(sizeof(int*));
bool cleanup = false;


void* handler(void* arg) {
    while(!cleanup || !ready->isEmpty()) {
        /* Select a user-space thread from the ready queue */
        //if(*cleanup==1) return NULL;
        printf("ready queue is : %d\n",ready->isEmpty());

    	while(ready->isEmpty() && !cleanup) ;
        /* Take the thread off the ready queue */
    	next_uthread = ready->pop();
    	//printf("next uthread is : %lu\n", (unsigned long)next_uthread);
        /* Run the task of the user-space thread */
        current_uthread = next_uthread;
        printf("current uthread is : %lu\n", (unsigned long)current_uthread);
        //how to save all the registers of current thread?
        //saving scheduler registers
        if(next_uthread!=NULL){
        	 unsigned long b = (unsigned long)(current_uthread->reg) + (unsigned long) 4096;
        	 //printf("pushing scheduler register to stack at: %lu\n",b);
        __asm__ __volatile__ (
        		// "mov %1, %%rsp\n\t;"
        		
        		
        		
        			// "push %es;"
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
        		// "push %edi;"
        		// "push %esi;"
        		// "push %edx;"
        		// "push %ecx;"
        		// "push %ebx;"
        		// "movl %edx,%ds;"
        		// "movl %edx,%ds;"
        		// "push %es;"
        		// "push %es;"

        		// "mov %0, %%rbp\n\t;"
        		"mov %%rsp, %0"
        		:"=r" ( current_uthread->sch_rsp )
        		:"r" (b)
        		:"%eax"
        		);
        	//printf("pushed scheduler registers, rsp: %lu\n",current_uthread->sch_rsp);
        	
        }
       

        //move_sp(a+4096);
        if(next_uthread!=NULL && !next_uthread->running){
        	current_uthread->running=true;
        	unsigned long a = (unsigned long)(current_uthread->stack) + (unsigned long) 4096;
        	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
        	//printf("moving to thread stack is : %lu\n", a);
        	//void* pass = current_uthread->arg;
        	//printf("\x1B[36m arg: %lu \e[0m\n", (unsigned long) current_uthread->arg);
        	//this is causing segmenatation falut, is there issue with the assembly code or the value of stack pointer?
        	// fixed by changing mov order to %%rsp, %0.
        	__asm__ __volatile__ (
        		"mov %1, %%rsp\n\t;"
        		"mov %1, %%rbp \n\t;"
        		"mov %%rbp, %0"
        		:"=r" ( out )
        		:"r" (a)
        		// :"%eax"
        		);

        	__asm__ __volatile__ (
        		// "push %%rbp\n"
        		"push %2\n"
        		"call *%1\n"
        		//"movq %1, %%rdi"
        		// "mov %0, %1"
        		// "pop %0\n"
        		:"=r" ( out )
        		:"0" ( current_uthread->func ),
        		"D" ((unsigned long) current_uthread->arg)
        		// :"%eax"
        		);
        	//printf("\x1B[36m out arg: %lu \e[0m\n", (unsigned long) out);
        	// __asm__ __volatile__ (
        	// 	"call *%1\n"
        	// 	"add $32, %%rsp\n"
        		
        	// 	:"=r" ( out )
        	// 	:"r" (current_uthread->func)
        	// 	// :"%eax"
        	// 	);

        	//__asm__ ("movl %0, %%rbp\n\t;" : "=r"( a ));
        	//printf("assembly running function in user thread\n");
        	//current_uthread->func(current_uthread->arg);
        } 
        // else if(next_uthread!=NULL){
        // 	unsigned long b = (unsigned long)(current_uthread->reg) + (unsigned long) 4096;		//4096-16 bytes for 16 registers
        // 	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
        // 	printf("restoring regg : %lu\n", b);
        // 	//restoring registers
        // 	__asm__ __volatile__ (
        // 		"mov %1, %%rsp\n\t;"
        // 		"mov %1, %%rbp \n\t;"
        		
        // 		"pop %%r15;"
        // 		// "push %ebp;"
        // 		"pop %%r14;"
        // 		"pop %%r13;"
        // 		"pop %%r12;"
        // 		"pop %%r11;"
        // 		"pop %%r10;"
        // 		"pop %%r9;"
        // 		"pop %%r8;"
        // 		"pop %%rbp;"
        // 		"pop %%rsp;"
        // 		"pop %%rdi;"
        // 		"pop %%rsi;"
        // 		"pop %%rbx;"
        // 		"pop %%rdx;"
        // 		"push %%rcx;"
        // 		"pop %%rax;"
        // 		"mov %%rax, %0"
        // 		:"=r" (out)
        // 		:"r" ( b )
        // 		:"%eax"
        // 		);
        // 	printf("popping in user thread, out: %lu\n",out);
        // }
        if(current_uthread!=NULL){
	        //restoring scheduler registers
	    	unsigned long b = (unsigned long)(current_uthread->reg) + (unsigned long) 4032; //3968;		//4096-64 = 4080 bytes for 16 registers
	    	//size_t a = (size_t)current_uthread->stack + (size_t)4096;
	    	printf("restoring scheduler reg from : %lu\n", current_uthread->sch_rsp);
	    	//restoring registers
	    	__asm__ __volatile__ (
	    		"mov %1, %%rsp\n\t;"
	    		
	    		
	    		"pop %%r15;"
	    		// "push %ebp;"
	    		"pop %%r14;"
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
	    		"push %%rcx;"
	    		"pop %%rax;"
	    		"mov %%rbp, %0"
	    		:"=r" (out)
	    		:"r" ( current_uthread->sch_rsp )
	    		:"%eax"
	    		);
	    	//printf("popping back scheduler registers, rbp: %lu\n",out);
	    	// printf("\x1B[36m arg: %lu \e[0m\n", (unsigned long)current_uthread->arg);
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
		regg = malloc (4096);
		regt = malloc(4096);

		tinfo.reg_th = regt;
		tinfo.reg = regg;
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
	pthread_yield();
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

}
