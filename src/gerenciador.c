/**
 * ============================================================================
 * gerenciador.c — Processo Gerenciador de Processos
 * ============================================================================
 *
 * Núcleo do simulador. Gerencia processos simulados, executa instruções,
 * escalona e realiza troca de contexto.
 *
 * Autor: Grupo
 * Disciplina: Sistemas Operacionais — UFV Florestal
 *
 * TODO: Implementar a lógica completa de processamento de comandos.
 */

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

/**
 * Loop principal do processo gerenciador.
 *
 * 1. Inicializa as 6 estruturas de dados
 * 2. Cria o primeiro processo simulado (PID 0, programa "init")
 * 3. Loop: lê comando do pipe e processa
 *    - U: executa instrução, incrementa tempo, escalona
 *    - I: dispara processo de impressão
 *    - M: imprime e finaliza
 */
void processo_gerenciador(int pipefd[], int opcao_inicial, const char *nome_arquivo_inicial) {
    char comando;
    char nome_arquivo[TAMANHO_BUFFER];
    
    strncpy(nome_arquivo, nome_arquivo_inicial, TAMANHO_BUFFER - 1);
    nome_arquivo[TAMANHO_BUFFER - 1] = '\0';
    /* Fecha a ponta de escrita — gerenciador só lê do pipe */
    close(pipefd[1]);

    printf("Processo gerenciador criado.\n");

    /* ---- Inicialização das 6 estruturas de dados ---- */

    int tempo = 0;                /* Estrutura 1: Tempo */

    Cpu cpu;                      /* Estrutura 2: CPU */
    inicializar_cpu(&cpu);

    TabelaDeProcessos tabela;     /* Estrutura 3: TabelaDeProcessos */
    inicializar_tabela(&tabela);

    EstadoPronto pronto;          /* Estrutura 4: EstadoPronto */
    inicializar_estado_pronto(&pronto);

    EstadoBloqueado bloqueado;    /* Estrutura 5: EstadoBloqueado */
    inicializar_estado_bloqueado(&bloqueado);

    EstadoExecucao execucao;      /* Estrutura 6: EstadoExecucao */
    inicializar_estado_execucao(&execucao);

    printf("Estruturas de dados inicializadas.\n");

    int num_instrucoes = 0;
    Instrucao *programa = parser_carregar_programa(nome_arquivo, &num_instrucoes);
    if (programa == NULL) {
        fprintf(stderr, "[ERRO] Falha ao carregar programa '%s'\n", nome_arquivo);
        close(pipefd[0]);
        exit(1);
    }

    int idx = criar_processo(&tabela, -1, 0, tempo);
    tabela.processos[idx].programa = programa;
    tabela.processos[idx].tamanho_programa = num_instrucoes;
    execucao.indice = idx;
    tabela.processos[idx].estado = EXECUCAO;
    /* TODO: trocar_contexto(&cpu, &tabela, -1, idx); */

    printf("Gerenciador pronto. Programa '%s' carregado.\n\n", nome_arquivo);

    /* ---- Criar o primeiro processo simulado (init, PID 0) ---- */

    /*
     * TODO: Usar parser_carregar_programa("programs/init", &num_instrucoes)
     *       para carregar o programa, criar o processo na tabela,
     *       e colocá-lo em execução.
     *
     * int idx = criar_processo(&tabela, -1, 0, tempo);
     * tabela.processos[idx].programa = parser_carregar_programa("programs/init", &n);
     * tabela.processos[idx].tamanho_programa = n;
     * execucao.indice = idx;
     * tabela.processos[idx].estado = EXECUCAO;
     * trocar_contexto(&cpu, &tabela, -1, idx);
     */

    printf("Gerenciador pronto para receber comandos.\n\n");

    /* ---- Loop principal: processar comandos do pipe ---- */

    while (read(pipefd[0], &comando, sizeof(char)) > 0) {
        switch (comando) {
            case 'U':
                /*
                 * TODO: Implementar processamento do comando U
                 *
                 * 1. Executar próxima instrução: executar_instrucao(...)
                 * 2. Incrementar PC (exceto para F e R)
                 * 3. Incrementar tempo
                 * 4. Atualizar bloqueados: atualizar_bloqueados(...)
                 * 5. Escalonar: escalonar(...)
                 */
                tempo++;
                printf("[t=%d] Comando U recebido.\n", tempo);
                break;

            case 'I':
                /*
                 * TODO: Implementar processamento do comando I
                 *
                 * criar_processo_impressao(&tabela, &pronto, &bloqueado,
                 *                          &execucao, &cpu, tempo);
                 */
                printf("[t=%d] Comando I recebido — imprimir estado.\n", tempo);
                break;

            case 'M':
                /*
                 * TODO: Implementar processamento do comando M
                 *
                 * 1. Disparar processo de impressão
                 * 2. Aguardar finalização
                 * 3. Calcular e imprimir tempo médio de resposta
                 */
                printf("[t=%d] Comando M recebido — finalizando.\n", tempo);
                goto fim_loop;

            default:
                printf("[t=%d] Comando desconhecido: '%c'\n", tempo, comando);
                break;
        }
    }

fim_loop:
    close(pipefd[0]);
    printf("Processo gerenciador finalizado.\n");
    exit(0);
}
