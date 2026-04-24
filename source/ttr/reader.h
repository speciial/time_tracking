#ifndef reader_h
#define reader_h

#include <base_arena.h>
#include <base_string.h>

#include "ttr.h"

TTRHeader ttr_read_header(String fileContent);
TTR *ttr_read_entries(Arena *arena, TTRHeader header, String fileContent);

// Helpers
TimerState timer_state_from_string(String stateString);

#endif // reader_h