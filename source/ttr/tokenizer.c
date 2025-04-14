#include "tokenizer.h"

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

    for (int TokenIndex = 0; TokenIndex < TokenCount; ++TokenIndex)
    {
        token CurrentToken = PeekToken(Tokenizer, (TokenIndex + 1));
        if (CurrentToken.Type != TokenPattern[TokenIndex])
        {
            Result = false;
            break;
        }
    }

    return Result;
}

bool RequireToken(tokenizer *Tokenizer, token_type DesiredType)
{
    // TODO(speciial): Write tokenizer error and exit program
    token Token = PeekNextToken(Tokenizer);
    return (Token.Type == DesiredType);
}

bool RequireTokenAndEat(tokenizer *Tokenizer, token_type DesiredType)
{
    // TODO(speciial): Write tokenizer error and exit program
    token Token = EatNextToken(Tokenizer);
    return (Token.Type == DesiredType);
}