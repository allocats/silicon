#include "lexer.h"

#include "../utils/macros.h"
#include "../utils/silicon.h"

#include "../token/token.h"
#include "../generator/generator.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define IS_DIGIT(c) (char_map[(unsigned char)(c)] & 1)
#define IS_ALPHA(c) (char_map[(unsigned char)(c)] & 2)
#define IS_OPERATOR(c) (char_map[(unsigned char)(c)] & 4)
#define IS_DELIMITERS(c) (char_map[(unsigned char)(c)] & 8)
#define IS_WHITESPACE(c) (char_map[(unsigned char)(c)] & 16)
#define IS_STRING_DELIMS(c) (char_map[(unsigned char)(c)] & 32)

static const uint8_t char_map[256] = {
    ['0'] = 1,
    ['1'] = 1,
    ['2'] = 1,
    ['3'] = 1,
    ['4'] = 1,
    ['5'] = 1,
    ['6'] = 1,
    ['7'] = 1,
    ['8'] = 1,
    ['9'] = 1,

    ['a'] = 2,
    ['b'] = 2,
    ['c'] = 2,
    ['d'] = 2,
    ['e'] = 2,
    ['f'] = 2,
    ['g'] = 2,
    ['h'] = 2,
    ['i'] = 2,
    ['j'] = 2,
    ['k'] = 2,
    ['l'] = 2,
    ['m'] = 2,
    ['n'] = 2,
    ['o'] = 2,
    ['p'] = 2,
    ['q'] = 2,
    ['r'] = 2,
    ['s'] = 2,
    ['t'] = 2,
    ['u'] = 2,
    ['v'] = 2,
    ['w'] = 2,
    ['x'] = 2,
    ['y'] = 2,
    ['z'] = 2,

    ['A'] = 2,
    ['B'] = 2,
    ['C'] = 2,
    ['D'] = 2,
    ['E'] = 2,
    ['F'] = 2,
    ['G'] = 2,
    ['H'] = 2,
    ['I'] = 2,
    ['J'] = 2,
    ['K'] = 2,
    ['L'] = 2,
    ['M'] = 2,
    ['N'] = 2,
    ['O'] = 2,
    ['P'] = 2,
    ['Q'] = 2,
    ['R'] = 2,
    ['S'] = 2,
    ['T'] = 2,
    ['U'] = 2,
    ['V'] = 2,
    ['W'] = 2,
    ['X'] = 2,
    ['Y'] = 2,
    ['Z'] = 2,
    ['_'] = 2,
    
    ['-'] = 4,
    ['+'] = 4,
    ['/'] = 4,
    ['*'] = 4,
    ['='] = 4,
    ['!'] = 4,
    ['<'] = 4,
    ['>'] = 4,

    ['.'] = 8,
    ['['] = 8,
    [']'] = 8,
    ['('] = 8,
    [')'] = 8,
    ['{'] = 8,
    ['}'] = 8,
    [';'] = 8,
    [':'] = 8,

    [' '] = 16, 
    ['\t'] = 16, 
    ['\n'] = 16,
    ['\f'] = 16,
    ['\r'] = 16,

    ['\''] = 32,
    ['\"'] = 32,
};

static void lexer_err(Lexer* lexer, const char* msg) {
    const char* start = lexer -> buffer;
    const char* cursor = lexer -> cursor;

    size_t line = 1;
    size_t col = 1;

    while (start != cursor) {
        start++;
        col++;

        if (*start == '\n') {
            line++;
            col = 1;
        }
    }

    fprintf(stderr, "Silicon Error: %s at Line %zu:%zu\n", msg, line, col);
    exit(1);
}

static inline void advance(Lexer* lexer) {
    lexer -> cursor = ++lexer -> cursor >= lexer -> end ? lexer -> end : lexer -> cursor;
}

static inline void skip_whitespace(Lexer* lexer) {
    const char** cursor = (const char**) &lexer -> cursor;
    while (IS_WHITESPACE(**cursor)) {
        advance(lexer);
    }
}

static TokenType match_operator(const char c) {
    TokenType type;

    switch (c) {
        case '/':
            type = TokenSlash;
            break;  

        case '*':
            type = TokenAsterix;
            break;  

        case '+':
            type = TokenPlus;
            break;  

        case '-':
            type = TokenMinus;
            break;  

        case '=':
            type = TokenEquals;
            break;  

        case '!':
            type = TokenBang;
            break;  

        case '>':
            type = TokenGreaterThan;
            break;  
        
        case '<':
            type = TokenLessThan;
            break;  
    }

    return type;
}

