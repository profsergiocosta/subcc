#include "error.h"

void error(int line, int code, const char *fmt, ...) {
    va_list args;

    fputs("Encontrado erro", stderr);

    if (line != 0)
        fprintf(stderr, " na linha %d", line);

    fprintf(stderr, ": '");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "'");

    switch (code) {
        case UNDEC: fputs(" ainda não declarado!\n", stderr); break;
        case REDEC: fputs(" previamente declarado!\n", stderr); break;
        case EXPEC: fputs(" esperado!\n", stderr); break;
        default: fputs(" código de erro desconhecido.\n", stderr); break;
    }

    exit(1);
}

void *fmalloc(size_t s) {
    void *ptr = malloc(s);

    if (!ptr) {
        fprintf(stderr, "Out of memory in fmalloc(%zu)\n", s);
        exit(1);
    }

    return ptr;
}
