#include "time_tracking.hpp"

#include <common.hpp>
#include <base_files.hpp>
#include <base_datetime.hpp>

ttr_record_list ReadRecordFile(arena *Arena, const char *Filename)
{
    ttr_record_list Result = {};

    if (FileExists(Filename))
    {
        // TODO(speciial): Make files use strings and arenas.
        string FileContent = ReadFile(Arena, Filename);
        if (FileContent.Content)
        {
            tokenizer Tokenizer = {};
            Tokenizer.At = FileContent.Content;

            ParseRecords(Arena, &Tokenizer, &Result);
            if (Tokenizer.HasError)
            {
                Result.First = 0;
                Result.Last = 0;
                Result.Count = 0;
                Result.Invalid = true;
            }
        }
        else
        {
            // TODO(speciial): What if this fails?
        }
    }
    else
    {
        // TODO(speciial): Should we log this case to the user/console?
        CreateFile(Filename);
    }

    return Result;
}

void ParseRecords(arena *Arena, tokenizer *Tokenizer, ttr_record_list *RecordList)
{
    ttr_day LastDay = {};
    ttr_record_list_node *CurrentListNode;

    bool Parsing = true;
    while (Parsing)
    {
        token Token = PeekNextToken(Tokenizer);
        if (Tokenizer->HasError || Token.Type == Token_EndOfStream)
        {
            Parsing = false;
        }
        else
        {
            ttr_day CurrentDay = ParseDay(Tokenizer);

            if (!Tokenizer->HasError)
            {
                if (CurrentDay.Year != LastDay.Year || CurrentDay.Month != LastDay.Month)
                {
                    RecordListPush(Arena, RecordList);
                    CurrentListNode = RecordList->Last;
                    CurrentListNode->Year = CurrentDay.Year;
                    CurrentListNode->Month = CurrentDay.Month;
                }
                LastDay = CurrentDay;

                ParseWorkDay(Tokenizer, CurrentListNode, CurrentDay.Year, CurrentDay.Month, CurrentDay.Day);

                RequireTokenAndEat(Tokenizer, Token_Semicolon);
                if (Tokenizer->HasError)
                {
                    Parsing = false;
                }
            }
            else
            {
                Parsing = false;
            }
        }
    }
}

void ParseWorkDay(tokenizer *Tokenizer, ttr_record_list_node *RecordListNode,
                  int Year, int Month, int Day)
{
    RequireTokenAndEat(Tokenizer, Token_OpenBrace);
    if (!Tokenizer->HasError)
    {
        RecordListNode->Records[Day].Used = true;
        RecordListNode->Records[Day].Work = ParseInterval(Tokenizer, Year, Month, Day);

        ParsePauses(Tokenizer, &RecordListNode->Records[Day], Year, Month, Day);

        ++RecordListNode->RecordCountPerMonth;

        RequireTokenAndEat(Tokenizer, Token_CloseBrace);
    }
}

void ParsePauses(tokenizer *Tokenizer, ttr_record *OutRecord,
                 int Year, int Month, int Day)
{
    RequireTokenAndEat(Tokenizer, Token_OpenBracket);

    if (!Tokenizer->HasError)
    {
        token CurrentToken = PeekNextToken(Tokenizer);
        while (!Tokenizer->HasError &&
               CurrentToken.Type != Token_EndOfStream &&
               CurrentToken.Type != Token_CloseBracket)
        {
            OutRecord->Pauses[OutRecord->PauseCount] = ParseInterval(Tokenizer, Year, Month, Day);
            ++OutRecord->PauseCount;

            CurrentToken = PeekNextToken(Tokenizer);
        }

        RequireTokenAndEat(Tokenizer, Token_CloseBracket);
    }
}

ttr_interval ParseInterval(tokenizer *Tokenizer, int Year, int Month, int Day)
{
    ttr_interval Result = {};
    RequireTokenAndEat(Tokenizer, Token_OpenParen);

    if (!Tokenizer->HasError)
    {
        Result.Start = ParseTime(Tokenizer, Year, Month, Day);

        RequireTokenAndEat(Tokenizer, Token_Comma);
        if (!Tokenizer->HasError)
        {
            token TimeOrUnderscore = PeekNextToken(Tokenizer);
            if (TimeOrUnderscore.Type == Token_Number)
            {
                Result.End = ParseTime(Tokenizer, Year, Month, Day);
            }
            else
            {
                RequireTokenAndEat(Tokenizer, Token_Underscore);
                Result.Incomplete = true;
            }
            RequireTokenAndEat(Tokenizer, Token_CloseParen);
        }
    }

    return Result;
}

