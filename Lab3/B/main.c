#include <time.h>
#include <omp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_add.h"


int main(int argc, char* argv[]) {
    int i;
    int run_method; /* Flag for implementation to use (0: par, 1: seq) */
    int start_point; /* Index of where a thread should start add operations */
    pthread_t* tids; /* List of tids */
    pthread_attr_t* attrs; /* List of each tid's corresponding attributes */
    struct timespec start, finish;
    double elapsed;

    /* Check command line arguments */
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

    tids = malloc(NUM_THREADS * sizeof(pthread_t));
    attrs = malloc(NUM_THREADS * sizeof(pthread_attr_t));

    clock_gettime(CLOCK_MONOTONIC, &start); /* Get start time */

    if (run_method == PARALLEL) {
	#pragma omp parallel
	{
    	    for (i = 0; i < NUM_THREADS; i++) {
	        /* Create and execute each thread */
		pthread_attr_init(&attrs[i]);

	    	start_point = i * (MATRIX_SIZE / NUM_THREADS);
            	pthread_create(&tids[i], NULL, matrix_addition, &start_point);
	    }
	}

	for (i = 0; i < NUM_THREADS; i++) {
	    pthread_join(tids[i], NULL);
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

    clock_gettime(CLOCK_MONOTONIC, &finish); /* Get finish time */

    /* Calculate elapsed time and print it to stdout */
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / NSEC_IN_SEC;
    printf("Time Elapsed: %f\n", elapsed);

    /* Uncomment this if you want to print the resulting matrix
    print_matrices();
    */

    free(tids);
    free(attrs);
    
    return 0;
}
