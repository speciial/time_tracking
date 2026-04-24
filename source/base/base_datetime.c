#include "base_datetime.h"

#include <time.h>

Timestamp get_current_timestamp()
{
    return (Timestamp)time(0);
}

DateTime get_current_datetime()
{
    return datetime_from_timestamp(get_current_timestamp());
}

Timestamp timestamp_from_datetime(DateTime dateTime)
{
    struct tm timeInfo = { 0 };
    timeInfo.tm_year = dateTime.year - 1900;
    timeInfo.tm_mon = dateTime.month - 1;
    timeInfo.tm_mday = dateTime.day;
    timeInfo.tm_hour = dateTime.hour;
    timeInfo.tm_min = dateTime.minute;
    timeInfo.tm_sec = dateTime.second;

    time_t timeValue = mktime(&timeInfo);
    return (Timestamp)timeValue;
}

DateTime datetime_from_timestamp(Timestamp timestamp)
{
    DateTime result = { 0 };

    time_t timeValue = (time_t)timestamp;
    struct tm *timeInfo = gmtime(&timeValue);
    if (timeInfo)
    {
        result.year = (U16)(timeInfo->tm_year + 1900);
        result.month = (U16)(timeInfo->tm_mon + 1);
        result.day = (U16)timeInfo->tm_mday;
        result.hour = (U16)timeInfo->tm_hour;
        result.minute = (U16)timeInfo->tm_min;
        result.second = (U16)timeInfo->tm_sec;
    }

    return result;
}

S64 timestamp_diff_seconds(Timestamp end, Timestamp begin)
{
    return ((S64)end - (S64)begin);
}

B32 is_same_date(DateTime dt1, DateTime dt2)
{
    B32 result = ((dt1.year == dt2.year)
                  && (dt1.month == dt2.month)
                  && (dt1.day == dt2.day));
    return result;
}

B32 is_same_datetime(DateTime dt1, DateTime dt2)
{
    B32 result = ((dt1.year == dt2.year)
                  && (dt1.month == dt2.month)
                  && (dt1.day == dt2.day)
                  && (dt1.hour == dt2.hour)
                  && (dt1.minute == dt2.minute)
                  && (dt1.second == dt2.second));
    return result;
}

B32 is_weekday(Timestamp timestamp)
{
    time_t timeValue = (time_t)timestamp;
    struct tm *timeInfo = gmtime(&timestamp);
    return (timeInfo->tm_wday != 0) && (timeInfo->tm_wday != 6);
}
