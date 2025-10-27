#include "base_datetime.hpp"

namespace base
{

timestamp timestamp_now()
{
    return (timestamp)time(0);
}

date_time date_time_from_timestamp(timestamp timestamp)
{
    date_time result = {};

    struct tm *t = localtime(&timestamp);
    if (t)
    {
        result.year = t->tm_year + 1900;
        result.month = t->tm_mon + 1;
        result.day = t->tm_mday;
        result.hour = t->tm_hour;
        result.minute = t->tm_min;
        result.second = t->tm_sec;
    }

    return result;
}

timestamp timestamp_from_date_time(date_time date_time)
{
    return 0;
}

} // namespace base
