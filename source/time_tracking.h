#ifndef TIME_TRACKING_H
#define TIME_TRACKING_H

#include <time.h>

enum
{
    MAX_PAUSE_COUNT = 6
};

struct ttr_time_tracking
{
    time_t start;
    time_t end;

    time_t pauses[MAX_PAUSE_COUNT];
    int pauseCount;
};
typedef struct ttr_time_tracking TTRTimeTracking;

void ttr_start();

void ttr_end();

void ttr_startPause();

void ttr_endPause();

void ttr_show();

void ttr_showMonth();

TTRTimeTracking ttr_internal_parseLine(char *line);

TTRTimeTracking ttr_internal_parseLastRecordEntry(const char *fileName);

void ttr_internal_printCompleteTracking(TTRTimeTracking timeTracking);

void ttr_internal_printPauseTime(TTRTimeTracking timeTracking);

void ttr_internal_printStartTime(TTRTimeTracking timeTracking);

struct tm ttr_internal_convertSecondsToTM(long seconds);

#endif // TIME_TRACKING_H