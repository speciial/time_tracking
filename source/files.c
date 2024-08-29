#include "files.h"

#include <stdio.h>
#include <stdlib.h>

void freeFileContent(FileContent *fileContent)
{
    free(fileContent->content);
    fileContent->content = 0;
    fileContent->sizeInBytes = 0;
}

void appendToFile(const char *filename, char *content, long bytesToWrite)
{
    FILE *filePointer = fopen(filename, "a");

    if (filePointer)
    {
        size_t bytesWritten = fwrite(content, sizeof(char), bytesToWrite, filePointer);
        if (bytesWritten == 0)
        {
            printf("Failed to write file content.\n");
        }
    }
    else
    {
        printf("Could not open file: %s\n", filename);
    }

    fclose(filePointer);
}

FileContent readFullFile(const char *filename)
{
    FileContent result = {0};
    FILE *filePointer = fopen(filename, "r");

    if (filePointer)
    {
        fseek(filePointer, 0, SEEK_END);
        long sizeInBytes = ftell(filePointer);
        fseek(filePointer, 0, SEEK_SET);

        char *content = (char *)malloc(sizeInBytes + 1);
        if (!content)
        {
            content = 0;
            sizeInBytes = 0;
            printf("Could not allocate requested size: %d\n", sizeInBytes);
        }
        memset(content, 0, sizeInBytes + 1);

        size_t bytesRead = fread(content, sizeof(char), sizeInBytes, filePointer);
        if (bytesRead > sizeInBytes)
        {
            free(content);
            content = 0;
            sizeInBytes = 0;
            printf("Bytes read exceeds bytes allocated: %d, %zu\n", sizeInBytes, bytesRead);
        }

        result.content = content;
        result.sizeInBytes = sizeInBytes;
    }
    else
    {
        printf("Could not open file: %s\n", filename);
    }

    fclose(filePointer);
    return result;
}

void writeFullFile(const char *filename, const char *content, int bytesToWrite)
{
    FILE *filePointer = fopen(filename, "w");

    if (filePointer)
    {
        size_t bytesWritten = fwrite(content, sizeof(char), bytesToWrite, filePointer);
        if (bytesWritten <= 0)
        {
            printf("File content could not be written.");
        }
    }
    else
    {
        printf("Could not open file: %s\n", filename);
    }

    fclose(filePointer);
}

void createFileIfNotExists(const char *filename)
{
    // NOTE: When opening a file with x-flag while specified file
    //       already exists, fclose will throw an error on that
    //       file handle. The handle only needs to be cleaned if
    //       a new file was actually created.
    FILE *fileToCreate = fopen(filename, "wx");
    if (fileToCreate)
    {
        fclose(fileToCreate);
    }
}
