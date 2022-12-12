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
	void (*ret_func) (void*);
	void* arg;
	int id;
	int priority;
	void* stack;
	//void* reg;
	void* reg_th;
	void* kernel_stack;
	bool running;
	bool yield;
	unsigned long sch_rsp;
	unsigned long sch_rbp;
	unsigned long rsp;
	unsigned long kernel_bottom;
	unsigned long rip, rip_sch;

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
		temp5 = (struct thread_info *) malloc(sizeof(thread_info));
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

unsigned long save_regs(unsigned long stack){
	unsigned long ret;
	__asm__ __volatile__ (
		"mov %%rsp, %0;"
		"mov %1, %%rsp;"

		// "push %0;"
		"push %%rax;"
		"push %%rcx;"
		"push %%rdx;" 
		"push %%rbx;"
		"push %%rsi;"
		"push %%rdi;"
		"push %%rbp;"
		"push %%r8;"
		"push %%r9;"
		"push %%r10;"
		"push %%r11;"
		"push %%r12;"
		"push %%r13;"
		"push %%r14;"
		"push %%r15;"

		// "mov %0, %%rsp;"

		:"=r" ( ret )
		:"r" (stack)
		);
	// printf("saved from rsp: %lu\n",ret);
	unsigned long bottom;
	__asm__ __volatile__ (
		"mov %%rsp, %0;"
		:"=r" ( bottom )
		// :"r" (ret)
		);
	// printf("bottom: %lu\n",bottom);
	// __asm__ __volatile__ (
	// 	"mov %1, %%rsp;"
	// 	:"=r" ( stack )
	// 	:"r" (ret)
	// 	);

	printf("saved at: %lu\n",stack);
	// printf("2**saved from rsp: %lu\n",ret);
	return bottom;
}
void restore_regs(unsigned long stack){
	unsigned long ret;
	printf("start restore from: %lu\n",stack);
	__asm__ __volatile__ (
		
		"mov %1, %%rsp;"
		
		"pop %%r15;"
		"pop %%r14;"
		"pop %%r13;"
		"pop %%r12;"
		"pop %%r11;"
		"pop %%r10;"
		"pop %%r9;"
		"pop %%r8;"
		"pop %%rbp;"
		"pop %%rdi;"
		"pop %%rsi;"
		"pop %%rbx;"
		"pop %%rdx;" 
		"pop %%rcx;"
		"pop %%rax;"
		"pop %%rsp;"

		// "mov %%rax, %%rsp;"
		"mov %%rsp, %0;"

		:"=r" ( ret )
		:"r" (stack)
		);
	printf("restored to: %lu\n",ret);
	return;
}

pthread_t list[1000];
int index=0;
int* ret;
int turn=0;
LinkedList *ready = (LinkedList*) malloc(sizeof(LinkedList));
uthread_policy sch_policy = UTHREAD_DIRECT_PTHREAD;
__thread thread_info* current_uthread;
__thread thread_info* next_uthread;

unsigned long out=0, stack_size=80;
// int cln = 0;
// int *cleanup = (int*) malloc(sizeof(int*));
bool cleanup = false;




