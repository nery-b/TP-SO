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

ResultadoExecucao executar_instrucao(Cpu *cpu, TabelaDeProcessos *tabela,
                                     EstadoPronto *pronto,
                                     EstadoBloqueado *bloqueado,
                                     EstadoExecucao *execucao, int *tempo) {
    (void)bloqueado;
    (void)tempo;

    if (cpu == NULL || cpu->programa == NULL || cpu->pc >= cpu->tamanho_programa) {
        return EXEC_TERMINO;
    }

    Instrucao inst = cpu->programa[cpu->pc];
    int indice_atual = (execucao != NULL ? execucao->indice : -1);

    switch (inst.tipo) {
        case 'N':
            if (cpu->variaveis != NULL) free(cpu->variaveis);
            cpu->variaveis = calloc(inst.operando1, sizeof(int));
            cpu->num_variaveis = inst.operando1;
            cpu->pc++;
            return EXEC_OK;

        case 'D':
            if (inst.operando1 >= 0 && inst.operando1 < cpu->num_variaveis) {
                cpu->variaveis[inst.operando1] = 0;
            }
            cpu->pc++;
            return EXEC_OK;

        case 'V':
            if (inst.operando1 >= 0 && inst.operando1 < cpu->num_variaveis) {
                cpu->variaveis[inst.operando1] = inst.operando2;
            }
            cpu->pc++;
            return EXEC_OK;

        case 'A':
            if (inst.operando1 >= 0 && inst.operando1 < cpu->num_variaveis) {
                cpu->variaveis[inst.operando1] += inst.operando2;
            }
            cpu->pc++;
            return EXEC_OK;

        case 'S':
            if (inst.operando1 >= 0 && inst.operando1 < cpu->num_variaveis) {
                cpu->variaveis[inst.operando1] -= inst.operando2;
            }
            cpu->pc++;
            return EXEC_OK;

        case 'B':
            if (indice_atual >= 0 && indice_atual < MAX_PROCESSOS) {
                tabela->processos[indice_atual].tempo_bloqueio = inst.operando1;
            }
            cpu->pc++;
            return EXEC_BLOQUEIO;

        case 'T':
            return EXEC_TERMINO;

        case 'F': {
            if (indice_atual < 0 || indice_atual >= MAX_PROCESSOS) {
                cpu->pc++;
                return EXEC_OK;
            }

            ProcessoSimulado *pai = &tabela->processos[indice_atual];
            int filho_idx = criar_processo(tabela, pai->pid, pai->prioridade, (tempo != NULL) ? *tempo : 0);
            if (filho_idx >= 0) {
                ProcessoSimulado *filho = &tabela->processos[filho_idx];
                /* copia do programa */
                filho->tamanho_programa = cpu->tamanho_programa;
                filho->programa = NULL;
                if (cpu->tamanho_programa > 0 && cpu->programa != NULL) {
                    filho->programa = malloc(cpu->tamanho_programa * sizeof(Instrucao));
                    if (filho->programa != NULL) {
                        memcpy(filho->programa, cpu->programa, cpu->tamanho_programa * sizeof(Instrucao));
                    }
                }

                /* copia da memória/variáveis */
                if (cpu->num_variaveis > 0 && cpu->variaveis != NULL) {
                    filho->num_variaveis = cpu->num_variaveis;
                    filho->variaveis = malloc(cpu->num_variaveis * sizeof(int));
                    if (filho->variaveis != NULL) {
                        memcpy(filho->variaveis, cpu->variaveis, cpu->num_variaveis * sizeof(int));
                    }
                } else {
                    filho->num_variaveis = 0;
                    filho->variaveis = NULL;
                }

                filho->pc = cpu->pc + 1; /* filho continua na próxima instrução */
                filho->estado = PRONTO;
                filho->tempo_cpu = 0; /* inicializa tempo de CPU do filho */
                enfileirar_pronto(pronto, filho_idx, filho->prioridade);
            }

            /* pai pula n instruções após o F */
            cpu->pc = cpu->pc + 1 + inst.operando1;
            return EXEC_OK;
        }

        case 'R': {
            if (inst.nome_arquivo[0] == '\0') {
                cpu->pc++;
                return EXEC_OK;
            }
            int novo_tam = 0;
            Instrucao *novo_programa = parser_carregar_programa(inst.nome_arquivo, &novo_tam);
            if (novo_programa == NULL) {
                fprintf(stderr, "[ERRO] Não foi possível carregar o arquivo de replace '%s'\n", inst.nome_arquivo);
                return EXEC_TERMINO;
            }
            /* substitui o programa atual */
            if (cpu->programa != NULL) {
                free(cpu->programa);
            }
            cpu->programa = novo_programa;
            cpu->tamanho_programa = novo_tam;
            cpu->pc = 0;

            /* recria memória/variáveis conforme novo programa (inicialmente vazias) */
            if (cpu->variaveis != NULL) {
                free(cpu->variaveis);
            }
            cpu->variaveis = NULL;
            cpu->num_variaveis = 0;

            if (indice_atual >= 0 && indice_atual < MAX_PROCESSOS) {
                tabela->processos[indice_atual].programa = cpu->programa;
                tabela->processos[indice_atual].tamanho_programa = cpu->tamanho_programa;
                tabela->processos[indice_atual].pc = cpu->pc;
                tabela->processos[indice_atual].variaveis = cpu->variaveis;
                tabela->processos[indice_atual].num_variaveis = cpu->num_variaveis;
            }
            return EXEC_OK;
        }

        default:
            cpu->pc++;
            return EXEC_OK;
    }
}