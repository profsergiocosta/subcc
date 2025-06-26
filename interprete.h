
#ifndef	INTERPRETE_H
#define	INTERPRETE_H


typedef enum {OPATR, OPSOMA, OPMULT, OPSUB, OPDIV, OPMAIS,OPMENOS,OPMOD,
         OPEQU,OPDIF,JEQZ,OPNEG,EMPTY,PRINTF,SCANF,BLOCK,BLOCKEND,JUMP,
	 OPBOR,OPBAND,OPMAIOR,OPMENOR,OPMAIORIG,OPMENORIG,ACESSIND } OPCODE;

struct instruction {
        int op_code;
        char* args[2];
        char* result;
        struct instruction* next;
};

/* create quadruple representing instruction */
int gen3ai(int op_code,const char* arg1, const char* arg2, const char* result,int codinst=0);

void interprete(struct symtab *st);

void backpatch(int codins, const char* arg2);

#endif

