#ifndef ttr_h
#define ttr_h

#include <base_core.h>
#include <base_arena.h>
#include <base_string.h>
#include <base_datetime.h>

#include "timer.h" 

typedef enum TTRReturnCode TTRReturnCode;
enum TTRReturnCode
{
    TTR_SUCCESS = 0,
    TTR_ERROR
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
};

typedef struct TTR TTR;
struct TTR
{
    TTRHeader header;

    TTRRecord *records;
    U64 capacity;
    U64 count;
};

TTR *ttr_init_empty(Arena *arena, U64 capacity);
TTR *ttr_init_from_file(Arena *arena, String recordFile, U64 additionalCapacity);

// TODO(speciial): return value? error handling?
void ttr_save_to_file(Arena *arena, TTR *ttr, String recordFile);

TTRRecord *ttr_get_active(TTR *ttr);
TTRRecord *ttr_get_day(TTR *ttr, DateTime dateTime);
B32 ttr_has_active(TTR *ttr);
B32 ttr_has_day(TTR *ttr, DateTime dateTime);

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_end(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_pause(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_unpause(TTR *ttr, Timestamp timestamp);

#endif // ttr_h