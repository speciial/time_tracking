#ifndef tokenizer_H
#define tokenizer_H

#include <stdbool.h>

typedef enum
{
    Token_Number,
    Token_Slash,
    Token_OpenBrace,
    Token_CloseBrace,
    Token_OpenParen,
    Token_CloseParen,
    Token_OpenBracket,
    Token_CloseBracket,
    Token_Colon,
    Token_Semicolon,
    Token_Comma,
    Token_Underscore,
    Token_EndOfStream
} token_type;

typedef struct
{
    token_type Type;

    union {
        int IntValue;
        float FloatValue;
        char *StringValue;
    } Data;

    int TextLength;
    char *Text;
} token;

typedef struct
{
    char *At;
    int Line;
    int Char;
} tokenizer;

bool IsWhitespace(char C);

void EatAllWhitespace(tokenizer *Tokenizer);

token PeekNextToken(tokenizer *Tokenizer);

token EatNextToken(tokenizer *Tokenizer);

token PeekToken(tokenizer *Tokenizer, int Offset);

bool RequireTokenPattern(tokenizer *Tokenizer, token_type *TokenPattern, int TokenCount);

bool RequireToken(tokenizer *Tokenizer, token_type DesiredType);

bool RequireTokenAndEat(tokenizer *Tokenizer, token_type DesiredType);

#endif // tokenizer_H