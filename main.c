#include <stdio.h>
#include <stdlib.h>
#include "banker.h"
#include "entrada.h"

#define ARQUIVO_CLIENTES "customer.txt"
#define ARQUIVO_COMANDOS "commands.txt"
#define ARQUIVO_SAIDA "result.txt"

int main(int argc, char *argv[]){
    int num_recursos;
    int num_clientes;
    int *available;
    int **maximum;
    Banco banco;
    FILE *saida;
    int comandos_validados;
    int comandos_executados;

    available = ler_argumentos(argc, argv, &num_recursos);
    if (available == NULL){
        return EXIT_FAILURE;
    }

    maximum = ler_clientes(ARQUIVO_CLIENTES, num_recursos, &num_clientes);
    if (maximum == NULL){
        free(available);
        return EXIT_FAILURE;
    }

    if (!inicia_banco(&banco, num_clientes, num_recursos, available, maximum)){
        fprintf(stderr, "Error: not enough memory.\n");
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    comandos_validados = processa_comandos(ARQUIVO_COMANDOS, &banco, NULL);
    if (comandos_validados < 0){
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    saida = fopen(ARQUIVO_SAIDA, "w");
    if (saida == NULL){
        fprintf(stderr, "Error: could not create %s.\n", ARQUIVO_SAIDA);
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    comandos_executados = processa_comandos(ARQUIVO_COMANDOS, &banco, saida);

    if (comandos_executados != comandos_validados){
        if (comandos_executados >= 0){
            fprintf(stderr, "Error: %s changed while it was being read.\n", ARQUIVO_COMANDOS);
        }
        fclose(saida);
        remove(ARQUIVO_SAIDA);
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    if (ferror(saida) || fclose(saida) != 0){
        fprintf(stderr, "Error: could not write %s.\n", ARQUIVO_SAIDA);
        remove(ARQUIVO_SAIDA);
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    libera_banco(&banco);
    return EXIT_SUCCESS;
}