/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;

    // started adding code here
    struct node *tasklist = NULL;
    if (argc != 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    in = fopen(argv[1],"r");
    if (!in) {
        printf("Error: '%s' not found\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        if (priority > 10 || priority < 1) {
            printf("Error: Task %s has invalid priority %d\n", name, priority);
            exit(EXIT_FAILURE);
        }

        // add the task to the scheduler's list of tasks
        add(&tasklist,name,priority,burst);

        free(temp);
    }

    // TESTING: call traverse()
    // printf("Task list order:\n");
    // traverse(tasklist);
    // END TESTING

    fclose(in);

    // invoke the scheduler
    printf("Scheduling and executing tasks...\n");
    schedule(tasklist);

    return 0;
}

