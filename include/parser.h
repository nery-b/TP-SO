/**
 * ============================================================================
 * parser.h — Parser de Programas Simulados
 * ============================================================================
 *
 * Lê arquivos de programa simulado (instruções N, D, V, A, S, B, T, F, R)
 * e converte em um vetor de structs Instrucao para ser armazenado na
 * TabelaDeProcessos ou na Cpu.
 *
 * Formato do arquivo:
 *   Cada linha contém uma instrução com tipo e operandos separados por espaço.
 *   Exemplo: "V 0 1000" → tipo='V', operando1=0, operando2=1000
 */

#ifndef PARSER_H
#define PARSER_H

#include "estruturas.h"

/**
 * Carrega um programa simulado a partir de um arquivo.
 *
 * Lê o arquivo linha por linha, converte cada instrução em uma struct
 * Instrucao e armazena num vetor alocado dinamicamente.
 *
 * @param nome_arquivo  Caminho do arquivo de programa
 * @param num_instrucoes Ponteiro para armazenar o número de instruções lidas
 * @return Vetor de Instrucao alocado com malloc (caller deve liberar),
 *         ou NULL em caso de erro
 */
Instrucao* parser_carregar_programa(const char *nome_arquivo, int *num_instrucoes);

/**
 * Libera a memória de um vetor de instruções carregado pelo parser.
 *
 * @param programa Ponteiro para o vetor de instruções
 */
void parser_liberar_programa(Instrucao *programa);

#endif /* PARSER_H */
