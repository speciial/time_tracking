#include <stdio.h>

#include <base_arena.h>
#include <base_string.h>

#include <ttr.h>
#include <reader.h>

#include <assert.h>

void test_read_header_empty()
{
    String headerContent = str_lit("");
    TTRHeader header = ttr_read_header(headerContent);

    assert(0 == header.version);
    assert(0 == header.entryCount);
}

void test_read_header_version_only()
{
    String headerContent = str_lit("<version=2;>");
    TTRHeader header = ttr_read_header(headerContent);

    assert(2 == header.version);
    assert(0 == header.entryCount);
}

void test_read_header_entries_only()
{
    String headerContent = str_lit("<entries=13;>");
    TTRHeader header = ttr_read_header(headerContent);

    assert(0 == header.version);
    assert(13 == header.entryCount);
}

void test_read_header_unknown_only()
{
    String headerContent = str_lit("<bla=13;>");
    TTRHeader header = ttr_read_header(headerContent);

    assert(0 == header.version);
    assert(0 == header.entryCount);
}

void test_read_entries_empty(Arena *arena)
{
    String fileContent = str_lit("<version=2;entries=0;>");
    TTRHeader header = ttr_read_header(fileContent);

    TTR *ttr = ttr_read_entries(arena, header, fileContent, 1);

    assert(0 != ttr->capacity);
    assert(0 != ttr->records);
    assert(0 == ttr->count);
}

void test_read_entries_single_row(Arena *arena)
{
    String fileContent = str_lit("<version=2;entries=1;>\n0:1773058784,1773058784,0,0,TIMER_STATE_ENDED,TTR_WORK_LOCATION_OFFICE,\"Some Message\";");
    TTRHeader header = ttr_read_header(fileContent);

    TTR *ttr = ttr_read_entries(arena, header, fileContent, 1);

    assert(0 != ttr->capacity);
    assert(0 != ttr->records);
    assert(1 == ttr->count);

    assert(1773058784 == ttr->records[0].timer.start);
    assert(1773058784 == ttr->records[0].timer.end);
    assert(0 == ttr->records[0].timer.lastPause);
    assert(0 == ttr->records[0].timer.totalPauseTimeSeconds);
    assert(TIMER_STATE_ENDED == ttr->records[0].timer.state);
    assert(string_equals(str_lit("Some Message"), ttr->records[0].message));
}

void test_read_entries_multiple_rows(Arena *arena)
{
    String fileContent = str_lit("<version=2;entries=2;>\n0:1773058784,1773058784,0,0,TIMER_STATE_ENDED,TTR_WORK_LOCATION_OFFICE,\"Some Message\";\n1:1773385216,0,0,0,TIMER_STATE_STARTED,TTR_WORK_LOCATION_OFFICE,\"Text goes here\";");
    TTRHeader header = ttr_read_header(fileContent);

    TTR *ttr = ttr_read_entries(arena, header, fileContent, 1);

    assert(0 != ttr->capacity);
    assert(0 != ttr->records);
    assert(2 == ttr->count);

    assert(1773385216 == ttr->records[1].timer.start);
    assert(0 == ttr->records[1].timer.end);
    assert(0 == ttr->records[1].timer.lastPause);
    assert(0 == ttr->records[1].timer.totalPauseTimeSeconds);
    assert(TIMER_STATE_STARTED == ttr->records[1].timer.state);
    assert(string_equals(str_lit("Text goes here"), ttr->records[1].message));
}

int test_reader(int argc, char **argv)
{
    printf("Test Reader\n");

    Arena arena = arena_create(MB(20));

    test_read_header_empty();
    test_read_header_version_only();
    test_read_header_entries_only();
    test_read_header_unknown_only();

    test_read_entries_empty(&arena);
    test_read_entries_single_row(&arena);
    test_read_entries_multiple_rows(&arena);

    return 0;
}