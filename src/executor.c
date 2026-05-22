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
                                         
    if (cpu == NULL || cpu->programa == NULL || cpu->pc >= cpu->tamanho_programa) {
        return EXEC_TERMINO; 
    }

    Instrucao inst = cpu->programa[cpu->pc];

    switch (inst.tipo) {
        case 'N':
            if (cpu->variaveis != NULL) free(cpu->variaveis);
            cpu->variaveis = calloc(inst.operando1, sizeof(int));
            cpu->num_variaveis = inst.operando1;
            return EXEC_OK;

        case 'D':
            if (inst.operando1 < cpu->num_variaveis) cpu->variaveis[inst.operando1] = 0;
            return EXEC_OK;

        case 'V':
            if (inst.operando1 < cpu->num_variaveis) cpu->variaveis[inst.operando1] = inst.operando2;
            return EXEC_OK;

        case 'A':
            if (inst.operando1 < cpu->num_variaveis) cpu->variaveis[inst.operando1] += inst.operando2;
            return EXEC_OK;

        case 'S':
            if (inst.operando1 < cpu->num_variaveis) cpu->variaveis[inst.operando1] -= inst.operando2;
            return EXEC_OK;

        case 'B':
            if (execucao != NULL && execucao->indice != -1) {
                int indice_atual = execucao->indice; 
                tabela->processos[indice_atual].tempo_bloqueio = inst.operando1;
            }
            return EXEC_BLOQUEIO;

        case 'T':
            return EXEC_TERMINO;

        case 'F':
            return EXEC_FORK;

        case 'R':
            return EXEC_REPLACE;

        default:
            return EXEC_OK;
    }
}