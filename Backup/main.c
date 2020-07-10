#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix_add.h"


int main() {
    pthread_t tid;
    pthread_attr_t attr;
    /* Number of add operations each thread must execute */
    int add_ops = MATRIX_SIZE / NUM_THREADS;

    build_matrices(); /* Populate matrices A and B */

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, matrix_addition, &add_ops);
    pthread_join(tid, NULL);

    return 0;
}
