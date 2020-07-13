#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix_add.h"

static int a[MATRIX_SIZE]; /* First matrix (Matrix A) */
static int b[MATRIX_SIZE]; /* Second matrix (Matrix B) */
static int res[MATRIX_SIZE]; /* Resulting matrix (Matrix A + B) */


void build_matrices() {
    /* Using the buffers that have already been initialized above (a[] and
     * b[]), fill up each array with random numbers using rand() function
     */
 
    int i;

    for (i = 0; i < MATRIX_SIZE; i++) {
	a[i] = i;
	b[i] = i * 2;
    } 
}


void* matrix_addition(void* ops) {
    /* Using the buffers that have been filled by the build_matrices()
     * function, build a result array contains the result of the addition
     * of matrix a[] and b[]
     */

    int start, end; /* Start and end index values for matrices */
    int add_ops; /* Number of add operations each thread must perform */
    int rem; /* Number of add operations not assigned to thread yet */

    add_ops = MATRIX_SIZE / NUM_THREADS;
    start = *((int*) ops); /* Convert void* to int */
    end = start + add_ops;
    rem = MATRIX_SIZE - (NUM_THREADS * add_ops);

    if (start == ((NUM_THREADS - 1) * add_ops)) {
	/* If last thread */
	if (rem > 0) {
	    /* Force last thread to execute remaining add operations */
	    end += rem;
	}
    }

    while (start < end) {
	res[start] = a[start] + b[start];
	start++;
    }

    pthread_exit(0);
}


void print_matrices() {
    /* Extra function used to print the results of the matrix addition.
     * (Note: Printed in the form - A[n] | B[n] | RESULT[n])
     */

    int i;

    printf("A | B | RESULT\n");

    for (i = 0; i < MATRIX_SIZE; i++) {
	printf("%d | %d | %d\n", a[i], b[i], res[i]);
    }
}
