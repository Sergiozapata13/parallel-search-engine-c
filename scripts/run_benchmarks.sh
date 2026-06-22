#!/usr/bin/env bash

set -euo pipefail

CONFIG="${1:-config/benchmark.conf}"
THREADS="${THREADS:-1 2 4 8 12}"
RUNS="${RUNS:-3}"
FULL_SCAN="${FULL_SCAN:-1}"
OUT="${OUT:-benchmarks/results.csv}"

mkdir -p "$(dirname "$OUT")"

make clean >/dev/null
make >/dev/null

echo "timestamp,config,threads,run,full_scan,found_index,attempts,elapsed_seconds,throughput_attempts_per_sec" > "$OUT"

for threads in $THREADS; do
    for run in $(seq 1 "$RUNS"); do
        args=("$CONFIG" "--threads" "$threads")

        if [ "$FULL_SCAN" = "1" ]; then
            args+=("--full-scan")
        fi

        output="$(./parallel_search "${args[@]}")"

        found_index="$(printf "%s\n" "$output" | awk -F: '/Found index/ {gsub(/^[ \t]+/, "", $2); print $2}')"
        attempts="$(printf "%s\n" "$output" | awk -F: '/Attempts/ {gsub(/^[ \t]+/, "", $2); print $2}')"
        elapsed="$(printf "%s\n" "$output" | awk -F: '/Elapsed time/ {gsub(/^[ \t]+/, "", $2); gsub(/ s$/, "", $2); print $2}')"
        throughput="$(printf "%s\n" "$output" | awk -F: '/Throughput/ {gsub(/^[ \t]+/, "", $2); gsub(/ attempts\/s$/, "", $2); print $2}')"
        timestamp="$(date -Is)"

        echo "$timestamp,$CONFIG,$threads,$run,$FULL_SCAN,$found_index,$attempts,$elapsed,$throughput" >> "$OUT"

        echo "threads=$threads run=$run full_scan=$FULL_SCAN elapsed=${elapsed}s throughput=${throughput} attempts/s"
    done
done

echo "Benchmark results written to $OUT"
