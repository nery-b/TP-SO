/**
 * ============================================================================
 * estruturas.h — Estruturas de dados centrais do simulador de processos
 * ============================================================================
 *
 * Define as 6 estruturas de dados principais do gerenciador de processos:
 *   1. Tempo           — Relógio do simulador
 *   2. Cpu             — Registradores do processo em execução
 *   3. TabelaDeProcessos — Vetor com todos os processos simulados
 *   4. EstadoPronto    — Fila de prioridades (processos prontos)
 *   5. EstadoBloqueado — Fila simples (processos bloqueados)
 *   6. EstadoExecucao  — Índice do processo em execução
 *
 * Autor: Pessoa 3
 * Disciplina: Sistemas Operacionais — UFV Florestal
 */

#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

/* ============================================================================
 * Constantes do simulador
 * ============================================================================ */

/** Número máximo de processos simultâneos na tabela */
#define MAX_PROCESSOS 64

/** Número máximo de instruções por programa simulado */
#define MAX_INSTRUCOES 256

/** Número máximo de variáveis por processo simulado */
#define MAX_VARIAVEIS 64

/** Número de classes de prioridade (0 a 3) */
#define NUM_PRIORIDADES 4

/** Tamanho máximo do nome de arquivo (instrução R) */
#define MAX_NOME_ARQUIVO 256

/* ============================================================================
 * Enumerações
 * ============================================================================ */

/**
 * Estados possíveis de um processo simulado.
 */
typedef enum {
    PRONTO,      /**< Processo pronto para executar */
    BLOQUEADO,   /**< Processo bloqueado (aguardando) */
    EXECUCAO,    /**< Processo em execução na CPU */
    TERMINADO    /**< Processo finalizado */
} EstadoProcesso;

/* ============================================================================
 * Structs
 * ============================================================================ */

/**
 * Instrucao — Representa uma instrução do programa simulado.
 *
 * Cada instrução é armazenada num vetor (o "programa"). O campo 'tipo'
 * identifica a instrução (N, D, V, A, S, B, T, F, R) e os operandos
 * carregam os argumentos necessários conforme o tipo.
 */
typedef struct {
    char tipo;                          /**< Tipo da instrução: N,D,V,A,S,B,T,F,R */
    int operando1;                      /**< 1º argumento (variável, valor, etc.) */
    int operando2;                      /**< 2º argumento (para V, A, S) */
    char nome_arquivo[MAX_NOME_ARQUIVO]; /**< Nome do arquivo (usado apenas por R) */
} Instrucao;

/**
 * Cpu — Simula os registradores do processo em execução.
 *
 * Toda a execução de instruções é confinada a esta estrutura.
 * A TabelaDeProcessos NÃO é acessada durante a execução — apenas
 * durante troca de contexto os valores são copiados de/para aqui.
 */
typedef struct {
    Instrucao *programa;     /**< Ponteiro para o vetor de programa carregado */
    int tamanho_programa;    /**< Número de instruções no programa */
    int pc;                  /**< Contador de Programa (Program Counter) */
    int *variaveis;          /**< Ponteiro para o vetor de variáveis (memória) */
    int num_variaveis;       /**< Quantidade de variáveis alocadas */
    int quantum;             /**< Fatia de tempo alocada para a prioridade atual */
    int tempo_usado_quantum; /**< Unidades de tempo já usadas nesta fatia */
} Cpu;

/**
 * ProcessoSimulado — Entrada individual na TabelaDeProcessos.
 *
 * Cada entrada representa um processo simulado que ainda não terminou
 * (ou que já terminou e foi marcado como inativo para reutilização do slot).
 */
