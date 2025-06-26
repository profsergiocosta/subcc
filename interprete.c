#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "interprete.h"
#include "mstring.h"
#include "symbol.h"
#include "error.h"

// Funções internas
int isCodOper(int op_code);
void operate(int op_code, struct symtab* tabcur, struct instruction* inst);

struct instructMapEntry {
    int key;
    struct instruction* value;
    struct instructMapEntry* next;
};

static struct instructMapEntry* instructs_ = NULL;
static int codinstr_ = 1;


int nextInstructionCode() {
    return codinstr_;
}

void printEscaped(const char* s);

void disassemble() {
    printf("==== D I S A S S E M B L Y ====\n");

    // 1. Contar quantas instruções existem
    int count = 0;
    struct instructMapEntry* entry = instructs_;
    while (entry) {
        count++;
        entry = entry->next;
    }

    if (count == 0) {
        printf("[nenhuma instrução gerada]\n");
        return;
    }

    // 2. Copiar para vetor
    struct instructMapEntry** entries = malloc(sizeof(struct instructMapEntry*) * count);
    entry = instructs_;
    for (int i = 0; i < count; i++) {
        entries[i] = entry;
        entry = entry->next;
    }

    // 3. Ordenar por chave (key)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (entries[i]->key > entries[j]->key) {
                struct instructMapEntry* tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }

    // 4. Imprimir instruções em ordem
    for (int i = 0; i < count; i++) {
        struct instruction* inst = entries[i]->value;
        const char* opname;

        switch (inst->op_code) {
            case OPATR:      opname = "OPATR"; break;
            case OPSOMA:     opname = "OPSOMA"; break;
            case OPMULT:     opname = "OPMULT"; break;
            case OPSUB:      opname = "OPSUB"; break;
            case OPDIV:      opname = "OPDIV"; break;
            case OPMAIS:     opname = "OPMAIS"; break;
            case OPMENOS:    opname = "OPMENOS"; break;
            case OPMOD:      opname = "OPMOD"; break;
            case OPEQU:      opname = "OPEQU"; break;
            case OPDIF:      opname = "OPDIF"; break;
            case JEQZ:       opname = "JEQZ"; break;
            case OPNEG:      opname = "OPNEG"; break;
            case EMPTY:      opname = "EMPTY"; break;
            case PRINTF:     opname = "PRINTF"; break;
            case SCANF:      opname = "SCANF"; break;
            case BLOCK:      opname = "BLOCK"; break;
            case BLOCKEND:   opname = "BLOCKEND"; break;
            case JUMP:       opname = "JUMP"; break;
            case OPBOR:      opname = "OPBOR"; break;
            case OPBAND:     opname = "OPBAND"; break;
            case OPMAIOR:    opname = "OPMAIOR"; break;
            case OPMENOR:    opname = "OPMENOR"; break;
            case OPMAIORIG:  opname = "OPMAIORIG"; break;
            case OPMENORIG:  opname = "OPMENORIG"; break;
            case ACESSIND:   opname = "ACESSIND"; break;
            default:         opname = "UNKNOWN"; break;
        }

        printf("[%03d] %s", entries[i]->key, opname);
        // Argumento 0
        if (inst->args[0]) {
            printf(" %s", inst->args[0]);

            struct symbol* s = findSymbol(rootTable(), inst->args[0]);
            if (s) {
                switch (s->t) {
                    case int_t:
                        printf("(%d)", getInt(s));
                        break;
                    case float_t:
                        printf("(%.3f)", getFloat(s));
                        break;
                    case string_t:
                        printf("(");
                        printEscaped(getString(s));
                        printf(")");
                        break;
                }
            }
        }

        // Argumento 1
        if (inst->args[1]) {
            printf(", %s", inst->args[1]);

            struct symbol* s = findSymbol(rootTable(), inst->args[1]);
            if (s) {
                switch (s->t) {
                    case int_t:
                        printf("(%d)", getInt(s));
                        break;
                    case float_t:
                        printf("(%.3f)", getFloat(s));
                        break;
                    case string_t:
                        printf("(");
                        printEscaped(getString(s));
                        printf(")");
                        break;
                }
            }
        }

        // Resultado
        if (inst->result) {
            printf(" -> %s", inst->result);

            struct symbol* s = findSymbol(rootTable(), inst->result);
            if (s) {
                switch (s->t) {
                    case int_t:
                        printf("(%d)", getInt(s));
                        break;
                    case float_t:
                        printf("(%.3f)", getFloat(s));
                        break;
                    case string_t:
                        printf("(");
                        printEscaped(getString(s));
                        printf(")");
                        break;
                }
            }
        }

        printf("\n");
        
    }

    free(entries);
    printf("==============================\n");
}


