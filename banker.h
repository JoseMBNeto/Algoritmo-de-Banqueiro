#ifndef BANKER_H
#define BANKER_H
#include <stdio.h>

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
int processa_requisicao(Banco *banco, int cliente, const int *request, FILE *saida);
int processa_liberacao(Banco *banco, int cliente, const int *release, FILE *saida);
void imprime_estado(Banco *banco, FILE *saida);

#endif