#ifndef COR_H
#define COR_H

typedef enum { VERDE, VERMELHO, AZUL, ROXO, LARANJA, NUM_CORES } Cor;

const char *nome_cor(Cor cor);

/* Aceita o numero (1 a 5) ou o nome, em qualquer caixa. Retorna 1 ou 0. */
int ler_cor(const char *texto, Cor *cor);

#endif
