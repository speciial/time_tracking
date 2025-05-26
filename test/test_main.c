#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>

#include <common.h>
#include <arena.h>
#include <time_tracking.h>
#include <tokenizer.h>

void TestReadRecordFile()
{
    /*
        Test cases:
            - Read empty file
            - Read file with one record
            - Read file with many records
            - Read file with broken records
    */

    arena Arena = NewArena(MB(1));

    // Read empty file
    const char *EmptyRecordFileName = "empty_record.ttr";
    ttr_record_list EmptyRecords = ReadRecordFile(&Arena, EmptyRecordFileName);
    assert(0 == EmptyRecords.Count);
    assert(0 == EmptyRecords.First);
    assert(0 == EmptyRecords.Last);
    assert(false == EmptyRecords.Invalid);

    // Read file with single record
    const char *SingleRecordFileName = "single_record.ttr";
    ttr_record_list SingleRecord = ReadRecordFile(&Arena, SingleRecordFileName);
    assert(1 == SingleRecord.Count);
    assert(SingleRecord.First == SingleRecord.Last);
    assert(false == SingleRecord.Invalid);
    assert(1 == SingleRecord.First->RecordCountPerMonth);
    assert(25 == SingleRecord.First->Year);
    assert(3 == SingleRecord.First->Month);
    assert(true == SingleRecord.First->Records[23].Used);
    assert(2 == SingleRecord.First->Records[23].PauseCount);
    assert(1742712720 == SingleRecord.First->Records[23].Work.Start);

    // Read file with multiple records
    const char *MultipleRecordFileName = "multiple_record.ttr";
    ttr_record_list MultipleRecord = ReadRecordFile(&Arena, MultipleRecordFileName);
    assert(3 == MultipleRecord.Count);
    assert(MultipleRecord.First != MultipleRecord.Last);
    assert(MultipleRecord.Last == MultipleRecord.First->NextMonth->NextMonth);
    assert(false == MultipleRecord.Invalid);
    assert(0 == MultipleRecord.First->Records[28].PauseCount);
    assert(4 == MultipleRecord.First->RecordCountPerMonth);
    assert(5 == MultipleRecord.First->NextMonth->RecordCountPerMonth);
    assert(2 == MultipleRecord.First->NextMonth->NextMonth->RecordCountPerMonth);
    assert(0 == MultipleRecord.First->NextMonth->Records[8].PauseCount);
    assert(0 == MultipleRecord.First->NextMonth->Records[13].PauseCount);
    assert(true == MultipleRecord.Last->Records[2].Work.Incomplete);
    assert(true == MultipleRecord.Last->Records[2].Pauses[0].Incomplete);

    // Read file with broken records
    const char *BrokenRecordFileName = "broken_record.ttr";
    ttr_record_list BrokenRecord = ReadRecordFile(&Arena, BrokenRecordFileName);
    assert(0 == BrokenRecord.Count);
    assert(true == BrokenRecord.Invalid);

    // TODO(speciial): Free arena
}

void TestReadRecordFileInternal()
{
    /*
        Test cases:
            - Missing Day
            - Missing Paren, Braces, Brackets
            - Missing Time / Underscore
            - Unexpected character
    */

    // NOTE(speciial): These cases test internal behavior but don't represent 
    // the API from the users POV.  

    // TODO(speciial): The error reporting for this parser isn't all that good.
    // There are many edge cases where it's not accurately placing the indicator
    // or where it expects a different kind of token. 
    // For now, I can live with that but once I get around to writing something 
    // more robust, I should take a close look at that.

    arena Arena = NewArena(MB(1));
    tokenizer Tokenizer = { 0 };
    ttr_record_list RecordList = { 0 };

    // Missing Day
    Tokenizer.At = "{(07:52,16:52)[(08:45,09:00)(11:30,12:00)]};";
    ParseRecords(&Arena, &Tokenizer, &RecordList);

    assert(Tokenizer.HasError);
    assert(0 == Tokenizer.Error.Char);
    assert(0 == Tokenizer.Error.Line);
    assert(Token_Number == Tokenizer.Error.ExpectedToken);
    assert(Token_OpenBrace == Tokenizer.Error.ReceivedToken);

    // Missing Paren, Braces, Brackets
    memset(&Tokenizer, 0, sizeof(tokenizer));
    Tokenizer.At = "25/03/23{07:52,16:52)[(08:45,09:00)(11:30,12:00)]};";
    ParseRecords(&Arena, &Tokenizer, &RecordList);
    assert(Tokenizer.HasError);
    // assert(11 == Tokenizer.Error.Char);
    // assert(0 == Tokenizer.Error.Line);

    memset(&Tokenizer, 0, sizeof(tokenizer));
    Tokenizer.At = "25/03/23{(07:52,16:52)[(08:45,09:00)(11:30,12:00)};";
    ParseRecords(&Arena, &Tokenizer, &RecordList);
    assert(Tokenizer.HasError);
    // assert(50 == Tokenizer.Error.Char);
    // assert(0 == Tokenizer.Error.Line);

    memset(&Tokenizer, 0, sizeof(tokenizer));
    Tokenizer.At = "25/03/23{(07:52,16:52)[(08:45,09:00)(11:30,12:00)];";
    ParseRecords(&Arena, &Tokenizer, &RecordList);
    assert(Tokenizer.HasError);
    // assert(51 == Tokenizer.Error.Char);
    // assert(0 == Tokenizer.Error.Line);

}

TestReadRecordFileStressTest()
{
    // TODO(speciial): Implement
}


void TestWriteRecordFile()
{
    // TODO(speciial): Implement
}

TestWriteRecrodFileStressTest()
{
    // TODO(speciial): Implement
}

int main()
{
    printf("Time Tracking Test\n");

    TestReadRecordFile();

    TestReadRecordFileInternal();

    TestReadRecordFileStressTest();

    TestWriteRecordFile();

    TestWriteRecrodFileStressTest();

    return 0;
}