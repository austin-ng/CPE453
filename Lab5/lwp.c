#include <stdlib.h>

#include "fp.h"
#include "lwp.h"


static tid_t next_tid = 0; /* Thread ID value to set new thread to */
static int admitted = 0; /* Number of threads currently in the scheduler */

thread head; /* Head (start) of the global threads linked list */
thread tail; /* Tail (end) of the global threads linked list */
thread cur_thread; /* Current thread being ran by the scheduler */
scheduler cur_sched; /* Current scheduler being ran by the system */
context* startCont; /* Original/Starting context */


tid_t lwp_create(lwpfun fun, void* args, size_t ssize) {
    /* Takes a function pointer (fun), a list of arguments (args), and
     * a stack size (ssize) and creates a new lightweight process (LWP).
     * Returns the thread id of the new LWP
     */

    /* Allocate memory for new LWP */
    thread newthread = malloc(sizeof(context));
    unsigned long* stack = malloc(ssize * sizeof(unsigned long));

    /* Push data to the stack */
    unsigned long sp = 0;
    stack[sp++] = (unsigned long) lwp_exit; /* Exit Call */
    stack[sp++] = (unsigned long) fun; /* Function Pointer */
    stack[sp++] = 0xdeadbeef; /* Required fluff */

    if (next_tid == 0) { /* Check if this is first thread being made */
	head = newthread;
	tail = head;

	static struct scheduler init_sched = {NULL, NULL, rr_admit, rr_remove,
					      rr_next};
	scheduler RoundRobin = &init_sched;
	lwp_set_scheduler(RoundRobin);
    }
    __sync_fetch_and_add(&next_tid, 1);
    newthread->tid = next_tid;

    /* Set Stack Pointer and Stack Size */
    newthread->stack = stack;
    newthread->stacksize = ssize;

    /* Set Stack Pointer State */
    newthread->state.rsp = sp;
    newthread->state.rbp = sp;

    /* Store Arguments into Registers */
    int count = 1;
    int i = 0;
    unsigned long* arguments = (unsigned long*) args;

    while (arguments[i]) {
	switch (count) {
	    case 1:
        	newthread->state.rdi = (unsigned long) arguments[i];
                break;
	    case 2:
		newthread->state.rsi = (unsigned long) arguments[i];
		break;
	    case 3:
		newthread->state.rdx = (unsigned long) arguments[i];
		break;
	    case 4:
		newthread->state.rcx = (unsigned long) arguments[i];
		break;
	    case 5:
		newthread->state.r8 = (unsigned long) arguments[i];
		break;
	    case 6:
		newthread->state.r9 = (unsigned long) arguments[i];
		break;
	    default:
		break;
	}

	count++;
	i++;
    }

    newthread->state.fxsave = FPU_INIT; /* Store intital FPU state */

    /* Admit the new LWP into the scheduler */
    scheduler sched = lwp_get_scheduler();
    sched->admit(newthread);

    return newthread->tid;
}


void lwp_exit() {
    /* Terminates the LWP that called this function */

    cur_sched->remove(cur_thread);
    thread next_thread = cur_sched->next();

    free(cur_thread->stack);
    free(cur_thread);

    /* Insert context switch here */
}


tid_t lwp_gettid() {
    /* Returns the thread ID of the LWP that called this function */

    if (cur_thread) {
        return cur_thread->tid;
    }

    return NO_THREAD;
}


void lwp_yield() {
    /* Saves the context of the LWP that called this function and stops
     * the LWP from running
     */

    thread next_thread = cur_sched->next(); /* Get next thread */

    save_context(&(cur_thread->state)); /* Save current state */

    if (!next_thread) { /* If there's no new thread to go to */
	lwp_stop();
    }
    else {
	cur_thread = next_thread;
	load_context(&(cur_thread->state)); /* Load new state */
    }
}


void lwp_start() {
    /* Starts the LWP that called this function */

    thread next_thread = cur_sched->next(); /* Get next thread */

    if (next_thread) {
	cur_thread = next_thread; /* Pick next thread */
	save_context(&(startCont->state)); /* Save current state */
	load_context(&(cur_thread->state)); /* Load state of new thread */
    }
    else { /* There was no next */
        cur_thread = next_thread; /* So this should just be NULL */
    }
}


void lwp_stop() {
    /* Stops the LWP that called this function */

    load_context(&startCont->state); /* Go back to original state */

    if (cur_thread) { /* Also save the other state */
	save_context(&(cur_thread->state));
    }
}


void lwp_set_scheduler(scheduler fun) {
    /* Takes a new scheduling function (fun) and sets it as the current
     * scheduler
     */

    cur_sched = fun;
}


scheduler lwp_get_scheduler() {
    /* Returns the current scheduling function being ran */

    return cur_sched;
}


thread tid2thread(tid_t tid) {
    /* Takes a thread ID (tid) and returns the thread that is mapped to that
     * thread id
     */

    thread ct = head;

    if ((tid < (next_tid + 1)) && (tid > NO_THREAD)) {
        while (ct->tid != tid) {
	    ct = ct->lib_two;

	    if (ct == NULL) {
		return NULL;
	    }
        }

        return ct;
    }
    else { /* Invalid Thread ID */
        return NULL;
    }
}
