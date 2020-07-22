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
int fds[NUM_READ_THREADS]; /* Read file descriptors of the input file */
pthread_t tids[NUM_READ_THREADS];
int write_fd; /* File descriptor for write */


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
    int beg, end;
    int s_len; /* Length of read segment (stored in param) */
    char* read_data; /* Buffer for data read from input file */

    sem_wait(&mutex);

    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt);
    }

    i = read_count - 1;

    sem_post(&mutex);

    beg = ((int*) param)[0];
    end = ((int*) param)[1];

    fds[i] = open(filename, O_RDONLY);

    lseek(fds[i], beg, SEEK_SET);

    s_len = end - beg;

    read_data = malloc(s_len + 1);
    memset(read_data, '\0', s_len + 1);

    read(fds[i], read_data, s_len);

    printf("%s\n", read_data);
    fflush(stdout);

    free(read_data);

    sem_wait(&mutex);

    close(fds[i]);

    read_count--;
    if (read_count == 0) {
        sem_post(&wrt);
    }
    
    sem_post(&mutex);
}


void create_read_thread(int start, int end) {
    /* Takes a read area (start, end) and creates a new read thread 
     * that reads the file in the specified area
     */
    
    int thread_vars[3];

    thread_vars[0] = start;
    thread_vars[1] = end;

    if ((end - start) <= 0) {
        /* If end position is not greater than start position */
        return;
    }

    pthread_create(&tids[read_count], NULL, read_file, (void*) thread_vars);
    pthread_join(tids[read_count], NULL);
}

void* file_write(void* param) {
    /* Takes a write starting position (pos) and creates a new write
     * thread that writes the specified characters (data) to the file
     */

    int fd;
    char* data;
    data = (char*) param;

    sem_wait(&wrt);

    /* begin critical section: write to file */
    fd = open(filename, O_WRONLY);
    if (lseek(fd, 0, SEEK_END) < 0) {
        perror("lseek");
    }
    else if (write(fd, data, strlen(data)) < 0){
        perror("write");
    }
    close(fd);

    /* end crticial section */
    sem_post(&wrt);

    return NULL;
}


void create_write_thread(char* data) {
    pthread_t tid;

    pthread_create(&tid, NULL, file_write, (void*) data);
    pthread_join(tid, NULL);
}
