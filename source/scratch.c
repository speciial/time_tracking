#include <stdio.h>
#include <stdlib.h>

#include <base_arena.h>
#include <base_datetime.h>

#include <ttr.h>
#include <ttr.h>

void create_test_data(Arena *arena)
{
    TTR *ttr = ttr_init_empty(&arena, 100);

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
        ttr_start(ttr, timestamp_from_datetime(startDt));

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
        ttr_save_to_file(&arena, ttr, str_lit("ttr_test_data.records"));
    }
    else
    {
        fprintf(stderr, "Something went wrong!\n");
    }

}

void print_record_data(Arena *arena)
{
    TTR *ttr = ttr_init_from_file(arena, str_lit("ttr_test_data.records"), 0);

    for (S32 recordIndex = 0; recordIndex < ttr->count; ++recordIndex)
    {
        TTRRecord *record = &ttr->records[recordIndex];

        DateTime startDt = datetime_from_timestamp(record->timer.start);
        DateTime endDt = datetime_from_timestamp(record->timer.end);

        S64 netWorkTimeInSeconds = timer_total_active_time_seconds(&record->timer);
        F32 netWorkTime = (F32)netWorkTimeInSeconds / (F32)(60 * 60);

        fprintf(stdout, "%02d.%02d.%d: %02d:%02d - %02d:%02d, %.2f\n",
                record->day.day, record->day.month, record->day.year,
                startDt.hour, startDt.minute, endDt.hour, endDt.minute,
                netWorkTime);
    }
}

int main(int argc, char **argv)
{
    Arena arena = arena_create(MB(20));
    create_test_data(&arena);

    print_record_data(&arena);

    return 0;
}