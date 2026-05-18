# DOCUMENTACIÓN COMPLETA DE shell.c

## Encabezado y Comentarios (líneas 1-11)
- **Línea 1-11**: Comentarios descriptivos indicando que es un intérprete de comandos simple
- Listado de llamadas a sistema utilizadas y qué item del taller cubre cada una

## Inclusiones de Librerías (líneas 13-19)
- **`#include <stdio.h>`**: Entrada/salida estándar (printf, fprintf, fgets)
- **`#include <stdlib.h>`**: Funciones estándar (exit, EXIT_SUCCESS, EXIT_FAILURE)
- **`#include <string.h>`**: Manejo de strings (strcmp, strtok, strlen)
- **`#include <unistd.h>`**: Llamadas de sistema POSIX (fork, execvp, write)
- **`#include <sys/types.h>`**: Tipos de datos (pid_t)
- **`#include <sys/wait.h>`**: Funciones de espera (waitpid)
- **`#include <errno.h>`**: Variables de error (errno, strerror)

## Definiciones de Constantes (líneas 21-23)
- **`MAX_INPUT 1024`**: Tamaño máximo del buffer para entrada del usuario
- **`MAX_ARGS 128`**: Número máximo de argumentos permitidos
- **`PROMPT "myshell> "`**: Cadena del prompt mostrado al usuario

## Función parse_input (líneas 25-38)
Divide una línea de entrada en tokens individuales:
- **Línea 29**: Definición de función que recibe la entrada y un array de punteros
- **Línea 30**: Inicializa contador a 0
- **Línea 31**: `strtok()` divide la cadena por espacios, tabulaciones y saltos de línea
- **Línea 32**: Bucle mientras haya tokens y no se exceda MAX_ARGS
- **Línea 33**: Guarda cada token en el array `args`
- **Línea 36**: Termina el array con NULL (requerido por execvp)
- **Línea 37**: Retorna el número de argumentos encontrados

## Función main - Inicio (líneas 40-43)
- **Línea 40**: Función principal
- **Línea 41**: Buffer para almacenar la línea de entrada (1024 caracteres)
- **Línea 42**: Array de punteros para los argumentos (hasta 128)

## Loop Principal (línea 44)
- **Línea 44**: `while(1)` - Bucle infinito que mantiene la shell ejecutándose

## ITEM 1: Mostrar el Prompt (líneas 45-49)
- **Línea 49**: `write(STDOUT_FILENO, PROMPT, strlen(PROMPT))`
  - `write()` es una llamada a sistema que escribe directamente en stdout
  - Muestra el prompt "myshell> " sin necesidad de buffer (a diferencia de printf)

## ITEM 2: Leer Comando del Usuario (líneas 51-61)
- **Línea 57**: `fgets(input, sizeof(input), stdin)` - Lee hasta 1024 caracteres
- **Línea 57-61**: Si fgets retorna NULL (Ctrl+D), se imprime newline y sale
- Maneja entrada indeterminada de argumentos

## ITEM 5: Validar Entrada Vacía (líneas 63-70)
- **Línea 68**: Verifica si primer carácter es '\n' (línea vacía)
- **Línea 69**: `continue` salta al siguiente ciclo sin hacer nada (reimprimir prompt)

## Parsear la Entrada (líneas 72-78)
- **Línea 75**: Llama a parse_input() para dividir el comando en tokens
- **Línea 76-78**: Si argc es 0, continúa (no hay comandos válidos)

## ITEM 3: Comando "exit" (líneas 80-87)
- **Línea 84**: `strcmp(args[0], "exit")` - Compara si el primer argumento es "exit"
- **Línea 85-86**: Si es "exit", imprime mensaje y termina con exit(EXIT_SUCCESS)

## ITEM 6 & ITEM 4: Ejecutar Comando (líneas 89-128)

### Línea 100: fork()
- Crea un nuevo proceso (proceso hijo)
- Retorna -1 si hay error, 0 en el hijo, PID del hijo en el padre

### Líneas 102-106: Error en fork()
- Si pid < 0, hubo error
- `perror()` muestra el error
- `continue` para continuar en el loop

### Líneas 108-118: Proceso Hijo
- **Línea 110**: `execvp(args[0], args)` - ITEM 4
  - Reemplaza la imagen del proceso con el comando
  - Si el comando existe, execvp NO retorna
  - Si falla, retorna -1
- **Línea 117**: Si execvp retorna, imprime error mostrando el comando y la razón
- **Línea 118**: `_exit(EXIT_FAILURE)` - Termina el hijo sin ejecutar destructores

