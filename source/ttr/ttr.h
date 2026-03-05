#ifndef ttr_h
#define ttr_h

#include <base_core.h>
#include <base_arena.h>
#include <base_string.h>

#define TTR_MAX_INTERVAL 4

typedef enum TTRReturnCode TTRReturnCode;
enum TTRReturnCode
{
    TTR_SUCCESS = 0,
    TTR_ERROR
};

typedef enum TTRRecordState TTRRecordState;
enum TTRRecordState
{
    TTR_RECORD_UNINITIALIZED = 0,
    TTR_RECORD_STARTED,
    TTR_RECORD_PAUSED,
    TTR_RECORD_ENDED
};

typedef struct TTRInterval TTRInterval;
struct TTRInterval
{
    Timestamp start;
    Timestamp end;
};

typedef struct TTRRecord TTRRecord;
struct TTRRecord
{
    TTRInterval interval[TTR_MAX_INTERVAL];
    U16 intervalCount;
    TTRRecordState state;
};

typedef struct TTR TTR;
struct TTR
{
    TTRRecord *records;
    U64 capacity;
    U64 count;
    S64 activeIndex;
};

TTR *ttr_init(Arena *arena, String recordFile);
TTRRecord *ttr_get_current(TTR *ttr);

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_end(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_pause(TTR *ttr, Timestamp timestamp);
TTRReturnCode ttr_unpause(TTR *ttr, Timestamp timestamp);

#endif // ttr_h