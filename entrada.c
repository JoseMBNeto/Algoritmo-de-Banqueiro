#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "entrada.h"
#define TAMANHO_LINHA 4096
#define LINHA_OK        1
#define FIM_DO_ARQUIVO  0
#define LINHA_GRANDE   -1

int converte_inteiro(const char *texto, int *resultado){
    int valor = 0;
    int i;

    if (texto == NULL || texto[0] == '\0'){
        return 0;
    }

    for (i = 0; texto[i] != '\0'; i++){
        int digito;

        if (texto[i] < '0' || texto[i] > '9'){
            return 0;
        }

        digito = texto[i] - '0';

        if (valor > (INT_MAX - digito) / 10){
            return 0;
        }

        valor = valor * 10 + digito;
    }

    *resultado = valor;
    return 1;
}

int *ler_argumentos(int argc, char *argv[], int *num_recursos){
    int m = argc - 1;
    int *available;
    int i;

    if (m < 1){
        fprintf(stderr, "Error: no resources were given.\n");
        fprintf(stderr, "Usage: %s <instances of resource 1> <instances of resource 2> ...\n", argv[0]);
        return NULL;
    }

    available = malloc(m * sizeof(int));

    if (available == NULL){
        fprintf(stderr, "Error: not enough memory.\n");
        return NULL;
    }

    for (i = 0; i < m; i++){
        if (!converte_inteiro(argv[i + 1], &available[i])) {
            fprintf(stderr, "Error: argument %d (\"%s\") is not a valid non-negative integer.\n", i + 1, argv[i + 1]);
            free(available);
            return NULL;
        }
    }

    *num_recursos = m;
    return available;
}

static int le_linha(FILE *arquivo, char *buffer, int tamanho){
    size_t len;

    if (fgets(buffer, tamanho, arquivo) == NULL){
        return FIM_DO_ARQUIVO;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
        len--;
    }else{
        int proximo = fgetc(arquivo);
        if (proximo != EOF) {
            return LINHA_GRANDE;
        }
    }
    if (len > 0 && buffer[len - 1] == '\r'){
        buffer[len - 1] = '\0';
    }

    return LINHA_OK;
}

static int conta_campos(const char *linha, char separador){
    int campos = 1;
    int i;

    for (i = 0; linha[i] != '\0'; i++){
        if (linha[i] == separador){
            campos++;
        }
    }
    return campos;
}

static int converte_campos(char *linha, char separador, int *valores, int quantidade, char **campo_invalido){
    char *inicio = linha;
    int i;

    for (i = 0; i < quantidade; i++){
        char *fim = inicio;
        int era_ultimo;

        while (*fim != separador && *fim != '\0'){
            fim++;
        }

        era_ultimo = (*fim == '\0');
        *fim = '\0';

        if (!converte_inteiro(inicio, &valores[i])){
            *campo_invalido = inicio;
            return 0;
        }

        if (!era_ultimo){
            inicio = fim + 1;
        }else{
            inicio = fim;
        }
    }

    return 1;
}

static int passada_clientes(FILE *arquivo, const char *nome_arquivo, int num_recursos, int **maximum, int capacidade, int *temporario){
    char linha[TAMANHO_LINHA];
    int num_linha = 0;
    int clientes = 0;
    int primeira_vazia = 0;
    int status;

    while ((status = le_linha(arquivo, linha, TAMANHO_LINHA)) != FIM_DO_ARQUIVO){
        int campos;
        int *destino;
        char *invalido;

        num_linha++;

        if (status == LINHA_GRANDE){
            fprintf(stderr, "Error: %s line %d: line is too long (limit is %d characters).\n", nome_arquivo, num_linha, TAMANHO_LINHA - 2);
            return -1;
        }

        if (linha[0] == '\0'){
            if (primeira_vazia == 0) {
                primeira_vazia = num_linha;
            }
            continue;
        }

        if (primeira_vazia != 0){
            fprintf(stderr, "Error: %s line %d: empty line (empty lines are only allowed at the end of the file).\n", nome_arquivo, primeira_vazia);
            return -1;
        }

        campos = conta_campos(linha, ',');

        if (campos != num_recursos){
            fprintf(stderr, "Error: %s line %d: expected %d values (one per resource given on the command line), but found %d.\n", nome_arquivo, num_linha, num_recursos, campos);
            return -1;
        }

        if (maximum != NULL){
            if (clientes >= capacidade){
                fprintf(stderr, "Error: %s changed while it was being read.\n", nome_arquivo);
                return -1;
            }
            destino = maximum[clientes];
        }else{
            destino = temporario;
        }

        if (!converte_campos(linha, ',', destino, campos, &invalido)){
            fprintf(stderr, "Error: %s line %d: \"%s\" is not a valid non-negative integer.\n", nome_arquivo, num_linha, invalido);
            return -1;
        }

        clientes++;
    }

    if (ferror(arquivo)){
        fprintf(stderr, "Error: could not read %s.\n", nome_arquivo);
        return -1;
    }

    return clientes;
}