### Líneas 120-128: Proceso Padre
- **Línea 127**: `waitpid(pid, &status, 0)`
  - El padre se bloquea esperando que el hijo termine
  - Implementa comportamiento foreground (ITEM 6)
  - El padre continúa el loop solo después de que el hijo terminó

### Línea 131
- Retorna EXIT_SUCCESS si la shell termina normalmente

---

## Resumen del Flujo Completo

```
1. Mostrar prompt "myshell> "
   ↓
2. Leer comando del usuario (fgets)
   ↓
3. ¿Entrada vacía? → Sí: Volver a mostrar prompt
   ↓ No
4. Parsear entrada en tokens
   ↓
5. ¿Comando es "exit"? → Sí: Salir
   ↓ No
6. fork() - Crear proceso hijo
   ↓
7. Proceso hijo: execvp() para ejecutar comando
   ↓
8. Proceso padre: waitpid() espera al hijo (foreground)
   ↓
9. Volver al paso 1
```

## Llamadas a Sistema Utilizadas

| Llamada | Línea | Descripción |
|---------|-------|-------------|
| `write()` | 49, 59, 85 | Escribir en stdout |
| `fgets()` | 57 | Leer entrada del usuario |
| `strtok()` | 31, 34 | Dividir strings en tokens |
| `strcmp()` | 84 | Comparar strings |
| `fork()` | 100 | Crear proceso hijo |
| `execvp()` | 110 | Ejecutar comando |
| `waitpid()` | 127 | Esperar a proceso hijo |
| `_exit()` | 118 | Terminar proceso hijo |
| `exit()` | 86 | Terminar shell |

## Notas Importantes

1. **write() vs printf()**: Se usa `write()` directamente en lugar de `printf()` para mayor control
2. **execvp()**: Busca el comando en el PATH del sistema automáticamente
3. **fork()**: Crea un nuevo proceso con copia de memoria
4. **waitpid()**: Bloquea al padre hasta que el hijo termine (comportamiento foreground)
5. **_exit()**: Termina el proceso sin ejecutar destructores de C++
6. **NULL en args**: Requerido por execvp() para indicar fin de argumentos

---

# DOCUMENTACIÓN DETALLADA - PROFUNDIZACIÓN -------------------------------------------------------------------------------------------------------------------------------------------------------------

---

## Definiciones de Constantes (líneas 21-23) - PROFUNDIZACIÓN

### MAX_INPUT (línea 21) - Explicación Completa
```c
#define MAX_INPUT   1024
```
- **¿Qué es?**: Define el tamaño máximo del buffer para entrada del usuario
- **Capacidad real**: Soporta hasta 1023 caracteres + 1 para el terminador nulo `\0`
- **Para qué sirve**: Especifica el tamaño del array `char input[MAX_INPUT]` en la línea 41
- **Por qué 1024?**: 
  - No es demasiado pequeño (evita cortes de comandos largos)
  - No es demasiado grande (ahorra memoria)
  - Es una potencia de 2 (eficiente en sistemas operativos)
- **Ejemplo**: Si escribes `ls -la /home/usuario/archivo.txt`, esos caracteres se guardan en este buffer

**Visualización del buffer**:
```
Buffer input[1024]:
┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬───┐
│l │s │  │- │l │a │\n│\0│  │  │...│
└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴───┘
 0   1  2  3  4  5  6  7  8  9    1023
                           ↑
                    Terminador nulo
                    (obligatorio)
```

---

### MAX_ARGS (línea 22) - Explicación Completa
```c
#define MAX_ARGS    128
```
- **¿Qué es?**: Define la cantidad máxima de argumentos (palabras) que puede tener un comando
- **Capacidad**: Hasta 128 argumentos diferentes (punteros a palabras)
- **Para qué sirve**: Especifica el tamaño del array `char *args[MAX_ARGS]` en la línea 42
- **Nota importante**: Es para PALABRAS/TOKENS, no caracteres
- **Ejemplo**: En `comando arg1 arg2 arg3 ... arg128`, cada palabra es un argumento

### Diferencia CRÍTICA entre MAX_INPUT y MAX_ARGS
```
Comando escrito: ls -la /home/usuario/archivo.txt

MAX_INPUT (1024): Cantidad de CARACTERES
┌──────────────────────────────────────────────────────┐
│ ls -la /home/usuario/archivo.txt                     │
│ (toda la línea como texto continuo)                  │
│ ~ 30 caracteres en este ejemplo                      │
└──────────────────────────────────────────────────────┘

MAX_ARGS (128): Cantidad de PALABRAS/ARGUMENTOS
┌────────┬────────┬──────────────────────────────────┐
│ ls     │ -la    │ /home/usuario/archivo.txt        │
│ args[0]│ args[1]│         args[2]                  │
│        │        │                                  │
│ (3 palabras en total)                              │
└────────┴────────┴──────────────────────────────────┘
```

**Ejemplo de relación**:
```
Comando: gcc -Wall -O2 -o programa programa.c

Usa caracteres de MAX_INPUT: ~45 caracteres
Usa palabras de MAX_ARGS: 6 argumentos

args[0] = "gcc"
args[1] = "-Wall"
args[2] = "-O2"
args[3] = "-o"
args[4] = "programa"
args[5] = "programa.c"
args[6] = NULL
```

---

### PROMPT (línea 23) - Explicación Completa
```c
#define PROMPT      "myshell> "
```
- **¿Qué es?**: Define el texto del símbolo de la shell
- **Propósito**: Es una "invitación" visual que le indica al usuario que puede escribir un comando
- **Visualización en pantalla**:
  ```
  myshell> 
  ```
- **Longitud**: 9 caracteres (incluyendo el espacio final)
- **Uso**: Se imprime en la línea 49 con `write(STDOUT_FILENO, PROMPT, strlen(PROMPT))`
- **Analogía**: Es como el empleado de una tienda que dice "¿Qué deseas?" - es una invitación
- **En otras shells conocidas**:
  - **bash**: `$ ` o `# ` (dependiendo si eres usuario normal o root)
  - **zsh**: `% `
  - **Windows CMD**: `C:\Users\nombre> `
  - **PowerShell**: `PS C:\Users\nombre> `
  - **Tu shell**: `myshell> ` (personalizado)

---

## Función parse_input (líneas 25-38) - PROFUNDIZACIÓN COMPLETA

### ¿Qué hace?
Divide una línea completa de texto en palabras individuales (tokens) para separarlas en comando y argumentos. Es el "dividor" entre la entrada completa y los argumentos separados que necesita `execvp()`.

### Código completo
```c
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
```

### Explicación línea por línea

#### Línea 29: Declaración de función
```c
int parse_input(char *input, char **args)
```
**Parámetros**:
- **`char *input`**: Puntero a la cadena completa que escribió el usuario
  - Proviene de `fgets()` en la línea 57
  - Contiene toda la línea, máximo 1024 caracteres
  - Ejemplo: `"ls -la /home\n"`
  
- **`char **args`**: Array de punteros donde se guardarán cada una de las palabras
  - Es el array declarado en la línea 42: `char *args[MAX_ARGS];`
  - Cada elemento apunta a una palabra diferente
  - Ejemplo después de procesar: `args[0]="ls"`, `args[1]="-la"`, `args[2]="/home"`

**Retorna**: `int` - La cantidad de palabras (tokens) encontradas

---

#### Línea 30: Inicializar contador
```c
int count = 0;
```
- Comienza en 0
- Se incrementa cada vez que se encuentra y guarda una palabra
- Finaliza con el número total de argumentos encontrados
- Se usa en `args[count++] = token;` para saber dónde guardar cada palabra

---

#### Línea 31: Primera tokenización
```c
char *token = strtok(input, " \t\n");
```

**¿Qué es `strtok()`?**
- Función de la librería `<string.h>` que divide una cadena en partes
- Las partes se llaman "tokens"
- Busca separadores y los reemplaza internamente con `\0`

**Parámetros de `strtok()`**:
- **Primera llamada**: `strtok(input, " \t\n")`
  - `input`: cadena a dividir
  - `" \t\n"`: los separadores (espacios, tabulaciones, saltos de línea)
  
- **Llamadas siguientes**: `strtok(NULL, " \t\n")`
  - `NULL`: le dice a `strtok()` que continúe desde donde paró
  - `strtok()` mantiene un puntero interno para recordar dónde está

**Ejemplo visual**: Si escribes `ls -la /home`

```
Antes:  input = "ls -la /home\n"
                 ↓ strtok(input, " \t\n")
Después: token apunta a "ls"
         strtok marca internamente que cortó en el primer espacio

En la siguiente línea:
         strtok(NULL, " \t\n")
         ↓ 
         token apunta a "-la"
         strtok marca que cortó en el siguiente espacio

Y así sucesivamente...
```

---

#### Línea 32: Bucle mientras haya tokens
```c
while (token != NULL && count < MAX_ARGS - 1)
```

