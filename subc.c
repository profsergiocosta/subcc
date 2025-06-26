
#include <stdio.h>

#include "symbol.h"
#include "mstring.h"
#include "error.h"

#include "subc.h"
#include "lexical.h"
#include "interprete.h"

char *newTemp(TYPE cons=float_t);
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

int numtemp_=1;
int numblock_=1;
int type_ = -1;
char* arg_;
Token *token_;

struct symtab* curTab_=0;

int dec(){

  while (type())
    dec();
  return 1;
}

int type(){

    type_=token_->type;
    if ((type_ == KW_INT) || (type_ == KW_FLOAT))
    {
    	listVar();
	return 1;
    }
    else
    	return 0;
}

void listVar(){

    var();
    int tk =token_->type;
    while (tk == TK_VIRG){
    	var();
     tk = token_->type;
    }
    if (tk == TK_PTOVIRG)
	      token_=nextToken();
    else	
       error(token_->line,EXPEC,";");
}

int ident(){
    
    char* arg1;
    
    if (token_->type == TK_IDENT ) {
	arg_= str(token_->value);
	arg1=str(arg_);
	      
	if(!(findSymbol(curTab_,token_->value)))
	    error(token_->line,UNDEC,"%s",token_->value);
	token_=nextToken();

	if (token_->type == TK_ACOLC){
	    token_=nextToken();
	    expr(); 
	       
	    gen3ai(ACESSIND,arg1,str(arg_),"");
	    arg_=str(arg1);
	    
	    if (token_->type == TK_FCOLC)
		token_=nextToken();
	    else
		error(token_->line,EXPEC,"]");
	}
	return 1;
    }
    else return 0;
}	

void var(){

    token_=nextToken();
    if (token_->type == TK_IDENT){	
	int size=0;
	char* name=str(token_->value);	
	if(findSymbol(curTab_->list,token_->value))
	    error(token_->line,REDEC,"%s",token_->value);


	token_=nextToken();

	if (token_->type == TK_ACOLC){
	    token_=nextToken();

	    size=integer();
	    			
	    if (token_->type == TK_FCOLC)
		token_=nextToken();
	    else	
		error(token_->line,EXPEC,"]");
	}
	
	if (type_ == KW_INT)
	    insertSymbol(curTab_,name,int_t,size);
	
	if (type_ == KW_FLOAT)
	    insertSymbol(curTab_,name,float_t,size);
    }
    else
	error(token_->line,EXPEC,"ident");
}

void block(){
        
    if (token_->type == TK_ACHAVE){
	token_=nextToken();
	curTab_= newTable(curTab_);
	numblock_++; //identificador do bloco
	curTab_->id=numblock_;
	
	dec();
	listComand();

	if (token_->type == TK_FCHAVE){
	    token_=nextToken();	
	    if (curTab_->parent) {
		curTab_=curTab_->parent;
	    }
	}	
	else
	    error(token_->line,EXPEC,"}");
    }
    else
	error(token_->line,EXPEC,"{");

}

void sPrintf(){

    char* arg1;
    char* arg2;
    token_=nextToken();
    if (token_->type == TK_APAREN)
    {
	token_=nextToken();

	if (token_->type == TK_STRING)
	{
	    string str1=token_->value;
	    string str2;
	    token_=nextToken();          
	    while(str1.find_first_of("%") != -1)
	    {	
		str2=str1.substr(0,(str1.find_first_of("%")+2));
		str1.erase(0,(str1.find_first_of("%")+2));
		arg1=newTemp(string_t);
		setValue(findSymbol(curTab_,arg1),(char*) str2.c_str());

		if(token_->type == TK_VIRG)
		{
		    token_=nextToken();
		    expr();
		    arg2= str(arg_);
		    gen3ai(PRINTF,arg1,arg2,"");
		}	
	    }
		
	    arg1=newTemp(string_t);
	    setValue(findSymbol(curTab_,arg1),(char*) str1.c_str());
	    gen3ai(PRINTF,arg1,"","");
	          	
	}
	else
		error(token_->line,EXPEC,"string");
	if (token_->type == TK_FPAREN)
		token_=nextToken();
	else
		error(token_->line,EXPEC,")");
	if (token_->type == TK_PTOVIRG)
		token_=nextToken();
	else
		error(token_->line,EXPEC,";");
    }
    else
	error(token_->line,EXPEC,"(");

}

void sScanf(){

    char* arg1;
    char* arg2;

    token_=nextToken();

    if (token_->type == TK_APAREN){
	token_=nextToken();
			
	if (token_->type == TK_STRING){
	    
    	    string str1=token_->value;
	    string str2;
	    token_=nextToken();          
	    while(str1.find_first_of("%") != -1)
	    {	
		str2=str1.substr(0,(str1.find_first_of("%")+2));
		str1.erase(0,(str1.find_first_of("%")+2));
		arg1=newTemp(string_t);
		setValue(findSymbol(curTab_,arg1),(char*) str2.c_str());

		if(token_->type == TK_VIRG)
		{
		    token_=nextToken();
		    if (token_->type == TK_ENDER)
			token_=nextToken();
		    else
			error(token_->line,EXPEC,"&");
		
		    if (!ident())
			error(token_->line,EXPEC,"ident");
		    
		    arg2=str(arg_);
		    gen3ai(SCANF,arg1,arg2,"");
		}	
	    }
		
	 }
	else
	    error(token_->line,EXPEC,"string");

	if (token_->type == TK_FPAREN)
	    token_=nextToken();
	else	
	    error(token_->line,EXPEC,")");

	if (token_->type == TK_PTOVIRG)
	    token_=nextToken();
	else	
	    error(token_->line,EXPEC,";");
    }
    else
	error(token_->line,EXPEC,"(");

}

