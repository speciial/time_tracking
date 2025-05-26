#ifndef time_tracking_H
#define time_tracking_H

#include <stdbool.h>

#include <arena.h>
#include <strings.h>

#include "tokenizer.h"

enum { MAX_PAUSE_COUNT = 4 };
enum { DAYS_PER_MONTH = 32 };

typedef struct _ttr_day
{
    int Year;
    int Month;
    int Day;
} ttr_day;

// TODO(speciial): Use timestamp types here!
typedef struct _ttr_interval
{
    unsigned int Start;
    unsigned int End;
    bool Incomplete;
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
    bool Invalid;
} ttr_record_list;


// ========================================
//  Interacting With Records
// ========================================

// TODO(speciial): StartNewRecord();
// TODO(speciial): StartNewRecordAt();
// TODO(speciial): EndCurrentRecord();
// TODO(speciial): EndCurrentRecordAt();
// TODO(speciial): PauseCurrentRecord();
// TODO(speciial): UnpauseCurrentRecord();

// TODO(speciial): AddRecord();
// TODO(speciial): EditRecord();
// TODO(speciial): RemoveRecord();

// TODO(speciial): RecordToString();
// TODO(speciial): RecordToMailSubject();
// TODO(speciial): RecordMonthToString();

// ========================================
//  Reading Record Files
// ========================================

ttr_record_list ReadRecordFile(arena *Arena, const char *Filename);

// ========================================
//  Reading Record Files Internal
// ========================================

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

// ========================================
//  Writing Record Files Internal
// ========================================

string WriteFormattedRecord(arena *Arena, ttr_record *Record, int Year, int Month, int Day);

string WriteFormattedInterval(arena *Arena, ttr_interval Interval);

#endif // time_tracking_H