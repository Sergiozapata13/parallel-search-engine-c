#!/usr/bin/env python3

import csv
import subprocess
import sys
from collections import defaultdict
from datetime import datetime
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


def command_output(command):
    try:
        completed = subprocess.run(
            command,
            check=False,
            capture_output=True,
            text=True,
            timeout=5,
        )
    except (OSError, subprocess.SubprocessError):
        return "not available"

    output = completed.stdout.strip()
    if not output:
        return "not available"

    return output


def first_line(text):
    return text.splitlines()[0] if text else "not available"


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


def fmt_int(value):
    return f"{value:,.0f}"


def fmt_float(value, decimals):
    return f"{value:.{decimals}f}"


def build_results(grouped):
    baseline_threads = 1 if 1 in grouped else min(grouped)
    baseline_time = mean(row["elapsed_seconds"] for row in grouped[baseline_threads])

    results = []

    for threads in sorted(grouped):
        rows = grouped[threads]

        avg_attempts = mean(row["attempts"] for row in rows)
        avg_time = mean(row["elapsed_seconds"] for row in rows)
        avg_throughput = mean(row["throughput_attempts_per_sec"] for row in rows)

        speedup = baseline_time / avg_time
        relative_threads = threads / baseline_threads
        efficiency = (speedup / relative_threads) * 100.0

        results.append({
            "threads": threads,
            "runs": len(rows),
            "avg_attempts": avg_attempts,
            "avg_time": avg_time,
            "avg_throughput": avg_throughput,
            "speedup": speedup,
            "efficiency": efficiency,
        })

    return baseline_threads, results


def read_file_if_exists(path):
    p = Path(path)
    if not p.exists():
        return "not available"

    return p.read_text().strip()


def write_summary(csv_path, output_path, baseline_threads, results):
    timestamp = datetime.now().astimezone().isoformat(timespec="seconds")
    uname = command_output(["uname", "-a"])
    nproc = command_output(["nproc"])
    gcc_version = first_line(command_output(["gcc", "--version"]))
    git_commit = command_output(["git", "rev-parse", "--short", "HEAD"])
    benchmark_config = read_file_if_exists("config/benchmark.conf")

    best = max(results, key=lambda item: item["avg_throughput"])

    lines = []
    lines.append("# Benchmark Summary")
    lines.append("")
    lines.append("This document records a benchmark run for the local educational parallel search-space engine.")
    lines.append("")
    lines.append("## Environment")
    lines.append("")
    lines.append(f"- Generated at: `{timestamp}`")
    lines.append(f"- Git commit: `{git_commit}`")
    lines.append(f"- System: `{uname}`")
    lines.append(f"- Logical processors: `{nproc}`")
    lines.append(f"- Compiler: `{gcc_version}`")
    lines.append("")
    lines.append("## Benchmark configuration")
    lines.append("")
    lines.append("Configuration file:")
    lines.append("")
    lines.append("```text")
    lines.append(benchmark_config)
    lines.append("```")
    lines.append("")
    lines.append(f"Input CSV: `{csv_path}`")
    lines.append(f"Baseline threads: `{baseline_threads}`")
    lines.append("")
    lines.append("## Results")
    lines.append("")
    lines.append("| Threads | Runs | Avg attempts | Avg time (s) | Avg throughput (M attempts/s) | Speedup | Efficiency |")
    lines.append("|---:|---:|---:|---:|---:|---:|---:|")

    for item in results:
        lines.append(
            f"| {item['threads']} "
            f"| {item['runs']} "
            f"| {fmt_int(item['avg_attempts'])} "
            f"| {fmt_float(item['avg_time'], 6)} "
            f"| {fmt_float(item['avg_throughput'] / 1_000_000.0, 2)} "
            f"| {fmt_float(item['speedup'], 2)}x "
            f"| {fmt_float(item['efficiency'], 2)}% |"
        )

    lines.append("")
    lines.append("## Interpretation")
    lines.append("")
    lines.append(
        f"The best measured throughput in this run was obtained with "
        f"{best['threads']} thread(s), reaching approximately "
        f"{fmt_float(best['avg_throughput'] / 1_000_000.0, 2)} million attempts per second."
    )
    lines.append("")
    lines.append(
        "The full-scan benchmark mode makes all tested thread counts process the same search-space size, "
        "which makes elapsed time, throughput, speedup and parallel efficiency comparable."
    )
    lines.append("")
    lines.append("## Notes")
    lines.append("")
    lines.append("- Results depend on CPU model, system load, WSL/native Linux behavior and thermal conditions.")
    lines.append("- Final measurements should be repeated on native Ubuntu Linux when possible.")
    lines.append("- `benchmarks/results.csv` is generated and ignored by Git; this summary is versioned intentionally.")
    lines.append("")

    output_path.write_text("\n".join(lines))


def main():
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("benchmarks/results.csv")
    output_path = Path(sys.argv[2]) if len(sys.argv) > 2 else Path("benchmarks/summary.md")

    if not csv_path.exists():
        return fail(f"no existe el archivo {csv_path}")

    try:
        rows = parse_csv(csv_path)
        grouped = group_by_threads(rows)
        baseline_threads, results = build_results(grouped)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        write_summary(csv_path, output_path, baseline_threads, results)
    except ValueError as exc:
        return fail(str(exc))

    print(f"Benchmark summary written to {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
