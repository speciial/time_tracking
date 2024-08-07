#include "time_tracking.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct file_content
{
    long sizeInBytes;
    char *content;
};
typedef struct file_content FileContent;

static void freeFileContent(FileContent *fileContent)
{
    free(fileContent->content);
    fileContent->content = 0;
    fileContent->sizeInBytes = 0;
}

static void appendToFile(const char *filename, char *content, long bytesToWrite)
{
    FILE *filePointer = fopen(filename, "a");

    if (!filePointer)
    {
        fclose(filePointer);
        printf("Could not open file: %s\n", filename);
        return;
    }

    size_t bytesWritten = fwrite(content, sizeof(char), bytesToWrite, filePointer);
    if (bytesWritten == 0)
    {
        printf("Failed to write file content.\n");
    }

    fclose(filePointer);
}

static FileContent readFullFile(const char *filename)
{
    // TODO: Clean up checking/early returns

    FileContent result = {0};
    FILE *filePointer = fopen(filename, "r");

    if (!filePointer)
    {
        fclose(filePointer);
        printf("Could not open file: %s\n", filename);
        return result;
    }

    fseek(filePointer, 0, SEEK_END);
    long sizeInBytes = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);

    result.content = (char *)malloc(sizeInBytes + 1);
    if (!result.content)
    {
        fclose(filePointer);
        result.content = 0;
        printf("Could not allocate requested size: %d\n", sizeInBytes);
        return result;
    }
    memset(result.content, 0, sizeInBytes + 1);

    size_t bytesRead = fread(result.content, sizeof(char), sizeInBytes, filePointer);
    if (bytesRead > sizeInBytes)
    {
        fclose(filePointer);
        free(result.content);
        result.content = 0;
        printf("Bytes read exceeds bytes allocated: %d, %zu\n", sizeInBytes, bytesRead);
        return result;
    }
    result.sizeInBytes = sizeInBytes;

    fclose(filePointer);
    return result;
}

TTRTimeTracking ttr_internal_parseLine(char *line)
{
    TTRTimeTracking result = {0};

    // parse line
    int timerIndex = 0;
    int pauseIndex = 0;

    int charIndex = 0;
    while ((line[charIndex] != 0) && (line[charIndex] != '\n'))
    {
        // parse number
        bool isPauseTime = false;
        if (line[charIndex] == 'p')
        {
            isPauseTime = true;
            pauseIndex++;
            charIndex++; // skip over p
        }
        else
        {
            timerIndex++;
        }

        char timeStampString[10] = {0};
        for (int timeStampIndex = 0; timeStampIndex < 10; timeStampIndex++)
        {
            if (line[charIndex] == ',' || line[charIndex] == 0)
            {
                charIndex++;
                continue;
            }

            timeStampString[timeStampIndex] = line[charIndex];
            charIndex++;
        }
        charIndex++; // skip comma

        time_t timeStamp = (time_t)atoi(timeStampString);

        if (isPauseTime)
        {
            if (pauseIndex <= MAX_PAUSE_COUNT)
            {
                result.pauses[(pauseIndex - 1)] = timeStamp;
                result.pauseCount = pauseIndex;
            }
        }
        else
        {
            if (timerIndex == 1)
            {
                result.start = timeStamp;
            }
            else if (timerIndex == 2)
            {
                result.end = timeStamp;
            }
        }
    }

    return result;
}

void ttr_internal_printStartTime(TTRTimeTracking timeTracking)
{
    char startTimeString[13] = {0};
    strftime(startTimeString, 13, "%d.%m. %H:%M", localtime(&timeTracking.start));

    long pauseSecondsToSubstract = 0;
    if ((timeTracking.pauseCount % 2) == 0 && timeTracking.pauseCount > 0)
    {
        for (int pauseIndex = 0; pauseIndex < (timeTracking.pauseCount - 1); pauseIndex += 2)
        {
            pauseSecondsToSubstract += (long)difftime(
                timeTracking.pauses[pauseIndex + 1],
                timeTracking.pauses[pauseIndex]);
        }
    }

    time_t currentTime = time(0);
    long elapsedTimeInSeconds = (long)difftime(currentTime, timeTracking.start) - pauseSecondsToSubstract;

    struct tm elapsedTimeStruct = {0};
    elapsedTimeStruct.tm_sec += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_min += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_hour += elapsedTimeInSeconds % 24;

    char elapsedTimeString[6] = {0};
    strftime(elapsedTimeString, 6, "%H:%M", &elapsedTimeStruct);

    printf("[STARTED] %s, Worked for: %s\n", startTimeString, elapsedTimeString);
}

