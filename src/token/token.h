#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/arena.h"

#include <stddef.h>

typedef enum {
    TokenDot,
    TokenComma,
    TokenColon,
    TokenSemiColon,
    TokenLeftParen,
    TokenRightParen,
    TokenLeftSquare,
    TokenRightSquare,
    TokenLeftBrace,
    TokenRightBrace,

    TokenSlash,
    TokenAsterix,
    TokenPlus,
    TokenMinus,
    TokenEquals,
    TokenBang,
    TokenLessThan,
    TokenGreaterThan,

    TokenEqualsEquals,
    TokenBangEquals,
    TokenLessEquals,
    TokenGreaterEquals,

    TokenNumber,
    TokenIdentifier,
    TokenLiteral,

    TokenI8,
    TokenI16,
    TokenI32,
    TokenI64,

    TokenPrint,
    TokenReturn,

    TokenEof
} TokenType;

typedef struct {
    const TokenType type;
    const char* literal;
    const size_t len;
} Token;

typedef struct {
    Token* tokens;
    size_t count;
    size_t capacity;
    ArenaAllocator* arena;
} TokenArray;

TokenArray* create_token_array(ArenaAllocator* arena, const size_t capacity);

void push_token(TokenArray* arr, const TokenType type, const char* start, const size_t len);

void print_token_arr(TokenArray* arr);
#endif // !TOKEN_H
