#include <math.h>
#include <stdio.h>
#include "part.h"

int diff()
{
    size_t sum1 = 0, sum2 = 0;

    for (int i = 0; i < n; i++)
    {
        sub[i] ? sum1 += set[i] : sum2 += set[i];
    }

    return abs(sum1 - sum2);
}

void load(int index)
{
    
}

int main()
{
    init(10);

    for (size_t i = 0; i < 1 << n; i++)
    {
    }

    return 0;
}
