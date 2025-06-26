#ifndef INTERPRETE_H
#define INTERPRETE_H

#include "symbol.h"

typedef enum {
    OPATR, OPSOMA, OPMULT, OPSUB, OPDIV, OPMAIS, OPMENOS, OPMOD,
    OPEQU, OPDIF, JEQZ, OPNEG, EMPTY, PRINTF, SCANF, BLOCK, BLOCKEND, JUMP,
    OPBOR, OPBAND, OPMAIOR, OPMENOR, OPMAIORIG, OPMENORIG, ACESSIND
} OPCODE;

struct instruction {
    int op_code;
    char* args[2];
    char* result;
    struct instruction* next;
};

// Cria instrução (se codinst == 0, gera próximo código)
int gen3ai(int op_code, const char* arg1, const char* arg2, const char* result, int codinst);

// Interpretador executa a tabela de símbolos
void interprete(struct symtab* st);

int nextInstructionCode();

// Atualiza instrução com endereço alvo (backpatch)
void backpatch(int codins, const char* arg2);

void disassemble() ;
#endif
