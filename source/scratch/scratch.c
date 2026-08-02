#include <stdio.h>
#include <stdlib.h>

#include <base_arena.h>
#include <base_datetime.h>

#include <ttr.h>
#include <ttr.h>

void create_test_data(Arena *arena)
{
    TTR *ttr = ttr_init_empty(arena, 100);

    Timestamp now = get_current_timestamp();
    S32 dayOffset = 0;
    while (ttr->count < 100)
    {
        Timestamp baseTimestamp = now - DAYS(150 - dayOffset);
        ++dayOffset;

        if (!is_weekday(baseTimestamp))
        {
            continue;
        }

        DateTime startDt = datetime_from_timestamp(baseTimestamp);
        startDt.hour = 8;
        startDt.minute = 15;
        ttr_start(ttr, timestamp_from_datetime(startDt), TTR_WORK_LOCATION_REMOTE);

        if ((dayOffset % 3) == 0)
        {
            DateTime pauseDt = datetime_from_timestamp(baseTimestamp);
            pauseDt.hour = 11;
            pauseDt.minute = 30;
            ttr_pause(ttr, timestamp_from_datetime(pauseDt));

            DateTime unpauseDt = datetime_from_timestamp(baseTimestamp);
            unpauseDt.hour = 12;
            unpauseDt.minute = 15;
            ttr_unpause(ttr, timestamp_from_datetime(unpauseDt));
        }

        DateTime endDt = datetime_from_timestamp(baseTimestamp);
        endDt.hour = 16;
        endDt.minute = 30;
        ttr_end(ttr, timestamp_from_datetime(endDt));

        if ((dayOffset % 2) == 0)
        {
            // TODO(speciial): comment!
        }
    }

    if (ttr->count == 100)
    {
        ttr_save_to_file(arena, ttr, str_lit("ttr_test_data.records"));
    }
    else
    {
        fprintf(stderr, "Something went wrong!\n");
    }

}

void print_record_data(Arena *arena)
{
    TTR *ttr = ttr_init_from_file(arena, str_lit("ttr_test_data.records"), 0);

    String status = ttr_status(arena, ttr, 1770275731);
    fprintf(stdout, "%.*s\n", (S32)status.length, status.content);

    String month = ttr_log(arena, ttr, 2025, 12);
    fprintf(stdout, "%.*s\n", (S32)month.length, month.content);
}

int main(int argc, char **argv)
{
    Arena arena = arena_create(MB(20));
    create_test_data(&arena);

    print_record_data(&arena);

    return 0;
}