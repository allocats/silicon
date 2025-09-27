#include "ast.h"

#include "../utils/arena.h"

AstNode* new_ast(ArenaAllocator* arena, AstNode ast) {
    AstNode* ptr = arena_alloc(arena, sizeof(*ptr));
    *ptr = ast;
    return ptr;
}
