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
// rr: reverse list then execute/delete until empty, decrementing burst times
void schedule(struct node *head) {
    struct node *prev = NULL;
    struct node *current = head;
    struct node *next = NULL;

    int turnaround = 0;
    int waiting = 0; 
    int response = 0;
    int totalturn = 0;
    int totalwait = 0;
    int totalresp = 0;
    int totaltasks = 0;


    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head = prev;
    int timespent = 0;
    struct node *temp = head;
    while (temp) {
        timespent = temp->task->burst - QUANTUM;
        if (timespent <= 0) {
            timespent = 0;
            run(temp->task, temp->task->burst);
            // add total turn/wait/resp times
            totaltasks++;
            waiting += totalturn;
            turnaround += temp->task->burst;
            response = waiting;
            totalturn += turnaround;
            totalwait += waiting;
            totalresp += response;
            // delete task after running
            delete(&head, temp->task);
        } else {
            run(temp->task, QUANTUM);
            // add times after running
            totaltasks++;
            waiting += totalturn;
            turnaround += temp->task->burst;
            response = waiting;
            totalturn += turnaround;
            totalwait += waiting;
            totalresp += response;
            // decrement remaining burst time
            temp->task->burst -= QUANTUM;
        }
        temp = temp->next;
        if (temp == NULL) {
            temp = head;
        }
    }

    // print times
    printf("Average turnaround time: %d ms\n", totalturn / totaltasks);
    printf("Average waiting time: %d ms\n", totalwait / totaltasks);
    printf("Average response time: %d ms\n", totalresp / totaltasks);

}



