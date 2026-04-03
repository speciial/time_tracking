#ifndef base_string_h
#define base_string_h

#include "base_core.h"
#include "base_arena.h"

typedef struct String String;
struct String
{
    U8 *content;
    U64 length;
};

String string_alloc(Arena *arena, U64 stringLength);

String string_sub_string(String string, U64 start, U64 end);
String string_advance(String string, U64 amount);
String string_skip_white_spaces(String string);

B32 string_equals(String s1, String s2);
S64 string_index_of_u8(String string, U8 character);

U16 string_to_u16(String string);
U32 string_to_u32(String string);
U64 string_to_u64(String string);

#define str_lit(s) ((String){(U8 *)(s), (sizeof(s) - 1)}) 

#endif // base_string_h