ttr_day ParseDay(tokenizer *Tokenizer)
{
    ttr_day Result = {};

    token_type TokenPattern[] = { Token_Number, Token_Slash,
                                  Token_Number, Token_Slash,
                                  Token_Number };
    if (RequireTokenPattern(Tokenizer, TokenPattern, ArrayCount(TokenPattern)))
    {
        token CurrentToken = EatNextToken(Tokenizer);
        Result.Year = CurrentToken.Data.IntValue;

        EatNextToken(Tokenizer); // Skip slash
        CurrentToken = EatNextToken(Tokenizer);
        Result.Month = CurrentToken.Data.IntValue;

        EatNextToken(Tokenizer); // Skip slash
        CurrentToken = EatNextToken(Tokenizer);
        Result.Day = CurrentToken.Data.IntValue;
    }

    return Result;
}

s64 ParseTime(tokenizer *Tokenizer, int Year, int Month, int Day)
{
    s64 Result = 0;

    token_type TokenPattern[] = { Token_Number, Token_Colon, Token_Number };
    if (RequireTokenPattern(Tokenizer, TokenPattern, ArrayCount(TokenPattern)))
    {
        token CurrentToken = EatNextToken(Tokenizer);
        int Hour = CurrentToken.Data.IntValue;

        RequireTokenAndEat(Tokenizer, Token_Colon);
        CurrentToken = EatNextToken(Tokenizer);
        int Minute = CurrentToken.Data.IntValue;

        // TODO(speciial): This can fail if there is an invalid time being used!
        Result = TimestampFromDateTime(Year, Month, Day, Hour, Minute);
    }

    return Result;
}

void RecordListPush(arena *Arena, ttr_record_list *RecordList)
{
    ttr_record_list_node *NewNode = PushStruct(Arena, ttr_record_list_node);
    if (!RecordList->First && !RecordList->Last)
    {
        RecordList->First = NewNode;
        RecordList->Last = NewNode;
    }
    else
    {
        RecordList->Last->NextMonth = NewNode;
        RecordList->Last = NewNode;
    }
    RecordList->Count += 1;
}

void WriteRecordFile(arena *Arena, const char *Filename, ttr_record_list Records)
{
    string_list RecordStringList = {};

    ttr_record_list_node *CurrentMonth = Records.First;
    while (CurrentMonth)
    {
        for (int Day = 0; Day < DAYS_PER_MONTH; ++Day)
        {
            if (CurrentMonth->Records[Day].Used)
            {
                string WrittenRecord = WriteFormattedRecord(Arena, &CurrentMonth->Records[Day],
                                                            CurrentMonth->Year, CurrentMonth->Month, Day);
                StringListAppend(Arena, &RecordStringList, WrittenRecord);
            }
        }
        CurrentMonth = CurrentMonth->NextMonth;
    }
    string RecordsToWrite = StringListToString(Arena, &RecordStringList);

    WriteFile(Filename, RecordsToWrite.Content, RecordsToWrite.Length);
}

string WriteFormattedRecord(arena *Arena, ttr_record *Record, int Year, int Month, int Day)
{
    string Result = {};
    arena Scratch = NewArena(MB(1));

    // Work hours
    string WorkTimeString = WriteFormattedInterval(&Scratch, Record->Work);

    // Pause timers
    string PauseTimersString;
    for (int PauseIndex = 0; PauseIndex < Record->PauseCount; ++PauseIndex)
    {
        string PauseInterval = WriteFormattedInterval(&Scratch, Record->Pauses[PauseIndex]);

        if (PauseIndex == 0)
        {
            PauseTimersString = PauseInterval;
        }
        else
        {
            PauseTimersString = StringConcat(&Scratch, PauseTimersString, PauseInterval);
        }
    }

    char *FinalFormat = "%02d/%02d/%02d{%s[%s]};\n";
    Result = StringFormat(Arena, FinalFormat, Year, Month, Day, WorkTimeString.Content, PauseTimersString.Content);

    FreeArena(&Scratch);

    return Result;
}

string WriteFormattedInterval(arena *Arena, ttr_interval Interval)
{
    string Result = {};

    char *CompleteIntervalFormat = "(%02d:%02d,%02d:%02d)";
    char *IncompleteIntervalFormat = "(%02d:%02d,_)";

    date_time WorkStart = DateTimeFromTimestamp(Interval.Start);
    if (Interval.End != 0)
    {
        date_time WorkEnd = DateTimeFromTimestamp(Interval.End);
        Result = StringFormat(Arena, CompleteIntervalFormat,
                              WorkStart.Hour, WorkStart.Minute,
                              WorkEnd.Hour, WorkEnd.Minute);
    }
    else
    {
        Result = StringFormat(Arena, IncompleteIntervalFormat,
                              WorkStart.Hour, WorkStart.Minute);
    }

    return Result;
}
