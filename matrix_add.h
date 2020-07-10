#ifndef MATRIX_ADD_H
#define MATRIX_ADD_H

#define MATRIX_SIZE 1000000 /* Size of matrix (10^6 From Lab 3 Instructions) */
#define NUM_THREADS 1 /* Number of threads to create */

void build_matrices(void);
void* matrix_addition(void* param);

#endif /* MATRIX_ADD_H */ 
