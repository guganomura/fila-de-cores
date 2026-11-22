#!/bin/sh
# Executa o jogo com entradas prontas e confere a saida de cada cenario.
# A semente 1903 sorteia: VERDE, VERMELHO, AZUL, ROXO, VERDE...
# Uso: sh tests/test_sistema.sh [executavel]

programa=${1:-build/fila-de-cores}
total=0
falhas=0

resultado() {
    total=$((total + 1))
    if [ "$1" -eq 0 ]; then
        echo "[OK] $2 $3 -> $4"
    else
        falhas=$((falhas + 1))
        echo "[FALHA] $2 $3 -> $4"
    fi
}

# testar ID CENARIO ESPERADO ENTRADA TRECHO_DA_SAIDA [TRECHO_PROIBIDO]
testar() {
    passou=1
    if saida=$(printf "$4" | "$programa" --semente 1903 2>&1) &&
        printf '%s\n' "$saida" | grep -Fq -- "$5" &&
        { [ -z "$6" ] || ! printf '%s\n' "$saida" | grep -Fq -- "$6"; }; then
        passou=0
    fi
    resultado "$passou" "$1" "$2" "$3"
}

R1='1\n\n1\n'           # nova partida e rodada 1 certa
R2="${R1}\n1\n2\n"      # ate a rodada 2 certa
R3="${R2}\n1\n2\n3\n"   # ate a rodada 3 certa

testar SIS-01 "iniciar nova partida" "rodada 1" '1\ns\n' "Rodada 1"
testar SIS-02 "visualizar a sequencia" "VERDE" '1\ns\n' "Sequencia: VERDE"
testar SIS-03 "pontuacao inicial" "0" '1\ns\n' "Pontuacao: 0"
testar SIS-04 "pedir para memorizar" "espera Enter" '1\ns\n' "Memorize e pressione Enter"
testar SIS-05 "informar a sequencia" "pede cor por cor" '1\n\ns\n' "Cor 1 de 1"
testar SIS-06 "acertar a rodada 1" "mensagem e 1 ponto" "${R1}s\n" "Sequencia correta! Pontuacao: 1"
testar SIS-07 "avancar de rodada" "rodada 2" "${R1}s\n" "Rodada 2"
testar SIS-08 "nova cor na rodada 2" "entra no fim" "${R1}s\n" "Sequencia: VERDE -> VERMELHO"
testar SIS-09 "exemplo do enunciado" "tres cores" "${R2}s\n" "Sequencia: VERDE -> VERMELHO -> AZUL"
testar SIS-10 "acertar a rodada 3" "3 pontos" "${R3}s\n" "Sequencia correta! Pontuacao: 3"
testar SIS-11 "quarta cor" "entra no fim" "${R3}s\n" "Sequencia: VERDE -> VERMELHO -> AZUL -> ROXO"
testar SIS-12 "responder pelo nome" "aceita" '1\n\nverde\ns\n' "Sequencia correta! Pontuacao: 1"
testar SIS-13 "nome em maiusculas com espacos" "aceita" '1\n\n  VERDE  \ns\n' "Sequencia correta! Pontuacao: 1"
testar SIS-14 "cor errada" "mensagem de erro" '1\n\n2\n0\n' "Sequencia incorreta"
testar SIS-15 "explicar o erro" "cor esperada e informada" '1\n\n2\n0\n' "Esperado: VERDE. Informado: VERMELHO."
testar SIS-16 "avisar o fim da partida" "partida encerrada" '1\n\n2\n0\n' "PARTIDA ENCERRADA"
testar SIS-17 "pontuacao final apos erro" "2" "${R2}\n1\n2\n4\n0\n" "Pontuacao final: 2"
testar SIS-18 "cores certas fora de ordem" "erro na primeira" "${R1}\n2\n0\n" "Esperado: VERDE. Informado: VERMELHO."
testar SIS-19 "sequencia no resumo final" "completa" "${R2}\n1\n2\n4\n0\n" "A sequencia era: VERDE -> VERMELHO -> AZUL"
testar SIS-20 "jogar de novo apos errar" "nova sequencia" '1\n\n2\n1\ns\n' "Sequencia: VERMELHO"
testar SIS-21 "pontuacao da nova partida" "comeca em 0" "${R1}\n5\n1\ns\n" "Pontuacao final: 0"
testar SIS-22 "reiniciar ao memorizar" "partida reiniciada" '1\nr\ns\n' "Partida reiniciada"
testar SIS-23 "reiniciar ao responder" "partida reiniciada" '1\n\nR\ns\n' "Partida reiniciada"
testar SIS-24 "pontuacao apos reiniciar" "zera" "${R1}r\ns\n" "Pontuacao final: 0"
testar SIS-25 "sequencia apos reiniciar" "recomeca com uma cor" "${R1}r\ns\n" "Sequencia: AZUL"
testar SIS-26 "encerrar partida" "pontuacao final" "${R1}m\n0\n" "Pontuacao final: 1"
testar SIS-27 "encerrar partida ao responder" "volta ao menu" '1\n\nM\n0\n' "PARTIDA ENCERRADA" "Fim da entrada"
testar SIS-28 "nova partida apos encerrar" "nova sequencia" '1\nm\n1\ns\n' "Sequencia: VERMELHO"
testar SIS-29 "sair ao memorizar" "programa termina" '1\ns\n' "Jogo encerrado" "Fim da entrada"
testar SIS-30 "sair ao responder" "programa termina" '1\n\nS\n' "Jogo encerrado" "Fim da entrada"
testar SIS-31 "sair pelo menu" "programa termina" '0\n' "Jogo encerrado" "Fim da entrada"
testar SIS-32 "sair no meio da partida" "pontuacao final" "${R2}s\n" "Pontuacao final: 2"
testar SIS-33 "cor inexistente" "pede de novo" '1\n\n9\n1\ns\n' "Cor invalida" "Sequencia incorreta"
testar SIS-34 "entrada invalida nao e erro" "ainda pontua" '1\n\nabc\n1\ns\n' "Sequencia correta! Pontuacao: 1"
testar SIS-35 "resposta vazia" "pede de novo" '1\n\n\n1\ns\n' "Cor invalida"
testar SIS-36 "responder antes de esconder" "pede Enter" '1\n1\ns\n' "Pressione apenas Enter"
testar SIS-37 "opcao invalida no menu" "pede de novo" '7\n0\n' "Opcao invalida"
testar SIS-38 "fim inesperado da entrada" "encerra sem travar" '1\n\n' "Fim da entrada"

# A sequencia nao pode continuar visivel enquanto o jogador responde.
saida=$(printf "${R1}\ns\n" | "$programa" --semente 1903)
depois=$(printf '%s\n' "$saida" | awk '/Repita a sequencia/ { t = "" } { t = t $0 "\n" } END { printf "%s", t }' |
    sed '/PARTIDA ENCERRADA/,$d')
printf '%s\n' "$depois" | grep -Fq "Cor 1 de 2" &&
    ! printf '%s\n' "$depois" | grep -Fq "VERDE -> VERMELHO"
resultado $? SIS-39 "sequencia durante a resposta" "escondida"

limpar=$(printf '\033[2J')
printf '1\n\ns\n' | "$programa" --semente 1903 | grep -Fq "$limpar"
resultado $? SIS-40 "limpar a tela apos memorizar" "comando de limpeza enviado"

! "$programa" --semente abc </dev/null >/dev/null 2>&1
resultado $? SIS-41 "semente invalida" "recusa"

echo
echo "Sistema: $total testes, $falhas falhas"
[ "$falhas" -eq 0 ]
