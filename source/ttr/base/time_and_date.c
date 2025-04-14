#include "time_and_date.h"


time_t TimestampFromDateTime(int Year, int Month, int Day, int Hour, int Minute)
{
    time_t Result = 0;

    struct tm ConstructedTm = { 0 };
    ConstructedTm.tm_year = Year + (2000 - 1900);
    ConstructedTm.tm_mon = (Month - 1);
    ConstructedTm.tm_mday = Day;
    ConstructedTm.tm_hour = Hour;
    ConstructedTm.tm_min = Minute;
    
    Result = mktime(&ConstructedTm);
    return Result;
}

date_time DateTimeFromTimestamp(time_t Timestamp)
{
    date_time Result = {0};
    return Result;
}
