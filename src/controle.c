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

#include "controle.h"

void processo_controle(int pipefd[], int origem, const char *nome_arquivo) {
    char comando;
    FILE *entrada = stdin; 

    close(pipefd[0]); 

    printf("Processo controle iniciado.\n");

    if (origem == 2) {
        entrada = fopen(nome_arquivo, "r");
        if (entrada == NULL) {
            perror("[ERRO] Nao abriu arquivo de comandos. Usando terminal.");
            entrada = stdin;
            origem = 1;
        } else {
            printf("Lendo comandos do arquivo '%s'...\n", nome_arquivo);
        }
    } else {
        printf("Comandos disponíveis: U, I, M\n\n");
    }

    while (1) {
        if (origem == 1) printf("Comando: ");

        if (fscanf(entrada, " %c", &comando) != 1) {
            if (origem == 2) {
                printf("\n[Controle] Fim do arquivo. Enviando 'M'.\n");
                comando = 'M';
            } else {
                break;
            }
        }

        if (comando >= 'a' && comando <= 'z') comando -= 32;

        if (comando != 'U' && comando != 'I' && comando != 'M') {
            if (origem == 1) printf("Comando inválido.\n");
            continue;
        }

        write(pipefd[1], &comando, sizeof(char));
        if (origem == 2) {
            printf("Controle enviou comando '%c' do arquivo.\n", comando);
            sleep(1); 
        }

        if (comando == 'M') break;
    }

    if (origem == 2 && entrada != stdin) fclose(entrada);
}