#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mstring.h"

#define INCREMENT_SIZE 1000000

static char *buf = NULL;
static int buf_size = 0;
static size_t buf_avail = 0;
static char *next_name = NULL;

#define INVARIANT \
    assert(buf + buf_size == next_name + buf_avail); \
    assert(buf_size >= 0); \
    assert(buf_avail >= 0); \
    assert(next_name >= buf);

static void grow(size_t size) {
    size_t next_offset = next_name - buf;
    { INVARIANT }

    buf = (char *)realloc(buf, buf_size + size);
    if (!buf) {
        fprintf(stderr, "Cannot expand name space (%d bytes)\n", buf_size + (int)size);
        exit(1);
    }

    buf_avail += size;
    buf_size += size;
    next_name = buf + next_offset;

    { INVARIANT }
}

char* str(const char *s) {
    size_t l = strlen(s) + 1;
    char *ps;

    { INVARIANT }
    while (l > buf_avail)
        grow(INCREMENT_SIZE);

    ps = strcpy(next_name, s);
    buf_avail -= l;
    next_name += l;

    { INVARIANT }

    return ps;
}

// Funções auxiliares de conversão
char* intToString(int value) {
    char tmp[50];
    sprintf(tmp, "%d", value);
    return str(tmp);
}

char* longToString(long value) {
    char tmp[50];
    sprintf(tmp, "%ld", value);
    return str(tmp);
}

char* doubleToString(double value) {
    char tmp[50];
    sprintf(tmp, "%lf", value);
    return str(tmp);
}

char* uintToString(unsigned int value) {
    char tmp[50];
    sprintf(tmp, "%u", value);
    return str(tmp);
}

char* cstrToString(const char *value) {
    return str(value);
}
