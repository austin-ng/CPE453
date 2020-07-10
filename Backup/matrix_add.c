#include <pthread.h>
#include <stdlib.h>

#include "matrix_add.h"

static int a[MATRIX_SIZE]; /* First matrix (Matrix A) */
static int b[MATRIX_SIZE]; /* Second matrix (Matrix B) */
static int matrix_pos = 0;

void build_matrices() {
    /* Using the buffers that have already been initialized above (a[] and
     * b[]), fill up each array with random numbers using rand() function
     */
 
    int i;

    for (i = 0; i < MATRIX_SIZE; i++) {
	a[i] = rand();
	b[i] = rand();
    } 
}

void* matrix_addition(void* ops) {
    /* Using the buffers that have been filled by the build_matrices()
     * function, build a result array contains the result of the addition
     * of matrix a[] and b[]
     */

    int i;
    int res[MATRIX_SIZE]; /* Resulting matrix */

    i = *((int*) ops); /* Convert void* to int */

    while (i--) {
	res[matrix_pos] = a[matrix_pos] + b[matrix_pos];
	matrix_pos++;
    }

    pthread_exit(0);
}
