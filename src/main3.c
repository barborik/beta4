#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "part.h"

int PER_GEN;
int MAX_GEN;

int cmp_diff(const void *a, const void *b)
{
    part_t *part1 = a;
    part_t *part2 = b;

    if (part1->diff > part2->diff)
    {
        return 1;
    }

    if (part1->diff < part2->diff)
    {
        return -1;
    }

    return 0;
}

void swap(part_t *part1, part_t *part2)
{
    int swp = rand() % size, tmp;
    for (int i = 0; i < swp; i++)
    {
        tmp = part1->buf[i];
        part1->buf[i] = part2->buf[i];
        part2->buf[i] = tmp;
    }
}

void mutate(part_t *part)
{
    int bit = rand() % size;
    part->buf[bit] ? (part->buf[bit] = 0) : (part->buf[bit] = 1);
}

void cross(part_t *new, part_t *old, part_t *best)
{
    qsort(old, PER_GEN, sizeof(part_t), cmp_diff);

    new[0] = old[0];
    new[1] = old[1];

    for (int i = 2; i < PER_GEN; i += 2)
    {
        new[i] = old[i];
        new[i + 1] = old[i + 1];
        swap(&new[i], &new[i + 1]);

        if (rand() % 2)
        {
            mutate(&new[i]);
        }

        if (rand() % 2)
        {
            mutate(&new[i + 1]);
        }

        eval(&new[i]);
        eval(&new[i + 1]);

        if (new[i].diff < best->diff)
        {
            copy(best, &new[i]);
        }

        if (new[i + 1].diff < best->diff)
        {
            copy(best, &new[i + 1]);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("usage: %s <n items> <max gen> <per gen>\n", argv[0]);
        return 1;
    }

    init(atoi(argv[1]));
    MAX_GEN = atoi(argv[2]);
    PER_GEN = atoi(argv[3]);

    part_t best;
    pinit(&best);

    srand(time(NULL));

    part_t *old = malloc(PER_GEN * sizeof(part_t));
    part_t *new = malloc(PER_GEN * sizeof(part_t));

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // gen 0
    for (int i = 0; i < PER_GEN; i++)
    {
        pinit(&old[i]);
        for (int j = 0; j < size; j++)
        {
            if (rand() % 2)
            {
                old[i].buf[j] = 1;
            }
        }
    }

    for (int i = 0; i < MAX_GEN; i++)
    {
        cross(new, old, &best);
    }

    gettimeofday(&end, NULL);

    printf("sum(S1) = %d\nsum(S2) = %d\n", best.sum1, best.sum2);
    int elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("total elapsed time:\n%d microseconds\n%d milliseconds\n%f seconds", elapsed, elapsed / 1000, (double)elapsed / 1000000);

    return 0;
}
