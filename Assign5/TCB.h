// Dhananjayan Santhanakrishnan - 1211181423 & Joel Mascarenhas - 1211194319
#include <ucontext.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct TCB_t{
		int		thread_id;
		struct TCB_t 	*next;
		struct TCB_t 	*prev;
		ucontext_t context;
}TCB_t;

// arguments to init_TCB are
//   1. pointer to the function, to be executed
//   2. pointer to the thread stack
//   3. size of the stack
// Changing for Assignment 5
void init_TCB (TCB_t *tcb, void *function, void *stackP, int stack_size, char *argGive)
{
    memset(tcb, '\0', sizeof(TCB_t));       // wash, rinse
    getcontext(&tcb->context);              // have to get parent context, else snow forms on hell
    tcb->context.uc_stack.ss_sp = stackP;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    makecontext(&tcb->context, function, 1,argGive);// context is now cooked
}
