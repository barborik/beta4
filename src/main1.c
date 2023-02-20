#include <time.h>
#include <stdio.h>

#include "part.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <number of items>\n", argv[0]);
        return 1;
    }

    init(atoi(argv[1]));

    part_t part;
    part_t best;

    pinit(&part);
    pinit(&best);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (inc(&part))
    {
        eval(&part);

        if (part.diff < best.diff)
        {
            copy(&best, &part);
        }
    }

    gettimeofday(&end, NULL);

    printp(&best);
    int elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("total elapsed time:\n%d microseconds\n%d milliseconds\n%f seconds\n", elapsed, elapsed / 1000, (double)elapsed / 1000000);

    return 0;
}
