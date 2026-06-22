# Architecture

## Overview

The program implements a local exhaustive search engine using POSIX pthreads.

The search space is represented as a numeric interval:

    [0, total_space)

Each numeric index maps to one candidate string using base conversion, where the base is the charset size.

Example:

    charset = 0123456789
    length = 3

    index 0   -> 000
    index 1   -> 001
    index 42  -> 042
    index 999 -> 999

For alphanumeric mode:

    charset size = 36
    length = 5
    total_space = 36^5 = 60,466,176

## Modules

### config

Files:

    include/config.h
    src/config.c

Responsibilities:

- Read key-value configuration files.
- Validate mode, length, target, verbose, print_ranges and chunk_size.
- Store configuration in the Config structure.

### charset

Files:

    include/charset.h
    src/charset.c

Responsibilities:

- Build the charset according to the selected mode.
- Validate that the target string only contains allowed characters.

### search

Files:

    include/search.h
    src/search.c

Responsibilities:

- Calculate total search-space size.
- Detect overflow when computing base^length.
- Convert numeric indexes to candidate strings.
- Provide the sequential baseline search.

### scheduler

Files:

    include/scheduler.h
    src/scheduler.c

Responsibilities:

- Detect logical processors.
- Create worker threads.
- Assign dynamic chunks of work.
- Coordinate shared state using mutexes and atomics.
- Stop early when the target is found, unless full_scan is enabled.

### timer

Files:

    include/timer.h
    src/timer.c

Responsibilities:

- Measure elapsed time using CLOCK_MONOTONIC.

## Dynamic scheduling

The scheduler maintains a shared `next_start` index.

Each worker thread repeatedly:

1. Locks the work mutex.
2. Takes the next available chunk.
3. Updates `next_start`.
4. Unlocks the mutex.
5. Processes its assigned range.

This design avoids static imbalance and allows threads to keep taking work until the search finishes.

## Early stop mode

Default behavior:

    ./parallel_search config/example.conf --threads 4

When a thread finds the target:

- The found flag is set.
- The winning thread is recorded.
- The stop flag is set.
- Other threads finish shortly after observing the stop flag.

This mode simulates a real search where work stops once the target is found.

## Full scan mode

Benchmark behavior:

    ./parallel_search config/benchmark.conf --threads 4 --full-scan

When a thread finds the target:

- The found flag is set.
- The winning thread is recorded.
- The search continues until all chunks are processed.

This mode is useful for reproducible benchmarking because attempts should equal the full search-space size.

## Synchronization

The project uses:

- `pthread_mutex_t work_mutex` for chunk assignment and shared counters.
- `pthread_mutex_t print_mutex` for clean console output.
- `atomic_int stop`, `found` and `error` for cross-thread state checks.

## Design priorities

- Correctness before optimization.
- No segmentation faults.
- No silent integer overflow in search-space calculation.
- No data races on shared state.
- Clear modular C code.
- Portable Linux/POSIX behavior.
