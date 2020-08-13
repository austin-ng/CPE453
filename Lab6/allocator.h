#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MAX 16777216 /* Maximum amount of memory (16MB) */
#define CMD_MAX_LEN 80 /* Max length of allocator command */
#define MAX_PROCESSES 1024 /* Maximum number of created processes in one run */

void showCommands(void);
void setMemorySize(int m_size);
int validCommand(char* cmd);
void requestMemory(char* cmd);
void releaseMemory(char* cmd);
void compactMemory(void);
void printStatus(void);

#endif /* ALLOCATOR_H */
