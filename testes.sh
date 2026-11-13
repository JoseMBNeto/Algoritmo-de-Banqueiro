#!/bin/bash
#
# testes.sh - bateria de testes do banker
#
# Uso:  make teste   (compila, roda tudo e grava no evidencias.log)
#
# Os testes rodam numa pasta temporaria (testes_tmp), entao os arquivos
# customer.txt, commands.txt e result.txt da sua pasta NUNCA sao alterados.

passou=0
falhou=0

if [ ! -x ./banker ]; then
    echo "Compile antes com: make"
    exit 1
fi

rm -rf testes_tmp
mkdir testes_tmp
cp banker testes_tmp/
cd testes_tmp || exit 1

# ---------------------------------------------------------------------------
# Arquivos de exemplo do professor (escritos com printf para os espacos do
# fim das linhas nao sumirem se o editor apagar espacos sobrando)
# ---------------------------------------------------------------------------
CLIENTES_PROF='7,5,3\n3,2,2\n9,0,2\n2,2,2\n4,3,3\n'
COMANDOS_PROF='RQ 0 0 2 1\nRQ 2 3 0 2\nRQ 1 2 0 0\nRQ 4 0 0 2\nRL 0 0 1 1\nRQ 3 2 1 1\n*\nRQ 4 4 0 0\nRQ 1 1 0 2\nRQ 0 0 2 0\nRQ 2 0 1 0\n'

printf 'Allocate to customer 0 the resources 0 2 1\n' > esperado.txt
printf 'Allocate to customer 2 the resources 3 0 2\n' >> esperado.txt
printf 'Allocate to customer 1 the resources 2 0 0\n' >> esperado.txt
printf 'Allocate to customer 4 the resources 0 0 2\n' >> esperado.txt
printf 'Release from customer 0 the resources 0 1 1\n' >> esperado.txt
printf 'Allocate to customer 3 the resources 2 1 1\n' >> esperado.txt
printf 'MAXIMUM   |   ALLOCATION  |   NEED\n' >> esperado.txt
printf '7 5 3     |   0 1 0       |   7 4 3 \n' >> esperado.txt
printf '3 2 2     |   2 0 0       |   1 2 2 \n' >> esperado.txt
printf '9 0 2     |   3 0 2       |   6 0 0 \n' >> esperado.txt
printf '2 2 2     |   2 1 1       |   0 1 1 \n' >> esperado.txt
printf '4 3 3     |   0 0 2       |   4 3 1 \n' >> esperado.txt
printf 'AVAILABLE 3 3 2 \n' >> esperado.txt
printf 'The resources 3 3 2 are not enough to customer 4 request 4 0 0 \n' >> esperado.txt
printf 'Allocate to customer 1 the resources 1 0 2\n' >> esperado.txt
printf 'The customer 0 request 0 2 0 was denied because result in an unsafe state\n' >> esperado.txt
printf 'The customer 2 request 0 1 0 was denied because exceed its maximum allocation\n' >> esperado.txt

# ---------------------------------------------------------------------------
# caso DESCRICAO ARGUMENTOS CLIENTES COMANDOS CODIGO_ESPERADO
#   CLIENTES / COMANDOS: conteudo do arquivo (use \n para quebrar linha),
#                        ou SEM_ARQUIVO para o arquivo nao existir.
#   CODIGO_ESPERADO: 0 = deve rodar normalmente, 1 = deve dar erro
# ---------------------------------------------------------------------------
caso() {
    local descricao="$1"
    local argumentos="$2"
    local clientes="$3"
    local comandos="$4"
    local esperado="$5"
    local obtido

    echo "--------------------------------------------------------------"
    echo "TESTE: $descricao"

    rm -f customer.txt commands.txt result.txt
    if [ "$clientes" != "SEM_ARQUIVO" ]; then
        printf -- "$clientes" > customer.txt
    fi
    if [ "$comandos" != "SEM_ARQUIVO" ]; then
        printf -- "$comandos" > commands.txt
    fi

    echo "\$ ./banker $argumentos"
    ./banker $argumentos
    obtido=$?

    if [ "$obtido" -ne "$esperado" ]; then
        echo ">> FALHOU: codigo de saida esperado $esperado, obtido $obtido"
        falhou=$((falhou + 1))
    elif [ "$esperado" -ne 0 ] && [ -f result.txt ]; then
        echo ">> FALHOU: deu erro, mas o result.txt foi criado"
        falhou=$((falhou + 1))
    else
        echo ">> OK (codigo de saida $obtido)"
        passou=$((passou + 1))
    fi

    if [ "$obtido" -eq 0 ] && [ -f result.txt ]; then
        echo "result.txt:"
        cat result.txt
    fi
}