void* handler(void* arg) {
	void* kernel_stack = malloc (4096);
	unsigned long sch_stack = (unsigned long)kernel_stack+(unsigned long)4096;
	// unsigned long kernel_bottom = save_regs(sch_stack);
	unsigned long kernel_bottom = (unsigned long)sch_stack - stack_size;

	unsigned long ret;
	unsigned long rip_sch = (unsigned long)  &&sch_ret;
	__asm__ __volatile__ (
		"mov %%rsp, %%r13;"
		"mov %1, %%rsp;"

		"push %%r14;"
		"push %%rax;"
		"push %%rcx;"
		"push %%rdx;" 
		"push %%rbx;"
		"push %%rsi;"
		"push %%rdi;"
		"push %%rbp;"
		"push %%rsi;"
		"push %%rdi;"
		// "push %%r8;"
		// "push %%r9;"
		// "push %%r10;"
		// "push %%r11;"
		// "push %%r12;"
		// "push %%r13;"
		// "push %%r14;"
		// "push %%r15;"
		"mov %%rsp, %0;"
		"mov %%r13, %%rsp;"
		// "jmp *%2;"
		:"=r" ( ret )
		:"r" (sch_stack),
		"S" (rip_sch)
		);
	// __asm__ __volatile__ (
		
	// 			"mov %1, %%rsp;"
				
	// 			"pop %%r15;"
	// 			"pop %%r14;"
	// 			"pop %%r13;"
	// 			"pop %%r12;"
	// 			"pop %%r11;"
	// 			"pop %%r10;"
	// 			"pop %%r9;"
	// 			"pop %%r8;"
	// 			"pop %%rbp;"
	// 			"pop %%rdi;"
	// 			"pop %%rsi;"
	// 			"pop %%rbx;"
	// 			"pop %%rdx;" 
	// 			"pop %%rcx;"
	// 			"pop %%rax;"
	// 			"pop %%rsp;"

	// 			// "mov %%rax, %%rsp;"
	// 			"mov %%rsp, %0;"

	// 			:"=r" ( out )
	// 			:"r" (kernel_bottom)
	// 			);
sch_ret:
    while(!cleanup || !ready->isEmpty()) {
    	
	// printf("sch_bottom: %lu kernel_bottom: %lu, kernel_stack: %lu\n", ret, kernel_bottom, (unsigned long) kernel_stack);
        /* Select a user-space thread from the ready queue */
    	// ready->print_list();
    	while(ready->isEmpty() && !cleanup) ;
        /* Take the thread off the ready queue */
    	next_uthread = ready->pop();
    	// printf("next uthread is : %lu\n", (unsigned long)next_uthread->arg);
        /* Run the task of the user-space thread */
        current_uthread = next_uthread;

        if(current_uthread!=NULL && !current_uthread->running){
        	
			// unsigned long kernel_bottom = sch_stack - (unsigned long) 128;

        	current_uthread->running = true;
        	current_uthread->kernel_bottom = kernel_bottom;
        	current_uthread->rip_sch = rip_sch;
        	// __asm__ __volatile__("lea 0(%%rip), %0" : "=r"(current_uthread->rip_sch));
        	unsigned long a = (unsigned long)(current_uthread->stack) + (unsigned long) 4096;

        	printf("\x1B[36m @First switch to THREAD # %lu, from : %lu \e[0m\n", (unsigned long)current_uthread->arg,a);
        	// printf("kernel_bottom: %lu\n", current_uthread->kernel_bottom);
        	// restore_regs(current_uthread->kernel_bottom);
        	// current_uthread->kernel_bottom = save_regs(sch_stack);

        	__asm__ __volatile__ (
        		"mov %%rsp, %%r15;"
        		"mov %%rbp, %%r14;"
        		
        		"mov %3, %%rsp\n\t;"
        		"mov %3, %%rbp \n\t;"
        		"call *%1\n"

        		"mov %%r15, %%rsp;"
        		"mov %%r14, %%rbp;"

        		:"=r" ( out )		//0
        		:"r" ( current_uthread->func ),	//1
        		"D" ((unsigned long) current_uthread->arg),	//2
        		"S" (a)	//3
        		);
        	
        	printf("\x1B[36m **1** First time call complete! \e[0m\n");


        } 
        else if(current_uthread!=NULL && current_uthread->running ){
        	//restoring thread's stack for previously yielded thread.
        	// unsigned long addr = (unsigned long) current_uthread->reg_th + (unsigned long) (4096-120);
        	current_uthread->yield = true;
        	unsigned long thread_stack_bot = (unsigned long)(current_uthread->reg_th) + (unsigned long) (4096 - stack_size);
        	printf("\x1B[36m @@@@Running switch to THREAD # %lu, from : %lu \e[0m\n", (unsigned long)current_uthread->arg,thread_stack_bot);
        	// restore_regs(current_uthread->rsp);
        	__asm__ __volatile__ (
		
				"mov %1, %%rsp;"
				
				// "pop %%r15;"
				// "pop %%r14;"
				// "pop %%r13;"
				// "pop %%r12;"
				// "pop %%r11;"
				// "pop %%r10;"
				// "pop %%r9;"
				// "pop %%r8;"
				"pop %%rdi;"
				"pop %%rsi;"
				"pop %%rbp;"
				"pop %%rdi;"
				"pop %%rsi;"
				"pop %%rbx;"
				"pop %%rdx;" 
				"pop %%rcx;"
				"pop %%rax;"
				
				"pop %%rax;"
				"mov %%rax, %%rsp;"

				"mov %%rsp, %0;"
				"jmp *%2;"
				// "jmp *%%rax;"
				:"=r" ( out )
				:"r" (thread_stack_bot),
				"S" (current_uthread->rip)
				);
        	// current_uthread->yield = true;
			// current_uthread->func(current_uthread->arg);
        	printf("\x1B[32m restored previously yielded thread's stack succesfully \e[0m\n");
        	
        	// printf("\x1B[32m Called yielded thread \e[0m\n");
        }
        
    }
    // printf("\x1B[32m Exiting Core threads \e[0m\n");
    return NULL;
}

