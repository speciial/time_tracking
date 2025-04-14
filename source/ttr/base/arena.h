#ifndef arena_H
#define arena_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>
#include <assert.h>

#define KB(Value) ((Value) * 1024)
#define MB(Value) (KB(Value) * 1024)
#define GB(Value) (MB(Value) * 1024)

// NOTE(speciial): Stolen from https://nullprogram.com/blog/2023/09/27/ :)

typedef struct _arena
{
    char *Begin;
    char *End;
} arena;

#define PushStruct(Arena, Type, Count) ((Type *)ArenaAlloc(Arena, sizeof(Type), alignof(Type), Count))

arena NewArena(int Capacity);

void *ArenaAlloc(arena *Arena, int Size, int Align, int Count);

#endif arena_H