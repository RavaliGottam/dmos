//Assignment 2 - Joel Mascarenhas (1211194319) & Dhananjayan Santhanakrishnan (1211181423)

#include "threads.h"
int global_count = 0;

void IncrementFunction1()
{
	int local_count = 0;
	while(1)
	{
    global_count++;
		local_count++;
    printf("\n\n Printing from Thread# %d :- Global value = %d and Local value = %d\n",currentThreadId(), global_count, local_count);
		yield();
	}
}

void IncrementFunction2()
{
	int local_count = 0;
	while(1)
	{
    global_count++;
		local_count++;
    printf("\n\n Printing from Thread# %d :- Global value = %d and Local value = %d\n",currentThreadId(), global_count, local_count);
		yield();
	}
}

void IncrementFunction3()
{
	int local_count = 0;
	while(1)
	{
    global_count++;
		local_count++;
    printf("\n\n Printing from Thread# %d :- Global value = %d and Local value = %d\n",currentThreadId(), global_count, local_count);
		yield();
	}
}


int main()
{
  createQueue();
  start_thread(IncrementFunction1,NULL);
  start_thread(IncrementFunction2,NULL);
  start_thread(IncrementFunction3,NULL);
  run();
  return 0;
}
