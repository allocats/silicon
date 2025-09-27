#ifndef SILICON_H
#define SILICON_H

#include <stdio.h>
#include <stdlib.h>

static inline void silicon_err(const char* msg) {
    fprintf(stderr, "Silicon Error: %s\n", msg);
    exit(1);
}

#endif // !SILICON_H
