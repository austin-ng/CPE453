#ifndef MATRIX_ADD_H
#define MATRIX_ADD_H

#define MATRIX_SIZE 10 /* Size of matrix (10^6 From Lab 3 Instructions) */
#define NUM_THREADS 5 /* Number of threads to create */

#define PARALLEL 0 /* Value that indicates parallel threads */ 
#define SEQUENTIAL 1 /* Value that indicates sequential threads */

void build_matrices(void);
void* matrix_addition(void* param);

#endif /* MATRIX_ADD_H */ 
