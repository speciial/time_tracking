#include "ttr.h"

#include <assert.h>
#include <stdio.h>
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
    result->currentlyActiveIndex = -1;

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
            result = ttr_read_entries(arena, header, fileContent, additionalCapacity);
        }
    }
    return result;
}

TTRReturnCode ttr_save_to_file(Arena *arena, TTR *ttr, String recordFile)
{
    TTRReturnCode result = TTR_ERROR;
    if (ttr_write_entries(arena, ttr, recordFile))
    {
        result = TTR_SUCCESS;
    }
    return result;
}

TTRRecord *ttr_get_active(TTR *ttr)
{
    assert(ttr->count < ttr->capacity);

    TTRRecord *result = 0;
    if (ttr_has_active(ttr))
    {
        result = &ttr->records[ttr->currentlyActiveIndex];
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
    B32 result = (ttr->currentlyActiveIndex >= 0)
        && (ttr->records[ttr->currentlyActiveIndex].timer.state != TIMER_STATE_UNINITIALIZED)
        && (ttr->records[ttr->currentlyActiveIndex].timer.state != TIMER_STATE_ENDED);
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

TTRReturnCode ttr_start(TTR *ttr, Timestamp timestamp, TTRWorkLocation workLocation)
{
    assert(ttr->count < ttr->capacity);

    TTRReturnCode result = TTR_ERROR;

    // TODO(speciial): technically, it is possible to insert invalid days (e.g. weekends)
    //                 but this should be good enough for what i need right now.
    Timestamp start = (timestamp == 0) ? get_current_timestamp() : timestamp;
    DateTime startDt = datetime_from_timestamp(start);

    // TODO(speciial): restart day that has ended?
    if (!ttr_has_active(ttr) && !ttr_has_day(ttr, startDt))
    {
        TTRRecord *record = &ttr->records[ttr->count];
        if (timer_start(&record->timer, start) == TIMER_RESULT_SUCCESS)
        {
            ttr->currentlyActiveIndex = ttr->count;
            ttr->count++;
            record->day = startDt;
            record->location = workLocation;
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
            ttr->currentlyActiveIndex = -1;
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

    TTRRecord *record = ttr_get_day(ttr, datetime_from_timestamp(commentTime));
    if (record != 0)
    {
        record->message = comment;
        result = TTR_SUCCESS;
    }
    return result;
}

String ttr_status(Arena *arena, TTR *ttr, Timestamp timestamp)
{
    String result = { 0 };
    Timestamp statusTime = (timestamp == 0) ? get_current_timestamp() : timestamp;
    DateTime statusDt = datetime_from_timestamp(statusTime);

    TTRRecord *record = ttr_get_day(ttr, statusDt);
    if (record)
    {
        result = ttr_format_timer_string(arena, record->timer);
    }
    else
    {
        // TODO(speciial): alloc reponse string?
        // result = str_lit("No record found for given date.");
    }
    return result;
}

String ttr_log(Arena *arena, TTR *ttr, U16 year, U16 month)
{
    String result = { 0 };

    if (ttr->count > 0)
    {
        StringList stringList = { 0 };
        TTRRecord *current;

        DateTime currentDt = get_current_datetime();
        year = (year == 0) ? currentDt.year : year;
        month = (month == 0) ? currentDt.month : month;

        for (U64 recordIndex = 0; recordIndex < ttr->count; recordIndex++)
        {
            current = &ttr->records[recordIndex];
            if (current->day.year == year && current->day.month == month)
            {
                String timerString = ttr_format_timer_string(arena, current->timer);
                string_list_append(arena, &stringList, timerString);
            }
        }

        result = string_list_flatten(arena, &stringList);
    }
    return result;
}

String ttr_format_timer_string(Arena *arena, Timer timer)
{
    // TODO(speciial): this implementation for the formatting will probably replaced 
    // later on since it doesn't cover all the features I'd like to provide with this 
    // tool.

    String result = string_alloc(arena, 128);
    S32 printedLength = 0;

    DateTime startDt = datetime_from_timestamp(timer.start);
    if (timer.state == TIMER_STATE_STARTED)
    {
        // [STARTED] 19.05., 08:30, NET: 3.45h, PAUSE: 0.75h
        F32 netWorkTimeHours = (F32)timer_total_active_time_seconds(&timer) / (60.0f * 60.0f);
        F32 pauseTimeHours = (F32)timer.totalPauseTimeSeconds / (60.0f * 60.0f);

        printedLength = snprintf(result.content, result.length,
                                 "[STARTED] %02d.%02d., %02d:%02d, NET: %.2fh, PAUSE: %.2fh\n",
                                 startDt.day, startDt.month, startDt.hour, startDt.minute,
                                 netWorkTimeHours, pauseTimeHours);
    }
    else if (timer.state == TIMER_STATE_PAUSED)
    {
        // [PAUSED] 19.05., 08:30, NET: 4.56h, PAUSE: 1.3h
        F32 netWorkTimeHours = (F32)timer_total_active_time_seconds(&timer) / (60.0f * 60.0f);
        F32 pauseTimeHours = (F32)timer.totalPauseTimeSeconds / (60.0f * 60.0f);

        printedLength = snprintf(result.content, result.length,
                                 "[PAUSED] %02d.%02d., %02d:%02d, NET: %.2fh, PAUSE: %.2fh\n",
                                 startDt.day, startDt.month, startDt.hour, startDt.minute,
                                 netWorkTimeHours, pauseTimeHours);
    }
    else if (timer.state == TIMER_STATE_ENDED)
    {
        // [ENDED] 19.05., 08:30 - 16:30, NET: 7.5h, PAUSE: 0.75h, TOTAL: 8.25h
        DateTime endDt = datetime_from_timestamp(timer.end);
        F32 netWorkTimeHours = (F32)timer_total_active_time_seconds(&timer) / (60.0f * 60.0f);
        F32 pauseTimeHours = (F32)timer.totalPauseTimeSeconds / (60.0f * 60.0f);

        printedLength = snprintf(result.content, result.length,
                                 "[ENDED] %02d.%02d., %02d:%02d - %02d:%02d, NET: %.2fh, PAUSE: %.2fh, TOTAL: %.2fh\n",
                                 startDt.day, startDt.month,
                                 startDt.hour, startDt.minute, endDt.hour, endDt.minute,
                                 netWorkTimeHours, pauseTimeHours, (netWorkTimeHours + pauseTimeHours));
    }
    else
    {
        // TODO(speciial): free alloced string?
        S32 printedLength = 0;
    }

    if (printedLength > 0)
    {
        result.length = printedLength;
    }

    return result;
}
