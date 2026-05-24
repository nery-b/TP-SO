/**
 * ============================================================================
 * gerenciador.h — Processo Gerenciador de Processos
 * ============================================================================
 *
 * O gerenciador de processos é o núcleo do simulador. Ele:
 *   - Cria o primeiro processo simulado (PID 0, programa "init")
 *   - Inicializa todas as 6 estruturas de dados
 *   - Recebe comandos do processo controle via pipe
 *   - Comando U: executa instrução, incrementa tempo, escalona
 *   - Comando I: dispara processo de impressão
 *   - Comando M: imprime e finaliza
 */

#ifndef GERENCIADOR_H
#define GERENCIADOR_H

#include "escalonador.h"

/**
 * Executa o loop principal do processo gerenciador.
 *
 * Inicializa as estruturas de dados, cria o primeiro processo simulado
 * (lendo o programa do arquivo "init") e entra no loop de processamento
 * de comandos recebidos pelo pipe.
 *
 * @param pipefd Array com os descritores do pipe [leitura, escrita]
 * @param arquivo_init Nome do arquivo do programa inicial
 * @param politica Política de escalonamento selecionada
 */


void processo_gerenciador(int pipefd[], const char *arquivo_init, int modo_impressao, int modo_escalonamento);



#endif /* GERENCIADOR_H */
