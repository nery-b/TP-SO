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

typedef enum {
    EXEC_OK,  
    EXEC_BLOQUEIO,  
    EXEC_TERMINO,   
    EXEC_FORK,      
    EXEC_REPLACE    
} ResultadoExecucao;

ResultadoExecucao executar_instrucao(Cpu *cpu, TabelaDeProcessos *tabela,
                                     EstadoPronto *pronto,
                                     EstadoBloqueado *bloqueado,
                                     EstadoExecucao *execucao, int *tempo);

#endif