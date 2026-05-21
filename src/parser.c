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

/**
 * Carrega um programa simulado a partir de um arquivo.
 */
Instrucao* parser_carregar_programa(const char *nome_arquivo, int *num_instrucoes) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "[ERRO] Não foi possível abrir o arquivo de programa '%s'\n", nome_arquivo);
        *num_instrucoes = 0;
        return NULL;
    }

    int capacidade = 10; // Capacidade inicial do vetor dinâmico
    int qtd = 0;
    Instrucao *programa = malloc(capacidade * sizeof(Instrucao));
    
    if (programa == NULL) {
        fprintf(stderr, "[ERRO] Falha de alocação de memória no parser.\n");
        fclose(arquivo);
        return NULL;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        // Remove os caracteres de nova linha (\n e \r)
        linha[strcspn(linha, "\r\n")] = 0;

        // Ignora linhas totalmente vazias
        if (strlen(linha) == 0) {
            continue;
        }

        // Se o vetor estiver cheio, dobra o tamanho dele dinamicamente
        if (qtd >= capacidade) {
            capacidade *= 2;
            Instrucao *novo_programa = realloc(programa, capacidade * sizeof(Instrucao));
            if (novo_programa == NULL) {
                fprintf(stderr, "[ERRO] Falha ao realocar memória no parser.\n");
                free(programa);
                fclose(arquivo);
                return NULL;
            }
            programa = novo_programa;
        }

        Instrucao *inst = &programa[qtd];
        // Limpa a estrutura para evitar lixo de memória nos campos
        memset(inst, 0, sizeof(Instrucao));

        // Captura o primeiro caractere identificador do tipo de instrução
        if (sscanf(linha, " %c", &inst->tipo) != 1) {
            continue; 
        }

        // Mapeia e processa os operandos com base no tipo da instrução
        switch (inst->tipo) {
            case 'N':
                // Instrução com 1 operando inteiro (ex: N 2)
                sscanf(linha, " %*c %d", &inst->operando1);
                break;
            case 'D':
                // Instrução com 1 operando inteiro (ex: D 0)
                sscanf(linha, " %*c %d", &inst->operando1);
                break;
            case 'B':
                // Instrução com 1 operando inteiro (ex: B 5)
                sscanf(linha, " %*c %d", &inst->operando1);
                break;
            case 'F':
                // Instruções com 1 operando inteiro (ex: N 2, B 5)
                sscanf(linha, " %*c %d", &inst->operando1);
                break;
                
            case 'V':
                // Instrução com 2 operandos inteiros (ex: V 0 1000)
                sscanf(linha, " %*c %d %d", &inst->operando1, &inst->operando2);
                break;
            case 'A':
                // Instrução com 2 operandos inteiros (ex: A 0 500)
                sscanf(linha, " %*c %d %d", &inst->operando1, &inst->operando2);
                break;
            case 'S':
                // Instruções com 2 operandos inteiros (ex: V 0 1000)
                sscanf(linha, " %*c %d %d", &inst->operando1, &inst->operando2);
                break;
                
            case 'R':
                // Instrução com uma string/nome de arquivo (ex: R file_a)
                sscanf(linha, " %*c %255s", inst->nome_arquivo);
                break;
                
            case 'T':
                // Instrução de término (não possui operandos adicionais)
                break;
                
            default:
                fprintf(stderr, "[AVISO] Tipo de instrução inválido ou desconhecido: '%c'\n", inst->tipo);
                continue; // Pula essa linha e vai para a próxima
        }

        qtd++;
    }

    fclose(arquivo);
    *num_instrucoes = qtd; // Retorna a quantidade exata de instruções lidas
    return programa;
}

/**
 * Libera a memória de um vetor de instruções.
 */
void parser_liberar_programa(Instrucao *programa) {
    if (programa != NULL) {
        free(programa);
    }
}