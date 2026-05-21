/**
 * ============================================================================
 * impressao.c — Processo de Impressão
 * ============================================================================
 *
 * Cria um processo filho para imprimir o estado do sistema.
 *
 * Autor: Grupo (Pessoa 3)
 * Disciplina: Sistemas Operacionais — UFV Florestal
 *
 * TODO: Implementar a impressão completa do estado do sistema.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "impressao.h"

/**
 * Cria um processo filho (fork) para imprimir o estado do sistema.
 *
 * O processo pai (gerenciador) aguarda a conclusão do processo de
 * impressão antes de continuar (garante que só um executa por vez).
 */
void criar_processo_impressao(TabelaDeProcessos *tabela,
                              EstadoPronto *pronto,
                              EstadoBloqueado *bloqueado,
                              EstadoExecucao *execucao,
                              Cpu *cpu,
                              int tempo) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("[ERRO] Falha ao criar processo de impressão");
        return;
    }

    if (pid == 0) {
        /* Processo filho: imprime e sai */
        imprimir_estado_sistema(tabela, pronto, bloqueado, execucao, cpu, tempo);
        exit(0);
    } else {
        /* Processo pai: aguarda o filho terminar */
        wait(NULL);
    }
}

/**
 * Imprime o estado completo do sistema na saída padrão.
 *
 * TODO: Expandir com mais detalhes (variáveis, programa, etc.)
 */
void imprimir_estado_sistema(TabelaDeProcessos *tabela,
                             EstadoPronto *pronto,
                             EstadoBloqueado *bloqueado,
                             EstadoExecucao *execucao,
                             Cpu *cpu,
                             int tempo) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║         ESTADO ATUAL DO SISTEMA                  ║\n");
    printf("╠══════════════════════════════════════════════════╣\n");
    printf("║ Tempo: %-42d║\n", tempo);
    printf("╠══════════════════════════════════════════════════╣\n");

    /* Processo em execução */
    printf("║ PROCESSO EM EXECUÇÃO:                            ║\n");
    if (execucao->indice >= 0 && execucao->indice < MAX_PROCESSOS) {
        ProcessoSimulado *proc = &tabela->processos[execucao->indice];
        printf("║   PID: %-3d | PC: %-3d | Prio: %d | CPU: %-3d       ║\n",
               proc->pid, cpu->pc, proc->prioridade, proc->tempo_cpu);
    } else {
        printf("║   Nenhum                                         ║\n");
    }

    /* Fila de prontos */
    printf("╠══════════════════════════════════════════════════╣\n");
    printf("║ FILA DE PRONTOS:                                 ║\n");
    for (int p = 0; p < NUM_PRIORIDADES; p++) {
        printf("║   Prioridade %d (quantum %d): ", p, obter_quantum(p));
        if (fila_vazia(&pronto->filas[p])) {
            printf("vazia");
        } else {
            FilaSimples *f = &pronto->filas[p];
            int pos = f->inicio;
            for (int j = 0; j < f->tamanho; j++) {
                int idx = f->indices[pos];
                printf("PID%d ", tabela->processos[idx].pid);
                pos = (pos + 1) % MAX_PROCESSOS;
            }
        }
        printf("\n");
    }

    /* Fila de bloqueados */
    printf("╠══════════════════════════════════════════════════╣\n");
    printf("║ FILA DE BLOQUEADOS:                              ║\n");
    if (fila_vazia(bloqueado)) {
        printf("║   Nenhum processo bloqueado                      ║\n");
    } else {
        int pos = bloqueado->inicio;
        for (int j = 0; j < bloqueado->tamanho; j++) {
            int idx = bloqueado->indices[pos];
            ProcessoSimulado *proc = &tabela->processos[idx];
            printf("║   PID: %-3d | Tempo restante: %-3d                ║\n",
                   proc->pid, proc->tempo_bloqueio);
            pos = (pos + 1) % MAX_PROCESSOS;
        }
    }

    /* Tabela de processos */
    printf("╠══════════════════════════════════════════════════╣\n");
    printf("║ TABELA DE PROCESSOS (%d ativos):                 ║\n",
           tabela->quantidade);
    printf("║   PID | PAI | ESTADO     | PRIO | PC  | T.CPU   ║\n");
    printf("║   ----+-----+------------+------+-----+-------- ║\n");
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela->processos[i].ativo) {
            ProcessoSimulado *proc = &tabela->processos[i];
            printf("║   %-3d | %-3d | %-10s | %-4d | %-3d | %-3d     ║\n",
                   proc->pid, proc->pid_pai,
                   estado_para_string(proc->estado),
                   proc->prioridade, proc->pc, proc->tempo_cpu);
        }
    }

    printf("╚══════════════════════════════════════════════════╝\n\n");
}
