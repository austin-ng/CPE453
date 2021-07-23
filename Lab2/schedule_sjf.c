#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

void add(struct node **head, char *name, int priority, int burst) {
    // create new task
    struct task *newTask = malloc(sizeof(struct task));
    int value = 0;
    __sync_fetch_and_add(&value, 1);
    newTask->name = name;
    newTask->tid = value;
    newTask->priority = priority;
    newTask->burst = burst;

    // add task to linked list (create if null)
    if (*head == NULL) {
        struct node *newNode = malloc(sizeof(struct node));
        newNode->task = newTask;
        newNode->next = NULL;
        *head = newNode;
    } else {
        insert(head, newTask);
    }
}

// SJF: iteratively execute and remove task with shortest burst
void schedule(struct node *head) {
    struct node *temp;
    struct node *cur_lowest;

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
        cur_lowest = temp;
        while (temp) {
            if (temp->task->burst <= cur_lowest->task->burst) cur_lowest = temp;
            temp = temp->next;
        }
        //run task with lowest burst
        run(cur_lowest->task, cur_lowest->task->burst);

        // add turn/wait/resp times
        totaltasks++;
        waiting += totalturn;
        turnaround += cur_lowest->task->burst;
        response = waiting;
        totalturn += turnaround;
        totalwait += waiting;
        totalresp += response;

        // delete completed task from list and reset head
        delete(&head, cur_lowest->task);
        temp = head;
    }

    // print averages
    printf("Average turnaround time: %d ms\n", totalturn / totaltasks);
    printf("Average waiting time: %d ms\n", totalwait / totaltasks);
    printf("Average response time: %d ms\n", totalresp / totaltasks);

}
