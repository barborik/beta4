#ifndef __GEN_
#define __GEN_

#include "part.h"

int MAX_GEN;
int PER_GEN;

int found = 0;

typedef struct
{
    part_t *gen;
    part_t best;
} pop_t;

part_t best;

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

void cross(part_t *gen, part_t *best)
{
    qsort(gen, PER_GEN, sizeof(part_t), cmp_diff);

    for (int i = 2; i < PER_GEN; i += 2)
    {
        swap(&gen[i], &gen[i + 1]);

        if (rand() % 2)
        {
            mutate(&gen[i]);
        }

        if (rand() % 2)
        {
            mutate(&gen[i + 1]);
        }

        eval(&gen[i]);
        eval(&gen[i + 1]);

        if (gen[i].diff < best->diff)
        {
            copy(best, &gen[i]);
        }

        if (gen[i + 1].diff < best->diff)
        {
            copy(best, &gen[i + 1]);
        }

        if (!best->diff)
        {
            return;
        }
    }
}

void *run(void *arg)
{
    pop_t *pop = arg;
    for (int i = 0; i < MAX_GEN; i++)
    {
        cross(pop->gen, &pop->best);

        if (!pop->best.diff)
        {
            found = 1;
            return NULL;
        }

        if (found)
        {
            return NULL;
        }
    }

    return NULL;
}

int gen()
{
    int nthreads = get_nthreads();
    pthread_t *threads = malloc(nthreads * sizeof(pthread_t));

    srand(time(NULL));

    pinit(&best);

    pop_t *pops = malloc(nthreads * sizeof(pop_t));

    for (int i = 0; i < nthreads; i++)
    {
        pop_t *pop = &pops[i];
        pop->gen = malloc(PER_GEN * sizeof(part_t));

        pinit(&pop->best);

        for (int i = 0; i < PER_GEN; i++)
        {
            pinit(&pop->gen[i]);
            for (int j = 0; j < size; j++)
            {
                if (rand() % 2)
                {
                    pop->gen[i].buf[j] = 1;
                }
            }
        }

        pthread_create(&threads[i], NULL, run, &pops[i]);
    }

    for (int i = 0; i < nthreads; i++)
    {
        pop_t *pop = &pops[i];
        pthread_join(threads[i], NULL);

        if (pop->best.diff < best.diff)
        {
            copy(&best, &pop->best);
        }

        free(pop->gen);
    }

    return 0;
}

#endif