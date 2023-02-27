#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "gen.h"
#include "part.h"

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

    int nthreads = get_nthreads();
    pthread_t *threads = malloc(nthreads * sizeof(pthread_t));

    srand(time(NULL));

    pinit(&best);

    pop_t *pops = malloc(nthreads * sizeof(pop_t));

    struct timeval start, end;
    gettimeofday(&start, NULL);

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

    gettimeofday(&end, NULL);

    printp_brief(&best);
    int elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("total elapsed time:\n%d microseconds\n%d milliseconds\n%f seconds\n", elapsed, elapsed / 1000, (double)elapsed / 1000000);

    return 0;
}
