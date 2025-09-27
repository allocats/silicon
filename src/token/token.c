#include "token.h"

#include "../utils/arena.h"
#include "../utils/macros.h"
#include "../utils/silicon.h"

#define T_ARR_DEFAULT_CAPACITY 128
#define GROWTH_SCALE 1.5

void push_token(TokenArray* arr, const TokenType type, const char* start, const size_t len) {
    if (UNLIKELY(start == NULL)) {
        silicon_err("Passed NULL into push_token()");
    }

    Token token = { type, start, len};

    if (UNLIKELY(arr -> count >= arr -> capacity)) {
        const size_t old_size = sizeof(Token) * arr -> capacity;
        const size_t new_size = old_size * GROWTH_SCALE;

        arena_realloc(arr -> arena, arr -> tokens, old_size, new_size);
        arr -> capacity = new_size;
    }

    arena_memcpy(&arr -> tokens[arr -> count++], &token, sizeof(token));
}

TokenArray* create_token_array(ArenaAllocator* arena, const size_t capacity) {
    TokenArray* arr = arena_alloc(arena, sizeof(*arr));

    arr -> count = 0;
    arr -> capacity = capacity == 0 ? T_ARR_DEFAULT_CAPACITY : capacity;
    arr -> tokens = arena_array_zero(arena, Token, arr -> capacity);
    arr -> arena = arena;

    return arr;
}

static const char* get_type(TokenType type) {
    switch (type) {
        case TokenDot: return "TokenDot";
        case TokenComma: return "TokenComma";
        case TokenColon: return "TokenColon";
        case TokenSemiColon: return "TokenSemiColon";
        case TokenLeftParen: return "TokenLeftParen";
        case TokenRightParen: return "TokenRightParen";
        case TokenLeftSquare: return "TokenLeftSquare";
        case TokenRightSquare: return "TokenRightSquare";
        case TokenLeftBrace: return "TokenLeftBrace";
        case TokenRightBrace: return "TokenRightBrace";
        case TokenSlash: return "TokenSlash";
        case TokenAsterix: return "TokenAsterix";
        case TokenPlus: return "TokenPlus";
        case TokenMinus: return "TokenMinus";
        case TokenEquals: return "TokenEquals";
        case TokenBang: return "TokenBang";
        case TokenLessThan: return "TokenLessThan";
        case TokenGreaterThan: return "TokenGreaterThan";
        case TokenEqualsEquals: return "TokenEqualsEquals";
        case TokenBangEquals: return "TokenBangEquals";
        case TokenLessEquals: return "TokenLessEquals";
        case TokenGreaterEquals: return "TokenGreaterEquals";
        case TokenNumber: return "TokenNumber";
        case TokenIdentifier: return "TokenIdentifier";
        case TokenLiteral: return "TokenLiteral";
        case TokenI8: return "TokenI8";
        case TokenI16: return "TokenI16";
        case TokenI32: return "TokenI32";
        case TokenI64: return "TokenI64";
        case TokenPrint: return "TokenPrint";
        case TokenReturn: return "TokenReturn";
        case TokenEof:  return "TokenEof";
    }
}

void print_token_arr(TokenArray* arr) {
    printf("=== Tokens ===\n\n");

    for (size_t i = 0; i < arr -> count; i++) {
        Token token = arr -> tokens[i];
        printf("Token %zu:\n", i);
        printf("\tType: %s\n", get_type(token.type));
        printf("\tLiteral: %.*s\n", (int) token.len, token.literal);
        printf("\tLen: %zu\n", token.len);
    }

    printf("\n=== End Tokens ===\n\n");
}
