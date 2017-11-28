// Dhananjayan Santhanakrishnan - 1211181423 & Joel Mascarenhas - 1211194319
#include "q.h"

TCB_t *ReadyQ;
TCB_t *Curr_Thread;
int thread_id_counter = 0;

void start_thread(void *func, char* arg)
{
  //Allocate a stack (via malloc) of a certain size (choose 8192)
	void  *stackP = malloc(8192*2);

  //allocate a TCB (via malloc)
	TCB_t *tcb_object    = (TCB_t *)malloc(sizeof(TCB_t));

  //call init_TCB with appropriate arguments
	init_TCB(tcb_object, func, stackP, 8192*2,arg);

  //Add a thread_id (use a counter)
  thread_id_counter++;
  tcb_object->thread_id = 	thread_id_counter;

  //call addQ to add this TCB into the “ReadyQ” which is a global header pointer
	AddQueue(ReadyQ, tcb_object);
	printf("Thread %d created \n",thread_id_counter);
	printf("Ready Queue Size: %d \n",sizeOfQ(tcb_object));
}

void run()
{
	Curr_Thread = DelQueue(ReadyQ);
	//printf("after");
  ucontext_t parent;     // get a place to store the main context, for faking
	getcontext(&parent);   // magic sauce
	swapcontext(&parent, &(Curr_Thread->context));  // start the first thread
}

void yield() // similar to run
{
	TCB_t *Prev_Thread;
	AddQueue(ReadyQ, Curr_Thread);
	Prev_Thread = Curr_Thread;
  Curr_Thread = DelQueue(ReadyQ);

  //swap the context, from Prev_Thread to the thread pointed to Curr_Thread
	swapcontext(&(Prev_Thread->context), &(Curr_Thread->context));
}

void createQueue()
{
	ReadyQ = newQueue();
}

void printThread_id(TCB_t *tcb_object)
{
  printf("Printing Thread %d\n",tcb_object->thread_id);
}

int currentThreadId(){
  return Curr_Thread->thread_id;
}
