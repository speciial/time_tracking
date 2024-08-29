#include "time_tracking.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "common_fnc.h"
#include "files.h"

const char *RECORD_FILE_NAME = "record.ttr";
const char *SETTINGS_FILE_NAME = "ttr.settings";

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

void ttr_showMonth(int month)
{
    // NOTE: 0 means show everything.
    if ((month < 0) || (month > 12))
    {
        printf("The filter has to be in the range of 0 - 12");
        return;
    }

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
                if (month == 0)
                {
                    ttr_internal_printCompleteTracking(timeTracking);
                }
                else
                {
                    struct tm tmLocal = *localtime(&timeTracking.start);
                    if (tmLocal.tm_mon == (month - 1))
                    {
                        ttr_internal_printCompleteTracking(timeTracking);
                    }
                }
            }

            // Skip over the line that was just read
            while (fileContent.content[charIndex] != '\n' && charIndex < fileContent.sizeInBytes)
            {
                charIndex++;
            }
            charIndex++;
        }
    }

    freeFileContent(&fileContent);
}

void ttr_config(const char *setting, const char *value)
{
    createFileIfNotExists(SETTINGS_FILE_NAME);

    TTRSettings ttrSettings = ttr_internal_loadSettings(SETTINGS_FILE_NAME);

    if (strcmp(setting, "pausetime") == 0)
    {
        long pauseTime = (long)stringToInt(value);
        if (pauseTime > 0)
        {
            ttrSettings.defaultPauseTime = pauseTime;
        }
        else
        {
            printf("Invalid pause time.");
        }
    }
    else if (strcmp(setting, "format") == 0)
    {
        if (strcmp(value, "basic") == 0)
        {
            ttrSettings.format = BASIC;
        }
        else if (strcmp(value, "decimal") == 0)
        {
            ttrSettings.format = DECIMAL;
        }
        else
        {
            printf("Invalid format.");
        }
    }
    else
    {
        printf("Invalid setting. The following are allowed: pausetime, format.");
    }

    ttr_internal_saveSettings(SETTINGS_FILE_NAME, ttrSettings);
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
            charIndex++; // skip over new line;
        }

        timeTracking = ttr_internal_parseLine(&fileContent.content[charIndex]);
    }
    freeFileContent(&fileContent);

    return timeTracking;
}

TTRSettings ttr_internal_loadSettings(const char *fileName)
{
    TTRSettings settings = {0};

    FileContent fileContent = readFullFile(fileName);
    if (fileContent.sizeInBytes > 0)
    {
        long charIndex = 0;
        while (charIndex != fileContent.sizeInBytes && fileContent.content[charIndex] != 0)
        {
            // parse line
            char setting_key[32] = {0};
            char setting_value[32] = {0};

            char *buffer = setting_key;
            int bufferIndex = 0;
            while ((charIndex != fileContent.sizeInBytes) &&
                   (fileContent.content[charIndex] != 0) &&
                   (fileContent.content[charIndex] != '\n'))
            {
                if (fileContent.content[charIndex] == ':')
                {
                    buffer = setting_value;
                    bufferIndex = 0;
                    charIndex++;
                }

                if (bufferIndex < 32)
                {
                    buffer[bufferIndex] = fileContent.content[charIndex];

                    bufferIndex++;
                    charIndex++;
                }
            }
            charIndex++; // skip over new line;

            // TODO: Should we check if the settings file is valid?
            if (strcmp(setting_key, "pausetime") == 0)
            {
                long pauseTime = (long)stringToInt(setting_value);
                if (pauseTime > 0)
                {
                    settings.defaultPauseTime = pauseTime;
                }
            }
            else if (strcmp(setting_key, "format") == 0)
            {
                long format = (long)stringToInt(setting_value);
                if (format == (long)BASIC)
                {
                    settings.format = BASIC;
                }
                else if (format == (long)DECIMAL)
                {
                    settings.format = DECIMAL;
                }
            }
        }
    }
    else
    {
        printf("Couldn't parse setttings file.");
    }
    freeFileContent(&fileContent);

    return settings;
}

