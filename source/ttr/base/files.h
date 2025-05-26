#ifndef files_H
#define files_H

#include <stdbool.h>

// TODO(speciial): Use arena and strings here
// TODO(speciial): Add info about failed read to struct
typedef struct
{
    char *Content;
    unsigned int Size;
} file_content;

// TODO(speciial): Return the file content instead of having an out parameter
bool ReadFile(file_content *Content, const char *Filename);

bool WriteFile(const char *Filename, const char *Content, unsigned int Size);

void FreeFileContent(file_content *Content);

bool FileExists(const char *Filename);

bool FileCreate(const char *Filename);

#endif // files_H