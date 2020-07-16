#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

/* comparator for qsort */
int cmpfunc(const void *a, const void *b) {
    return ( *(int*)a - *(int*)b );
}

/* sort left half */
void *sortArr1(void *ops) {
    qsort(start1, len1, sizeof(int), cmpfunc);
    pthread_exit(0);
}

/* sort right half */
void *sortArr2(void *ops) {
    qsort(start2, len2, sizeof(int), cmpfunc);
    pthread_exit(0);
}

/* merge two sorted arrays */
void *mergeArrs(void *ops) {
    int count, left, right, mid;
    left = count = 0;
    right = len / 2;
    mid = right;

    /* add smallest number from each half to res until one is finished */
    while (left < mid && right < len) {
        if (arr[left] < arr[right]) {
            res[count++] = arr[left++];
        } else {
            res[count++] = arr[right++];
        }
    }
    /* finish filling res with left if unfinished */
    while (left < mid) {
        res[count++] = arr[left++];
    }
    /* finish filling res with right if unfinished */
    while (right < len) {
        res[count++] = arr[right++];
    }
    /* exit merge thread */
    pthread_exit(0);
}





