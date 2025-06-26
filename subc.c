#include <stdio.h>
#include <string.h>

#include "symbol.h"
#include "mstring.h"
#include "error.h"

#include "subc.h"
#include "lexical.h"
#include "interprete.h"


char *newTemp(TYPE cons);
void newConst(TYPE cons);
void program();
void block();
int dec();
int type();
void var();
void listVar();
int ident();
void listComand();
int comand();
void atribuicao();
void ifStruct();
void whileStruct();
void sPrintf();
void sScanf();
void expr();
void exprSimples();
void sinal();
void termo();
void fator();

int opRel();
int opAdit();
int opMult();

int integer();

int numtemp_ = 1;
int numblock_ = 1;
int type_ = -1;
char* arg_;
Token *token_;

struct symtab* curTab_ = 0;

int dec() {
    while (type())
        dec();
    return 1;
}

int type() {
    type_ = token_->type;
    if ((type_ == KW_INT) || (type_ == KW_FLOAT)) {
        listVar();
        return 1;
    }
    else
        return 0;
}

void listVar() {
    var();
    int tk = token_->type;
    while (tk == TK_VIRG) {
        var();
        tk = token_->type;
    }
    if (tk == TK_PTOVIRG)
        token_ = nextToken();
    else
        error(token_->line, EXPEC, ";");
}

int ident() {
    char* arg1;

    if (token_->type == TK_IDENT) {
        arg_ = str(token_->value);
        arg1 = str(arg_);
        if (!(findSymbol(curTab_, token_->value)))
            error(token_->line, UNDEC, "%s", token_->value);
        token_ = nextToken();

        if (token_->type == TK_ACOLC) {
            token_ = nextToken();
            expr();

            gen3ai(ACESSIND, arg1, str(arg_), "", 0);
            arg_ = str(arg1);

            if (token_->type == TK_FCOLC)
                token_ = nextToken();
            else
                error(token_->line, EXPEC, "]");
        }
        return 1;
    }
    else return 0;
}

void var() {
    token_ = nextToken();
    if (token_->type == TK_IDENT) {
        int size = 0;
        char* name = str(token_->value);
        if (findSymbolInList(curTab_->list, token_->value))
            error(token_->line, REDEC, "%s", token_->value);

        token_ = nextToken();

        if (token_->type == TK_ACOLC) {
            token_ = nextToken();

            size = integer();

            if (token_->type == TK_FCOLC)
                token_ = nextToken();
            else
                error(token_->line, EXPEC, "]");
        }

        if (type_ == KW_INT)
            insertSymbol(curTab_, name, int_t, size);

        if (type_ == KW_FLOAT)
            insertSymbol(curTab_, name, float_t, size);
    }
    else
        error(token_->line, EXPEC, "ident");
}

void block() {
    if (token_->type == TK_ACHAVE) {
        token_ = nextToken();
        curTab_ = newTable(curTab_);
        curTab_->id = numblock_;
		numblock_++; // identificador do bloco
		printf("[BLOCK] criando bloco id=%d, tabcur=%p  pai %p\n", curTab_->id, (void*)curTab_, (void*)curTab_->parent );
        dec();
        listComand();

        if (token_->type == TK_FCHAVE) {
            token_ = nextToken();
            if (curTab_->parent) {
                curTab_ = curTab_->parent;
            }
        }
        else
            error(token_->line, EXPEC, "}");
    }
    else
        error(token_->line, EXPEC, "{");
}

void sPrintf() {
    char* arg1;
    char* arg2;
    token_ = nextToken();

    if (token_->type == TK_APAREN) {
        token_ = nextToken();

        if (token_->type == TK_STRING) {
            // Copia o valor do token para um buffer modificável
            char *str1 = strdup(token_->value);
            if (!str1) {
                error(token_->line, EXPEC, "memory allocation failed");
                return;
            }
            
            token_ = nextToken();

            // Loop para processar cada formato %
            while (1) {
                // Procura o próximo %
                char *percent = strchr(str1, '%');
                if (!percent)
                    break;

                // Copia até o % + 2 caracteres para pegar o especificador (%d, %s, etc)
                size_t len = (percent - str1) + 2;
                char *str2 = (char*)malloc(len + 1);
                if (!str2) {
                    free(str1);
                    error(token_->line, EXPEC, "memory allocation failed");
                    return;
                }
                strncpy(str2, str1, len);
                str2[len] = '\0';

                // Remove a parte copiada de str1 (shift left)
                memmove(str1, str1 + len, strlen(str1 + len) + 1);

                arg1 = newTemp(string_t);
                setValueString(findSymbol(curTab_, arg1), str2);
                free(str2);

                if (token_->type == TK_VIRG) {
                    token_ = nextToken();
                    expr();
                    arg2 = str(arg_);
                    gen3ai(PRINTF, arg1, arg2, "", 0);
                }
                else {
                    // Se não tem vírgula, sai do loop
                    break;
                }
            }

            // Após o loop, o que sobrou em str1 é o texto restante
            if (strlen(str1) > 0) {
                arg1 = newTemp(string_t);
                setValueString(findSymbol(curTab_, arg1), str1);
                gen3ai(PRINTF, arg1, "", "", 0);
            }

            free(str1);
        }
        else {
            error(token_->line, EXPEC, "string");
        }

        if (token_->type == TK_FPAREN)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ")");

        if (token_->type == TK_PTOVIRG)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ";");
    }
    else {
        error(token_->line, EXPEC, "(");
    }
}


