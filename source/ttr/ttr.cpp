#include "ttr.hpp"

#include <assert.h>

namespace ttr
{

bool month_is_equal_or_more_recent(unsigned int y0, unsigned int m0, unsigned int y1, unsigned int m1)
{
    // NOTE(speciial): Is function is used to ensure that a recordList does not contain any 
    // entries that are more recent than now (aka. from the future). 
    bool yearIsEqualOrMoreRecent = y0 <= y1;
    bool monthIsEqualOrMoreRecent = (y0 < y1) || (m0 <= m1);

    bool dateIsEqualOrMoreRecent = yearIsEqualOrMoreRecent && monthIsEqualOrMoreRecent;
    return dateIsEqualOrMoreRecent;
}

bool is_valid(record_list *recordList)
{
    bool result = false;

    if (recordList)
    {
        if (recordList->first && recordList->last)
        {
            base::timestamp now = base::timestamp_now();
            base::date_time nowDate = base::date_time_from_timestamp(now);

            if (month_is_equal_or_more_recent(recordList->last->year, recordList->last->month,
                                              nowDate.year, nowDate.month))
            {
                result = true;
            }
        }
        else if (!recordList->first && !recordList->last && recordList->monthCount == 0)
        {
            // NOTE(speciial): Uninitialized recordList is valid.
            result = true;
        }
    }

    return result;
}

bool requires_new_month(record_list *recordList, base::date_time *nowDate)
{
    bool result = false;

    if (!recordList->first && !recordList->last)
    {
        result = true;
    }
    else
    {
        if (recordList->last->month < nowDate->month)
        {
            result = true;
        }
    }

    return result;
}

void alloc_new_month(base::arena *arena, record_list *recordList, unsigned int year, unsigned int month)
{
    ttr::month *newMonth = (ttr::month *)base::arena_alloc(arena, sizeof(ttr::month), 1);
    newMonth->year = year;
    newMonth->month = month;

    if (!recordList->first && !recordList->last)
    {
        recordList->first = newMonth;
        recordList->last = newMonth;
    }
    else
    {
        recordList->last->nextMonth = newMonth;
        recordList->last = newMonth;
    }
    recordList->monthCount += 1;
}

return_code start_now(base::arena *arena, record_list *recordList)
{
    return_code result;

    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    if (is_valid(recordList))
    {
        if (requires_new_month(recordList, &nowDate))
        {
            alloc_new_month(arena, recordList, nowDate.year, nowDate.month);
        }

        switch (recordList->last->days[nowDate.day].state)
        {
            case day_state::UNUSED: {
                recordList->last->days[nowDate.day].workInterval.complete = false;
                recordList->last->days[nowDate.day].workInterval.start = now;
                recordList->last->days[nowDate.day].state = day_state::STARTED;
                result = return_code::STARTED;
            } break;

            case day_state::STARTED:
            case day_state::PAUSED: {
                result = return_code::ALREADY_STARTED;
            } break;

            case day_state::ENDED: {
                result = return_code::ALREADY_ENDED;
            } break;

            default: {
                // TODO(speciial): Something went wrong.
            } break;
        }
    }
    else
    {
        result = return_code::ERROR;
    }

    return result;
}

return_code end_now(record_list *recordList)
{
    return_code result;

    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    if (is_valid(recordList))
    {
        if (!requires_new_month(recordList, &nowDate))
        {
            switch (recordList->last->days[nowDate.day].state)
            {
                case day_state::UNUSED: {
                    result = return_code::NOT_YET_STARTED;
                } break;

                case day_state::STARTED: {
                    recordList->last->days[nowDate.day].workInterval.end = now;
                    recordList->last->days[nowDate.day].workInterval.complete = true;
                    recordList->last->days[nowDate.day].state = day_state::ENDED;
                    result = return_code::ENDED;
                } break;

                case day_state::PAUSED: {
                    // TODO(speciial): Handle this case.
                } break;

                case day_state::ENDED: {
                    result = return_code::ALREADY_ENDED;
                } break;

                default: {
                    // TODO(speciial): Something went wrong.
                } break;
            }
        }
        else
        {
            result = return_code::NOT_YET_STARTED;
        }
    }
    else
    {
        result = return_code::ERROR;
    }

    return result;
}

return_code pause_now(record_list *recordList)
{
    return_code result;

    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    if (is_valid(recordList))
    {
        if (!requires_new_month(recordList, &nowDate))
        {
            switch (recordList->last->days[nowDate.day].state)
            {
                case day_state::UNUSED: {
                    result = return_code::NOT_YET_STARTED;
                } break;

                case day_state::STARTED: {
                    int pauseIntervalIndex = recordList->last->days[nowDate.day].pauseIntervalCount;
                    recordList->last->days[nowDate.day].pauseIntervals[pauseIntervalIndex].start = now;
                    recordList->last->days[nowDate.day].state = day_state::PAUSED;
                    result = return_code::PAUSED;
                } break;

                case day_state::PAUSED: {
                    result = return_code::ALREADY_PAUSED;
                } break;

                case day_state::ENDED: {
                    result = return_code::ALREADY_ENDED;
                } break;

                default: {
                    // TODO(speciial): Something went wrong.
                } break;
            }
        }
        else
        {
            result = return_code::NOT_YET_STARTED;
        }
    }
    else
    {
        result = return_code::ERROR;
    }

    return result;
}

return_code unpause_now(record_list *recordList)
{
    return_code result;

    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    if (is_valid(recordList))
    {
        if (!requires_new_month(recordList, &nowDate))
        {
            switch (recordList->last->days[nowDate.day].state)
            {
                case day_state::UNUSED: {
                    result = return_code::NOT_YET_STARTED;
                } break;

                case day_state::STARTED: {
                    result = return_code::NOT_YET_PAUSED;
                } break;

                case day_state::PAUSED: {
                    int pauseIntervalIndex = recordList->last->days[nowDate.day].pauseIntervalCount;
                    recordList->last->days[nowDate.day].pauseIntervals[pauseIntervalIndex].end = now;
                    recordList->last->days[nowDate.day].pauseIntervals[pauseIntervalIndex].complete = true;
                    recordList->last->days[nowDate.day].pauseIntervalCount++;
                    recordList->last->days[nowDate.day].state = day_state::STARTED;
                    result = return_code::UNPAUSED;
                } break;

                case day_state::ENDED: {
                    result = return_code::ALREADY_ENDED;
                } break;

                default: {
                    // TODO(speciial): Something went wrong.
                } break;
            }
        }
        else
        {
            result = return_code::NOT_YET_STARTED;
        }
    }
    else
    {
        result = return_code::ERROR;
    }

    return result;
}

base::string show_now(base::arena *arena, record_list *recordList)
{
    base::string result = {};

    base::timestamp now = base::timestamp_now();
    base::date_time nowDate = base::date_time_from_timestamp(now);

    // [STARTED] 07.10.2025, 08:34, Time:2.30(Net:1.55) 
    // [ PAUSED] 07.10.2025, 08:34, Time:2.30(Net:1.55) 
    // [  ENDED] 07.10.2025, 08:34, Time:2.30(Net:1.55)

    if (is_valid(recordList) && !requires_new_month(recordList, &nowDate))
    {
        ttr::day today = recordList->last->days[nowDate.day];

        switch (recordList->last->days[nowDate.day].state)
        {
            case day_state::UNUSED: {
                result.content = "There is nothing tracked for today.";
                result.length = base::string_length(result.content);
            } break;

            case day_state::STARTED: {
                result.content = "[STARTED]";
                result.length = base::string_length(result.content);
            } break;

            case day_state::PAUSED: {
                result.content = "[ PAUSED]";
                result.length = base::string_length(result.content);
            } break;

            case day_state::ENDED: {
                result.content = "[  ENDED]";
                result.length = base::string_length(result.content);
            } break;

            default: {
                // TODO(speciial): Something went wrong.
            } break;
        }
    }

    return result;
}

} // namespace ttr