echo "=============================================================="
echo " 1. EXEMPLO DO PROFESSOR"
echo "=============================================================="
caso "arquivos do professor" "10 5 7" "$CLIENTES_PROF" "$COMANDOS_PROF" 0
echo "\$ diff result.txt esperado.txt"
if diff result.txt esperado.txt; then
    echo ">> OK: result.txt IDENTICO ao do professor"
    passou=$((passou + 1))
else
    echo ">> FALHOU: result.txt diferente do professor"
    falhou=$((falhou + 1))
fi

echo "=============================================================="
echo " 2. ARGUMENTOS DA LINHA DE COMANDO"
echo "=============================================================="
caso "sem argumentos"                  ""               "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "argumento com letra"             "10 abc 7"       "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "argumento negativo"              "10 -5 7"        "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "argumento decimal"               "10 5.5 7"       "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "argumento com sinal +"           "10 +5 7"        "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "argumento maior que um int"      "10 5 2147483648" "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "recursos a menos que o arquivo"  "10 5"           "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "recursos a mais que o arquivo"   "10 5 7 1"       "$CLIENTES_PROF" "$COMANDOS_PROF" 1
caso "recurso com 0 instancias (valido)" "0 0 0"        '0,0,0\n'        'RQ 0 0 0 0\n*\n' 0

echo "=============================================================="
echo " 3. customer.txt"
echo "=============================================================="
caso "customer.txt nao existe"         "10 5 7" "SEM_ARQUIVO"       "$COMANDOS_PROF" 1
caso "customer.txt vazio"              "10 5 7" ""                  "$COMANDOS_PROF" 1
caso "numero faltando (7,5)"           "10 5 7" '7,5,3\n7,5\n'      "RQ 0 1 0 0\n" 1
caso "numero sobrando (7,5,3,1)"       "10 5 7" '7,5,3,1\n'         "RQ 0 1 0 0\n" 1
caso "campo vazio (7,,3)"              "10 5 7" '7,,3\n'            "RQ 0 1 0 0\n" 1
caso "virgula no fim (7,5,3,)"         "10 5 7" '7,5,3,\n'          "RQ 0 1 0 0\n" 1
caso "letras (a,b,c)"                  "10 5 7" 'a,b,c\n'           "RQ 0 1 0 0\n" 1
caso "espacos (7, 5, 3)"               "10 5 7" '7, 5, 3\n'         "RQ 0 1 0 0\n" 1
caso "negativo (-7,5,3)"               "10 5 7" '-7,5,3\n'          "RQ 0 1 0 0\n" 1
caso "linha vazia no meio"             "10 5 7" '7,5,3\n\n3,2,2\n'  "RQ 0 1 0 0\n" 1
caso "linhas vazias no fim (valido)"   "10 5 7" '7,5,3\n\n\n'       "RQ 0 1 0 0\n" 0
caso "quebra de linha Windows (valido)" "10 5 7" '7,5,3\r\n3,2,2\r\n' "RQ 0 1 0 0\n" 0
caso "sem \\n na ultima linha (valido)" "10 5 7" '7,5,3\n3,2,2'     "RQ 0 1 0 0\n" 0
caso "maximo maior que o total (valido, pedido negado)" "10 5 7" '11,0,0\n' "RQ 0 1 0 0\n" 0

echo "=============================================================="
echo " 4. commands.txt"
echo "=============================================================="
caso "commands.txt nao existe"         "10 5 7" "$CLIENTES_PROF" "SEM_ARQUIVO"          1
caso "commands.txt vazio"              "10 5 7" "$CLIENTES_PROF" ""                     1
caso "comando desconhecido (XX)"       "10 5 7" "$CLIENTES_PROF" 'XX 0 1 0 0\n'         1
caso "comando minusculo (rq)"          "10 5 7" "$CLIENTES_PROF" 'rq 0 1 0 0\n'         1
caso "RQ sem numeros"                  "10 5 7" "$CLIENTES_PROF" 'RQ\n'                 1
caso "recurso faltando"                "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0\n'           1
caso "recurso sobrando"                "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0 0 0\n'       1
caso "espaco duplo"                    "10 5 7" "$CLIENTES_PROF" 'RQ  0 1 0 0\n'        1
caso "espaco no fim da linha"          "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0 0 \n'        1
caso "numero negativo"                 "10 5 7" "$CLIENTES_PROF" 'RQ 0 -1 0 0\n'        1
caso "cliente inexistente (5)"         "10 5 7" "$CLIENTES_PROF" 'RQ 5 1 0 0\n'         1
caso "asterisco com espaco"            "10 5 7" "$CLIENTES_PROF" '* \n'                 1
caso "linha vazia no meio"             "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0 0\n\n*\n'    1
caso "erro so na ultima linha (nada pode ser executado)" "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0 0\n*\nRQ 9 0 0 0\n' 1
caso "quebra de linha Windows (valido)" "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 0 0\r\n*\r\n' 0
caso "pedido zero (valido)"            "10 5 7" "$CLIENTES_PROF" 'RQ 0 0 0 0\n'         0
caso "RL de quem nao tem (valido, recusado no result.txt)" "10 5 7" "$CLIENTES_PROF" 'RL 1 1 0 0\n' 0
caso "RQ, RL e RL de novo (valido)"    "10 5 7" "$CLIENTES_PROF" 'RQ 0 1 1 1\nRL 0 1 1 1\nRL 0 1 0 0\n*\n' 0