void sScanf() {
    char* arg1;
    char* arg2;

    token_ = nextToken();

    if (token_->type == TK_APAREN) {
        token_ = nextToken();

        if (token_->type == TK_STRING) {
            // Copia o valor da string para uma buffer local
            char buffer[1024];
            strncpy(buffer, token_->value, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';

            char *percent_pos;
            token_ = nextToken();

            // Enquanto tiver '%' na string
            while ((percent_pos = strchr(buffer, '%')) != NULL) {
                int length = (int)(percent_pos - buffer) + 2; // inclui o especificador e próximo char
                char str2[50] = {0};

                if (length >= (int)sizeof(str2))
                    length = (int)sizeof(str2) - 1;

                // Copia parte da string com o formato (ex: "%d", "%f")
                strncpy(str2, buffer, length);
                str2[length] = '\0';

                // Remove a parte já processada do buffer
                memmove(buffer, buffer + length, strlen(buffer + length) + 1);

                arg1 = newTemp(string_t);
                setValueString(findSymbol(curTab_, arg1), str2);

                if (token_->type == TK_VIRG) {
                    token_ = nextToken();

                    if (token_->type == TK_ENDER)
                        token_ = nextToken();
                    else
                        error(token_->line, EXPEC, "&");

                    if (!ident())
                        error(token_->line, EXPEC, "ident");

                    arg2 = str(arg_);
                    gen3ai(SCANF, arg1, arg2, "", 0);
                }
            }
        } else {
            error(token_->line, EXPEC, "string");
        }

        if (token_->type == TK_FPAREN)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ")");

        if (token_->type == TK_PTOVIRG)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ";");
    } else {
        error(token_->line, EXPEC, "(");
    }
}


void listComand() {
    if (comand())
        while (comand());
    else
        error(token_->line, EXPEC, "comando");
}

void expr() {
    char* arg1;
    char* arg2;
    char* res;
    int type;
    exprSimples();

    arg1 = str(arg_);

    while (opRel()) {
        type = token_->type;
        res = newTemp(int_t);
        token_ = nextToken();
        exprSimples();
        arg2 = str(arg_);

        if (type == TK_IGUAL)
            gen3ai(OPEQU, arg1, arg2, res, 0);

        if (type == TK_DIF)
            gen3ai(OPDIF, arg1, arg2, res, 0);

        if (type == TK_MAIOR)
            gen3ai(OPMAIOR, arg1, arg2, res, 0);

        if (type == TK_MENOR)
            gen3ai(OPMENOR, arg1, arg2, res, 0);

        if (type == TK_MAIORIG)
            gen3ai(OPMAIORIG, arg1, arg2, res, 0);

        if (type == TK_MENORIG)
            gen3ai(OPMENORIG, arg1, arg2, res, 0);

        arg1 = str(res);
        arg_ = str(res);
    }
}

void exprSimples() {
    char* arg1;
    char* arg2;
    char* res;
    int type;

    sinal();

    termo();

    arg1 = str(arg_);
    while (opAdit()) {
        type = token_->type;
        res = newTemp(float_t);
        token_ = nextToken();
        termo();

        arg2 = str(arg_);

        if ((findSymbol(curTab_, arg1)->t == int_t) &&
            (findSymbol(curTab_, arg2)->t == int_t))
            findSymbol(curTab_, res)->t = int_t;

        if (type == TK_MAIS)
            gen3ai(OPSOMA, arg1, arg2, res, 0);

        if (type == TK_MENOS)
            gen3ai(OPSUB, arg1, arg2, res, 0);

        if (type == TK_OR)
            gen3ai(OPBOR, arg1, arg2, res, 0);

        arg1 = str(res);
        arg_ = str(res);
    }

    if (type_ == TK_MAIS)
        gen3ai(OPMAIS, "", "", str(arg1), 0);

    if (type_ == TK_MENOS)
        gen3ai(OPMENOS, "", "", str(arg1), 0);
}