void ttr_internal_printPauseTime(TTRTimeTracking timeTracking)
{
    char pauseTimeString[13] = {0};
    strftime(pauseTimeString, 13, "%d.%m. %H:%M", localtime(&timeTracking.pauses[timeTracking.pauseCount - 1]));

    time_t currentTime = time(0);
    long elapsedTimeInSeconds = (long)difftime(currentTime, timeTracking.pauses[timeTracking.pauseCount - 1]);

    struct tm elapsedTimeStruct = {0};
    elapsedTimeStruct.tm_sec += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_min += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_hour += elapsedTimeInSeconds % 24;

    char elapsedTimeString[6] = {0};
    strftime(elapsedTimeString, 6, "%H:%M", &elapsedTimeStruct);

    printf("[PAUSED] %s, Paused for: %s\n", pauseTimeString, elapsedTimeString);
}

void ttr_internal_printCompleteTracking(TTRTimeTracking timeTracking)
{
    char dayString[7] = {0};
    strftime(dayString, 7, "%d.%m.", localtime(&timeTracking.start));

    char startTimeString[6] = {0};
    strftime(startTimeString, 6, "%H:%M", localtime(&timeTracking.start));

    char endTimeString[6] = {0};
    strftime(endTimeString, 6, "%H:%M", localtime(&timeTracking.end));

    long pauseSecondsToSubstract = 0;
    if ((timeTracking.pauseCount % 2) == 0 && timeTracking.pauseCount > 0)
    {
        for (int pauseIndex = 0; pauseIndex < (timeTracking.pauseCount - 1); pauseIndex += 2)
        {
            pauseSecondsToSubstract += (long)difftime(
                timeTracking.pauses[pauseIndex + 1],
                timeTracking.pauses[pauseIndex]);
        }
    }

    long elapsedTimeInSeconds = (long)difftime(timeTracking.end, timeTracking.start) - pauseSecondsToSubstract;
    long netElapsedTimeInSeconds = elapsedTimeInSeconds - 45 * 60;
    if (netElapsedTimeInSeconds < 0)
    {
        netElapsedTimeInSeconds = elapsedTimeInSeconds;
    }

    struct tm elapsedTimeStruct = {0};
    elapsedTimeStruct.tm_sec += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_min += elapsedTimeInSeconds % 60;
    elapsedTimeInSeconds /= 60;
    elapsedTimeStruct.tm_hour += elapsedTimeInSeconds % 24;

    char elapsedTimeString[6] = {0};
    strftime(elapsedTimeString, 6, "%H:%M", &elapsedTimeStruct);

    struct tm netElapsedTimeStruct = {0};
    netElapsedTimeStruct.tm_sec += netElapsedTimeInSeconds % 60;
    netElapsedTimeInSeconds /= 60;
    netElapsedTimeStruct.tm_min += netElapsedTimeInSeconds % 60;
    netElapsedTimeInSeconds /= 60;
    netElapsedTimeStruct.tm_hour += netElapsedTimeInSeconds % 24;

    char netElapsedTimeString[6] = {0};
    strftime(netElapsedTimeString, 6, "%H:%M", &netElapsedTimeStruct);

    printf("%s, %s - %s, Total Time: %s, Net Time: %s\n", dayString, startTimeString, endTimeString, elapsedTimeString, netElapsedTimeString);
}

void ttr_start()
{
    FileContent fileContent = readFullFile("record.ttr");

    long charIndex = fileContent.sizeInBytes;
    if (charIndex != 0)
    {
        while (charIndex != 0 && fileContent.content[charIndex] != '\n')
        {
            charIndex--;
        }
        if (charIndex != 0)
        {
            charIndex += 1; // skip over new line;
        }

        TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

        if (timeTracking.start != 0)
        {
            printf("Timer has already been started.\n");
            return;
        }
    }

    time_t timer = time(0);
    char timeStamp[12] = {0};
    sprintf(timeStamp, "%jd,", timer);
    appendToFile("record.ttr", timeStamp, 11);

    freeFileContent(&fileContent);
}

