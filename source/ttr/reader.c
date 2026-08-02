#include "reader.h"

#include <stdio.h>

#include <base_file.h>

/*
    Things we could do here:
    - [ ] properly validate header options
    - [ ] properly support versioning
    - [ ] better error handling
    - [ ] extract row info using string array and delimiter
*/

TTRHeader ttr_read_header(String fileContent)
{
    TTRHeader result = { 0 };

    String remaining = fileContent;

    remaining = string_advance(remaining, 1); // skip <
    while (remaining.length > 0 && remaining.content[0] != '>')
    {
        S64 identifierEnd = string_index_of_u8(remaining, '=');
        String identifierString = string_sub_string(remaining, 0, identifierEnd);
        remaining = string_advance(remaining, identifierEnd + 1);

        S64 valueEnd = string_index_of_u8(remaining, ';');
        String valueString = string_sub_string(remaining, 0, valueEnd);

        if (string_equals(str_lit("version"), identifierString))
        {
            result.version = string_to_u16(valueString);
            remaining = string_advance(remaining, valueEnd);
        }
        else if (string_equals(str_lit("entries"), identifierString))
        {
            result.entryCount = string_to_u64(valueString);
            remaining = string_advance(remaining, valueEnd);
        }
        else
        {
            // TODO(speciial): unsupported flag in header
            fprintf(stdout, "[TTR_READER]: Unsupported header option \"%.*s\".\n", (S32)identifierString.length, identifierString.content);

            remaining = string_advance(remaining, valueEnd);
        }

        if (remaining.content[0] == ';')
        {
            remaining = string_advance(remaining, 1);
        }
    }

    return result;
}

TTR *ttr_read_entries(Arena *arena, TTRHeader header, String fileContent, U64 additionalCapacity)
{
    TTR *result = push_struct(arena, TTR);
    result->header = header;
    result->capacity = header.entryCount + additionalCapacity;
    result->count = 0;
    result->records = push_array(arena, TTRRecord, result->capacity);
    result->currentlyActiveIndex = -1;

    if (result->header.entryCount != 0)
    {
        String remaining = fileContent;
        S64 firstNewLine = string_index_of_u8(remaining, '\n');
        remaining = string_advance(remaining, firstNewLine + 1);

        while (remaining.length > 0)
        {
            S64 colonIndex = string_index_of_u8(remaining, ':');
            String recordIndexString = string_sub_string(remaining, 0, colonIndex);
            remaining = string_advance(remaining, colonIndex + 1);

            U64 recordIndex = string_to_u64(recordIndexString);
            if (result->count == recordIndex)
            {
                S64 commaIndex = string_index_of_u8(remaining, ',');
                String startString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                commaIndex = string_index_of_u8(remaining, ',');
                String endString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                commaIndex = string_index_of_u8(remaining, ',');
                String lastPauseString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                commaIndex = string_index_of_u8(remaining, ',');
                String pauseTimeString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                commaIndex = string_index_of_u8(remaining, ',');
                String stateString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                commaIndex = string_index_of_u8(remaining, ',');
                String workLocationString = string_sub_string(remaining, 0, commaIndex);
                remaining = string_advance(remaining, commaIndex + 1);

                S64 endOfLineIndex = string_index_of_u8(remaining, ';');
                String messageString = string_sub_string(remaining, 0, endOfLineIndex);
                remaining = string_advance(remaining, endOfLineIndex + 1);

                TTRRecord *current = &result->records[recordIndex];

                current->timer.start = string_to_u64(startString);
                current->timer.end = string_to_u64(endString);
                current->timer.lastPause = string_to_u64(lastPauseString);
                current->timer.totalPauseTimeSeconds = (S64)string_to_u64(pauseTimeString);
                current->timer.state = timer_state_from_string(stateString);
                current->location = work_location_from_string(workLocationString);
                current->day = datetime_from_timestamp(current->timer.start);
                current->message = string_sub_string(messageString, 1, messageString.length - 1);

                if (current->timer.state == TIMER_STATE_STARTED || current->timer.state == TIMER_STATE_PAUSED)
                {
                    if (result->currentlyActiveIndex == -1)
                    {
                        result->currentlyActiveIndex = recordIndex;
                    }
                    else
                    {
                        // TODO(speciial): file corrupted
                        printf("ERROR: Multiple active timers in record file.\n");
                    }
                }

                result->count++;
            }
            else
            {
                // TODO(speciial): file corrupted?
                printf("ERROR: Multiple or skipped indices in record file.\n");
            }

            remaining = string_skip_white_spaces(remaining);
        }
    }

    // TODO(speciial): check if header.entryCount == ttr.count!

    return result;
}

TimerState timer_state_from_string(String stateString)
{
    TimerState result = TIMER_STATE_UNINITIALIZED;
    if (string_equals(stateString, str_lit("TIMER_STATE_STARTED")))
    {
        result = TIMER_STATE_STARTED;
    }
    else if (string_equals(stateString, str_lit("TIMER_STATE_ENDED")))
    {
        result = TIMER_STATE_ENDED;
    }
    else if (string_equals(stateString, str_lit("TIMER_STATE_PAUSED")))
    {
        result = TIMER_STATE_PAUSED;
    }
    return result;
}

TTRWorkLocation work_location_from_string(String workLocationString)
{
    TTRWorkLocation result = TTR_WORK_LOCATION_REMOTE;
    if (string_equals(str_lit("TTR_WORK_LOCATION_REMOTE"), workLocationString))
    {
        result = TTR_WORK_LOCATION_REMOTE;
    }
    else if (string_equals(str_lit("TTR_WORK_LOCATION_OFFICE"), workLocationString))
    {
        result = TTR_WORK_LOCATION_OFFICE;
    }
    else if (string_equals(str_lit("TTR_WORK_LOCATION_TRAVEL"), workLocationString))
    {
        result = TTR_WORK_LOCATION_TRAVEL;
    }
    return result;
}