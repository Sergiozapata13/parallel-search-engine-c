# Benchmark Summary

This document records a benchmark run for the local educational parallel search-space engine.

## Environment

- Generated at: `2026-06-21T21:52:12-06:00`
- Git commit: `b680e27`
- System: `Linux SzapataV 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux`
- Logical processors: `12`
- Compiler: `gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0`

## Benchmark configuration

Configuration file:

```text
# Benchmark configuration.
# Local educational search-space exploration only.

mode=2
length=5
target=zzzzz
verbose=0
print_ranges=0
chunk_size=1000000
```

Input CSV: `benchmarks/results.csv`
Baseline threads: `1`

## Results

| Threads | Runs | Avg attempts | Avg time (s) | Avg throughput (M attempts/s) | Speedup | Efficiency |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 3 | 60,466,176 | 1.277835 | 47.32 | 1.00x | 100.00% |
| 2 | 3 | 60,466,176 | 0.696770 | 86.80 | 1.83x | 91.70% |
| 4 | 3 | 60,466,176 | 0.381466 | 158.73 | 3.35x | 83.75% |
| 8 | 3 | 60,466,176 | 0.203933 | 296.75 | 6.27x | 78.32% |
| 12 | 3 | 60,466,176 | 0.160210 | 377.76 | 7.98x | 66.47% |

## Interpretation

The best measured throughput in this run was obtained with 12 thread(s), reaching approximately 377.76 million attempts per second.

The full-scan benchmark mode makes all tested thread counts process the same search-space size, which makes elapsed time, throughput, speedup and parallel efficiency comparable.

## Notes

- Results depend on CPU model, system load, WSL/native Linux behavior and thermal conditions.
- Final measurements should be repeated on native Ubuntu Linux when possible.
- `benchmarks/results.csv` is generated and ignored by Git; this summary is versioned intentionally.