**Dos condiciones de continuidad**:

1. **`token != NULL`**: Mientras `strtok()` encuentre más palabras
   - `strtok()` retorna `NULL` cuando no hay más separadores
   - Significa que no hay más palabras que procesar
   
2. **`count < MAX_ARGS - 1`**: Mientras no excedamos el límite de argumentos
   - `MAX_ARGS = 128`
   - `count < 127` (dejamos espacio para el NULL final en posición 127)
   - Protege contra entrada de comandos con más de 128 argumentos

**Flujo del bucle**:
```
Iteración 1: token = "ls" (no es NULL), count = 0 (< 127) → Continúa
Iteración 2: token = "-la" (no es NULL), count = 1 (< 127) → Continúa
Iteración 3: token = "/home" (no es NULL), count = 2 (< 127) → Continúa
Iteración 4: token = NULL → SALE del while
```

---

#### Línea 33: Guardar el token
```c
args[count++] = token;
```

**Desglose**:
- **`args[count]`**: Accede al elemento del array en la posición `count`
- **`= token`**: Asigna el puntero a la palabra encontrada
- **`count++`**: Post-incremento (usa el valor, luego incrementa)

**Visualización paso a paso**:
```
Iteración 1:
  args[0] = token ("ls"), luego count = 1

Iteración 2:
  args[1] = token ("-la"), luego count = 2

Iteración 3:
  args[2] = token ("/home"), luego count = 3

Estado del array:
args[0] → "ls"
args[1] → "-la"
args[2] → "/home"
args[3] → ??? (sin inicializar aún)
```

---

#### Línea 34: Siguiente token
```c
token = strtok(NULL, " \t\n");
```

**Cómo funciona `strtok()` internamente**:
- **Primera llamada** (línea 31): `strtok(input, " \t\n")`
  - `strtok()` examina la cadena desde el principio
  - Encuentra el primer separador
  - Retorna un puntero a la primera palabra
  - **Guarda internamente** dónde está el próximo carácter a examinar
  
- **Llamadas siguientes** (línea 34): `strtok(NULL, " \t\n")`
  - `NULL` le dice a `strtok()`: "Continúa desde donde estabas"
  - Examina desde el punto guardado
  - Encuentra el siguiente separador
  - Retorna un puntero a la siguiente palabra

**Ejemplo completo**:
```
Input: "ls -la /home\n"

Llamada 1: strtok(input, " \t\n")
           Examina: l s [espacio] -la /home\n
           Retorna: "ls"
           Marca internamente: posición después del espacio

Llamada 2: strtok(NULL, " \t\n")
           Continúa desde la posición marcada
           Examina: - l a [espacio] /home\n
           Retorna: "-la"
           Marca internamente: posición después del espacio

Llamada 3: strtok(NULL, " \t\n")
           Continúa desde la posición marcada
           Examina: / h o m e \n
           Retorna: "/home"
           Marca internamente: posición del \n

Llamada 4: strtok(NULL, " \t\n")
           Continúa desde la posición marcada
           Examina: \n (solo separador, fin de cadena)
           Retorna: NULL
           (sale del bucle while)
```

---

#### Línea 36: Terminar el array con NULL
```c
args[count] = NULL;
```

**Este paso es CRÍTICO y OBLIGATORIO**:
- Después de guardar todos los tokens, pone `NULL` en la posición siguiente
- Es un marcador de "fin de lista"
- Es **obligatorio** para `execvp()` en la línea 110
- Le indica a `execvp()` dónde termina la lista de argumentos
- Sin esto, `execvp()` no sabría dónde termina el array

**Visualización completa**:
```
Después del bucle while (3 palabras encontradas, count = 3):

args[0] → "ls"
args[1] → "-la"
args[2] → "/home"
args[3] → sin definir

Después de args[count] = NULL (que es args[3] = NULL):

args[0] → "ls"
args[1] → "-la"
args[2] → "/home"
args[3] → NULL    ← MARCADOR DE FIN (crítico)
args[4] → sin definir (no se usa)
...
args[127] → sin definir (no se usa)
```

**¿Por qué es obligatorio?**
```c
// execvp() en la línea 110 necesita saber dónde termina el array:
execvp(args[0], args);
// execvp recorre: args[0], args[1], args[2], ...
// Hasta encontrar args[i] == NULL, entonces se detiene
```

Sin el NULL, `execvp()` seguiría leyendo memoria basura después de `/home`, lo que causaría comportamiento impredecible o crash.

---

