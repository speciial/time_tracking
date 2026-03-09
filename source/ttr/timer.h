#ifndef timer_h
#define timer_h

#include <base_core.h>

// TODO(speciial): ensure the timer only uses "reasonable" timestamps 
//                 as arguments, e.g. no end before start etc.

// TODO(speciial): check if i really need enume as result type or if a
//                 B32 would be enough. 


typedef enum TimerResult TimerResult;
enum TimerResult
{
    TIMER_ERROR = 0,
    TIMER_SUCCESS
};

typedef enum TimerState TimerState;
enum TimerState
{
    TIMER_UNINITIALIZED = 0,
    TIMER_STARTED,
    TIMER_PAUSED,
    TIMER_ENDED
};

typedef struct Timer Timer;
struct Timer
{
    Timestamp start;
    Timestamp end;
    Timestamp lastPause;
    S64 totalPauseTimeSeconds;
    TimerState state;
};

TimerResult timer_start(Timer *timer, Timestamp timestamp);
TimerResult timer_end(Timer *timer, Timestamp timestamp);
TimerResult timer_pause(Timer *timer, Timestamp timestamp);
TimerResult timer_unpause(Timer *timer, Timestamp timestamp);

S64 timer_total_active_time_seconds(Timer *timer);

#endif // timer_h