int **ler_clientes(const char *nome_arquivo, int num_recursos, int *num_clientes){
    FILE *arquivo;
    int *temporario;
    int **maximum;
    int n;
    int conferencia;

    arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL){
        fprintf(stderr, "Error: could not open %s (it does not exist or cannot be read).\n", nome_arquivo);
        return NULL;
    }

    temporario = malloc(num_recursos * sizeof(int));

    if (temporario == NULL){
        fprintf(stderr, "Error: not enough memory.\n");
        fclose(arquivo);
        return NULL;
    }

    n = passada_clientes(arquivo, nome_arquivo, num_recursos, NULL, 0, temporario);
    free(temporario);

    if (n < 0){
        fclose(arquivo);
        return NULL;
    }
    if (n == 0){
        fprintf(stderr, "Error: %s has no customers.\n", nome_arquivo);
        fclose(arquivo);
        return NULL;
    }

    maximum = cria_matriz(n, num_recursos);

    if (maximum == NULL){
        fprintf(stderr, "Error: not enough memory.\n");
        fclose(arquivo);
        return NULL;
    }

    rewind(arquivo);
    conferencia = passada_clientes(arquivo, nome_arquivo, num_recursos, maximum, n, NULL);
    fclose(arquivo);

    if (conferencia != n){
        if (conferencia >= 0){
            fprintf(stderr, "Error: %s changed while it was being read.\n", nome_arquivo);
        }
        libera_matriz(maximum, n);
        return NULL;
    }

    *num_clientes = n;
    return maximum;
}

static int interpreta_comando(char *linha, int num_linha, const char *nome_arquivo, Banco *banco, int *valores, char *tipo){
    int m = banco->num_recursos;
    int campos;
    char *invalido;

    if (strcmp(linha, "*") == 0){
        *tipo = '*';
        return 1;
    }

    if (linha[0] == 'R' && (linha[1] == 'Q' || linha[1] == 'L') && (linha[2] == ' ' || linha[2] == '\0')){
        *tipo = linha[1];
    }else{
        fprintf(stderr, "Error: %s line %d: unknown command \"%s\" (expected RQ, RL or *).\n", nome_arquivo, num_linha, linha);
        return 0;
    }

    if (linha[2] == '\0'){
        campos = 0;
    }else{
        campos = conta_campos(linha + 3, ' ');
    }

    if (campos != m + 1){
        fprintf(stderr, "Error: %s line %d: expected %d values after R%c (customer number + %d resources given on the command line), but found %d.\n", nome_arquivo, num_linha, m + 1, *tipo, m, campos);
        return 0;
    }

    if (!converte_campos(linha + 3, ' ', valores, campos, &invalido)){
        fprintf(stderr, "Error: %s line %d: \"%s\" is not a valid non-negative integer.\n", nome_arquivo, num_linha, invalido);
        return 0;
    }

    if (valores[0] >= banco->num_clientes){
        fprintf(stderr, "Error: %s line %d: customer %d does not exist (valid customers: 0 to %d).\n", nome_arquivo, num_linha, valores[0], banco->num_clientes - 1);
        return 0;
    }

    return 1;
}

static int passada_comandos(FILE *arquivo, const char *nome_arquivo, Banco *banco, FILE *saida, int *valores){
    char linha[TAMANHO_LINHA];
    int num_linha = 0;
    int comandos = 0;
    int primeira_vazia = 0;
    int status;

    while ((status = le_linha(arquivo, linha, TAMANHO_LINHA)) != FIM_DO_ARQUIVO){
        char tipo;
        num_linha++;

        if (status == LINHA_GRANDE){
            fprintf(stderr, "Error: %s line %d: line is too long (limit is %d characters).\n", nome_arquivo, num_linha, TAMANHO_LINHA - 2);
            return -1;
        }

        if (linha[0] == '\0'){
            if (primeira_vazia == 0){
                primeira_vazia = num_linha;
            }
            continue;
        }

        if (primeira_vazia != 0){
            fprintf(stderr, "Error: %s line %d: empty line (empty lines are only allowed at the end of the file).\n", nome_arquivo, primeira_vazia);
            return -1;
        }

        if (!interpreta_comando(linha, num_linha, nome_arquivo, banco, valores, &tipo)){
            return -1;
        }

        if (saida != NULL){
            if (tipo == 'Q'){
                processa_requisicao(banco, valores[0], valores + 1, saida);
            }else if (tipo == 'L'){
                processa_liberacao(banco, valores[0], valores + 1, saida);
            }else{
                imprime_estado(banco, saida);
            }
        }

        comandos++;
    }

    if (ferror(arquivo)){
        fprintf(stderr, "Error: could not read %s.\n", nome_arquivo);
        return -1;
    }

    return comandos;
}

int processa_comandos(const char *nome_arquivo, Banco *banco, FILE *saida){
    FILE *arquivo;
    int *valores;
    int comandos;

    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL){
        fprintf(stderr, "Error: could not open %s (it does not exist or cannot be read).\n", nome_arquivo);
        return -1;
    }

    valores = malloc((banco->num_recursos + 1) * sizeof(int));
    if (valores == NULL){
        fprintf(stderr, "Error: not enough memory.\n");
        fclose(arquivo);
        return -1;
    }

    comandos = passada_comandos(arquivo, nome_arquivo, banco, saida, valores);

    free(valores);
    fclose(arquivo);

    if (comandos == 0){
        fprintf(stderr, "Error: %s has no commands.\n", nome_arquivo);
        return -1;
    }

    return comandos;
}