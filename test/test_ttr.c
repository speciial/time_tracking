#include <stdio.h>
#include <assert.h>

#include <base_arena.h>

#include <ttr.h>

void test_init(Arena *arena)
{
    TTR *ttr = ttr_init_empty(arena, 16);
    assert(0 != ttr);

    // TODO(speciial): test init once file parsing is implemented
}

void test_start(Arena *arena)
{
    // test empty
    TTR *ttr = ttr_init_empty(arena, 16);
    Timestamp start = 1773058784;
    DateTime startDt = datetime_from_timestamp(start);

    TTRReturnCode result = ttr_start(ttr, start);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_day(ttr, startDt);
    assert(0 != current);
    assert(TIMER_STATE_STARTED == current->timer.state);
    assert(is_same_date(startDt, current->day));

    // test dirty
    ttr = ttr_init_empty(arena, 16);
    result = ttr_start(ttr, 0);
    assert(TTR_SUCCESS == result);

    result = ttr_start(ttr, 0);
    assert(TTR_ERROR == result);
}

void test_end(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init_empty(arena, 16);

    Timestamp start = 1773058784;
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    DateTime startDt = datetime_from_timestamp(start);

    ttr_start(ttr, start);
    TTRReturnCode result = ttr_end(ttr, end);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_day(ttr, startDt);
    assert(0 != current);
    assert(TIMER_STATE_ENDED == current->timer.state);

    // test empty, not started
    ttr = ttr_init_empty(arena, 16);
    result = ttr_end(ttr, end);
    assert(TTR_ERROR == result);

    current = ttr_get_day(ttr, startDt);
    assert(0 == current);

    // test dirty, already ended 
    ttr = ttr_init_empty(arena, 16);
    ttr_start(ttr, 0);
    ttr_end(ttr, 0);
    result = ttr_end(ttr, 0);
    assert(TTR_ERROR == result);
}

void test_pause(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init_empty(arena, 16);
    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    DateTime startDt = datetime_from_timestamp(start);

    ttr_start(ttr, start);
    TTRReturnCode result = ttr_pause(ttr, pause);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_day(ttr, startDt);
    assert(0 != current);
    assert(TIMER_STATE_PAUSED == current->timer.state);

    // test empty, not started
    ttr = ttr_init_empty(arena, 16);
    ttr_start(ttr, 0);
    ttr_end(ttr, 0);
    result = ttr_pause(ttr, 0);
    assert(TTR_ERROR == result);

    // test empty, paused
    ttr = ttr_init_empty(arena, 16);
    ttr_start(ttr, 0);
    ttr_pause(ttr, 0);
    result = ttr_pause(ttr, 0);
    assert(TTR_ERROR == result);
}

void test_unpause(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init_empty(arena, 16);

    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp unpause = (Timestamp)(1773058784 + HOURS(1) + MINUTES(30));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    DateTime startDt = datetime_from_timestamp(start);

    ttr_start(ttr, start);
    ttr_pause(ttr, pause);
    TTRReturnCode result = ttr_unpause(ttr, unpause);
    assert(TTR_SUCCESS == result);

    TTRRecord *current = ttr_get_day(ttr, startDt);
    assert(0 != current);
    assert(TIMER_STATE_STARTED == current->timer.state);

    // test empty, not started
    ttr = ttr_init_empty(arena, 16);
    result = ttr_unpause(ttr, 0);
    assert(TTR_ERROR == result);

    // test empty, not paused
    ttr = ttr_init_empty(arena, 16);
    ttr_start(ttr, 0);
    result = ttr_unpause(ttr, 0);
    assert(TTR_ERROR == result);
}

int test_ttr(int argc, char **argv)
{
    printf("Test TTR\n");

    // fprintf(stdout, "sizeof(TTR) == %zd\n", sizeof(TTR));
    // fprintf(stdout, "sizeof(TTRRecord) == %zd\n", sizeof(TTRRecord));

    Arena arena = arena_create(MB(20));

    test_init(&arena);
    test_start(&arena);
    test_end(&arena);
    test_pause(&arena);
    test_unpause(&arena);

    return 0;
}