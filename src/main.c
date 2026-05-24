#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "controle.h"
#include "gerenciador.h"

int main(void) {
    int pipefd[2];
    pid_t pid;

    char arquivo_init[256] = "teste.txt";
    int origem_comandos = 1;
    char arquivo_comandos[256] = "";
    int modo_impressao = 2;

    printf("=== Simulador de Gerenciamento de Processos ===\n\n");

    /* 1. Coleta o arquivo do programa inicial */
    printf("Digite o nome do arquivo do programa inicial (ex: teste.txt): ");
    scanf("%255s", arquivo_init);

    /* 2. Coleta a forma de envio dos comandos U, I, M */
    printf("\nOrigem dos comandos de controle (U, I, M):\n");
    printf("  1 — Digitar no terminal (interativo)\n");
    printf("  2 — Ler de um arquivo de comandos\n");
    printf("Escolha: ");
    if (scanf("%d", &origem_comandos) != 1) origem_comandos = 1;

    if (origem_comandos == 2) {
        printf("Digite o nome do arquivo de comandos: ");
        scanf("%255s", arquivo_comandos);
    }
    
    /* 3. Coleta o modo de impressão */
    printf("\nModo de execução da impressão do estado:\n");
    printf("  1 — Processo filho (fork)\n");
    printf("  2 — Thread (pthread)\n");
    printf("Escolha: ");
    if (scanf("%d", &modo_impressao) != 1) modo_impressao = 2;

    /* 4. Coleta o modo de escalonamento */
    int modo_escalonamento = 2;
    printf("\nModo de Escalonamento:\n");
    printf("  1 — FIFO (First-In, First-Out sem preempção)\n");
    printf("  2 — MLFQ (Modo do Professor)\n");
    printf("Escolha: ");
    if (scanf("%d", &modo_escalonamento) != 1) modo_escalonamento = 2;

    printf("\n");

    if (pipe(pipefd) == -1) {
        perror("[ERRO] Falha ao criar pipe");
        return EXIT_FAILURE;
    }

    pid = fork();
    if (pid < 0) {
        perror("[ERRO] Falha no fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* Processo filho → gerenciador */
        processo_gerenciador(pipefd, arquivo_init, modo_impressao, modo_escalonamento);
    } else {
        /* Processo pai → controle */
        processo_controle(pipefd, origem_comandos, arquivo_comandos);
        wait(NULL); 
    }

    return EXIT_SUCCESS;
}