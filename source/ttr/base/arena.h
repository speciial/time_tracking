#ifndef arena_H
#define arena_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>
#include <assert.h>

// NOTE(speciial): Stolen from https://nullprogram.com/blog/2023/09/27/ :)

typedef struct _arena
{
    char *Begin;
    char *End;
} arena;

#define PushStruct(Arena, Type, Count) ((Type *)ArenaAlloc(Arena, sizeof(Type), alignof(Type), Count))

arena NewArena(int Capacity);

arena NewScratchArena();

void *ArenaAlloc(arena *Arena, int Size, int Align, int Count);

#endif arena_H