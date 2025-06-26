#include	<stdlib.h>	/* for malloc() and friends */
#include	<stdio.h>	/* for fprintf() and friends */

#include <map>
using namespace std;

//includes locais
#include "interprete.h"
#include "mstring.h"
#include "symbol.h"
#include "error.h"


typedef map<int,struct instruction*> instructMap;

//funções internas
int isCodOper(int op_code);
void operate(int op_code,struct symtab*,struct instruction* inst);

instructMap instructs_;
int codinstr_=1;

int gen3ai(int op_code,const char* arg1, const char* arg2, const char* result,int codinst)
{


 struct instruction* aux;
 aux=(struct instruction*)fmalloc(sizeof(struct instruction));
 aux->args[0]=(char*)arg1;
 aux->args[1]=(char*)arg2;
 aux->result=(char*)result;
 aux->op_code=OPCODE(op_code);
 aux->next=0;

 if (codinst) {
     instructs_[codinst]=aux;
     return (codinst);
 }
 else
 {
        instructs_.insert(instructMap::value_type (codinstr_,aux));
        codinstr_++;
        return (codinstr_-1);
 }

}

void backpatch(int codins, const char* arg2)
{
   instructMap::iterator it=instructs_.find(codins);
   if (it != instructs_.end())
   {
      struct instruction* aux=it->second;
      if (aux->op_code == JEQZ)
	  aux->args[1]=str((char*)arg2);
      else if (aux->op_code == JUMP)
	  aux->args[0]=str((char*)arg2);
	  
   }
}

void interprete(struct symtab *st)
{

 int find=0;
 int valint; float valfloat;
 
 struct symtab *auxt;
 struct symtab *tabcur=st; // tabela corrente
 
 struct symbol* s0=0;  //simbolo argumento 1
 struct symbol* s1=0; //simbolo argumento 2
 struct symbol* sr=0; //simbolo resultado
     
 instructMap::iterator it=instructs_.begin();
 while(it != instructs_.end())
 {
     struct instruction* aux=it->second;
     int op_code=aux->op_code;
     
     if (isCodOper(op_code)) operate(op_code,tabcur,aux);
     
     else
     	     switch(op_code)	{
	    
	     case OPATR:
		 assign((findSymbol(tabcur,aux->result)),(findSymbol(tabcur,aux->args[0])));
		 break;
	    
	     case BLOCK:
		 auxt=st; // aponta para o primeiro
		 find=0;
		 for ( ; ((!find) && (auxt)); auxt = auxt->next)
		        find=(auxt->id == atoi(aux->args[0]));
		  if (find) tabcur=auxt;
		  break;

	      case BLOCKEND:
		  tabcur=tabcur->parent;
		  break;
	
	      case PRINTF:
		  s0=findSymbol(tabcur,aux->args[0]);
		  s1=findSymbol(tabcur,aux->args[1]);
		  if (!s1)
		      printf((char*)getString(s0));
		  else if (s1->t == int_t)
		      printf((char*)getString(s0),getInt(s1));
		  else if (s1->t == string_t)
		      printf((char*)getString(s0),getString(s1));
		  else if (s1->t == float_t)
		      printf((char*)getString(s0),getFloat(s1));
		  break;

	      case SCANF:

		  s0=findSymbol(tabcur,aux->args[0]);
		  s1=findSymbol(tabcur,aux->args[1]);
		  if (s1->t == int_t) {
		      scanf(getString(s0), &valint);
		      assign(s1,valint);
		  }
		  else if (s1->t == float_t){
		      scanf(getString(s0), &valfloat);
		      assign(s1,valfloat);
		  }
		  break;
		  
	      case JEQZ:
		  if ( getInt( findSymbol(tabcur,aux->args[0]))  == 0)
		      it=instructs_.find(atoi(aux->args[1])-1);
		  break;

	      case JUMP:
		  it=instructs_.find(atoi(aux->args[0])-1);
		  break;
		 
	      case ACESSIND:
		  s0=findSymbol(tabcur,aux->args[0]);		 
		  s1=findSymbol(tabcur,aux->args[1]);
		  s0->val=(s0->val - s0->lastindex)+getInt(s1); // acessa o index
		  s0->lastindex=getInt(s1); //atualiza o ultimo index acessado
		  break;

		  default:
		  break;
	      }
     it++;
 }

}

void operate(int op_code,struct symtab *tabcur,struct instruction* inst)
{

 float res,arg1,arg2;
 

 struct symbol* s0=findSymbol(tabcur,inst->args[0]);  //simbolo argumento 1
 struct symbol* s1=findSymbol(tabcur,inst->args[1]); //simbolo argumento 2
 struct symbol* sr=findSymbol(tabcur,inst->result); //simbolo resultado

 if ((op_code != OPMAIS) && (op_code != OPMENOS))
 {
     
     //-- primeiro argumento
     if (s0->t == int_t)
	 arg1=getInt(s0);	
     else if (s0->t == float_t)
	 arg1=getFloat(s0);

     //-- segundo argumento
     if (s1->t == int_t)
	 arg2=getInt(s1);
     else if (s1->t == float_t)
	 arg2=getFloat(s1);
}
 else
 {
     if (sr->t == int_t)
	 res=getInt(sr);
     else if (sr->t == float_t)
	 res=getFloat(sr);
 }

 
 switch(op_code)
 {
          case OPSOMA:
               res=arg1+arg2;
               break;

          case OPSUB:
	       res=arg1-arg2;
	       break;

          case OPMAIS:
               res=+res;
               break;

          case OPMENOS:
	       res=-res;	
	       break;

          case OPMULT:
               res=arg1*arg2;
               break;

          case OPDIV:
	       res=arg1/arg2;
               break;

          case OPEQU:
	       res=arg1 == arg2;
	       break;

          case OPMOD:
	       res= ((int)arg1 % (int)arg2);
	       break;

	  case OPDIF:
	       res=arg1 != arg2;
	       break;

          case OPMAIOR:
                 res=arg1 > arg2;
                 break;

          case OPMENOR:
		 res=arg1 < arg2;
                 break;

           case OPMAIORIG:
                 res=arg1 >= arg2;
                 break;

           case OPMENORIG:
                 res=arg1 <= arg2;
                 break;
		 
	   case OPBOR:
                 res=arg1 || arg2;
		 printf("res=%d\n",res);
                 break;
		 
          case OPBAND:
                 res=arg1 && arg2;
                 break;

    }

          
    //-- retornando o resultado
    if (sr->t == int_t)
         assign(sr,(int)res);
    else if (sr->t == float_t)
          assign(sr,res);
    
}

int isCodOper(int op_code)
{
    
return  ( (op_code == OPSOMA) ||  (op_code == OPSUB) ||
        (op_code == OPDIV) || (op_code == OPMULT) ||
        (op_code == OPMOD) || (op_code == OPBOR) ||
	(op_code == OPBAND) ||(op_code == OPMENOS) ||
	(op_code == OPMAIS) || (op_code == OPDIF) ||
        (op_code == OPMAIOR) || (op_code == OPMAIORIG) ||
        (op_code == OPMENOR) || (op_code == OPMENORIG) ||
	(op_code == OPEQU) );

}

