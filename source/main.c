#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <base_core.h>
#include <base_arena.h>
#include <base_string.h>

#include <ttr.h>

// TODO(speciial): move this to base_string.h
typedef struct StringArray StringArray;
struct StringArray
{
    String *strings;
    U64 size;
    U64 count;
};

typedef struct CommandArgs CommandArgs;
struct CommandArgs
{
    Timestamp timestamp;
    String timeFormat;
    String comment;
    TTRWorkLocation workLocation;
    U16 year;
    U16 month;

    B32 parseFailed;
};

StringArray convert_command_line_args(Arena *arena, int argc, char **argv)
{
    StringArray commandLineArgs = { 0 };
    commandLineArgs.strings = push_array(arena, String, argc);
    commandLineArgs.size = argc - 1;
    commandLineArgs.count = argc - 1;

    // TODO(speciial): I don't like how specific the index has to be set here!
    for (U64 argIndex = 1; argIndex < argc; argIndex++)
    {
        String arg = { .content = (U8 *)argv[argIndex], .length = strlen(argv[argIndex]) };
        commandLineArgs.strings[argIndex - 1] = arg;
    }

    return commandLineArgs;
}

B32 is_time_format_delimiter(U8 value)
{
    B32 result = ((value == '.') || (value == ':'));
    return result;
}

Timestamp parse_timestamp_string(String timestampString, String timeFormat)
{
    Timestamp result = 0;
    // yy = year, MM = month, dd = day, hh = hour, mm = minute

    B32 failedToParse = 0;
    DateTime dt = get_current_datetime();

    for (U64 formatIndex = 0; formatIndex + 1 < timeFormat.length; formatIndex += 2)
    {
        String currentIdentifier = string_sub_string(timeFormat, formatIndex, formatIndex + 2);
        String numberString = string_sub_string(timestampString, formatIndex, formatIndex + 2);
        if (numberString.length > 0 && numberString.content[0] == '0')
        {
            numberString = string_advance(numberString, 1);
        }

        if (string_equals(str_lit("yy"), currentIdentifier))
        {
            dt.year = 2000 + string_to_u16(numberString);
        }
        else if (string_equals(str_lit("MM"), currentIdentifier))
        {
            dt.month = string_to_u16(numberString);
        }
        else if (string_equals(str_lit("dd"), currentIdentifier))
        {
            dt.day = string_to_u16(numberString);
        }
        else if (string_equals(str_lit("hh"), currentIdentifier))
        {
            dt.hour = string_to_u16(numberString);
        }
        else if (string_equals(str_lit("mm"), currentIdentifier))
        {
            dt.minute = string_to_u16(numberString);
        }
        else
        {
            fprintf(stderr, "Failed to parse time format %.*s\n", (S32)timeFormat.length, timeFormat.content);
            failedToParse = 1;
            break;
        }

        if (formatIndex + 2 < timeFormat.length && is_time_format_delimiter(timeFormat.content[formatIndex + 2]))
        {
            formatIndex++;
        }
    }

    if (!failedToParse)
    {
        result = timestamp_from_datetime(dt);
    }

    return result;
}

