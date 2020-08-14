#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MAX 16777216 /* Maximum amount of memory (16MB) */
#define CMD_MAX_LEN 80 /* Max length of allocator command */

typedef struct memory_block {
    char* name; /* Name of the process */
    int start; /* Start address of the process's memory block */
    int end; /* End address of the process's memory block */
    struct memory_block* next; /* Pointer to next memory block */
} memblock;

void showCommands(void);
void setMemorySize(int m_size);
int validCommand(char* cmd);

void requestMemory(void);
void releaseMemory(void);
void compactMemory(void);
void printStatus(void);

void freeMemory(void);

#endif /* ALLOCATOR_H */
