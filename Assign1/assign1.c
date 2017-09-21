// (c) Partha Dasgupta 2009
// permission to use and distribute granted.

#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

#define N 4
semaphore_t empty, full, mutex;
int buffer[N];
int counter = 0;
int in = 0, out = 0, item_num=0, prod_delay = 1, cons_delay = 1;


void prod (int *arg)
{
    while (1){

        P(&empty);
          P(&mutex);
          printf("Writing... \n");

               buffer[*arg]++;
               counter++;
          V(&mutex);
          if(counter == 3){
            counter = 0;
            V(&full);
          }
	sleep(prod_delay);
    }
}



int main()
{
    int id[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8,};
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;

    init_sem(&full, 1);
    init_sem(&empty,0);
    init_sem(&mutex, 1);

    start_thread(prod, &id[0]);

    start_thread(prod, &id[1]);

    start_thread(prod, &id[2]);


    while(1){

        P(&full);
          P(&mutex);
            printf("Reading...\n");
        	    for(int i=0;i<3;i++){
                printf("%d ",buffer[i]);
                V(&empty);
              }
              printf("\n");
          V(&mutex);

        sleep(cons_delay);
    }
}
