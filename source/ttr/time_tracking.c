#include "time_tracking.h"

#include "tokenizer.h"

#include <files.h>
#include <time_and_date.h>

// TODO(speciial): Use stdint 

// TODO(speciial): Move this to base
#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

// TODO(speciial): Move this to base
int StringToInt(char *String, int Size)
{
    int Index = 0;
    while (*String == '0' && Index < Size)
    {
        ++String;
        ++Index;
    }

    char *End = (char *)(String + Size);
    int Result = strtol(String, &End, 10);
    return Result;
}

time_t ParseTime(tokenizer *Tokenizer, int Year, int Month, int Day)
{
    token CurrentToken = EatNextToken(Tokenizer);
    int Hour = CurrentToken.Data.IntValue;

    EatNextToken(Tokenizer); // Skip colon
    CurrentToken = EatNextToken(Tokenizer);
    int Minute = CurrentToken.Data.IntValue;

    return TimestampFromDateTime(Year, Month, Day, Hour, Minute);
}

ttr_day ParseDay(tokenizer *Tokenizer)
{
    ttr_day Result = { 0 };
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

ttr_interval ParseInterval(tokenizer *Tokenizer, int Year, int Month, int Day)
{
    ttr_interval Result = { 0 };
    RequireTokenAndEat(Tokenizer, Token_OpenParen);

    token_type TokenPattern[] = { Token_Number, Token_Colon, Token_Number };
    if (RequireTokenPattern(Tokenizer, TokenPattern, ArrayCount(TokenPattern)))
    {
        Result.Start = ParseTime(Tokenizer, Year, Month, Day);

        RequireTokenAndEat(Tokenizer, Token_Comma);
        if (RequireTokenPattern(Tokenizer, TokenPattern, ArrayCount(TokenPattern)))
        {
            Result.End = ParseTime(Tokenizer, Year, Month, Day);
        }
        else
        {
            RequireTokenAndEat(Tokenizer, Token_Underscore);
        }
    }
    else
    {
        // TODO(speciial): Handle error 
    }

    RequireTokenAndEat(Tokenizer, Token_CloseParen);
    return Result;
}

void ParsePauses(tokenizer *Tokenizer, ttr_record *OutRecord,
                 int Year, int Month, int Day)
{
    RequireTokenAndEat(Tokenizer, Token_OpenBracket);

    token CurrentToken = PeekNextToken(Tokenizer);
    while (CurrentToken.Type != Token_CloseBracket)
    {
        OutRecord->Pauses[OutRecord->PauseCount] = ParseInterval(Tokenizer, Year, Month, Day);
        ++OutRecord->PauseCount;

        CurrentToken = PeekNextToken(Tokenizer);
    }

    RequireTokenAndEat(Tokenizer, Token_CloseBracket);
}

void ParseRecord(tokenizer *Tokenizer, ttr_record_list *RecordList,
                 int Year, int Month, int Day)
{
    RequireTokenAndEat(Tokenizer, Token_OpenBrace);

    RecordList->Records[Day].Used = true;
    RecordList->Records[Day].Work = ParseInterval(Tokenizer, Year, Month, Day);

    ParsePauses(Tokenizer, &RecordList->Records[Day], Year, Month, Day);

    ++RecordList->RecordCountPerMonth;

    RequireTokenAndEat(Tokenizer, Token_CloseBrace);
}

void ParseMonth(tokenizer *Tokenizer, ttr_record_list *RecordList,
                int Year, int Month)
{
    tokenizer Copy = *Tokenizer;
    ttr_day CurrentDay = ParseDay(&Copy);

    while ((CurrentDay.Year == Year) && (CurrentDay.Month == Month))
    {
        ParseRecord(&Copy, RecordList,
                    CurrentDay.Year, CurrentDay.Month, CurrentDay.Day);

        RequireTokenAndEat(&Copy, Token_Semicolon);

        // Sync Tokenizer while we want to advance
        Tokenizer->At = Copy.At;
        Tokenizer->Line = Copy.Line;
        Tokenizer->Char = Copy.Char;

        // This call shouldn't advance the actual tokenizer so we can still use it 
        // for the next month. 
        CurrentDay = ParseDay(&Copy);
    }
}

ttr_record_list *ReadRecordFile(arena *Arena, const char *Filename)
{
    ttr_record_list *Result = PushStruct(Arena, ttr_record_list, 1);

    if (FileExists(Filename))
    {
        file_content Content = { 0 };
        if (ReadFile(&Content, Filename))
        {
            tokenizer Tokenizer = { 0 };
            Tokenizer.At = Content.Content;

            int CurrentYear = 0;
            int CurrentMonth = 0;
            ttr_record_list *CurrentList = Result;
            bool FirstMonth = true;

            bool Parsing = true;
            while (Parsing)
            {
                token Token = PeekNextToken(&Tokenizer);
                if (Token.Type == Token_EndOfStream)
                {
                    Parsing = false;
                }
                else
                {
                    token_type TokenPattern[] = { Token_Number, Token_Slash,
                                                  Token_Number, Token_Slash,
                                                  Token_Number };
                    if (RequireTokenPattern(&Tokenizer, TokenPattern, ArrayCount(TokenPattern)))
                    {
                        token YearToken = PeekToken(&Tokenizer, 1);
                        token MonthToken = PeekToken(&Tokenizer, 3);

                        // NOTE(speciial): We could check if we're actually skip to the next month
                        if (CurrentYear != YearToken.Data.IntValue ||
                            CurrentMonth != MonthToken.Data.IntValue)
                        {
                            CurrentYear = YearToken.Data.IntValue;
                            CurrentMonth = MonthToken.Data.IntValue;

                            if (FirstMonth)
                            {
                                FirstMonth = false;
                            }
                            else
                            {
                                CurrentList->NextMonth = PushStruct(Arena, ttr_record_list, 1);
                                CurrentList = CurrentList->NextMonth;
                            }

                            CurrentList->Year = CurrentYear;
                            CurrentList->Month = CurrentMonth;
                        }

                        ParseMonth(&Tokenizer, CurrentList, CurrentYear, CurrentMonth);
                    }
                    else
                    {
                        Parsing = false;
                    }
                }
            }
            FreeFileContent(&Content);
        }
        else
        {
            // TODO(speciial): What if this fails?
        }
    }
    else
    {
        printf("Creating new record file.\n");
        FileCreate(Filename);
    }

    return Result;
}

void WriteRecordFile(arena *Arena, const char *Filename, ttr_record_list *Records)
{

}
