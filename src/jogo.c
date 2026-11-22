#include "jogo.h"

void inicializar_jogo(Jogo *jogo) {
    inicializar_fila(&jogo->sequencia);
    inicializar_fila(&jogo->restantes);
    jogo->pontuacao = 0;
    jogo->ativa = 0;
}

void liberar_jogo(Jogo *jogo) {
    liberar_fila(&jogo->sequencia);
    liberar_fila(&jogo->restantes);
    jogo->pontuacao = 0;
    jogo->ativa = 0;
}

void encerrar_partida(Jogo *jogo) {
    liberar_fila(&jogo->restantes);
    jogo->ativa = 0;
}

/* Poe a nova cor no fim da sequencia e copia a sequencia para as respostas. */
static Status preparar_rodada(Jogo *jogo, Cor nova) {
    const No *atual;

    if (!enfileirar(&jogo->sequencia, nova)) {
        encerrar_partida(jogo);
        return ERRO_MEMORIA;
    }
    for (atual = jogo->sequencia.inicio; atual != NULL; atual = atual->proximo) {
        if (!enfileirar(&jogo->restantes, atual->cor)) {
            encerrar_partida(jogo);
            return ERRO_MEMORIA;
        }
    }
    return OK;
}

Status iniciar_partida(Jogo *jogo, Cor primeira) {
    liberar_jogo(jogo);
    jogo->ativa = 1;
    return preparar_rodada(jogo, primeira);
}

Status proxima_rodada(Jogo *jogo, Cor nova) {
    Cor pendente;

    if (!jogo->ativa || frente(&jogo->restantes, &pendente)) return ERRO_FORA_DE_HORA;
    return preparar_rodada(jogo, nova);
}

Status responder(Jogo *jogo, Cor resposta) {
    Cor esperada;

    /* A cor esperada e sempre a que entrou primeiro (FIFO). */
    if (!jogo->ativa || !frente(&jogo->restantes, &esperada)) return ERRO_FORA_DE_HORA;

    if (resposta != esperada) {
        encerrar_partida(jogo);
        return COR_ERRADA;
    }
    desenfileirar(&jogo->restantes, NULL);
    if (jogo->restantes.quantidade > 0) return COR_CERTA;

    jogo->pontuacao++;
    return RODADA_COMPLETA;
}
