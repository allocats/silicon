#ifndef AST_H
#define AST_H

#include "../utils/arena.h"

#include <stdint.h>
#include <stddef.h>

#define NEW_AST(arena, type, ...) \
    new_ast(arena, (AstNode) {type, {.type=(struct type){__VA_ARGS__}}})

typedef enum {
    AstAssignment,
    AstInt8,
    AstInt16,
    AstInt32,
    AstInt64,
} AstType;

typedef struct AstNode AstNode ;

struct AstNode {
    AstType type;
    union {
        struct AstAssignment { const char* name; size_t size; AstNode* value; } AstAssignment;
        struct AstInt8 { int8_t value; } AstInt8;
        struct AstInt16 { int16_t value; } AstInt16;
        struct AstInt32 { int32_t value; } AstInt32;
        struct AstInt64 { int64_t value; } AstInt64;
    } data;
};  

AstNode* new_ast(ArenaAllocator* arena, AstNode ast);

#endif // !AST_H
