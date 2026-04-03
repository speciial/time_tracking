#ifndef base_file_h
#define base_file_h

#include "base_core.h"
#include "base_string.h"

/*
    Things I still want to do:
    -[ ] Offset reads/writes?
    -[ ] Safe writing
    -[ ] File watching
*/

B32 file_read(String filePath, U8 *buffer, U64 size);
B32 file_write(String filePath, U8 *buffer, U64 size);

U64 file_size(String filePath);
B32 file_exists(String filePath);
B32 file_create(String filePath);

#endif // base_file_h