#include "ttr.h"

#include <assert.h>
#include <string.h>

#include <base_datetime.h>
#include <base_file.h>

#include "reader.h"
#include "writer.h"

TTR *ttr_init_empty(Arena *arena, U64 capacity)
{
    TTR *result = push_struct(arena, TTR);
    TTRHeader header = { .version = 2, .entryCount = 0 };

    result->header = header;
    result->capacity = capacity;
    result->count = 0;
    result->records = push_array(arena, TTRRecord, result->capacity);

    return result;
}

TTR *ttr_init_from_file(Arena *arena, String recordFile, U64 additionalCapacity)
{
    TTR *result = 0;
    U64 fileSize = file_size(recordFile);
    if (fileSize > 0)
    {
        String fileContent = string_alloc(arena, fileSize);
        if (file_read(recordFile, fileContent.content, fileContent.length))
        {
            // TODO(speciial): move ttr_read_header into ttr_read_entries
            TTRHeader header = ttr_read_header(fileContent);
            result = ttr_read_entries(arena, header, fileContent);
        }
    }
    return result;
}

void ttr_save_to_file(Arena *arena, TTR *ttr, String recordFile)
{
    // TODO(speciial): implement this properly!
    ttr_write_entries(arena, ttr, recordFile);
}

TTRRecord *ttr_get_active(TTR *ttr)
{
    assert(ttr->count < ttr->capacity);

    TTRRecord *result = 0;
    // NOTE(speciial): there should never be a case where an active record
    //                 isn't the last in the list!
    if (ttr->records[ttr->count].timer.state != TIMER_STATE_UNINITIALIZED
        && ttr->records[ttr->count].timer.state != TIMER_STATE_ENDED)
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
    B32 result = (ttr->records[ttr->count].timer.state != TIMER_STATE_UNINITIALIZED
                  && ttr->records[ttr->count].timer.state != TIMER_STATE_ENDED);
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
        if (timer_start(&record->timer, start) == TIMER_RESULT_SUCCESS)
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
        if (timer_end(&record->timer, end) == TIMER_RESULT_SUCCESS)
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
        if (timer_pause(&record->timer, pause) == TIMER_RESULT_SUCCESS)
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
        if (timer_unpause(&record->timer, unpause) == TIMER_RESULT_SUCCESS)
        {
            result = TTR_SUCCESS;
        }

    }
    return result;
}

TTRReturnCode ttr_comment(TTR *ttr, Timestamp timestamp, String comment)
{
    TTRReturnCode result = TTR_ERROR;
    Timestamp commentTime = (timestamp == 0) ? get_current_timestamp() : timestamp;

    if (ttr_has_active(ttr))
    {
        TTRRecord *record = ttr_get_active(ttr);
        record->message = comment;
        result = TTR_SUCCESS;
    }

    return result;
}
