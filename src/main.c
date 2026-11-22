/* Interface de terminal. As mensagens nao usam acentos para aparecerem
 * corretamente tambem no terminal do Windows. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cor.h"
#include "jogo.h"

#define TAM_LINHA 64

typedef enum { CONTINUAR, REINICIAR, VOLTAR_AO_MENU } Acao;

static Jogo jogo;
static uint32_t estado_sorteio;

static void ao_sair(void) {
    liberar_jogo(&jogo);
}

/* ---------- Sorteio ---------- */

/* Xorshift: ao contrario de rand(), a mesma semente da as mesmas cores em
 * qualquer sistema, o que permite testar o jogo. */
static Cor sortear_cor(void) {
    estado_sorteio ^= estado_sorteio << 13;
    estado_sorteio ^= estado_sorteio >> 17;
    estado_sorteio ^= estado_sorteio << 5;
    return (Cor)((estado_sorteio >> 16) % NUM_CORES);
}

static void semear(uint32_t semente) {
    estado_sorteio = semente * UINT32_C(2654435761);
    if (estado_sorteio == 0) estado_sorteio = 1; /* zero travaria o xorshift */
}

/* ---------- Tela ---------- */

static void limpar_tela(void) {
    fflush(stdout);
#ifdef _WIN32
    system("cls");
#else
    /* Vai ao topo, limpa a tela e apaga o historico de rolagem. */
    printf("\033[H\033[2J\033[3J");
#endif
}

static void mostrar_fim(void) {
    printf("\n*** PARTIDA ENCERRADA ***\n");
    printf("A sequencia era: ");
    imprimir(&jogo.sequencia, stdout);
    printf("\nPontuacao final: %d\n", jogo.pontuacao);
}

static void sair(void) {
    if (jogo.ativa) mostrar_fim();
    printf("\nJogo encerrado.\n");
    exit(0); /* ao_sair e chamada pelo atexit */
}

/* ---------- Entrada ---------- */

/* Le uma linha, sem espacos nas pontas. O fim da entrada encerra o jogo. */
static void ler_linha(const char *pergunta, char linha[TAM_LINHA]) {
    size_t inicio, fim;
    int c;

    for (;;) {
        printf("%s", pergunta);
        fflush(stdout);
        if (fgets(linha, TAM_LINHA, stdin) == NULL) {
            printf("\nFim da entrada.\n");
            sair();
        }
        if (strchr(linha, '\n') != NULL || feof(stdin)) break;

        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o restante da linha */
        }
        printf("Entrada muito longa.\n");
    }
    inicio = strspn(linha, " \t\r\n");
    fim = strlen(linha);
    while (fim > inicio && strchr(" \t\r\n", linha[fim - 1]) != NULL) fim--;
    memmove(linha, linha + inicio, fim - inicio);
    linha[fim - inicio] = '\0';
}

static Acao ler_comando(const char *linha) {
    if (strcmp(linha, "r") == 0 || strcmp(linha, "R") == 0) return REINICIAR;
    if (strcmp(linha, "m") == 0 || strcmp(linha, "M") == 0) return VOLTAR_AO_MENU;
    if (strcmp(linha, "s") == 0 || strcmp(linha, "S") == 0) sair();
    return CONTINUAR;
}

/* ---------- Partida ---------- */

static Acao apresentar_sequencia(void) {
    char linha[TAM_LINHA];
    Acao acao;

    printf("\n--- Rodada %d | Pontuacao: %d ---\n", jogo.sequencia.quantidade, jogo.pontuacao);
    printf("Sequencia: ");
    imprimir(&jogo.sequencia, stdout);
    printf("\n(R reinicia | M encerra a partida | S sai do jogo)\n");

    for (;;) {
        ler_linha("Memorize e pressione Enter: ", linha);
        if (linha[0] == '\0') break;
        if ((acao = ler_comando(linha)) != CONTINUAR) return acao;
        printf("Pressione apenas Enter. A resposta vem depois.\n");
    }
    limpar_tela();
    return CONTINUAR;
}

static Acao receber_resposta(void) {
    char linha[TAM_LINHA], pergunta[64];
    Cor resposta, esperada;
    Status status = COR_CERTA;
    Acao acao;
    int i, tamanho = jogo.sequencia.quantidade;

    printf("Repita a sequencia | Rodada %d | Pontuacao: %d\n", tamanho, jogo.pontuacao);
    for (i = 0; i < NUM_CORES; i++) printf("%d-%s  ", i + 1, nome_cor((Cor)i));
    printf("\n(R reinicia | M encerra a partida | S sai do jogo)\n");

    while (status == COR_CERTA) {
        snprintf(pergunta, sizeof pergunta, "Cor %d de %d: ",
                 tamanho - jogo.restantes.quantidade + 1, tamanho);
        ler_linha(pergunta, linha);
        if ((acao = ler_comando(linha)) != CONTINUAR) return acao;
        if (!ler_cor(linha, &resposta)) {
            printf("Cor invalida. Digite o numero (1 a %d) ou o nome.\n", NUM_CORES);
            continue;
        }

        /* Guarda a esperada antes, porque um erro esvazia a fila de respostas. */
        if (!frente(&jogo.restantes, &esperada)) break;
        status = responder(&jogo, resposta);
        if (status == COR_ERRADA) {
            printf("\nSequencia incorreta! Esperado: %s. Informado: %s.\n",
                   nome_cor(esperada), nome_cor(resposta));
            return VOLTAR_AO_MENU;
        }
    }
    printf("\nSequencia correta! Pontuacao: %d\n", jogo.pontuacao);
    return CONTINUAR;
}

/* Cada volta do laco e uma rodada. */
static void jogar_partida(void) {
    Status status = iniciar_partida(&jogo, sortear_cor());
    Acao acao;

    while (status == OK) {
        acao = apresentar_sequencia();
        if (acao == CONTINUAR) acao = receber_resposta();

        if (acao == VOLTAR_AO_MENU) {
            encerrar_partida(&jogo);
            mostrar_fim();
            return;
        }
        if (acao == REINICIAR) {
            status = iniciar_partida(&jogo, sortear_cor());
            limpar_tela();
            printf("Partida reiniciada.\n");
        } else {
            status = proxima_rodada(&jogo, sortear_cor());
        }
    }
    printf("Erro: memoria insuficiente.\n");
    mostrar_fim();
}

int main(int argc, char *argv[]) {
    char linha[TAM_LINHA];

    if (argc == 1) {
        semear((uint32_t)time(NULL) ^ (uint32_t)clock());
    } else if (argc == 3 && strcmp(argv[1], "--semente") == 0 && argv[2][0] != '\0' &&
               strspn(argv[2], "0123456789") == strlen(argv[2])) {
        semear((uint32_t)strtoul(argv[2], NULL, 10));
    } else {
        fprintf(stderr, "Uso: %s [--semente NUMERO]\n", argv[0]);
        return 1;
    }

    inicializar_jogo(&jogo);
    atexit(ao_sair);

    for (;;) {
        printf("\n=== FILA DE CORES ===\n"
               "Memorize a sequencia e repita na mesma ordem.\n"
               "Cada rodada acrescenta uma cor e cada rodada certa vale um ponto.\n"
               " 1. Nova partida\n"
               " 0. Encerrar o jogo\n");
        ler_linha("Opcao: ", linha);

        if (strcmp(linha, "1") == 0) jogar_partida();
        else if (strcmp(linha, "0") == 0) sair();
        else printf("Opcao invalida.\n");
    }
}
