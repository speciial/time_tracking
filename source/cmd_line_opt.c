#include "cmd_line_opt.h"

#include <string.h>

int clop_matchArgList(const char *arg, CLOPArgOptions *argOptions, int argCount)
{
    int result = -1;

    for (int argIndex = 0; argIndex < argCount; argIndex++)
    {
        CLOPArgOptions argOption = argOptions[argIndex];
        if (strcmp(arg, argOption.name) == 0)
        {
            result = argOption.index;
        }
    }

    return result;
}

int clop_matchArg(const char *arg, CLOPArgOptions argOption)
{
    int result = -1;

    if (strcmp(arg, argOption.name) == 0)
    {
        result = argOption.index;
    }

    return result;
}
