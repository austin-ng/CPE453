#include <stdlib.h>

#include "fp.h"
#include "lwp.h"

#define NUM_THREADS 64

static tid_t next_tid = 0; /* Thread ID value to set new thread to */
static int admitted = 0; /* Number of threads currently in the scheduler */

static thread head = NULL; /* Head (start) of the global threads linked list */
static thread tail = NULL; /* Tail (end) of the global threads linked list */
static thread cur_thread = NULL; /* Current thread being ran by the scheduler */
static scheduler cur_sched; /* Current scheduler being ran by the system */
static context* startCont; /* Original/Starting context */

/* Round Robin Scheduling Functions */
void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next(void);


void rr_admit(thread new) {
    /* Adds a new thread pointed to by (new) to the scheduler */
 
    if (admitted > 0) {
	thread prev_thread = tail;
	prev_thread->lib_two = new;
        new->lib_one = prev_thread;
        new->lib_two = NULL;
	tail = new;
    }

    admitted++;
}


void rr_remove(thread victim) {
    /* Removes the thread pointed to by (victim) from the scheduler and fixes 
     * the global linked list to fix broken ends
     */ 

    thread ct = head;
    int thread_found = TRUE;

    while (ct->tid != victim->tid) {
	ct = ct->lib_two;

	if (ct == NULL) {
	    thread_found = FALSE;
	    break;
        }
    }

    if (thread_found) {
	if ((ct->tid) == (head->tid)) {
	    head = ct->lib_two;
	    if (head) {
	        head->lib_one = NULL;
	    }
        }
	else {
	    victim->lib_one->lib_two = victim->lib_two;
	    if ((ct->tid) == (tail->tid)) {
		tail = victim->lib_one;
	    }
	    else {
        	victim->lib_two->lib_one = victim->lib_one;
	    }
	}
    }
   
    admitted--;
}


thread rr_next() {
    /* Saves the context of the current thread, and returns the thread that
     * is now running on the scheduler
     */

    thread next_thread;

    next_thread = cur_thread->lib_two;

    if (next_thread) {
	return next_thread; /* Get next item in linked list */
    }
    else {
        return head; /* Jump back to beginning in linked list */
    }
}


tid_t lwp_create(lwpfun fun, void* args, size_t ssize) {
    /* Takes a function pointer (fun), a list of arguments (args), and
     * a stack size (ssize) and creates a new lightweight process (LWP).
     * Returns the thread id of the new LWP
     */

    unsigned long sp; /* Stack indexer for traversing stack */

    /* Allocate memory for new LWP */
    thread newthread = malloc(sizeof(context));
    unsigned long* stack = malloc(ssize * sizeof(unsigned long));

    /* Push data to the stack */
    sp = ssize; /* Move to TOS (Top of Stack) */

    stack[sp--] = (unsigned long) lwp_exit; /* Return address */
    stack[sp--] = (unsigned long) fun; /* Function pointer */
    stack[sp] = 0xdeadbeef; /* Base Pointer (required fluff) */
 
    newthread->tid = ++next_tid; /* Set tid */

    /* Set Stack Pointer and Stack Size */
    newthread->stack = stack;
    newthread->stacksize = ssize;

    /* Set Stack Pointer State */
    newthread->state.rsp = (unsigned long) (stack + sp);
    newthread->state.rbp = (unsigned long) (stack + sp);

    /* Store arguments into register */
    newthread->state.rdi = (unsigned long) args;
    newthread->state.fxsave = FPU_INIT; /* Store intital FPU state */

    if (admitted == 0) { /* Check if this is first thread being made */
	head = newthread;
	tail = head;

	#ifndef INIT_SCHED
	    #define INIT_SCHED
	    static struct scheduler init_sched = {NULL, NULL, rr_admit,
					          rr_remove, rr_next};
	    scheduler RoundRobin = &init_sched;
	    lwp_set_scheduler(RoundRobin);
	#endif
    }

    cur_sched->admit(newthread); /* Admit the new LWP into the scheduler */

    return next_tid;
}


void lwp_exit() {
    /* Terminates the LWP that called this function */

    thread next_thread = cur_sched->next();
    cur_sched->remove(cur_thread); /* Remove from scheduler */

    free(cur_thread->stack); /* Free memory */
    free(cur_thread);

    cur_thread = next_thread;

    if (admitted != 0) {
        load_context(&(cur_thread->state)); /* Context switch */
    }
    else {
	lwp_stop();
    }
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

    if (admitted > 1) {
        thread next_thread = cur_sched->next(); /* Get next thread */
        save_context(&(cur_thread->state)); /* Save current state */

        cur_thread = next_thread;
        load_context(&(cur_thread->state)); /* Load new state */
    }
    else {
	save_context(&(cur_thread->state));
	load_context(&(cur_thread->state));
    }
}


void lwp_start() {
    /* Starts the LWP thiat called this function */

    /* Save "real" context (original state) */
    startCont = malloc(sizeof(context));
    unsigned long sp = 0;
    GetSP(sp);
    startCont->state.rsp = sp;
    startCont->state.rbp = sp;
    startCont->state.fxsave = FPU_INIT;

    save_context(&(startCont->state));

    if (!cur_thread) { /* If there isn't an unfinished thread in scheduler */
        if (admitted) {
            cur_thread = head;
        }
    }
    
    load_context(&(cur_thread->state));	/* Load first thread's context */
}


void lwp_stop() {
    /* Stops the LWP that called this function */

    load_context(&(startCont->state)); /* Go back to original state */

    if (cur_thread) { /* Also save the other state */
	save_context(&(cur_thread->state));
    }
}


void lwp_set_scheduler(scheduler fun) {
    /* Takes a new scheduling function (fun) and sets it as the current
     * scheduler
     */

    if (fun == NULL) {
        return;
    }

    thread nextthread;
    scheduler current;

    if (cur_sched) {
        current = cur_sched;
        nextthread = current->next();
        while (nextthread != NULL) {
            current->remove(nextthread);
            fun->admit(nextthread);
        }
    }

    cur_sched = fun;
    return;
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
