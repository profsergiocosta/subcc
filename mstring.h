#ifndef MSTRING_H
#define MSTRING_H

#include <stdlib.h>
#include <stdio.h>

/**
 * Armazena a string em uma área de memória persistente e retorna ponteiro para ela.
 */
char* str(const char *s);

/**
 * Converte valores básicos para string. O buffer retornado é gerenciado internamente por str().
 */
char* intToString(int value);
char* longToString(long value);
char* doubleToString(double value);
char* uintToString(unsigned int value);
char* cstrToString(const char *value);

#endif
