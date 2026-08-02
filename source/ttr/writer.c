#include "writer.h"

#include <stdio.h>
#include <string.h>

#include <base_file.h>
#include <base_string.h>

B32 ttr_write_entries(Arena *arena, TTR *ttr, String filePath)
{
    B32 result = 0;
    String entriesToWrite = ttr_convert_entries_to_string(arena, ttr);
    if (entriesToWrite.length > 0)
    {
        result = file_write(filePath, entriesToWrite.content, entriesToWrite.length);
    }
    return result;
}

String ttr_convert_entries_to_string(Arena *arena, TTR *ttr)
{
    String result = { 0 };
    StringList stringList = { 0 };

    B32 failedToConvert = 0;

    String headerString = string_alloc(arena, 256);
    S32 printedHeaderStringLength = snprintf(headerString.content, headerString.length,
                                             "<version=%d;entries=%lld;>\n",
                                             ttr->header.version, ttr->count);
    if (printedHeaderStringLength > 0)
    {
        headerString.length = printedHeaderStringLength;
    }
    else
    {
        fprintf(stdout, "[TTR_WRITER]: Failed to write header content.\n");
        failedToConvert = 1;
    }

    string_list_append(arena, &stringList, headerString);

    for (U64 recordIndex = 0; recordIndex < ttr->count; ++recordIndex)
    {
        String *message = &ttr->records[recordIndex].message;
        Timer *timer = &ttr->records[recordIndex].timer;
        TTRWorkLocation workLocation = ttr->records[recordIndex].location;

        // TODO(speciial): move this to timer? 
        String timerStateString = { 0 };
        if (timer->state == TIMER_STATE_STARTED)
        {
            timerStateString = str_lit("TIMER_STATE_STARTED");
        }
        else if (timer->state == TIMER_STATE_ENDED)
        {
            timerStateString = str_lit("TIMER_STATE_ENDED");
        }
        else if (timer->state == TIMER_STATE_PAUSED)
        {
            timerStateString = str_lit("TIMER_STATE_PAUSED");
        }
        else
        {
            fprintf(stdout, "[TTR_WRITER]: Failed to write record with index %lld. Record state was UNINITIALIZED.\n", recordIndex);
            failedToConvert = 1;
            break;
        }

        String workLocationString = { 0 };
        if (workLocation == TTR_WORK_LOCATION_REMOTE)
        {
            workLocationString = str_lit("TTR_WORK_LOCATION_REMOTE");
        }
        else if (workLocation == TTR_WORK_LOCATION_OFFICE)
        {
            workLocationString = str_lit("TTR_WORK_LOCATION_OFFICE");
        }
        else if (workLocation == TTR_WORK_LOCATION_TRAVEL)
        {
            workLocationString = str_lit("TTR_WORK_LOCATION_TRAVEL");
        }
        else
        {
            fprintf(stdout, "[TTR_WRITER]: Failed to write record with index %lld. Record work location was unkown value.\n", recordIndex);
            failedToConvert = 1;
            break;
        }

        String recordString = string_alloc(arena, 256 + message->length);
        S32 printedRecordStringLength = snprintf(recordString.content, recordString.length,
                                                 "%lld:%lld,%lld,%lld,%lld,%.*s,%.*s,\"%.*s\";\n",
                                                 recordIndex, timer->start, timer->end, timer->lastPause, timer->totalPauseTimeSeconds,
                                                 (S32)timerStateString.length, timerStateString.content,
                                                 (S32)workLocationString.length, workLocationString.content,
                                                 (S32)message->length, message->content);
        if (printedRecordStringLength > 0)
        {
            recordString.length = printedRecordStringLength;
        }
        else
        {
            fprintf(stdout, "[TTR_WRITER]: Failed to write record with index %lld. Formatting failed.\n", recordIndex);
            failedToConvert = 1;
            break;
        }

        string_list_append(arena, &stringList, recordString);
    }

    if (!failedToConvert)
    {
        // TODO(speciial): we don't need to keep the contents of the list once 
        //                 we flattened it. aka. use arena properly!
        result = string_list_flatten(arena, &stringList);
    }

    return result;
}
