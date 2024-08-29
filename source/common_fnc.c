#include "common_fnc.h"

int stringToInt(const char *string)
{
    // NOTE: This works for now but there are multiple issues with this implementation:
    //       - A return value of 0 may indicate that an error occurred
    //       - You might want to have a negative number to be parsed
    if (string != 0 && (string[0] >= 48 && string[0] <= 57))
    {
        int result = strtol(string, 0, 10);
        return result;
    }
    else
    {
        printf("Failed integer conversion for argument: %s\n", string);
        return -1;
    }
}