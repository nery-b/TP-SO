# ============================================================================
# Makefile — Simulador de Gerenciamento de Processos
# ============================================================================
#
# Estrutura do projeto:
#   include/  — Headers (.h)
#   src/      — Fontes (.c)
#   tests/    — Testes
#   programs/ — Programas simulados de exemplo
#
# Uso:
#   make          → compila o simulador completo
#   make test     → compila e roda os testes das estruturas
#   make clean    → remove binários e objetos
#   make run      → compila e executa o simulador
# ============================================================================

CC       = gcc
CFLAGS   = -Wall -Wextra -g -std=c11 -pthread
INCDIR   = include
SRCDIR   = src
TESTDIR  = tests

# Adiciona include/ ao caminho de busca de headers
CFLAGS  += -I$(INCDIR)

# ---- Fontes e objetos do simulador ----
SRCS     = $(wildcard $(SRCDIR)/*.c)
OBJS     = $(SRCS:$(SRCDIR)/%.c=$(SRCDIR)/%.o)
TARGET   = simulador

# ============================================================================
# Alvos principais
# ============================================================================

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# Testes
# ============================================================================

test: teste_estruturas
	@echo ""
	@echo "=== Executando testes ==="
	@echo ""
	./teste_estruturas

teste_estruturas: $(TESTDIR)/teste_estruturas.o $(SRCDIR)/estruturas.o
	$(CC) $(CFLAGS) -o $@ $^

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# Executar
# ============================================================================

run: $(TARGET)
	./$(TARGET)

# ============================================================================
# Limpeza
# ============================================================================

clean:
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(TARGET) teste_estruturas

# ============================================================================
# Dependências explícitas (headers)
# ============================================================================

$(SRCDIR)/main.o:         $(SRCDIR)/main.c $(INCDIR)/controle.h $(INCDIR)/gerenciador.h
$(SRCDIR)/estruturas.o:   $(SRCDIR)/estruturas.c $(INCDIR)/estruturas.h
$(SRCDIR)/controle.o:     $(SRCDIR)/controle.c $(INCDIR)/controle.h
$(SRCDIR)/gerenciador.o:  $(SRCDIR)/gerenciador.c $(INCDIR)/gerenciador.h $(INCDIR)/estruturas.h \
                          $(INCDIR)/parser.h $(INCDIR)/executor.h $(INCDIR)/escalonador.h $(INCDIR)/impressao.h
$(SRCDIR)/parser.o:       $(SRCDIR)/parser.c $(INCDIR)/parser.h $(INCDIR)/estruturas.h
$(SRCDIR)/executor.o:     $(SRCDIR)/executor.c $(INCDIR)/executor.h $(INCDIR)/parser.h $(INCDIR)/estruturas.h
$(SRCDIR)/escalonador.o:  $(SRCDIR)/escalonador.c $(INCDIR)/escalonador.h $(INCDIR)/estruturas.h
$(SRCDIR)/impressao.o:    $(SRCDIR)/impressao.c $(INCDIR)/impressao.h $(INCDIR)/estruturas.h

$(TESTDIR)/teste_estruturas.o: $(TESTDIR)/teste_estruturas.c $(INCDIR)/estruturas.h
