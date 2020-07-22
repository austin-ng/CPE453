#ifndef RW_THREADS_H
#define RW_THREADS_H

#define NUM_READ_THREADS 8 /* Number of read threads to create */

void init_sems(void);
void save_filename(char* name);
void* read_file(void* param);
void create_read_thread(int start, int end);
void* file_write(void* param);
void create_write_thread(char* data);

#endif /* RW_THREADS_H */
