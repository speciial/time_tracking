#ifndef write_h
#define write_h

#include <base_arena.h>
#include <base_string.h>

#include "ttr.h"

B32 ttr_write_entries(Arena *arena, TTR *ttr, String filePath);

String ttr_convert_entries_to_string(Arena *arena, TTR *ttr);

#endif // write_h