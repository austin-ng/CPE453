#ifndef MATRIX_ADD_H
#define MATRIX_ADD_H

#define MATRIX_SIZE 1000000 /* Size of matrix (10^6 From Lab 3 Instructions) */
#define NUM_THREADS 4 /* Number of threads to create */

#define PARALLEL 0 /* Value that indicates parallel threads */ 
#define SEQUENTIAL 1 /* Value that indicates sequential threads */

#define NSEC_IN_SEC 1000000000.0 /* Conversion of nsec->sec (1 s = 10^9 ns) */

void build_matrices(void);
void* matrix_addition(void* param);
void print_matrices(void);

#endif /* MATRIX_ADD_H */
