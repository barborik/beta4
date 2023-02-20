#ifndef __PART_
#define __PART_

#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    int sum1;
    int sum2;
    int diff;
    int *buf; // big endian
} part_t;

int *set;
int size;

void init(int n)
{
    size = n;
    set = malloc(sizeof(int) * size);

    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        set[i] = rand() % 10 + 1;
    }
}

void pinit(part_t *part)
{
    part->diff = INT_MAX;
    part->buf = malloc(sizeof(int) * size);
    memset(part->buf, 0, sizeof(int) * size);
}

void copy(part_t *dst, part_t *src)
{
    dst->sum1 = src->sum1;
    dst->sum2 = src->sum2;
    dst->diff = src->diff;
    memcpy(dst->buf, src->buf, sizeof(int) * size);
}

void eval(part_t *part)
{
    int sum1 = 0, sum2 = 0;

    for (int i = 0; i < size; i++)
    {
        part->buf[i] ? (sum1 += set[i]) : (sum2 += set[i]);
    }

    part->sum1 = sum1;
    part->sum2 = sum2;
    part->diff = max(sum1, sum2) - min(sum1, sum2);
}

int cmp(part_t *part1, part_t *part2)
{
    for (int i = 0; i < size; i++)
    {
        if (part1->buf[i] != part2->buf[i])
        {
            return part1->buf[i] - part2->buf[i];
        }
    }
    return 0;
}

int inc(part_t *part)
{
    for (int i = size - 1; i > -1; i--)
    {
        if (!part->buf[i])
        {
            part->buf[i] = 1;
            for (int j = i + 1; j < size; j++)
            {
                part->buf[j] = 0;
            }
            return 1;
        }
    }
    return 0;
}

void printp(part_t *part)
{
    printf("S1 |");
    for (int i = 0; i < size; i++)
    {
        if (part->buf[i])
        {
            printf(" %d", set[i]);
        }
    }
    printf(" = %d\n", part->sum1);

    printf("S2 |");
    for (int i = 0; i < size; i++)
    {
        if (!part->buf[i])
        {
            printf(" %d", set[i]);
        }
    }
    printf(" = %d\n", part->sum2);
}

int get_nthreads()
{
#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#endif