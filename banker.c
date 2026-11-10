#include <stdio.h>
#include <stdlib.h>
#include "banker.h"

int **cria_matriz(int linhas, int colunas){
    int **matriz;
    int i;

    matriz = malloc(linhas * sizeof(int *));
    if (matriz == NULL){
        return NULL;
    }

    for (i = 0; i < linhas; i++){
        matriz[i] = calloc(colunas, sizeof(int));
        if (matriz[i] == NULL) {
            libera_matriz(matriz, i);
            return NULL;
        }
    }

    return matriz;
}

void libera_matriz(int **matriz, int linhas){
    int i;

    if (matriz == NULL){
        return;
    }

    for (i = 0; i < linhas; i++){
        free(matriz[i]);
    }
    free(matriz);
}