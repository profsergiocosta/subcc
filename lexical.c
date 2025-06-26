#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mstring.h"
#include "error.h"
#include "lexical.h"

#define KWLIST_SZ 8
#define MAXTOKEN 64

char *kwlist[KWLIST_SZ] = {
    "main", "int", "float", "if", "else", "while", "printf", "scanf"
};

char look_;
int line_;
FILE *inputfile;

char tokvalue_[MAXTOKEN + 1];

int lookup(const char *w);
int nextChar();
void getNoBlank();
void getNoComment(const char *t);

Token *newToken(int tk, const char *value);
Token *getNum();
Token *getAlpha();
Token *getOper();
Token *scanString();

int isOp(char c);

// Inicialização
void init() {
    tokvalue_[0] = '\0';
    look_ = 0;
    line_ = 1;
    nextChar();
}

// Busca palavra-chave
int lookup(const char *w) {
    int i;
    for (i = 0; i < KWLIST_SZ; i++) {
        if (strcmp(kwlist[i], w) == 0)
            return i;
    }
    return -1;
}

// Criação de novo token
Token *newToken(int tk, const char *value) {
    Token *token = (Token *) fmalloc(sizeof(Token));
    token->type = tk;
    token->line = line_;
    token->value = str(value);  // Usa o buffer global de mstring
    return token;
}

// Leitura do próximo token
Token *nextToken() {
    if (!feof(inputfile)) {
        tokvalue_[0] = '\0';
        getNoBlank();

        if (isdigit(look_)) return getNum();
        if (isalpha(look_)) return getAlpha();
        if (isOp(look_)) return getOper();
        if (look_ == '\n') return newToken(TK_EOF, "eof");

        return newToken(TK_ERROR, "error");
    }
    return newToken(TK_EOF, "eof");
}

Token *getAlpha() {
    int len = 0;
    tokvalue_[0] = '\0';

    while (!feof(inputfile) && (look_ == '_' || isalnum(look_))) {
        if (len < MAXTOKEN) {
            tokvalue_[len++] = look_;
            tokvalue_[len] = '\0';
        }
        nextChar();
    }

    int kw = lookup(tokvalue_);
    int tk = (kw == -1) ? TK_IDENT : kw;

    return newToken(tk, tokvalue_);
}

Token *scanString() {
    int csp = 0;
    int len = 0;
    tokvalue_[0] = '\0';

    while (look_ != '"') {
        if (feof(inputfile))
            error(0, EXPEC, "\"");

        if (look_ == '\\') {
            csp = 1;
        } else {
            char ch = look_;
            if (csp) {
                switch (look_) {
                    case 'n': ch = '\n'; break;
                    case 't': ch = '\t'; break;
                    default: break;
                }
                csp = 0;
            }

            if (len < MAXTOKEN) {
                tokvalue_[len++] = ch;
                tokvalue_[len] = '\0';
            }
        }
        nextChar();
    }
    nextChar();
    return newToken(TK_STRING, tokvalue_);
}

Token *getNum() {
    int tk = TK_INT;
    int len = 0;
    tokvalue_[0] = '\0';

    while (!feof(inputfile) && (isdigit(look_) || (look_ == '.' && tk == TK_INT))) {
        if (look_ == '.') tk = TK_FLOAT;

        if (len < MAXTOKEN) {
            tokvalue_[len++] = look_;
            tokvalue_[len] = '\0';
        }
        nextChar();
    }
    return newToken(tk, tokvalue_);
}

Token *getOper() {
    int tk = TK_ERROR;
    char tmp[3] = {0};
    tmp[0] = look_;
    tmp[1] = '\0';

    switch (look_) {
        case '(': tk = TK_APAREN; break;
        case ')': tk = TK_FPAREN; break;
        case '+': tk = TK_MAIS; break;
        case '-': tk = TK_MENOS; break;
        case '*': tk = TK_VEZES; break;
        case '%': tk = TK_PORCENT; break;
        case ',': tk = TK_VIRG; break;
        case ';': tk = TK_PTOVIRG; break;
        case '{': tk = TK_ACHAVE; break;
        case '}': tk = TK_FCHAVE; break;
        case '[': tk = TK_ACOLC; break;
        case ']': tk = TK_FCOLC; break;

        case '|':
            nextChar();
            if (look_ == '|') {
                strcpy(tmp, "||");
                tk = TK_OR;
                nextChar();
            } else return newToken(TK_ERROR, tmp);
            break;

        case '&':
            nextChar();
            if (look_ == '&') {
                strcpy(tmp, "&&");
                tk = TK_AND;
                nextChar();
            } else {
                tk = TK_ENDER;
                return newToken(tk, tmp);
            }
            break;

        case '=':
            nextChar();
            if (look_ == '=') {
                strcpy(tmp, "==");
                tk = TK_IGUAL;
                nextChar();
            } else {
                tk = TK_ATRIBUICAO;
                return newToken(tk, tmp);
            }
            break;

        case '!':
            nextChar();
            if (look_ == '=') {
                strcpy(tmp, "!=");
                tk = TK_DIF;
                nextChar();
            } else {
                tk = TK_NEG;
                return newToken(tk, tmp);
            }
            break;

        case '<':
            nextChar();
            if (look_ == '=') {
                strcpy(tmp, "<=");
                tk = TK_MENORIG;
                nextChar();
            } else {
                tk = TK_MENOR;
                return newToken(tk, tmp);
            }
            break;

        case '>':
            nextChar();
            if (look_ == '=') {
                strcpy(tmp, ">=");
                tk = TK_MAIORIG;
                nextChar();
            } else {
                tk = TK_MAIOR;
                return newToken(tk, tmp);
            }
            break;

        case '/':
            nextChar();
            if (look_ == '/') {
                nextChar();
                getNoComment("//");
                return nextToken();
            } else if (look_ == '*') {
                nextChar();
                getNoComment("/*");
                return nextToken();
            } else {
                tk = TK_DIVISAO;
                return newToken(tk, "/");
            }
            break;

        case '"':
            nextChar();
            return scanString();
    }

    nextChar();
    return newToken(tk, tmp);
}

void getNoBlank() {
    while (isspace(look_) && !feof(inputfile)) {
        if (look_ == '\n')
            line_++;
        nextChar();
    }
}

void getNoComment(const char *t) {
    int endComment = 0;
    if (strcmp(t, "//") == 0) {
        while (look_ != '\n' && !feof(inputfile))
            nextChar();
    } else {
        while (!endComment) {
            if (feof(inputfile))
                error(0, EXPEC, "*/");
            if (look_ == '\n')
                line_++;
            nextChar();
            if (look_ == '*') {
                while (look_ == '*')
                    nextChar();
                if (look_ == '/') {
                    endComment = 1;
                    nextChar();
                }
            }
        }
    }
}

int nextChar() {
    if (feof(inputfile)) return 0;
    fscanf(inputfile, "%c", &look_);
    return 1;
}

int isOp(char c) {
    return (strchr("+-|*/%&=!<>{}()[]\",;", c) != NULL);
}

int openFile(const char *filename) {
    inputfile = fopen(filename, "r");
    return (inputfile != NULL);
}

void closeFile() {
    fclose(inputfile);
}
