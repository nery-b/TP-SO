/**
 * ============================================================================
 * controle.c — Processo Controle
 * ============================================================================
 *
 * Lê comandos (U, I, M) da entrada padrão e os envia para o processo
 * gerenciador através do pipe.
 *
 * Autor: Grupo
 * Disciplina: Sistemas Operacionais — UFV Florestal
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "controle.h"

/**
 * Loop principal do processo controle.
 *
 * Fecha a ponta de leitura do pipe (só escreve), lê comandos do usuário
 * e os envia para o gerenciador. Encerra quando o comando 'M' é enviado.
 */
void processo_controle(int pipefd[]) {
    char comando;

    /* Fecha a ponta de leitura — controle só escreve no pipe */
    close(pipefd[0]);

    printf("Processo controle iniciado.\n");
    printf("Comandos disponíveis:\n");
    printf("  U — Fim de uma unidade de tempo\n");
    printf("  I — Imprimir estado atual do sistema\n");
    printf("  M — Imprimir tempo médio e finalizar\n\n");

    while (1) {
        printf("Comando: ");
        scanf(" %c", &comando);

        /* Valida o comando */
        if (comando != 'U' && comando != 'u' &&
            comando != 'I' && comando != 'i' &&
            comando != 'M' && comando != 'm') {
            printf("Comando inválido. Digite apenas U, I ou M.\n");
            continue;
        }

        /* Converte para maiúsculo */
        if (comando >= 'a' && comando <= 'z') {
            comando = comando - 'a' + 'A';
        }

        /* Envia o comando para o gerenciador via pipe */
        write(pipefd[1], &comando, sizeof(char));
        printf("Controle enviou comando '%c' para o gerenciador.\n", comando);

        if (comando == 'M') {
            break;
        }
    }

    /* Fecha a ponta de escrita e aguarda o gerenciador finalizar */
    close(pipefd[1]);
    wait(NULL);

    printf("Processo controle finalizado.\n");
}
