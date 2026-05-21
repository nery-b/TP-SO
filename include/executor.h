/**
 * ============================================================================
 * executor.h — Executor de Instruções de Processos Simulados
 * ============================================================================
 *
 * Executa a próxima instrução do processo simulado atualmente em execução.
 * A execução é totalmente confinada à estrutura de dados Cpu — a
 * TabelaDeProcessos NÃO é acessada durante a execução.
 *
 * Instruções suportadas:
 *   N n     — Define número de variáveis
 *   D x     — Declara variável x (valor inicial 0)
 *   V x n   — Define variável x para n
 *   A x n   — Adiciona n à variável x
 *   S x n   — Subtrai n da variável x
 *   B n     — Bloqueia processo por n unidades de tempo
 *   T       — Termina o processo
 *   F n     — Fork: cria novo processo (cópia do pai)
 *   R arq   — Replace: substitui imagem do processo
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "estruturas.h"

/**
 * Códigos de retorno da execução de instrução.
 * Indicam ao gerenciador que ação tomar após a execução.
 */
typedef enum {
    EXEC_OK,          /**< Instrução normal executada (D, V, A, S, N) */
    EXEC_BLOQUEIO,    /**< Instrução B: processo deve ser bloqueado */
    EXEC_TERMINO,     /**< Instrução T: processo terminou */
    EXEC_FORK,        /**< Instrução F: novo processo criado */
    EXEC_REPLACE,     /**< Instrução R: imagem substituída */
    EXEC_ERRO         /**< Erro na execução */
} ResultadoExecucao;

/**
 * Executa a próxima instrução do processo atualmente na CPU.
 *
 * @param cpu       Ponteiro para a CPU (onde a instrução é executada)
 * @param tabela    Ponteiro para a TabelaDeProcessos (usado por F e T)
 * @param pronto    Ponteiro para a fila de prontos (usado por F)
 * @param bloqueado Ponteiro para a fila de bloqueados (usado por B)
 * @param execucao  Ponteiro para o estado de execução
 * @param tempo     Ponteiro para o tempo atual do simulador
 * @return Código de ResultadoExecucao indicando o resultado
 */
ResultadoExecucao executar_instrucao(Cpu *cpu, TabelaDeProcessos *tabela,
                                     EstadoPronto *pronto,
                                     EstadoBloqueado *bloqueado,
                                     EstadoExecucao *execucao, int *tempo);

#endif /* EXECUTOR_H */
