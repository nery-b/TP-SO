/**
 * ============================================================================
 * escalonador.c — Escalonador de Processos e Troca de Contexto
 * ============================================================================
 *
 * Implementa a política MLFQ e troca de contexto.
 *
 * Autor: Grupo (Pessoa 1)
 * Disciplina: Sistemas Operacionais — UFV Florestal
 *
 * TODO: Implementar a lógica de escalonamento e troca de contexto.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "escalonador.h"

/**
 * Realiza o escalonamento após cada unidade de tempo.
 *
 * TODO: Implementar a lógica MLFQ:
 *
 *   1. Incrementar tempo_usado_quantum da CPU
 *   2. Se quantum expirou:
 *      a. Diminuir prioridade do processo (min 3)
 *      b. Salvar estado (trocar_contexto)
 *      c. Mover processo para fila de prontos
 *      d. Selecionar próximo processo
 *   3. Se processo bloqueou (antes do quantum expirar):
 *      a. Aumentar prioridade do processo (max 0)
 *   4. Atualizar processos bloqueados (decrementar tempo)
 */
void escalonar(Cpu *cpu, TabelaDeProcessos *tabela,
               EstadoPronto *pronto, EstadoBloqueado *bloqueado,
               EstadoExecucao *execucao) {
    /* Stub: a ser implementado */
    (void)cpu;
    (void)tabela;
    (void)pronto;
    (void)bloqueado;
    (void)execucao;

    fprintf(stderr, "[STUB] escalonar: não implementado ainda\n");
}

/**
 * Realiza a troca de contexto entre dois processos.
 *
 * TODO: Implementar:
 *
 *   1. Se indice_antigo >= 0 (existe processo saindo):
 *      - Salvar cpu->pc       → tabela->processos[antigo].pc
 *      - Salvar cpu->programa → tabela->processos[antigo].programa
 *      - Salvar cpu->variaveis→ tabela->processos[antigo].variaveis
 *      - (e demais campos)
 *
 *   2. Carregar processo novo:
 *      - cpu->programa  = tabela->processos[novo].programa
 *      - cpu->pc        = tabela->processos[novo].pc
 *      - cpu->variaveis = tabela->processos[novo].variaveis
 *      - cpu->quantum   = obter_quantum(tabela->processos[novo].prioridade)
 *      - cpu->tempo_usado_quantum = 0
 */
void trocar_contexto(Cpu *cpu, TabelaDeProcessos *tabela,
                     int indice_antigo, int indice_novo) {
    /* Stub: a ser implementado */
    (void)cpu;
    (void)tabela;
    (void)indice_antigo;
    (void)indice_novo;

    fprintf(stderr, "[STUB] trocar_contexto: não implementado ainda\n");
}

/**
 * Atualiza processos bloqueados.
 *
 * TODO: Implementar:
 *
 *   Para cada processo na fila de bloqueados:
 *     1. Decrementar tempo_bloqueio
 *     2. Se tempo_bloqueio == 0:
 *        - Remover da fila de bloqueados
 *        - Mudar estado para PRONTO
 *        - Enfileirar na fila de prontos (com sua prioridade)
 */
void atualizar_bloqueados(TabelaDeProcessos *tabela,
                          EstadoPronto *pronto,
                          EstadoBloqueado *bloqueado) {
    /* Stub: a ser implementado */
    (void)tabela;
    (void)pronto;
    (void)bloqueado;

    fprintf(stderr, "[STUB] atualizar_bloqueados: não implementado ainda\n");
}
