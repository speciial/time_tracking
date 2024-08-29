#ifndef FILES_H
#define FILES_H

struct file_content
{
    long sizeInBytes;
    char *content;
};
typedef struct file_content FileContent;

void freeFileContent(FileContent *fileContent);

void appendToFile(const char *filename, char *content, long bytesToWrite);

FileContent readFullFile(const char *filename);

void writeFullFile(const char *filename, const char *content, int bytesToWrite);

void createFileIfNotExists(const char *filename);

#endif // FILES_H