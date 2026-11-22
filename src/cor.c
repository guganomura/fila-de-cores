#include "cor.h"

#include <ctype.h>
#include <stddef.h>

static const char *NOMES[NUM_CORES] = {"VERDE", "VERMELHO", "AZUL", "ROXO", "LARANJA"};

const char *nome_cor(Cor cor) {
    return NOMES[cor];
}

int ler_cor(const char *texto, Cor *cor) {
    int i;
    size_t j;

    for (i = 0; i < NUM_CORES; i++) {
        if (texto[0] == '1' + i && texto[1] == '\0') {
            *cor = (Cor)i;
            return 1;
        }
        j = 0;
        while (NOMES[i][j] != '\0' && toupper((unsigned char)texto[j]) == NOMES[i][j]) j++;
        if (NOMES[i][j] == '\0' && texto[j] == '\0') {
            *cor = (Cor)i;
            return 1;
        }
    }
    return 0;
}
