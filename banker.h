#ifndef BANKER_H
#define BANKER_H

typedef struct Banco{
    int num_clientes;
    int num_recursos;
    int *available;
    int **maximum;
    int **allocation;
    int **need;
    int *work;
    int *finish;
} Banco;

int **cria_matriz(int linhas, int colunas);
void libera_matriz(int **matriz, int linhas);
int inicia_banco(Banco *banco, int num_clientes, int num_recursos, int *available, int **maximum);
void libera_banco(Banco *banco);
int estado_seguro(Banco *banco);

#endif