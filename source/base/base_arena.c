#include "base_arena.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEFAULT_ALIGNMENT sizeof(void *)

B32 is_power_of_two(uintptr_t value)
{
    return (value & (value - 1)) == 0;
}

U64 align_forward(U64 position, U64 align)
{
    assert(is_power_of_two(align));

    U64 modulo = position & (align - 1);
    if (modulo != 0)
    {
        position += align - modulo;
    }
    return position;
}

Arena arena_create(U64 size)
{
    Arena result = { 0 };

    result.baseAddress = (U8 *)malloc(size);
    result.currentAddress = result.baseAddress;
    result.totalSize = result.baseAddress ? size : 0;

    return result;
}

void arena_free(Arena *arena)
{
    if (arena->baseAddress)
    {
        free(arena->baseAddress);
    }
    arena->baseAddress = 0;
    arena->currentAddress = 0;
    arena->totalSize = 0;
}

void *arena_alloc(Arena *arena, U64 size, U64 count)
{
    void *result = 0;

    U64 alignedSize = align_forward(size * count, DEFAULT_ALIGNMENT);
    U64 remainingCapacity = (arena->baseAddress + arena->totalSize) - arena->currentAddress;

    // TODO(speciial): we should make sure, that this is checked in release as well. 
    assert(remainingCapacity > alignedSize);

    result = arena->currentAddress;
    arena->currentAddress += alignedSize;

    return memset(result, 0, alignedSize);
}
