#include "lexer/lexer.h"

#include "utils/arena.h"
#include "utils/silicon.h"

static ArenaAllocator arena = {0};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        silicon_err("Expected 1 input file");
    }
    
    init_arena(&arena, 0);

    tokenize(&arena, argv[1]);
}
