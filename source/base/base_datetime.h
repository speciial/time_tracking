#ifndef base_datetime_h
#define base_datetime_h

#include "base_core.h"

#define SECONDS(value) (value)
#define MINUTES(value) (SECONDS(value) * 60)
#define HOURS(value)   (MINUTES(value) * 60)

// TODO(speciial): this should at some point be reworked to use os
// specifc calls where i can ensure types have certain sizes and
// actually represent what i want them to be.

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
DateTime get_current_datetime();
Timestamp timestamp_from_datetime(DateTime dateTime);
DateTime datetime_from_timestamp(Timestamp timestamp);

S64 timestamp_diff_seconds(Timestamp end, Timestamp begin);

#endif // base_datetime_h