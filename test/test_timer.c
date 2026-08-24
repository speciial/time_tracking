#include <stdio.h>
#include <assert.h>

#include <base_datetime.h>
#include <timer.h>

void test_timer_start()
{
    Timestamp start = 1773058784;
    Timer timer = { 0 };
    TimerResult result = timer_start(&timer, start);

    assert(TIMER_RESULT_SUCCESS == result);
    assert(TIMER_STATE_STARTED == timer.state);
    assert(start == timer.start);

    result = timer_start(&timer, (Timestamp)(start + MINUTES(3)));

    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_STARTED == timer.state);
    assert(start == timer.start);
}

void test_timer_end()
{
    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    Timer timer = { 0 };
    TimerResult result = timer_end(&timer, end);

    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_UNINITIALIZED == timer.state);

    timer_start(&timer, start);
    result = timer_end(&timer, end);

    assert(TIMER_RESULT_SUCCESS == result);
    assert(TIMER_STATE_ENDED == timer.state);
    assert(start == timer.start);
    assert(end == timer.end);

    Timer pausedTimer = { 0 };
    timer_start(&pausedTimer, start);
    timer_pause(&pausedTimer, pause);
    result = timer_end(&pausedTimer, end);

    assert(TIMER_RESULT_SUCCESS == result);
    assert(TIMER_STATE_ENDED == timer.state);
    assert(start == timer.start);
    assert(end == timer.end);
}

void test_timer_pause()
{
    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    Timer timer = { 0 };

    TimerResult result = timer_pause(&timer, pause);

    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_UNINITIALIZED == timer.state);

    timer_start(&timer, start);
    result = timer_pause(&timer, pause);

    assert(TIMER_RESULT_SUCCESS == result);
    assert(TIMER_STATE_PAUSED == timer.state);
    assert(start == timer.start);
    assert(pause == timer.lastPause);

    result = timer_pause(&timer, pause);
    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_PAUSED == timer.state);
}

void test_timer_unpause()
{
    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp unpause = (Timestamp)(1773058784 + HOURS(1) + MINUTES(30));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    Timer timer = { 0 };

    TimerResult result = timer_unpause(&timer, pause);

    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_UNINITIALIZED == timer.state);

    timer_start(&timer, start);
    timer_pause(&timer, pause);
    result = timer_unpause(&timer, unpause);

    assert(TIMER_RESULT_SUCCESS == result);
    assert(TIMER_STATE_STARTED == timer.state);
    assert(start == timer.start);
    assert(MINUTES(30) == timer.totalPauseTimeSeconds);
    assert(0 == timer.lastPause);

    result = timer_unpause(&timer, unpause);
    assert(TIMER_RESULT_ERROR == result);
    assert(TIMER_STATE_STARTED == timer.state);
}

void test_timer_total_active_time_seconds()
{
    Timestamp start = 1773058784;
    Timestamp pause = (Timestamp)(1773058784 + HOURS(1));
    Timestamp unpause = (Timestamp)(1773058784 + HOURS(1) + MINUTES(30));
    Timestamp end = (Timestamp)(1773058784 + HOURS(2));
    Timer timer = { 0 };

    timer_start(&timer, start);
    S64 activeTime = timer_total_active_time_seconds(&timer, 0);

    assert(0 < activeTime);

    timer_pause(&timer, pause);
    activeTime = timer_total_active_time_seconds(&timer, 0);

    assert(HOURS(1) == activeTime);

    timer_unpause(&timer, unpause);
    activeTime = timer_total_active_time_seconds(&timer, 0);

    assert(0 < activeTime);
    assert(MINUTES(30) == timer.totalPauseTimeSeconds);

    timer_end(&timer, end);
    activeTime = timer_total_active_time_seconds(&timer, 0);

    assert(HOURS(1) + MINUTES(30) == activeTime);
}

int test_timer(int argc, char **argv)
{
    printf("Test Timer\n");

    test_timer_start();
    test_timer_end();
    test_timer_pause();
    test_timer_unpause();
    test_timer_total_active_time_seconds();

    return 0;
}