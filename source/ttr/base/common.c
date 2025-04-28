#include "common.h"

#include <stdio.h>
#include <stdarg.h>

void PrintFormatString(const char *Format, ...)
{
    va_list Arguments;
    va_start(Arguments, Format);
    vprintf(Format, Arguments);
    va_end(Arguments);
}
