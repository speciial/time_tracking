#ifndef tokenizer_H
#define tokenizer_H

typedef enum
{
    Token_Slash = '/',
    Token_OpenBrace = '{',
    Token_CloseBrace = '}',
    Token_OpenParen = '(',
    Token_CloseParen = ')',
    Token_OpenBracket = '[',
    Token_CloseBracket = ']',
    Token_Colon = ':',
    Token_Semicolon = ';',
    Token_Comma = ',',
    Token_Underscore = '_',
    
    Token_Number = 256,
    Token_EndOfStream = 257
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

typedef struct _tokenizer_error
{
    char *At;
    int Line;
    int Char;

    token_type ExpectedToken;
    token_type ReceivedToken;
} tokenizer_error;

typedef struct _tokenizer
{
    char *At;
    int Line;
    int Char;

    bool HasError;
    tokenizer_error Error;
} tokenizer;

bool IsWhitespace(char C);

void EatAllWhitespace(tokenizer *Tokenizer);

token PeekNextToken(tokenizer *Tokenizer);

token EatNextToken(tokenizer *Tokenizer);

token PeekToken(tokenizer *Tokenizer, int Offset);

bool RequireTokenPattern(tokenizer *Tokenizer, token_type *TokenPattern, int TokenCount);

bool RequireToken(tokenizer *Tokenizer, token_type DesiredType);

bool RequireTokenAndEat(tokenizer *Tokenizer, token_type DesiredType);

// TODO(speciial): Move error handling to the usage code!
// void PlaceError(tokenizer *Tokenizer, token_type ExpectedToken, token_type ReceivedToken);

#endif // tokenizer_H