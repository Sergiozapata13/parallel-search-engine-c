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

## Análisis automático de benchmarks

El proyecto incluye un script para analizar `benchmarks/results.csv` y calcular métricas agregadas:

- tiempo promedio
- throughput promedio
- speedup
- eficiencia paralela

Ejecutar análisis sobre el CSV existente:

    make analyze-benchmark

Ejecutar benchmark y análisis en un solo paso:

    make benchmark-report

También puede ejecutarse directamente:

    python3 scripts/analyze_benchmarks.py benchmarks/results.csv

## Reporte versionable de benchmark

El proyecto puede generar un resumen Markdown versionable del benchmark:

    make benchmark-summary

Este comando:

1. ejecuta benchmarks,
2. genera `benchmarks/results.csv`,
3. analiza los resultados,
4. escribe `benchmarks/summary.md`.

El archivo `benchmarks/results.csv` es un artefacto generado e ignorado por Git. El archivo `benchmarks/summary.md` sí se versiona para documentar resultados representativos.

## Comandos de prueba y visualización de resultados

Esta sección resume los comandos principales para compilar, probar, validar memoria, ejecutar benchmarks y revisar resultados.

### 1. Compilación normal

Compilar el proyecto:

    make clean
    make

Ejecutar con la configuración de ejemplo:

    ./parallel_search config/example.conf

Ejecutar con una cantidad específica de hilos:

    ./parallel_search config/example.conf --threads 4

### 2. Pruebas funcionales

Ejecutar pruebas funcionales básicas:

    make test

Este comando prueba el programa con distintas cantidades de hilos y verifica que las configuraciones principales funcionen correctamente.

### 3. Pruebas de configuraciones inválidas

Ejecutar pruebas de archivos de configuración incorrectos:

    make test-invalid

Este comando valida que el programa rechace casos como:

- modo inválido,
- longitud inválida,
- target con longitud incorrecta,
- target con caracteres no permitidos,
- verbose inválido,
- print_ranges inválido,
- chunk_size inválido,
- archivo inexistente.

### 4. Validación completa

Ejecutar una validación general del proyecto:

    make check

Este comando combina:

- compilación limpia,
- pruebas funcionales,
- pruebas de configuraciones inválidas,
- compilación con sanitizers,
- ejecución con sanitizers.

### 5. Validación con Valgrind

Ejecutar Valgrind:

    make valgrind

Resultado esperado:

    All heap blocks were freed -- no leaks are possible
    ERROR SUMMARY: 0 errors from 0 contexts

Esto valida que no haya fugas de memoria ni errores detectados por Valgrind en la prueba ejecutada.

### 6. Benchmark rápido

Ejecutar benchmark rápido:

    make benchmark

Este comando genera:

    benchmarks/results.csv

El archivo CSV contiene resultados por cantidad de hilos, incluyendo tiempo, intentos y throughput.

### 7. Ver resultados del benchmark

Ver el CSV directamente:

    cat benchmarks/results.csv

Verlo en formato de tabla en consola:

    column -s, -t benchmarks/results.csv | less -S

Analizar el CSV y calcular speedup y eficiencia:

    make analyze-benchmark

También puede ejecutarse directamente:

    python3 scripts/analyze_benchmarks.py benchmarks/results.csv

### 8. Benchmark con reporte automático

Ejecutar benchmark y análisis en un solo paso:

    make benchmark-report

Este comando:

1. ejecuta el benchmark,
2. genera `benchmarks/results.csv`,
3. calcula tiempo promedio, throughput, speedup y eficiencia,
4. imprime una tabla Markdown en consola.

### 9. Reporte versionable de benchmark

Generar un resumen Markdown versionable:

    make benchmark-summary

Este comando genera:

    benchmarks/summary.md

Ver el resumen:

    sed -n '1,160p' benchmarks/summary.md

El archivo `benchmarks/results.csv` es un artefacto generado e ignorado por Git. El archivo `benchmarks/summary.md` se versiona para documentar resultados representativos.

### 10. Flujo recomendado de validación final

Antes de considerar estable una versión, ejecutar:

    make clean
    make check
    make valgrind
    make benchmark-report

Después revisar el estado del repositorio:

    git status

El estado esperado es:

    nothing to commit, working tree clean

## Comandos de prueba y visualización de resultados

Esta sección resume los comandos principales para compilar, probar, validar memoria, ejecutar benchmarks y revisar resultados.

### 1. Compilación normal

Compilar el proyecto:

    make clean
    make

Ejecutar con la configuración de ejemplo:

    ./parallel_search config/example.conf

Ejecutar con una cantidad específica de hilos:

    ./parallel_search config/example.conf --threads 4

### 2. Pruebas funcionales

Ejecutar pruebas funcionales básicas:

    make test

Este comando prueba el programa con distintas cantidades de hilos y verifica que las configuraciones principales funcionen correctamente.

### 3. Pruebas de configuraciones inválidas

Ejecutar pruebas de archivos de configuración incorrectos:

    make test-invalid

Este comando valida que el programa rechace casos como:

- modo inválido,
- longitud inválida,
- target con longitud incorrecta,
- target con caracteres no permitidos,
- verbose inválido,
- print_ranges inválido,
- chunk_size inválido,
- archivo inexistente.

### 4. Validación completa

Ejecutar una validación general del proyecto:

    make check

Este comando combina:

- compilación limpia,
- pruebas funcionales,
- pruebas de configuraciones inválidas,
- compilación con sanitizers,
- ejecución con sanitizers.

### 5. Validación con Valgrind

Ejecutar Valgrind:

    make valgrind

Resultado esperado:

    All heap blocks were freed -- no leaks are possible
    ERROR SUMMARY: 0 errors from 0 contexts

Esto valida que no haya fugas de memoria ni errores detectados por Valgrind en la prueba ejecutada.

### 6. Benchmark rápido

Ejecutar benchmark rápido:

    make benchmark

Este comando genera:

    benchmarks/results.csv

El archivo CSV contiene resultados por cantidad de hilos, incluyendo tiempo, intentos y throughput.

### 7. Ver resultados del benchmark

Ver el CSV directamente:

    cat benchmarks/results.csv

Verlo en formato de tabla en consola:

    column -s, -t benchmarks/results.csv | less -S

Analizar el CSV y calcular speedup y eficiencia:

    make analyze-benchmark

También puede ejecutarse directamente:

    python3 scripts/analyze_benchmarks.py benchmarks/results.csv

### 8. Benchmark con reporte automático

Ejecutar benchmark y análisis en un solo paso:

    make benchmark-report

Este comando:

1. ejecuta el benchmark,
2. genera `benchmarks/results.csv`,
3. calcula tiempo promedio, throughput, speedup y eficiencia,
4. imprime una tabla Markdown en consola.

### 9. Reporte versionable de benchmark

Generar un resumen Markdown versionable:

    make benchmark-summary

Este comando genera:

    benchmarks/summary.md

Ver el resumen:

    sed -n '1,160p' benchmarks/summary.md

El archivo `benchmarks/results.csv` es un artefacto generado e ignorado por Git. El archivo `benchmarks/summary.md` se versiona para documentar resultados representativos.

### 10. Flujo recomendado de validación final

Antes de considerar estable una versión, ejecutar:

    make clean
    make check
    make valgrind
    make benchmark-report

Después revisar el estado del repositorio:

    git status

El estado esperado es:

    nothing to commit, working tree clean