void printEscaped(const char* s) {
    putchar('"');
    for (; *s; s++) {
        if (*s == '\n') {
            printf("\\n");
        } else if (*s == '\r') {
            printf("\\r");
        } else if (*s == '\t') {
            printf("\\t");
        } else if (*s == '\"') {
            printf("\\\"");
        } else if (*s == '\\') {
            printf("\\\\");
        } else if ((unsigned char)*s < 32) {
            printf("\\x%02X", (unsigned char)*s);
        } else {
            putchar(*s);
        }
    }
    putchar('"');
}

// Insere ou atualiza a instrução no mapa simples (implementação simples de mapa)
static void instructs_insert(int key, struct instruction* value) {
    struct instructMapEntry* entry = instructs_;
    while (entry) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }
    // Não encontrou, insere novo no início
    entry = (struct instructMapEntry*) malloc(sizeof(struct instructMapEntry));
    if (!entry) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    entry->key = key;
    entry->value = value;
    entry->next = instructs_;
    instructs_ = entry;
}

// Busca uma instrução pelo código
static struct instruction* instructs_find(int key) {
    struct instructMapEntry* entry = instructs_;
    while (entry) {
        if (entry->key == key)
            return entry->value;
        entry = entry->next;
    }
    return NULL;
}

int gen3ai(int op_code, const char* arg1, const char* arg2, const char* result, int codinst) {
    struct instruction* aux = (struct instruction*) fmalloc(sizeof(struct instruction));
    aux->args[0] = (char*)arg1;
    aux->args[1] = (char*)arg2;
    aux->result = (char*)result;
    aux->op_code = op_code;
    aux->next = NULL;

    if (codinst) {
        instructs_insert(codinst, aux);
        return codinst;
    } else {
        instructs_insert(codinstr_, aux);
        codinstr_++;
        return codinstr_ - 1;
    }
}

void backpatch(int codins, const char* arg2) {
    struct instruction* aux = instructs_find(codins);
    if (!aux) return;

    if (aux->op_code == JEQZ)
        aux->args[1] = str((char*)arg2);
    else if (aux->op_code == JUMP)
        aux->args[0] = str((char*)arg2);
}

void interprete(struct symtab* st) {
    int find = 0;
    int valint;
    float valfloat;

    struct symtab* auxt;
    struct symtab* tabcur = st; // tabela corrente

    struct symbol* s0 = NULL;  // símbolo argumento 1
    struct symbol* s1 = NULL;  // símbolo argumento 2
    struct symbol* sr = NULL;  // símbolo resultado

    int current_code = 1;

    while (current_code < codinstr_) {
        struct instruction* aux = instructs_find(current_code);
        if (!aux) break;

        int op_code = aux->op_code;

        if (isCodOper(op_code)) {
            operate(op_code, tabcur, aux);
        } else {
            switch (op_code) {
            case OPATR:
                assignSymbolSymbol(findSymbol(tabcur, aux->result), findSymbol(tabcur, aux->args[0]));
                break;

            case BLOCK:
                auxt = st; // aponta para o primeiro
                find = 0;
                //printf("[BLOCK] antes de entrar no bloco id=%d, tabcur=%p pai%p\n", auxt->id, (void*)auxt, (void*)auxt->parent);
                while (auxt && !find) {
                    find = (auxt->id == atoi(aux->args[0]));
                    if (!find) auxt = auxt->next;
                }
                if (find) {
                    //printf("[BLOCK] Entrando no bloco id=%d, tabcur=%p\n", auxt->id, (void*)auxt);
                    tabcur = auxt;
                }
                break;

            case BLOCKEND:
                //printf("[BLOCKEND] Saindo do bloco id=%d, tabcur=%p\n", tabcur->id, (void*)tabcur);

                if (tabcur) {
                    tabcur = tabcur->parent;
                    //printf("[BLOCKEND] Saindo do bloco tabcur=%p pai %p\n", (void*)tabcur, (void*)tabcur->parent);
                }
                    
		
                break;

            case PRINTF:
                s0 = findSymbol(tabcur, aux->args[0]);
                s1 = findSymbol(tabcur, aux->args[1]);
                if (!s1)
                    printf("%s", getString(s0));
                else if (s1->t == int_t)
                    printf(getString(s0), getInt(s1));
                else if (s1->t == string_t)
                    printf(getString(s0), getString(s1));
                else if (s1->t == float_t)
                    printf(getString(s0), getFloat(s1));
                break;

            case SCANF:
                s0 = findSymbol(tabcur, aux->args[0]);
                s1 = findSymbol(tabcur, aux->args[1]);
                if (s1->t == int_t) {
                    scanf(getString(s0), &valint);
                    assignSymbolInt(s1, valint);
                } else if (s1->t == float_t) {
                    scanf(getString(s0), &valfloat);
                    assignSymbolFloat(s1, valfloat);
                }
                break;

			case JEQZ:
				s0 = findSymbol(tabcur, aux->args[0]);
				if (!s0) {
					//printf("[ERRO] JEQZ: símbolo '%s' não encontrado!\n", aux->args[0]);
				}
				else {
					//printf("[DEBUG] JEQZ : símbolo '%s' valor = %d\n", aux->args[0], getInt(s0));
					int cond = 0;
					switch (s0->t) {
						case int_t:
							cond = getInt(s0);
							break;
						case float_t:
							cond = (getFloat(s0) != 0.0f);
							break;
						case string_t:
							cond = (getString(s0)[0] != '\0');
							break;
					}
					if (!cond) {
						//printf("[DEBUG] JEQZ salta para %s\n", aux->args[1]);
						current_code = atoi(aux->args[1]);
						continue;
					}
				}
	break;

            case JUMP:
                current_code = atoi(aux->args[0]);
                continue; // pula incremento

            case ACESSIND:
                s0 = findSymbol(tabcur, aux->args[0]);
                s1 = findSymbol(tabcur, aux->args[1]);
                if (s0 && s1) {
                    s0->val = (s0->val - s0->lastindex) + getInt(s1); // acessa o index
                    s0->lastindex = getInt(s1);
                }
                break;

            default:
                break;
            }
        }
        current_code++;
    }
}

