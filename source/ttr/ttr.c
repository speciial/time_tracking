#include "ttr.h"

#include <assert.h>
#include <string.h>

#include <base_datetime.h>

TTR *ttr_init(Arena *arena, String recordFile)
{
    // TODO(speciial): implement record file parsing

    TTR *result = push_struct(arena, TTR);
    result->capacity = 10;
    result->count = 0;
    result->activeIndex = -1;
    result->records = push_array(arena, TTRRecord, result->capacity);
    return result;
}

TTRRecord *ttr_get_current(TTR *ttr)
{
    TTRRecord *result = 0;
    if (ttr->activeIndex != -1)
    {
        result = &ttr->records[ttr->activeIndex];
    }
    else if (ttr->count > 0)
    {
        result = &ttr->records[ttr->count - 1];
    }
    return result;
}

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    // TODO(speciial): do we need to check if the timestamp makes sense?
    if (timestamp == 0)
    {
        timestamp = get_current_timestamp();
    }

    if (ttr->activeIndex == -1)
    {
        TTRRecord *record = &ttr->records[ttr->count];
        record->state = TTR_RECORD_STARTED;
        record->interval[0].start = timestamp;
        ttr->activeIndex = ttr->count;
        ttr->count++;
        result = TTR_SUCCESS;
    }
    return result;
}

TTRReturnCode ttr_end(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    if (timestamp == 0)
    {
        timestamp = get_current_timestamp();
    }

    if (ttr->activeIndex != -1)
    {
        TTRRecord *record = ttr_get_current(ttr);
        // TODO(speciial): allow paused recrods to be ended immediately
        if (record && record->state == TTR_RECORD_STARTED)
        {
            record->state = TTR_RECORD_ENDED;
            record->interval[record->intervalCount++].end = timestamp;
            ttr->activeIndex = -1;
            result = TTR_SUCCESS;
        }
    }
    return result;
}

TTRReturnCode ttr_pause(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    if (timestamp == 0)
    {
        timestamp = get_current_timestamp();
    }

    if (ttr->activeIndex != -1)
    {
        TTRRecord *record = ttr_get_current(ttr);
        if (record && record->state == TTR_RECORD_STARTED && record->intervalCount < TTR_MAX_INTERVAL)
        {
            record->state = TTR_RECORD_PAUSED;
            record->interval[record->intervalCount++].end = timestamp;
            result = TTR_SUCCESS;
        }
    }
    return result;
}

TTRReturnCode ttr_unpause(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    if (timestamp == 0)
    {
        timestamp = get_current_timestamp();
    }

    if (ttr->activeIndex != -1)
    {
        TTRRecord *record = ttr_get_current(ttr);
        if (record && record->state == TTR_RECORD_PAUSED)
        {
            record->state = TTR_RECORD_STARTED;
            record->interval[record->intervalCount].start = timestamp;
            result = TTR_SUCCESS;
        }
    }
    return result;
}