void ttr_internal_saveSettings(const char *fileName, TTRSettings settings)
{
    // NOTE: Buffer size is fixed here since I don't have a better solution right now.
    char bufferToWrite[512] = {0};

    int charsWritten = sprintf(bufferToWrite, "pausetime:%ld\nformat:%d", settings.defaultPauseTime, settings.format);

    if (charsWritten > 0)
    {
        writeFullFile(SETTINGS_FILE_NAME, bufferToWrite, charsWritten);
    }
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
    TTRSettings settings = ttr_internal_loadSettings(SETTINGS_FILE_NAME);

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

    if (settings.format == BASIC)
    {
        int hours = (int)(elapsedTimeInSeconds / (60 * 60));
        int minutes = (int)(((elapsedTimeInSeconds) - (hours * (60 * 60))) / 60);
        printf("[STARTED] %s, Worked for: %.2d:%.2d\n", startTimeString, hours, minutes);
    }
    else if (settings.format == DECIMAL)
    {
        float fHours = ((float)elapsedTimeInSeconds) / (60.0f * 60.0f);
        printf("[STARTED] %s, Worked for: %.2f\n", startTimeString, fHours);
    }
}

void ttr_internal_printPauseTime(TTRTimeTracking timeTracking)
{
    TTRSettings settings = ttr_internal_loadSettings(SETTINGS_FILE_NAME);

    char pauseTimeString[13] = {0};
    strftime(pauseTimeString, 13, "%d.%m. %H:%M", localtime(&timeTracking.pauses[timeTracking.pauseCount - 1]));

    time_t currentTime = time(0);
    long elapsedTimeInSeconds = (long)difftime(currentTime, timeTracking.pauses[timeTracking.pauseCount - 1]);

    if (settings.format == BASIC)
    {
        int hours = (int)(elapsedTimeInSeconds / (60 * 60));
        int minutes = (int)(((elapsedTimeInSeconds) - (hours * (60 * 60))) / 60);
        printf("[PAUSED] %s, Paused for: %.2d:%.2d\n", pauseTimeString, hours, minutes);
    }
    else if (settings.format == DECIMAL)
    {
        float fHours = ((float)elapsedTimeInSeconds) / (60.0f * 60.0f);
        printf("[PAUSED] %s, Paused for: %.2f\n", pauseTimeString, fHours);
    }
}

void ttr_internal_printCompleteTracking(TTRTimeTracking timeTracking)
{
    TTRSettings settings = ttr_internal_loadSettings(SETTINGS_FILE_NAME);

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
    long netElapsedTimeInSeconds = elapsedTimeInSeconds - settings.defaultPauseTime * 60;

    if (settings.format == BASIC)
    {
        int hours = (int)(elapsedTimeInSeconds / (60 * 60));
        int minutes = (int)(((elapsedTimeInSeconds) - (hours * (60 * 60))) / 60);

        int netHours = (int)max(0, ((netElapsedTimeInSeconds / (60 * 60))));
        int netMinutes = (int)max(0, ((((netElapsedTimeInSeconds) - (netHours * (60 * 60))) / 60)));

        printf("%s, %s - %s, Total Time: %.2d:%.2d, Net Time: %.2d:%.2d\n", dayString, startTimeString, endTimeString, hours, minutes, netHours, netMinutes);
    }
    else if (settings.format == DECIMAL)
    {
        float fHours = ((float)elapsedTimeInSeconds) / (60.0f * 60.0f);
        float fNetHours = fmaxf(0, ((float)netElapsedTimeInSeconds) / (60.0f * 60.0f));

        printf("%s, %s - %s, Total Time: %.2f, Net Time: %.2f\n", dayString, startTimeString, endTimeString, fHours, fNetHours);
    }
}
