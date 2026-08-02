#include <stdio.h>

#include <base_arena.h>
#include <base_string.h>

#include <ttr.h>
#include <writer.h>

#include <assert.h>

void test_write_empty(Arena *arena)
{
    TTR ttr = { 0 };
    ttr.header.version = 2;
    ttr.header.entryCount = 0;

    String writeResult = ttr_convert_entries_to_string(arena, &ttr);

    assert(string_equals(str_lit("<version=2;entries=0;>\n"), writeResult));
}

void test_write_single_row(Arena *arena)
{
    TTRRecord records[1] =
    {
        {
            .day = datetime_from_timestamp(1773058784),
            .message = str_lit("Some Message"),
            .timer =
            {
                .start = 1773058784,
                .end = 1773058784,
                .lastPause = 0,
                .totalPauseTimeSeconds = 0,
                .state = TIMER_STATE_ENDED
            },
            .location = TTR_WORK_LOCATION_REMOTE
        }
    };

    TTR ttr = { 0 };
    ttr.header.version = 2;
    ttr.header.entryCount = 1;
    ttr.capacity = 1;
    ttr.count = 1;
    ttr.records = records;

    String writeResult = ttr_convert_entries_to_string(arena, &ttr);

    assert(string_equals(str_lit("<version=2;entries=1;>\n0:1773058784,1773058784,0,0,TIMER_STATE_ENDED,TTR_WORK_LOCATION_REMOTE,\"Some Message\";\n"), writeResult));
}

int test_writer(int argc, char **argv)
{
    printf("Test Writer\n");

    Arena arena = arena_create(MB(20));

    // TODO(speciial): implement more tests

    test_write_empty(&arena);
    test_write_single_row(&arena);

    return 0;
}