#### Línea 37: Retornar cantidad
```c
return count;
```
- Devuelve cuántas palabras se encontraron (sin contar el NULL)
- En el ejemplo anterior retorna `3`
- Se guarda en `argc` en la línea 75: `int argc = parse_input(input, args);`

---

### Ejemplo Completo de Ejecución

**Si escribes en la shell**: `ls -la /home`

```
PASO 1: Entrada en buffer de fgets()
        "ls -la /home\n"
        
PASO 2: parse_input() es llamado con:
        - input = "ls -la /home\n"
        - args = array vacío de 128 punteros

PASO 3: Ejecución interna
        
        Línea 30: count = 0
        
        Línea 31: strtok(input, " \t\n")
                  token = "ls"
        
        Línea 32: while ("ls" != NULL && 0 < 127) → VERDADERO
        
        Línea 33: args[0] = "ls", count = 1
        
        Línea 34: strtok(NULL, " \t\n")
                  token = "-la"
        
        Línea 32: while ("-la" != NULL && 1 < 127) → VERDADERO
        
        Línea 33: args[1] = "-la", count = 2
        
        Línea 34: strtok(NULL, " \t\n")
                  token = "/home"
        
        Línea 32: while ("/home" != NULL && 2 < 127) → VERDADERO
        
        Línea 33: args[2] = "/home", count = 3
        
        Línea 34: strtok(NULL, " \t\n")
                  token = NULL (no hay más palabras)
        
        Línea 32: while (NULL != NULL && ...) → FALSO (sale del bucle)
        
        Línea 36: args[3] = NULL (marcador de fin)
        
        Línea 37: return 3

PASO 4: Después de parse_input()

        args[0] = "ls"
        args[1] = "-la"
        args[2] = "/home"
        args[3] = NULL
        
        argc = 3 (número de argumentos)
```

---

### ¿Por qué es importante parse_input()?

**Sin `parse_input()`**, tendrías solo una cadena entera: `"ls -la /home"`

**Pero `execvp()` necesita cada parte separada**:

```c
// ❌ INCORRECTO: le pasas todo junto
execvp("ls -la /home", ...);  
// execvp buscaría un comando literalmente llamado "ls -la /home"
// No lo encontraría (porque los comandos no tienen espacios)
// FALLA

// ✅ CORRECTO: le pasas las partes separadas
execvp(args[0], args);
// args[0] = "ls" (comando a ejecutar)
// args[1] = "-la" (argumento 1)
// args[2] = "/home" (argumento 2)
// args[3] = NULL (fin de lista)
// FUNCIONA: ejecuta "ls" con argumentos "-la" y "/home"
```

**Resumen de la importancia**:
- `strtok()` divide la cadena encontrando separadores
- El array `args[]` guarda punteros a cada palabra
- El NULL al final es obligatorio para `execvp()`
- Sin esta separación, el sistema operativo no sabría dónde termina el comando y dónde comienzan los argumentos
- Es el puente entre la entrada del usuario y lo que necesita el sistema operativo

---

## Conexión entre Constantes, Buffer y parse_input()

**Diagrama de flujo de datos**:

```
┌─────────────────────────────────────┐
│   Usuario escribe en teclado        │
│   "ls -la /home"                    │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   fgets() lee del teclado           │
│   (usa MAX_INPUT = 1024)            │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   Buffer input[1024]                │
│   "ls -la /home\n"                  │
│   (9 caracteres en este caso)       │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   parse_input(input, args) es       │
│   llamado para dividir el buffer    │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   strtok() busca separadores        │
│   (" ", "\t", "\n")                 │
│   y divide la cadena                │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   Array args[128] se llena          │
│   args[0] = "ls"                    │
│   args[1] = "-la"                   │
│   args[2] = "/home"                 │
│   args[3] = NULL (marcador)         │
│   args[4...127] = sin usar          │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   parse_input() retorna 3           │
│   (número de argumentos)            │
└─────────────────────────────────────┘
```

---

## Función main - Inicio (líneas 40-43) - PROFUNDIZACIÓN

- **Línea 40**: Función principal
- **Línea 41**: Buffer para almacenar la línea de entrada (1024 caracteres)
  - Declaración: `char input[MAX_INPUT];`
  - `MAX_INPUT` = 1024
  - Capacidad real: 1023 caracteres + 1 para `\0`
  
- **Línea 42**: Array de punteros para los argumentos (hasta 128)
  - Declaración: `char *args[MAX_ARGS];`
  - `MAX_ARGS` = 128
  - Cada elemento es un puntero a una cadena (una palabra)
