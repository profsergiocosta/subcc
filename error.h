#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/* Códigos de erro */
enum { UNDEC, REDEC, EXPEC };

/* malloc que aborta em caso de erro */
void *fmalloc(size_t size);

/* Função para exibir mensagens de erro formatadas */
void error(int line, int code, const char *fmt, ...);

#endif /* ERROR_H */
