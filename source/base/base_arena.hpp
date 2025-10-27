#ifndef base_arena_HPP
#define base_arena_HPP

#include <stdint.h>

namespace base
{

struct arena
{
    uint8_t *baseAddress;
    uint8_t *currentAddress;
    uint64_t totalSize;
};

arena arena_create(uint64_t size);

void arena_release(arena *arena);

void *arena_alloc(arena *arena, uint64_t size, uint64_t count);

} // namespace base

#endif // base_arena_HPP