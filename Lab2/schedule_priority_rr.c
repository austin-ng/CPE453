#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

void scheduleRR(struct node *head);

// variables for average times
int turnaround = 0;
int waiting = 0; 
int response = 0;
int totalturn = 0;
int totalwait = 0;
int totalresp = 0;
int totaltasks = 0;

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

// priority_rr: create list and call scheduleRR() for each priority level 1-10
void schedule(struct node *head) {
    struct node *temp;
    struct node *ordered = NULL;
    int i;
    int curcnt;
    for (i = 10; i > 0; i--)
    {
        curcnt = 0;
        ordered = NULL;
        temp = head;
        while (temp) {
            // add to current priority list, increment curcnt
            if (temp->task->priority == i) {
                insert(&ordered, temp->task);
                curcnt++;
            }
            temp = temp->next;
        }
        // if there are multiples of same priority, use round robin scheduler
        if (curcnt > 1)
            scheduleRR(ordered);
        else {
            if (curcnt == 1) {
                // run and add times
                run(ordered->task, ordered->task->burst);
                totaltasks++;
                waiting += totalturn;
                turnaround += head->task->burst;
                response = waiting;
                totalturn += turnaround;
                totalwait += waiting;
                totalresp += response;
            }
        }
    }
    // calculate/print averages   
    printf("Average turnaround time: %d ms\n", totalturn / totaltasks);
    printf("Average waiting time: %d ms\n", totalwait / totaltasks);
    printf("Average response time: %d ms\n", totalresp / totaltasks);

}

// scheduleRR: repeateadly iterate through list until empty
// decrement Tasks by QUANTUM and delete when burst < 0
void scheduleRR(struct node *head) {
    int timespent = 0;
    struct node *temp = head;
    while (temp) {
        timespent = temp->task->burst - QUANTUM;
        if (timespent <= 0) {
            timespent = 0;
            // task is finished, delete after running, record stats
            run(temp->task, temp->task->burst);
            totaltasks++;
            waiting += totalturn;
            turnaround += head->task->burst;
            response = waiting;
            totalturn += turnaround;
            totalwait += waiting;
            totalresp += response;
            delete(&head, temp->task);
        } else {
            // run/record but do not delete, task is not finished
            run(temp->task, QUANTUM);
            totaltasks++;
            waiting += totalturn;
            turnaround += head->task->burst;
            response = waiting;
            totalturn += turnaround;
            totalwait += waiting;
        totalresp += response;
            temp->task->burst -= QUANTUM;
        }
        temp = temp->next;
        if (temp == NULL) {
            temp = head;
        }
    }
}
