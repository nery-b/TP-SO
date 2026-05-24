#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "controle.h"
#include "gerenciador.h"
#include "escalonador.h"

int main(void) {
    int pipefd[2];
    pid_t pid;

    char arquivo_init[256] = "teste.txt";
    char primeiro_token[256] = "";
    int origem_comandos = 1;
    char arquivo_comandos[256] = "";
    int modo_impressao = 2;

    printf("=== Simulador de Gerenciamento de Processos ===\n\n");

    if (isatty(STDIN_FILENO)) {
        printf("Politica de escalonamento:\n");
        printf("  1 — FIFO\n");
        printf("  2 — MLFQ\n");
        printf("Escolha: ");
        while (scanf("%d", &escolha_politica) != 1 ||
               (escolha_politica != 1 && escolha_politica != 2)) {
            printf("Escolha invalida. Digite 1 para FIFO ou 2 para MLFQ: ");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
        }
        politica = (escolha_politica == 1) ? POLITICA_FIFO : POLITICA_MLFQ;

        printf("Digite o nome do arquivo do programa inicial (ex: teste.txt): ");
        scanf("%255s", arquivo_init);
    } else if (scanf("%255s", primeiro_token) == 1) {
        if (strcmp(primeiro_token, "1") == 0 || strcmp(primeiro_token, "2") == 0) {
            politica = (strcmp(primeiro_token, "1") == 0) ? POLITICA_FIFO : POLITICA_MLFQ;
            if (scanf("%255s", arquivo_init) != 1) {
                strcpy(arquivo_init, "teste.txt");
            }
        } else {
            strncpy(arquivo_init, primeiro_token, sizeof(arquivo_init) - 1);
            arquivo_init[sizeof(arquivo_init) - 1] = '\0';
        }
    }

    if (isatty(STDIN_FILENO)) {
        printf("\nOrigem dos comandos de controle (U, I, M):\n");
        printf("  1 — Digitar no terminal (interativo)\n");
        printf("  2 — Ler de um arquivo de comandos\n");
        printf("Escolha: ");
    }

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
        processo_controle(pipefd, origem_comandos, arquivo_comandos);
        wait(NULL);
    }

    return EXIT_SUCCESS;
}