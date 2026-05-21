/**
 * ============================================================================
 * parser.c — Parser de Programas Simulados
 * ============================================================================
 *
 * Lê arquivos de programa simulado e converte em vetor de Instrucao.
 *
 * Autor: Grupo (Pessoa 2)
 * Disciplina: Sistemas Operacionais — UFV Florestal
 *
 * TODO: Implementar a lógica de parsing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/**
 * Carrega um programa simulado a partir de um arquivo.
 *
 * TODO: Implementar a leitura do arquivo e conversão de cada linha
 *       em uma struct Instrucao.
 *
 * Formato esperado:
 *   "N 2"         → tipo='N', operando1=2
 *   "D 0"         → tipo='D', operando1=0
 *   "V 0 1000"    → tipo='V', operando1=0, operando2=1000
 *   "A 0 19"      → tipo='A', operando1=0, operando2=19
 *   "S 1 53"      → tipo='S', operando1=1, operando2=53
 *   "B 5"         → tipo='B', operando1=5
 *   "T"           → tipo='T'
 *   "F 1"         → tipo='F', operando1=1
 *   "R file_a"    → tipo='R', nome_arquivo="file_a"
 */
Instrucao* parser_carregar_programa(const char *nome_arquivo, int *num_instrucoes) {
    /* Stub: a ser implementado */
    (void)nome_arquivo;
    (void)num_instrucoes;

    fprintf(stderr, "[STUB] parser_carregar_programa: não implementado ainda\n");
    return NULL;
}

/**
 * Libera a memória de um vetor de instruções.
 */
void parser_liberar_programa(Instrucao *programa) {
    if (programa != NULL) {
        free(programa);
    }
}
