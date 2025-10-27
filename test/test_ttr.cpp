#include "test_ttr_reader.hpp"

#include <base_arena.hpp>
#include <base_datetime.hpp>

#include <ttr.hpp>

#include <assert.h>

namespace testing
{

void test_ttr_start_now_empty(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    uint8_t *currentArenaAddress = arena->currentAddress;

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(currentArenaAddress != arena->currentAddress);

    assert(ttr::return_code::STARTED == startResult);
    assert(recordList.first);
    assert(recordList.last);
    assert(1 == recordList.monthCount);
    assert(ttr::day_state::STARTED == recordList.last->days[nowDate.day].state);
    assert(!recordList.last->days[nowDate.day].workInterval.complete);
    assert(0 != recordList.last->days[nowDate.day].workInterval.start);
}

void test_ttr_start_now_filled(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(ttr::return_code::STARTED == startResult);
    assert(1 == recordList.monthCount);
    assert(ttr::day_state::STARTED == recordList.last->days[nowDate.day].state);
    assert(!recordList.last->days[nowDate.day].workInterval.complete);
    assert(0 != recordList.last->days[nowDate.day].workInterval.start);
}

void test_ttr_start_now_skip_month(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month - 1; // TODO(speciial): make sure this isn't 0
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    uint8_t *currentArenaAddress = arena->currentAddress;

    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(currentArenaAddress != arena->currentAddress);

    assert(ttr::return_code::STARTED == startResult);
    assert(2 == recordList.monthCount);
    assert(ttr::day_state::STARTED == recordList.last->days[nowDate.day].state);
    assert(!recordList.last->days[nowDate.day].workInterval.complete);
    assert(0 != recordList.last->days[nowDate.day].workInterval.start);
}

void test_ttr_start_now_skip_year(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year - 1;
    month->month = nowDate.month + 1; // TODO(speciial): make sure this isn't 13
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    uint8_t *currentArenaAddress = arena->currentAddress;

    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(currentArenaAddress != arena->currentAddress);

    assert(ttr::return_code::STARTED == startResult);
    assert(2 == recordList.monthCount);
    assert(ttr::day_state::STARTED == recordList.last->days[nowDate.day].state);
    assert(!recordList.last->days[nowDate.day].workInterval.complete);
    assert(0 != recordList.last->days[nowDate.day].workInterval.start);
}

void test_ttr_start_now_already_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code firstStartResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == firstStartResult);

    ttr::return_code secondStartResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::ALREADY_STARTED == secondStartResult);
}

void test_ttr_start_now_already_ended(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code firstStartResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == firstStartResult);
    ttr::return_code firstEndResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ENDED == firstEndResult);

    ttr::return_code secondStartResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::ALREADY_ENDED == secondStartResult);
}

void test_ttr_start_now_with_more_recent_month(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month + 1; // TODO(speciial): make sure this isn't 13
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    uint8_t *currentArenaAddress = arena->currentAddress;
    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(currentArenaAddress == arena->currentAddress);
    assert(ttr::return_code::ERROR == startResult);
}

void test_ttr_start_now_with_missing_last(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    // recordList.last = month;
    recordList.monthCount = 1;

    uint8_t *currentArenaAddress = arena->currentAddress;
    ttr::return_code startResult = ttr::start_now(arena, &recordList);

    assert(currentArenaAddress == arena->currentAddress);
    assert(ttr::return_code::ERROR == startResult);
}

void test_ttr_end_now_with_already_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ENDED == endResult);
    assert(ttr::day_state::ENDED == recordList.last->days[nowDate.day].state);
    assert(recordList.last->days[nowDate.day].workInterval.complete);
    assert(0 != recordList.last->days[nowDate.day].workInterval.start);
    assert(0 != recordList.last->days[nowDate.day].workInterval.end);
}

void test_ttr_end_now_with_not_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    // ttr::return_code startResult = ttr::start_now(arena, &recordList);
    // assert(ttr::return_code::STARTED == startResult);

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == endResult);
    assert(ttr::day_state::UNUSED == recordList.last->days[nowDate.day].state);
    assert(!recordList.last->days[nowDate.day].workInterval.complete);
}

void test_ttr_end_now_with_no_month(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == endResult);
    assert(!recordList.first);
    assert(!recordList.last);
}

void test_ttr_end_now_with_already_ended(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code firstEndResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ENDED == firstEndResult);

    ttr::return_code secondEndResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ALREADY_ENDED == secondEndResult);
}

void test_ttr_end_now_with_last_missing(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    // recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ERROR == endResult);
}

void test_ttr_pause_now_with_already_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::PAUSED == pauseResult);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 != recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_pause_now_with_not_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == pauseResult);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_pause_now_with_already_paused(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code firstPauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::PAUSED == firstPauseResult);

    ttr::return_code secondPauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::ALREADY_PAUSED == secondPauseResult);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 != recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_pause_now_with_already_ended(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ENDED == endResult);

    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::ALREADY_ENDED == pauseResult);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_pause_now_with_no_month(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == pauseResult);
    assert(!recordList.first);
    assert(!recordList.last);
}