void listComand(){

    if (comand())
	while (comand());
    else
	error(token_->line,EXPEC,"comando");

}

void expr(){
    
    char *arg1;
    char *arg2;
    char* res;
    int type;
    exprSimples();

    arg1= str(arg_);

    while (opRel()){
	
	type=token_->type;
	res=newTemp(int_t);
	token_=nextToken();
	exprSimples();
	arg2= str(arg_);

	if (type == TK_IGUAL )
          gen3ai(OPEQU,arg1,arg2,res);

        if (type == TK_DIF )
          gen3ai(OPDIF,arg1,arg2,res);

        if (type == TK_MAIOR )
          gen3ai(OPMAIOR,arg1,arg2,res);

        if (type == TK_MENOR )
          gen3ai(OPMENOR,arg1,arg2,res);

        if (type == TK_MAIORIG )
          gen3ai(OPMAIORIG,arg1,arg2,res);

        if (type == TK_MENORIG )
          gen3ai(OPMENORIG,arg1,arg2,res);
	
        arg1=str(res);
        arg_=str(res);

    }

}

void exprSimples()
{
    char *arg1;
    char *arg2;
    char* res;
    int type;

    sinal();
    
    termo();

    arg1= str(arg_);
    while (opAdit()){
	
	type=token_->type;
	res=newTemp();
	token_=nextToken();
	termo();

	arg2= str(arg_);

        if ((findSymbol(curTab_,arg1)->t == int_t) &&   
           (findSymbol(curTab_,arg2)->t == int_t))
                findSymbol(curTab_,res)->t=int_t;

        if (type == TK_MAIS )
          gen3ai(OPSOMA,arg1,arg2,res);

        if (type == TK_MENOS )
          gen3ai(OPSUB,arg1,arg2,res);

        if (type == TK_OR )
          gen3ai(OPBOR,arg1,arg2,res);

        arg1=str(res);
        arg_=str(res);
    }
    
    if (type_ == TK_MAIS  )
	gen3ai(OPMAIS,"","",str(arg1));
    
    if (type_ == TK_MENOS )
       	gen3ai(OPMENOS,"","",str(arg1));
   
}


void termo(){

    char *arg1;
    char *arg2;
    char* res;
    int type;

    fator();
    arg1= str(arg_);
    while (opMult())	{
	
	type=token_->type;
        res=newTemp();
	token_=nextToken();
        fator();
        arg2= str(arg_);

        if ((findSymbol(curTab_,arg1)->t == int_t) &&   // os dois argumentos inteiro resultado inteiro
           (findSymbol(curTab_,arg2)->t == int_t))
                findSymbol(curTab_,res)->t=int_t;

	if (type == TK_VEZES )
	    gen3ai(OPMULT,arg1,arg2,res);
	
	if (type == TK_DIVISAO )
	    gen3ai(OPDIV,arg1,arg2,res);
	
 	if (type == TK_PORCENT )
	    gen3ai(OPMOD,arg1,arg2,res);
	
	if (type == TK_AND )
	    gen3ai(OPBAND,arg1,arg2,res);
	
	arg1=str(res);
	arg_=str(res);

    }
}

void fator(){

    if ((token_->type == TK_INT) || (token_->type == TK_FLOAT))   {
	
	if (token_->type == TK_INT)
	    newConst(int_t);
	else
	    newConst(float_t);

	token_=nextToken();

    }	
    else
	if (ident());
    else
	if (token_->type == TK_NEG){	
	    token_=nextToken();
	    fator();
	}
    else
	if (token_->type == TK_APAREN)
	{	
	   token_=nextToken();
	   expr();
	   if (token_->type == TK_FPAREN)
	       token_=nextToken();
	   else
	       error(token_->line,EXPEC,")");
       }
    else	
	error(token_->line,EXPEC,"fator");
}

void sinal(){
    type_=token_->type;
    if ((token_->type == TK_MAIS  )
	|| (token_->type == TK_MENOS ))
	token_=nextToken();
}

int opRel(){

 return ((token_->type == TK_IGUAL) ||
            (token_->type == TK_DIF)    ||
            (token_->type == TK_MAIOR)  ||
            (token_->type == TK_MENOR)   ||
            (token_->type == TK_MAIORIG) ||
            (token_->type == TK_MENORIG));
}

int opAdit(){

 return ((token_->type == TK_MAIS) ||
         (token_->type == TK_MENOS)||
         (token_->type == TK_OR));
}

