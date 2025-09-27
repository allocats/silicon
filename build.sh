#!/usr/bin/env bash

rm -rvf build &>/dev/null
mkdir -p build/bin &>/dev/null 

CFLAGS="-Wall -Wextra -Werror -pedantic -O3 -flto -march=native"

clang $CFLAGS -c src/main.c -o build/main.o
clang $CFLAGS -c src/lexer/lexer.c -o build/lexer.o
clang $CFLAGS -c src/token/token.c -o build/token.o
clang $CFLAGS -c src/ast/ast.c -o build/ast.o
clang $CFLAGS -c src/generator/generator.c -o build/generator.o

clang $CFLAGS build/token.o build/ast.o build/generator.o build/lexer.o build/main.o src/lib/libarena.a -o build/bin/silicon
