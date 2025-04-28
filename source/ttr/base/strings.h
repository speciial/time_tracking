#ifndef strings_H
#define strings_H

#include "arena.h"

typedef struct _string
{
    char *Content;
    int Length;
} string;

typedef struct _string_list_node
{
    string String;
    struct _string_list_node *Next;
} string_list_node;

typedef struct _string_list
{
    string_list_node *First;
    string_list_node *Last;
    int Count;
    int TotalSize;
} string_list;

string StringAlloc(arena *Arena, int Size);

void StringListAppend(arena *Arena, string_list *StringList, string String);

string StringListToString(arena *Arena, string_list *StringList);

void StringCopy(char *Destination, int DestinationSize, char *Source, int SourceSize);

string StringConcat(arena *Arena, string A, string B);

string StringFormat(arena *Arena, char *Format, ...);

int StringToInt(char *String, int Size);

#endif // strings_H