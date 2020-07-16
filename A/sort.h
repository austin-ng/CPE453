#ifndef SORT_H
#define SORT_H

#define ARR_SIZE 3000000

extern int arr[ARR_SIZE];
extern int len, len1, len2;
extern int res[ARR_SIZE];
extern int *start1;
extern int *start2;

int cmpfunc(const void *a, const void *b);
void *sortArr1(void *ops);
void *sortArr2(void *ops);
void *mergeArrs(void *ops);

#endif
