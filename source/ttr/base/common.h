#ifndef common_H
#define common_H

#include <stdint.h>

// TODO(speciial): Use stdint 

#define KB(Value) ((Value) * 1024)
#define MB(Value) (KB(Value) * 1024)
#define GB(Value) (MB(Value) * 1024)

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

void PrintFormatString(const char *Format, ...);

#endif // common_H