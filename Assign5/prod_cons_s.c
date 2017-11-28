// (c) Partha Dasgupta 2009
// permission to use and distribute granted.

#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

#define N 4
semaphore_t *empty, *full, *mutex;
int buffer[N];
int in = 0, out = 0, item_num=0, prod_delay = 1, cons_delay = 1;


void prod ()
{
    while (1){
	//printf("Producer %d: ready to produce\n", currentThreadId());
        P(empty);
          P(mutex);
	    printf("Producer: inserting item#%d, into slot #%d\n", item_num, in);
            buffer[in] = item_num++; in = (in+1) % N;
          V(mutex);
	V(full);
	//sleep(prod_delay);
    }
}

void cons()
{
    while(1){
	//printf("    Consumer %d: ready to consume\n", currentThreadId());
        P(full);
          P(mutex);
	    printf("   Consumer: deleting item#%d, from slot #%d\n", buffer[out], out);
            out = (out+1) % N;
          V(mutex);
	V(empty);
        //sleep(cons_delay);
    }
}


int main()
{
    int id[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    full = CreateSem(0);
    empty = CreateSem(N);
    mutex = CreateSem(1);
    createQueue();

    start_thread(prod);
    start_thread(cons);
    start_thread(prod);
    start_thread(cons);
    start_thread(prod);
    start_thread(cons);
    start_thread(prod);
    start_thread(cons);
    run();
    while (1) { scanf("%d %d", &prod_delay, &cons_delay);
                printf ("\n\n\t\t\t\tP=%d C=%d\n\n\n", prod_delay, cons_delay);
    };
}
