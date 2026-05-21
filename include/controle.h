#ifndef CONTROLE_H
#define CONTROLE_H

/**
 * Executa o loop do processo controle.
 *
 * Lê comandos repetidamente da entrada padrão (ou arquivo) e os envia
 * para o gerenciador de processos através do pipe. O loop termina quando
 * o comando 'M' é enviado.
 *
 * @param pipefd Array com os descritores do pipe [leitura, escrita]
 */
void processo_controle(int pipefd[]);

#endif /* CONTROLE_H */
