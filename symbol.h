#ifndef	SYMBOL_H
#define	SYMBOL_H

#include <stdio.h>
#include <stdlib.h>


#include "mstring.h"

typedef enum { int_t, float_t ,string_t} TYPE; 

union m_value {
		int	int_value;
		float	float_value;
		char*   str_value;
} ;

struct symbol {
    const char* name;
    union m_value* val;
    TYPE t;
    int lastindex; //ultimo index acessado
};


struct symcell {
	symbol	*info;
	struct symcell	*next;
};

struct symtab {

	struct symtab	*parent;
	struct symcell	*list;
	struct symtab  *next;
	int level;
	int id;
	
};



struct symtab* rootTable();
    
struct symtab* newTable(struct symtab* enclosing_scope);
struct symbol* findSymbol(struct symtab*,char*);
struct symbol* findSymbol(struct symcell*,char*);

struct symbol* insertSymbol(struct symtab*,char*,TYPE t,int size=0);

struct symcell* insertSymbol(struct symcell*,symbol*);
struct symbol* newSymbol(char*,TYPE t,int size=1);

int assign(struct symbol *sym,struct symbol *other);
int assign(struct symbol *sym,int val);
int assign(struct symbol *sym,float val);
int assign(struct symbol *sym,char* val);


inline void setValue(struct symbol* sym,int obj){
    sym->val->int_value=  obj;
}

inline void setValue(struct symbol* sym,float obj){
     sym->val->float_value=  obj;
}

inline void setValue(struct symbol* sym,char* obj){

  sym->val->str_value = str(obj);
}

inline int getInt(struct symbol* sym){
 return sym->val->int_value;
}

inline float getFloat(struct symbol* sym){
 return sym->val->float_value;
}

inline const char* getString(struct symbol* sym){
 return sym->val->str_value;
}



#endif
