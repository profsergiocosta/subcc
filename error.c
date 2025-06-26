#include "error.h"

void error(int line,int cod,char *fmt, ...){

    va_list args;

    fputs("Encontrado erro", stderr);
	
    if (line != 0)
	fprintf(stderr," na linha %d",line);
	
    fprintf(stderr,": '",line);
	
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
	
    fprintf(stderr,"'",line);

    switch(cod)
    {
        case UNDEC: fputs(" ainda nao declarado!\n", stderr); break;
        case REDEC: fputs(" previamente declarado!\n", stderr); break;
        case EXPEC: fputs(" esperado!\n", stderr); break;
    }

    exit(1);
}



/* aborts on failure */
void *fmalloc(size_t s){

 void *ptr = malloc(s);

 if (!ptr) {

    fprintf(stderr,"Out of memory in fmalloc(%d)\n",s);
    exit(1);

    }

    return ptr;
}
