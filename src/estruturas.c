/**
 * ============================================================================
 * estruturas.c — Implementação das estruturas de dados centrais do simulador
 * ============================================================================
 *
 * Funções de inicialização e manipulação para as 6 estruturas de dados
 * do gerenciador de processos simulado.
 *
 * Autor: Pessoa 3
 * Disciplina: Sistemas Operacionais — UFV Florestal
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"  /* include/ via -Iinclude no Makefile */

/* ============================================================================
 * Funções de Inicialização
 * ============================================================================ */

/**
 * Inicializa a CPU com todos os campos zerados/nulos.
 * Deve ser chamada antes de carregar qualquer processo para execução.
 */
void inicializar_cpu(Cpu *cpu) {
    if (cpu == NULL) return;

    cpu->programa = NULL;
    cpu->tamanho_programa = 0;
    cpu->pc = 0;
    cpu->variaveis = NULL;
    cpu->num_variaveis = 0;
    cpu->quantum = 0;
    cpu->tempo_usado_quantum = 0;
}

/**
 * Inicializa a TabelaDeProcessos.
 * Todas as entradas são marcadas como inativas, com ponteiros NULL
 * e contadores zerados. O próximo PID começa em 0.
 */
void inicializar_tabela(TabelaDeProcessos *tabela) {
    if (tabela == NULL) return;

    for (int i = 0; i < MAX_PROCESSOS; i++) {
        tabela->processos[i].pid = -1;
        tabela->processos[i].pid_pai = -1;
        tabela->processos[i].pc = 0;
        tabela->processos[i].programa = NULL;
        tabela->processos[i].tamanho_programa = 0;
        tabela->processos[i].variaveis = NULL;
        tabela->processos[i].num_variaveis = 0;
        tabela->processos[i].prioridade = 0;
        tabela->processos[i].estado = PRONTO;
        tabela->processos[i].tempo_inicio = 0;
        tabela->processos[i].tempo_cpu = 0;
        tabela->processos[i].tempo_bloqueio = 0;
        tabela->processos[i].ativo = 0;
    }

    tabela->quantidade = 0;
    tabela->proximo_pid = 0;
}

/**
 * Inicializa uma fila circular simples.
 * Posições de início e fim apontam para 0, tamanho é 0.
 */
void inicializar_fila(FilaSimples *fila) {
    if (fila == NULL) return;

    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;

    /* Limpa o buffer para facilitar debug */
    memset(fila->indices, -1, sizeof(fila->indices));
}

/**
 * Inicializa o EstadoPronto — todas as 4 sub-filas de prioridade.
 */
void inicializar_estado_pronto(EstadoPronto *pronto) {
    if (pronto == NULL) return;

    for (int i = 0; i < NUM_PRIORIDADES; i++) {
        inicializar_fila(&pronto->filas[i]);
    }
}

/**
 * Inicializa o EstadoBloqueado (fila simples).
 * EstadoBloqueado é um typedef de FilaSimples.
 */
void inicializar_estado_bloqueado(EstadoBloqueado *bloqueado) {
    inicializar_fila(bloqueado);
}

/**
 * Inicializa o EstadoExecucao.
 * indice = -1 indica que nenhum processo está em execução.
 */
void inicializar_estado_execucao(EstadoExecucao *execucao) {
    if (execucao == NULL) return;

    execucao->indice = -1;
}

/* ============================================================================
 * Operações de Fila Circular (FilaSimples)
 * ============================================================================ */

/**
 * Adiciona um índice ao final da fila circular.
 *
 * @return 0 em sucesso, -1 se a fila está cheia
 */
int enfileirar(FilaSimples *fila, int indice) {
    if (fila == NULL) return -1;

    /* Verifica se a fila está cheia */
    if (fila->tamanho >= MAX_PROCESSOS) {
        fprintf(stderr, "[ERRO] Fila cheia: não é possível enfileirar índice %d\n",
                indice);
        return -1;
    }

    fila->indices[fila->fim] = indice;
    fila->fim = (fila->fim + 1) % MAX_PROCESSOS;
    fila->tamanho++;

    return 0;
}

/**
 * Remove e retorna o índice do início da fila circular.
 *
 * @return Índice removido, ou -1 se a fila está vazia
 */
int desenfileirar(FilaSimples *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        return -1;
    }

    int indice = fila->indices[fila->inicio];
    fila->indices[fila->inicio] = -1; /* Limpa para debug */
    fila->inicio = (fila->inicio + 1) % MAX_PROCESSOS;
    fila->tamanho--;

    return indice;
}

/**
 * Verifica se a fila está vazia.
 *
 * @return 1 se vazia, 0 caso contrário
 */
int fila_vazia(FilaSimples *fila) {
    if (fila == NULL) return 1;

    return (fila->tamanho == 0) ? 1 : 0;
}

/* ============================================================================
 * Operações do EstadoPronto (Fila de Prioridades)
 * ============================================================================ */

/**
 * Enfileira um processo na sub-fila de prioridade correta.
 * Valida que a prioridade está no intervalo [0, NUM_PRIORIDADES-1].
 *
 * @return 0 em sucesso, -1 em erro
 */
int enfileirar_pronto(EstadoPronto *pronto, int indice, int prioridade) {
    if (pronto == NULL) return -1;

    /* Valida a prioridade */
    if (prioridade < 0 || prioridade >= NUM_PRIORIDADES) {
        fprintf(stderr, "[ERRO] Prioridade inválida: %d (esperado 0–%d)\n",
                prioridade, NUM_PRIORIDADES - 1);
        return -1;
    }

    return enfileirar(&pronto->filas[prioridade], indice);
}

