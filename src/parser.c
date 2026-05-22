/**
 * ============================================================================
 * parser.c — Parser de Programas Simulados
 * ============================================================================
 *
 * Lê arquivos de programa simulado e converte em vetor de Instrucao.
 *
 * Autor: Grupo (Pessoa 2)
 * Disciplina: Sistemas Operacionais — UFV Florestal
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Instrucao* parser_carregar_programa(const char *nome_arquivo, int *num_instrucoes) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        *num_instrucoes = 0;
        return NULL;
    }

    int capacidade = 10;
    int qtd = 0;
    Instrucao *programa = malloc(capacidade * sizeof(Instrucao));

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\r\n")] = 0;
        if (strlen(linha) == 0) continue;

        if (qtd >= capacidade) {
            capacidade *= 2;
            programa = realloc(programa, capacidade * sizeof(Instrucao));
        }

        Instrucao *inst = &programa[qtd];
        memset(inst, 0, sizeof(Instrucao));

        if (sscanf(linha, " %c", &inst->tipo) != 1) continue; 

        switch (inst->tipo) {
            case 'N': case 'D': case 'B': case 'F':
                sscanf(linha, " %*c %d", &inst->operando1);
                break;
            case 'V': case 'A': case 'S':
                sscanf(linha, " %*c %d %d", &inst->operando1, &inst->operando2);
                break;
            case 'R':
                sscanf(linha, " %*c %255s", inst->nome_arquivo);
                break;
            case 'T':
                break;
        }
        qtd++;
    }
    fclose(arquivo);
    *num_instrucoes = qtd;
    return programa;
}

void parser_liberar_programa(Instrucao *programa) {
    if (programa != NULL) free(programa);
}