void termo() {
    char* arg1;
    char* arg2;
    char* res;
    int type;

    fator();
    arg1 = str(arg_);
    while (opMult()) {
        type = token_->type;
        res = newTemp(float_t);
        token_ = nextToken();
        fator();
        arg2 = str(arg_);

        if ((findSymbol(curTab_, arg1)->t == int_t) &&
            (findSymbol(curTab_, arg2)->t == int_t))
            findSymbol(curTab_, res)->t = int_t;

        if (type == TK_VEZES)
            gen3ai(OPMULT, arg1, arg2, res, 0);

        if (type == TK_DIVISAO)
            gen3ai(OPDIV, arg1, arg2, res, 0);

        if (type == TK_PORCENT)
            gen3ai(OPMOD, arg1, arg2, res, 0);

        if (type == TK_AND)
            gen3ai(OPBAND, arg1, arg2, res, 0);

        arg1 = str(res);
        arg_ = str(res);
    }
}

void fator() {
    if ((token_->type == TK_INT) || (token_->type == TK_FLOAT)) {
        if (token_->type == TK_INT)
            newConst(int_t);
        else
            newConst(float_t);

        token_ = nextToken();
    }
    else if (ident()) {
        // nothing to do here, ident() already advanced token
    }
    else if (token_->type == TK_NEG) {
        token_ = nextToken();
        fator();
    }
    else if (token_->type == TK_APAREN) {
        token_ = nextToken();
        expr();
        if (token_->type == TK_FPAREN)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ")");
    }
    else
        error(token_->line, EXPEC, "fator");
}

void sinal() {
    type_ = token_->type;
    if ((token_->type == TK_MAIS) || (token_->type == TK_MENOS))
        token_ = nextToken();
}

int opRel() {
    return ((token_->type == TK_IGUAL) ||
        (token_->type == TK_DIF) ||
        (token_->type == TK_MAIOR) ||
        (token_->type == TK_MENOR) ||
        (token_->type == TK_MAIORIG) ||
        (token_->type == TK_MENORIG));
}

int opAdit() {
    return ((token_->type == TK_MAIS) ||
        (token_->type == TK_MENOS) ||
        (token_->type == TK_OR));
}

int opMult() {
    return ((token_->type == TK_VEZES) ||
        (token_->type == TK_DIVISAO) ||
        (token_->type == TK_PORCENT) ||
        (token_->type == TK_AND));
}

int integer() {
    int val;

    if (token_->type == TK_INT) {
        val = atoi(token_->value);
        token_ = nextToken();
        return val;
    }
    else
        error(token_->line, EXPEC, "inteiro");
}

int comand() {
    if (ident()) {
        atribuicao();
        return 1;
    }
    else if (token_->type == KW_IF) {
        ifStruct();
        return 1;
    }
    else if (token_->type == KW_WHILE) {
        whileStruct();
        return 1;
    }
    else if (token_->type == KW_PRINTF) {
        sPrintf();
        return 1;
    }
    else if (token_->type == KW_SCANF) {
        sScanf();
        return 1;
    }

    return 0;
}

void ifStruct() {
    int codinst_1;
    int codinst_2;

    token_ = nextToken();

    if (token_->type == TK_APAREN) {
        token_ = nextToken();
        expr();
        if (token_->type == TK_FPAREN) {
            token_ = nextToken();

            codinst_1 = gen3ai(JEQZ, str(arg_), "", "", 0);
            gen3ai(BLOCK, intToString(numblock_), "", "", 0);
            block();
            codinst_2 = gen3ai(BLOCKEND, "", "", "", 0) + 1;
            backpatch(codinst_1, intToString(codinst_2));
        }
        else
            error(token_->line, EXPEC, ")");

        if (token_->type == KW_ELSE) {
            token_ = nextToken();
            codinst_2 = gen3ai(JUMP, "", "", "", 0);
            gen3ai(BLOCK, intToString(numblock_), "", "", 0);
            backpatch(codinst_1, intToString(codinst_2 + 1));
            block();
            backpatch(codinst_2, intToString((gen3ai(BLOCKEND, "", "", "", 0) + 1)));
        }
    }
    else
        error(token_->line, EXPEC, "(");
}