void uthread_yield(void)
{
	return;


	if(sch_policy == UTHREAD_DIRECT_PTHREAD){
		printf("yielded the thread, UTHREAD_DIRECT_PTHREAD\n");
		pthread_yield();

	}
	else if (sch_policy == UTHREAD_PRIORITY){
		th_ret:
		if(current_uthread->yield){
			current_uthread->yield = false;
			printf("--------@yield return\n");
			return;
		}
		else{
			// current_uthread->yield = false;
			unsigned long thread_stack_top = (unsigned long)(current_uthread->reg_th) + (unsigned long) (4096);
			// printf("\x1B[36m @@yield saving regs  \e[0m\n");//current_uthread->sch_rsp);
			// current_uthread->rsp = save_regs(thread_stack_top);
			//yielded = true;
			unsigned long ret;
			current_uthread->rip = (unsigned long) &&th_ret;
			// unsigned long jmp_ptr = (unsigned long ) &&lable;
			// __asm__ __volatile__("lea 0(%%rip), %0" : "=r"(current_uthread->rip));
			__asm__ __volatile__ (
			"mov %%rsp, %%r11;"
			"mov %1, %%rsp;"

			"push %%r11;"
			"push %%rax;"
			"push %%rcx;"
			"push %%rdx;" 
			"push %%rbx;"
			"push %%rsi;"
			"push %%rdi;"
			"push %%rbp;"
			"push %%rsi;"
			"push %%rdi;"
			// "push %%r8;"
			// "push %%r9;"
			// "push %%r10;"
			// "push %%r11;"
			// "push %%r12;"
			// "push %%r13;"
			// "push %%r14;"
			// "push %%r15;"
			// "mov %%rsp, %0;"
			"mov %%r11, %%rsp;"
			// "jmp *%2;"
			:"=r" ( ret )
			:"r" (thread_stack_top),
			"S" ( current_uthread->rip)
			);
// lable:
			// printf("yielded the user thread # %lu, pushed at: current_uthread->thread_rsp: %lu\n",(unsigned long) current_uthread->arg,current_uthread->thread_rsp);
		
			// if(!current_uthread->yield){
			

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

				 // next_uthread = ready->pop();
				 printf("\x1B[36m @@yield switch to SCHEDULER, from: %lu  \e[0m\n", current_uthread->kernel_bottom);//current_uthread->sch_rsp);
				 // restore_regs(current_uthread->kernel_bottom);
				 __asm__ __volatile__ (
			
					
					"mov %1, %%rsp;"
					
					// "pop %%r15;"
					// "pop %%r14;"
					// "pop %%r13;"
					// "pop %%r12;"
					// "pop %%r11;"
					// "pop %%r10;"
					// "pop %%r9;"
					// "pop %%r8;"
					"pop %%rdi;"
					"pop %%rsi;"
					"pop %%rbp;"
					"pop %%rdi;"
					"pop %%rsi;"
					"pop %%rbx;"
					"pop %%rdx;" 
					"pop %%rcx;"
					"pop %%rax;"

					
					"pop %%rax;"
					"mov %%rax, %%rsp;"
					
					"mov %%rsp, %0;"
					
					"call *%2;"
					// "ret;"

					:"=r" ( out )
					:"r" (current_uthread->kernel_bottom),
					"S" (current_uthread->rip_sch)
					);
				}
		
			 // handler(NULL);
			 printf("yield complete---------jump back to thread\n");
	}
	
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

		tinfo.reg_th = malloc(4096);
		//tinfo.reg = regg;
		tinfo.stack = malloc(4096);
		tinfo.kernel_stack = malloc(4096);

		tinfo.id=turn;
		tinfo.priority=0;
		tinfo.func= func;

		tinfo.arg= (void*) arg;
		tinfo.running=false;
		tinfo.yield=false;
		ready->push(tinfo);
		turn++;
		//printf("\x1B[36m arg: %lu \e[0m\n", (unsigned long)tinfo.arg);
	}

}

