#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_add.h"


int main(int argc, char* argv[]) {
    int i;
    int run_method;
    int start_point;
    int add_ops;
    int* b;

    if (argc != 2) {
	fprintf(stderr, "usage: ./matrix_add <'par' or 'seq'>\n");
	exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[1], "par") == 0) {
	run_method = PARALLEL;
    }
    else if (strcmp(argv[1], "seq") == 0) {
	run_method = SEQUENTIAL;
    }
    else {
	fprintf(stderr, "usage: ./matrix_add <'par' or 'seq'>\n");
	exit(EXIT_FAILURE);
    }

    /* Number of add operations each thread must execute */
    add_ops = MATRIX_SIZE / NUM_THREADS;

    build_matrices(); /* Populate matrices A and B */

    if (run_method == PARALLEL) {
	//#pragma omp parallel
	//{
    	for (i = 0; i < NUM_THREADS; i++) {
	    /* Create and execute each thread */
	    pthread_t tid; /* Thread id */
            pthread_attr_t attr; /* Set of thread attributes */
            pthread_attr_init(&attr); /* Get default attributes */

	    start_point = i * add_ops;

	    /* Create and wait for thread to complete/exit */
            pthread_create(&tid, &attr, matrix_addition, &start_point);
            pthread_join(tid, NULL);
	}
	//}
    }
    else {
	for (i = 0; i < NUM_THREADS; i++) {
	    pthread_t tid;
	    pthread_attr_t attr;
	    pthread_attr_init(&attr);

	    start_point = i * add_ops;
            b = &start_point;
 
            pthread_create(&tid, &attr, &matrix_addition, &start_point);
            pthread_join(tid, NULL);
	}
    }

    return 0;
}
