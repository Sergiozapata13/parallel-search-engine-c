# Benchmark Summary

This document records a benchmark run for the local educational parallel search-space engine.

## Environment

- Generated at: `2026-06-21T21:51:12-06:00`
- Git commit: `ec0f7e4`
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
| 1 | 3 | 60,466,176 | 1.282159 | 47.16 | 1.00x | 100.00% |
| 2 | 3 | 60,466,176 | 0.696459 | 86.83 | 1.84x | 92.05% |
| 4 | 3 | 60,466,176 | 0.370469 | 163.29 | 3.46x | 86.52% |
| 8 | 3 | 60,466,176 | 0.206502 | 293.20 | 6.21x | 77.61% |
| 12 | 3 | 60,466,176 | 0.162475 | 372.20 | 7.89x | 65.76% |

## Interpretation

The best measured throughput in this run was obtained with 12 thread(s), reaching approximately 372.20 million attempts per second.

The full-scan benchmark mode makes all tested thread counts process the same search-space size, which makes elapsed time, throughput, speedup and parallel efficiency comparable.

## Notes

- Results depend on CPU model, system load, WSL/native Linux behavior and thermal conditions.
- Final measurements should be repeated on native Ubuntu Linux when possible.
- `benchmarks/results.csv` is generated and ignored by Git; this summary is versioned intentionally.
