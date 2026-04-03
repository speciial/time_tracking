#include "ttr.h"

#include <assert.h>
#include <string.h>

#include <base_datetime.h>
#include <base_file.h>

#include "reader.h"

TTR *ttr_init(Arena *arena, String recordFile)
{
    TTR *result = 0;

    U64 fileSize = file_size(recordFile);
    if (fileSize > 0)
    {
        String fileContent = string_alloc(arena, fileSize);
        if (file_read(recordFile, fileContent.content, fileContent.length))
        {
            TTRHeader header = ttr_read_header(fileContent);
            result = ttr_read_entries(arena, header, fileContent);
        }
    }

    return result;
}

TTRRecord *ttr_get_active(TTR *ttr)
{
    assert(ttr->count < ttr->capacity);

    TTRRecord *result = 0;
    // NOTE(speciial): there should never be a case where an active record
    //                 isn't the last in the list!
    if (ttr->records[ttr->count].timer.state != TIMER_UNINITIALIZED
        && ttr->records[ttr->count].timer.state != TIMER_ENDED)
    {
        result = &ttr->records[ttr->count];
    }
    return result;
}

TTRRecord *ttr_get_day(TTR *ttr, DateTime dateTime)
{
    assert(ttr->count < ttr->capacity);

    TTRRecord *result = 0;
    for (U64 recordIndex = 0; recordIndex <= ttr->count; ++recordIndex)
    {
        if (is_same_date(dateTime, ttr->records[recordIndex].day))
        {
            result = &ttr->records[recordIndex];
            break;
        }
    }
    return result;
}

B32 ttr_has_active(TTR *ttr)
{
    assert(ttr->count < ttr->capacity);
    B32 result = (ttr->records[ttr->count].timer.state != TIMER_UNINITIALIZED
                  && ttr->records[ttr->count].timer.state != TIMER_ENDED);
    return result;
}

B32 ttr_has_day(TTR *ttr, DateTime dateTime)
{
    assert(ttr->count < ttr->capacity);

    B32 result = 0;
    for (U64 recordIndex = 0; recordIndex <= ttr->count; ++recordIndex)
    {
        if (is_same_date(dateTime, ttr->records[recordIndex].day))
        {
            result = 1;
            break;
        }
    }
    return result;
}

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;

    // TODO(speciial): technically, it is possible to insert invalid days (e.g. weekends)
    //                 but this should be good enough for what i need right now.
    Timestamp start = (timestamp == 0) ? get_current_timestamp() : timestamp;
    DateTime startDt = datetime_from_timestamp(start);

    if (!ttr_has_active(ttr) && !ttr_has_day(ttr, startDt))
    {
        TTRRecord *record = &ttr->records[ttr->count];
        if (timer_start(&record->timer, start) == TIMER_SUCCESS)
        {
            record->day = startDt;
            result = TTR_SUCCESS;
        }
    }
    return result;
}

TTRReturnCode ttr_end(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    Timestamp end = (timestamp == 0) ? get_current_timestamp() : timestamp;

    if (ttr_has_active(ttr))
    {
        TTRRecord *record = ttr_get_active(ttr);
        if (timer_end(&record->timer, end) == TIMER_SUCCESS)
        {
            ttr->count++;
            result = TTR_SUCCESS;
        }
    }
    return result;
}

TTRReturnCode ttr_pause(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    Timestamp pause = (timestamp == 0) ? get_current_timestamp() : timestamp;

    if (ttr_has_active(ttr))
    {
        TTRRecord *record = ttr_get_active(ttr);
        if (timer_pause(&record->timer, pause) == TIMER_SUCCESS)
        {
            result = TTR_SUCCESS;
        }

    }
    return result;
}

TTRReturnCode ttr_unpause(TTR *ttr, Timestamp timestamp)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;
    Timestamp unpause = (timestamp == 0) ? get_current_timestamp() : timestamp;

    if (ttr_has_active(ttr))
    {
        TTRRecord *record = ttr_get_active(ttr);
        if (timer_unpause(&record->timer, unpause) == TIMER_SUCCESS)
        {
            result = TTR_SUCCESS;
        }

    }
    return result;
}
