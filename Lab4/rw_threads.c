#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "rw_threads.h"


static int read_count = 0; /* Number of threads currently reading file */

sem_t wrt; /* Semaphore for handling reading from/writing to file */
sem_t mutex; /* Semaphore for handling reading from file */
char* filename; /* Name of the input file */
int fds[NUM_READ_THREADS]; /* Read file descriptors of the input file */
pthread_t tids[NUM_READ_THREADS]; /* Thread id's of reader threads */


void init_sems() {
    /* Initializes the 'wrt' and 'mutex' semaphores */

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
    int beg, end; /* Start and end index of file to read */
    int s_len; /* Length of read segment (stored in param) */
    char* read_data; /* Buffer for data read from input file */

    sem_wait(&mutex); /* Lock mutex */

    /* Beginning of critical section */
    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt); /* Lock wrt (Prevent write thread from executing) */
    }

    i = read_count - 1;
    /* End of critical section */

    sem_post(&mutex); /* Unlock mutex */

    /* Get values from input parameter */
    beg = ((int*) param)[0];
    end = ((int*) param)[1];
    s_len = end - beg;

    fds[i] = open(filename, O_RDONLY);

    lseek(fds[i], beg, SEEK_SET); /* Set file position to desired position */

    /* Read from file */
    read_data = malloc(s_len + 1);
    memset(read_data, '\0', s_len + 1);

    read(fds[i], read_data, s_len);

    printf("%s\n", read_data);
    fflush(stdout);

    free(read_data);

    close(fds[i]);

    sem_wait(&mutex); /* Lock mutex */

    /* Beginning of the critical section */
    read_count--;
    if (read_count == 0) {
        sem_post(&wrt); /* Unlock wrt (Allow write thread to execute) */
    }
    /* End of the critical section */

    sem_post(&mutex); /* Unlock mutex */

    return NULL;
}


void* write_file(void* param) {
    /* Takes a string (param) and writes that string to the 
     * end of the input file
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


void create_write_thread(char* data) {
    /* Takes a string to write (data) and creates a write thread that
     * will write that data to the input file
     */
    
    pthread_t tid;

    pthread_create(&tid, NULL, write_file, (void*) data);
    pthread_join(tid, NULL);
}
