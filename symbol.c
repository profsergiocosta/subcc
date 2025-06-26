#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mstring.h"
#include "error.h"
#include "symbol.h"

static struct symtab* lastTab_ = NULL;
static struct symtab* firstTab_ = NULL;

static void tablelistInsert(struct symtab* tl) {
    if (firstTab_) {
        lastTab_->next = tl;
        lastTab_ = tl;
    } else {
        firstTab_ = lastTab_ = tl;
    }
}

struct symtab* rootTable(void) {
    return firstTab_;
}

struct symtab* rootTable_(void) {
    if (!firstTab_) {
        // Se não existe raiz, cria com escopo NULL (global)
        firstTab_ = lastTab_ = newTable(NULL);
    }
    return firstTab_;
}

struct symtab* newTable(struct symtab* enclosing_scope) {
    struct symtab* st = (struct symtab*) fmalloc(sizeof(struct symtab));
    st->parent = enclosing_scope;
    st->list = NULL;
    st->next = NULL;
    st->level = (enclosing_scope) ? enclosing_scope->level + 1 : 0;
    st->id = 0;
    tablelistInsert(st);
    return st;
}


struct symbol* findSymbol(struct symtab* st, char* name) {
    while (st) {
        struct symbol* sym = findSymbolInList(st->list, name);
        if (sym) return sym;
        st = st->parent;
    }
    return NULL;
}

struct symbol* findSymbolInList(struct symcell* list, char* name) {
    while (list) {
        if (strcmp(name, list->info->name) == 0)
            return list->info;
        list = list->next;
    }
    return NULL;
}

struct symbol* insertSymbol(struct symtab* st, char* name, TYPE t, int size) {
    struct symbol* sym = newSymbol(name, t, size);
    st->list = insertSymbolCell(st->list, sym);
    return sym;
}

struct symbol* newSymbol(char* name, TYPE t, int size) {
    struct symbol* sym = (struct symbol*) fmalloc(sizeof(struct symbol));
    sym->name = str(name);
    sym->t = t;
    sym->lastindex = 0;
    if (size < 1) size = 1;
    sym->val = (union m_value*) fmalloc(sizeof(union m_value) * size);
    return sym;
}

struct symcell* insertSymbolCell(struct symcell* list, struct symbol* sym) {
    struct symcell* cell = (struct symcell*) fmalloc(sizeof(struct symcell));
    cell->info = sym;
    cell->next = list;
    return cell;
}

// Atribuições

int assignSymbolSymbol(struct symbol* sym, struct symbol* other) {
    if (!sym || !other) return 0;

    if (sym->t == int_t) {
        sym->val->int_value = (other->t == int_t) ?
            other->val->int_value :
            (int) other->val->float_value;
    } else if (sym->t == float_t) {
        sym->val->float_value = (other->t == float_t) ?
            other->val->float_value :
            (float) other->val->int_value;
    } else if (sym->t == string_t) {
        sym->val->str_value = other->val->str_value;
    }

    return 1;
}

int assignSymbolInt(struct symbol* sym, int val) {
    if (!sym || sym->t != int_t) return 0;
    sym->val->int_value = val;
    return 1;
}

int assignSymbolFloat(struct symbol* sym, float val) {
    if (!sym || sym->t != float_t) return 0;
    sym->val->float_value = val;
    return 1;
}

int assignSymbolString(struct symbol* sym, char* val) {
    if (!sym || sym->t != string_t) return 0;
    sym->val->str_value = str(val);
    return 1;
}

// Getters/Setters

void setValueInt(struct symbol* sym, int obj) {
    if (sym) sym->val->int_value = obj;
}

void setValueFloat(struct symbol* sym, float obj) {
    if (sym) sym->val->float_value = obj;
}

void setValueString(struct symbol* sym, char* obj) {
    if (sym) sym->val->str_value = str(obj);
}

int getInt(struct symbol* sym) {
    if (!sym || !sym->val) return 0;
    switch (sym->t) {
        case int_t:
            return sym->val->int_value;
        case float_t:
            return (int)sym->val->float_value;
        case string_t:
            return atoi(sym->val->str_value);
        default:
            return 0;
    }
}

float getFloat(struct symbol* sym) {
    if (!sym || !sym->val) return 0.0f;
    switch (sym->t) {
        case int_t:
            return (float)sym->val->int_value;
        case float_t:
            return sym->val->float_value;
        case string_t:
            return (float)atof(sym->val->str_value);
        default:
            return 0.0f;
    }
}

const char* getString(struct symbol* sym) {
    if (!sym || !sym->val) return "";
    switch (sym->t) {
        case int_t: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "%d", sym->val->int_value);
            return buf;
        }
        case float_t: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "%.6f", sym->val->float_value);
            return buf;
        }
        case string_t:
            return sym->val->str_value;
        default:
            return "";
    }
}