static TokenType match_delimiter(const char c) {
    TokenType type;

    switch (c) {
        case '.':
            type = TokenDot;
            break;

        case ',':
            type = TokenComma;
            break;  

        case ':':
            type = TokenColon;
            break;  

        case ';':
            type = TokenSemiColon;
            break;  

        case '(':
            type = TokenLeftParen;
            break;  

        case ')':
            type = TokenRightParen;
            break;  

        case '[':
            type = TokenLeftSquare;
            break;  

        case ']':
            type = TokenRightSquare;
            break;  

        case '{':
            type = TokenLeftBrace;
            break;  

        case '}':
            type = TokenLeftBrace;
            break;  
    }

    return type;
}

static TokenType match_word(const char* start, const size_t len) {
    TokenType type;

    switch (*start) {
        case 'i':
            if (strncmp("i8", start, len) == 0) {
                type = TokenI8;
                break;
            } else if (strncmp("i16", start, len) == 0) {
                type = TokenI16;
                break;
            } else if (strncmp("i32", start, len) == 0) {
                type = TokenI32;
                break;
            } else if (strncmp("i64", start, len) == 0) {
                type = TokenI64;
                break;
            }
            type = TokenIdentifier;
            break;

        case 'p':
            if (strncmp("print", start, len) == 0) {
                type = TokenPrint;
                break;
            }
            type = TokenIdentifier;
            break;

        case 'r':
            if (strncmp("return", start, len) == 0) {
                type = TokenReturn;
                break;
            }
            type = TokenIdentifier;
            break;

        default:
            type = TokenIdentifier;
            break;
    }

    return type;
}

static void lex(Lexer* lexer) {
    const char** cursor = (const char**) &lexer -> cursor;
    const char** end = (const char**) &lexer -> end;

    while (*cursor != *end) {
        skip_whitespace(lexer);

        TokenType type;
        const char* start = *cursor;
        size_t len = 0;

        if (IS_ALPHA(**cursor)) {
            while (IS_ALPHA(**cursor) || IS_DIGIT(**cursor)) {
                len++;
                advance(lexer);
            }

            type = match_word(start, len);
        } else if (IS_DIGIT(**cursor)) {
            type = TokenNumber;
            while (IS_DIGIT(**cursor)) {
                len++;
                advance(lexer);

                if (IS_ALPHA(**cursor)) {
                    lexer_err(lexer, "Invalid number");
                }
            }
        } else if (IS_STRING_DELIMS(**cursor)) {
            type = TokenLiteral;
            const char delim = **cursor;
            advance(lexer);
            len++;

            while (**cursor != delim) {
                len++;
                advance(lexer);
            }
        } else if (IS_OPERATOR(**cursor)) {
            while (IS_OPERATOR(**cursor)) {
                len++;
                advance(lexer);
            }

            if (len > 2) {
                lexer_err(lexer, "Unexpected operator");
            }

            type = match_operator(*start);
        } else if (IS_DELIMITERS(**cursor)) {
            len = 1;
            advance(lexer);

            type = match_delimiter(*start);
        } else {
            skip_whitespace(lexer);
            continue;
        }

        push_token(lexer -> tokens, type, start, len);
    }
}

static void lexer_parse(ArenaAllocator* arena, char* buffer, const size_t size) {
    Lexer* lexer = arena_alloc(arena, sizeof(*lexer));

    lexer -> arena = arena;
    lexer -> buffer = buffer;
    lexer -> cursor = lexer -> buffer;
    lexer -> end = lexer -> buffer + size;
    lexer -> tokens = create_token_array(arena, 64);

    lex(lexer);

    // print_token_arr(lexer -> tokens);

    generate_asm(lexer -> tokens);
}

void tokenize(ArenaAllocator* arena, const char* path) {
    int fd = open(path, O_RDONLY); 
    if (UNLIKELY(fd == -1)) {
        silicon_err("Failed to open file");
    }

    struct stat st;
    int fstat_result = fstat(fd, &st);
    if (UNLIKELY(fstat_result == -1)) {
        close(fd);
        silicon_err("Failed to fstat file");
    }

    char* buffer = arena_alloc(arena, st.st_size + 1);
    ssize_t bytes_read = read(fd, buffer, st.st_size);
    if (UNLIKELY(bytes_read != st.st_size)) {
        close(fd);
        silicon_err("Failed to read file");
    }

    buffer[st.st_size] = 0;
    close(fd);
    lexer_parse(arena, buffer, st.st_size);
}
