# Benchmarking

## Goal

The benchmark measures how the search engine scales with different thread counts.

Metrics:

- elapsed time
- attempts
- throughput
- speedup

## Benchmark configuration

The benchmark config is:

    config/benchmark.conf

Current configuration:

    mode=2
    length=5
    target=zzzzz
    verbose=0
    print_ranges=0
    chunk_size=1000000

This produces:

    charset size = 36
    search space = 36^5 = 60,466,176

Since `zzzzz` is the last candidate in this charset ordering, it is useful for controlled measurement.

## Full scan

Benchmarks use full scan by default:

    --full-scan

This ensures that all runs process the full search space.

Expected attempts:

    60,466,176

for every tested thread count.

## Running benchmarks

Quick benchmark:

    RUNS=1 THREADS="1 2 4" scripts/run_benchmarks.sh

Recommended benchmark:

    RUNS=3 THREADS="1 2 4 8 12" scripts/run_benchmarks.sh

Generated output:

    benchmarks/results.csv

## CSV columns

The generated CSV contains:

| Column | Meaning |
|---|---|
| timestamp | Run timestamp |
| config | Config file used |
| threads | Number of worker threads |
| run | Run number |
| full_scan | Whether full scan was enabled |
| found_index | Index where target was found |
| attempts | Number of candidates generated |
| elapsed_seconds | Runtime in seconds |
| throughput_attempts_per_sec | Attempts per second |

## Speedup

Speedup is calculated as:

    speedup_N = time_1_thread / time_N_threads

Example:

    time_1_thread = 0.639366 s
    time_4_threads = 0.189481 s

    speedup_4 = 0.639366 / 0.189481 = 3.37x

## Notes

Benchmark results depend on:

- CPU model
- number of logical processors
- WSL vs native Linux
- compiler optimization flags
- system load
- thermal throttling
- chunk size
- console output

For final measurements, use native Ubuntu Linux when possible.

## Automatic benchmark analysis

The project includes a Python script to analyze the generated CSV file:

    python3 scripts/analyze_benchmarks.py benchmarks/results.csv

It computes:

- average elapsed time
- average throughput
- speedup
- parallel efficiency

The Makefile exposes two targets:

    make analyze-benchmark
    make benchmark-report

`make benchmark-report` runs a benchmark and immediately prints the analysis table.
