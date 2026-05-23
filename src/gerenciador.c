#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gerenciador.h"
#include "estruturas.h"
#include "parser.h"
#include "executor.h"
#include "escalonador.h"
#include "impressao.h"

#define TAMANHO_BUFFER 256

void processo_gerenciador(int pipefd[], const char *arquivo_init,
                          PoliticaEscalonamento politica) {
    char comando;
    char nome_arquivo[TAMANHO_BUFFER];

    strncpy(nome_arquivo, arquivo_init, TAMANHO_BUFFER - 1);
    nome_arquivo[TAMANHO_BUFFER - 1] = '\0';

    Cpu cpu;
    TabelaDeProcessos tabela;
    EstadoPronto pronto;
    EstadoBloqueado bloqueado;
    EstadoExecucao execucao;
    int tempo = 0;

    inicializar_cpu(&cpu);
    inicializar_tabela(&tabela);
    inicializar_estado_pronto(&pronto);
    inicializar_estado_bloqueado(&bloqueado);
    inicializar_estado_execucao(&execucao);

    int num_instrucoes = 0;
    Instrucao *programa = parser_carregar_programa(nome_arquivo, &num_instrucoes);
    if (programa == NULL) {
        fprintf(stderr, "[ERRO] Falha ao carregar o programa inicial.\n");
        exit(1);
    }
    
    int idx = criar_processo(&tabela, -1, 0, tempo);
    if (idx < 0) {
        fprintf(stderr, "[ERRO] Não foi possível criar o processo inicial.\n");
        exit(1);
    }

    tabela.processos[idx].programa = programa;
    tabela.processos[idx].tamanho_programa = num_instrucoes;
    tabela.processos[idx].pc = 0;
    tabela.processos[idx].estado = EXECUCAO;
    tabela.processos[idx].prioridade = 0;
    tabela.processos[idx].tempo_cpu = 0;
    tabela.processos[idx].variaveis = NULL;
    tabela.processos[idx].num_variaveis = 0;

    cpu.programa = programa;
    cpu.tamanho_programa = num_instrucoes;
    cpu.pc = 0;
    cpu.num_variaveis = 0;
    cpu.variaveis = NULL;
    cpu.quantum = obter_quantum(0);
    cpu.tempo_usado_quantum = 0;
    execucao.indice = idx;

    printf("Gerenciador pronto. Programa '%s' carregado.\n", nome_arquivo);
    printf("Politica de escalonamento: %s\n\n",
           politica == POLITICA_FIFO ? "FIFO" : "MLFQ");

    while (read(pipefd[0], &comando, sizeof(char)) > 0) {
        switch (comando) {
            case 'U': {
                tempo++;
                printf("\n[t=%d] Comando U recebido.\n", tempo);
                fflush(stdout);

                if (execucao.indice == -1) {
                    printf("[t=%d] CPU ociosa.\n", tempo);
                    fflush(stdout);
                } else {
                    int idx_atual = execucao.indice;
                    if (cpu.programa == NULL) {
                        printf("[ERRO] cpu.programa NULL!\n");
                    } else {
                        printf("[t=%d] Executando PID %d (PC=%d, prio=%d)...\n",
                               tempo,
                               tabela.processos[idx_atual].pid,
                               cpu.pc,
                               tabela.processos[idx_atual].prioridade);
                        fflush(stdout);

                        ResultadoExecucao res = executar_instrucao(&cpu, &tabela,
                                                                   &pronto, &bloqueado,
                                                                   &execucao, &tempo);
                        ProcessoSimulado *proc = &tabela.processos[idx_atual];

                        if (res == EXEC_OK) {
                            proc->tempo_cpu++;
                            proc->pc = cpu.pc;
                            proc->variaveis = cpu.variaveis;
                            proc->num_variaveis = cpu.num_variaveis;

                            cpu.tempo_usado_quantum++;
                            if (cpu.tempo_usado_quantum >= cpu.quantum) {
                                if (politica == POLITICA_MLFQ &&
                                    proc->prioridade < NUM_PRIORIDADES - 1) {
                                    proc->prioridade++;
                                }
                                proc->estado = PRONTO;
                                enfileirar_pronto(&pronto, idx_atual,
                                                  politica == POLITICA_FIFO ? 0 : proc->prioridade);
                                execucao.indice = -1;
                            }
                        } else if (res == EXEC_BLOQUEIO) {
                            proc->pc = cpu.pc;
                            proc->variaveis = cpu.variaveis;
                            proc->num_variaveis = cpu.num_variaveis;
                            proc->estado = BLOQUEADO;
                            enfileirar(&bloqueado, idx_atual);
                            if (politica == POLITICA_MLFQ && proc->prioridade > 0) {
                                proc->prioridade--;
                            }
                            execucao.indice = -1;
                        } else if (res == EXEC_TERMINO) {
                            proc->pc = cpu.pc;
                            proc->variaveis = cpu.variaveis;
                            proc->num_variaveis = cpu.num_variaveis;
                            liberar_processo(&tabela, idx_atual);
                            execucao.indice = -1;
                        }
                    }
                }

                atualizar_bloqueados(&tabela, &pronto, &bloqueado);
                escalonar(&cpu, &tabela, &pronto, &bloqueado, &execucao,
                          politica);
                break;
            }
            case 'I':
                printf("\n[t=%d] Imprimindo estado do sistema...\n", tempo);
                criar_processo_impressao(&tabela, &pronto, &bloqueado, &execucao, &cpu, tempo);
                break;

            case 'M':
                printf("\n[t=%d] Encerrando simulador e exibindo estatísticas finais...\n", tempo);
                criar_processo_impressao(&tabela, &pronto, &bloqueado, &execucao, &cpu, tempo);
                goto fim_loop;
        }
    }

fim_loop:
    close(pipefd[0]);
    exit(0);
}