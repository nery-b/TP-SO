/**
 * ============================================================================
 * main.c — Ponto de entrada do simulador de gerenciamento de processos
 * ============================================================================
 *
 * Cria o pipe de comunicação e faz fork para criar dois processos Linux:
 *   - Processo filho: gerenciador de processos (núcleo do simulador)
 *   - Processo pai:   controle (lê comandos U/I/M e envia pelo pipe)
 *
 * Autor: Grupo
 * Disciplina: Sistemas Operacionais — UFV Florestal
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define TAMANHO_BUFFER 256

#include "controle.h"
#include "gerenciador.h"

int main(void) {
    int pipefd[2];
    pid_t pid;
    int opcao;
    char nome_arquivo[TAMANHO_BUFFER];

    /* Escolha: qual arquivo usar como programa init */
    printf("Fonte do programa inicial (init):\n");
    printf("  1 — Informar nome do arquivo\n");
    printf("  2 — Usar arquivo padrão 'init'\n");
    printf("Opção: ");
    scanf("%d", &opcao);

    if (opcao == 1) {
        printf("Nome do arquivo: ");
        scanf("%s", nome_arquivo);
    } else {
        strcpy(nome_arquivo, "init"); /* padrão da especificação */
    }


    /* Cria o pipe para comunicação controle → gerenciador */
    if (pipe(pipefd) == -1) {
        perror("[ERRO] Falha ao criar pipe");
        return EXIT_FAILURE;
    }

    printf("=== Simulador de Gerenciamento de Processos ===\n");
    printf("Pipe criado com sucesso.\n\n");

    /* Fork: cria o processo gerenciador (filho) */
    pid = fork();

    if (pid < 0) {
        perror("[ERRO] Falha ao criar processo gerenciador (fork)");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* Processo filho → gerenciador de processos */
        processo_gerenciador(pipefd, opcao, nome_arquivo);
    } else {
        /* Processo pai → controle */
        processo_controle(pipefd);
    }


    return EXIT_SUCCESS;
}
