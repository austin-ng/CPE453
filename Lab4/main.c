#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "rw_threads.h"

int main(int argc, char* argv[]) {
    int fd; /* File descriptor of the input file */

    if (argc != 2) {
	fprintf(stderr, "usage: ./rw_problem <input_file>\n");
	exit(EXIT_FAILURE);
    }

    return 0;
}
