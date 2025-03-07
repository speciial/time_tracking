#include <stdio.h>
#include <stdlib.h>

#include <time_tracking.h>

int main ()
{
    printf("Testing\n");

    FILE *test = fopen("test_record.ttr", "r");

    if (!test)
    {
        return 2;
    }

    if (!DummyFunction())
    {
        return 1;
    }

    return 0;
}