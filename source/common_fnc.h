#ifndef COMMON_FNC_H
#define COMMON_FNC_H

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int stringToInt(const char *string);

#endif // COMMON_FNC_H