/**
 * Desenfileira o processo de maior prioridade disponível.
 * Percorre da prioridade 0 (mais alta) à 3 (mais baixa) e retorna
 * o primeiro processo encontrado.
 *
 * @return Índice do processo, ou -1 se todas as filas estão vazias
 */
int desenfileirar_pronto(EstadoPronto *pronto) {
    if (pronto == NULL) return -1;

    for (int i = 0; i < NUM_PRIORIDADES; i++) {
        if (!fila_vazia(&pronto->filas[i])) {
            return desenfileirar(&pronto->filas[i]);
        }
    }

    return -1; /* Nenhum processo pronto */
}

/**
 * Verifica se o EstadoPronto está completamente vazio
 * (todas as sub-filas vazias).
 *
 * @return 1 se vazio, 0 caso contrário
 */
int estado_pronto_vazio(EstadoPronto *pronto) {
    if (pronto == NULL) return 1;

    for (int i = 0; i < NUM_PRIORIDADES; i++) {
        if (!fila_vazia(&pronto->filas[i])) {
            return 0;
        }
    }

    return 1;
}

/* ============================================================================
 * Operações na TabelaDeProcessos
 * ============================================================================ */

/**
 * Cria um novo processo na tabela.
 *
 * Busca o primeiro slot inativo, inicializa seus campos com os valores
 * fornecidos e retorna o índice na tabela. O PID é atribuído automaticamente
 * de forma incremental.
 *
 * NOTA: Esta função NÃO aloca o vetor de programa nem o de variáveis —
 * isso deve ser feito pelo módulo que chama esta função (parser ou
 * gerenciador ao processar instrução F).
 *
 * @return Índice na tabela do novo processo, ou -1 se a tabela está cheia
 */
int criar_processo(TabelaDeProcessos *tabela, int pid_pai, int prioridade,
                   int tempo_atual) {
    if (tabela == NULL) return -1;

    /* Busca um slot livre na tabela */
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (!tabela->processos[i].ativo) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        fprintf(stderr, "[ERRO] TabelaDeProcessos cheia: não é possível criar "
                "novo processo\n");
        return -1;
    }

    /* Inicializa os campos do novo processo */
    ProcessoSimulado *proc = &tabela->processos[slot];
    proc->pid = tabela->proximo_pid++;
    proc->pid_pai = pid_pai;
    proc->pc = 0;
    proc->programa = NULL;         /* Será alocado externamente */
    proc->tamanho_programa = 0;
    proc->variaveis = NULL;        /* Será alocado externamente */
    proc->num_variaveis = 0;
    proc->prioridade = prioridade;
    proc->estado = PRONTO;
    proc->tempo_inicio = tempo_atual;
    proc->tempo_cpu = 0;
    proc->tempo_bloqueio = 0;
    proc->ativo = 1;

    tabela->quantidade++;

    return slot;
}

/**
 * Libera toda a memória associada a um processo e marca o slot como inativo.
 *
 * Libera os vetores de programa e variáveis (se alocados), marca o slot
 * como inativo e decrementa o contador de processos ativos.
 */
void liberar_processo(TabelaDeProcessos *tabela, int indice) {
    if (tabela == NULL) return;

    /* Valida o índice */
    if (indice < 0 || indice >= MAX_PROCESSOS) {
        fprintf(stderr, "[ERRO] Índice inválido para liberar_processo: %d\n",
                indice);
        return;
    }

    ProcessoSimulado *proc = &tabela->processos[indice];

    if (!proc->ativo) {
        fprintf(stderr, "[AVISO] Tentativa de liberar processo já inativo "
                "(índice %d)\n", indice);
        return;
    }

    /* Libera vetor de programa */
    if (proc->programa != NULL) {
        free(proc->programa);
        proc->programa = NULL;
    }

    /* Libera vetor de variáveis */
    if (proc->variaveis != NULL) {
        free(proc->variaveis);
        proc->variaveis = NULL;
    }

    /* Marca o slot como inativo */
    proc->ativo = 0;
    proc->estado = TERMINADO;
    proc->pid = -1;
    proc->tamanho_programa = 0;
    proc->num_variaveis = 0;

    tabela->quantidade--;
}

/* ============================================================================
 * Funções Utilitárias
 * ============================================================================ */

/**
 * Retorna o quantum (fatia de tempo) para uma dada prioridade.
 *
 * Conforme a especificação:
 *   Prioridade 0 → quantum 1
 *   Prioridade 1 → quantum 2
 *   Prioridade 2 → quantum 4
 *   Prioridade 3 → quantum 8
 *
 * Fórmula: quantum = 2^prioridade (ou 1 << prioridade)
 */
int obter_quantum(int prioridade) {
    if (prioridade < 0 || prioridade >= NUM_PRIORIDADES) {
        fprintf(stderr, "[AVISO] Prioridade inválida em obter_quantum: %d, "
                "usando padrão (1)\n", prioridade);
        return 1;
    }

    /* quantum = 2^prioridade: {1, 2, 4, 8} */
    return 1 << prioridade;
}

/**
 * Converte um EstadoProcesso para sua representação em string legível.
 * Utilizada pelo processo de impressão e para debug.
 */
const char* estado_para_string(EstadoProcesso estado) {
    switch (estado) {
        case PRONTO:    return "PRONTO";
        case BLOQUEADO: return "BLOQUEADO";
        case EXECUCAO:  return "EXECUCAO";
        case TERMINADO: return "TERMINADO";
        default:        return "DESCONHECIDO";
    }
}
