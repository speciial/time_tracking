#ifndef time_tracking_H
#define time_tracking_H

#include <stdbool.h>

#include <arena.h>

enum { MAX_PAUSE_COUNT = 5 };
enum { DAYS_PER_MONTH = 32 };

typedef struct _ttr_day
{
    int Year;
    int Month;
    int Day;
} ttr_day;

typedef struct _ttr_interval
{
    unsigned int Start;
    unsigned int End;
} ttr_interval;

typedef struct _ttr_record
{
    bool Used;
    ttr_interval Work;

    unsigned int PauseCount;
    ttr_interval Pauses[MAX_PAUSE_COUNT];
} ttr_record;

typedef struct _ttr_record_list
{
    unsigned int Year;
    unsigned int Month;

    unsigned int RecordCountPerMonth;
    ttr_record Records[DAYS_PER_MONTH];

    struct _ttr_record_list *NextMonth;
} ttr_record_list;

ttr_record_list *ReadRecordFile(arena *Arena, const char *Filename);

void WriteRecordFile(arena *Arena, const char *Filename, ttr_record_list *Records);

#endif // time_tracking_H