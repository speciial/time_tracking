#ifndef base_datetime_h
#define base_datetime_h

#include "base_core.h"

typedef struct DateTime DateTime;
struct DateTime
{
    U16 year;
    U16 month;
    U16 day;
    U16 hour;
    U16 minute;
    U16 second;
};

Timestamp get_current_timestamp();
Timestamp timestamp_from_datetime(DateTime dateTime);
DateTime datetime_from_timestamp(Timestamp timestamp);

#endif // base_datetime_h