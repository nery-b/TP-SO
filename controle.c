#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void processo_gerenciador(int pipefd[]) {
    char comando;

    close(pipefd[1]);

    printf("Processo gerenciador criado.\n");

    while (read(pipefd[0], &comando, sizeof(char)) > 0) {
        if (comando == 'U') {
            printf("Gerenciador recebeu comando U: fim de uma unidade de tempo.\n");
        } else if (comando == 'I') {
            printf("Gerenciador recebeu comando I: imprimir estado atual do sistema.\n");
        } else if (comando == 'M') {
            printf("Gerenciador recebeu comando M: imprimir tempo medio e finalizar.\n");
            break;
        } else {
            printf("Gerenciador recebeu comando invalido: %c\n", comando);
        }
    }

    close(pipefd[0]);

    printf("Processo gerenciador finalizado.\n");
    exit(0);
}

void processo_controle(int pipefd[]) {
    char comando;

    close(pipefd[0]);

    printf("Processo controle iniciado.\n");
    printf("Digite os comandos U, I ou M.\n");

    while (1) {
        printf("Comando: ");
        scanf(" %c", &comando);

        if (comando != 'U' && comando != 'I' && comando != 'M') {
            printf("Comando invalido. Digite apenas U, I ou M.\n");
            continue;
        }

        write(pipefd[1], &comando, sizeof(char));

        printf("Controle enviou comando %c para o gerenciador.\n", comando);

        if (comando == 'M') {
            break;
        }
    }

    close(pipefd[1]);
    wait(NULL);

    printf("Processo controle finalizado.\n");
}

int main() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        printf("Erro ao criar pipe.\n");
        return 1;
    }

    printf("Pipe criado com sucesso.\n");

    pid = fork();

    if (pid < 0) {
        printf("Erro ao criar processo gerenciador.\n");
        return 1;
    }

    if (pid == 0) {
        processo_gerenciador(pipefd);
    } else {
        processo_controle(pipefd);
    }

    return 0;
}
