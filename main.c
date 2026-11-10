#include <stdio.h>
#include <stdlib.h>
#include "banker.h"
#include "entrada.h"

int main(int argc, char *argv[]){
    int num_recursos;
    int num_clientes;
    int *available;
    int **maximum;
    int i, j;

    available = ler_argumentos(argc, argv, &num_recursos);
    if (available == NULL){
        return EXIT_FAILURE;
    }

    maximum = ler_clientes("customer.txt", num_recursos, &num_clientes);
    if (maximum == NULL){
        free(available);
        return EXIT_FAILURE;
    }

    printf("num_recursos = %d\navailable =", num_recursos);
    for (i = 0; i < num_recursos; i++){
        printf(" %d", available[i]);
    }
    printf("\nnum_clientes = %d\nmaximum:\n", num_clientes);
    for (i = 0; i < num_clientes; i++){
        for (j = 0; j < num_recursos; j++){
            printf(" %d", maximum[i][j]);
        }
        printf("\n");
    }

    libera_matriz(maximum, num_clientes);
    free(available);
    return EXIT_SUCCESS;
}