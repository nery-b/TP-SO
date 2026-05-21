/**
 * ============================================================================
 * teste_estruturas.c — Testes de validação das estruturas de dados
 * ============================================================================
 *
 * Testa inicialização, operações de fila, criação/liberação de processos
 * e funções utilitárias. Pode ser removido antes da entrega final.
 *
 * Compilar: make test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "estruturas.h"  /* include/ via -Iinclude no Makefile */

/* Contadores de testes */
static int testes_ok = 0;
static int testes_total = 0;

#define TESTE(descricao, condicao) do {                          \
    testes_total++;                                               \
    if (condicao) {                                               \
        printf("  [OK] %s\n", descricao);                        \
        testes_ok++;                                              \
    } else {                                                      \
        printf("  [FALHA] %s (linha %d)\n", descricao, __LINE__); \
    }                                                             \
} while(0)

/* ============================================================================ */

void testar_inicializacao_cpu(void) {
    printf("\n=== Teste: Inicialização da CPU ===\n");

    Cpu cpu;
    inicializar_cpu(&cpu);

    TESTE("programa é NULL", cpu.programa == NULL);
    TESTE("tamanho_programa é 0", cpu.tamanho_programa == 0);
    TESTE("pc é 0", cpu.pc == 0);
    TESTE("variaveis é NULL", cpu.variaveis == NULL);
    TESTE("num_variaveis é 0", cpu.num_variaveis == 0);
    TESTE("quantum é 0", cpu.quantum == 0);
    TESTE("tempo_usado_quantum é 0", cpu.tempo_usado_quantum == 0);
}

void testar_inicializacao_tabela(void) {
    printf("\n=== Teste: Inicialização da TabelaDeProcessos ===\n");

    TabelaDeProcessos tabela;
    inicializar_tabela(&tabela);

    TESTE("quantidade é 0", tabela.quantidade == 0);
    TESTE("proximo_pid é 0", tabela.proximo_pid == 0);
    TESTE("primeiro processo inativo", tabela.processos[0].ativo == 0);
    TESTE("último processo inativo", tabela.processos[MAX_PROCESSOS - 1].ativo == 0);
    TESTE("PID do primeiro é -1", tabela.processos[0].pid == -1);
}

void testar_fila_simples(void) {
    printf("\n=== Teste: FilaSimples (Fila Circular) ===\n");

    FilaSimples fila;
    inicializar_fila(&fila);

    TESTE("fila começa vazia", fila_vazia(&fila) == 1);

    /* Enfileira 3 elementos */
    TESTE("enfileirar 10 retorna 0", enfileirar(&fila, 10) == 0);
    TESTE("enfileirar 20 retorna 0", enfileirar(&fila, 20) == 0);
    TESTE("enfileirar 30 retorna 0", enfileirar(&fila, 30) == 0);

    TESTE("fila não está vazia", fila_vazia(&fila) == 0);
    TESTE("tamanho é 3", fila.tamanho == 3);

    /* Desenfileira em ordem FIFO */
    TESTE("desenfileirar retorna 10", desenfileirar(&fila) == 10);
    TESTE("desenfileirar retorna 20", desenfileirar(&fila) == 20);
    TESTE("desenfileirar retorna 30", desenfileirar(&fila) == 30);

    TESTE("fila volta a ficar vazia", fila_vazia(&fila) == 1);
    TESTE("desenfileirar de fila vazia retorna -1", desenfileirar(&fila) == -1);
}

void testar_fila_circular_wrap(void) {
    printf("\n=== Teste: FilaSimples (wrap-around circular) ===\n");

    FilaSimples fila;
    inicializar_fila(&fila);

    /* Enfileira e desenfileira várias vezes para testar wrap-around */
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        enfileirar(&fila, i);
    }
    TESTE("fila cheia com MAX_PROCESSOS elementos", fila.tamanho == MAX_PROCESSOS);

    /* Tenta enfileirar em fila cheia */
    TESTE("enfileirar em fila cheia retorna -1", enfileirar(&fila, 999) == -1);

    /* Esvazia */
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        int val = desenfileirar(&fila);
        if (val != i) {
            printf("  [FALHA] Esperado %d, obtido %d\n", i, val);
            testes_total++;
            return;
        }
    }
    testes_total++;
    testes_ok++;
    printf("  [OK] Todos os %d elementos desenfileirados corretamente\n",
           MAX_PROCESSOS);

    /* Testa wrap-around: enfileira novamente após esvaziar */
    TESTE("enfileirar após esvaziar funciona", enfileirar(&fila, 42) == 0);
    TESTE("desenfileirar retorna 42", desenfileirar(&fila) == 42);
}

