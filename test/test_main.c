#include <stdio.h>

#include <time_tracking.h>

int main ()
{
    printf("Testing\n");

    if (!DummyFunction())
    {
        return 1;
    }

    return 0;
}