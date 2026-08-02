#ifndef ttr_h
#define ttr_h

#include <base_core.h>
#include <base_arena.h>
#include <base_string.h>
#include <base_datetime.h>

#include "timer.h" 

/*
    Things I still want to do:
    - [ ] Options for printing more information in status and log (net, pause, total, overtimer, etc.)
    - [ ] Better error codes to show what went wrong!
*/

typedef enum TTRReturnCode TTRReturnCode;
enum TTRReturnCode
{
    TTR_SUCCESS = 0,
    TTR_ERROR
};

typedef enum TTRWorkLocation TTRWorkLocation;
enum TTRWorkLocation
{
    TTR_WORK_LOCATION_REMOTE = 0,
    TTR_WORK_LOCATION_OFFICE,
    TTR_WORK_LOCATION_TRAVEL,
};

typedef struct TTRHeader TTRHeader;
struct TTRHeader
{
    U16 version;
    U64 entryCount;
};

typedef struct TTRRecord TTRRecord;
struct TTRRecord
{
    Timer timer;
    DateTime day;
    String message;
    TTRWorkLocation location;
};

typedef struct TTR TTR;
struct TTR
{
    TTRHeader header;

    TTRRecord *records;
    U64 capacity;
    U64 count;
    S64 currentlyActiveIndex;
};

// TODO(speciial): i'm not sure if the api for accessing records is 
//                 what i actually need for all the upcoming features.
//                 perhaps something that queries for a record and 
//                 returns an index to the record is more useful to me.

TTR *ttr_init_empty(Arena *arena, U64 capacity);
TTR *ttr_init_from_file(Arena *arena, String recordFile, U64 additionalCapacity);
TTRReturnCode ttr_save_to_file(Arena *arena, TTR *ttr, String recordFile);

TTRRecord *ttr_get_active(TTR *ttr);

// TODO(speciial): rename to ttr_get_by_day and add ttr_get_by_timestamp
TTRRecord *ttr_get_day(TTR *ttr, DateTime dateTime);
B32 ttr_has_active(TTR *ttr);
B32 ttr_has_day(TTR *ttr, DateTime dateTime);

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp, TTRWorkLocation workLocation);
TTRReturnCode ttr_end(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_pause(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_unpause(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_comment(TTR *ttr, Timestamp timestamp, String comment);

String ttr_status(Arena *arena, TTR *ttr, Timestamp timestamp);
String ttr_log(Arena *arena, TTR *ttr, U16 year, U16 month);

String ttr_format_timer_string(Arena *arena, Timer timer);

#endif // ttr_h