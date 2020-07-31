#include <stdlib.h>

#include "fp.h"
#include "lwp.h"

static tid_t next_tid = 0; /* Thread ID value to set new thread to */

tid_t lwp_create(lwpfun fun, void* args, size_t ssize) {
    /* Takes a function pointer (fun), a list of arguments (args), and
     * a stack size (ssize) and creates a new lightweight process (LWP).
     * Returns the thread id of the new LWP
     */

    unsigned long sp; /* Base stack pointer */
    int count;

    /* Allocate memory for new LWP */
    thread newthread = malloc(sizeof(context));
    unsigned long* stack = malloc(ssize * sizeof(unsigned long));

    /* Push data to the stack */
    GetSP(sp);
    push(sp, lwp_exit());
    push(sp, 0xdeadbeef);

    /* Set Thread ID */
    sync_and_fetch(&next_tid, 1);
    newthread->tid = next_tid;

    /* Set Stack Pointer and Stack Size */
    newthread->stack = stack;
    newthread->stacksize = ssize;

    /* Set Stack Pointer State */
    newthread->state.rsp = sp;
    newthread->state.rbp = sp;

    /* Store Arguments into Registers */
    count = 1;
    while (args) {
	switch (count) {
	    case 1:
        	newthread->state.rdi = (unsigned long) args;
                break;
	    case 2:
		newthread->state.rsi = (unsigned long) args;
		break;
	    case 3:
		newthread->state.rdx = (unsigned long) args;
		break;
	    case 4:
		newthread->state.rcx = (unsigned long) args;
		break;
	    case 5:
		newthread->state.r8 = (unsigned long) args;
		break;
	    case 6:
		newthread->state.r9 = (unsigned long) args;
		break;
	    default:
		break;
	}

        args++;
    }

    newthread->state.fxsave = FPU_INIT; /* Store intital FPU state */

    /* Admit the new LWP into the scheduler */
    sched = lwp_get_scheduler();
    sched->admit(newthread);

    return newthread->tid;
}

void lwp_exit() {
    /* Terminates the LWP that called this function */
}

tid_t lwp_gettid() {
    /* Returns the thread ID of the LWP that called this function */
}

void lwp_yield() {
    /* Saves the context of the LWP that called this function and stops
     * the LWP from running
     */
}

void lwp_start() {
    /* Starts the LWP that called this function */
}

void lwp_stop() {
    /* Stops the LWP that called this function */
}

void lwp_set_scheduler(scheduler fun) {
    /* Takes a new scheduling function (fun) and sets it as the current
     * scheduler
     */
}

scheduler lwp_get_scheduler() {
    /* Returns the current scheduling function being ran */
}

thread tid2thread(tid_t tid) {
    /* Takes a thread ID (tid) and returns the thread that is mapped to that
     * thread id
     */

    if ((tid < next_tid) && (tid > NO_THREAD)) {
	return NULL; /*FIX THIS TO BE THE CALLING LWP*/
    }
    else { /* Invalid Thread ID */
        return NULL;
    }
}