void uthread_exit(void)
{
	// handler(NULL);
}

void uthread_cleanup(void)
{
	/* Wait for all 4 pthreads to exit */
	// cln=1;
	// *cleanup = 1;
	printf("Cleanup start \n");
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
	if(sch_policy == UTHREAD_PRIORITY) current_uthread->priority=param;
	// printf("@Set param: current_uthread->priority : %d \n",current_uthread->priority);
}

        		// "push %%r8;"
        		// "push %%r9;"
        		// "push %%r10;"
        		// "push %%r11;"
        		// "push %%r12;"
        		// "push %%r13;"
        		// "push %%r14;"
        		// "push %%r15;"
// current_uthread=next_uthread;
     //    		printf("\x1B[36m returned from thread \e[0m\n");
     //    		ready->print_list();
     //    		if(ready->isEmpty()) {
					// printf("breaking away \n");
     //    			break;
     //    		}
        		// a = (unsigned long)(current_uthread->stack) + (unsigned long) 4096;
        		

        	//restoring registers
		    	// __asm__ __volatile__ (
		    	// 	"mov %1, %%rsp\n\t;"
		    	// 	"mov %3, %%rsp\n\t;"
		    		
		    	// 	"mov %%rsp, %0;"
		    	// 	// "mov %%r14, %%rbp;"
		    	// 	// "mov %%r15, %%rsp;"
		    	// 	// "call *%2;"
		    	// 	:"=r" (current_uthread->ret)
		    	// 	:"r" ( current_uthread->sch_rsp ),
		    	// 	"D" (handler),
		    	// 	"S" (current_uthread->sch_rsp)
		    	// 	:"%eax"
		    	// 	);

        	// printf("\x1B[36m return value of thread is: %lu \e[0m\n", (unsigned long) out);
        	//printf("assembly running function in user thread\n");
	//how to pass the value for return withing the thread execution, since the stack pointers are changed, 
	//is this step really necessary? program works without this.
	//One issue can be that if a thread needs to be stopped, we cant do it. The thread will complete only then it can be stopped.
	// void* retval=malloc(sizeof(void*));
	// printf("exiting\n");
	// pthread_exit(retval);


