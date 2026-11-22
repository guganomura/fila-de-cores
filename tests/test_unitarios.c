#include <string.h>

#include "../src/cor.h"
#include "../src/fila.h"
#include "../src/jogo.h"

static int total, falhas;

static void verificar(const char *id, const char *cenario, const char *esperado, int condicao) {
    total++;
    if (!condicao) falhas++;
    printf("[%s] %s %s -> %s\n", condicao ? "OK" : "FALHA", id, cenario, esperado);
}

/* Confere do inicio ao fim, com cores como "123", e tambem o ponteiro fim. */
static int conteudo(const Fila *f, const char *cores) {
    const No *no = f->inicio, *ultimo = NULL;
    int n = 0;
    for (; cores[n] != '\0'; n++) {
        if (no == NULL || (int)no->cor != cores[n] - '1') return 0;
        ultimo = no;
        no = no->proximo;
    }
    return no == NULL && f->fim == ultimo && f->quantidade == n;
}

static int impresso(const Fila *f, const char *esperado) {
    char texto[128];
    FILE *arq = tmpfile();
    size_t n;
    if (arq == NULL) return 0;
    imprimir(f, arq);
    rewind(arq);
    n = fread(texto, 1, sizeof texto - 1, arq);
    texto[n] = '\0';
    fclose(arq);
    return strcmp(texto, esperado) == 0;
}

static void testar_inicializar_fila(void) {
    Fila f;
    f.inicio = f.fim = (No *)&f;
    f.quantidade = 42;
    inicializar_fila(&f);
    verificar("INI-01", "inicializar sobre lixo de memoria", "inicio = fim = NULL, quantidade = 0",
              conteudo(&f, ""));
    enfileirar(&f, VERDE);
    enfileirar(&f, AZUL);
    liberar_fila(&f);
    verificar("INI-02", "liberar fila com dois nos", "fila vazia", conteudo(&f, ""));
    liberar_fila(&f);
    verificar("INI-03", "liberar duas vezes", "fila vazia, sem falha", conteudo(&f, ""));
}

static void testar_enfileirar(void) {
    Fila f;
    const No *primeiro, *antigo;
    int i, certo = 1;
    inicializar_fila(&f);
    verificar("ENF-01", "enfileirar em fila vazia", "inicio e fim no mesmo no",
              enfileirar(&f, VERDE) && conteudo(&f, "1") && f.inicio == f.fim);
    primeiro = f.inicio;
    verificar("ENF-02", "enfileirar o segundo", "entra no fim",
              enfileirar(&f, VERMELHO) && conteudo(&f, "12"));
    verificar("ENF-03", "inicio apos enfileirar", "nao muda", f.inicio == primeiro);
    antigo = f.fim;
    enfileirar(&f, AZUL);
    verificar("ENF-04", "referencia do antigo fim", "aponta para o novo no",
              antigo && antigo->proximo == f.fim && conteudo(&f, "123"));
    verificar("ENF-05", "cor repetida", "aceita", enfileirar(&f, AZUL) && conteudo(&f, "1233"));
    liberar_fila(&f);
    for (i = 0; i < 100000; i++)
        if (!enfileirar(&f, (Cor)(i % NUM_CORES))) certo = 0;
    verificar("ENF-06", "100000 nos sem capacidade previa", "quantidade = 100000",
              certo && f.quantidade == 100000);
    liberar_fila(&f);
}

static void testar_desenfileirar(void) {
    Fila f;
    const No *segundo;
    Cor c = ROXO;
    inicializar_fila(&f);
    verificar("DES-01", "desenfileirar fila vazia", "falha, destino inalterado",
              !desenfileirar(&f, &c) && c == ROXO && conteudo(&f, ""));
    enfileirar(&f, VERDE);
    enfileirar(&f, VERMELHO);
    enfileirar(&f, AZUL);
    segundo = f.inicio ? f.inicio->proximo : NULL;
    verificar("DES-02", "desenfileirar de tres", "sai o primeiro que entrou",
              desenfileirar(&f, &c) && c == VERDE && conteudo(&f, "23"));
    verificar("DES-03", "referencia do inicio", "passa ao segundo no", f.inicio == segundo);
    verificar("DES-04", "ordem FIFO", "depois VERMELHO e AZUL",
              desenfileirar(&f, &c) && c == VERMELHO && desenfileirar(&f, &c) && c == AZUL);
    verificar("DES-05", "retirar o ultimo", "inicio e fim voltam a NULL", conteudo(&f, ""));
    verificar("DES-06", "enfileirar apos esvaziar", "inicio e fim no novo no",
              enfileirar(&f, LARANJA) && conteudo(&f, "5"));
    verificar("DES-07", "destino NULL", "retira normalmente",
              desenfileirar(&f, NULL) && conteudo(&f, ""));
    liberar_fila(&f);
}

