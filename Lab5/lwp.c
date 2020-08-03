#include <stdlib.h>

#include "fp.h"
#include "lwp.h"


#define NUM_THREADS 64


static tid_t next_tid = 0; /* Thread ID value to set new thread to */
static int thread_len = 0; /* Length of threads buffer */
static int cur_max_threads = NUM_THREADS; /* Dynamic max number of threads */
static int running_threads = 0; /* Number of threads that are running */
thread* threads; /* Buffer holding created threads */


tid_t lwp_create(lwpfun fun, void* args, size_t ssize) {
    /* Takes a function pointer (fun), a list of arguments (args), and
     * a stack size (ssize) and creates a new lightweight process (LWP).
     * Returns the thread id of the new LWP
     */

    #ifndef INIT_ARRAY /* Malloc the array of threads if not already done */
	#define INIT_ARRAY
	threads = malloc(NUM_THREADS);
    #endif

    unsigned long sp; /* Base stack pointer */
    int count;

    /* Allocate memory for new LWP */
    thread newthread = malloc(sizeof(context));
    unsigned long* stack = malloc(ssize * sizeof(unsigned long));

    /* Push data to the stack */
    GetSP(sp);

    sp--;
    SetSP(lwp_exit); /* Return Address */
    sp--;
    SetSP(fun); /* Function Pointer */
    sp--;
    SetSP(0xdeadbeef); /* Required fluff */

    /* Set Thread ID */
    __sync_fetch_and_add(&next_tid, 1);
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

	count++;
        args++;
    }

    newthread->state.fxsave = FPU_INIT; /* Store intital FPU state */

    /* Admit the new LWP into the scheduler */
    scheduler sched = lwp_get_scheduler();
    sched->admit(newthread);

    /* Updating outer scope variables */
    __sync_fetch_and_add(&running_threads, 1);
    __sync_fetch_and_add(&thread_len, 1);

    /* Add new thread to threads buffer */
    if (thread_len == cur_max_threads) {
	cur_max_threads += NUM_THREADS;
	threads = realloc(threads, cur_max_threads);
    }

    threads[newthread->tid - 1] = newthread; 
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
	return threads[tid-1];
    }
    else { /* Invalid Thread ID */
        return NULL;
    }
}
