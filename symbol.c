/*	symtab.c(1.5)	10:07:40	97/12/10
*	
*	Symbol table management
*/
#include <stdlib.h>	/* for malloc() and friends */
#include <stdio.h>	/* for fprintf() and friends */
#include <string.h>	/* for fprintf() and friends */

#include "mstring.h"
#include "error.h"
#include "symbol.h"


void tablelistInsert(struct symtab* tl);

struct symtab* lastTab_=0;
struct symtab* firstTab_=0;

struct symtab* rootTable(){
    return firstTab_;
}

struct symtab* newTable(struct symtab * enclosing_scope){
    
    struct symtab* st = (struct symtab*)fmalloc(sizeof(struct symtab));
    st->parent	= enclosing_scope;
    st->list=0;
    st->next=0;
    
	
    if (enclosing_scope)
        st->level= enclosing_scope->level +1 ;
    else
	st->level= 0 ;

    tablelistInsert(st);
    
    return st;
}


struct symbol* findSymbol(struct symtab *st,char *name){
    struct symbol *i;

    for ( ; (st); st = st->parent)
	if ((i=findSymbol(st->list,name)))
		return i;
    return 0;
}


struct symbol* findSymbol(struct symcell* l,char* name)
{
    for (; (l); l = l->next)
      if (strcmp(name, l->info->name) == 0)
		return l->info;
    return 0;
}

struct symbol* insertSymbol(struct symtab *st,char *name,TYPE t,int size)
{
    struct symbol* i;
    if (size)
    	i = newSymbol(name,t,size);
    else
    	i = newSymbol(name,t);
    st->list = insertSymbol(st->list,i);
    return i;
}

symbol* newSymbol(char* name,TYPE t,int size)
{
    struct symbol* i = (struct symbol* ) fmalloc(sizeof(struct symbol));
    i->name = str(name);
    i->t= t;
    i->lastindex=0;
    i->val=(union m_value* ) fmalloc(size * sizeof(union m_value));
    return i;
}

struct symcell* insertSymbol(struct symcell* l,struct symbol* i)
{
    struct symcell* nl =(struct symcell*) fmalloc(sizeof(struct symcell));
    nl->info	= i;
    nl->next	= l;
    return nl;
}

void tablelistInsert(struct symtab* tl)
{
    if (firstTab_){
	lastTab_->next=tl;
	lastTab_=tl;
    }
    else{
	firstTab_=tl;
	lastTab_=tl;
    }
}



int assign(struct symbol *sym ,struct symbol *other){

if ((!sym) || (!sym))
    return 0;

 if (sym->t == int_t)
 {
   if (other->t == int_t)
      sym->val->int_value=other->val->int_value ;
   else
       sym->val->int_value=(int)other->val->float_value ;
 }
 else
 {
   if (other->t == float_t)
      sym->val->float_value=other->val->float_value ;
   else
      sym->val->float_value=(float)other->val->int_value ;
 }
}

int assign(struct symbol *sym,int val){
  sym->val->int_value=val;
}

int assign(struct symbol *sym,float val){
  sym->val->float_value = val ;
}

int assign(struct symbol *sym,char* val){
  sym->val->str_value= val ;
}





