# myshell — Intérprete de Comandos Simple

Taller 1 — Sistemas Operativos

## Descripción

`myshell` es un intérprete de comandos (shell) básico para Linux, implementado en C como parte del Taller 1 de la asignatura Sistemas Operativos. Utiliza llamadas al sistema POSIX para la gestión de procesos y E/S.

## Integrantes

- Javier Morales
- Ignacio Barra
- Carlos Bastidas
- Matias Salazar

## Compilación

```bash
make
# o manualmente:
gcc -Wall -Wextra -o myshell shell.c
```

## Ejecución

```bash
./myshell
```

## Uso

| Comando               | Descripción                              |
|-----------------------|------------------------------------------|
| `comando [args...]`   | Ejecuta un comando con argumentos        |
| `exit`                | Termina la shell                         |
| `Enter` (vacío)       | Reimprime el prompt                      |
| `Ctrl+D`              | Termina la shell (EOF)                   |

## Llamadas a Sistema Utilizadas

| Syscall      | Propósito                          |
|-------------|------------------------------------|
| `write()`   | Mostrar el prompt                  |
| `fgets()`   | Leer comandos del usuario          |
| `fork()`    | Crear proceso hijo                 |
| `execvp()`  | Ejecutar el comando en el hijo     |
| `waitpid()` | Esperar finalización del hijo      |
| `exit()`    | Terminar la shell                  |
| `_exit()`   | Terminar el hijo ante error        |

## Estructura del Proyecto

```
sistemasoperativos/
├── shell.c                          # Código fuente
├── Makefile                         # Compilación automatizada
├── README.md                        # Este archivo
├── .gitignore                       # Ignorar ejecutable
└── screenshots/
    ├── 01_compilacion.png
    ├── 02_comandos_basicos.png
    ├── 03_comando_exit.png
    ├── 04_comando_inexistente.png
    ├── 05_enter_vacio.png
    ├── 06_sesion_completa.png
    └── 07_ejecucion_foreground.png

```
