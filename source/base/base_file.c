#include "base_file.h"

#include <stdio.h>
#include <string.h>

B32 file_read(String filePath, U8 *buffer, U64 size)
{
    B32 result = 0;
    FILE *fileHandle = fopen(filePath.content, "rb");
    if (fileHandle)
    {
        U64 bytesRead = fread(buffer, sizeof(U8), size, fileHandle);
        result = (bytesRead == size);

        // TODO(speciial): should we make sure the buffer is cleared if the read failed?
        if (!result)
        {
            memset(buffer, 0, size);
        }
    }
    return result;
}

B32 file_write(String filePath, U8 *buffer, U64 size)
{
    B32 result = 0;
    FILE *fileHandle = fopen(filePath.content, "rb");
    if (fileHandle)
    {
        U64 bytesWritten = fwrite(buffer, sizeof(U8), size, fileHandle);
        result = (bytesWritten == size); // TODO(speciial): is this condition correct?
    }
    return result;
}

U64 file_size(String filePath)
{
    U64 result = 0;
    FILE *fileHandle = fopen(filePath.content, "rb");
    if (fileHandle)
    {
        fseek(fileHandle, 0, SEEK_END);
        result = (U64)ftell(fileHandle);
        fclose(fileHandle);
    }
    return result;
}

B32 file_exists(String filePath)
{
    // TODO(speciial): implement
    return 0;
}

B32 file_create(String filePath)
{
    // TODO(speciial): implement
    return 0;
}
