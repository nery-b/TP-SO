# Relatório de Testes - Simulador de Gerenciamento de Processos

## Programas Executados

### init.txt (Programa Principal)
- **Objetivo:** Criar dois processos filho via fork e substituir programas via replace
- **Instruções:**
  - `N 2` - Aloca 2 variáveis
  - `D 0, D 1` - Declara variáveis 0 e 1
  - `V 0 1, V 1 500` - Atribui valores iniciais
  - `F 1` - Fork: cria PID 1, salta 1 instrução (pula R)
  - `R tests/file_a.txt` - Replace: substitui programa
  - `F 1` - Fork: cria PID 2, salta 1 instrução
  - `R tests/file_b.txt` - Replace: substitui programa novamente
  - `T` - Termina

### file_a.txt (Filho do Processo 0)
- Executado após replace do PID 0
- Contém operações de soma (A) e subtração (S)
- Faz fork criando PID 3
- Replace com `tests/file_d.txt`

### file_b.txt (Filho do Processo 1)
- Executado após replace do PID 1
- Contém múltiplas operações aritméticas
- Faz dois forks criando novos processos
- Replace com `tests/file_c.txt` e `tests/file_d.txt`

### file_c.txt e file_d.txt (Programas Substitutos)
- Programas executados após operações de replace
- Contêm operações simples para simular carga

## Resultado da Execução

### Estado Final (t=20)
- **Tempo Total:** 20 unidades
- **Processos Ativos:** 2
  - **PID 1:** Em EXECUÇÃO, Prioridade 3, CPU = 8, PC = 7
  - **PID 2:** Em PRONTO, Prioridade 3, CPU = 4, PC = 3
- **Filas:** MLFQ funcionando corretamente
  - Prioridade 3 tem PID 2 aguardando execução

### Demonstração do bloqueio (`B`)

- **Programa usado:** `tests/programa_bloqueio.txt`
- **Comandos usados:** `tests/comandos_bloqueio.txt`
- **Instrução adicionada:** `B 2`

#### Visualização observada

- Em `t=4`, a impressão do estado mostrou:
  - `FILA DE BLOQUEADOS: PID: 0 | Tempo restante: 1`
  - `TABELA DE PROCESSOS: PID 0 | ESTADO = BLOQUEADO`
- Em `t=5`, o processo voltou para `EXECUCAO` e a fila de bloqueados ficou vazia.

```
[t=4] Imprimindo estado do sistema...
║ FILA DE BLOQUEADOS: PID: 0   | Tempo restante: 1
║   0   | -1  | BLOQUEADO  | 1    | 4   | 3

[t=5] Imprimindo estado do sistema...
║ FILA DE BLOQUEADOS: Nenhum processo bloqueado
║   0   | -1  | EXECUCAO   | 1    | 4   | 3
```

### Observações

1. **Fork (F):** Funcionou corretamente
   - Criou novos processos (PID 1 e 2)
   - Filho recebeu cópia do programa e memória
   - Pai pulou n instruções conforme especificado
   - Filhos foram enfileirados em PRONTO

2. **Replace (R):** Funcionou corretamente
   - Carregou novos programas dos arquivos
   - PC foi resetado para 0
   - Manteve PID, PPID e prioridade do processo

3. **Escalonamento MLFQ:**
   - Prioridades aumentaram (pioraram) quando quantum foi atingido
   - Processos alternaram execução corretamente
   - Fila de prontos funcionou como esperado

4. **Testes Executados:**
   - `programa_basico.txt` / `comandos_basico.txt` - Teste simples ✓
   - `programa_fandr.txt` / `comandos_fandr.txt` - Teste de F e R isolados ✓
   - `programs/init.txt` / `tests/comandos_data.txt` - Teste completo com múltiplos processos ✓

## Conclusão

O simulador está funcionando conforme especificação:
- ✓ Instruções básicas implementadas (N, D, V, A, S, B, T)
- ✓ Fork (F) cria novos processos com cópia de programa e memória
- ✓ Replace (R) substitui programa e recria memória
- ✓ Escalonamento MLFQ com 4 níveis de prioridade
- ✓ Troca de contexto funcional
- ✓ Gerenciamento de filas de prontos e bloqueados