typedef struct {
    int pid;              /**< Identificador único do processo */
    int pid_pai;          /**< Identificador do processo pai */
    int pc;               /**< Valor do contador de programa (inicialmente 0) */
    Instrucao *programa;  /**< Ponteiro para o vetor de programa */
    int tamanho_programa; /**< Número de instruções no programa */
    int *variaveis;       /**< Ponteiro para o vetor de variáveis */
    int num_variaveis;    /**< Número de variáveis alocadas */
    int prioridade;       /**< Classe de prioridade (0 = mais alta, 3 = mais baixa) */
    EstadoProcesso estado; /**< Estado atual do processo */
    int tempo_inicio;     /**< Tempo (tick) em que o processo foi criado */
    int tempo_cpu;        /**< Tempo de CPU já utilizado pelo processo */
    int tempo_bloqueio;   /**< Tempo restante de bloqueio (instrução B) */
    int ativo;            /**< 1 se a entrada está em uso, 0 se livre */
} ProcessoSimulado;

/**
 * TabelaDeProcessos — Vetor de processos simulados.
 *
 * Armazena todos os processos que existem no simulador.
 * Slots inativos (ativo == 0) podem ser reutilizados quando
 * novos processos são criados.
 */
typedef struct {
    ProcessoSimulado processos[MAX_PROCESSOS]; /**< Vetor de processos */
    int quantidade;   /**< Número de processos ativos no momento */
    int proximo_pid;  /**< Próximo PID disponível (incrementado a cada criação) */
} TabelaDeProcessos;

/**
 * FilaSimples — Fila circular implementada com array estático.
 *
 * Usada como bloco base tanto para EstadoPronto (uma fila por prioridade)
 * quanto para EstadoBloqueado. Armazena índices da TabelaDeProcessos.
 */
typedef struct {
    int indices[MAX_PROCESSOS]; /**< Buffer circular de índices */
    int inicio;   /**< Posição do primeiro elemento */
    int fim;      /**< Posição após o último elemento */
    int tamanho;  /**< Número de elementos na fila */
} FilaSimples;

/**
 * EstadoPronto — Fila de prioridades para processos prontos.
 *
 * Implementada como 4 filas separadas (uma por nível de prioridade).
 * filas[0] = prioridade 0 (mais alta, quantum = 1)
 * filas[1] = prioridade 1 (quantum = 2)
 * filas[2] = prioridade 2 (quantum = 4)
 * filas[3] = prioridade 3 (mais baixa, quantum = 8)
 */
typedef struct {
    FilaSimples filas[NUM_PRIORIDADES]; /**< Uma fila por classe de prioridade */
} EstadoPronto;

/**
 * EstadoBloqueado — Fila simples de processos bloqueados.
 *
 * Armazena os índices da TabelaDeProcessos dos processos
 * que estão aguardando (instrução B).
 */
typedef FilaSimples EstadoBloqueado;

/**
 * EstadoExecucao — Processo atualmente em execução.
 *
 * Armazena o índice na TabelaDeProcessos do processo que está
 * rodando no momento. indice == -1 indica que nenhum processo
 * está em execução.
 */
typedef struct {
    int indice; /**< Índice na TabelaDeProcessos (-1 = nenhum) */
} EstadoExecucao;

/* ============================================================================
 * Protótipos de funções — Inicialização
 * ============================================================================ */

/**
 * Inicializa a estrutura da CPU com valores padrão.
 * Ponteiros são definidos como NULL, contadores zerados.
 *
 * @param cpu Ponteiro para a estrutura Cpu a ser inicializada
 */
void inicializar_cpu(Cpu *cpu);

/**
 * Inicializa a TabelaDeProcessos.
 * Marca todas as entradas como inativas e zera contadores.
 *
 * @param tabela Ponteiro para a tabela a ser inicializada
 */
void inicializar_tabela(TabelaDeProcessos *tabela);

/**
 * Inicializa uma FilaSimples (fila circular).
 *
 * @param fila Ponteiro para a fila a ser inicializada
 */
void inicializar_fila(FilaSimples *fila);

/**
 * Inicializa o EstadoPronto (todas as 4 sub-filas).
 *
 * @param pronto Ponteiro para o EstadoPronto a ser inicializado
 */
void inicializar_estado_pronto(EstadoPronto *pronto);

/**
 * Inicializa o EstadoBloqueado (fila simples).
 *
 * @param bloqueado Ponteiro para o EstadoBloqueado a ser inicializado
 */
