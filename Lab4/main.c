#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rw_threads.h"

int main(int argc, char* argv[]) {
    int fd; /* Test file descriptor */

    if (argc != 2) {
        fprintf(stderr, "usage: ./rw_problem <input_file>\n");
        exit(EXIT_FAILURE);
    }

    /* Check if input file can be written to and read from */
    if ((fd = open(argv[1], O_WRONLY,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)) {
        /* Opening file for write caused an error */
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    close(fd);

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        /* Opening file for read caused an error */
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    close(fd);

    init_sems();
    save_filename(argv[1]);

    /* Testing Code */
    create_read_thread(0, 10);
    create_read_thread(0, 5);
    create_read_thread(0, 7);
    create_read_thread(3, 4);
    create_read_thread(4, 8);

    create_write_thread("This is the new line\n");

    create_read_thread(49, 60);

    return 0;
}
