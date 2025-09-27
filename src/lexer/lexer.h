#ifndef LEXER_H
#define LEXER_H

#include "../utils/arena.h"

#include "../token/token.h"

#include <stdint.h>

typedef struct {
    char* buffer;
    char* cursor;
    char* end;
    TokenArray* tokens;
    ArenaAllocator* arena;
} Lexer;

void tokenize(ArenaAllocator* arena, const char* path);

#endif // !LEXER_H
