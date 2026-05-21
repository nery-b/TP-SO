/**
 * ============================================================================
 * executor.c — Executor de Instruções de Processos Simulados
 * ============================================================================
 *
 * Executa instruções do processo simulado atualmente na CPU.
 * A execução é totalmente confinada à estrutura Cpu.
 *
 * Autor: Grupo (Pessoa 2)
 * Disciplina: Sistemas Operacionais — UFV Florestal
 *
 * TODO: Implementar a lógica de execução de cada instrução.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "parser.h"

/**
 * Executa a próxima instrução na CPU.
 *
 * TODO: Implementar o switch para cada tipo de instrução:
 *
 *   N n     — Aloca vetor de n variáveis (calloc)
 *   D x     — Inicializa variável x com 0
 *   V x n   — cpu->variaveis[x] = n
 *   A x n   — cpu->variaveis[x] += n
 *   S x n   — cpu->variaveis[x] -= n
 *   B n     — Retorna EXEC_BLOQUEIO, seta tempo_bloqueio
 *   T       — Retorna EXEC_TERMINO
 *   F n     — Copia processo, cria filho, retorna EXEC_FORK
 *   R arq   — Carrega novo programa com parser, retorna EXEC_REPLACE
 */
ResultadoExecucao executar_instrucao(Cpu *cpu, TabelaDeProcessos *tabela,
                                     EstadoPronto *pronto,
                                     EstadoBloqueado *bloqueado,
                                     EstadoExecucao *execucao, int *tempo) {
    /* Stub: a ser implementado */
    (void)cpu;
    (void)tabela;
    (void)pronto;
    (void)bloqueado;
    (void)execucao;
    (void)tempo;

    fprintf(stderr, "[STUB] executar_instrucao: não implementado ainda\n");
    return EXEC_ERRO;
}
