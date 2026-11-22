#ifndef JOGO_H
#define JOGO_H

#include "fila.h"

typedef struct {
    Fila sequencia; /* sequencia completa, cresce uma cor por rodada */
    Fila restantes; /* copia consumida enquanto o jogador responde */
    int pontuacao;  /* rodadas completadas */
    int ativa;
} Jogo;

typedef enum {
    OK,
    COR_CERTA,
    RODADA_COMPLETA,
    COR_ERRADA,
    ERRO_FORA_DE_HORA,
    ERRO_MEMORIA
} Status;

/* Deve ser chamada uma vez, antes das demais. */
void inicializar_jogo(Jogo *jogo);

/* Serve tanto para iniciar quanto para reiniciar. */
Status iniciar_partida(Jogo *jogo, Cor primeira);

/* So e aceita depois que a rodada atual foi completada. */
Status proxima_rodada(Jogo *jogo, Cor nova);

/* Compara com a frente de restantes. Um erro encerra a partida. */
Status responder(Jogo *jogo, Cor resposta);

/* Mantem a sequencia e a pontuacao, para o resumo final. */
void encerrar_partida(Jogo *jogo);

void liberar_jogo(Jogo *jogo);

#endif
