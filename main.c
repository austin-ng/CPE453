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

    if (argc != 2) {
	fprintf(stderr, "usage: ./matrix_add method\n");
	fprintf(stderr, "(possible methods: seq, par)\n");
	exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[1], "par") == 0) {
	run_method = PARALLEL;
    }
    else if (strcmp(argv[1], "seq") == 0) {
	run_method = SEQUENTIAL;
    }
    else {
	fprintf(stderr, "'%s' is not a valid method\n", argv[1]);
	fprintf(stderr, "(possible methods: seq, par)\n");
	exit(EXIT_FAILURE);
    }

    build_matrices(); /* Populate matrices A and B */ 

    if (run_method == PARALLEL) {
	#pragma omp parallel
	{
    	for (i = 0; i < NUM_THREADS; i++) {
	    /* Create and execute each thread */
	    pthread_t tid; /* Thread id */
            pthread_attr_t attr; /* Set of thread attributes */
            pthread_attr_init(&attr); /* Get default attributes */

	    start_point = i * (MATRIX_SIZE / NUM_THREADS);

	    /* Create and wait for thread to complete/exit */
            pthread_create(&tid, &attr, matrix_addition, &start_point);
            pthread_join(tid, NULL);
	}
	}
    }
    else {
	/* Same code as parallel, but without 'pragma omp parallel' */
	for (i = 0; i < NUM_THREADS; i++) {
	    pthread_t tid;
	    pthread_attr_t attr;
	    pthread_attr_init(&attr);

	    start_point = i * (MATRIX_SIZE / NUM_THREADS);

            pthread_create(&tid, &attr, matrix_addition, &start_point);
            pthread_join(tid, NULL);
	}
    }

    print_matrices();

    return 0;
}
