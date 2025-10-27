#ifndef base_datetime_HPP
#define base_datetime_HPP

#include <time.h>

namespace base
{

typedef time_t timestamp;

struct date_time
{
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
};

timestamp timestamp_now();

date_time date_time_from_timestamp(timestamp timestamp);

timestamp timestamp_from_date_time(date_time date_time);

} // namespace base

#endif base_datetime_HPP