CommandArgs parse_command_args(StringArray args)
{
    // TODO(speciial): check if arg is supported for command
    // TODO(speciial): check if arg is required for command
    // TODO(speciial): reduce code duplication
    CommandArgs result = {
        .timestamp = 0,
        .timeFormat = str_lit("hh:mm"), // TODO(speciial): this should probably be alloced
        .comment = { 0 },
        .workLocation = TTR_WORK_LOCATION_REMOTE,
        .year = 0,
        .month = 0,
        .parseFailed = 0
    };

    String timestampString = { 0 };

    for (U64 argIndex = 1; argIndex < args.count; argIndex++)
    {
        String arg = args.strings[argIndex];

        if (string_equals(str_lit("-t"), arg) || string_equals(str_lit("--time"), arg))
        {
            if (argIndex < (args.count - 1))
            {
                timestampString = args.strings[argIndex + 1];
            }
            else
            {
                fprintf(stderr, "Argument time is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
        if (string_equals(str_lit("-f"), arg) || string_equals(str_lit("--format"), arg))
        {
            if (argIndex < (args.count - 1))
            {
                result.timeFormat = args.strings[argIndex + 1];
            }
            else
            {
                fprintf(stderr, "Argument format is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
        if (string_equals(str_lit("-c"), arg) || string_equals(str_lit("--comment"), arg))
        {
            if (argIndex < (args.count - 1))
            {
                result.comment = args.strings[argIndex + 1];
            }
            else
            {
                fprintf(stderr, "Argument comment is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
        if (string_equals(str_lit("-m"), arg) || string_equals(str_lit("--month"), arg))
        {
            if (argIndex < (args.count - 1))
            {
                result.month = string_to_u16(args.strings[argIndex + 1]);
            }
            else
            {
                fprintf(stderr, "Argument month is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
        if (string_equals(str_lit("-y"), arg) || string_equals(str_lit("--year"), arg))
        {
            if (argIndex < (args.count - 1))
            {
                result.year = string_to_u16(args.strings[argIndex + 1]);
            }
            else
            {
                fprintf(stderr, "Argument year is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
        if (string_equals(str_lit("-l"), arg) || string_equals(str_lit("-location"), arg))
        {
            // TODO(speciial): this should be required
            if (argIndex < (args.count - 1))
            {
                String workLocation = args.strings[argIndex + 1];
                if (string_equals(str_lit("remote"), workLocation))
                {
                    result.workLocation = TTR_WORK_LOCATION_REMOTE;
                }
                else if (string_equals(str_lit("office"), workLocation))
                {
                    result.workLocation = TTR_WORK_LOCATION_OFFICE;
                }
                else if (string_equals(str_lit("travel"), workLocation))
                {
                    result.workLocation = TTR_WORK_LOCATION_TRAVEL;
                }
                else
                {
                    printf("Unkown work location option %.*s", (S32)workLocation.length, workLocation.content);
                    result.parseFailed = 1;
                    break;
                }
            }
            else
            {
                fprintf(stderr, "Argument location is missing the value\n");
                result.parseFailed = 1;
                break;
            }
        }
    }

    if (!result.parseFailed && timestampString.length > 0)
    {
        result.timestamp = parse_timestamp_string(timestampString, result.timeFormat);
    }

    return result;
}

int main(int argc, char **argv)
{
    Arena ttrArena = arena_create(MB(20));
    Arena stringArena = arena_create(MB(10));

    if (argc > 1)
    {
        StringArray commandLineArgs = convert_command_line_args(&stringArena, argc, argv);
        String command = commandLineArgs.strings[0];
        CommandArgs commandArgs = parse_command_args(commandLineArgs);

        if (!commandArgs.parseFailed)
        {
            TTR *ttr = ttr_init_from_file(&ttrArena, str_lit("ttr.records"), 16);

            if (string_equals(str_lit("start"), command))
            {
                TTRReturnCode returnCode = ttr_start(ttr, commandArgs.timestamp, commandArgs.workLocation);
                if (returnCode == TTR_SUCCESS)
                {
                    // TODO(speciial): I really don't like the api in this if case!
                    ttr_save_to_file(&stringArena, ttr, str_lit("ttr.records"));

                    // TODO(speciial): this is duplicated from ttr_start
                    Timestamp start = (commandArgs.timestamp == 0) ? get_current_timestamp() : commandArgs.timestamp;
                    TTRRecord *current = ttr_get_day(ttr, datetime_from_timestamp(start));
                    DateTime currentDt = datetime_from_timestamp(current->timer.start);
                    fprintf(stdout, "Started time tracking at %02d.%02d. %02d:%02d\n",
                            currentDt.month, currentDt.day, currentDt.hour, currentDt.minute);
                }
                else
                {
                    fprintf(stderr, "Failed to start time tracking\n");
                }
            }
            else if (string_equals(str_lit("end"), command))
            {
                TTRReturnCode returnCode = ttr_end(ttr, commandArgs.timestamp);
                if (returnCode == TTR_SUCCESS)
                {
                    ttr_save_to_file(&stringArena, ttr, str_lit("ttr.records"));

                    // TODO(speciial): this is duplicated from ttr_end
                    Timestamp end = (commandArgs.timestamp == 0) ? get_current_timestamp() : commandArgs.timestamp;
                    TTRRecord *current = ttr_get_day(ttr, datetime_from_timestamp(end));
                    DateTime currentDt = datetime_from_timestamp(current->timer.end);
                    fprintf(stdout, "Ended time tracking at %02d.%02d. %02d:%02d\n",
                            currentDt.month, currentDt.day, currentDt.hour, currentDt.minute);
                }
                else
                {
                    fprintf(stderr, "Failed to end time tracking\n");
                }
            }
            else if (string_equals(str_lit("pause"), command))
            {
                TTRReturnCode returnCode = ttr_pause(ttr, commandArgs.timestamp);
                if (returnCode == TTR_SUCCESS)
                {
                    ttr_save_to_file(&stringArena, ttr, str_lit("ttr.records"));

                    // TODO(speciial): this is duplicated from ttr_pause
                    Timestamp pause = (commandArgs.timestamp == 0) ? get_current_timestamp() : commandArgs.timestamp;
                    TTRRecord *current = ttr_get_day(ttr, datetime_from_timestamp(pause));
                    DateTime currentDt = datetime_from_timestamp(current->timer.lastPause);
                    fprintf(stdout, "Paused time tracking at %02d.%02d. %02d:%02d\n",
                            currentDt.month, currentDt.day, currentDt.hour, currentDt.minute);
                }
                else
                {
                    fprintf(stderr, "Failed to pause time tracking\n");
                }
            }
            else if (string_equals(str_lit("unpause"), command))
            {
                TTRReturnCode returnCode = ttr_unpause(ttr, commandArgs.timestamp);
                if (returnCode == TTR_SUCCESS)
                {
                    ttr_save_to_file(&stringArena, ttr, str_lit("ttr.records"));

                    // TODO(speciial): this is duplicated from ttr_unpause
                    Timestamp unpause = (commandArgs.timestamp == 0) ? get_current_timestamp() : commandArgs.timestamp;
                    // TTRRecord *current = ttr_get_day(ttr, datetime_from_timestamp(unpause));
                    // TODO(speciial): this is broken. I can't cleanly determine the unpause time here. I also don't know,
                    //                 if I like this kind of print anyway.
                    DateTime currentDt = datetime_from_timestamp(unpause);
                    fprintf(stdout, "Unpaused time tracking at %02d.%02d. %02d:%02d\n",
                            currentDt.month, currentDt.day, currentDt.hour, currentDt.minute);
                }
                else
                {
                    fprintf(stderr, "Failed to unpause time tracking\n");
                }
            }
            else if (string_equals(str_lit("comment"), command))
            {
                TTRReturnCode returnCode = ttr_comment(ttr, commandArgs.timestamp, commandArgs.comment);
                if (returnCode == TTR_SUCCESS)
                {
                    ttr_save_to_file(&stringArena, ttr, str_lit("ttr.records"));

                    fprintf(stdout, "Comment added\n");
                }
                else
                {
                    fprintf(stderr, "Failed to add comment\n");
                }
            }
            else if (string_equals(str_lit("status"), command))
            {
                String statusResult = ttr_status(&stringArena, ttr, commandArgs.timestamp);
                if (statusResult.length > 0)
                {
                    fprintf(stdout, "%.*s\n", (S32)statusResult.length, statusResult.content);
                }
                else
                {
                    fprintf(stderr, "Failed to get time tracking status\n");
                }
            }
            else if (string_equals(str_lit("log"), command))
            {
                String logResult = ttr_log(&stringArena, ttr, commandArgs.year, commandArgs.month);
                if (logResult.length > 0)
                {
                    fprintf(stdout, "%.*s\n", (S32)logResult.length, logResult.content);
                }
                else
                {
                    fprintf(stderr, "Failed to get time tracking log\n");
                }
            }
            else
            {
                // TODO(speciial): unknown command, print help
                fprintf(stderr, "Unknown command: %.*s\n", (S32)command.length, command.content);
            }
        }
    }
    else
    {
        // TODO(speciial): print help
        fprintf(stderr, "No command was provided\n");
    }

    return 0;
}