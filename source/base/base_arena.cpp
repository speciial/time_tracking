#include "base_arena.hpp"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace base
{

#define DEFAULT_ALIGNMENT sizeof(void *)

bool is_power_of_two(uintptr_t value)
{
    return (value & (value - 1)) == 0;
}

uint64_t align_forward(uint64_t position, uint64_t alignTo)
{
    assert(is_power_of_two(alignTo));

    uint64_t modTwo = position & (alignTo - 1);
    if (modTwo != 0)
    {
        position += alignTo - modTwo;
    }
    return position;
}

arena arena_create(uint64_t sizeInBytes)
{
    arena result = {};

    result.baseAddress = (uint8_t *)malloc(sizeInBytes);
    result.currentAddress = result.baseAddress;
    result.totalSize = result.baseAddress ? sizeInBytes : 0;

    return result;
}

void arena_release(arena *arena)
{
    if (arena->baseAddress)
    {
        free(arena->baseAddress);
    }
    arena->baseAddress = 0;
    arena->currentAddress = 0;
    arena->totalSize = 0;
}

void *arena_alloc(arena *arena, uint64_t size, uint64_t count)
{
    void *result = 0;

    uint64_t alignedSize = align_forward(size * count, DEFAULT_ALIGNMENT);
    uint64_t remainingCapacity = (arena->baseAddress + arena->totalSize) - arena->currentAddress;
    assert(remainingCapacity > alignedSize);

    result = arena->currentAddress;
    arena->currentAddress += alignedSize;

    return memset(result, 0, alignedSize);
}

} // namespace base