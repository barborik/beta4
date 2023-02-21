#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#include "zahl.h"

#include "part.h"

typedef struct
{
    part_t best;
    part_t start;
    part_t end;
} ival_t;

/*

int subsize = 1 << size;
int start_i = subsize * i;
int end_i = start_i + subsize - 1;

for (int j = 0; j < size; j++)
{
    start->buf[j] = (start_i >> (size - j - 1)) & 1;
    end->buf[j] = (end_i >> (size - j - 1)) & 1;
}

*/
void split(part_t *start, part_t *end, int n, int i)
{
    z_t tmp, one, start_i, end_i, subsize;

    zinit(tmp);
    zinit(one);
    zinit(start_i);
    zinit(end_i);
    zinit(subsize);

    zsets(one, "1");
    zlsh(start_i, one, size);

    zseti(tmp, n);

    zdiv(subsize, start_i, tmp);

    zseti(tmp, i);
    zmul(start_i, subsize, tmp);

    zadd(end_i, start_i, subsize);
    zsub(end_i, end_i, one);

    for (int j = 0; j < size; j++)
    {
        zrsh(tmp, start_i, size - j - 1);
        zand(tmp, tmp, one);
        zzero(tmp) ? (start->buf[j] = 0) : (start->buf[j] = 1);

        zrsh(tmp, end_i, size - j - 1);
        zand(tmp, tmp, one);
        zzero(tmp) ? (end->buf[j] = 0) : (end->buf[j] = 1);
    }

    zfree(tmp);
    zfree(one);
    zfree(start_i);
    zfree(end_i);
    zfree(subsize);
}

void *search(void *arg)
{
    ival_t *ival = arg;

    while (cmp(&ival->start, &ival->end))
    {
        eval(&ival->start);

        if (ival->start.diff < ival->best.diff)
        {
            copy(&ival->best, &ival->start);
        }

        inc(&ival->start);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <number of items>\n", argv[0]);
        return 1;
    }

    jmp_buf env;
    if (setjmp(env))
    {
        return 1;
    }
    zsetup(env);

    init(atoi(argv[1]));

    part_t best;
    pinit(&best);

    int nthreads = get_nthreads();
    if (nthreads > size)
    {
        nthreads = size - 1;
    }

    ival_t *ivals = malloc(nthreads * sizeof(ival_t));
    pthread_t *threads = malloc(nthreads * sizeof(pthread_t));

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < nthreads; i++)
    {
        ival_t *ival = &ivals[i];

        pinit(&ival->best);
        pinit(&ival->start);
        pinit(&ival->end);

        split(&ival->start, &ival->end, nthreads, i);

        pthread_create(&threads[i], NULL, search, ival);
    }

    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(threads[i], NULL);

        if (ivals[i].best.diff < best.diff)
        {
            copy(&best, &ivals[i].best);
        }
    }

    gettimeofday(&end, NULL);

    printp(&best);
    int elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("total elapsed time:\n%d microseconds\n%d milliseconds\n%f seconds\n", elapsed, elapsed / 1000, (double)elapsed / 1000000);

    zunsetup();
    return 0;
}
