#ifndef files_H
#define files_H

#include <stdbool.h>

// TODO(speciial): Use arena and strings here
typedef struct
{
    char *Content;
    unsigned int Size;
} file_content;

bool ReadFile(file_content *Content, const char *Filename);

bool WriteFile(const char *Filename, const char *Content, unsigned int Size);

void FreeFileContent(file_content *Content);

bool FileExists(const char *Filename);

bool FileCreate(const char *Filename);

#endif // files_H