int opMult(){

 return ((token_->type == TK_VEZES)||
        (token_->type == TK_DIVISAO)||
        (token_->type == TK_PORCENT)||
        (token_->type == TK_AND));
}

int integer(){
    int val;

    if (token_->type == TK_INT)
    {
	val=atoi(token_->value);
	token_=nextToken();
	return val;
    }
    else
	error(token_->line,EXPEC,"inteiro");
}

int comand(){

    	if (ident())
	{
		atribuicao();
		return 1;
	}
	
	else if (token_->type == KW_IF)
	{
		ifStruct();
		return 1;
	}

	else if (token_->type == KW_WHILE)
	{
		whileStruct();
		return 1;
	}
	
	else if (token_->type == KW_PRINTF)
	{
		sPrintf();
		return 1;
	}

	else if (token_->type == KW_SCANF)
	{
		sScanf();
		return 1;
	}


	return 0;
}

void ifStruct(){

  int codinst_1;
  int codinst_2;
  
     
  token_=nextToken();

  if (token_->type == TK_APAREN){
    token_=nextToken();
    expr();
    if (token_->type == TK_FPAREN){
      token_=nextToken();
 
      codinst_1=gen3ai(JEQZ,str(arg_),"","");
      gen3ai(BLOCK,ToString(numblock_),"","");
      block();
      codinst_2 = (gen3ai(BLOCKEND,"","","")+1);
      backpatch(codinst_1,ToString(codinst_2));
  
      
    }
    else
	error(token_->line,EXPEC,")");

    if (token_->type == KW_ELSE){
    	token_=nextToken();
	codinst_2 =gen3ai(JUMP,"","","");
	gen3ai(BLOCK,ToString(numblock_),"","");
	backpatch(codinst_1,ToString(codinst_2 + 1));
	block();
    	backpatch(codinst_2,ToString((gen3ai(BLOCKEND,"","","")+1)));
	
    }
  }
  else
	error(token_->line,EXPEC,"(");
}

void whileStruct(){

    int codinst_1,codinst_2,codinst_3,codinst_4;
    token_=nextToken();
    if (token_->type == TK_APAREN)
    {
        token_=nextToken();
        codinst_3=gen3ai(EMPTY,"","","");  // para pode verifvar antes da expressao
	expr();

        if (token_->type == TK_FPAREN){
            token_=nextToken();
     	    
	     codinst_1=gen3ai(JEQZ,str(arg_),"","");
	     gen3ai(BLOCK,ToString(numblock_),"","");
	     block();
	     (gen3ai(BLOCKEND,"","","")+1);
	     gen3ai(JUMP,ToString(codinst_3),"","");
	     codinst_2 = gen3ai(EMPTY,"","","");
	     backpatch(codinst_1,ToString(codinst_2));
    	    
        }
	else
		error(token_->line,EXPEC,")");
    }
    else
	error(token_->line,EXPEC,"(");

}


void atribuicao(){

    char *arg1;
    char *arg2;
    char* res;

    if (token_->type == TK_ATRIBUICAO)
    {	

	token_=nextToken();
 
	res=str(arg_);
	
	expr();
 
	arg1=str(arg_);
        gen3ai(OPATR,arg1,0,res);

	if (token_->type == TK_PTOVIRG)
		token_=nextToken();
	else
		error(token_->line,EXPEC,";");
    }
    else
	error(token_->line,EXPEC,"=");
}

void programa(){
    
    token_=nextToken();
    if (token_->type == KW_MAIN ){
       	token_=nextToken();
	if (token_->type == TK_APAREN)
	{
		token_=nextToken();
		if (token_->type == TK_FPAREN)
		{
			token_=nextToken();
			block();
		}
		else
		    error(token_->line,EXPEC,")");
	}
	else
		error(token_->line,EXPEC,"(");
    }
    else
	error(token_->line,EXPEC,"Main");

}

int executa(const char* filename){

    if (openFile(filename))	{

        init();
	programa();
	token_=nextToken();
	closeFile();
        interprete(rootTable());
	return 1;

    }
    else{
	printf("error to open file: %s\n",filename);
	return 0;
    }
}

char *newTemp(TYPE cons){

  char Buf[10];
  sprintf(Buf, "%i",numtemp_);

  char var[50];
  strcpy (var,"_TEMP_");
  strcat(var,Buf);

   insertSymbol(curTab_,str(var),cons);

  numtemp_++;

  return  str(var);
}


void newConst(TYPE cons){

  char Buf[10];
  sprintf(Buf, "%i",numtemp_);

  char cons_name[50];
  strcpy (cons_name,"_CONST_");
  strcat(cons_name,Buf);

  insertSymbol(curTab_,str(cons_name),cons);
  
  if (cons == int_t)
    setValue(findSymbol(curTab_,cons_name), atoi(token_->value));

  else if (cons == string_t)
    setValue(findSymbol(curTab_,cons_name), token_->value);

  else if (cons == float_t)
    setValue(findSymbol(curTab_,cons_name), (float)atof(token_->value));

  arg_=str(cons_name);

  numtemp_++;
  
}


