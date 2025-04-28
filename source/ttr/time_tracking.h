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

typedef struct _ttr_record_list_node
{
    unsigned int Year;
    unsigned int Month;

    unsigned int RecordCountPerMonth;
    ttr_record Records[DAYS_PER_MONTH];

    struct _ttr_record_list_node *NextMonth;
} ttr_record_list_node;

typedef struct _ttr_record_list
{
    ttr_record_list_node *First;
    ttr_record_list_node *Last;
    int Count;
} ttr_record_list;

// ========================================
//  Reading Record Files
// ========================================

ttr_record_list ReadRecordFile(arena *Arena, const char *Filename);

void ParseRecords(arena *Arena, tokenizer *Tokenizer, ttr_record_list *RecordList);

void ParseWorkDay(tokenizer *Tokenizer, ttr_record_list_node *RecordListNode,
                  int Year, int Month, int Day);

void ParsePauses(tokenizer *Tokenizer, ttr_record *OutRecord,
                 int Year, int Month, int Day);

ttr_interval ParseInterval(tokenizer *Tokenizer, int Year, int Month, int Day);

ttr_day ParseDay(tokenizer *Tokenizer);

time_t ParseTime(tokenizer *Tokenizer, int Year, int Month, int Day);

void RecordListPush(arena *Arena, ttr_record_list *RecordList);

// ========================================
//  Writing Record Files
// ========================================

void WriteRecordFile(arena *Arena, const char *Filename, ttr_record_list Records);

string WriteFormattedRecord(arena *Arena, ttr_record *Record, int Year, int Month, int Day);

string WriteFormattedInterval(arena *Arena, ttr_interval Interval);

#endif // time_tracking_H