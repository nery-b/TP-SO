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

void processo_gerenciador(int pipefd[], const char *arquivo_init) {
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
    bloqueado.inicio = 0; bloqueado.fim = 0; bloqueado.tamanho = 0;
    execucao.indice = -1;

    int num_instrucoes = 0;
    Instrucao *programa = parser_carregar_programa(nome_arquivo, &num_instrucoes);
    if (programa == NULL) {
        fprintf(stderr, "[ERRO] Falha ao carregar o programa inicial.\n");
        exit(1);
    }
    
    int idx = criar_processo(&tabela, -1, 0, tempo);
    tabela.processos[idx].programa = programa;
    tabela.processos[idx].tamanho_programa = num_instrucoes;
    execucao.indice = idx;
    tabela.processos[idx].estado = EXECUCAO;

    /* Carga manual na CPU (Proteção) */
    cpu.pc = 0;
    cpu.programa = programa;
    cpu.tamanho_programa = num_instrucoes;
    cpu.num_variaveis = 0;
    cpu.variaveis = NULL;
    cpu.quantum = obter_quantum(0);
    cpu.tempo_usado_quantum = 0;

    printf("Gerenciador pronto. Programa '%s' carregado.\n\n", nome_arquivo);

    while (read(pipefd[0], &comando, sizeof(char)) > 0) {
        switch (comando) {
            case 'U':
                tempo++;
                printf("\n[t=%d] Comando U recebido.\n", tempo);
                fflush(stdout);

                if (execucao.indice == -1) {
                    printf("[t=%d] CPU Ociosa.\n", tempo);
                    fflush(stdout);
                } else {
                    int idx_atual = execucao.indice;
                    if (cpu.programa == NULL) {
                        printf("[ERRO] cpu.programa NULL!\n");
                    } else {
                        printf("[t=%d] Executando PID %d (PC=%d)...\n", tempo, tabela.processos[idx_atual].pid, cpu.pc);
                        
                        ResultadoExecucao res = executar_instrucao(&cpu, &tabela, &pronto, &bloqueado, &execucao, &tempo);
                        
                        switch (res) {
                            case EXEC_OK:
                                cpu.pc++;
                                break;
                            case EXEC_BLOQUEIO:
                                cpu.pc++;
                                tabela.processos[idx_atual].estado = BLOQUEADO;
                                execucao.indice = -1; 
                                break;
                            case EXEC_TERMINO:
                                liberar_processo(&tabela, idx_atual);
                                execucao.indice = -1; 
                                break;
                            default:
                                cpu.pc++; 
                                break;
                        }
                    }
                }

                /* Escalonador Temporário */
                if (execucao.indice == -1) {
                    for (int i = 0; i < MAX_PROCESSOS; i++) {
                        if (tabela.processos[i].ativo && tabela.processos[i].estado == PRONTO) {
                            execucao.indice = i;
                            tabela.processos[i].estado = EXECUCAO;
                            cpu.pc = tabela.processos[i].pc;
                            cpu.programa = tabela.processos[i].programa;
                            cpu.tamanho_programa = tabela.processos[i].tamanho_programa;
                            cpu.num_variaveis = tabela.processos[i].num_variaveis;
                            cpu.variaveis = tabela.processos[i].variaveis;
                            printf("[TESTE] Processo PID %d escalonado.\n", tabela.processos[i].pid);
                            break;
                        }
                    }
                }
                break;

            case 'I':
                printf("\n[t=%d] Imprimindo estado do sistema...\n", tempo);
                criar_processo_impressao(&tabela, &pronto, &bloqueado, &execucao, &cpu, tempo);
                break;

            case 'M':
                goto fim_loop;
        }
    }

fim_loop:
    close(pipefd[0]);
    exit(0);
}