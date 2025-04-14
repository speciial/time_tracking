#include "files.h"

#include <stdio.h>
#include <stdlib.h>

bool ReadFile(file_content *Content, const char *Filename)
{
    bool Result = false;

    FILE *FileHandle = fopen(Filename, "rb");
    if (FileHandle)
    {
        fseek(FileHandle, 0, SEEK_END);
        long FileSizeInBytes = ftell(FileHandle);
        fseek(FileHandle, 0, SEEK_SET);

        char *Buffer = (char *)malloc(FileSizeInBytes + 1);
        if (Buffer)
        {
            unsigned int BytesRead = fread(Buffer, sizeof(char), FileSizeInBytes, FileHandle);
            if (BytesRead == FileSizeInBytes)
            {
                Buffer[FileSizeInBytes] = 0;
                Content->Content = Buffer;
                Content->Size = FileSizeInBytes;

                Result = true;
            }
            else
            {
                printf("Failed to read file.\n");
                free(Buffer);
            }
        }
        else
        {
            printf("Failed to alloc buffer.\n");
        }
        fclose(FileHandle);
    }
    else
    {
        printf("Failed to open file.\n");
    }

    return Result;
}

bool WriteFile(const char *Filename, const char *Content, unsigned int Size)
{
    bool Result = false;

    FILE *FileHandle = fopen(Filename, "w");
    if (FileHandle)
    {
        unsigned int BytesWritten = fwrite(Content, sizeof(char), Size, FileHandle);
        if (BytesWritten == Size)
        {
            Result = true;
        }
        else
        {
            printf("Failed to write file.\n");
        }
        fclose(FileHandle);
    }
    else
    {
        printf("Failed to open file.\n");
    }

    return Result;
}

void FreeFileContent(file_content *Content)
{
    free(Content->Content);
    Content->Content = 0;
    Content->Size = 0;
}

bool FileExists(const char *Filename)
{
    bool Result = false;

    FILE *FileHandle = fopen(Filename, "r");
    if (FileHandle)
    {
        Result = true;
        fclose(FileHandle);
    }

    return Result;
}

bool FileCreate(const char *Filename)
{
    bool Result = false;

    FILE *FileHandle = fopen(Filename, "wx");
    if (FileHandle)
    {
        Result = true;
        fclose(FileHandle);
    }

    return Result;
}