CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic

MODULOS = src/cor.c src/fila.c src/jogo.c
CABECALHOS = src/cor.h src/fila.h src/jogo.h

all: build/fila-de-cores

build/fila-de-cores: src/main.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) src/main.c $(MODULOS) -o $@

build/testes: tests/test_unitarios.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_unitarios.c $(MODULOS) -o $@

run: build/fila-de-cores
	./build/fila-de-cores

test: build/testes build/fila-de-cores
	./build/testes
	sh tests/test_sistema.sh

clean:
	rm -rf build

.PHONY: all run test clean
