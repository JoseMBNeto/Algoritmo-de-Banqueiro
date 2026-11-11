#include <stdio.h>
#include <stdlib.h>
#include "banker.h"
#include "entrada.h"

static void da_recursos(Banco *banco, int cliente, const int *recursos){
    int j;

    for (j = 0; j < banco->num_recursos; j++){
        banco->available[j] -= recursos[j];
        banco->allocation[cliente][j] += recursos[j];
        banco->need[cliente][j] -= recursos[j];
    }
}

static void teste_temporario(Banco *banco){
    int alocacoes[5][3] = {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}};
    int pedido_cliente1[3] = {1, 0, 2};
    int pedido_cliente0[3] = {0, 2, 0};
    int i;

    printf("Teste 1 (ninguem tem nada): %s\n", estado_seguro(banco) ? "SAFE" : "UNSAFE");

    if (banco->num_clientes != 5 || banco->num_recursos != 3 || banco->available[0] != 10 || banco->available[1] != 5 || banco->available[2] != 7){
        printf("(os testes 2 a 4 precisam de ./banker 10 5 7)\n");
        return;
    }

    for (i = 0; i < 5; i++){
        da_recursos(banco, i, alocacoes[i]);
    }
    printf("Teste 2 (estado do '*', available = %d %d %d): %s\n", banco->available[0], banco->available[1], banco->available[2], estado_seguro(banco) ? "SAFE" : "UNSAFE");

    da_recursos(banco, 1, pedido_cliente1);

    printf("Teste 3 (cliente 1 recebe 1 0 2): %s\n", estado_seguro(banco) ? "SAFE" : "UNSAFE");

    da_recursos(banco, 0, pedido_cliente0);

    printf("Teste 4 (cliente 0 recebe 0 2 0): %s\n", estado_seguro(banco) ? "SAFE" : "UNSAFE");
}

int main(int argc, char *argv[]){
    int num_recursos;
    int num_clientes;
    int *available;
    int **maximum;
    Banco banco;

    available = ler_argumentos(argc, argv, &num_recursos);
    if (available == NULL){
        return EXIT_FAILURE;
    }

    maximum = ler_clientes("customer.txt", num_recursos, &num_clientes);
    if (maximum == NULL){
        free(available);
        return EXIT_FAILURE;
    }

    if (!inicia_banco(&banco, num_clientes, num_recursos, available, maximum)){
        fprintf(stderr, "Error: not enough memory.\n");
        libera_banco(&banco);
        return EXIT_FAILURE;
    }

    teste_temporario(&banco);

    libera_banco(&banco);
    return EXIT_SUCCESS;
}