# Validation

This document describes how the project is validated.

## Compiler flags

The project uses strict compiler flags:

    -Wall -Wextra -Werror -pedantic -std=c11 -pthread

The Makefile also defines:

    -D_POSIX_C_SOURCE=200809L

## Normal build

    make clean
    make

Expected result:

- Compilation succeeds.
- No warnings.
- Binary `parallel_search` is generated.

## Functional tests

    make test

This runs:

    ./parallel_search config/example.conf --threads 1
    ./parallel_search config/example.conf --threads 2
    ./parallel_search config/example.conf --threads 4
    ./parallel_search config/benchmark.conf --threads 4 --full-scan

Expected result:

- Target is found.
- Found candidate matches the configured target.
- Full scan mode processes the complete search space.

## Sanitizers

    make clean
    make sanitize
    ./parallel_search config/example.conf --threads 4 --full-scan

Expected result:

- No AddressSanitizer errors.
- No UndefinedBehaviorSanitizer errors.

## Valgrind

    make valgrind

Expected result:

    All heap blocks were freed -- no leaks are possible
    ERROR SUMMARY: 0 errors from 0 contexts

## Git workflow

Each milestone should be committed separately with descriptive messages.

Examples:

    Initialize project structure and configuration parser
    Add sequential search baseline
    Add pthread dynamic scheduler
    Add configurable thread count option
    Add benchmark configuration and runner
    Add full scan benchmark mode
    Add configurable range printing
    Add Makefile validation targets
