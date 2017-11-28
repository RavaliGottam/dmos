// Dhananjayan Santhanakrishnan - 1211181423 & Joel Mascarenhas - 1211194319

#include "threads.h"
#define LOCKED 1
#define UNLOCKED 0
typedef struct Semaphore_t{
  int locker;
  TCB_t *semQ;
  int count;
}semaphore_t;

// mallocs a semaphore structure, initializes it to the InitValue and returns
// the pointer to the semaphore.
semaphore_t* CreateSem(int InputValue);
// takes a pointer to a semaphore and performs P, i.e. decrements the semaphore,
// and if the value is less than zero then blocks the thread in the queue
// associated with the semaphore.
void P(semaphore_t *sem);
// increments the semaphore, and if the value is 0 or negative,
// then takes a PCB out of the semaphore queue and puts it into the ReadyQ.
// Note: The V routine also "yields" to the next runnable thread.
void V(semaphore_t *sem);

semaphore_t* CreateSem(int InputValue){
    semaphore_t* tempSem = (semaphore_t*) (malloc(sizeof(semaphore_t)));
    tempSem->semQ = newQueue();
    tempSem->count = InputValue;
    tempSem->locker = 1;
    return tempSem;
}

void P(semaphore_t* sem){
  sem->count--;
  TCB_t* Prev_Thread;
  if(sem->count<0){
    AddQueue(sem->semQ,Curr_Thread);
    Prev_Thread = Curr_Thread;
    Curr_Thread = DelQueue(ReadyQ);
    swapcontext(&(Prev_Thread->context), &(Curr_Thread->context));
  }
}

void V(semaphore_t* sem){
  sem->count++;
  if(sem->count<=0){
    AddQueue(ReadyQ,DelQueue(sem->semQ));
  }
  yield();
}

void lock(semaphore_t* sem){
  while(sem->locker==LOCKED);
  sem->locker = LOCKED;
}

void unlock(semaphore_t* sem){
  sem->locker = UNLOCKED;
}
