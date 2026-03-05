#ifndef base_arena_h
#define base_arena_h

#include "base_core.h"

/*
    Things I still want to do:
    - [ ] Sub allocate arenas
    - [ ] Mark and reset arenas
*/

typedef struct Arena Arena;
struct Arena
{
    U8 *baseAddress;
    U8 *currentAddress;
    U64 totalSize;
};

Arena arena_create(U64 size);
void arena_free(Arena *arena);
void *arena_alloc(Arena *arena, U64 size, U64 count);

#define push_struct(arena, type) ((type *)arena_alloc(arena, sizeof(type), 1))
#define push_array(arena, type, count) ((type *)arena_alloc(arena, sizeof(type), count))

#endif // base_arena_h