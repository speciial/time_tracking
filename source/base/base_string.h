#ifndef base_string_h
#define base_string_h

#include "base_core.h"

typedef struct String String;
struct String
{
    U8 *content;
    U64 length;
};

#define str_lit(s) ((String){(U8 *)(s), (sizeof(s) - 1)}) 

#endif // base_string_h