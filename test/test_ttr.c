#include <stdio.h>
#include <assert.h>

#include <base_arena.h>

#include <ttr.h>

void test_init(Arena *arena)
{
    TTR *ttr = ttr_init_empty(arena, 16);
    assert(0 != ttr);
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

void test_comment(Arena *arena)
{
    // test empty, started
    TTR *ttr = ttr_init_empty(arena, 16);

    Timestamp start = 1773058784;
    Timestamp end = (Timestamp)(start + HOURS(2));
    DateTime startDt = datetime_from_timestamp(start);
    String comment = str_lit("some message");

    // test comment with no active record
    TTRReturnCode result = ttr_comment(ttr, start, comment);
    assert(TTR_ERROR == result);

    // test comment with active record
    ttr_start(ttr, start);
    result = ttr_comment(ttr, start, comment);
    assert(TTR_SUCCESS == result);
    TTRRecord *current = ttr_get_day(ttr, startDt);
    assert(string_equals(comment, current->message));

    // test on ended comment
    ttr_end(ttr, end);
    result = ttr_comment(ttr, start, comment);
    assert(TTR_SUCCESS == result);

    // test with 0 timestamp
    ttr_start(ttr, 0);
    result = ttr_comment(ttr, 0, comment);
    assert(TTR_SUCCESS == result);
}

void test_end_to_end(Arena *arena)
{
    // file has 11 ended timers and 1 started timer
    TTR *ttr = ttr_init_from_file(arena, str_lit("ttr_multiple_entries.records"), 2);
    assert(-1 != ttr->currentlyActiveIndex);

    TTRRecord *currentlyActive = ttr_get_active(ttr);
    assert(0 != currentlyActive);

    Timestamp currentlyActiveStart = currentlyActive->timer.start;
    Timestamp currentlyActiveEnd = currentlyActiveStart + HOURS(8);
    ttr_end(ttr, currentlyActiveEnd);
    TTRReturnCode result = ttr_save_to_file(arena, ttr, str_lit("test_output/output.records"));
    assert(TTR_ERROR != result);

    // file now has 12 ended timers and 0 started
    ttr = ttr_init_from_file(arena, str_lit("test_output/output.records"), 2);
    assert(-1 == ttr->currentlyActiveIndex);
    assert(0 != ttr_get_day(ttr, datetime_from_timestamp(currentlyActiveStart)));

    Timestamp start = currentlyActiveStart + DAYS(1);
    String comment = str_lit("some message");
    ttr_start(ttr, start);
    ttr_comment(ttr, start, comment);

    currentlyActive = ttr_get_active(ttr);
    assert(0 != currentlyActive);

    result = ttr_save_to_file(arena, ttr, str_lit("test_output/output.records"));
    assert(TTR_ERROR != result);

    // file now has 12 ended timers and 1 started
    ttr = ttr_init_from_file(arena, str_lit("test_output/output.records"), 2);
    assert(-1 != ttr->currentlyActiveIndex);

    currentlyActive = ttr_get_active(ttr);
    assert(0 != currentlyActive);
    assert(string_equals(comment, currentlyActive->message));

    Timestamp pause = (Timestamp)(start + HOURS(1));
    ttr_pause(ttr, pause);
    assert(TIMER_STATE_PAUSED == currentlyActive->timer.state);

    result = ttr_save_to_file(arena, ttr, str_lit("test_output/output.records"));
    assert(TTR_ERROR != result);

    // file now has 12 ended timers and 1 paused
    ttr = ttr_init_from_file(arena, str_lit("test_output/output.records"), 2);
    assert(-1 != ttr->currentlyActiveIndex);

    currentlyActive = ttr_get_active(ttr);
    assert(0 != currentlyActive);
    assert(TIMER_STATE_PAUSED == currentlyActive->timer.state);

    Timestamp unpause = (Timestamp)(start + HOURS(1) + MINUTES(30));
    ttr_unpause(ttr, unpause);

    result = ttr_save_to_file(arena, ttr, str_lit("test_output/output.records"));
    assert(TTR_ERROR != result);

    // file now has 12 ended timers and 1 started
    ttr = ttr_init_from_file(arena, str_lit("test_output/output.records"), 2);
    assert(-1 != ttr->currentlyActiveIndex);

    currentlyActive = ttr_get_active(ttr);
    assert(0 != currentlyActive);
    assert(TIMER_STATE_STARTED == currentlyActive->timer.state);

    Timestamp end = (Timestamp)(start + HOURS(4));
    ttr_end(ttr, end);

    result = ttr_save_to_file(arena, ttr, str_lit("test_output/output.records"));
    assert(TTR_ERROR != result);

    // file now has 13 ended timers
    ttr = ttr_init_from_file(arena, str_lit("test_output/output.records"), 1);
    assert(-1 == ttr->currentlyActiveIndex);

    TTRRecord *record = ttr_get_day(ttr, datetime_from_timestamp(start));
    assert(start == record->timer.start);
    assert(end == record->timer.end);
    assert(string_equals(comment, record->message));
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
    test_comment(&arena);
    test_end_to_end(&arena);

    return 0;
}