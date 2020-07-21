#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int read_count = 0;
/*static int thread_count = 0;*/

sem_t wrt;
sem_t mutex;
char* filename;
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
     * the values stored in the int array to read data from the input
     * file
     */

    int fd; /* File descriptor of input file (stored in param[0]) */
    int len; /* Length of read segment (stored in param[1]) */
    char* read_data; /* Buffer for data read from input file */

    fd = ((int*) param)[0];
    len = ((int*) param)[1];

    read_data = malloc(len);

    if (read(fd, read_data, len) < 0) {
        perror("read");
    }
    else {
        printf("%s\n", read_data);
        fflush(stdout);
    }

    free(read_data);

    close(fd);

    pthread_exit(0);
}

void create_read_thread(int start, int end) {
    /* Takes a read area (start, end) and creates a new read thread 
     * that reads the file in the specified area
     */
    
    int fd; /* File descriptor for input file */
    pthread_t tid;
    pthread_attr_t attr;
    int thread_vars[2];

    if ((end - start) <= 0) {
        /* If end position is not greater than start position */
        return;
    }

    sem_wait(&mutex);
    write(STDOUT_FILENO, "progress\n", 9);

    /* Check if there is a write thread that might/might not be finished */

    /* JUST IN CASE (PLS NO KEEP IF NO NEEDED)
    if ((thread_count > 0) && (read_count == 0)) {
        while (thread_count > 0) {
            if (tids[--thread_count]) {
                pthread_join(tids[thread_count]);
            }
        }
    } */

    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt);
        write(STDOUT_FILENO, "progress\n", 9);
    }
    sem_post(&mutex);
    write(STDOUT_FILENO, "progress\n", 9);

    pthread_attr_init(&attr);

    fd = open(filename, O_RDONLY);
    if (lseek(fd, start, SEEK_SET) < 0) {
        perror("lseek");
    }
    else {
        thread_vars[0] = fd;
        thread_vars[1] = end - start;

        pthread_create(&tid, &attr, read_file, thread_vars);
    }

    sem_wait(&mutex);
    write(STDOUT_FILENO, "progress\n", 9);

    read_count--;
    if (read_count == 0) {
        sem_post(&wrt);
        write(STDOUT_FILENO, "progress\n", 9);
    }
    sem_post(&mutex);
    write(STDOUT_FILENO, "progress\n", 9);
}

void file_write(void* param) {
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

}