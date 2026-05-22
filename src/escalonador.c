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
static int prioridade_mais_alta_pronto(EstadoPronto *pronto) {
    if (pronto == NULL) return -1;

    for (int p = 0; p < NUM_PRIORIDADES; p++) {
        if (!fila_vazia(&pronto->filas[p])) {
            return p;
        }
    }
    return -1;
}

void escalonar(Cpu *cpu, TabelaDeProcessos *tabela,
               EstadoPronto *pronto, EstadoBloqueado *bloqueado,
               EstadoExecucao *execucao) {
    if (cpu == NULL || tabela == NULL || pronto == NULL || bloqueado == NULL || execucao == NULL) {
        return;
    }

    int indice_antigo = execucao->indice;
    if (indice_antigo >= 0 && indice_antigo < MAX_PROCESSOS) {
        ProcessoSimulado *proc = &tabela->processos[indice_antigo];
        int top_prioridade = prioridade_mais_alta_pronto(pronto);

        if (top_prioridade >= 0 && top_prioridade < proc->prioridade) {
            proc->estado = PRONTO;
            enfileirar_pronto(pronto, indice_antigo, proc->prioridade);
            execucao->indice = -1;
        } else if (cpu->tempo_usado_quantum >= cpu->quantum) {
            if (proc->prioridade < NUM_PRIORIDADES - 1) {
                proc->prioridade++;
            }
            proc->estado = PRONTO;
            enfileirar_pronto(pronto, indice_antigo, proc->prioridade);
            execucao->indice = -1;
        }
    }

    if (execucao->indice == -1) {
        int proximo = desenfileirar_pronto(pronto);
        if (proximo >= 0) {
            trocar_contexto(cpu, tabela, indice_antigo, proximo);
            tabela->processos[proximo].estado = EXECUCAO;
            execucao->indice = proximo;
        } else {
            trocar_contexto(cpu, tabela, indice_antigo, -1);
        }
    }
}

void trocar_contexto(Cpu *cpu, TabelaDeProcessos *tabela,
                     int indice_antigo, int indice_novo) {
    if (cpu == NULL || tabela == NULL) return;

    if (indice_antigo >= 0 && indice_antigo < MAX_PROCESSOS) {
        ProcessoSimulado *antigo = &tabela->processos[indice_antigo];
        antigo->pc = cpu->pc;
        antigo->programa = cpu->programa;
        antigo->tamanho_programa = cpu->tamanho_programa;
        antigo->variaveis = cpu->variaveis;
        antigo->num_variaveis = cpu->num_variaveis;
    }

    if (indice_novo >= 0 && indice_novo < MAX_PROCESSOS) {
        ProcessoSimulado *novo = &tabela->processos[indice_novo];
        cpu->programa = novo->programa;
        cpu->tamanho_programa = novo->tamanho_programa;
        cpu->pc = novo->pc;
        cpu->variaveis = novo->variaveis;
        cpu->num_variaveis = novo->num_variaveis;
        cpu->quantum = obter_quantum(novo->prioridade);
        cpu->tempo_usado_quantum = 0;
    } else {
        cpu->programa = NULL;
        cpu->tamanho_programa = 0;
        cpu->pc = 0;
        cpu->variaveis = NULL;
        cpu->num_variaveis = 0;
        cpu->quantum = 0;
        cpu->tempo_usado_quantum = 0;
    }
}

void atualizar_bloqueados(TabelaDeProcessos *tabela,
                          EstadoPronto *pronto,
                          EstadoBloqueado *bloqueado) {
    if (tabela == NULL || pronto == NULL || bloqueado == NULL) return;

    int tamanho_inicial = bloqueado->tamanho;
    for (int i = 0; i < tamanho_inicial; i++) {
        int indice = desenfileirar(bloqueado);
        if (indice < 0) break;

        ProcessoSimulado *proc = &tabela->processos[indice];
        proc->tempo_bloqueio--;
        if (proc->tempo_bloqueio <= 0) {
            proc->estado = PRONTO;
            enfileirar_pronto(pronto, indice, proc->prioridade);
        } else {
            enfileirar(bloqueado, indice);
        }
    }
}
