#ifndef base_core_h
#define base_core_h

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef S32 B32;
typedef S64 B64;

typedef float F32;
typedef double F64;

typedef U64 Timestamp;

#define KB(value) ((value) * 1024)
#define MB(value) (KB(value) * 1024)
#define GB(value) (MB(value) * 1024)

#define min_value(a, b) (((a)<(b)) ? (a) : (b))
#define max_value(a, b) (((a)>(b)) ? (a) : (b))

#endif // base_core_h