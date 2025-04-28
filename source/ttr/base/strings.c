#include "strings.h"

#include <stdio.h>
#include <stdarg.h>

string StringAlloc(arena *Arena, int Size)
{
    string Result = { 0 };
    Result.Content = PushStruct(Arena, char, Size + 1);
    Result.Content[Size] = '\0';
    Result.Length = Size;
    return Result;
}

void StringListAppend(arena *Arena, string_list *StringList, string String)
{
    string_list_node *NewNode = PushStruct(Arena, string_list_node, 1);
    NewNode->String = String;

    if (!StringList->First && !StringList->Last)
    {
        StringList->First = NewNode;
        StringList->Last = NewNode;
    }
    else
    {
        StringList->Last->Next = NewNode;
        StringList->Last = NewNode;
    }
    StringList->Count += 1;
    StringList->TotalSize += NewNode->String.Length;
}

string StringListToString(arena *Arena, string_list *StringList)
{
    string Result = StringAlloc(Arena, StringList->TotalSize);

    string_list_node *CurrentNode = StringList->First;
    int CurrentPos = 0;
    while (CurrentNode)
    {
        memcpy_s(Result.Content + CurrentPos, StringList->TotalSize - CurrentPos,
                 CurrentNode->String.Content, CurrentNode->String.Length);
        CurrentPos += CurrentNode->String.Length;
        CurrentNode = CurrentNode->Next;
    }

    return Result;
}

void StringCopy(char *Destination, int DestinationSize, char *Source, int SourceSize)
{
    strncpy_s(Destination, DestinationSize, Source, SourceSize);
}

string StringConcat(arena *Arena, string A, string B)
{
    string Result = { 0 };
    Result = StringAlloc(Arena, (A.Length + B.Length));

    memcpy_s(Result.Content, Result.Length, A.Content, A.Length);
    memcpy_s(Result.Content + A.Length, Result.Length - A.Length,
             B.Content, B.Length);

    return Result;
}

string StringFormat(arena *Arena, char *Format, ...)
{
    va_list Arguments;
    va_start(Arguments, Format);
    
    string Result = { 0 };

    char Buffer[8092] = {0}; 
    vsnprintf(Buffer, 8092, Format, Arguments);
    
    int StringLength = strlen(Buffer);
    Result = StringAlloc(Arena, StringLength);
    memmove_s(Result.Content, Result.Length, Buffer, StringLength);

    va_end(Arguments);

    return Result;
}

int StringToInt(char *String, int Size)
{
    // Skip leading zeros
    int ZeroIndex = 0;
    while (*String == '0' && ZeroIndex < Size)
    {
        ++String;
        ++ZeroIndex;
    }

    int Result = 0;
    for (int CharIndex = 0; CharIndex < (Size - ZeroIndex); ++CharIndex)
    {
        Result = (Result * 10) + (String[CharIndex] - '0');
    }
    return Result;
}