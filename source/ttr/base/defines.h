#ifndef defines_H
#define defines_H

// TODO(speciial): Use stdint 

#define KB(Value) ((Value) * 1024)
#define MB(Value) (KB(Value) * 1024)
#define GB(Value) (MB(Value) * 1024)

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#endif // defines_H