void inicializar_estado_bloqueado(EstadoBloqueado *bloqueado);

/**
 * Inicializa o EstadoExecucao (indice = -1, nenhum processo em execução).
 *
 * @param execucao Ponteiro para o EstadoExecucao a ser inicializado
 */
void inicializar_estado_execucao(EstadoExecucao *execucao);

/* ============================================================================
 * Protótipos de funções — Operações de fila
 * ============================================================================ */

/**
 * Adiciona um índice ao final da fila.
 *
 * @param fila   Ponteiro para a FilaSimples
 * @param indice Índice da TabelaDeProcessos a enfileirar
 * @return 0 em sucesso, -1 se a fila está cheia
 */
int enfileirar(FilaSimples *fila, int indice);

/**
 * Remove e retorna o índice do início da fila.
 *
 * @param fila Ponteiro para a FilaSimples
 * @return Índice removido, ou -1 se a fila está vazia
 */
int desenfileirar(FilaSimples *fila);

/**
 * Verifica se a fila está vazia.
 *
 * @param fila Ponteiro para a FilaSimples
 * @return 1 se vazia, 0 caso contrário
 */
int fila_vazia(FilaSimples *fila);

/**
 * Enfileira um processo na sub-fila de prioridade correta do EstadoPronto.
 *
 * @param pronto    Ponteiro para o EstadoPronto
 * @param indice    Índice da TabelaDeProcessos
 * @param prioridade Classe de prioridade (0–3)
 * @return 0 em sucesso, -1 em erro
 */
int enfileirar_pronto(EstadoPronto *pronto, int indice, int prioridade);

/**
 * Desenfileira o processo de maior prioridade do EstadoPronto.
 * Percorre da prioridade 0 (mais alta) à 3 (mais baixa).
 *
 * @param pronto Ponteiro para o EstadoPronto
 * @return Índice do processo, ou -1 se todas as filas estão vazias
 */
int desenfileirar_pronto(EstadoPronto *pronto);

/**
 * Verifica se o EstadoPronto está completamente vazio.
 *
 * @param pronto Ponteiro para o EstadoPronto
 * @return 1 se todas as filas estão vazias, 0 caso contrário
 */
int estado_pronto_vazio(EstadoPronto *pronto);

/* ============================================================================
 * Protótipos de funções — Operações na TabelaDeProcessos
 * ============================================================================ */

/**
 * Cria um novo processo na TabelaDeProcessos.
 * Encontra um slot inativo, inicializa os campos e retorna o índice.
 *
 * @param tabela      Ponteiro para a TabelaDeProcessos
 * @param pid_pai     PID do processo pai (-1 se for o processo inicial)
 * @param prioridade  Classe de prioridade inicial
 * @param tempo_atual Tempo atual do simulador
 * @return Índice na tabela do novo processo, ou -1 se a tabela está cheia
 */
int criar_processo(TabelaDeProcessos *tabela, int pid_pai, int prioridade,
                   int tempo_atual);

/**
 * Libera a memória (programa e variáveis) de um processo e marca
 * o slot como inativo na TabelaDeProcessos.
 *
 * @param tabela Ponteiro para a TabelaDeProcessos
 * @param indice Índice do processo a ser liberado
 */
void liberar_processo(TabelaDeProcessos *tabela, int indice);

/* ============================================================================
 * Protótipos de funções — Utilitários
 * ============================================================================ */

/**
 * Retorna o quantum (fatia de tempo) correspondente a uma prioridade.
 *   Prioridade 0 → 1 unidade
 *   Prioridade 1 → 2 unidades
 *   Prioridade 2 → 4 unidades
 *   Prioridade 3 → 8 unidades
 *
 * @param prioridade Classe de prioridade (0–3)
 * @return Quantum em unidades de tempo
 */
int obter_quantum(int prioridade);

/**
 * Converte um EstadoProcesso para sua representação em string.
 * Útil para funções de impressão/debug.
 *
 * @param estado Estado do processo
 * @return String constante representando o estado
 */
const char* estado_para_string(EstadoProcesso estado);

#endif /* ESTRUTURAS_H */
