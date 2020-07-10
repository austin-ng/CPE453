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
	a[i] = 1 * i;
	b[i] = 2 * i;
    } 
}

void* matrix_addition(void* ops) {
    /* Using the buffers that have been filled by the build_matrices()
     * function, build a result array contains the result of the addition
     * of matrix a[] and b[]
     */

    int i;
    int add_ops;

    i = *((int*) ops); /* Convert void* to int */
    add_ops = MATRIX_SIZE / NUM_THREADS;

    while (i < (i + add_ops)) {
	res[i] = a[i] + b[i];
	i++;
    }

    pthread_exit(0);
}
