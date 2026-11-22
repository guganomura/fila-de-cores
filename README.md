# Fila de Cores

Jogo de memória de sequência em C, feito para a atividade de fila dinâmica. A cada rodada uma cor
entra na sequência e o jogador a repete na mesma ordem. A sequência fica em uma fila implementada
manualmente com nós e ponteiros. Usa apenas a biblioteca padrão do C.

- [Compilação e execução](#compilação-e-execução)
- [Como jogar](#como-jogar)
- [A fila e suas operações](#a-fila-e-suas-operações)
- [Funcionalidades](#funcionalidades)
- [Exemplos de execução](#exemplos-de-execução)
- [Testes realizados](#testes-realizados)
- [Resultados dos testes](#resultados-dos-testes)

## Compilação e execução

É necessário um compilador C99 (GCC ou Clang).

Com Make (macOS e Linux):

```bash
make        # compila
make run    # executa
make test   # roda todos os testes
make clean  # apaga os binários
```

Sem Make:

```bash
mkdir -p build
cc -std=c99 src/main.c src/cor.c src/fila.c src/jogo.c -o build/fila-de-cores
./build/fila-de-cores

cc -std=c99 tests/test_unitarios.c src/cor.c src/fila.c src/jogo.c -o build/testes
./build/testes
sh tests/test_sistema.sh
```

No Windows, com MinGW:

```bat
gcc -std=c99 src/main.c src/cor.c src/fila.c src/jogo.c -o fila-de-cores.exe
fila-de-cores.exe
```

Para repetir um sorteio, como em uma demonstração, informe a semente. A 1903 começa com o exemplo
do enunciado (VERDE, VERMELHO, AZUL):

```bash
./build/fila-de-cores --semente 1903
```

## Como jogar

1. Escolha **1. Nova partida**.
2. Memorize a sequência e pressione Enter. A tela é limpa.
3. Informe uma cor por vez, pelo número ou pelo nome.
4. Se acertar toda a sequência, ganha um ponto e vai para a rodada seguinte, com mais uma cor.
5. Se errar, a partida termina e a pontuação final aparece.

| Número | 1 | 2 | 3 | 4 | 5 |
|---|:-:|:-:|:-:|:-:|:-:|
| Cor | VERDE | VERMELHO | AZUL | ROXO | LARANJA |

| Comando | Efeito |
|:-:|---|
| `R` | reinicia a partida |
| `M` | encerra a partida e volta ao menu |
| `S` | encerra o jogo |

## Organização

```text
src/cor.h, cor.c          cores e leitura de uma cor
src/fila.h, fila.c        fila dinâmica
src/jogo.h, jogo.c        regras, rodadas e pontuação
src/main.c                menu, sorteio e tela
tests/test_unitarios.c    testes da fila e das regras
tests/test_sistema.sh     testes do jogo completo
```

## A fila e suas operações

```c
typedef struct No {
    Cor cor;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int quantidade;
} Fila;
```

```text
       sai por aqui                                  entra por aqui
            |                                              |
            v                                              v
          inicio                                          fim
            |                                              |
            v                                              v
       [VERDE | *] ---> [VERMELHO | *] ---> [AZUL | *] ---> NULL
```

O primeiro que entra é o primeiro que sai (FIFO), a mesma ordem em que o jogador deve responder.

| Operação | Retorno | Tempo |
|---|---|:-:|
| `inicializar_fila(fila)` | nenhum | O(1) |
| `enfileirar(fila, cor)` | 1, ou 0 se faltar memória | O(1) |
| `desenfileirar(fila, &cor)` | 1, ou 0 se estiver vazia | O(1) |
| `frente(fila, &cor)` | 1, ou 0 se estiver vazia | O(1) |
| `imprimir(fila, saida)` | nenhum | O(n) |

### `inicializar_fila`

Cria o estado de fila vazia: `inicio = NULL`, `fim = NULL` e `quantidade = 0`.

### `enfileirar`

Cria um nó com `malloc` e o coloca no fim. O ponteiro `fim` evita percorrer a fila.

```text
fila vazia:
1. novo->proximo = NULL
2. inicio = novo
3. fim = novo

fila com elementos:     [A] -> [B] -> NULL      fim = B
1. novo->proximo = NULL
2. fim->proximo = novo
3. fim = novo
resultado:              [A] -> [B] -> [N] -> NULL      fim = N
```

Se os passos 2 e 3 fossem invertidos, o novo nó apontaria para si mesmo e B não se ligaria a ele.

### `desenfileirar`

Retira o nó do início e devolve sua memória com `free`.

```text
antes:    [A] -> [B] -> NULL      inicio = A

1. retirado = inicio
2. inicio = retirado->proximo
3. se inicio == NULL, então fim = NULL
4. free(retirado)

depois:   [B] -> NULL             inicio = B
```

O passo 3 é necessário ao retirar o último nó. Sem ele, `fim` apontaria para memória já liberada.

### `frente`

Informa a cor do início sem retirá-la. É a cor que o jogador deve informar.

### `imprimir`

Percorre os nós do início ao fim, sem retirar, e escreve `VERDE -> VERMELHO -> AZUL`.

### Como o jogo usa a fila

`desenfileirar` destrói o que lê, e a sequência precisa continuar existindo nas rodadas seguintes.
Por isso o jogo tem duas filas:

- `sequencia`: guarda a sequência completa e recebe uma cor por rodada;
- `restantes`: cópia criada a cada rodada, com as cores que faltam responder.

```text
rodada 3             sequencia                       restantes
início               VERDE -> VERMELHO -> AZUL       VERDE -> VERMELHO -> AZUL
responde VERDE       VERDE -> VERMELHO -> AZUL       VERMELHO -> AZUL
responde VERMELHO    VERDE -> VERMELHO -> AZUL       AZUL
responde AZUL        VERDE -> VERMELHO -> AZUL       (vazia): rodada completa
nova rodada          ... -> AZUL -> ROXO             recebe uma nova cópia
```

Cada resposta é comparada com `frente(restantes)`. Se for igual, `desenfileirar` a retira. Se for
diferente, a partida termina. `liberar_fila` é uma função auxiliar, necessária em C.

## Funcionalidades

| Requisito | Como usar | Função | Testes |
|---|---|---|---|
| Iniciar nova partida | `1` no menu | `iniciar_partida` | PAR-04, SIS-01 |
| Visualizar a sequência | automático | `imprimir` | SIS-02, SIS-09 |
| Apresentar antes da resposta | Enter para esconder | `apresentar_sequencia` | SIS-04, SIS-39, SIS-40 |
| Informar a sequência | número ou nome | `responder` | SIS-05, SIS-12 |
| Avançar ao acertar | automático | `proxima_rodada` | ROD-03, SIS-07 |
| Nova cor a cada rodada | automático | `enfileirar` | SIS-08, SIS-11 |
| Visualizar a pontuação | automático | `responder` | ROD-01, SIS-03, SIS-06 |
| Mensagem de sequência incorreta | automático | `responder` | ERR-01, SIS-14, SIS-15 |
| Saber que a partida acabou | automático | `mostrar_fim` | SIS-16, SIS-17 |
| Reiniciar a partida | `R` | `iniciar_partida` | REI-02, SIS-22 a SIS-25 |
| Encerrar o jogo | `S`, ou `0` no menu | `sair` | SIS-29 a SIS-32 |

### Decisões de projeto

- **Pontuação:** um ponto por sequência completa. Acertos parciais não pontuam.
- **Erro:** a partida termina na primeira cor diferente, sem esperar o resto.
- **Entrada inválida**, como `9` ou `abc`, é pedida de novo e não conta como erro.
- **Cores:** são cinco e podem se repetir, inclusive em seguida.
- **Sorteio:** usa um gerador próprio (xorshift), para a semente funcionar igual em qualquer sistema.

## Exemplos de execução

Execuções reais com `--semente 1903`. `[tela limpa]` marca onde a tela é apagada.

### Exemplo 1: acertos, erro e fim da partida

```text
=== FILA DE CORES ===
Memorize a sequencia e repita na mesma ordem.
Cada rodada acrescenta uma cor e cada rodada certa vale um ponto.
 1. Nova partida
 0. Encerrar o jogo
Opcao: 1

--- Rodada 1 | Pontuacao: 0 ---
Sequencia: VERDE
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter:
[tela limpa]
Repita a sequencia | Rodada 1 | Pontuacao: 0
1-VERDE  2-VERMELHO  3-AZUL  4-ROXO  5-LARANJA
(R reinicia | M encerra a partida | S sai do jogo)
Cor 1 de 1: 1

Sequencia correta! Pontuacao: 1

--- Rodada 2 | Pontuacao: 1 ---
Sequencia: VERDE -> VERMELHO
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter:
[tela limpa]
Repita a sequencia | Rodada 2 | Pontuacao: 1
1-VERDE  2-VERMELHO  3-AZUL  4-ROXO  5-LARANJA
(R reinicia | M encerra a partida | S sai do jogo)
Cor 1 de 2: 1
Cor 2 de 2: 2

Sequencia correta! Pontuacao: 2

--- Rodada 3 | Pontuacao: 2 ---
Sequencia: VERDE -> VERMELHO -> AZUL
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter:
[tela limpa]
Repita a sequencia | Rodada 3 | Pontuacao: 2
1-VERDE  2-VERMELHO  3-AZUL  4-ROXO  5-LARANJA
(R reinicia | M encerra a partida | S sai do jogo)
Cor 1 de 3: 1
Cor 2 de 3: 2
Cor 3 de 3: 4

Sequencia incorreta! Esperado: AZUL. Informado: ROXO.

*** PARTIDA ENCERRADA ***
A sequencia era: VERDE -> VERMELHO -> AZUL
Pontuacao final: 2
```

### Exemplo 2: entrada inválida, reinício e saída

```text
=== FILA DE CORES ===
Memorize a sequencia e repita na mesma ordem.
Cada rodada acrescenta uma cor e cada rodada certa vale um ponto.
 1. Nova partida
 0. Encerrar o jogo
Opcao: 1

--- Rodada 1 | Pontuacao: 0 ---
Sequencia: VERDE
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter:
[tela limpa]
Repita a sequencia | Rodada 1 | Pontuacao: 0
1-VERDE  2-VERMELHO  3-AZUL  4-ROXO  5-LARANJA
(R reinicia | M encerra a partida | S sai do jogo)
Cor 1 de 1: 9
Cor invalida. Digite o numero (1 a 5) ou o nome.
Cor 1 de 1: verde

Sequencia correta! Pontuacao: 1

--- Rodada 2 | Pontuacao: 1 ---
Sequencia: VERDE -> VERMELHO
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter:
[tela limpa]
Repita a sequencia | Rodada 2 | Pontuacao: 1
1-VERDE  2-VERMELHO  3-AZUL  4-ROXO  5-LARANJA
(R reinicia | M encerra a partida | S sai do jogo)
Cor 1 de 2: r
[tela limpa]
Partida reiniciada.

--- Rodada 1 | Pontuacao: 0 ---
Sequencia: AZUL
(R reinicia | M encerra a partida | S sai do jogo)
Memorize e pressione Enter: s

*** PARTIDA ENCERRADA ***
A sequencia era: AZUL
Pontuacao final: 0

Jogo encerrado.
```

## Testes realizados

São **90 testes automatizados**: 49 unitários e 41 de sistema. Para repetir:

```bash
make test
```

Os testes EXA são exaustivos: jogam as 3905 sequências possíveis de até 5 cores e os 74220 erros
possíveis nelas.

### Fila

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| INI-01 | inicializar sobre lixo de memoria | `inicio = fim = NULL, quantidade = 0` | Passou |
| INI-02 | liberar fila com dois nos | `fila vazia` | Passou |
| INI-03 | liberar duas vezes | `fila vazia, sem falha` | Passou |
| ENF-01 | enfileirar em fila vazia | `inicio e fim no mesmo no` | Passou |
| ENF-02 | enfileirar o segundo | `entra no fim` | Passou |
| ENF-03 | inicio apos enfileirar | `nao muda` | Passou |
| ENF-04 | referencia do antigo fim | `aponta para o novo no` | Passou |
| ENF-05 | cor repetida | `aceita` | Passou |
| ENF-06 | 100000 nos sem capacidade previa | `quantidade = 100000` | Passou |
| DES-01 | desenfileirar fila vazia | `falha, destino inalterado` | Passou |
| DES-02 | desenfileirar de tres | `sai o primeiro que entrou` | Passou |
| DES-03 | referencia do inicio | `passa ao segundo no` | Passou |
| DES-04 | ordem FIFO | `depois VERMELHO e AZUL` | Passou |
| DES-05 | retirar o ultimo | `inicio e fim voltam a NULL` | Passou |
| DES-06 | enfileirar apos esvaziar | `inicio e fim no novo no` | Passou |
| DES-07 | destino NULL | `retira normalmente` | Passou |
| FRE-01 | frente de fila vazia | `falha, destino inalterado` | Passou |
| FRE-02 | frente de AZUL, VERDE | `AZUL` | Passou |
| FRE-03 | consultar nao retira | `fila inalterada` | Passou |
| FRE-04 | frente apos desenfileirar | `VERDE` | Passou |
| IMP-01 | imprimir fila vazia | `(vazia)` | Passou |
| IMP-02 | imprimir uma cor | `VERDE` | Passou |
| IMP-03 | exemplo do enunciado | `VERDE -> VERMELHO -> AZUL` | Passou |
| IMP-04 | imprimir nao retira | `fila inalterada` | Passou |

### Regras do jogo

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| COR-01 | ler numeros 1 e 5 | `VERDE e LARANJA` | Passou |
| COR-02 | ler nome em qualquer caixa | `reconhece` | Passou |
| COR-03 | numeros fora de 1 a 5 | `rejeita` | Passou |
| COR-04 | nome incompleto, errado ou vazio | `rejeita` | Passou |
| PAR-01 | jogo recem-criado | `inativo, vazio, 0 pontos` | Passou |
| PAR-02 | responder sem partida | `rejeita` | Passou |
| PAR-03 | avancar sem partida | `rejeita` | Passou |
| PAR-04 | nova partida | `uma cor, 0 pontos` | Passou |
| PAR-05 | avancar antes de responder | `rejeita, nada muda` | Passou |
| ROD-01 | acertar a rodada 1 | `1 ponto` | Passou |
| ROD-02 | responder rodada ja completa | `rejeita, pontos iguais` | Passou |
| ROD-03 | avancar de rodada | `nova cor no fim` | Passou |
| ROD-04 | fila de respostas da rodada | `copia da sequencia inteira` | Passou |
| ROD-05 | acerto parcial | `consome so a copia, sem ponto` | Passou |
| ROD-06 | completar a rodada 2 | `2 pontos` | Passou |
| ERR-01 | cor certa fora de ordem | `erro, partida encerrada` | Passou |
| ERR-02 | dados apos o erro | `pontos e sequencia mantidos` | Passou |
| ERR-03 | responder apos o erro | `rejeita` | Passou |
| ERR-04 | avancar apos o erro | `rejeita` | Passou |
| REI-01 | nova partida apos errar | `uma cor, 0 pontos` | Passou |
| REI-02 | reiniciar no meio da resposta | `descarta a anterior` | Passou |
| FIM-01 | encerrar a partida | `inativa, sequencia mantida` | Passou |
| FIM-02 | liberar o jogo | `as duas filas vazias` | Passou |
| EXA-01 | acertar as 3905 sequencias de ate 5 cores | `todas completas` | Passou |
| EXA-02 | os 74220 erros possiveis nelas | `todos encerram a partida` | Passou |

### Sistema

Enviam entradas ao executável com semente fixa e conferem a saída.

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| SIS-01 | iniciar nova partida | `rodada 1` | Passou |
| SIS-02 | visualizar a sequencia | `VERDE` | Passou |
| SIS-03 | pontuacao inicial | `0` | Passou |
| SIS-04 | pedir para memorizar | `espera Enter` | Passou |
| SIS-05 | informar a sequencia | `pede cor por cor` | Passou |
| SIS-06 | acertar a rodada 1 | `mensagem e 1 ponto` | Passou |
| SIS-07 | avancar de rodada | `rodada 2` | Passou |
| SIS-08 | nova cor na rodada 2 | `entra no fim` | Passou |
| SIS-09 | exemplo do enunciado | `tres cores` | Passou |
| SIS-10 | acertar a rodada 3 | `3 pontos` | Passou |
| SIS-11 | quarta cor | `entra no fim` | Passou |
| SIS-12 | responder pelo nome | `aceita` | Passou |
| SIS-13 | nome em maiusculas com espacos | `aceita` | Passou |
| SIS-14 | cor errada | `mensagem de erro` | Passou |
| SIS-15 | explicar o erro | `cor esperada e informada` | Passou |
| SIS-16 | avisar o fim da partida | `partida encerrada` | Passou |
| SIS-17 | pontuacao final apos erro | `2` | Passou |
| SIS-18 | cores certas fora de ordem | `erro na primeira` | Passou |
| SIS-19 | sequencia no resumo final | `completa` | Passou |
| SIS-20 | jogar de novo apos errar | `nova sequencia` | Passou |
| SIS-21 | pontuacao da nova partida | `comeca em 0` | Passou |
| SIS-22 | reiniciar ao memorizar | `partida reiniciada` | Passou |
| SIS-23 | reiniciar ao responder | `partida reiniciada` | Passou |
| SIS-24 | pontuacao apos reiniciar | `zera` | Passou |
| SIS-25 | sequencia apos reiniciar | `recomeca com uma cor` | Passou |
| SIS-26 | encerrar partida | `pontuacao final` | Passou |
| SIS-27 | encerrar partida ao responder | `volta ao menu` | Passou |
| SIS-28 | nova partida apos encerrar | `nova sequencia` | Passou |
| SIS-29 | sair ao memorizar | `programa termina` | Passou |
| SIS-30 | sair ao responder | `programa termina` | Passou |
| SIS-31 | sair pelo menu | `programa termina` | Passou |
| SIS-32 | sair no meio da partida | `pontuacao final` | Passou |
| SIS-33 | cor inexistente | `pede de novo` | Passou |
| SIS-34 | entrada invalida nao e erro | `ainda pontua` | Passou |
| SIS-35 | resposta vazia | `pede de novo` | Passou |
| SIS-36 | responder antes de esconder | `pede Enter` | Passou |
| SIS-37 | opcao invalida no menu | `pede de novo` | Passou |
| SIS-38 | fim inesperado da entrada | `encerra sem travar` | Passou |
| SIS-39 | sequencia durante a resposta | `escondida` | Passou |
| SIS-40 | limpar a tela apos memorizar | `comando de limpeza enviado` | Passou |
| SIS-41 | semente invalida | `recusa` | Passou |

## Resultados dos testes

| Grupo | Executados | Passaram | Falharam |
|---|:-:|:-:|:-:|
| Fila | 24 | 24 | 0 |
| Regras do jogo | 25 | 25 | 0 |
| Sistema | 41 | 41 | 0 |
| **Total** | 90 | 90 | 0 |

Ambiente da execução: macOS 26.5.1 (arm64), Apple clang version 21.0.0 (clang-2100.1.1.101).

<details>
<summary>Saída completa de <code>make test</code></summary>

```text
[OK] INI-01 inicializar sobre lixo de memoria -> inicio = fim = NULL, quantidade = 0
[OK] INI-02 liberar fila com dois nos -> fila vazia
[OK] INI-03 liberar duas vezes -> fila vazia, sem falha
[OK] ENF-01 enfileirar em fila vazia -> inicio e fim no mesmo no
[OK] ENF-02 enfileirar o segundo -> entra no fim
[OK] ENF-03 inicio apos enfileirar -> nao muda
[OK] ENF-04 referencia do antigo fim -> aponta para o novo no
[OK] ENF-05 cor repetida -> aceita
[OK] ENF-06 100000 nos sem capacidade previa -> quantidade = 100000
[OK] DES-01 desenfileirar fila vazia -> falha, destino inalterado
[OK] DES-02 desenfileirar de tres -> sai o primeiro que entrou
[OK] DES-03 referencia do inicio -> passa ao segundo no
[OK] DES-04 ordem FIFO -> depois VERMELHO e AZUL
[OK] DES-05 retirar o ultimo -> inicio e fim voltam a NULL
[OK] DES-06 enfileirar apos esvaziar -> inicio e fim no novo no
[OK] DES-07 destino NULL -> retira normalmente
[OK] FRE-01 frente de fila vazia -> falha, destino inalterado
[OK] FRE-02 frente de AZUL, VERDE -> AZUL
[OK] FRE-03 consultar nao retira -> fila inalterada
[OK] FRE-04 frente apos desenfileirar -> VERDE
[OK] IMP-01 imprimir fila vazia -> (vazia)
[OK] IMP-02 imprimir uma cor -> VERDE
[OK] IMP-03 exemplo do enunciado -> VERDE -> VERMELHO -> AZUL
[OK] IMP-04 imprimir nao retira -> fila inalterada
[OK] COR-01 ler numeros 1 e 5 -> VERDE e LARANJA
[OK] COR-02 ler nome em qualquer caixa -> reconhece
[OK] COR-03 numeros fora de 1 a 5 -> rejeita
[OK] COR-04 nome incompleto, errado ou vazio -> rejeita
[OK] PAR-01 jogo recem-criado -> inativo, vazio, 0 pontos
[OK] PAR-02 responder sem partida -> rejeita
[OK] PAR-03 avancar sem partida -> rejeita
[OK] PAR-04 nova partida -> uma cor, 0 pontos
[OK] PAR-05 avancar antes de responder -> rejeita, nada muda
[OK] ROD-01 acertar a rodada 1 -> 1 ponto
[OK] ROD-02 responder rodada ja completa -> rejeita, pontos iguais
[OK] ROD-03 avancar de rodada -> nova cor no fim
[OK] ROD-04 fila de respostas da rodada -> copia da sequencia inteira
[OK] ROD-05 acerto parcial -> consome so a copia, sem ponto
[OK] ROD-06 completar a rodada 2 -> 2 pontos
[OK] ERR-01 cor certa fora de ordem -> erro, partida encerrada
[OK] ERR-02 dados apos o erro -> pontos e sequencia mantidos
[OK] ERR-03 responder apos o erro -> rejeita
[OK] ERR-04 avancar apos o erro -> rejeita
[OK] REI-01 nova partida apos errar -> uma cor, 0 pontos
[OK] REI-02 reiniciar no meio da resposta -> descarta a anterior
[OK] FIM-01 encerrar a partida -> inativa, sequencia mantida
[OK] FIM-02 liberar o jogo -> as duas filas vazias
[OK] EXA-01 acertar as 3905 sequencias de ate 5 cores -> todas completas
[OK] EXA-02 os 74220 erros possiveis nelas -> todos encerram a partida

Unitarios: 49 testes, 0 falhas

[OK] SIS-01 iniciar nova partida -> rodada 1
[OK] SIS-02 visualizar a sequencia -> VERDE
[OK] SIS-03 pontuacao inicial -> 0
[OK] SIS-04 pedir para memorizar -> espera Enter
[OK] SIS-05 informar a sequencia -> pede cor por cor
[OK] SIS-06 acertar a rodada 1 -> mensagem e 1 ponto
[OK] SIS-07 avancar de rodada -> rodada 2
[OK] SIS-08 nova cor na rodada 2 -> entra no fim
[OK] SIS-09 exemplo do enunciado -> tres cores
[OK] SIS-10 acertar a rodada 3 -> 3 pontos
[OK] SIS-11 quarta cor -> entra no fim
[OK] SIS-12 responder pelo nome -> aceita
[OK] SIS-13 nome em maiusculas com espacos -> aceita
[OK] SIS-14 cor errada -> mensagem de erro
[OK] SIS-15 explicar o erro -> cor esperada e informada
[OK] SIS-16 avisar o fim da partida -> partida encerrada
[OK] SIS-17 pontuacao final apos erro -> 2
[OK] SIS-18 cores certas fora de ordem -> erro na primeira
[OK] SIS-19 sequencia no resumo final -> completa
[OK] SIS-20 jogar de novo apos errar -> nova sequencia
[OK] SIS-21 pontuacao da nova partida -> comeca em 0
[OK] SIS-22 reiniciar ao memorizar -> partida reiniciada
[OK] SIS-23 reiniciar ao responder -> partida reiniciada
[OK] SIS-24 pontuacao apos reiniciar -> zera
[OK] SIS-25 sequencia apos reiniciar -> recomeca com uma cor
[OK] SIS-26 encerrar partida -> pontuacao final
[OK] SIS-27 encerrar partida ao responder -> volta ao menu
[OK] SIS-28 nova partida apos encerrar -> nova sequencia
[OK] SIS-29 sair ao memorizar -> programa termina
[OK] SIS-30 sair ao responder -> programa termina
[OK] SIS-31 sair pelo menu -> programa termina
[OK] SIS-32 sair no meio da partida -> pontuacao final
[OK] SIS-33 cor inexistente -> pede de novo
[OK] SIS-34 entrada invalida nao e erro -> ainda pontua
[OK] SIS-35 resposta vazia -> pede de novo
[OK] SIS-36 responder antes de esconder -> pede Enter
[OK] SIS-37 opcao invalida no menu -> pede de novo
[OK] SIS-38 fim inesperado da entrada -> encerra sem travar
[OK] SIS-39 sequencia durante a resposta -> escondida
[OK] SIS-40 limpar a tela apos memorizar -> comando de limpeza enviado
[OK] SIS-41 semente invalida -> recusa

Sistema: 41 testes, 0 falhas
```

</details>