void testar_estado_pronto(void) {
    printf("\n=== Teste: EstadoPronto (Fila de Prioridades) ===\n");

    EstadoPronto pronto;
    inicializar_estado_pronto(&pronto);

    TESTE("estado pronto começa vazio", estado_pronto_vazio(&pronto) == 1);

    /* Enfileira processos em diferentes prioridades */
    TESTE("enfileirar prioridade 2 (índice 5)",
          enfileirar_pronto(&pronto, 5, 2) == 0);
    TESTE("enfileirar prioridade 0 (índice 1)",
          enfileirar_pronto(&pronto, 1, 0) == 0);
    TESTE("enfileirar prioridade 1 (índice 3)",
          enfileirar_pronto(&pronto, 3, 1) == 0);
    TESTE("enfileirar prioridade 0 (índice 7)",
          enfileirar_pronto(&pronto, 7, 0) == 0);

    TESTE("estado pronto não está vazio", estado_pronto_vazio(&pronto) == 0);

    /* Desenfileira — deve respeitar prioridade (0 primeiro) */
    TESTE("primeiro desenfileirado é 1 (prio 0)",
          desenfileirar_pronto(&pronto) == 1);
    TESTE("segundo desenfileirado é 7 (prio 0)",
          desenfileirar_pronto(&pronto) == 7);
    TESTE("terceiro desenfileirado é 3 (prio 1)",
          desenfileirar_pronto(&pronto) == 3);
    TESTE("quarto desenfileirado é 5 (prio 2)",
          desenfileirar_pronto(&pronto) == 5);

    TESTE("estado pronto volta a ficar vazio",
          estado_pronto_vazio(&pronto) == 1);

    /* Prioridade inválida */
    TESTE("prioridade -1 retorna erro",
          enfileirar_pronto(&pronto, 0, -1) == -1);
    TESTE("prioridade 4 retorna erro",
          enfileirar_pronto(&pronto, 0, NUM_PRIORIDADES) == -1);
}

void testar_criar_liberar_processo(void) {
    printf("\n=== Teste: Criar e Liberar Processos ===\n");

    TabelaDeProcessos tabela;
    inicializar_tabela(&tabela);

    /* Cria o primeiro processo (init, PID 0) */
    int idx0 = criar_processo(&tabela, -1, 0, 0);
    TESTE("criar processo retorna índice 0", idx0 == 0);
    TESTE("PID é 0", tabela.processos[idx0].pid == 0);
    TESTE("PID pai é -1", tabela.processos[idx0].pid_pai == -1);
    TESTE("prioridade é 0", tabela.processos[idx0].prioridade == 0);
    TESTE("estado é PRONTO", tabela.processos[idx0].estado == PRONTO);
    TESTE("ativo é 1", tabela.processos[idx0].ativo == 1);
    TESTE("quantidade é 1", tabela.quantidade == 1);
    TESTE("proximo_pid é 1", tabela.proximo_pid == 1);

    /* Cria um segundo processo (filho do init) */
    int idx1 = criar_processo(&tabela, 0, 0, 5);
    TESTE("criar segundo processo retorna índice 1", idx1 == 1);
    TESTE("PID é 1", tabela.processos[idx1].pid == 1);
    TESTE("PID pai é 0", tabela.processos[idx1].pid_pai == 0);
    TESTE("tempo_inicio é 5", tabela.processos[idx1].tempo_inicio == 5);
    TESTE("quantidade é 2", tabela.quantidade == 2);

    /* Simula alocação de programa e variáveis para o processo 0 */
    tabela.processos[idx0].programa = malloc(3 * sizeof(Instrucao));
    tabela.processos[idx0].tamanho_programa = 3;
    tabela.processos[idx0].variaveis = calloc(2, sizeof(int));
    tabela.processos[idx0].num_variaveis = 2;

    /* Libera processo 0 */
    liberar_processo(&tabela, idx0);
    TESTE("processo 0 inativo após liberar", tabela.processos[idx0].ativo == 0);
    TESTE("programa NULL após liberar", tabela.processos[idx0].programa == NULL);
    TESTE("variaveis NULL após liberar", tabela.processos[idx0].variaveis == NULL);
    TESTE("quantidade volta para 1", tabela.quantidade == 1);

    /* Cria novo processo — deve reutilizar slot 0 */
    int idx2 = criar_processo(&tabela, 1, 1, 10);
    TESTE("novo processo reutiliza slot 0", idx2 == 0);
    TESTE("PID do novo processo é 2", tabela.processos[idx2].pid == 2);

    /* Limpa */
    liberar_processo(&tabela, idx1);
    liberar_processo(&tabela, idx2);
}

