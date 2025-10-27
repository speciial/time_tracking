#ifndef ttr_HPP
#define ttr_HPP

#include <base_arena.hpp>
#include <base_string.hpp>
#include <base_datetime.hpp>

namespace ttr
{

#define MAX_DAYS_PER_MONTH          32
#define MAX_PAUSE_INTERVAL_COUNT    4

enum class return_code
{
    STARTED,
    ENDED,
    PAUSED,
    UNPAUSED,

    ALREADY_STARTED,
    ALREADY_ENDED,
    ALREADY_PAUSED,
    ALREADY_UNPAUSED,

    NOT_YET_STARTED,
    NOT_YET_PAUSED,

    ERROR
};

enum class day_state
{
    UNUSED = 0,
    STARTED = 1,
    PAUSED = 2,
    ENDED = 3
};

struct interval
{
    base::timestamp start;
    base::timestamp end;
    bool complete;
};

struct day
{
    interval workInterval;
    unsigned int pauseIntervalCount;
    interval pauseIntervals[MAX_PAUSE_INTERVAL_COUNT];
    day_state state;
};

struct month
{
    unsigned int year;
    unsigned int month;

    day days[MAX_DAYS_PER_MONTH];
    struct month *nextMonth;
};

struct record_list
{
    month *first;
    month *last;
    int monthCount;
};

return_code start_now(base::arena *arena, record_list *recordList);

return_code end_now(record_list *recordList);

return_code pause_now(record_list *recordList);

return_code unpause_now(record_list *recordList);

base::string show_now(base::arena *arena, record_list *recordList);

} // namespace ttr

#endif // ttr_HPP