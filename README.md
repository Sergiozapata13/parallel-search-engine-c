
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
