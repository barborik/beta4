#ifndef __PART_
#define __PART_

//#define KNAPSACK 60

#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#define sleep(x) usleep(x * 1000)
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    size_t sum1;
    size_t sum2;
    size_t diff;
    int8_t *buf;
} part_t;

#ifndef KNAPSACK
/*
+=============================+
|                             |
|          PARTITION          |
|                             |
+=============================+
*/

int *set;
int size;

void init(int n)
{
    size = n;
    set = malloc(sizeof(int *) * size);

    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        set[i] = rand() % 10 + 1;
    }
}

void pinit(part_t *part)
{
    part->diff = SIZE_MAX;
    part->buf = malloc(sizeof(int8_t) * size);
    memset(part->buf, 0, sizeof(int8_t) * size);
}

void copy(part_t *dst, part_t *src)
{
    dst->sum1 = src->sum1;
    dst->sum2 = src->sum2;
    dst->diff = src->diff;
    memcpy(dst->buf, src->buf, sizeof(int8_t) * size);
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
    printf(" = %zu\n", part->sum1);

    printf("S2 |");
    for (int i = 0; i < size; i++)
    {
        if (!part->buf[i])
        {
            printf(" %d", set[i]);
        }
    }
    printf(" = %zu\n", part->sum2);
}

void printp_brief(part_t *part)
{
    printf("sum(S1) = %d\nsum(S2) = %d\n", part->sum1, part->sum2);
}

#else
/*
+============================+
|                            |
|          KNAPSACK          |
|                            |
+============================+
*/

int *set;
int size;
int cap;

void init(int n)
{
    cap = KNAPSACK;
    size = n;
    set = malloc(sizeof(int) * size * 2);

    srand(time(NULL));
    for (int i = 0; i < size * 2; i += 2)
    {
        set[i + 0] = rand() % 10 + 1;
        set[i + 1] = rand() % 10 + 1;
    }
}

void pinit(part_t *part)
{
    part->diff = SIZE_MAX;
    part->buf = malloc(sizeof(int8_t) * size);
    memset(part->buf, 0, sizeof(int8_t) * size);
}

void copy(part_t *dst, part_t *src)
{
    dst->sum1 = src->sum1;
    dst->sum2 = src->sum2;
    dst->diff = src->diff;
    memcpy(dst->buf, src->buf, sizeof(int8_t) * size);
}

void eval(part_t *part)
{
    size_t wgt = 0, val = 0;

    for (int i = 0; i < size * 2; i += 2)
    {
        if (part->buf[i])
        {
            wgt += set[i + 0];
            val += set[i + 1];
        }
    }

    part->sum1 = wgt;
    part->sum2 = val;
    part->diff = 0;
    (wgt > cap) ? (part->diff = SIZE_MAX) : (part->diff = SIZE_MAX - val);
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
    printf("KNAPSACK (wgt, val) |");
    for (int i = 0; i < size * 2; i += 2)
    {
        if (part->buf[i])
        {
            printf(" (%d, %d)", set[i], set[i + 1]);
        }
    }
    printf("\nsum(wgt) = %zu\n", part->sum1);
    printf("sum(val) = %zu\n", part->sum2);
}

void printp_brief(part_t *part)
{
    printp(part);
}

#endif

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
