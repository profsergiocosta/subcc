#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>

#include "mstring.h"

typedef enum { int_t, float_t, string_t } TYPE;

union m_value {
    int     int_value;
    float   float_value;
    char*   str_value;
};

struct symbol {
    const char* name;
    union m_value* val;
    TYPE t;
    int lastindex;
};

struct symcell {
    struct symbol* info;
    struct symcell* next;
};

struct symtab {
    struct symtab* parent;
    struct symcell* list;
    struct symtab* next;
    int level;
    int id;
};

// Tabela de símbolos
struct symtab* rootTable(void);
struct symtab* newTable(struct symtab* enclosing_scope);

// Inserção e busca
struct symbol* insertSymbol(struct symtab* st, char* name, TYPE t, int size);
struct symbol* newSymbol(char* name, TYPE t, int size);
struct symcell* insertSymbolCell(struct symcell* list, struct symbol* sym);
struct symbol* findSymbol(struct symtab* st, const char* name);
struct symbol* findSymbolInList(struct symcell* list, const char* name);

// Atribuição de valores
int assignSymbolSymbol(struct symbol* sym, struct symbol* other);
int assignSymbolInt(struct symbol* sym, int val);
int assignSymbolFloat(struct symbol* sym, float val);
int assignSymbolString(struct symbol* sym, char* val);

// Getters/Setters
void setValueInt(struct symbol* sym, int obj);
void setValueFloat(struct symbol* sym, float obj);
void setValueString(struct symbol* sym, char* obj);

int getInt(struct symbol* sym);
float getFloat(struct symbol* sym);
const char* getString(struct symbol* sym);

#endif
