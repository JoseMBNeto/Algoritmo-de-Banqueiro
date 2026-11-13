# Algoritmo-de-Banqueiro

### Aluno: José Milton Batista Neto
### Turma: 3A

## Sistema Operacional

Implementado e testado no **Windows + WSL2**

## Arquivos

| Arquivo | Descrição |
|---|---|
| main.c | Coordena o programa: lê os argumentos e o customer.txt, valida o commands.txt inteiro e só depois executa os comandos, gerando o result.txt |
| entrada.h / entrada.c | Leitura e validação dos argumentos, do customer.txt e do commands.txt |
| banker.h / banker.c | Define a struct Banco (available, maximum, allocation, need) e implementa o algoritmo de segurança, os comandos RQ, RL e * |
| testes.sh | Testes gerados pelo Claude: 43 casos de erro e de limite + teste de força bruta com 200 cenários aleatórios |
| Makefile | Compila o projeto, roda os testes, cria o evidencias.log e limpa os arquivos gerados |
| evidencias.log | Registro das sessões de terminal |

## Como compilar

```bash
make