static void testar_frente(void) {
    Fila f;
    Cor c = ROXO;
    inicializar_fila(&f);
    verificar("FRE-01", "frente de fila vazia", "falha, destino inalterado",
              !frente(&f, &c) && c == ROXO);
    enfileirar(&f, AZUL);
    enfileirar(&f, VERDE);
    verificar("FRE-02", "frente de AZUL, VERDE", "AZUL", frente(&f, &c) && c == AZUL);
    verificar("FRE-03", "consultar nao retira", "fila inalterada", conteudo(&f, "31"));
    desenfileirar(&f, NULL);
    verificar("FRE-04", "frente apos desenfileirar", "VERDE", frente(&f, &c) && c == VERDE);
    liberar_fila(&f);
}

static void testar_imprimir(void) {
    Fila f;
    inicializar_fila(&f);
    verificar("IMP-01", "imprimir fila vazia", "(vazia)", impresso(&f, "(vazia)"));
    enfileirar(&f, VERDE);
    verificar("IMP-02", "imprimir uma cor", "VERDE", impresso(&f, "VERDE"));
    enfileirar(&f, VERMELHO);
    enfileirar(&f, AZUL);
    verificar("IMP-03", "exemplo do enunciado", "VERDE -> VERMELHO -> AZUL",
              impresso(&f, "VERDE -> VERMELHO -> AZUL"));
    verificar("IMP-04", "imprimir nao retira", "fila inalterada", conteudo(&f, "123"));
    liberar_fila(&f);
}

static void testar_cores(void) {
    Cor c;
    verificar("COR-01", "ler numeros 1 e 5", "VERDE e LARANJA",
              ler_cor("1", &c) && c == VERDE && ler_cor("5", &c) && c == LARANJA);
    verificar("COR-02", "ler nome em qualquer caixa", "reconhece",
              ler_cor("Vermelho", &c) && c == VERMELHO && ler_cor("ROXO", &c) && c == ROXO);
    verificar("COR-03", "numeros fora de 1 a 5", "rejeita",
              !ler_cor("0", &c) && !ler_cor("6", &c) && !ler_cor("12", &c));
    verificar("COR-04", "nome incompleto, errado ou vazio", "rejeita",
              !ler_cor("ver", &c) && !ler_cor("verdes", &c) && !ler_cor("", &c));
}

static void testar_partida(void) {
    Jogo j;
    inicializar_jogo(&j);
    verificar("PAR-01", "jogo recem-criado", "inativo, vazio, 0 pontos",
              !j.ativa && !j.pontuacao && conteudo(&j.sequencia, "") && conteudo(&j.restantes, ""));
    verificar("PAR-02", "responder sem partida", "rejeita", responder(&j, VERDE) == ERRO_FORA_DE_HORA);
    verificar("PAR-03", "avancar sem partida", "rejeita",
              proxima_rodada(&j, VERDE) == ERRO_FORA_DE_HORA);
    verificar("PAR-04", "nova partida", "uma cor, 0 pontos",
              iniciar_partida(&j, VERDE) == OK && j.ativa && !j.pontuacao &&
              conteudo(&j.sequencia, "1") && conteudo(&j.restantes, "1"));
    verificar("PAR-05", "avancar antes de responder", "rejeita, nada muda",
              proxima_rodada(&j, AZUL) == ERRO_FORA_DE_HORA && conteudo(&j.sequencia, "1"));
    verificar("ROD-01", "acertar a rodada 1", "1 ponto",
              responder(&j, VERDE) == RODADA_COMPLETA && j.pontuacao == 1);
    verificar("ROD-02", "responder rodada ja completa", "rejeita, pontos iguais",
              responder(&j, VERDE) == ERRO_FORA_DE_HORA && j.pontuacao == 1);
    verificar("ROD-03", "avancar de rodada", "nova cor no fim",
              proxima_rodada(&j, VERMELHO) == OK && conteudo(&j.sequencia, "12"));
    verificar("ROD-04", "fila de respostas da rodada", "copia da sequencia inteira",
              conteudo(&j.restantes, "12") && j.restantes.inicio != j.sequencia.inicio);
    verificar("ROD-05", "acerto parcial", "consome so a copia, sem ponto",
              responder(&j, VERDE) == COR_CERTA && j.pontuacao == 1 &&
              conteudo(&j.restantes, "2") && conteudo(&j.sequencia, "12"));
    verificar("ROD-06", "completar a rodada 2", "2 pontos",
              responder(&j, VERMELHO) == RODADA_COMPLETA && j.pontuacao == 2);
    proxima_rodada(&j, AZUL);
    verificar("ERR-01", "cor certa fora de ordem", "erro, partida encerrada",
              responder(&j, AZUL) == COR_ERRADA && !j.ativa);
    verificar("ERR-02", "dados apos o erro", "pontos e sequencia mantidos",
              j.pontuacao == 2 && conteudo(&j.sequencia, "123"));
    verificar("ERR-03", "responder apos o erro", "rejeita",
              responder(&j, VERDE) == ERRO_FORA_DE_HORA && j.pontuacao == 2);
    verificar("ERR-04", "avancar apos o erro", "rejeita",
              proxima_rodada(&j, VERDE) == ERRO_FORA_DE_HORA && conteudo(&j.sequencia, "123"));
    verificar("REI-01", "nova partida apos errar", "uma cor, 0 pontos",
              iniciar_partida(&j, ROXO) == OK && j.ativa && !j.pontuacao &&
              conteudo(&j.sequencia, "4") && conteudo(&j.restantes, "4"));
    responder(&j, ROXO);
    proxima_rodada(&j, AZUL);
    responder(&j, ROXO);
    verificar("REI-02", "reiniciar no meio da resposta", "descarta a anterior",
              iniciar_partida(&j, LARANJA) == OK && !j.pontuacao &&
              conteudo(&j.sequencia, "5") && conteudo(&j.restantes, "5"));
    encerrar_partida(&j);
    verificar("FIM-01", "encerrar a partida", "inativa, sequencia mantida",
              !j.ativa && conteudo(&j.sequencia, "5") && responder(&j, LARANJA) == ERRO_FORA_DE_HORA);
    liberar_jogo(&j);
    verificar("FIM-02", "liberar o jogo", "as duas filas vazias",
              conteudo(&j.sequencia, "") && conteudo(&j.restantes, ""));
}