echo "=============================================================="
echo " 5. FORCA BRUTA (sequencias aleatorias)"
echo "=============================================================="
# Em cada rodada: sorteia clientes, recursos e 30 comandos RQ/RL, com um '*'
# no inicio e depois de cada comando. Depois o awk confere o result.txt inteiro:
#   - cada tabela: need = maximum - allocation, nada negativo,
#     available + soma das allocations = total, e o estado e SEGURO
#   - cada mensagem: a decisao tomada estava CERTA, olhando o estado de antes
#     (ex.: "unsafe state" so pode aparecer se conceder o pedido deixasse o estado inseguro)
# O algoritmo de seguranca e refeito no awk, independente do codigo em C.
RODADAS=200
COMANDOS_POR_RODADA=30
falhas_forca_bruta=0

for ((r = 1; r <= RODADAS; r++)); do
    m=$((RANDOM % 4 + 1))          # 1 a 4 recursos
    n=$((RANDOM % 5 + 1))          # 1 a 5 clientes

    # Total de cada recurso (0 a 9)
    total=""
    for ((j = 0; j < m; j++)); do
        t[$j]=$((RANDOM % 10))
        total="$total ${t[$j]}"
    done

    # customer.txt: maximo de cada cliente entre 0 e o total (estado inicial seguro)
    : > customer.txt
    for ((i = 0; i < n; i++)); do
        linha=""
        for ((j = 0; j < m; j++)); do
            if [ "$j" -gt 0 ]; then linha="$linha,"; fi
            linha="$linha$((RANDOM % (t[$j] + 1)))"
        done
        echo "$linha" >> customer.txt
    done

    # commands.txt: '*' no inicio e depois de cada comando; 2 em cada 3 sao RQ
    echo "*" > commands.txt
    for ((k = 0; k < COMANDOS_POR_RODADA; k++)); do
        if [ $((RANDOM % 3)) -lt 2 ]; then linha="RQ"; else linha="RL"; fi
        linha="$linha $((RANDOM % n))"
        for ((j = 0; j < m; j++)); do
            linha="$linha $((RANDOM % 4))"
        done
        echo "$linha" >> commands.txt
        echo "*" >> commands.txt
    done

    ./banker $total
    if [ $? -ne 0 ]; then
        echo ">> FALHOU na rodada $r: o programa terminou com erro (./banker$total)"
        falhas_forca_bruta=$((falhas_forca_bruta + 1))
        continue
    fi

    awk -v n="$n" -v m="$m" -v total="$total" -v comandos="$COMANDOS_POR_RODADA" '
    function erro(texto) {
        print ">> " texto " (result.txt linha " NR ": " $0 ")"
        erros++
    }
    # Algoritmo de seguranca (mesma ideia do livro), usando AV, AL e NE
    function seguro(    i, j, W, F, terminados, achou, cabe) {
        for (j = 1; j <= m; j++) W[j] = AV[j]
        for (i = 1; i <= n; i++) F[i] = 0
        terminados = 0
        do {
            achou = 0
            for (i = 1; i <= n; i++) {
                if (F[i]) continue
                cabe = 1
                for (j = 1; j <= m; j++) if (NE[i, j] > W[j]) cabe = 0
                if (cabe) {
                    for (j = 1; j <= m; j++) W[j] += AL[i, j]
                    F[i] = 1; terminados++; achou = 1
                }
            }
        } while (achou)
        return terminados == n
    }
    # Le os m valores do pedido, a partir do campo "inicio", para o vetor P
    function le_pedido(inicio,    j) {
        for (j = 1; j <= m; j++) P[j] = $(inicio + j - 1) + 0
    }
    # Devolvem 1 se P cabe no need / na allocation do cliente c, ou no available
    function cabe_em_need(c,    j) { for (j = 1; j <= m; j++) if (P[j] > NE[c, j]) return 0; return 1 }
    function cabe_em_alloc(c,   j) { for (j = 1; j <= m; j++) if (P[j] > AL[c, j]) return 0; return 1 }
    function cabe_em_avail(     j) { for (j = 1; j <= m; j++) if (P[j] > AV[j]) return 0; return 1 }
    # 1 se conceder P ao cliente c deixaria o estado seguro (desfaz depois)
    function seguro_se_conceder(c,    j, resposta) {
        for (j = 1; j <= m; j++) { AV[j] -= P[j]; AL[c, j] += P[j]; NE[c, j] -= P[j] }
        resposta = seguro()
        for (j = 1; j <= m; j++) { AV[j] += P[j]; AL[c, j] -= P[j]; NE[c, j] += P[j] }
        return resposta
    }
    BEGIN { split(total, T, " "); erros = 0; mensagens = 0; tabelas = 0 }

    /^MAXIMUM/ { linha_cliente = 0; next }
    /\|/ {
        linha_cliente++
        split($0, partes, "|")
        split(partes[1], mx, " "); split(partes[2], al, " "); split(partes[3], ne, " ")
        for (j = 1; j <= m; j++) {
            AL[linha_cliente, j] = al[j] + 0
            NE[linha_cliente, j] = ne[j] + 0
            if (ne[j] + 0 != mx[j] - al[j]) erro("need diferente de maximum - allocation")
            if (al[j] + 0 < 0 || ne[j] + 0 < 0) erro("valor negativo na tabela")
        }
        next
    }
    /^AVAILABLE/ {
        tabelas++
        if (linha_cliente != n) erro("tabela com numero errado de clientes")
        for (j = 1; j <= m; j++) {
            AV[j] = $(j + 1) + 0
            soma = AV[j]
            for (i = 1; i <= n; i++) soma += AL[i, j]
            if (AV[j] < 0) erro("available negativo")
            if (soma != T[j]) erro("available + allocation diferente do total")
        }
        if (!seguro()) erro("estado INSEGURO")
        next
    }
    # A partir daqui, AV/AL/NE guardam o estado de ANTES do comando (a ultima tabela)
    /^Allocate to customer/ {
        mensagens++; c = $4 + 1; le_pedido(7)
        if (!cabe_em_need(c) || !cabe_em_avail() || !seguro_se_conceder(c))
            erro("pedido concedido, mas deveria ter sido negado")
        next
    }
    /^Release from customer/ {
        mensagens++; c = $4 + 1; le_pedido(7)
        if (!cabe_em_alloc(c)) erro("devolucao aceita, mas o cliente nao tinha isso")
        next
    }
    /exceed its maximum allocation$/ {
        mensagens++; c = $3 + 1; le_pedido(5)
        if (cabe_em_need(c)) erro("negado por exceder o maximo, mas nao excedia")
        next
    }
    /^The resources/ {
        mensagens++; c = $(m + 8) + 1; le_pedido(m + 10)
        for (j = 1; j <= m; j++) if ($(j + 2) + 0 != AV[j]) erro("available errado na mensagem")
        if (!cabe_em_need(c) || cabe_em_avail()) erro("negado por falta de recursos, mas havia recursos")
        next
    }
    /result in an unsafe state$/ {
        mensagens++; c = $3 + 1; le_pedido(5)
        if (!cabe_em_need(c) || !cabe_em_avail() || seguro_se_conceder(c))
            erro("negado como inseguro, mas era seguro conceder")
        next
    }
    /exceed its allocation$/ {
        mensagens++; c = $3 + 1; le_pedido(5)
        if (cabe_em_alloc(c)) erro("devolucao recusada, mas o cliente tinha isso")
        next
    }
    { erro("linha inesperada") }
    END {
        if (mensagens != comandos) erro("esperava " comandos " mensagens, achou " mensagens)
        if (tabelas != comandos + 1) erro("esperava " comandos + 1 " tabelas, achou " tabelas)
        exit (erros > 0)
    }' result.txt

    if [ $? -ne 0 ]; then
        echo ">> FALHOU na rodada $r (./banker$total). Arquivos usados:"
        echo "customer.txt:"; cat customer.txt
        echo "commands.txt:"; cat commands.txt
        falhas_forca_bruta=$((falhas_forca_bruta + 1))
    fi
done

if [ "$falhas_forca_bruta" -eq 0 ]; then
    echo ">> OK: $RODADAS rodadas aleatorias, todas consistentes e seguras"
    passou=$((passou + 1))
else
    echo ">> FALHOU: $falhas_forca_bruta de $RODADAS rodadas com problema"
    falhou=$((falhou + 1))
fi

cd ..
rm -rf testes_tmp

echo "=============================================================="
echo " RESUMO: $passou passaram, $falhou falharam"
echo "=============================================================="

if [ "$falhou" -ne 0 ]; then
    exit 1
fi
exit 0
