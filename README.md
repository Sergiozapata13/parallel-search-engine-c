# parallel-search-engine-c

Motor educativo de exploración exhaustiva de espacios de búsqueda en C usando POSIX pthreads sobre Linux.

El proyecto está basado en un ejercicio académico de procesamiento paralelo, pero fue estructurado como un proyecto demostrable de programación de sistemas para practicar C, Linux, concurrencia, validación de memoria, benchmarking y documentación técnica.

## Objetivo

Implementar un motor local de búsqueda exhaustiva que divida un espacio de combinaciones entre múltiples hilos usando POSIX pthreads.

El programa:

- Lee una configuración desde archivo.
- Construye un conjunto de caracteres según el modo seleccionado.
- Calcula el espacio total de búsqueda.
- Divide el trabajo en chunks dinámicos.
- Ejecuta búsqueda paralela con pthreads.
- Permite configurar la cantidad de hilos.
- Permite benchmark con escaneo completo.
- Mide tiempo, intentos y throughput.
- Incluye validación con sanitizers y Valgrind.

## Enfoque educativo

Este repositorio es únicamente educativo y local.

No está orientado a atacar cuentas reales, servicios externos, redes, sistemas de terceros ni contraseñas ajenas. La cadena objetivo se define localmente en un archivo de configuración para estudiar paralelismo, scheduling y rendimiento en C.

## Tecnologías

- C11
- POSIX pthreads
- Linux / WSL / Ubuntu nativo
- gcc
- make
- Valgrind
- AddressSanitizer / UndefinedBehaviorSanitizer
- Git

## Estructura del proyecto

    parallel-search-engine-c/
    ├── Makefile
    ├── README.md
    ├── benchmarks/
    │   └── README.md
    ├── config/
    │   ├── example.conf
    │   └── benchmark.conf
    ├── docs/
    │   ├── ARCHITECTURE.md
    │   ├── BENCHMARKING.md
    │   └── VALIDATION.md
    ├── include/
    │   ├── charset.h
    │   ├── config.h
    │   ├── metrics.h
    │   ├── scheduler.h
    │   ├── search.h
    │   └── timer.h
    ├── scripts/
    │   └── run_benchmarks.sh
    └── src/
        ├── charset.c
        ├── config.c
        ├── main.c
        ├── metrics.c
        ├── scheduler.c
        ├── search.c
        └── timer.c

## Configuración

Ejemplo básico:

    mode=2
    length=4
    target=a9z1
    verbose=0
    print_ranges=1
    chunk_size=100000

Campos:

| Campo | Descripción |
|---|---|
| mode | Modo del conjunto de caracteres |
| length | Longitud de la cadena objetivo |
| target | Cadena objetivo local |
| verbose | Imprime cada intento generado |
| print_ranges | Imprime rangos asignados a cada hilo |
| chunk_size | Tamaño de cada bloque dinámico de trabajo |

Modos de charset:

| Modo | Caracteres |
|---:|---|
| 1 | Números |
| 2 | Números y letras minúsculas |
| 3 | Números, minúsculas y mayúsculas |
| 4 | Números, letras y caracteres especiales |

## Compilación

    make

## Ejecución

Modo automático, usando todos los procesadores lógicos detectados:

    ./parallel_search config/example.conf

Cantidad de hilos configurable:

    ./parallel_search config/example.conf --threads 4

Modo full scan para benchmarking reproducible:

    ./parallel_search config/benchmark.conf --threads 4 --full-scan

## Validación automatizada

Pruebas funcionales básicas:

    make test

Compilación con sanitizers:

    make sanitize

Validación con Valgrind:

    make valgrind

Benchmark rápido:

    make benchmark

Limpieza:

    make clean

## Benchmarking

El script de benchmark ejecuta el programa con diferentes cantidades de hilos y genera un CSV:

    RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh

Salida generada:

    benchmarks/results.csv

El benchmark usa `--full-scan` por defecto para que todos los hilos procesen el espacio completo y los resultados sean comparables.

## Ejemplo de resultados

Ejemplo de benchmark en WSL2 con 12 procesadores lógicos detectados:

| Hilos | Attempts | Tiempo | Throughput |
|---:|---:|---:|---:|
| 1 | 60,466,176 | 0.639 s | 94.57 M attempts/s |
| 2 | 60,466,176 | 0.343 s | 176.50 M attempts/s |
| 4 | 60,466,176 | 0.189 s | 319.11 M attempts/s |

Los resultados varían según CPU, sistema operativo, carga del sistema y entorno de ejecución.

## Documentación adicional

- `docs/ARCHITECTURE.md`: diseño interno del sistema.
- `docs/VALIDATION.md`: comandos de validación y criterios de calidad.
- `docs/BENCHMARKING.md`: metodología de benchmark.

## Pruebas de configuración inválida

El proyecto incluye pruebas para validar que configuraciones incorrectas sean rechazadas correctamente.

Ejecutar:

    make test-invalid

Casos cubiertos:

- archivo inexistente
- mode inválido
- length inválido
- target con longitud incorrecta
- target con caracteres no permitidos
- verbose inválido
- print_ranges inválido
- chunk_size inválido
- target ausente
