#ifndef __PART_
#define __PART_

#include <time.h>
#include <stdlib.h>

int n;
int *set;
int *sub;

void init(int size)
{
    n = size;
    set = malloc(sizeof(int) * n);
    sub = malloc(sizeof(int) * n);

    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        set[i] = rand() % 10 + 1;
    }
}

#endif