/**
 * ============================================================================
 * escalonador.h — Escalonador de Processos e Troca de Contexto
 * ============================================================================
 *
 * Implementa a política de escalonamento de múltiplas filas com classes
 * de prioridade (MLFQ — Multi-Level Feedback Queue):
 *
 *   Prioridade 0 → quantum 1 (mais alta)
 *   Prioridade 1 → quantum 2
 *   Prioridade 2 → quantum 4
 *   Prioridade 3 → quantum 8 (mais baixa)
 *
 * Regras:
 *   - Se o processo usa toda a fatia de tempo → prioridade diminui
 *   - Se o processo bloqueia antes do quantum expirar → prioridade aumenta
 *   - Processos com maior prioridade são executados primeiro
 *
 * Também implementa uma segunda política de escalonamento definida
 * pelo grupo (a ser especificada na documentação).
 */

#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "estruturas.h"

/**
 * Tipo de política de escalonamento.
 */
typedef enum {
    POLITICA_FIFO,
    POLITICA_MLFQ
} PoliticaEscalonamento;

/**
 * Realiza o escalonamento após cada unidade de tempo (comando U).
 *
 * Dependendo da política selecionada, o processo em execução pode
 * ser mantido no CPU ou ser reenfileirado com base na fila de
 * prontos e na prioridade.
 *
 * @param cpu       Ponteiro para a CPU
 * @param tabela    Ponteiro para a TabelaDeProcessos
 * @param pronto    Ponteiro para a fila de prontos
 * @param bloqueado Ponteiro para a fila de bloqueados
 * @param execucao  Ponteiro para o estado de execução
 * @param politica  Política de escalonamento usada nesta execução
 */
void escalonar(Cpu *cpu, TabelaDeProcessos *tabela,
               EstadoPronto *pronto, EstadoBloqueado *bloqueado,
               EstadoExecucao *execucao, int modo_escalonamento);

/**
 * Realiza a troca de contexto entre dois processos simulados.
 *
 * 1. Salva o estado da CPU na entrada da TabelaDeProcessos do
 *    processo antigo (se indice_antigo >= 0)
 * 2. Carrega o estado da entrada da TabelaDeProcessos do processo
 *    novo para a CPU
 * 3. Configura o quantum conforme a prioridade do novo processo
 *
 * @param cpu           Ponteiro para a CPU
 * @param tabela        Ponteiro para a TabelaDeProcessos
 * @param indice_antigo Índice do processo saindo da CPU (-1 se nenhum)
 * @param indice_novo   Índice do processo entrando na CPU
 */
void trocar_contexto(Cpu *cpu, TabelaDeProcessos *tabela,
                     int indice_antigo, int indice_novo);

/**
 * Atualiza os processos bloqueados: decrementa tempo_bloqueio e
 * move para a fila de prontos os que completaram o bloqueio.
 *
 * @param tabela    Ponteiro para a TabelaDeProcessos
 * @param pronto    Ponteiro para a fila de prontos
 * @param bloqueado Ponteiro para a fila de bloqueados
 */
void atualizar_bloqueados(TabelaDeProcessos *tabela,
                          EstadoPronto *pronto,
                          EstadoBloqueado *bloqueado);

#endif /* ESCALONADOR_H */
