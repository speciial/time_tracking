#include "base_string.hpp"

namespace base
{

unsigned int string_length(const char *string)
{
    unsigned int result = 0;

    while (string[result] != 0)
    {
        result++;
    }

    return result;
}

bool string_equals(const char *string1, const char *string2)
{
    bool result = false;

    if (string_length(string1) == string_length(string2))
    {
        result = true;
        for (int stringIndex = 0; stringIndex < string_length(string1); stringIndex++)
        {
            if (string1[stringIndex] != string2[stringIndex])
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

} // namespace base

