#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

void swap(int num1, int num2) {
    int temp = arr[num1];
    arr[num1] = arr[num2];
    arr[num2] = temp;
}

int partition(int left, int right, int pivot) {
    int leftPointer = left - 1;
    int rightPointer = right;

    while (1) {
        while (arr[++leftPointer] < pivot) {
            /* do nothing */
        }
        while (rightPointer > 0 && arr[--rightPointer] > pivot) {
            /* do nothing */
        }
        if (leftPointer >= rightPointer) {
            break;
        } else {
            swap(leftPointer, rightPointer);
        }
    }
    swap(leftPointer, right);
    return leftPointer;
}

void quicksort(int left, int right) {
    if (right - left <= 0) {
        return;
    } else {
        int pivot = arr[right];
        int partitionPoint = partition(left, right, pivot);
        quicksort(left, partitionPoint - 1);
        quicksort(partitionPoint + 1, right);
    }
}

