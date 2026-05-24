/**
 * ============================================================================
 * impressao.h — Processo de Impressão
 * ============================================================================
 *
 * O processo de impressão é criado via fork() pelo gerenciador de processos
 * sempre que recebe um comando I ou M. Ele imprime o estado atual do
 * sistema para a saída padrão, incluindo:
 *   - Tempo atual do simulador
 *   - Processo em execução (PID, PC, prioridade, etc.)
 *   - Processos na fila de prontos (por prioridade)
 *   - Processos na fila de bloqueados
 *   - Tabela de processos completa
 *
 * O gerenciador garante que apenas um processo de impressão execute
 * ao mesmo tempo.
 */

#ifndef IMPRESSAO_H
#define IMPRESSAO_H

#include "estruturas.h"

/**
 * Cria um processo filho (fork) para imprimir o estado do sistema.
 * Mantido como opção legada para fins de demonstração (fork vs pthread).
 *
 * @param tabela    Ponteiro para a TabelaDeProcessos
 * @param pronto    Ponteiro para a fila de prontos
 * @param bloqueado Ponteiro para a fila de bloqueados
 * @param execucao  Ponteiro para o estado de execução
 * @param cpu       Ponteiro para a CPU
 * @param tempo     Tempo atual do simulador
 */
void criar_processo_impressao(TabelaDeProcessos *tabela,
                              EstadoPronto *pronto,
                              EstadoBloqueado *bloqueado,
                              EstadoExecucao *execucao,
                              Cpu *cpu,
                              int tempo);

/**
 * Cria uma thread para imprimir o estado do sistema.
 *
 * A thread pai (gerenciador) aguarda a conclusão da thread de
 * impressão antes de continuar processando comandos.
 *
 * @param tabela    Ponteiro para a TabelaDeProcessos
 * @param pronto    Ponteiro para a fila de prontos
 * @param bloqueado Ponteiro para a fila de bloqueados
 * @param execucao  Ponteiro para o estado de execução
 * @param cpu       Ponteiro para a CPU
 * @param tempo     Tempo atual do simulador
 */
void criar_thread_impressao(TabelaDeProcessos *tabela,
                            EstadoPronto *pronto,
                            EstadoBloqueado *bloqueado,
                            EstadoExecucao *execucao,
                            Cpu *cpu,
                            int tempo);

/**
 * Imprime o estado completo do sistema na saída padrão.
 * Chamada internamente pelo processo de impressão (filho).
 *
 * @param tabela    Ponteiro para a TabelaDeProcessos
 * @param pronto    Ponteiro para a fila de prontos
 * @param bloqueado Ponteiro para a fila de bloqueados
 * @param execucao  Ponteiro para o estado de execução
 * @param cpu       Ponteiro para a CPU
 * @param tempo     Tempo atual do simulador
 */
void imprimir_estado_sistema(TabelaDeProcessos *tabela,
                             EstadoPronto *pronto,
                             EstadoBloqueado *bloqueado,
                             EstadoExecucao *execucao,
                             Cpu *cpu,
                             int tempo);

#endif /* IMPRESSAO_H */
