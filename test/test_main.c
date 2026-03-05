#include <stdio.h>
#include <assert.h>

#include <base_arena.h>

#include <ttr.h>

void test_init(Arena *arena)
{
    TTR *ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    assert(0 != ttr);

    // TODO(speciial): test init once file parsing is implemented

    fprintf(stdout, "[PASSED] test_init\n");
}

void test_start(Arena *arena)
{
    // test empty, no timestamp
    TTR *ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    TTRReturnCode result = ttr_start(ttr, 0);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_current(ttr);
    assert(0 != current);
    assert(TTR_RECORD_STARTED == current->state);
    assert(0 != current->interval[0].start);

    // test empty, with timestamp
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    result = ttr_start(ttr, 1234);
    assert(TTR_SUCCESS == result);

    current = ttr_get_current(ttr);
    assert(0 != current);
    assert(TTR_RECORD_STARTED == current->state);
    assert(1234 == current->interval[0].start);

    // test dirty, no timestamp
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    result = ttr_start(ttr, 0);
    assert(TTR_SUCCESS == result);

    result = ttr_start(ttr, 0);
    assert(TTR_ERROR == result);

    fprintf(stdout, "[PASSED] test_start\n");
}

void test_end(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    TTRReturnCode result = ttr_end(ttr, 0);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_current(ttr);
    assert(0 != current);
    assert(TTR_RECORD_ENDED == current->state);
    assert(0 != current->interval[0].start);
    assert(0 != current->interval[0].end);

    // test empty, not started
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    result = ttr_end(ttr, 0);
    assert(TTR_ERROR == result);

    current = ttr_get_current(ttr);
    assert(0 == current);

    // test dirty, not started
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    ttr_end(ttr, 0);
    result = ttr_end(ttr, 0);
    assert(TTR_ERROR == result);

    fprintf(stdout, "[PASSED] test_end\n");
}

void test_pause(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    TTRReturnCode result = ttr_pause(ttr, 0);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_current(ttr);
    assert(0 != current);
    assert(TTR_RECORD_PAUSED == current->state);
    assert(0 != current->interval[0].start);
    assert(0 != current->interval[0].end);

    // test empty, not started
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    ttr_end(ttr, 0);
    result = ttr_pause(ttr, 0);
    assert(TTR_ERROR == result);

    // test empty, paused
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    ttr_pause(ttr, 0);
    result = ttr_pause(ttr, 0);
    assert(TTR_ERROR == result);
}

void test_unpause(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    ttr_pause(ttr, 0);
    TTRReturnCode result = ttr_unpause(ttr, 0);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_current(ttr);
    assert(0 != current);
    assert(TTR_RECORD_STARTED == current->state);
    assert(0 != current->interval[1].start);
    assert(0 == current->interval[1].end);

    // test empty, not started
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    result = ttr_unpause(ttr, 0);
    assert(TTR_ERROR == result);

    // test empty, not paused
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    result = ttr_unpause(ttr, 0);
    assert(TTR_ERROR == result);

    // test empty, pause overflow
    ttr = ttr_init(arena, str_lit("ttr_empty.records"));
    ttr_start(ttr, 0);
    for (U32 i = 0; i < TTR_MAX_INTERVAL; ++i)
    {
        ttr_pause(ttr, 0);
        ttr_unpause(ttr, 0);
    }
    result = ttr_pause(ttr, 0);
    assert(TTR_ERROR == result);
}

int main(int argc, char **argv)
{
    printf("Time Tracking Test\n");

    fprintf(stdout, "sizeof(TTR) == %zd\n", sizeof(TTR));
    fprintf(stdout, "sizeof(TTRRecord) == %zd\n", sizeof(TTRRecord));
    fprintf(stdout, "sizeof(TTRInterval) == %zd\n", sizeof(TTRInterval));

    Arena arena = arena_create(MB(20));

    test_init(&arena);
    test_start(&arena);
    test_end(&arena);
    test_pause(&arena);
    test_unpause(&arena);

    return 0;
}