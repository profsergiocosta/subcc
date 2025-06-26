#ifndef ERROR_H
#define ERROR_H

#include  <stdlib.h>
#include  <stdio.h>
#include  <stdarg.h>


enum {UNDEC,REDEC,EXPEC};


/* malloc() version that aborts on failure */
void *fmalloc(size_t);	

//mensagens de erro
void error(int line,int cod,char *fmt, ...);
//void expected(int line,char *fmt, ...);

#endif
