#include "fila.h"

#include <stdlib.h>

void inicializar_fila(Fila *fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

int enfileirar(Fila *fila, Cor cor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;

    novo->cor = cor;
    novo->proximo = NULL; /* 1. sera o ultimo */

    if (fila->fim == NULL) {
        fila->inicio = novo; /* 2a. fila vazia: tambem e o primeiro */
    } else {
        fila->fim->proximo = novo; /* 2b. o antigo ultimo aponta para ele */
    }
    fila->fim = novo; /* 3. o novo passa a ser o fim */
    fila->quantidade++;
    return 1;
}

int desenfileirar(Fila *fila, Cor *cor) {
    No *retirado = fila->inicio; /* 1. guarda o no do inicio */
    if (retirado == NULL) return 0;

    if (cor != NULL) *cor = retirado->cor;
    fila->inicio = retirado->proximo;             /* 2. o inicio avanca */
    if (fila->inicio == NULL) fila->fim = NULL;   /* 3. esvaziou: fim tambem zera */
    free(retirado);                               /* 4. libera o no que saiu */
    fila->quantidade--;
    return 1;
}

int frente(const Fila *fila, Cor *cor) {
    if (fila->inicio == NULL) return 0;
    *cor = fila->inicio->cor;
    return 1;
}

void imprimir(const Fila *fila, FILE *saida) {
    const No *atual;

    if (fila->inicio == NULL) fprintf(saida, "(vazia)");
    for (atual = fila->inicio; atual != NULL; atual = atual->proximo) {
        fprintf(saida, "%s", nome_cor(atual->cor));
        if (atual->proximo != NULL) fprintf(saida, " -> ");
    }
}

void liberar_fila(Fila *fila) {
    while (desenfileirar(fila, NULL)) {
        /* cada chamada libera um no */
    }
}
