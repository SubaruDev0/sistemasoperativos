/*
 * shell.c - Intérprete de comandos simple (Taller 1 - Sistemas Operativos)
 *
 * Llamadas a sistema utilizadas:
 *   - write()        -> Item 1: Mostrar el prompt
 *   - read()/fgets() -> Item 2: Leer comando desde teclado
 *   - exit()/_exit() -> Item 3: Comando "exit" para terminar
 *   - execvp()       -> Item 4: Ejecutar comandos; si no existe, retorna error
 *   - (lógica C)     -> Item 5: Enter sin comando => reimprimir prompt
 *   - fork()/execvp()/waitpid() -> Item 6: Ejecución concurrente en foreground
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT   1024
#define MAX_ARGS    128
#define PROMPT      "myshell> "

/*
 * parse_input: Divide la línea de entrada en tokens (comando + argumentos).
 * Retorna el número de argumentos encontrados.
 */
int parse_input(char *input, char **args) {
    int count = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL;
    return count;
}

int main(void) {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        /* -------------------------------------------------------
         * ITEM 1: Mostrar prompt
         * Llamada a sistema: write() sobre stdout
         * ------------------------------------------------------- */
        write(STDOUT_FILENO, PROMPT, strlen(PROMPT));

        /* -------------------------------------------------------
         * ITEM 2: Leer comando desde teclado
         * Usamos fgets() que internamente usa la syscall read()
         * Soporta número indeterminado de argumentos mediante
         * tokenización dinámica en parse_input().
         * ------------------------------------------------------- */
        if (fgets(input, sizeof(input), stdin) == NULL) {
            /* EOF (Ctrl+D) => salir limpiamente */
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        /* -------------------------------------------------------
         * ITEM 5: Si se presiona Enter sin comando, reimprimir prompt
         * No se necesita llamada a sistema adicional; se verifica
         * que el primer carácter sea '\n' (línea vacía).
         * ------------------------------------------------------- */
        if (input[0] == '\n') {
            continue;   /* volver al inicio del loop => nuevo prompt */
        }

        /* -------------------------------------------------------
         * ITEM 2 (cont.): Parsear la entrada
         * ------------------------------------------------------- */
        int argc = parse_input(input, args);
        if (argc == 0) {
            continue;
        }

        /* -------------------------------------------------------
         * ITEM 3: Comando "exit" para terminar la shell
         * Llamada a sistema: exit() / _exit()
         * ------------------------------------------------------- */
        if (strcmp(args[0], "exit") == 0) {
            write(STDOUT_FILENO, "Saliendo de myshell...\n", 23);
            exit(EXIT_SUCCESS);
        }

        /* -------------------------------------------------------
         * ITEM 6: Ejecutar comando en proceso concurrente (foreground)
         * Llamadas a sistema:
         *   fork()    -> crear proceso hijo
         *   execvp()  -> reemplazar imagen del proceso hijo con el comando
         *   waitpid() -> el padre espera a que el hijo termine (foreground)
         *
         * ITEM 4: Si el comando no existe, execvp() retorna -1 y
         * perror() muestra el error correspondiente. El padre
         * continúa gracias a waitpid() y el hijo termina con _exit().
         * ------------------------------------------------------- */
        pid_t pid = fork();

        if (pid < 0) {
            /* Error al crear el proceso */
            perror("fork");
            continue;
        }

        if (pid == 0) {
            /* --- PROCESO HIJO --- */
            execvp(args[0], args);

            /*
             * ITEM 4: Si execvp() retorna, el comando no fue encontrado.
             * Se imprime el error y se termina el hijo.
             * Llamada a sistema: _exit() para no ejecutar destructores.
             */
            fprintf(stderr, "myshell: %s: %s\n", args[0], strerror(errno));
            _exit(EXIT_FAILURE);

        } else {
            /* --- PROCESO PADRE --- */
            int status;
            /*
             * waitpid() bloquea al padre hasta que el hijo termine,
             * implementando el comportamiento foreground (Item 6).
             */
            waitpid(pid, &status, 0);
        }
    }

    return EXIT_SUCCESS;
}
