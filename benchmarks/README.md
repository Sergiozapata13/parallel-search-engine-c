# Benchmarks

This directory stores benchmark notes and generated performance data.

Generated CSV files are ignored by Git by default because benchmark results depend on:

- CPU model
- number of logical processors
- OS environment
- WSL vs native Linux
- compiler flags
- background system load

To generate benchmark data:

~~~~bash
RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh
~~~~

Quick test:

~~~~bash
RUNS=1 THREADS="1 2 4" scripts/run_benchmarks.sh
~~~~