void test_ttr_pause_now_with_last_missing(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    // recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::ERROR == pauseResult);
}

void test_ttr_unpause_now_with_already_started_and_paused(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code pauseResult = ttr::pause_now(&recordList);
    assert(ttr::return_code::PAUSED == pauseResult);

    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::UNPAUSED == unpauseResult);

    assert(1 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 != recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 != recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_unpause_now_with_already_started_and_not_paused(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::NOT_YET_PAUSED == unpauseResult);

    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_unpause_now_with_not_started(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == unpauseResult);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_unpause_now_with_already_ended(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code startResult = ttr::start_now(arena, &recordList);
    assert(ttr::return_code::STARTED == startResult);

    ttr::return_code endResult = ttr::end_now(&recordList);
    assert(ttr::return_code::ENDED == endResult);

    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::ALREADY_ENDED == unpauseResult);

    assert(0 == recordList.last->days[nowDate.day].pauseIntervalCount);
    assert(!recordList.last->days[nowDate.day].pauseIntervals[0].complete);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].start);
    assert(0 == recordList.last->days[nowDate.day].pauseIntervals[0].end);
}

void test_ttr_unpause_now_with_no_month(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::NOT_YET_STARTED == unpauseResult);
    assert(!recordList.first);
    assert(!recordList.last);
}

void test_ttr_unpause_now_with_last_missing(base::arena *arena)
{
    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    ttr::record_list recordList = {};
    ttr::month *month = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    month->year = nowDate.year;
    month->month = nowDate.month;
    recordList.first = month;
    // recordList.last = month;
    recordList.monthCount = 1;

    ttr::return_code unpauseResult = ttr::unpause_now(&recordList);
    assert(ttr::return_code::ERROR == unpauseResult);
}

void run_test_ttr()
{
    base::arena arena = base::arena_create(10 * 1024 * 1024);

    /*
        A few test cases I'd like to have:
            -[x] start with empty recordList
            -[x] start with filled recordList
            -[x] start with recordList where we "skip" a month
            -[x] start with recordList where we "skip" a year
            -[x] start with recordList where now is already started
            -[x] start with recordList where now is already ended
            -[x] start with recordList where last is more recent than now
            -[x] start with broken recordList with either first or last being set

            -[x] end with started day
            -[x] end with no started day
            -[x] end with no month existing
            -[x] end with day already ended
            -[x] end with broken recordList with either first or last being set

            -[x] pause with started day
            -[x] pause with no started day
            -[x] pause with already paused day
            -[x] pause with no month existing
            -[x] pause with day already ended
            -[x] pause with broken recordList with either first or last being set

            -[x] unpause with started and paused day
            -[x] unpause with not paused day
            -[x] unpause with not started day
            -[x] unpause with day already ended
            -[x] unpause with no month exisiting
            -[x] unpause broken recordList with either first or last being set

            -[ ] show with started
            -[ ] show with paused
            -[ ] show with unpaused
            -[ ] show with ended
    */

    // ttr::start_now
    test_ttr_start_now_empty(&arena);
    test_ttr_start_now_filled(&arena);
    test_ttr_start_now_skip_month(&arena);
    test_ttr_start_now_skip_year(&arena);
    test_ttr_start_now_already_started(&arena);
    test_ttr_start_now_already_ended(&arena);
    test_ttr_start_now_with_more_recent_month(&arena);
    test_ttr_start_now_with_missing_last(&arena);

    // ttr::end_now
    test_ttr_end_now_with_already_started(&arena);
    test_ttr_end_now_with_not_started(&arena);
    test_ttr_end_now_with_no_month(&arena);
    test_ttr_end_now_with_already_ended(&arena);
    test_ttr_end_now_with_last_missing(&arena);

    // ttr::pause_now
    test_ttr_pause_now_with_already_started(&arena);
    test_ttr_pause_now_with_not_started(&arena);
    test_ttr_pause_now_with_already_paused(&arena);
    test_ttr_pause_now_with_already_ended(&arena);
    test_ttr_pause_now_with_no_month(&arena);
    test_ttr_pause_now_with_last_missing(&arena);

    // ttr::unpause_now
    test_ttr_unpause_now_with_already_started_and_paused(&arena);
    test_ttr_unpause_now_with_already_started_and_not_paused(&arena);
    test_ttr_unpause_now_with_not_started(&arena);
    test_ttr_unpause_now_with_already_ended(&arena);
    test_ttr_unpause_now_with_no_month(&arena);
    test_ttr_unpause_now_with_last_missing(&arena);

    // ttr::show_now
}

} // namespace testing