/*
 * Ha 5 + 25 + 125 + 625 + 3125 = 3905 sequencias de ate 5 cores. Cada uma e
 * jogada acertando tudo e tambem com cada cor errada em cada posicao.
 */
static void testar_todas_as_sequencias(void) {
    Jogo j;
    int tam, n, i, r, erro, cor, partidas = 0, erros = 0, acertos_certos = 1, erros_certos = 1;
    int combinacoes = 1;
    Cor s[5];
    inicializar_jogo(&j);
    for (tam = 1; tam <= 5; tam++) {
        combinacoes *= NUM_CORES;
        for (n = 0; n < combinacoes; n++) {
            int resto = n;
            for (i = 0; i < tam; i++, resto /= NUM_CORES) s[i] = (Cor)(resto % NUM_CORES);

            /* erro == tam significa acertar todas. */
            for (erro = 0; erro <= tam; erro++) {
                for (cor = 0; cor < (erro == tam ? 1 : NUM_CORES); cor++) {
                    if (erro < tam && cor == (int)s[erro]) continue;
                    if (iniciar_partida(&j, s[0]) != OK) acertos_certos = 0;
                    for (r = 1; r < tam; r++) {
                        for (i = 0; i < r; i++) responder(&j, s[i]);
                        if (proxima_rodada(&j, s[r]) != OK) acertos_certos = 0;
                    }
                    for (i = 0; i < erro; i++)
                        if (responder(&j, s[i]) != (i == tam - 1 ? RODADA_COMPLETA : COR_CERTA))
                            acertos_certos = 0;
                    if (erro == tam) {
                        if (j.pontuacao != tam || !j.ativa) acertos_certos = 0;
                        partidas++;
                    } else {
                        if (responder(&j, (Cor)cor) != COR_ERRADA || j.ativa ||
                            j.pontuacao != tam - 1)
                            erros_certos = 0;
                        erros++;
                    }
                }
            }
        }
    }
    verificar("EXA-01", "acertar as 3905 sequencias de ate 5 cores", "todas completas",
              acertos_certos && partidas == 3905);
    verificar("EXA-02", "os 74220 erros possiveis nelas", "todos encerram a partida",
              erros_certos && erros == 74220);
    liberar_jogo(&j);
}

int main(void) {
    testar_inicializar_fila();
    testar_enfileirar();
    testar_desenfileirar();
    testar_frente();
    testar_imprimir();
    testar_cores();
    testar_partida();
    testar_todas_as_sequencias();
    printf("\nUnitarios: %d testes, %d falhas\n", total, falhas);
    return falhas != 0;
}
