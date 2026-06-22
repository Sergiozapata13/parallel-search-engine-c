#!/usr/bin/env python3

import csv
import sys
from collections import defaultdict
from pathlib import Path
from statistics import mean


REQUIRED_COLUMNS = {
    "threads",
    "attempts",
    "elapsed_seconds",
    "throughput_attempts_per_sec",
}


def fail(message):
    print(f"Error: {message}", file=sys.stderr)
    return 1


def parse_csv(path):
    rows = []

    with path.open(newline="") as file:
        reader = csv.DictReader(file)

        if reader.fieldnames is None:
            raise ValueError("CSV vacío o sin encabezado")

        missing = REQUIRED_COLUMNS - set(reader.fieldnames)
        if missing:
            missing_list = ", ".join(sorted(missing))
            raise ValueError(f"faltan columnas requeridas: {missing_list}")

        for line_number, row in enumerate(reader, start=2):
            try:
                rows.append({
                    "threads": int(row["threads"]),
                    "attempts": int(row["attempts"]),
                    "elapsed_seconds": float(row["elapsed_seconds"]),
                    "throughput_attempts_per_sec": float(row["throughput_attempts_per_sec"]),
                })
            except ValueError as exc:
                raise ValueError(f"fila {line_number} contiene valores inválidos: {exc}") from exc

    if not rows:
        raise ValueError("CSV no contiene filas de datos")

    return rows


def group_by_threads(rows):
    grouped = defaultdict(list)

    for row in rows:
        if row["threads"] <= 0:
            raise ValueError("threads debe ser mayor que cero")
        if row["elapsed_seconds"] <= 0.0:
            raise ValueError("elapsed_seconds debe ser mayor que cero")
        if row["attempts"] <= 0:
            raise ValueError("attempts debe ser mayor que cero")

        grouped[row["threads"]].append(row)

    return grouped


def format_int(value):
    return f"{value:,.0f}"


def format_float(value, decimals=3):
    return f"{value:.{decimals}f}"


def print_report(path, grouped):
    baseline_threads = 1 if 1 in grouped else min(grouped)
    baseline_time = mean(row["elapsed_seconds"] for row in grouped[baseline_threads])

    print("# Benchmark analysis")
    print()
    print(f"CSV: `{path}`")
    print(f"Baseline threads: `{baseline_threads}`")
    print()

    print("| Threads | Runs | Avg attempts | Avg time (s) | Avg throughput (M attempts/s) | Speedup | Efficiency |")
    print("|---:|---:|---:|---:|---:|---:|---:|")

    for threads in sorted(grouped):
        rows = grouped[threads]

        avg_attempts = mean(row["attempts"] for row in rows)
        avg_time = mean(row["elapsed_seconds"] for row in rows)
        avg_throughput = mean(row["throughput_attempts_per_sec"] for row in rows)

        speedup = baseline_time / avg_time
        relative_threads = threads / baseline_threads
        efficiency = (speedup / relative_threads) * 100.0

        print(
            f"| {threads} "
            f"| {len(rows)} "
            f"| {format_int(avg_attempts)} "
            f"| {format_float(avg_time, 6)} "
            f"| {format_float(avg_throughput / 1_000_000.0, 2)} "
            f"| {format_float(speedup, 2)}x "
            f"| {format_float(efficiency, 2)}% |"
        )


def main():
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("benchmarks/results.csv")

    if not csv_path.exists():
        return fail(f"no existe el archivo {csv_path}")

    try:
        rows = parse_csv(csv_path)
        grouped = group_by_threads(rows)
        print_report(csv_path, grouped)
    except ValueError as exc:
        return fail(str(exc))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