void testar_quantum(void) {
    printf("\n=== Teste: obter_quantum ===\n");

    TESTE("prioridade 0 → quantum 1", obter_quantum(0) == 1);
    TESTE("prioridade 1 → quantum 2", obter_quantum(1) == 2);
    TESTE("prioridade 2 → quantum 4", obter_quantum(2) == 4);
    TESTE("prioridade 3 → quantum 8", obter_quantum(3) == 8);
}

void testar_estado_para_string(void) {
    printf("\n=== Teste: estado_para_string ===\n");

    TESTE("PRONTO", strcmp(estado_para_string(PRONTO), "PRONTO") == 0);
    TESTE("BLOQUEADO", strcmp(estado_para_string(BLOQUEADO), "BLOQUEADO") == 0);
    TESTE("EXECUCAO", strcmp(estado_para_string(EXECUCAO), "EXECUCAO") == 0);
    TESTE("TERMINADO", strcmp(estado_para_string(TERMINADO), "TERMINADO") == 0);
}

void testar_estado_execucao(void) {
    printf("\n=== Teste: EstadoExecucao ===\n");

    EstadoExecucao exec;
    inicializar_estado_execucao(&exec);

    TESTE("indice inicializado com -1", exec.indice == -1);

    exec.indice = 5;
    TESTE("indice atualizado para 5", exec.indice == 5);
}

void testar_estado_bloqueado(void) {
    printf("\n=== Teste: EstadoBloqueado ===\n");

    EstadoBloqueado bloqueado;
    inicializar_estado_bloqueado(&bloqueado);

    TESTE("bloqueado começa vazio", fila_vazia(&bloqueado) == 1);

    enfileirar(&bloqueado, 2);
    enfileirar(&bloqueado, 4);

    TESTE("não está vazio após enfileirar", fila_vazia(&bloqueado) == 0);
    TESTE("desenfileirar retorna 2", desenfileirar(&bloqueado) == 2);
    TESTE("desenfileirar retorna 4", desenfileirar(&bloqueado) == 4);
    TESTE("volta a ficar vazio", fila_vazia(&bloqueado) == 1);
}

/* ============================================================================ */

int main(void) {
    printf("=============================================\n");
    printf(" Testes das Estruturas de Dados do Simulador\n");
    printf("=============================================\n");

    testar_inicializacao_cpu();
    testar_inicializacao_tabela();
    testar_fila_simples();
    testar_fila_circular_wrap();
    testar_estado_pronto();
    testar_criar_liberar_processo();
    testar_quantum();
    testar_estado_para_string();
    testar_estado_execucao();
    testar_estado_bloqueado();

    printf("\n=============================================\n");
    printf(" Resultado: %d/%d testes passaram\n", testes_ok, testes_total);
    printf("=============================================\n");

    return (testes_ok == testes_total) ? 0 : 1;
}
