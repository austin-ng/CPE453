#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

void add(struct node **head, char *name, int priority, int burst) {
    /* create new task */
    struct task *newTask = malloc(sizeof(struct task));
    int value = 0;
    __sync_fetch_and_add(&value, 1);
    newTask->name = name;
    newTask->tid = value;
    newTask->priority = priority;
    newTask->burst = burst;

    /* add task to linked list (create if null) */
    if (*head == NULL) {
        struct node *newNode = malloc(sizeof(struct node));
        newNode->task = newTask;
        newNode->next = NULL;
        *head = newNode;
    } else {
        insert(head, newTask);
    }
}

/* priority: iteratively execute highest priority and remove from list  */
void schedule(struct node *head) {
    struct node *temp;
    struct node *cur_highest;

    int turnaround = 0;
    int waiting = 0; 
    int response = 0;
    int totalturn = 0;
    int totalwait = 0;
    int totalresp = 0;
    int totaltasks = 0;

    while (head)
    {
        temp = head;
        cur_highest = temp;
        // find highest priority task remaining
        while (temp) {
            if (temp->task->priority >= cur_highest->task->priority)
                cur_highest = temp;
            temp = temp->next;
        }
        // run task and add to totals
        run(cur_highest->task, cur_highest->task->burst);


        totaltasks++;
        waiting += totalturn;
        turnaround += head->task->burst;
        response = waiting;
        totalturn += turnaround;
        totalwait += waiting;
        totalresp += response;

        // delete finished task from list and reiterate
        delete(&head, cur_highest->task);
    }

    // calculate average times
    printf("Average turnaround time: %d ms\n", totalturn / totaltasks);
    printf("Average waiting time: %d ms\n", totalwait / totaltasks);
    printf("Average response time: %d ms\n", totalresp / totaltasks);

}
