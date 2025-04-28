#include "tokenizer.h"

#include <common.h>
#include <strings.h>

bool IsWhitespace(char C)
{
    bool Result = ((C == ' ') ||
                   (C == '\t') ||
                   (C == '\n') ||
                   (C == '\r'));
    return Result;
}

void EatAllWhitespace(tokenizer *Tokenizer)
{
    while (IsWhitespace(Tokenizer->At[0]))
    {
        if (Tokenizer->At[0] == '\n' && Tokenizer->At[1] == '\r')
        {
            ++Tokenizer->Line;
            Tokenizer->Char = -2;
        }
        else if (Tokenizer->At[0] == '\n')
        {
            ++Tokenizer->Line;
            Tokenizer->Char = -1;
        }

        ++Tokenizer->Char;
        ++Tokenizer->At;
    }
}

token PeekNextToken(tokenizer *Tokenizer)
{
    token Result = { 0 };
    EatAllWhitespace(Tokenizer);

    Result.TextLength = 1;
    Result.Text = Tokenizer->At;

    switch (Tokenizer->At[0])
    {
        case '\0': { Result.Type = Token_EndOfStream; } break;
        case '/': { Result.Type = Token_Slash; } break;
        case '{': { Result.Type = Token_OpenBrace; } break;
        case '}': { Result.Type = Token_CloseBrace; } break;
        case '(': { Result.Type = Token_OpenParen; } break;
        case ')': { Result.Type = Token_CloseParen; } break;
        case '[': { Result.Type = Token_OpenBracket; } break;
        case ']': { Result.Type = Token_CloseBracket; } break;
        case '_': { Result.Type = Token_Underscore; } break;
        case ':': { Result.Type = Token_Colon; } break;
        case ',': { Result.Type = Token_Comma; } break;
        case ';': { Result.Type = Token_Semicolon; } break;
        default:
        {
            // Parse Number
            char *CurrentToken = Tokenizer->At;
            int TextLength = 0;
            while (CurrentToken[0] != '\0' &&
                   (CurrentToken[0] >= '0' && CurrentToken[0] <= '9'))
            {
                ++TextLength;
                ++CurrentToken;
            }
            Result.Type = Token_Number;
            Result.Data.IntValue = StringToInt(Tokenizer->At, TextLength);
            Result.TextLength = TextLength;
        } break;
    }
    return Result;
}

token EatNextToken(tokenizer *Tokenizer)
{
    token Result = PeekNextToken(Tokenizer);
    Tokenizer->At += Result.TextLength;
    Tokenizer->Char += Result.TextLength;
    return Result;
}

token PeekToken(tokenizer *Tokenizer, int Offset)
{
    token Result = { 0 };

    tokenizer Copy = *Tokenizer;
    for (int SkipIndex = 0; SkipIndex < Offset; ++SkipIndex)
    {
        Result = EatNextToken(&Copy);
        if (Result.Type == Token_EndOfStream)
        {
            break;
        }
    }

    return Result;
}

bool RequireTokenPattern(tokenizer *Tokenizer, token_type *TokenPattern, int TokenCount)
{
    bool Result = true;

    EatAllWhitespace(Tokenizer);

    for (int TokenIndex = 0; TokenIndex < TokenCount; ++TokenIndex)
    {
        token CurrentToken = PeekToken(Tokenizer, (TokenIndex + 1));
        if (CurrentToken.Type != TokenPattern[TokenIndex])
        {
            for (int ErrorIndex = 0; ErrorIndex < (TokenIndex + 1); ++ErrorIndex)
            {
                EatNextToken(Tokenizer);
            }
            PlaceError(Tokenizer, TokenPattern[TokenIndex], CurrentToken.Type);

            Result = false;
            break;
        }
    }

    return Result;
}

bool RequireToken(tokenizer *Tokenizer, token_type DesiredType)
{
    token CurrentToken = PeekNextToken(Tokenizer);
    bool Result = CurrentToken.Type == DesiredType;
    if (!Result)
    {
        PlaceError(Tokenizer, DesiredType, CurrentToken.Type);
    }
    return Result;
}

bool RequireTokenAndEat(tokenizer *Tokenizer, token_type DesiredType)
{
    token CurrentToken = EatNextToken(Tokenizer);
    bool Result = CurrentToken.Type == DesiredType;
    if (!Result)
    {
        PlaceError(Tokenizer, DesiredType, CurrentToken.Type);
    }
    return Result;
}

void PlaceError(tokenizer *Tokenizer, token_type ExpectedToken, token_type ReceivedToken)
{
    if (!Tokenizer->HasError)
    {
        int CharLeftOffset = 4;
        int CharRightOffset = 4;
        if (Tokenizer->Char < CharLeftOffset)
        {
            CharLeftOffset = Tokenizer->Char;
        }
        for (int CharIndex = 0; CharIndex < CharRightOffset; ++CharIndex)
        {
            if (Tokenizer->At[CharIndex] == '\r' ||
                Tokenizer->At[CharIndex] == '\n' ||
                Tokenizer->At[CharIndex] == '\0')
            {
                CharRightOffset = CharIndex - 1;
                break;
            }
        }

        char FileContentBuffer[16] = { 0 };
        StringCopy(FileContentBuffer, 16, Tokenizer->At -
                   CharLeftOffset, CharLeftOffset + CharRightOffset);
        
        PrintFormatString("Error in line %d, char %d\n", Tokenizer->Line, Tokenizer->Char);
        PrintFormatString("\t%s\n", FileContentBuffer);
        PrintFormatString("\t%*s\n", CharLeftOffset, "^");
        
        // TODO(speciial): Print Token_Number properly!
        PrintFormatString("\t%*s %c %s %c\n", CharLeftOffset, "Expected", ExpectedToken, "but got", ReceivedToken);

        token CurrentToken = PeekNextToken(Tokenizer);
        while (CurrentToken.Type != Token_EndOfStream)
        {
            CurrentToken = EatNextToken(Tokenizer);
        }
        Tokenizer->HasError = true;
    }
}