#include "base_string.h"

#include <string.h>

String string_alloc(Arena *arena, U64 stringLength)
{
    String result = { 0 };
    result.content = push_array(arena, U8, (stringLength + 1));
    result.length = result.content ? stringLength : 0;
    return result;
}

String string_sub_string(String string, U64 start, U64 end)
{
    String result = { 0 };
    if (start < string.length && (end - start) >= 0)
    {
        result.content = string.content + start;
        result.length = end - start;
    }
    return result;
}

String string_advance(String string, U64 amount)
{
    String result = string_sub_string(string, amount, string.length);
    return result;
}

String string_skip_white_spaces(String string)
{
    while (string.length != 0) {
        if (string.content[0] == ' ' ||
            string.content[0] == '\n' ||
            string.content[0] == '\r' ||
            string.content[0] == '\t')
        {
            string.content++;
            string.length--;
        }
        else
        {
            break;
        }
    }
    return string;
}

B32 string_equals(String s1, String s2)
{
    B32 result = 0;
    if (s1.length == s2.length)
    {
        result = (memcmp(s1.content, s2.content, s1.length) == 0);
    }
    return result;
}

S64 string_index_of_u8(String string, U8 character)
{
    S64 result = -1;
    for (S64 stringIndex = 0; stringIndex < string.length; ++stringIndex)
    {
        if (string.content[stringIndex] == character)
        {
            result = stringIndex;
            break;
        }
    }
    return result;
}

U16 string_to_u16(String string)
{
    // TODO(speciial): make sure this doesn't wrap!
    U16 result = (U16)string_to_u64(string);
    return result;
}

U32 string_to_u32(String string)
{
    // TODO(speciial): make sure this doesn't wrap!
    U32 result = (U32)string_to_u64(string);
    return result;
}

U64 string_to_u64(String string)
{
    U64 result = 0;
    for (U64 stringIndex = 0; stringIndex < string.length; ++stringIndex)
    {
        U8 c = string.content[stringIndex];
        if (c >= '0' && c <= '9')
        {
            result = (result * 10) + (c - '0');
        }
        else
        {
            // TODO(speciial): logging?
            break;
        }
    }
    return result;
}
