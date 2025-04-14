#ifndef time_and_date_H
#define time_and_date_H

#include <time.h>

typedef struct _date
{
    int Year;
    int Month;
    int Day;
} date; 

typedef struct _date_time
{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
} date_time; 

time_t TimestampFromDateTime(int Year, int Month, int Day, int Hour, int Minute);

date_time DateTimeFromTimestamp(time_t Timestamp);

#endif // time_and_date_H