#ifndef ENTRADA_H
#define ENTRADA_H
#include "banker.h"

int converte_inteiro(const char *texto, int *resultado);
int *ler_argumentos(int argc, char *argv[], int *num_recursos);
int **ler_clientes(const char *nome_arquivo, int num_recursos, int *num_clientes);
#endif