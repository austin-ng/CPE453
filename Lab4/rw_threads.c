#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "rw_threads.h"

static int read_count = 0;
/*static int thread_count = 0;*/

sem_t wrt;
sem_t mutex;
char* filename;
int fds[NUM_READ_THREADS]; /* File descriptors of the input file */
/*pthread_t tids[NUM_READ_THREADS];*/

void init_sems() {
    sem_init(&wrt, 0, 1);
    sem_init(&mutex, 0, 1);
}

void save_filename(char* name) {
    /* Creates a pointer (filename) that points to the input file name,
     * stored in argv[1] (name)
     */

    filename = name;
}

void* read_file(void* param) {
    /* Takes parameters from pthread_create() call (param) and uses
     * that value to read data from the input file
     */

    int i;
    int len; /* Length of read segment (stored in param) */
    char* read_data; /* Buffer for data read from input file */

    len = ((int*) param)[0];
    i = ((int*) param)[1];

    read_data = malloc(len + 1);
    memset(read_data, '\0', len+1);

    if (read(fds[i], read_data, len) < 0) {
        perror("read");
    }
    else {
        printf("%s\n", read_data);
        fflush(stdout);
    }

    free(read_data);

    pthread_exit(0);
}

void create_read_thread(int start, int end) {
    /* Takes a read area (start, end) and creates a new read thread 
     * that reads the file in the specified area
     */
    
    pthread_t tid;
    pthread_attr_t attr;
    int thread_vars[2];

    thread_vars[0] = end - start;

    if (thread_vars[0] <= 0) {
        /* If end position is not greater than start position */
        return;
    }

    sem_wait(&mutex);

    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt);
    }
    sem_post(&mutex);

    fds[read_count-1] = open(filename, O_RDONLY);

    pthread_attr_init(&attr);

    /* if (lseek(fds[read_count-1], start, SEEK_SET) < 0) {
        perror("lseek");
    }
    else {*/
    thread_vars[1] = read_count - 1;

    pthread_create(&tid, &attr, read_file, (void*) thread_vars);

    sem_wait(&mutex);

    read_count--;
    if (read_count == 0) {
        sem_post(&wrt);
    }
    
    sem_post(&mutex);

    close(fds[read_count]);
    /*}*/
}

void* file_write(void* param) {
    /* Takes a write starting position (pos) and creates a new write
     * thread that writes the specified characters (data) to the file
     */

    char* data;
    data = (char*) param;

    int fd = open(filename, O_WRONLY);
    if (lseek(fd, 0, SEEK_END) < 0) {
        perror("lseek");
    }
    else if (write(fd, data, strlen(data)) < 0){
        perror("write");
    }

    close(fd);

}

void create_write_thread() {
    int fd; /* File descriptor for input file */
    pthread_t tid;
    pthread_attr_t attr;

}