void whileStruct() {
    int cond_inst, jeqz_inst, end_inst;

    token_ = nextToken();
    if (token_->type != TK_APAREN)
        error(token_->line, EXPEC, "(");

    token_ = nextToken();
    
    cond_inst = gen3ai(EMPTY, "", "", "", 0);  // Inserir EMPTY ANTES da expressão
    expr();  // Gera OPMENOR

    if (token_->type != TK_FPAREN)
        error(token_->line, EXPEC, ")");

    token_ = nextToken();

    jeqz_inst = gen3ai(JEQZ, str(arg_), "", "", 0);  // salta se condição falsa
    gen3ai(BLOCK, intToString(numblock_), "", "", 0);
    block();
    gen3ai(BLOCKEND, "", "", "", 0);
    gen3ai(JUMP, intToString(cond_inst), "", "", 0);  // volta pro EMPTY antes da expr()
    end_inst = gen3ai(EMPTY, "", "", "", 0);
    backpatch(jeqz_inst, intToString(end_inst));
}



void atribuicao() {
    char* arg1;
    char* arg2;
    char* res;

    if (token_->type == TK_ATRIBUICAO) {
        token_ = nextToken();

        res = str(arg_);

        expr();

        arg1 = str(arg_);
        gen3ai(OPATR, arg1, 0, res, 0);

        if (token_->type == TK_PTOVIRG)
            token_ = nextToken();
        else
            error(token_->line, EXPEC, ";");
    }
    else
        error(token_->line, EXPEC, "=");
}

void programa() {
    token_ = nextToken();
    if (token_->type == KW_MAIN) {
        token_ = nextToken();
        if (token_->type == TK_APAREN) {
            token_ = nextToken();
            if (token_->type == TK_FPAREN) {
                token_ = nextToken();
                block();
				//dec();
        		//listComand();
            }
            else
                error(token_->line, EXPEC, ")");
        }
        else
            error(token_->line, EXPEC, "(");
    }
    else
        error(token_->line, EXPEC, "Main");
}

int executa(const char* filename) {
    if (openFile(filename)) {
        init();
        programa();
        token_ = nextToken();
        closeFile();
		disassemble();

        interprete(rootTable());
        return 1;
    }
    else {
        printf("error to open file: %s\n", filename);
        return 0;
    }
}


/*
char* newTemp(TYPE cons) {
    char Buf[10];
    sprintf(Buf, "%i", numtemp_);

    char var[50];
    strcpy(var, "_TEMP_");
    strcat(var, Buf);

    // ✅ Cria sempre no escopo raiz
    insertSymbol(rootTable(), str(var), cons, 0);  
	

    numtemp_++;

    return str(var);
}


void newConst(TYPE cons) {
    char Buf[10];
    sprintf(Buf, "%i", numtemp_);

    char cons_name[50];
    strcpy(cons_name, "_CONST_");
    strcat(cons_name, Buf);

    char* cname = str(cons_name);

    // ✅ Sempre cria no rootTable()
    insertSymbol(rootTable(), cname, cons, 0);

    if (cons == int_t)
        setValueInt(findSymbol(rootTable(), cons_name), atoi(token_->value));
    else if (cons == string_t)
        setValueString(findSymbol(rootTable(), cons_name), token_->value);
    else if (cons == float_t)
        setValueFloat(findSymbol(rootTable(), cons_name), (float)atof(token_->value));

    arg_ = cname;

    numtemp_++;
}
	*/


char* newTemp(TYPE cons) {
    char Buf[10];
    sprintf(Buf, "%i", numtemp_);

    char var[50];
    strcpy(var, "_TEMP_");
    strcat(var, Buf);

    insertSymbol(curTab_, str(var), cons, 0);  

    numtemp_++;

    return str(var);
}

void newConst(TYPE cons) {
    char Buf[10];
    sprintf(Buf, "%i", numtemp_);

    char cons_name[50];
    strcpy(cons_name, "_CONST_");
    strcat(cons_name, Buf);
	
    char* cname = str(cons_name);
    insertSymbol(curTab_, cname, cons, 0);  // 👈 agora com o argumento `size = 0`
	
    if (cons == int_t) {
		printf ("criando const %s %d\n", cname, atoi(token_->value));
        setValueInt(findSymbol(curTab_, cons_name), atoi(token_->value));
	}
    else if (cons == string_t)
        setValueString(findSymbol(curTab_, cons_name), token_->value);
    else if (cons == float_t)
        setValueFloat(findSymbol(curTab_, cons_name), (float)atof(token_->value));

    arg_ = cname;

    numtemp_++;
}