//restore running thread
        	// __asm__ __volatile__ (
        	// 	"mov %%rsp, %%r15;"

        	// 	// "mov %%rbp, %%r14;"
        	// 	"mov %3, %%rsp\n\t"
        	// 	//"mov %1, %%rbp \n\t;"
        	// // "1:"

        	// 	"pop %%rbp;"
        	// 	// "pop %%rsp;"
        	// 	"pop %%rdi;"
        	// 	"pop %%rsi;"
        	// 	"pop %%rbx;"
        	// 	"pop %%rdx;"
        	// 	"pop %%rcx;"
        	// 	"pop %%rax;"

        	// 	"mov %%rsp, %%r10;"
        	// 	"mov %%rbp, %%r11;"
        	// 	"mov %1, %%rax;"
        	// 	// "pop %%r9;"
        		
        	// 	// "pop %%rdi;"
        	// 	// "ret;"
        	// 	// "call *%1;"
        	// 	// "pop %%r13;"

        		
        	// 	// "mov %%r15, %%rsp;"
        	// 	// "mov %%r14, %%rbp;"
        	// 	:"=a" (out)
        	// 	:"0" ( current_uthread->func ),//1
        	// 	"D" ( (unsigned long) current_uthread->arg ),	//2
        	// 	"S" (current_uthread->thread_rsp),	//3
        	// 	"r" (thread_stack_top)
        	// 	// :"%eax"
        	// 	);
//thread store registers
		// __asm__ __volatile__ (
					
		// 			// "push %%r9;"
		// 			// "ret;"
		// 	// "mov %%rsp, 4096(%2);"
		// 			// "mov %%rip, %1;"
		// 			"push %%rax;"
	 //        		"push %%rcx;"
	 //        		"push %%rdx;" // Dirty
	 //        		"push %%rbx;"
	 //        		"push %%rsi;"
	 //        		"push %%rdi;"
	 //        		"push %%rbp;"

	 //        		"mov %%rsp, %0;"

	 //        // "mov %1, %%rsp;"		//restoring handler registers
	 //        // "mov %3, %%rbp;"

	 //        		// "mov %%r11, %%rsp\n\t;"
	 //        		:"=r" ( current_uthread->thread_rsp )
	 //        		:"r" ( current_uthread->sch_rsp ),
	 //        		"D" (thread_stack_top),
		//     		"S" (current_uthread->sch_rbp)
	 //        		);

//restoring scheduler registers
			    	// __asm__ __volatile__ (
			    	// 	// "mov %1, %%rsp\n\t;"
			    	// 	// "mov %3, %%rbp\n\t;"
			    		
			    	// 	// "mov %%rsp, %0;"
			    	// 	// "mov %%r14, %%rbp;"
			    	// 	// "mov %%r15, %%rsp;"
			    	// 	"call *%2;"

			    	// 	:"=r" (out)
			    	// 	:"r" ( current_uthread->sch_rsp ),
			    	// 	"D" (handler),
			    	// 	"S" (current_uthread->sch_rbp)
			    	// 	:"%eax"
			    	// 	);

			    	
			    	// return;
		// }
	 //    else{
	 //    	current_uthread->yield=false;
	 //    	// printf("@yield returning from yield\n");
	 //    	return;
	 //    }
		    	
		// return;
		//current_uthread = ready->pop();
		//pthread_yield();

			
			//ready->push(*current_uthread);		//For FCFS scheduling

			// next_uthread = ready->pop();
			
			        	// restore_regs(current_uthread->kernel_bottom);
    //     	__asm__ __volatile__ (
		
				// "mov %1, %%rsp;"
				
				// // "pop %%r15;"
				// // "pop %%r14;"
				// // "pop %%r13;"
				// // "pop %%r12;"
				// // "pop %%r11;"
				// // "pop %%r10;"
				// // "pop %%r9;"
				// // "pop %%r8;"
				// "pop %%rbp;"
				// "pop %%rdi;"
				// "pop %%rsi;"
				// "pop %%rbx;"
				// "pop %%rdx;" 
				// "pop %%rcx;"
				// "pop %%rax;"
				
				// "pop %%rax;"
				// "mov %%rax, %%rsp;"

				// "mov %%rsp, %0;"
				// "jmp *%2;"

				// :"=r" ( out )
				// :"r" (current_uthread->kernel_bottom),
				// "S" (current_uthread->rip_sch)
				// );
        	// printf("\x1B[36m **1** SCHEDULER switch complete! \e[0m\n");