void ttr_end()
{
    FileContent fileContent = readFullFile("record.ttr");

    long charIndex = fileContent.sizeInBytes;
    if (charIndex != 0)
    {
        while (charIndex != 0 && fileContent.content[charIndex] != '\n')
        {
            charIndex--;
        }
        if (charIndex != 0)
        {
            charIndex += 1; // skip over new line;
        }

        TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

        if (timeTracking.start == 0)
        {
            printf("No timer has been started yet.\n");
            return;
        }
        if ((timeTracking.pauseCount % 2) != 0)
        {
            printf("Active pause has not been ended.\n");
            return;
        }
    }

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "%jd,\n", timer);
    appendToFile("record.ttr", timeStamp, 12);

    freeFileContent(&fileContent);
}

void ttr_startPause()
{
    FileContent fileContent = readFullFile("record.ttr");

    long charIndex = fileContent.sizeInBytes;
    if (charIndex != 0)
    {
        while (charIndex != 0 && fileContent.content[charIndex] != '\n')
        {
            charIndex--;
        }
        if (charIndex != 0)
        {
            charIndex += 1; // skip over new line;
        }

        TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

        if (timeTracking.start == 0)
        {
            printf("No timer has been started yet.\n");
            return;
        }
        if ((timeTracking.pauseCount % 2) != 0)
        {
            printf("Active pause has not been ended.\n");
            return;
        }
        if (timeTracking.pauseCount >= MAX_PAUSE_COUNT)
        {
            printf("You can only pause up to 3 times.\n");
            return;
        }
    }

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "p%jd,", timer);
    appendToFile("record.ttr", timeStamp, 12);

    freeFileContent(&fileContent);
}

void ttr_endPause()
{
    FileContent fileContent = readFullFile("record.ttr");

    long charIndex = fileContent.sizeInBytes;
    if (charIndex != 0)
    {
        while (charIndex != 0 && fileContent.content[charIndex] != '\n')
        {
            charIndex--;
        }
        if (charIndex != 0)
        {
            charIndex += 1; // skip over new line;
        }

        TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

        if (timeTracking.start == 0)
        {
            printf("No timer has been started yet.\n");
            return;
        }
        if ((timeTracking.pauseCount % 2) != 1)
        {
            printf("No active pause has been started.\n");
            return;
        }
        if (timeTracking.pauseCount >= MAX_PAUSE_COUNT)
        {
            printf("You can only pause up to 3 times.\n");
            return;
        }
    }

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "p%jd,", timer);
    appendToFile("record.ttr", timeStamp, 12);

    freeFileContent(&fileContent);
}

void ttr_show()
{
    FileContent fileContent = readFullFile("record.ttr");

    long charIndex = fileContent.sizeInBytes;
    if (charIndex != 0)
    {
        while (charIndex != 0 && fileContent.content[charIndex] != '\n')
        {
            charIndex--;
        }
        if (charIndex != 0)
        {
            charIndex += 1; // skip over new line;
        }

        TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

        if (timeTracking.start == 0)
        {
            printf("No timer has been started yet.\n");
            return;
        }

        if ((timeTracking.pauseCount % 2) == 0)
        {
            ttr_internal_printStartTime(timeTracking);
        }
        else
        {
            ttr_internal_printPauseTime(timeTracking);
        }

        return;
    }

    printf("No times have been tracked.\n");

    freeFileContent(&fileContent);
}

void ttr_showMonth()
{
    FileContent fileContent = readFullFile("record.ttr");

    if (fileContent.sizeInBytes != 0)
    {
        long charIndex = 0;

        charIndex = 0;
        while (fileContent.content[charIndex] != 0 && charIndex < fileContent.sizeInBytes)
        {
            TTRTimeTracking timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);

            if (timeTracking.start != 0 && timeTracking.end != 0)
            {
                ttr_internal_printCompleteTracking(timeTracking);
            }

            while (fileContent.content[charIndex] != '\n' && charIndex < fileContent.sizeInBytes)
            {
                charIndex++;
            }
            charIndex++;
        }
    }

    freeFileContent(&fileContent);
}
