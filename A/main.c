#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

int arr[ARR_SIZE];
int len, len1, len2;
int res[ARR_SIZE];
int *start1;
int *start2;

int main(int argc, char* argv[]) {
    FILE *in;
    FILE *out;
    int temp, start, i;
    pthread_t tid1, tid2, tid3;
    start = 0;

    /* open input file and fill arr */
    in = fopen(argv[1], "r");
    while (fscanf(in, "%d", &temp) == 1) {
        arr[len++] = temp;
    }

    /* assign global variables */
    start1 = arr;
    start2 = arr + len / 2;
    len1 = len / 2;
    len2 = len - len / 2;

    /* create two sorting threads for each half */
    pthread_create(&tid1, NULL, sortArr1, &len);
    pthread_create(&tid2, NULL, sortArr2, &len);

    /* wait for sorting threads */
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    /* create merging thread, wait for terminate */
    pthread_create(&tid3, NULL, mergeArrs, &len);
    pthread_join(tid3, NULL);

    /* write sorted array to output */
    out = fopen("out.txt", "w");
    for (i = start; i < len; i++) {
        fprintf(out, "%d ", res[i]);
    }
    fprintf(out, "\n");

    return 0;
}


