#include "generator.h"

#include "../utils/silicon.h"
#include "../ast/ast.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void gen_assignment(FILE* fptr, AstNode* ast) {
    size_t size = ast -> data.AstAssignment.size;
    int32_t value = ast -> data.AstAssignment.value -> data.AstInt32.value;

    fprintf(fptr, "\tsub rsp, %zu\n", size);
    fprintf(fptr, "\tmov [rsp-%zu], word %d\n", size, value);
    fprintf(fptr, "\tmov rdi, [rsp-%zu]\n", size);
    fprintf(fptr, "\tadd rsp, %zu\n\n", size);
}

void generate_asm(TokenArray* tokens) {
    size_t i = 0;
    const size_t count = tokens -> count;

    size_t size = 8;
    const char* name = NULL;
    int32_t value = 0;

    while (i < count) {
        Token token = tokens -> tokens[i];

        switch (token.type) {
            case TokenI32:
                size = 4;
                break;

            case TokenIdentifier:
                name = token.literal;
                break;

            case TokenNumber:
                value = atoi(token.literal);
                break;

            default:
                break;
        }

        i++;
    }


    ArenaAllocator* arena = tokens -> arena;
    AstNode* ast = NEW_AST(arena, AstAssignment, name, size, NEW_AST(arena, AstInt32, value)); 

    char* start = 
        "global _start\n"
        "\n_start:\n"
        "\0"
    ;

    FILE* fptr = fopen("output/output.asm", "w+");
    if (!fptr) {
        silicon_err("Failed to create output/output.asm");
    }

    fprintf(fptr, "%s", start);

    gen_assignment(fptr, ast);

    char* end = 
        "\tmov rax, 60\n"
        "\tsyscall"
        "\0"
    ;

    fprintf(fptr, "%s", end);
    fclose(fptr);

    system("nasm -f elf64 output/output.asm -o output/main.o");
    system("ld output/main.o -o output/main");
}
