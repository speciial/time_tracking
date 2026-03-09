#include "timer.h"

#include <base_datetime.h>

TimerResult timer_start(Timer *timer, Timestamp timestamp)
{
    TimerResult result = TIMER_ERROR;
    if (timer->state == TIMER_UNINITIALIZED)
    {
        timer->start = (timestamp == 0) ? get_current_timestamp() : timestamp;
        timer->state = TIMER_STARTED;

        result = TIMER_SUCCESS;
    }
    return result;
}

TimerResult timer_end(Timer *timer, Timestamp timestamp)
{
    TimerResult result = TIMER_ERROR;
    if (timer->state == TIMER_STARTED)
    {
        timer->end = (timestamp == 0) ? get_current_timestamp() : timestamp;
        timer->state = TIMER_ENDED;

        result = TIMER_SUCCESS;
    }
    else if (timer->state == TIMER_PAUSED)
    {
        Timestamp end = (timestamp == 0) ? get_current_timestamp() : timestamp;
        timer->totalPauseTimeSeconds += timestamp_diff_seconds(end, timer->lastPause); // end - lastPause
        timer->end = end;
        timer->lastPause = 0;
        timer->state = TIMER_ENDED;

        result = TIMER_SUCCESS;
    }
    return result;
}

TimerResult timer_pause(Timer *timer, Timestamp timestamp)
{
    TimerResult result = TIMER_ERROR;
    if (timer->state == TIMER_STARTED)
    {
        timer->lastPause = (timestamp == 0) ? get_current_timestamp() : timestamp;
        timer->state = TIMER_PAUSED;

        result = TIMER_SUCCESS;
    }
    return result;
}

TimerResult timer_unpause(Timer *timer, Timestamp timestamp)
{
    TimerResult result = TIMER_ERROR;
    if (timer->state == TIMER_PAUSED)
    {
        Timestamp unpause = (timestamp == 0) ? get_current_timestamp() : timestamp;
        timer->totalPauseTimeSeconds += timestamp_diff_seconds(unpause, timer->lastPause); // unpause - lastPause
        timer->lastPause = 0;
        timer->state = TIMER_STARTED;

        result = TIMER_SUCCESS;
    }
    return result;
}

S64 timer_total_active_time_seconds(Timer *timer)
{
    S64 result = 0;

    if (timer->state == TIMER_STARTED)
    {
        Timestamp now = get_current_timestamp();
        result = max_value(0, timestamp_diff_seconds(now, timer->start) - timer->totalPauseTimeSeconds);
    }
    else if (timer->state == TIMER_ENDED)
    {
        result = max_value(0, timestamp_diff_seconds(timer->end, timer->start) - timer->totalPauseTimeSeconds);
    }
    else if (timer->state == TIMER_PAUSED)
    {
        result = max_value(0, timestamp_diff_seconds(timer->lastPause, timer->start) - timer->totalPauseTimeSeconds);
    }

    return result;
}
