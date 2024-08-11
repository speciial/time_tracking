#include "time_tracking.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "files.h"

const char *RECORD_FILE_NAME = "record.ttr";

void ttr_start()
{
    createFileIfNotExists(RECORD_FILE_NAME);

    TTRTimeTracking timeTracking = ttr_internal_parseLastRecordEntry(RECORD_FILE_NAME);
    if (timeTracking.start != 0)
    {
        printf("Timer has already been started.\n");
        return;
    }

    time_t timer = time(0);
    char timeStamp[12] = {0};
    sprintf(timeStamp, "%jd,", timer);
    appendToFile(RECORD_FILE_NAME, timeStamp, 11);
}

void ttr_end()
{
    TTRTimeTracking timeTracking = ttr_internal_parseLastRecordEntry(RECORD_FILE_NAME);
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

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "%jd,\n", timer);
    appendToFile(RECORD_FILE_NAME, timeStamp, 12);
}

void ttr_startPause()
{
    TTRTimeTracking timeTracking = ttr_internal_parseLastRecordEntry(RECORD_FILE_NAME);
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

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "p%jd,", timer);
    appendToFile(RECORD_FILE_NAME, timeStamp, 12);
}

void ttr_endPause()
{
    TTRTimeTracking timeTracking = ttr_internal_parseLastRecordEntry(RECORD_FILE_NAME);
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

    time_t timer = time(0);
    char timeStamp[13] = {0};
    sprintf(timeStamp, "p%jd,", timer);
    appendToFile(RECORD_FILE_NAME, timeStamp, 12);
}

void ttr_show()
{
    TTRTimeTracking timeTracking = ttr_internal_parseLastRecordEntry(RECORD_FILE_NAME);

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
}

void ttr_showMonth()
{
    FileContent fileContent = readFullFile(RECORD_FILE_NAME);

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

TTRTimeTracking ttr_internal_parseLastRecordEntry(const char *fileName)
{
    TTRTimeTracking timeTracking = {0};

    FileContent fileContent = readFullFile(RECORD_FILE_NAME);
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

        timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);
    }
    freeFileContent(&fileContent);

    return timeTracking;
}

struct tm ttr_internal_convertSecondsToTM(long seconds)
{
    struct tm result = {0};
    if (seconds > 0)
    {
        result.tm_sec += seconds % 60;
        seconds /= 60;
        result.tm_min += seconds % 60;
        seconds /= 60;
        result.tm_hour += seconds % 24;
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

    struct tm elapsedTimeStruct = ttr_internal_convertSecondsToTM(elapsedTimeInSeconds);
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

    struct tm elapsedTimeStruct = ttr_internal_convertSecondsToTM(elapsedTimeInSeconds);
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

    struct tm elapsedTimeStruct = ttr_internal_convertSecondsToTM(elapsedTimeInSeconds);
    char elapsedTimeString[6] = {0};
    strftime(elapsedTimeString, 6, "%H:%M", &elapsedTimeStruct);

    struct tm netElapsedTimeStruct = ttr_internal_convertSecondsToTM(netElapsedTimeInSeconds);
    char netElapsedTimeString[6] = {0};
    strftime(netElapsedTimeString, 6, "%H:%M", &netElapsedTimeStruct);

    printf("%s, %s - %s, Total Time: %s, Net Time: %s\n", dayString, startTimeString, endTimeString, elapsedTimeString, netElapsedTimeString);
}
