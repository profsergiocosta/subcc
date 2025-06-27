#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mstring.h"
#include "error.h"
#include "lexical.h"





#define KWLIST_SZ 8
#define MAXTOKEN 64


// Lista de palavras-chave
static char *kwlist[KWLIST_SZ] = {
    "main", "int", "float", "if", "else", "while", "printf", "scanf"
};

// Estado global
static char look_;
static int line_;
static FILE *inputfile;
static char tokvalue_[MAXTOKEN + 1];


void skipComment(const char *t);
int readChar() ;

/**
 * Inicialização do scanner.
 */
void init() {
    tokvalue_[0] = '\0';
    look_ = 0;
    line_ = 1;
    readChar();
}

/**
 * Abre arquivo fonte.
 */
int openFile(const char *filename) {
    inputfile = fopen(filename, "r");
    return (inputfile != NULL);
}

/**
 * Fecha arquivo fonte.
 */
void closeFile() {
    fclose(inputfile);
}

/**
 * Lê próximo caractere da entrada.
 */
int readChar() {
    if (feof(inputfile)) return 0;
    fscanf(inputfile, "%c", &look_);
    return 1;
}

/**
 * Verifica se caractere é operador/pontuação.
 */
int isOp(char c) {
    return (strchr("+-|*/%&=!<>{}()[]\",;", c) != NULL);
}

/**
 * Busca palavra-chave.
 */
int lookupKeyword(const char *w) {
    for (int i = 0; i < KWLIST_SZ; i++) {
        if (strcmp(kwlist[i], w) == 0)
            return i;
    }
    return -1;
}

/**
 * Cria novo token.
 */
Token* newToken(int tk, const char *value) {
    Token *token = (Token *) fmalloc(sizeof(Token));
    token->type = tk;
    token->line = line_;
    token->value = str(value);
    return token;
}

/**
 * Pula espaços e conta linhas.
 */
void skipWhitespace() {
    while (isspace(look_) && !feof(inputfile)) {
        if (look_ == '\n')
            line_++;
        readChar();
    }
}

/**
  Ignora comentários (// ou /* ).
*/
void skipComment(const char *t) {
    int endComment = 0;
    if (strcmp(t, "//") == 0) {
        while (look_ != '\n' && !feof(inputfile))
            readChar();
    } else {
        while (!endComment) {
            if (feof(inputfile))
                error(0, EXPEC, "*/");
            if (look_ == '\n')
                line_++;
            readChar();
            if (look_ == '*') {
                while (look_ == '*')
                    readChar();
                if (look_ == '/') {
                    endComment = 1;
                    readChar();
                }
            }
        }
    }
}

/**
 * Escaneia números inteiros ou floats.
 */
Token* scanNumber() {
    int tk = TK_INT;
    int len = 0;
    tokvalue_[0] = '\0';

    while (!feof(inputfile) && (isdigit(look_) || (look_ == '.' && tk == TK_INT))) {
        if (look_ == '.') tk = TK_FLOAT;

        if (len < MAXTOKEN) {
            tokvalue_[len++] = look_;
            tokvalue_[len] = '\0';
        }
        readChar();
    }
    return newToken(tk, tokvalue_);
}

/**
 * Escaneia identificadores ou palavras-chave.
 */
Token* scanIdentifierOrKeyword() {
    int len = 0;
    tokvalue_[0] = '\0';

    while (!feof(inputfile) && (look_ == '_' || isalnum(look_))) {
        if (len < MAXTOKEN) {
            tokvalue_[len++] = look_;
            tokvalue_[len] = '\0';
        }
        readChar();
    }

    int kw = lookupKeyword(tokvalue_);
    int tk = (kw == -1) ? TK_IDENT : kw;
    return newToken(tk, tokvalue_);
}

/**
 * Escaneia literais de string entre aspas.
 */
Token* scanStringLiteral() {
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
        readChar();
    }
    readChar();
    return newToken(TK_STRING, tokvalue_);
}

/**
 * Escaneia operadores e pontuação.
 */
Token* scanOperatorOrPunct() {
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
            readChar();
            if (look_ == '|') {
                strcpy(tmp, "||");
                tk = TK_OR;
                readChar();
            } else return newToken(TK_ERROR, tmp);
            break;

        case '&':
            readChar();
            if (look_ == '&') {
                strcpy(tmp, "&&");
                tk = TK_AND;
                readChar();
            } else {
                tk = TK_ENDER;
                return newToken(tk, tmp);
            }
            break;

        case '=':
            readChar();
            if (look_ == '=') {
                strcpy(tmp, "==");
                tk = TK_IGUAL;
                readChar();
            } else {
                tk = TK_ATRIBUICAO;
                return newToken(tk, tmp);
            }
            break;

        case '!':
            readChar();
            if (look_ == '=') {
                strcpy(tmp, "!=");
                tk = TK_DIF;
                readChar();
            } else {
                tk = TK_NEG;
                return newToken(tk, tmp);
            }
            break;

        case '<':
            readChar();
            if (look_ == '=') {
                strcpy(tmp, "<=");
                tk = TK_MENORIG;
                readChar();
            } else {
                tk = TK_MENOR;
                return newToken(tk, tmp);
            }
            break;

        case '>':
            readChar();
            if (look_ == '=') {
                strcpy(tmp, ">=");
                tk = TK_MAIORIG;
                readChar();
            } else {
                tk = TK_MAIOR;
                return newToken(tk, tmp);
            }
            break;

        case '/':
            readChar();
            if (look_ == '/') {
                readChar();
                skipComment("//");
                return nextToken();
            } else if (look_ == '*') {
                readChar();
                skipComment("/*");
                return nextToken();
            } else {
                tk = TK_DIVISAO;
                return newToken(tk, "/");
            }
            break;

        case '"':
            readChar();
            return scanStringLiteral();
    }

    readChar();
    return newToken(tk, tmp);
}

/**
 * Retorna próximo token.
 */
Token* nextToken() {
    if (!feof(inputfile)) {
        tokvalue_[0] = '\0';
        skipWhitespace();

        if (isdigit(look_)) return scanNumber();
        if (isalpha(look_)) return scanIdentifierOrKeyword();
        if (isOp(look_)) return scanOperatorOrPunct();
        if (look_ == '\n') return newToken(TK_EOF, "eof");

        return newToken(TK_ERROR, "error");
    }
    return newToken(TK_EOF, "eof");
}
