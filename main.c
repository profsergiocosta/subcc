

/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Seg Mai 12 16:27:21 BRT 2003
    copyright            : (C) 2025 by Prof. Sergio Souza Costa  
    email                : prof.sergio.costa@gmail.com
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "subc.h"
#include "interprete.h"
static char* program_name = NULL;

/**
 * Exibe mensagem de uso e finaliza o programa.
 */
static void print_usage(FILE* stream, int exit_code) {
    fprintf(stream, "Usage: %s inputfile\n", program_name);
    exit(exit_code);
}

int main(int argc, char* argv[]) {
    program_name = argv[0];

    if (argc > 1) {
        if (!executa(argv[1])) {
            fprintf(stderr, "Erro ao executar o arquivo: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        
    } else {
        print_usage(stdout, EXIT_FAILURE);
    }

     //disassemble();      // imprime as instruções geradas
    return EXIT_SUCCESS;
}
