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

int inicia_banco(Banco *banco, int num_clientes, int num_recursos, int *available, int **maximum){
    int i, j;

    banco->num_clientes = num_clientes;
    banco->num_recursos = num_recursos;
    banco->available = available;
    banco->maximum = maximum;
    banco->allocation = NULL;
    banco->need = NULL;
    banco->work = NULL;
    banco->finish = NULL;

    banco->allocation = cria_matriz(num_clientes, num_recursos);
    banco->need = cria_matriz(num_clientes, num_recursos);
    banco->work = malloc(num_recursos * sizeof(int));
    banco->finish = malloc(num_clientes * sizeof(int));

    if (banco->allocation == NULL || banco->need == NULL || banco->work == NULL || banco->finish == NULL){
        return 0;
    }

    for (i = 0; i < num_clientes; i++){
        for (j = 0; j < num_recursos; j++){
            banco->need[i][j] = maximum[i][j];
        }
    }

    return 1;
}

void libera_banco(Banco *banco){
    free(banco->available);
    libera_matriz(banco->maximum, banco->num_clientes);
    libera_matriz(banco->allocation, banco->num_clientes);
    libera_matriz(banco->need, banco->num_clientes);
    free(banco->work);
    free(banco->finish);

    banco->available = NULL;
    banco->maximum = NULL;
    banco->allocation = NULL;
    banco->need = NULL;
    banco->work = NULL;
    banco->finish = NULL;
}

static int menor_ou_igual(const int *a, const int *b, int tamanho){
    int j;

    for (j = 0; j < tamanho; j++){
        if (a[j] > b[j]){
            return 0;
        }
    }
    return 1;
}

int estado_seguro(Banco *banco){
    int n = banco->num_clientes;
    int m = banco->num_recursos;
    int i, j;
    int terminados = 0;
    int achou;

    for (j = 0; j < m; j++){
        banco->work[j] = banco->available[j];
    }
    for (i = 0; i < n; i++){
        banco->finish[i] = 0;
    }

    do{
        achou = 0;
        for (i = 0; i < n; i++){
            if (!banco->finish[i] && menor_ou_igual(banco->need[i], banco->work, m)){
                for (j = 0; j < m; j++){
                    banco->work[j] += banco->allocation[i][j];
                }
                banco->finish[i] = 1;
                terminados++;
                achou = 1;
            }
        }
    }while (achou);

    return terminados == n;
}