#ifndef SORT_H
#define SORT_H

#define ARR_SIZE 100

extern int arr[ARR_SIZE];

void swap(int num1, int num2);
int partition(int left, int right, int pivot);
void quicksort(int left, int right);

#endif
