#ifndef FILA_H
#define FILA_H

#include <stdio.h>

#include "cor.h"

typedef struct No {
    Cor cor;
    struct No *proximo; /* quem entrou depois; NULL no ultimo */
} No;

typedef struct {
    No *inicio; /* de onde sai */
    No *fim;    /* por onde entra */
    int quantidade;
} Fila;

/* Deixa a fila vazia. Se ela ja tiver nos, chame liberar_fila antes. */
void inicializar_fila(Fila *fila);

/* Coloca uma cor no fim. Retorna 1, ou 0 se faltar memoria. */
int enfileirar(Fila *fila, Cor cor);

/* Retira a cor do inicio. Retorna 1, ou 0 se a fila estiver vazia.
 * Se cor nao for NULL, recebe o valor retirado. */
int desenfileirar(Fila *fila, Cor *cor);

/* Consulta o inicio sem retirar. Retorna 1, ou 0 se a fila estiver vazia. */
int frente(const Fila *fila, Cor *cor);

/* Escreve do inicio para o fim: VERDE -> VERMELHO -> AZUL */
void imprimir(const Fila *fila, FILE *saida);

/* Auxiliar exigida pelo C: devolve a memoria de todos os nos. */
void liberar_fila(Fila *fila);

#endif
