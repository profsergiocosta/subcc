#ifndef LEXICAL_H
#define LEXICAL_H

enum {
    KW_MAIN, KW_INT, KW_FLOAT, KW_IF, KW_ELSE, KW_WHILE, KW_PRINTF, KW_SCANF,
    TK_IDENT, TK_INT, TK_FLOAT, TK_STRING, TK_MAIS, TK_MENOS, TK_VEZES, TK_DIVISAO, TK_PTO, TK_VIRG,
    TK_PTOVIRG, TK_ATRIBUICAO, TK_IGUAL, TK_DIF, TK_NEG, TK_MENOR, TK_MENORIG, TK_MAIOR, TK_MAIORIG, TK_PTOPTO,
    TK_APAREN, TK_FPAREN, TK_ACOLC, TK_FCOLC, TK_ACHAVE, TK_FCHAVE, TK_SLASH, TK_SLSL, TK_PORCENT,
    TK_ENDER, TK_AND, TK_OR, TK_EOF, TK_ERROR, TK_UNDEFINED, TK_COMMENT
};

struct Token {
    int type;
    char* value;
    int line;
};

void init();
int  openFile(const char *filename);
void closeFile();
struct Token* nextToken();

#endif /* LEXICAL_H */
