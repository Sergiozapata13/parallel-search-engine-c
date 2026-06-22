
## Uso con cantidad de hilos configurable

Por defecto, el programa detecta automáticamente la cantidad de procesadores lógicos disponibles:

~~~~bash
./parallel_search config/example.conf
~~~~

También se puede especificar la cantidad de hilos manualmente:

~~~~bash
./parallel_search config/example.conf --threads 4
~~~~

Esto permite realizar benchmarks comparando 1 hilo contra múltiples hilos:

~~~~bash
./parallel_search config/example.conf --threads 1
./parallel_search config/example.conf --threads 2
./parallel_search config/example.conf --threads 4
./parallel_search config/example.conf --threads 8
./parallel_search config/example.conf --threads 12
~~~~

Si se pasa una cantidad inválida de hilos, el programa muestra un mensaje de uso o rechaza el valor:

~~~~bash
./parallel_search config/example.conf --threads 0
./parallel_search config/example.conf --threads abc
./parallel_search config/example.conf --threads 99
~~~~

## Benchmarking

El proyecto incluye una configuración de benchmark en:

    config/benchmark.conf

Esta configuración usa una cadena objetivo local al final del espacio de búsqueda para medir mejor el comportamiento con diferentes cantidades de hilos.

Ejecución rápida:

    RUNS=1 THREADS="1 2 4" scripts/run_benchmarks.sh

Ejecución recomendada:

    RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh

El resultado se genera en:

    benchmarks/results.csv

Los archivos CSV generados no se versionan por defecto porque los resultados dependen del hardware, sistema operativo, carga del sistema y entorno de ejecución.

## Modo full scan

El modo normal se detiene cuando algún hilo encuentra la cadena objetivo:

    ./parallel_search config/benchmark.conf --threads 4

Para benchmarks más reproducibles, se puede usar `--full-scan`:

    ./parallel_search config/benchmark.conf --threads 4 --full-scan

En este modo, el programa registra el objetivo encontrado, pero continúa hasta procesar todo el espacio de búsqueda. Esto hace que `Attempts` sea igual a `Search space`, lo cual permite comparar mejor el throughput entre distintas cantidades de hilos.

El script de benchmarks usa `--full-scan` por defecto:

    RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh

Para desactivar full scan en el script:

    FULL_SCAN=0 RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh
