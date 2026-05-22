# TP-SO

Simulador de Sistemas Operacionais em C.

## Como compilar

```
make
```

## Como rodar o simulador básico

```
./simulador
```

Então informe:
- `tests/programa_basico.txt` como arquivo do programa inicial
- `2` como origem dos comandos
- `tests/comandos_basico.txt` como arquivo de comandos

## Como rodar os testes

```
make test
```

## Demonstração do bloqueio (`B`)

Para validar na prática o funcionamento da instrução `B`, use o programa de exemplo em `tests/programa_bloqueio.txt` e o roteiro de comandos em `tests/comandos_bloqueio.txt`:

```
printf 'tests/programa_bloqueio.txt\n2\ntests/comandos_bloqueio.txt\n' | ./simulador
```

A visualização deve mostrar:
- em `t=4`, o processo `PID 0` na fila de bloqueados com `Tempo restante: 1`
- em `t=5`, o processo voltando para `EXECUCAO`