void operate(int op_code, struct symtab* tabcur, struct instruction* inst) {
    float res = 0, arg1 = 0, arg2 = 0;

    struct symbol* s0 = findSymbol(tabcur, inst->args[0]);  // símbolo argumento 1
    struct symbol* s1 = findSymbol(tabcur, inst->args[1]);  // símbolo argumento 2
    struct symbol* sr = findSymbol(tabcur, inst->result);  // símbolo resultado

    if (!s0 || !s1 || !sr) return;

    if (op_code != OPMAIS && op_code != OPMENOS) {
        // primeiro argumento
        if (s0->t == int_t)
            arg1 = (float)getInt(s0);
        else if (s0->t == float_t)
            arg1 = getFloat(s0);

        // segundo argumento
        if (s1->t == int_t)
            arg2 = (float)getInt(s1);
        else if (s1->t == float_t)
            arg2 = getFloat(s1);
    } else {
        if (sr->t == int_t)
            res = (float)getInt(sr);
        else if (sr->t == float_t)
            res = getFloat(sr);
    }

    switch (op_code) {
    case OPSOMA:
        res = arg1 + arg2;
        break;
    case OPSUB:
        res = arg1 - arg2;
        break;
    case OPMAIS:
        res = +res;
        break;
    case OPMENOS:
        res = -res;
        break;
    case OPMULT:
        res = arg1 * arg2;
        break;
    case OPDIV:
        res = arg1 / arg2;
        break;
    case OPEQU:
        res = (arg1 == arg2);
        break;
    case OPMOD:
        res = (int)arg1 % (int)arg2;
        break;
    case OPDIF:
        res = (arg1 != arg2);
        break;
    case OPMAIOR:
        res = (arg1 > arg2);
        break;
    case OPMENOR:
        res = (arg1 < arg2);
        break;
    case OPMAIORIG:
        res = (arg1 >= arg2);
        break;
    case OPMENORIG:
        res = (arg1 <= arg2);
        break;
    case OPBOR:
        res = arg1 || arg2;
        break;
    case OPBAND:
        res = arg1 && arg2;
        break;
    default:
        break;
    }

    // retorna resultado
    if (sr->t == int_t)
        assignSymbolInt(sr, (int)res);
    else if (sr->t == float_t)
        assignSymbolFloat(sr, res);
}

int isCodOper(int op_code) {
    return (op_code == OPSOMA) || (op_code == OPSUB) || (op_code == OPDIV) ||
           (op_code == OPMULT) || (op_code == OPMOD) || (op_code == OPBOR) ||
           (op_code == OPBAND) || (op_code == OPMENOS) || (op_code == OPMAIS) ||
           (op_code == OPDIF) || (op_code == OPMAIOR) || (op_code == OPMAIORIG) ||
           (op_code == OPMENOR) || (op_code == OPMENORIG) || (op_code == OPEQU);
}
