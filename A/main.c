#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

int arr[ARR_SIZE];

int main(int argc, char* argv[]) {
    /* int arr[ARR_SIZE]; */
    FILE *in;
    int temp, start, end, i;
    start = end = 0;

    in = fopen(argv[1], "r");

    while (fscanf(in, "%d", &temp) == 1) {
        arr[end++] = temp;
    }

    printf("Original array:\n");
    for (i = start; i < end; i++) {
        printf("%d ", arr[i]);
    }

    quicksort(start, end - 1);
    printf("\nSorted array:\n");
    for (i = start; i < end; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

