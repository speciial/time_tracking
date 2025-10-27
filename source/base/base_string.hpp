#ifndef base_string_HPP
#define base_string_HPP

namespace base
{

struct string
{
    char *content;
    unsigned int length;
};

unsigned int string_length(const char *string);

bool string_equals(const char *string1, const char *string2);

} // namespace base

#endif // base_string_HPP