/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Seg Mai 12 16:27:21 BRT 2003
    copyright            : (C) 2003 by Sergio Souza Costa  
    email                : souzasc@yahoo.com
 ***************************************************************************/

#include "subc.h"
#include "stdio.h"

const char* program_name;

void print_usage(FILE* stream,int exit_code)
{
    fprintf(stream,"Usage: %s inputfile\n",program_name);
    exit(exit_code);
}
int main(int argc, char *argv[])
{
   program_name=argv[0]; 
   
   if (argc > 1)
       executa(argv[1]);
   else
       print_usage(stdout,1);
   return 0;
}

 
