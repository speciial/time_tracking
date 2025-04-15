#include "arena.h"

#include "defines.h"

#define DEFAULT_SCRATCH_SIZE KB(2)

arena NewArena(int Capacity)
{
    arena Result = { 0 };
    Result.Begin = malloc(Capacity);
    Result.End = Result.Begin ? Result.Begin + Capacity : 0;
    return Result;
}

arena NewScratchArena()
{
    arena Result = NewArena(DEFAULT_SCRATCH_SIZE);
    return Result;
}

void *ArenaAlloc(arena *Arena, int Size, int Align, int Count)
{
    int Padding = -(uintptr_t)Arena->Begin & (Align - 1);
    int Available = Arena->End - Arena->Begin - Padding;

    assert((Available > 0 && Count < (Available / Size)));

    void *Pointer = Arena->Begin + Padding;
    Arena->Begin += Padding + (Count * Size);

    return memset(Pointer, 0, (Count * Size) + Padding);
}